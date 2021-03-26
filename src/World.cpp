#include "World.h"
#include "GLEW/glew.h"
#include <string>

World::Chunk::Layer::Layer() {
	for (size_t i = 0; i < CHUNK_WIDTH; i++) {
		for (size_t j = 0; j < CHUNK_WIDTH; j++) {
			blocks[i][j] = BlockType::Air;
		}
	}
}

World::Chunk::Chunk(const Vec2& position, World& world) : world(world) {
	mMat.Position().x = position.x;
	mMat.Position().z = position.z;
}

World::Chunk::~Chunk() {
	
}

const bool World::Chunk::IsBlockOutOfBounds(const Vec3& blockPos) const {
	if (blockPos.x < 0 ||
		blockPos.y < 0 ||
		blockPos.z < 0 ||
		blockPos.x >= CHUNK_WIDTH ||
		blockPos.y >= CHUNK_HEIGHT ||
		blockPos.z >= CHUNK_WIDTH) {
		return true;
	}

	return false;
}

const BlockType& World::Chunk::GetBlock(const Vec3& blockPos) const {
	if (IsBlockOutOfBounds(blockPos)) {
		return BlockType::Air;
	}

	return layers[(int)blockPos.y].GetBlock(blockPos);
}

void World::Chunk::SetBlock(const BlockType& id, const Vec3& blockPos) {
	if (!IsBlockOutOfBounds(blockPos)) {
		(layers[(int)blockPos.y].SetBlock(id, blockPos));
	}
}

void World::Chunk::GenerateModel() {
	for (size_t i = 1; i < (int)BlockType::NUM_TYPES; i++) {
		verticesLists[i].clear();
		indicesLists[i].clear();
	}

	// preallocate
	Vec3 blockPosition;
	Chunk* otherChunk;
	int blockId;
	unsigned int offsets[(int)BlockType::NUM_TYPES];

	// set bounds of how far out to render based on what chunk the player is in
	Vec2 camChunkCoords = GetChunkCoords(world.camera.position);
	Vec2 lowChunkXZ(camChunkCoords.x - world.renderDistance, camChunkCoords.z - world.renderDistance);
	Vec2 highChunkXZ(camChunkCoords.x + world.renderDistance, camChunkCoords.z + world.renderDistance);

	Vec2 chunkPos(mMat.Position().x, mMat.Position().z);

	for (float y = 0; y < CHUNK_HEIGHT; y++) {
		for (float x = 0; x < CHUNK_WIDTH; x++) {
			for (float z = 0; z < CHUNK_WIDTH; z++) {

				// infer the block position using its coordinates
				blockPosition = { x, y, z };

				blockId = (int)GetBlock(blockPosition);

				// don't render air
				if (blockId == (int)BlockType::Air) {
					continue;
				}

				auto getBlock = [&](float x, float y, float z) {
					// this stops layer 0 from always being rendered
					if (y == -1) {
						return BlockType::Grass;
					}

					otherChunk = world.GetChunk(chunkPos);

					if ((x < 0 && chunkPos.x == lowChunkXZ.x) ||
						(z < 0 && chunkPos.z == lowChunkXZ.z) ||
						(x >= CHUNK_WIDTH && chunkPos.x == highChunkXZ.x) ||
						(z >= CHUNK_WIDTH && chunkPos.z == highChunkXZ.z)) {
						return BlockType::Grass;
					}

					if (x < 0 && z < 0) {
						x += CHUNK_WIDTH;
						z += CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x - 1, chunkPos.z - 1));
					}
					else if (x >= CHUNK_WIDTH && z >= CHUNK_WIDTH) {
						x -= CHUNK_WIDTH;
						z -= CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x + 1, chunkPos.z + 1));
					}
					else if (x < 0 && z >= CHUNK_WIDTH) {
						x += CHUNK_WIDTH;
						z -= CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x - 1, chunkPos.z + 1));
					}
					else if (x >= CHUNK_WIDTH && z < 0) {
						x -= CHUNK_WIDTH;
						z += CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x + 1, chunkPos.z - 1));
					}
					else if (x < 0) {
						x += CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x - 1, chunkPos.z));
					}
					else if (x >= CHUNK_WIDTH) {
						x -= CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x + 1, chunkPos.z));
					}
					else if (z < 0) {
						z += CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x, chunkPos.z - 1));
					}
					else if (z >= CHUNK_WIDTH) {
						z -= CHUNK_WIDTH;
						otherChunk = world.GetChunk(Vec2(chunkPos.x, chunkPos.z + 1));
					}


					return otherChunk->GetBlock(Vec3(x, y, z));
				};


				/////////////////////////////////////////////////////
				// decide if we actually need to render each block //
				/////////////////////////////////////////////////////
				if (getBlock(x, y + 1, z) == BlockType::Air ||
					getBlock(x, y - 1, z) == BlockType::Air ||
					getBlock(x + 1, y, z) == BlockType::Air ||
					getBlock(x - 1, y, z) == BlockType::Air ||
					getBlock(x, y, z + 1) == BlockType::Air ||
					getBlock(x, y, z - 1) == BlockType::Air) {
					auto& verts = world.blockDatabase[blockId].vertices;
					auto& inds = world.blockDatabase[blockId].indices;

					// save the offset for the indices
					offsets[blockId] = verticesLists[blockId].size();

					// account for the block position and chunk position and store the new verts for later
					for (int i = 0; i < verts.size(); i++) {
						Vertex v(verts[i]);
						v.position.x += blockPosition.x;
						v.position.y += blockPosition.y;
						v.position.z += blockPosition.z;
						v.position.x += mMat.Position().x * CHUNK_WIDTH; // place vertices in world space coordinates
						v.position.z += mMat.Position().z * CHUNK_WIDTH;
						verticesLists[blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < inds.size(); i++) {
						unsigned int ind(inds[i] + offsets[blockId]);
						indicesLists[blockId].push_back(ind);
					}
				}
			}
		}
	}

	isLoaded = true;
}

void World::Chunk::Draw() {
	// TODO: eventually fix this to actually do the proper calculation here
	Mat4 mvMat = world.camera.vMat * IdentityMatrix();

	// draw each type of block separately
	for (size_t i = 1; i < (int)BlockType::NUM_TYPES; i++) {
		VertexBufferArray va;

		VertexBuffer vb(verticesLists[i].data(), verticesLists[i].size() * sizeof(Vertex));

		VertexBufferLayout layout;

		layout.Push<float>(3, false);
		layout.Push<float>(2, true);

		va.AddBuffer(vb, layout);

		IndexBuffer ib(indicesLists[i].data(), indicesLists[i].size());
		
		ShaderProgram& shader = world.shaders[(int)ShaderType::Basic];
		shader.Bind();
		world.textures[i].Bind(0);
		shader.SetUniform1i("_texture", 0);
		shader.SetUniformMatrix4fv("mvMat", mvMat);
		shader.SetUniformMatrix4fv("pMat", world.camera.pMat);

		world.renderer.Draw(va, ib, shader);
	}
}

void World::Chunk::GenerateTerrain(TerrainGenerator& terrainGenerator, long long& seed) {
	// generate the terrain image
	auto image = terrainGenerator.GetTerrain(Vec2(mMat.Position().x, mMat.Position().z), seed, CHUNK_WIDTH, CHUNK_HEIGHT);

	// sample the image at x and z coords to get y coord
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_WIDTH; z++) {
			int y = image->GetValue(x, z).red;
			int r = rand() % 4;
			if (r == 1) {
				SetBlock(BlockType::Dirt, Vec3(x, y, z));
			}
			else {
				SetBlock(BlockType::Grass, Vec3(x, y, z));
			}
			

			// set every block below the surface as well
			y--;
			while (y >= 0) {
				SetBlock(BlockType::Dirt, Vec3(x, y, z));

				y--;
			}
		}
	}

	//// generate spheres of dirt lol
	//for (int z = 0; z < AppGlobals::CHUNK_WIDTH; z++) {
	//	for (int y = 0; y < AppGlobals::CHUNK_WIDTH; y++) {
	//		for (int x = 0; x < AppGlobals::CHUNK_WIDTH; x++) {
	//			if (sqrt((float)(x - AppGlobals::CHUNK_WIDTH / 2) * (x - AppGlobals::CHUNK_WIDTH / 2) + (y - AppGlobals::CHUNK_WIDTH / 2) * (y - AppGlobals::CHUNK_WIDTH / 2) + (z - AppGlobals::CHUNK_WIDTH / 2) * (z - AppGlobals::CHUNK_WIDTH / 2)) <= AppGlobals::CHUNK_WIDTH / 2) {
	//				chunk->SetBlock(BlockType::Grass, Vec4(x, y, z, 0));
	//			}
	//		}
	//	}
	//}

	isInitialized = true;
}

void World::UpdateLoadList() {
	int numOfChunksLoaded = 0;

	// set bounds of how far out to render based on what chunk the player is in
	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	Vec2 lowChunkXZ(camChunkCoords.x - (renderDistance + 1), camChunkCoords.z - (renderDistance + 1));
	Vec2 highChunkXZ(camChunkCoords.x + (renderDistance + 1), camChunkCoords.z + (renderDistance + 1));
	float sqRenderDistance = renderDistance * renderDistance;

	// for each chunk around the player
	Vec2 chunkPos;
	for (float x = lowChunkXZ.x; x <= highChunkXZ.x; x++) {
		for (float z = lowChunkXZ.z; z <= highChunkXZ.z; z++) {
			chunkPos = { x, z };

			if (!ChunkAlreadyExistsIn(chunkLoadList, chunkPos)) {
				chunkLoadList.push_back(chunkPos);
			}
		}
	}

	// initialize each chunk in the load list
	Chunk* chunk;
	for (int i = 0; i < chunkLoadList.size(); i++) {
		chunk = GetChunk(chunkLoadList[i]);

		if (!chunk->IsInitialized()) {
			chunk->GenerateTerrain(terrainGenerator, seed);
		}

		if (!ChunkAlreadyExistsIn(visibleChunksList, chunkLoadList[i])) {
			if (!ChunkOutsideRenderDistance(chunkLoadList[i], camChunkCoords, sqRenderDistance)) {
				// add the chunk to the visible list because it is potentially visible
				visibleChunksList.push_back(chunkLoadList[i]);

				// remove the chunk from the load list since it is now loaded
				chunkLoadList.erase(chunkLoadList.begin() + i);

				// subtract 1 from the index since the container size changed
				i--;
			}
			else {
				chunkUnloadList.push_back(chunkLoadList[i]);

				// remove the chunk from the load list
				chunkLoadList.erase(chunkLoadList.begin() + i);

				// subtract 1 from the index since the container size changed
				i--;
			}
		}
		else {
			// remove it from the load list
			chunkLoadList.erase(chunkLoadList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
	}
}

void World::UpdateVisibleList() {
	// TODO: frustum culling or something
	// for each chunk in the potentially visible list
	for (int i = 0; i < visibleChunksList.size(); i++) {
		if (!ChunkAlreadyExistsIn(renderableChunksList, visibleChunksList[i])) {
			// add the chunk to the renderable chunk list because it is able to be seen by the player
			renderableChunksList.push_back(visibleChunksList[i]);

			// remove it from the visible list
			visibleChunksList.erase(visibleChunksList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
		// if the chunk is already in the renderable list
		else {
			// remove it from the visible list
			visibleChunksList.erase(visibleChunksList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
	}
}

void World::UpdateRenderableList() {
	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	float sqRenderDistance = renderDistance * renderDistance;

	Chunk* chunk;
	int numChunksLoaded = 0;
	// for each chunk in the render list
	for (int i = 0; i < renderableChunksList.size(); i++) {
		if (numChunksLoaded != numChunksPerFrame) {
			chunk = GetChunk(renderableChunksList[i]);
			if (!chunk->IsLoaded()) {
				chunk->GenerateModel();
				numChunksLoaded++;
			}
		}

		if (ChunkOutsideRenderDistance(renderableChunksList[i], camChunkCoords, sqRenderDistance)) {
			// add the chunk to the unload list because its out of render distance
			chunkUnloadList.push_back(renderableChunksList[i]);

			// remove the chunk from the renderable chunks list so it is no longer rendered
			renderableChunksList.erase(renderableChunksList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
	}
}

void World::UpdateUnloadList() {
	// for each chunk in the unload list
	for (int i = 0; i < chunkUnloadList.size(); i++) {
		// TODO: save chunk to file eventually
		chunkMap.erase(chunkUnloadList[i]);

		// remove the chunk from the unload list since it is now unloaded
		chunkUnloadList.erase(chunkUnloadList.begin() + i);

		// subtract 1 from the index since the container size changed
		i--;
	}
}

bool World::ChunkAlreadyExistsIn(const std::vector<Vec2>& v, const Vec2& elem) const {
	// for each chunk in the list
	for (int i = 0; i < v.size(); i++) {
		// break if the chunk is in the load list
		if (v[i] == elem) {
			return true;
			break;
		}
	}

	return false;
}

bool World::ChunkOutsideRenderDistance(const Vec2& chunkPos, const Vec2& camChunkCoords, float sqRenderDistance) const {
	float sqDistanceToChunk = ((camChunkCoords.x - chunkPos.x) * (camChunkCoords.x - chunkPos.x)) +
		((camChunkCoords.z - chunkPos.z) * (camChunkCoords.z - chunkPos.z));

	return sqDistanceToChunk > sqRenderDistance;
}

World::World(Camera& camera, Renderer& renderer)
	: camera(camera), renderer(renderer) {

	// init shaders
	shaders[(int)ShaderType::Basic].Load("res/shaders/basic.shader");

	// init block database
	blockDatabase[(int)BlockType::Air].isCollidable = false;
	blockDatabase[(int)BlockType::Grass].isCollidable = true;
	renderer.LoadModel("res/models/blocks/BasicBlock.obj", blockDatabase[(int)BlockType::Grass].vertices, blockDatabase[(int)BlockType::Grass].indices);
	blockDatabase[(int)BlockType::Dirt].isCollidable = true;
	renderer.LoadModel("res/models/blocks/BasicBlock.obj", blockDatabase[(int)BlockType::Dirt].vertices, blockDatabase[(int)BlockType::Dirt].indices);

	// init textures
	textures[(int)BlockType::Grass].Load("res/textures/blocks/Grass.png");
	textures[(int)BlockType::Dirt].Load("res/textures/blocks/Dirt.png");

	UpdateLoadList();
}

void World::Update() {
	UpdateLoadList();
	UpdateVisibleList();
	UpdateRenderableList();
	UpdateUnloadList();
	// if the camera has crossed into a new chunk or a vertex update is being forced
	if (camChunkCoordsOld != GetChunkCoords(camera.position) || forceVertexUpdate) {
		camChunkCoordsOld = GetChunkCoords(camera.position);

		forceVertexUpdate = false;
	}
}

void World::Draw() {
	for (size_t i = 0; i < renderableChunksList.size(); i++) {
		GetChunk(renderableChunksList[i])->Draw();
	}
}

const BlockType& World::GetBlock(const Vec4& worldCoords) {
	auto blockPosition = GetBlockCoords(worldCoords);
	auto chunkPosition = GetChunkCoords(worldCoords);

	return GetChunk(chunkPosition)->GetBlock(blockPosition);
}

void World::SetBlock(const BlockType& id, const Vec4& blockPos) {
	auto blockPosition = GetBlockCoords(blockPos);
	auto chunkPosition = GetChunkCoords(blockPos);

	GetChunk(chunkPosition)->SetBlock(id, blockPosition);
}

World::Chunk* World::GetChunk(const Vec2& chunkPos) {
	if (chunkMap.find(chunkPos) == chunkMap.end()) {
		Chunk chunk(chunkPos, *this);
		chunkMap.emplace(chunkPos, std::move(chunk));
	}

	// operator[] requires the element to have a default constructor
	return &chunkMap.at(chunkPos);
}

void World::PrintDebugInfo() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { 0, 6 };
	SetConsoleCursorPosition(hStdout, position);

	printf(
R"(World Info										
loadListSize:		%d						
visibleListSize:	%d						
renderableListSize:	%d						
unloadListSize:		%d						
chunkMapSize:		%d						

		)", chunkLoadList.size(),
		visibleChunksList.size(),
		renderableChunksList.size(),
		chunkUnloadList.size(),
		chunkMap.size());
}