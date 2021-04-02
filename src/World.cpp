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

World::Chunk::Chunk(const Vec2& position) : highestBlockYPerColumn(){
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
		chunk->blockPositionLists[i].clear();
	}

	// preallocate
	Vec3 blockChunkPosition;
	Vec3 blockWorldPosition;
	float chunkWorldPosX = chunk->mMat.Position().x * CHUNK_WIDTH;
	float chunkWorldPosZ = chunk->mMat.Position().z * CHUNK_WIDTH;
	int blockId;
	unsigned int offsets[(int)BlockType::NUM_TYPES] {0};

	// set bounds of how far out to render based on what chunk the player is in
	Vec2 camChunkCoords = GetChunkCoords(camera.position);
	Vec2 lowChunkXZ(camChunkCoords.x - renderDistance, camChunkCoords.z - renderDistance);
	Vec2 highChunkXZ(camChunkCoords.x + renderDistance, camChunkCoords.z + renderDistance);

	Vec2 column;

	for (float z = 0; z < CHUNK_WIDTH; z++) {
		for (float x = 0; x < CHUNK_WIDTH; x++) {
			column = { x, z };

			for (float y = chunk->GetHighestBlockYPerColumn(column); y > -1; y--) {

				// infer the block position using its coordinates
				blockChunkPosition = { x, y, z };
				blockWorldPosition = { x + chunkWorldPosX, y, z + chunkWorldPosZ };

				blockId = (int)GetBlock(blockWorldPosition);

				// don't render air
				if (blockId == (int)BlockType::Air) {
					continue;
				}

				/////////////////////////////////////////////////////
				// decide if we actually need to render each block //
				/////////////////////////////////////////////////////
				if (GetBlock({ blockWorldPosition.x, blockWorldPosition.y + 1, blockWorldPosition.z }) == BlockType::Air ||
					GetBlock({ blockWorldPosition.x, blockWorldPosition.y - 1, blockWorldPosition.z }) == BlockType::Air ||
					GetBlock({ blockWorldPosition.x + 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air ||
					GetBlock({ blockWorldPosition.x - 1, blockWorldPosition.y, blockWorldPosition.z }) == BlockType::Air ||
					GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z + 1 }) == BlockType::Air ||
					GetBlock({ blockWorldPosition.x, blockWorldPosition.y, blockWorldPosition.z - 1 }) == BlockType::Air) {
					
					chunk->blockPositionLists[blockId].push_back(blockChunkPosition);
				}
				else {
					y = 0; // skip to the next column
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

	Vec3 chunkbboxMin;
	Vec3 chunkbboxMax;

	// for each chunk in the render list
	for (int i = 0; i < renderableChunksList.size(); i++) {
		Vec2& chunkPos = renderableChunksList[i];
		chunkbboxMin = { chunkPos.x * CHUNK_WIDTH, 0, chunkPos.z * CHUNK_WIDTH };
		chunkbboxMax = { (chunkPos.x * CHUNK_WIDTH) + CHUNK_WIDTH, CHUNK_HEIGHT, (chunkPos.z * CHUNK_WIDTH) + CHUNK_WIDTH };

		if (!ChunkOutsideRenderDistance(renderableChunksList[i], camChunkCoords, sqRenderDistance) &&
			camera.IsBoxInFrustum(chunkbboxMin, chunkbboxMax)) {

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

void World::UpdateUnloadList() {
	std::scoped_lock lk(chunkAccessMutex);

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
	UpdateUnloadList();
	// if the camera has crossed into a new chunk or a vertex update is being forced
	if (camChunkCoordsOld != GetChunkCoords(camera.position) || forceVertexUpdate) {
		camChunkCoordsOld = GetChunkCoords(camera.position);
		forceVertexUpdate = false;
		
	}
}

void World::Draw() {
	std::scoped_lock lk(chunkAccessMutex);

	for (size_t i = 0; i < renderableChunksList.size(); i++) {
		Chunk* chunk = GetChunk(renderableChunksList[i]);

		Vec3 blockWorldCoords;
		float chunkWorldPosX = chunk->mMat.Position().x * CHUNK_WIDTH;
		float chunkWorldPosZ = chunk->mMat.Position().z * CHUNK_WIDTH;
		Mat4 mvMat;

		std::vector<Mat4> mvMats;

		// draw each type of block separately
		for (size_t i = 1; i < (int)BlockType::NUM_TYPES; i++) {
			mvMats.clear();

			auto& blockPositions = chunk->blockPositionLists[i];

			auto& vertices = blockDatabase[i].vertices;
			auto& indices = blockDatabase[i].indices;

			for (size_t j = 0; j < blockPositions.size(); j++) {
				blockWorldCoords = { blockPositions[j].x + chunkWorldPosX, blockPositions[j].y, blockPositions[j].z + chunkWorldPosZ };

				mvMat = camera.vMat * MakeTranslationMatrix({ blockWorldCoords, 1 });

				mvMats.push_back(mvMat);
			}

			VertexBufferArray va;

			VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));
			VertexBufferLayout layout;
			layout.Push<float>(3, false, 0);
			layout.Push<float>(2, true, 0);

			VertexBuffer matrices(mvMats.data(), mvMats.size() * sizeof(Mat4));
			VertexBufferLayout layoutMatrices;
			layoutMatrices.Push<float>(4, false, 1);
			layoutMatrices.Push<float>(4, false, 1);
			layoutMatrices.Push<float>(4, false, 1);
			layoutMatrices.Push<float>(4, false, 1); // divisor of 1 because this stuff changes per instance

			va.AddBuffer(vb, layout);
			va.AddBuffer(matrices, layoutMatrices);

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

			renderer.DrawIndexedInstanced(va, ib, shader, mvMats.size());
		}
	}

	skybox.Draw(renderer, camera);
}

const BlockType& World::GetBlock(const Vec3& worldCoords) {
	if (worldCoords.y >= CHUNK_HEIGHT || worldCoords.y < 0) {
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