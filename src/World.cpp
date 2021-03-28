#include "World.h"
#include "GLEW/glew.h"
#include <string>
#include <thread>

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
		return BlockType::Dirt;
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
		blockPositionLists[i].clear();
	}

	// preallocate
	Vec3 blockChunkPosition;
	Vec3 blockWorldPosition;
	float chunkWorldPosX = mMat.Position().x * CHUNK_WIDTH;
	float chunkWorldPosZ = mMat.Position().z * CHUNK_WIDTH;
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
				blockChunkPosition = { x, y, z };
				blockWorldPosition = { x + chunkWorldPosX, y, z + chunkWorldPosZ };

				//ASSERT(blockWorldPosition != Vec3(966, 39, 1007));

				blockId = (int)GetBlock(blockChunkPosition);

				// don't render air
				if (blockId == (int)BlockType::Air) {
					continue;
				}

				/////////////////////////////////////////////////////
				// decide if we actually need to render each block //
				/////////////////////////////////////////////////////
				if (world.GetBlock({ blockWorldPosition.x, blockWorldPosition.y + 1, blockWorldPosition.z }) == BlockType::Air ||
					world.GetBlock({ blockWorldPosition.x, blockWorldPosition.y - 1, blockWorldPosition.z }) == BlockType::Air ||
					world.GetBlock({ blockWorldPosition.x + 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air ||
					world.GetBlock({ blockWorldPosition.x - 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air ||
					world.GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z + 1 }) == BlockType::Air ||
					world.GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z - 1 }) == BlockType::Air) {
					
					blockPositionLists[blockId].push_back(blockChunkPosition);
				}
			}
		}
	}

	isLoaded = true;
}

void World::Chunk::Draw() {
	Vec3 blockWorldCoords;
	float chunkWorldPosX = mMat.Position().x * CHUNK_WIDTH;
	float chunkWorldPosZ = mMat.Position().z * CHUNK_WIDTH;
	Mat4 mvMat;

	std::vector<Mat4> mvMats;

	// draw each type of block separately
	for (size_t i = 1; i < (int)BlockType::NUM_TYPES; i++) {
		mvMats.clear();

		auto& blockPositions = blockPositionLists[i];

		auto& vertices = world.blockDatabase[i].vertices;
		auto& indices = world.blockDatabase[i].indices;

		for (size_t j = 0; j < blockPositions.size(); j++) {
			blockWorldCoords = { blockPositions[j].x + chunkWorldPosX, blockPositions[j].y, blockPositions[j].z + chunkWorldPosZ };
			//ASSERT(blockWorldCoords != Vec3(966, 39, 1007));

			mvMat = world.camera.vMat * MakeTranslationMatrix({ blockWorldCoords, 1 });

			mvMats.push_back(mvMat);
		}

		VertexBufferArray va;

		VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));
		VertexBufferLayout layout;
		layout.Push<float>(3, false, 0);
		layout.Push<float>(2, true, 0);

		VertexBuffer matrices(mvMats.data(), mvMats.size() * sizeof(Mat4));
		VertexBufferLayout layoutMatrices;
		layoutMatrices.Push<float>(4, false, 0);
		layoutMatrices.Push<float>(4, false, 0);
		layoutMatrices.Push<float>(4, false, 0);
		layoutMatrices.Push<float>(4, false, 1);

		va.AddBuffer(vb, layout);
		va.AddBuffer(matrices, layoutMatrices);

		IndexBuffer ib(indices.data(), indices.size());

		ShaderProgram& shader = world.shaders[(int)ShaderType::Basic];
		shader.Bind();

		if (world.textures[i].isValid) {
			world.textures[i].Bind(0);
			shader.SetUniform1i("_texture", 0);
		}
		else {
			ASSERT(false);
		}

		shader.SetUniformMatrix4fv("pMat", world.camera.pMat);

		world.renderer.DrawIndexedInstanced(va, ib, shader, mvMats.size());
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
	float sqRenderDistance = (renderDistance) * (renderDistance);

	// for each chunk around the player
	Chunk* chunk;
	Vec2 chunkPos;
	for (float x = lowChunkXZ.x; x <= highChunkXZ.x; x++) {
		for (float z = lowChunkXZ.z; z <= highChunkXZ.z; z++) {
			chunkPos = { x, z };

			if (ChunkAlreadyExistsIn(renderableChunksList, chunkPos) ||
				ChunkAlreadyExistsIn(visibleChunksList, chunkPos) ||
				ChunkAlreadyExistsIn(chunkUnloadList, chunkPos)) {

				continue;
			}
			else if (!ChunkOutsideRenderDistance(chunkPos, camChunkCoords, sqRenderDistance)) {
				chunkAccessMutex.lock();

				chunk = GetChunk(chunkPos);
				if (!chunk->IsInitialized()) {
					chunk->GenerateTerrain(terrainGenerator, seed);
				}

				chunkAccessMutex.unlock();

				// add the chunk to the visible list because it is potentially visible
				visibleChunksList.push_back(chunkPos);
			}
		}
	}
}

void World::UpdateVisibleList() {
	// TODO: frustum culling or something
	// for each chunk in the potentially visible list
	for (int i = 0; i < visibleChunksList.size(); i++) {
		// add the chunk to the renderable chunk list because it is able to be seen by the player
		renderableChunksList.push_back(visibleChunksList[i]);

		// remove it from the visible list
		visibleChunksList.erase(visibleChunksList.begin() + i);

		// subtract 1 from the index since the container size changed
		i--;
	}
}

void World::UpdateRenderableList() {
	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	float sqRenderDistance = renderDistance * renderDistance;

	Chunk* chunk;
	int numChunksLoaded = 0;
	// for each chunk in the render list
	for (int i = 0; i < renderableChunksList.size(); i++) {
		if (ChunkOutsideRenderDistance(renderableChunksList[i], camChunkCoords, sqRenderDistance)) {
			// add the chunk to the unload list because its out of render distance
			chunkUnloadList.push_back(renderableChunksList[i]);

			// remove the chunk from the renderable chunks list so it is no longer rendered
			renderableChunksList.erase(renderableChunksList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
		else {
			chunkAccessMutex.lock();
			chunk = GetChunk(renderableChunksList[i]);
			if (!chunk->IsLoaded()) {
				chunk->GenerateModel();
				numChunksLoaded++;
			}
			chunkAccessMutex.unlock();
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

bool World::GetStop() {
	stopMutex.lock();
	bool result = stop;
	stopMutex.unlock();

	return result;
}

void World::SetStop(bool value) {
	stopMutex.lock();
	stop = value;
	stopMutex.unlock();
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

	std::thread([&]() {
		Sleep(50);
		while (!GetStop()) {
			Update();
		}
	}).detach();
}

World::~World() {
	SetStop(true);
	Sleep(50);
}

void World::Update() {
	UpdateLoadList();
	UpdateVisibleList();
	UpdateUnloadList();
	UpdateRenderableList();
	
	// if the camera has crossed into a new chunk or a vertex update is being forced
	if (camChunkCoordsOld != GetChunkCoords(camera.position) || forceVertexUpdate) {
		camChunkCoordsOld = GetChunkCoords(camera.position);
		forceVertexUpdate = false;
	}
}

void World::Draw() {
	chunkAccessMutex.lock();
	for (size_t i = 0; i < renderableChunksList.size(); i++) {
		GetChunk(renderableChunksList[i])->Draw();
	}
	chunkAccessMutex.unlock();
}

const BlockType& World::GetBlock(const Vec3& worldCoords) {
	auto blockPosition = GetBlockCoords(worldCoords);
	auto chunkPosition = GetChunkCoords(worldCoords);

	Chunk* chunk = GetChunk(chunkPosition);
	if (!chunk->IsInitialized()) { chunk->GenerateTerrain(terrainGenerator, seed); }

	return chunk->GetBlock(blockPosition);
}

void World::SetBlock(const BlockType& id, const Vec3& worldCoords) {
	auto blockPosition = GetBlockCoords(worldCoords);
	auto chunkPosition = GetChunkCoords(worldCoords);

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