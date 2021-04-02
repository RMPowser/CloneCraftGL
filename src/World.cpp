#include "World.h"
#include "GLEW/glew.h"
#include <string>
#include <thread>

#define FRUSTUM_CULLING_ENABLED

World::Chunk::Layer::Layer() {
	for (auto& row : blocks) {
		for (auto& block : row) {
			block = BlockType::Air;
		}
	}
}

World::Chunk::Chunk(const Vec2& position) : highestBlockYPerColumn() {
	for (auto& i : highestBlockYPerColumn) {
		i.fill(0);
	}

	mMat.Position().x = position.x;
	mMat.Position().z = position.z;
}

World::Chunk::~Chunk() {

}

const BlockType& World::Chunk::GetBlock(const Vec3& blockPos) const {
	return layers[(int)blockPos.y].GetBlock(blockPos);
}

void World::Chunk::SetBlock(const BlockType& id, const Vec3& blockPos) {
	(layers[(int)blockPos.y].SetBlock(id, blockPos));

	if (blockPos.y > highestBlockYPerColumn[(int)blockPos.x][(int)blockPos.z]) {
		highestBlockYPerColumn[(int)blockPos.x][(int)blockPos.z] = (int)blockPos.y;
	}
}

int World::Chunk::GetHighestBlockYPerColumn(class Vec2& column) {
	return highestBlockYPerColumn[(int)column.x][(int)column.z];
}

void World::GenerateChunkMesh(Chunk* chunk) {
	for (int i = 1; i < (int)BlockType::NUM_TYPES; i++) {
		chunk->verticesLists[i].clear();
		chunk->indicesLists[i].clear();
	}

	// preallocate
	Vec3 blockChunkPosition;
	Vec3 blockWorldPosition;
	float chunkWorldPosX = chunk->mMat.Position().x * CHUNK_WIDTH;
	float chunkWorldPosZ = chunk->mMat.Position().z * CHUNK_WIDTH;
	BlockType blockId;
	unsigned int offsets[(int)BlockType::NUM_TYPES] { 0 };

	Vec2 column;

	for (float z = 0; z < CHUNK_WIDTH; z++) {
		for (float x = 0; x < CHUNK_WIDTH; x++) {
			column = { x, z };

			for (float y = chunk->GetHighestBlockYPerColumn(column); y > -1; y--) {

				// infer the block position using its coordinates
				blockChunkPosition = { x, y, z };
				blockWorldPosition = { x + chunkWorldPosX, y, z + chunkWorldPosZ };

				blockId = GetBlock(blockWorldPosition);

				// don't render air
				if (blockId == BlockType::Air) {
					continue;
				}

				/////////////////////////////////////////
				// decide which faces we actually need //
				/////////////////////////////////////////

				// top
				if (GetBlock({ blockWorldPosition.x, blockWorldPosition.y + 1, blockWorldPosition.z }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::top);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::top);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}

				// bottom
				if (GetBlock({ blockWorldPosition.x, blockWorldPosition.y - 1, blockWorldPosition.z }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::bottom);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::bottom);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}

				// east
				if (GetBlock({ blockWorldPosition.x + 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::east);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::east);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}

				// west
				if (GetBlock({ blockWorldPosition.x - 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::west);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::west);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}

				// north
				if (GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z + 1 }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::north);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::north);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}

				// south
				if (GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z - 1 }) == BlockType::Air) {
					auto verts = GetBlockDataFor(blockId).GetFaceVertices(BlockFaces::south);
					auto inds = GetBlockDataFor(blockId).GetFaceIndices(BlockFaces::south);

					// save the offset for the indices for this block type
					offsets[(int)blockId] = chunk->verticesLists[(int)blockId].size();

					for (int i = 0; i < 4; i++) { // hard coded 4 because each block face always has 4 verts
						Vertex v(verts[i]);
						v.position.x += x;
						v.position.y += y;
						v.position.z += z;
						chunk->verticesLists[(int)blockId].push_back(v);
					}

					// account for the offset into vertices vector and store the indices for later
					for (int i = 0; i < 6; i++) { // hard coded 6 because each block face always has 2 triangles
						unsigned int ind(inds[i] + offsets[(int)blockId]);
						chunk->indicesLists[(int)blockId].push_back(ind);
					}
				}
			}
		}
	}

	chunk->isLoaded = true;
}

void World::GenerateChunkTerrain(Chunk* chunk, long long& seed) {
	// generate the terrain image
	auto image = terrainGenerator.GetTerrain(Vec2(chunk->mMat.Position().x, chunk->mMat.Position().z), seed, CHUNK_WIDTH, CHUNK_HEIGHT);

	// sample the image at x and z coords to get y coord
	for (int x = 0; x < CHUNK_WIDTH; x++) {
		for (int z = 0; z < CHUNK_WIDTH; z++) {
			int y = image->GetValue(x, z).red;
			int r = rand() % 4;
			if (r == 1) {
				chunk->SetBlock(BlockType::Dirt, Vec3(x, y, z));
			}
			else {
				chunk->SetBlock(BlockType::Grass, Vec3(x, y, z));
			}

			// set every block below the surface as well
			y--;
			while (y >= 0) {
				chunk->SetBlock(BlockType::Dirt, Vec3(x, y, z));

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

	chunk->hasTerrain = true;
}

void World::UpdateLoadList() {
	std::scoped_lock lk(chunkAccessMutex);

	int numOfChunksLoaded = 0;

	// set bounds of how far out to render based on what chunk the player is in
	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	Vec2 lowChunkXZ(camChunkCoords.x - (renderDistance + 1), camChunkCoords.z - (renderDistance + 1));
	Vec2 highChunkXZ(camChunkCoords.x + (renderDistance + 1), camChunkCoords.z + (renderDistance + 1));
	float sqRenderDistance = (renderDistance) * (renderDistance);

	// for each chunk around the player
	Chunk* chunk;
	Vec2 chunkPos;
	Vec3 chunkbboxMin;
	Vec3 chunkbboxMax;

	for (float x = lowChunkXZ.x; x <= highChunkXZ.x; x++) {
		for (float z = lowChunkXZ.z; z <= highChunkXZ.z; z++) {
			chunkPos = { x, z };

			chunkbboxMin = { chunkPos.x * CHUNK_WIDTH, 0, chunkPos.z * CHUNK_WIDTH };
			chunkbboxMax = { (chunkPos.x * CHUNK_WIDTH) + CHUNK_WIDTH, CHUNK_HEIGHT, (chunkPos.z * CHUNK_WIDTH) + CHUNK_WIDTH };

			if (!ChunkAlreadyExistsIn(renderableChunksList, chunkPos) &&
				!ChunkAlreadyExistsIn(chunkUnloadList, chunkPos) &&
				!ChunkOutsideRenderDistance(chunkPos, camChunkCoords, sqRenderDistance) &&
				camera.IsBoxInFrustum(chunkbboxMin, chunkbboxMax)) {

				chunk = GetChunk(chunkPos);
				if (!chunk->hasTerrain) {
					GenerateChunkTerrain(chunk, seed);
				}

				// add the chunk to the renderable list because it is potentially visible
				renderableChunksList.push_back(chunkPos);
			}
		}
	}
}

void World::UpdateRenderableList() {
	std::scoped_lock lk(chunkAccessMutex);

	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	float sqRenderDistance = renderDistance * renderDistance;

	Chunk* chunk;
	int numChunksLoaded = 0;

	// for each chunk in the render list
	for (int i = 0; i < renderableChunksList.size(); i++) {
		if (!ChunkOutsideRenderDistance(renderableChunksList[i], camChunkCoords, sqRenderDistance)) {
			chunk = GetChunk(renderableChunksList[i]);
			if (!chunk->isLoaded) {
				GenerateChunkMesh(chunk);
				numChunksLoaded++;
			}
		}
		else {
			// add the chunk to the unload list because its out of render distance
			chunkUnloadList.push_back(renderableChunksList[i]);

			// remove the chunk from the renderable chunks list so it is no longer rendered
			renderableChunksList.erase(renderableChunksList.begin() + i);

			// subtract 1 from the index since the container size changed
			i--;
		}
	}
}

void World::CleanupUnusedChunks() {
	std::scoped_lock lk(chunkAccessMutex);

	// find stragglers
	for (auto& pair : chunkMap) {
		if (pair.second.hasTerrain && !pair.second.isLoaded) {
			if (!ChunkAlreadyExistsIn(chunkUnloadList, pair.first)) {
				chunkUnloadList.push_back(pair.first);
			}
		}
	}

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
	std::scoped_lock lk(stopMutex);
	bool result = stop;

	return result;
}

void World::SetStop(bool value) {
	std::scoped_lock lk(stopMutex);
	stop = value;
}

World::World(Camera& camera, Renderer& renderer)
	: camera(camera), renderer(renderer) {

	// init skybox
	std::vector<std::string> paths {
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};
	skybox.Load(paths);

	// init shaders
	shaders[(int)ShaderType::Basic].Load("res/shaders/basic.shader");

	// init block database
	blockDatabase[(int)BlockType::Air].isCollidable = false;
	blockDatabase[(int)BlockType::Grass].isCollidable = true; GenerateBlockData(BlockType::Grass, blockDatabase);
	blockDatabase[(int)BlockType::Dirt].isCollidable = true; GenerateBlockData(BlockType::Dirt, blockDatabase);

	// init textures
	textures[(int)BlockType::Grass].Load("res/textures/blocks/Grass.png");
	textures[(int)BlockType::Dirt].Load("res/textures/blocks/Dirt.png");

	std::thread([&]() {
		Sleep(50);
		while (!GetStop()) {
			Update();
			Sleep(16);
		}
	}).detach();
}

World::~World() {
	SetStop(true);
	Sleep(50);
}

void World::Update() {
	UpdateLoadList();
	UpdateRenderableList();
	CleanupUnusedChunks();
	// if the camera has crossed into a new chunk or a vertex update is being forced
	if (camChunkCoordsOld != GetChunkCoords(camera.position) || forceVertexUpdate) {
		camChunkCoordsOld = GetChunkCoords(camera.position);
		forceVertexUpdate = false;

	}
}

void World::Draw() {
	//std::scoped_lock lk(chunkAccessMutex);

	Vec3 chunkbboxMin;
	Vec3 chunkbboxMax;
	for (size_t i = 0; i < renderableChunksList.size(); i++) {
		chunkbboxMin = { renderableChunksList[i].x * CHUNK_WIDTH, 0, renderableChunksList[i].z * CHUNK_WIDTH };
		chunkbboxMax = { (renderableChunksList[i].x * CHUNK_WIDTH) + CHUNK_WIDTH, CHUNK_HEIGHT, (renderableChunksList[i].z * CHUNK_WIDTH) + CHUNK_WIDTH };

		if (camera.IsBoxInFrustum(chunkbboxMin, chunkbboxMax)) {
			Chunk* chunk = GetChunk(renderableChunksList[i]);

			Mat4 chunkMMat = chunk->mMat;
			chunkMMat.Position().x *= CHUNK_WIDTH;
			chunkMMat.Position().z *= CHUNK_WIDTH;

			Mat4 mvMat = camera.vMat * chunkMMat;

			// draw each type of block separately
			for (size_t i = 1; i < (int)BlockType::NUM_TYPES; i++) {
				auto& vertices = chunk->verticesLists[i];
				auto& indices = chunk->indicesLists[i];

				VertexBufferArray va;
				VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));
				VertexBufferLayout layout;
				layout.Push<float>(3, false, 0);
				layout.Push<float>(2, true, 0);
				va.AddBuffer(vb, layout);

				IndexBuffer ib(indices.data(), indices.size());

				ShaderProgram& shader = shaders[(int)ShaderType::Basic];
				shader.Bind();

				if (textures[i].isValid) {
					textures[i].Bind(0);
					shader.SetUniform1i("_texture", 0);
				}
				else {
					ASSERT(false);
				}

				shader.SetUniformMatrix4fv("pMat", camera.pMat);
				shader.SetUniformMatrix4fv("mvMat", mvMat);

				renderer.DrawIndexed(va, ib, shader);
			}
		}
	}

	skybox.Draw(renderer, camera);
}

const BlockType World::GetBlock(const Vec3& worldCoords) {
	if (worldCoords.y > CHUNK_HEIGHT || worldCoords.y < 0) {
		return BlockType::Dirt;
	}

	auto blockPosition = GetBlockCoords(worldCoords);
	auto chunkPosition = GetChunkCoords(worldCoords);

	Chunk* chunk = GetChunk(chunkPosition);
	if (!chunk->hasTerrain) {
		GenerateChunkTerrain(chunk, seed);
	}

	return chunk->GetBlock(blockPosition);
}

void World::SetBlock(const BlockType& id, const Vec3& worldCoords) {
	if (worldCoords.y >= CHUNK_HEIGHT || worldCoords.y < 0) {
		return;
	}

	auto blockPosition = GetBlockCoords(worldCoords);
	auto chunkPosition = GetChunkCoords(worldCoords);
	GetChunk(chunkPosition)->SetBlock(id, blockPosition);
}

World::Chunk* World::GetChunk(const Vec2& chunkPos) {
	if (chunkMap.find(chunkPos) == chunkMap.end()) {
		chunkMap.emplace(chunkPos, std::move(Chunk(chunkPos)));
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
renderableListSize:	%d                              
unloadListSize:		%d                              
chunkMapSize:		%d                              
)",
renderableChunksList.size(),
chunkUnloadList.size(),
chunkMap.size());
}

void World::GenerateBlockData(const BlockType& id, BlockData* blockDatabase) {
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[0].position = { 0, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[1].position = { 1, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[2].position = { 1, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[3].position = { 0, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[0].texCoord = { 1, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[1].texCoord = { 0, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[2].texCoord = { 0, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].vertices[3].texCoord = { 1, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::north].indices[5] = 1;

	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[0].position = { 1, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[1].position = { 0, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[2].position = { 0, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[3].position = { 1, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[0].texCoord = { 1, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[1].texCoord = { 0, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[2].texCoord = { 0, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].vertices[3].texCoord = { 1, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::south].indices[5] = 1;

	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[0].position = { 1, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[1].position = { 1, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[2].position = { 1, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[3].position = { 1, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[0].texCoord = { 1, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[1].texCoord = { 0, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[2].texCoord = { 0, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].vertices[3].texCoord = { 1, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::east].indices[5] = 1;

	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[0].position = { 0, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[1].position = { 0, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[2].position = { 0, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[3].position = { 0, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[0].texCoord = { 1, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[1].texCoord = { 0, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[2].texCoord = { 0, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].vertices[3].texCoord = { 1, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::west].indices[5] = 1;

	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[0].position = { 0, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[1].position = { 1, 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[2].position = { 1, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[3].position = { 0, 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[0].texCoord = { 1, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[1].texCoord = { 0, 0.666667 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[2].texCoord = { 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].vertices[3].texCoord = { 1, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::top].indices[5] = 1;

	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[0].position = { 0, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[1].position = { 1, 0, 1 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[2].position = { 1, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[3].position = { 0, 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[0].texCoord = { 0, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[1].texCoord = { 1, 0.333333 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[2].texCoord = { 1, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].vertices[3].texCoord = { 0, 0 };
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[0] = 2;
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[1] = 1;
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[2] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[3] = 0;
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[4] = 3;
	blockDatabase[(int)id].faces[(int)BlockFaces::bottom].indices[5] = 1;
}