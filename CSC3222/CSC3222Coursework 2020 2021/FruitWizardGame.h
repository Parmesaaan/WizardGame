#pragma once
#include <vector>

namespace NCL::CSC3222 {
	class GameMap;
	class SimObject;
	class Guard;
	class Fruit;
	class Pixie;
	class Froggo;
	class GameSimsRenderer;
	class GameSimsPhysics;
	class TextureBase;
	class TextureManager;
	class PlayerCharacter;

	struct Platform {
		Platform(float startX, float endX, float y, std::vector<float> ladderUpX, std::vector<float> ladderDownX) {
			this->startX = startX;
			this->endX = endX;
			this->y = y;
			this->ladderUpX = ladderUpX;
			this->ladderDownX = ladderDownX;
		}

		float startX;
		float endX;
		float y;

		int guardCount = 0;
		int fruitCount = 0;
		int pixieCount = 0;
		int dustCount = 0;

		std::vector<float> fruitX;
		std::vector<float> dustX;
		std::vector<float> pixieX;
		std::vector<float> ladderUpX;
		std::vector<float> ladderDownX;
	};

	class FruitWizardGame {
	public:
		FruitWizardGame();
		~FruitWizardGame();

		void Update(float dt);

		void AddNewObject(SimObject* object);

		bool GetGameOver() {
			return gameOver;
		}

	protected:
		void InitialiseGame();

		void SpawnFruit(int fruitCount);
		void SpawnDust();
		void SpawnGuards(int guardCount);
		void SpawnPixies(int pixieCount);
		void StartPixieMode();
		void SpawnTheKing();
		void EndPixieMode();
		void OnDeath();

		TextureManager*		texManager;
		GameSimsRenderer*	renderer;
		GameSimsPhysics*	physics;
		GameMap* currentMap;

		PlayerCharacter* player;
		Froggo* king;

		float gameTime;
		float dustTimer;

		int currentScore;
		int magicCount;
		int dustCount;
		int lives;

		int dustOnMap;
		int fruitOnMap;
		int pixiesOnMap;
		int guardsOnMap;

		bool kingSpawned = false;
		bool pixieMode = false;
		bool gameOver = false;

		std::vector<SimObject*> gameObjects;
		std::vector<SimObject*> newObjects;

		std::vector<Platform*> platforms;
		std::vector<Fruit*> fruits;
		std::vector<Guard*> guards;
		std::vector<Pixie*> pixies;
	};
}