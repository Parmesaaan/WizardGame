#include "GameMap.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "SimObject.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics* physics) {
	tileTexture = texManager.GetTexture("FruitWizard//arcade_platformerV2.png");
	this->physics = physics;

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData = new char[mapWidth * mapHeight];
	mapCosts = new int[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = ((mapHeight - 1 - y) * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';
		}
	}

	BuildMapMesh();
}

GameMap::~GameMap() {
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;

	delete mapMesh;
}

void GameMap::DrawMap(GameSimsRenderer& r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
}

void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 primaryTile;

	AddNewTilePos(0, 0, positions, Vector2(mapWidth * 16.0f, mapHeight * 16.0f));
	AddNewTileTex(0, 176, texCoords, Vector2(160, 128));

	// Hardcoded colliders
	float widthOffset = 0;
	float heightOffset = 16;
	float xOffset = -8;
	float yOffset = (-0.5 * 16);
	float ladderOffset = 0;

	// Floor, left wall and right wall
	AddNewTileCollider(mapWidth * 16, 16, Vector2((mapWidth * 16) / 2, 16));
	AddNewTileCollider(32, (mapHeight * 16) - 16, Vector2(0, ((mapHeight * 16) / 2) + 16));
	AddNewTileCollider(32, (mapHeight * 16) - 16, Vector2((mapWidth * 16), ((mapHeight * 16) / 2) + 16));

	// Y = 5
	AddNewTileCollider((5 * 16) + widthOffset, 16 + heightOffset, Vector2((4 * 16) + xOffset, (5 * 16) + yOffset));
	AddNewTileCollider((1 * 16) + widthOffset, 16 + heightOffset, Vector2((9 * 16) + xOffset, (5 * 16) + yOffset));
	AddNewTileCollider((4 * 16) + widthOffset, 16 + heightOffset, Vector2((13.5 * 16) + xOffset, (5 * 16) + yOffset));
	AddNewTileCollider((12 * 16) + widthOffset, 16 + heightOffset, Vector2((23.5 * 16) + xOffset, (5 * 16) + yOffset));

	// Y = 9
	AddNewTileCollider((11 * 16) + widthOffset, 16 + heightOffset, Vector2((17 * 16) + xOffset, (9 * 16) + yOffset));
	AddNewTileCollider((5 * 16) + widthOffset, 16 + heightOffset, Vector2((27 * 16) + xOffset, (9 * 16) + yOffset));

	// Y = 13
	AddNewTileCollider((8 * 16) + widthOffset, 16 + heightOffset, Vector2((7.5 * 16) + xOffset, (13 * 16) + yOffset));
	AddNewTileCollider((1 * 16) + widthOffset, 16 + heightOffset, Vector2((14 * 16) + xOffset, (13 * 16) + yOffset));
	AddNewTileCollider((1 * 16) + widthOffset, 16 + heightOffset, Vector2((18 * 16) + xOffset, (13 * 16) + yOffset));
	AddNewTileCollider((3 * 16) + widthOffset, 16 + heightOffset, Vector2((22 * 16) + xOffset, (13 * 16) + yOffset));

	// Y = 17
	AddNewTileCollider((4 * 16) + widthOffset, 16 + heightOffset, Vector2((3.5 * 16) + xOffset, (17 * 16) + yOffset));
	AddNewTileCollider((1 * 16) + widthOffset, 16 + heightOffset, Vector2((8 * 16) + xOffset, (17 * 16) + yOffset));
	AddNewTileCollider((13 * 16) + widthOffset, 16 + heightOffset, Vector2((20 * 16) + xOffset, (17 * 16) + yOffset));
	AddNewTileCollider((1 * 16) + widthOffset, 16 + heightOffset, Vector2((29 * 16) + xOffset, (17 * 16) + yOffset));

	// Ladders
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((7.5 * 16) + xOffset, (3 * 16) + ladderOffset));
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((16.5 * 16) + xOffset, (3 * 16) + ladderOffset));
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((23.5 * 16) + xOffset, (7 * 16) + ladderOffset));
	AddNewLadderCollider((8 * 16) + heightOffset, Vector2((2.5 * 16) + xOffset, (9 * 16) + ladderOffset));
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((12.5 * 16) + xOffset, (11 * 16) + ladderOffset));
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((19.5 * 16) + xOffset, (11 * 16) + ladderOffset));
	AddNewLadderCollider((4 * 16) + heightOffset, Vector2((6.5 * 16) + xOffset, (15 * 16) + ladderOffset));
	AddNewLadderCollider((8 * 16) + heightOffset, Vector2((27.5 * 16) + xOffset, (13 * 16) + ladderOffset));


	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];

			switch (tileType) {
			case '0': {
				continue;
			}break; //No tile
				//the normal tile types first
			case 'T':primaryTile = Vector2(320, 0); break; //Top of ladder
			case 'L':primaryTile = Vector2(320, 16); break; //Ladder
			case 'B':primaryTile = Vector2(320, 32); break; //Bottom of ladder

			case 't':primaryTile = Vector2(336, 0); break; //Top of ladder
			case 'l':primaryTile = Vector2(336, 16); break; //Ladder
			case 'b':primaryTile = Vector2(336, 32); break; //Bottom of ladder

			case 'G': {
				primaryTile = Vector2(224, 48);
			}break;//Random grass tiles
			case 'F': { //Left End
				primaryTile = Vector2(208, 48);
			}break;//Random grass tiles
			case 'H': { //Right End
				primaryTile = Vector2(240, 48);
			}break;//Random grass tiles

			case 'W': {//wall
				primaryTile = Vector2(256, 112);
			}break;//Random grass tiles
			case 'w': {//wall top
				primaryTile = Vector2(256, 96);
			}break;//Random grass tiles
			}

			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos, Vector2 tileSize) {
	Vector3 topLeft = Vector3((x + 0) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 topRight = Vector3((x + 1) * tileSize.x, (y + 0) * tileSize.y, 0);
	Vector3 bottomLeft = Vector3((x + 0) * tileSize.x, (y + 1) * tileSize.y, 0);
	Vector3 bottomRight = Vector3((x + 1) * tileSize.x, (y + 1) * tileSize.y, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex, Vector2 texSize) {
	Vector2 topLeft = Vector2((x + 0.0f), (y + texSize.y));
	Vector2 topRight = Vector2((x + texSize.x), (y + texSize.y));
	Vector2 bottomLeft = Vector2((x + 0.0f), (y + 0.0f));
	Vector2 bottomRight = Vector2((x + texSize.x), (y + 0.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}

// Add in a terrain collider for grass and wall tiles
void GameMap::AddNewTileCollider(int width, int height, Vector2 pos) {
	CollisionVolume* newCollider = new CollisionVolume(width, height, pos, CollisionVolume::ColliderType::Terrain);
	physics->AddCollider(newCollider);
}

// Add in a ladder collider
void GameMap::AddNewLadderCollider(int height, Vector2 pos) {
	CollisionVolume* newCollider = new CollisionVolume(2 * 16, height, pos, CollisionVolume::ColliderType::Ladder);
	physics->AddCollider(newCollider);
}

Vector2 NCL::CSC3222::GameMap::WorldPosToTilePos(const Vector2& worldPos) const
{
	return Vector2();
}

Vector2 NCL::CSC3222::GameMap::TilePosToWorldPos(const Vector2& tilePos) const
{
	return Vector2();
}
