#pragma once

#include "Vertex.h"
#include "Camera.h"
#include "Renderer.h"
#include "TerrainGenerator.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <iostream>
#include <unordered_map>
#include "SkyBox.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

enum class BlockType : unsigned char {
	Air		= 0,
	Grass	= 1,
	Dirt	= 2,

	NUM_TYPES // always leave this as the last enumeration
};

enum class ShaderType : unsigned char {
	Basic = 0,

	NUM_TYPES // always leave this as the last enumeration
};

class World {
private:
	class Chunk {
	private:
		class Layer {
		private:
			BlockType blocks[CHUNK_WIDTH][CHUNK_WIDTH];

		public:
			Layer();

			inline const BlockType& GetBlock(const Vec3& blockPos) const { return blocks[(int)blockPos.x][(int)blockPos.z]; }
			inline void SetBlock(const BlockType& id, const Vec3& blockPos) { blocks[(int)blockPos.x][(int)blockPos.z] = id; }
		};





	// class Chunk {
	private:
		Layer layers[CHUNK_HEIGHT];
		std::array<std::array<int, CHUNK_WIDTH>, CHUNK_WIDTH> highestBlockYPerColumn;

	public:
		std::vector<Vertex> verticesLists[(int)BlockType::NUM_TYPES];
		std::vector<unsigned int> indicesLists[(int)BlockType::NUM_TYPES];
		Mat4 mMat;
		bool hasTerrain = false;
		bool isLoaded = false;

		Chunk(const Vec2& position);
		~Chunk();

		const BlockType& GetBlock(const Vec3& blockPos) const;
		void SetBlock(const BlockType& id, const Vec3& blockPos);
		int GetHighestBlockYPerColumn(class Vec2& column);
	};




	struct BlockFace {
		Vertex vertices[4];
		unsigned int indices[6];
	};

	enum class BlockFaces {
		north,
		south,
		east,
		west,
		top,
		bottom
	};

	class BlockData {
		friend class World;
	private:
		BlockFace faces[6];

	public:
		bool isCollidable = false;

		inline const Vertex* GetFaceVertices(const BlockFaces& face) const { return faces[(int)face].vertices; }
		inline const unsigned int* GetFaceIndices(const BlockFaces& face) const { return faces[(int)face].indices; }
	};





// class World {
private:
	std::mutex chunkAccessMutex;
	std::mutex stopMutex;
	bool stop = false;

	SkyBox skybox;
	ShaderProgram shaders[(int)ShaderType::NUM_TYPES];
	Texture textures[(int)BlockType::NUM_TYPES]; // one texture for each block
	BlockData blockDatabase[(int)BlockType::NUM_TYPES];
	Camera& camera;
	Renderer& renderer;
	unsigned int renderDistance = 8;
	long long seed = 1;
	int numChunksPerFrame = 3;

	TerrainGenerator terrainGenerator;
	std::vector<Vec2> renderableChunksList;
	std::vector<Vec2> chunkUnloadList;
	Vec3 camPositionOld;
	Vec2 camChunkCoordsOld;

	std::unordered_map<Vec2, Chunk> chunkMap;
	bool forceVertexUpdate = true;

	void UpdateLoadList();
	void UpdateRenderableList();
	void CleanupUnusedChunks();
	bool ChunkAlreadyExistsIn(const std::vector<Vec2>& v, const Vec2& elem) const;
	bool ChunkOutsideRenderDistance(const Vec2& chunkPos, const Vec2& camChunkCoords, float sqRenderDistance) const ;

	void GenerateChunkTerrain(Chunk* chunk, long long& seed);
	void GenerateBlockData(const BlockType& id, BlockData* blockDatabase);

	bool GetStop();
	void SetStop(bool value);

public:
	World(Camera& cam, Renderer& renderer);
	~World();

	void Update();
	void Draw();
	const BlockType GetBlock(const Vec3& worldCoords);
	void SetBlock(const BlockType& id, const Vec3& worldCoords);
	Chunk* GetChunk(const Vec2& chunkPos);
	void GenerateChunkMesh(Chunk* chunk);

	inline const BlockData& GetBlockDataFor(BlockType id) { return blockDatabase[(unsigned int)id]; };

	static Vec3 GetBlockCoords(Vec3 worldCoords) { return Vec3(abs((int)worldCoords.x % CHUNK_WIDTH), worldCoords.y, abs((int)worldCoords.z % CHUNK_WIDTH)); }
	static Vec2 GetChunkCoords(Vec3 worldCoords) { return Vec2((int)worldCoords.x / CHUNK_WIDTH, (int)worldCoords.z / CHUNK_WIDTH); }

	void PrintDebugInfo();
};