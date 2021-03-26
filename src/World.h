#pragma once

#include "Vertex.h"
#include "Camera.h"
#include "Renderer.h"
#include "TerrainGenerator.h"
#include "ShaderProgram.h"
#include <iostream>
#include <unordered_map>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

enum BlockType : unsigned char {
	Air = 0,
	Grass = 1,

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
		Mat4 mMat;

		bool isInitialized = false;
		bool isLoaded = false;

	public:
		World& world;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		Chunk(const Vec2& position, World& world);

		const bool IsBlockOutOfBounds(const Vec3& blockPos) const;
		const BlockType& GetBlock(const Vec3& blockPos) const;
		void SetBlock(const BlockType& id, const Vec3& blockPos);
		void Draw(Renderer& renderer, Camera& camera, ShaderProgram& shader, unsigned int textureAtlas);
		void GenerateModel();
		void GenerateTerrain(TerrainGenerator& terrainGenerator, long long& seed);

		inline bool IsInitialized() { return isInitialized; }
		inline bool IsLoaded() { return isLoaded; }
	};





	struct BlockData {
		BlockType type = BlockType::Air;
		bool collidable = false;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};





// class World {
private:
	ShaderProgram shaderBasic;
	unsigned int textureAtlas;
	Camera& camera;
	Renderer& renderer;
	BlockData blockDatabase[BlockType::NUM_TYPES];
	unsigned int renderDistance = 3;
	long long seed = -1;
	int numChunksPerFrame = 3;

	TerrainGenerator terrainGenerator;
	std::vector<Vec2> chunkLoadList;
	std::vector<Vec2> visibleChunksList;
	std::vector<Vec2> renderableChunksList;
	std::vector<Vec2> chunkUnloadList;
	Vec3 camPositionOld;
	Vec2 camChunkCoordsOld;

	std::unordered_map<Vec2, Chunk> chunkMap;
	bool forceVertexUpdate = false;

	void UpdateLoadList();
	void UpdateVisibleList();
	void UpdateRenderableList();
	void UpdateUnloadList();
	bool ChunkAlreadyExistsIn(const std::vector<Vec2>& v, const Vec2& elem) const;
	bool ChunkOutsideRenderDistance(const Vec2& chunkPos, const Vec2& camChunkCoords, float sqRenderDistance) const ;

public:
	World(Camera& cam, Renderer& renderer);

	void Update();
	void Draw();
	const BlockType& GetBlock(const Vec4& worldCoords);
	void SetBlock(const BlockType& id, const Vec4& blockPos);
	Chunk* GetChunk(const Vec2& chunkPos);

	inline const BlockData& GetBlockDataFor(BlockType id) const { return blockDatabase[(unsigned int)id]; };
	inline const unsigned int GetTextureAtlas() const { return textureAtlas; }

	static Vec3 GetBlockCoords(Vec3 worldCoords) { return Vec3((int)worldCoords.x % CHUNK_WIDTH, worldCoords.y, (int)worldCoords.z % CHUNK_WIDTH); }
	static Vec2 GetChunkCoords(Vec3 worldCoords) { return Vec2((int)worldCoords.x / CHUNK_WIDTH, (int)worldCoords.z / CHUNK_WIDTH); }

	void PrintDebugInfo();
};