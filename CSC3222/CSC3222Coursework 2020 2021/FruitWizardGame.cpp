#include "FruitWizardGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "CollisionVolume.h"
#include "PlayerCharacter.h"
#include "Fruit.h"
#include "Guard.h"
#include "Froggo.h"
#include "Pixie.h"
#include "PixieDust.h"
#include "Spell.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

FruitWizardGame::FruitWizardGame() {
	renderer = new GameSimsRenderer();
	texManager = new TextureManager();
	physics = new GameSimsPhysics();
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

FruitWizardGame::~FruitWizardGame() {
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

// Game loop update
void FruitWizardGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;
	dustTimer += dt;

	renderer->Update(dt);
	physics->Update(dt);
	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		(*i)->UpdateAnimFrame(dt);
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
			CollisionVolume* c = (*i)->GetCollider();
			if (c) {
				physics->RemoveCollider(c);
			}
			delete (*i);
			i = gameObjects.erase(i);
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}

	PlayerAction* playerAction = player->GetPlayerAction();
	if (playerAction->collectedFruit) {
		currentScore = currentScore + 1000;
		fruitOnMap--;
		playerAction->collectedFruit--;
		std::cout << "[Player] Collected a fruit." << std::endl;
	}
	if (playerAction->collectedPixie) {
		currentScore = currentScore + 1000;
		magicCount = magicCount + 1;
		pixiesOnMap--;
		playerAction->collectedPixie = false;
		std::cout << "[Player] Collected a pixie." << std::endl;
	}
	if (playerAction->collectedPixieDust) {
		currentScore = currentScore + 500;
		dustCount = dustCount + 1;
		dustOnMap--;
		playerAction->collectedPixieDust = false;
		std::cout << "[Player] Collected a pixie dust." << std::endl;
	}
	if (playerAction->damagedGuard) {
		currentScore = currentScore + 200;
		playerAction->damagedGuard = false;
		std::cout << "[Player] Hit!" << std::endl;
	}
	if (playerAction->loseLife) {
		if (lives >= 0) {
			std::cout << "[Player] You have died. " << lives << " lives left." << std::endl;
			OnDeath();
			lives--;
			player->dying = false;
			playerAction->loseLife = false;
			player->dying = false;
			player->SetPosition(Vector2(100, 33));
		}
		else {
			std::cout << "Game over." << std::endl;
			gameOver = true;
		}
	}

	if (playerAction->castSpell) {
		if (magicCount > 0) {
			magicCount--;
			Spell* newSpell = new Spell(player->GetFlipAnimFrame(), player);
			AddNewObject(newSpell);
			playerAction->castSpell = false;
			std::cout << "[Player] Casting a spell..." << std::endl;
		}
		else {
			std::cout << "[Player] Can't cast spell, out of magic." << std::endl;
			playerAction->castSpell = false;
		}
	}

	if (fruitOnMap < 1) {
		for (int i = 0; i < platforms.size(); i++) {
			platforms[i]->fruitCount = 0;
			platforms[i]->fruitX.clear();
		}
		if (!kingSpawned) {
			SpawnTheKing();
		}
		currentScore = currentScore + 5000;
		SpawnFruit(16);
		SpawnGuards(1);
	}

	if (dustTimer >= 30.0f) {
		SpawnDust();
		dustTimer = 0.0f;
	}

	if (lives < 0) {
		gameOver = true;
	}

	if (pixieMode && (pixiesOnMap < 1)) {
		EndPixieMode();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K)) {
		std::cout << "[Debug] Player is at (" << player->GetPosition().x << ", " << player->GetPosition().y << ")" << std::endl;
		SpawnTheKing();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::CONTROL)) {
		if (dustCount >= 4 && pixieMode == false) {
			dustCount = dustCount - 4;
			StartPixieMode();
		}
		else {
			std::cout << "[PixieMode] Can't start pixie mode now, " << ((pixieMode) ? "pixie mode is already started." : "not enough dust.") << std::endl;
		}
	}

	renderer->DrawString("Score:" + std::to_string(currentScore),
		Vector2(32, 12), Vector4(1, 1, 1, 1), 100.0f);
	renderer->DrawString("Lives:" + std::to_string(lives),
		Vector2(144, 12), Vector4(1, 0, 0, 1), 100.0f);
	renderer->DrawString("Spells:" + std::to_string(magicCount),
		Vector2(256, 12), Vector4(1, 0, 0.5f, 1), 100.0f);
	renderer->DrawString("Dust:" + std::to_string(dustCount),
		Vector2(366, 12), Vector4(0.5f, 0.3f, 0.8f, 1), 100.0f);

	renderer->Render();
}

// Initialisation of FruitWizardGame
void FruitWizardGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("FruitWizardMap.txt", gameObjects, *texManager, physics);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetPosition(Vector2(100, 33));
	AddNewObject(player);

	platforms.push_back(new Platform(32, 448, 32, std::vector<float> {112, 256}, std::vector<float> {-3})); // Ground floor
	platforms.push_back(new Platform(32, 132, 96, std::vector<float> {32}, std::vector<float> {112})); // Level 1
	platforms.push_back(new Platform(192, 448, 96, std::vector<float> {368}, std::vector<float> {256}));
	platforms.push_back(new Platform(192, 448, 160, std::vector<float> {192, 304, 432}, std::vector<float> {368})); // Level 2
	platforms.push_back(new Platform(32, 208, 224, std::vector<float> {96}, std::vector<float> {32, 192})); // Level 3
	platforms.push_back(new Platform(284, 356, 224, std::vector<float> {-3}, std::vector<float> {304}));
	platforms.push_back(new Platform(32, 116, 288, std::vector<float> {-3}, std::vector<float> {96})); // Level 4
	platforms.push_back(new Platform(226, 448, 288, std::vector<float> {-3}, std::vector<float> {432}));

	srand(time(NULL));
	SpawnGuards(4);
	SpawnFruit(16);

	gameTime = 0;
	currentScore = 0;
	magicCount = 4;
	dustCount = 0;
	lives = 3;
}

// Spawn in a number of fruit
void NCL::CSC3222::FruitWizardGame::SpawnFruit(int fruitCount)
{
	std::cout << "[SpawnFruit] Spawing " << fruitCount << "x fruit." << std::endl;
	while (fruitCount > 0) {
		int rNum = rand() % (platforms.size() - 1) + 1;
		if (platforms[rNum]->fruitCount < 3) {

			// Generate a random position on a random platform
			int random = rand() % ((int)(platforms[rNum]->endX - platforms[rNum]->startX)) + (int)platforms[rNum]->startX;
			Vector2 position = Vector2(random, platforms[rNum]->y - 8);

			// Make sure fruits don't overlap
			for (int i = 0; i < platforms[rNum]->fruitX.size(); i++) {
				if (position.x == platforms[rNum]->fruitX[i]) {
					continue;
				}
			}

			// Spawn the fruit and update platform info
			Fruit* newFruit = new Fruit(position);
			fruits.emplace_back(newFruit);
			AddNewObject(newFruit);

			platforms[rNum]->fruitX.emplace_back(position.x);
			platforms[rNum]->fruitCount++;
			fruitOnMap++;
			fruitCount--;
		}
	}
	fruits.shrink_to_fit();
}

// Spawn in a number of pixie dust
void NCL::CSC3222::FruitWizardGame::SpawnDust()
{
	int dustAvailable = dustOnMap + dustCount;
	if (dustAvailable < 4) {
		int dustToSpawn = rand() % 4 + 1;
		int rNum = rand() % (platforms.size() - 1) + 1;
		std::cout << "[SpawnDust] Spawning " << min(dustToSpawn, dustToSpawn - dustAvailable) << "x dust." << std::endl;

		while (dustToSpawn > 0 && dustAvailable < 4) {
			int rNum = rand() % (platforms.size() - 1) + 1;
			if (platforms[rNum]->dustCount < 2) {

				// Generate a random position on a random platform
				int random = rand() % ((int)(platforms[rNum]->endX - platforms[rNum]->startX)) + (int)platforms[rNum]->startX;
				Vector2 position = Vector2(random, platforms[rNum]->y - 8);

				// Make sure dusts don't overlap
				for (int i = 0; i < platforms[rNum]->dustX.size(); i++) {
					if (position.x == platforms[rNum]->dustX[i]) {
						continue;
					}
				}

				// Spawn the dust and update platform info
				AddNewObject(new PixieDust(position));
				platforms[rNum]->dustX.emplace_back(position.x);
				platforms[rNum]->dustCount++;

				dustAvailable++;
				dustOnMap++;
				dustToSpawn--;
			}
		}
	}
	else {
		std::cout << "[SpawnDust] Max dust reached, skipping dust spawn cycle." << std::endl;
	}
}

// Spawn in a number of guards
void NCL::CSC3222::FruitWizardGame::SpawnGuards(int guardCount)
{
	if (guardsOnMap < (platforms.size() - 1)) {
		std::cout << "[SpawnGuards] Spawning " << guardCount << "x guard." << std::endl;
		while (guardCount > 0) {
			int rNum = rand() % (platforms.size() - 1) + 1;
			if (platforms[rNum]->guardCount < 1) {

				// Spawn the guards and update platform info
				Guard* newGuard = new Guard(platforms[rNum], platforms, player);
				guards.emplace_back(newGuard);
				AddNewObject(newGuard);

				platforms[rNum]->guardCount++;
				guardsOnMap++;
				guardCount--;
			}
		}
		guards.shrink_to_fit();
	}
	else {
		std::cout << "[SpawnGuard] Max guards reached, skipping guard spawn cycle." << std::endl;
	}
}

// Spawn in a number of pixies
void NCL::CSC3222::FruitWizardGame::SpawnPixies(int pixieCount)
{
	std::cout << "[SpawnPixies] Spawning " << pixieCount << "x pixie." << std::endl;
	while (pixieCount > 0) {
		int rNum = rand() % (platforms.size() - 1) + 1;
		if (platforms[rNum]->pixieCount < 2) {

			// Generate a random position on a random platform
			int random = rand() % ((int)(platforms[rNum]->endX - platforms[rNum]->startX)) + (int)platforms[rNum]->startX;
			Vector2 position = Vector2(random, platforms[rNum]->y - 8);

			// Make sure pixies don't overlap
			for (int i = 0; i < platforms[rNum]->pixieX.size(); i++) {
				if (position.x == platforms[rNum]->pixieX[i]) {
					continue;
				}
			}

			// Spawn the pixies and update platform info
			FlockingInfo* flockingInfo = new FlockingInfo(&pixies, &guards, player);
			Pixie* newPixie = new Pixie(position, flockingInfo);
			pixies.emplace_back(newPixie);
			AddNewObject(newPixie);

			platforms[rNum]->pixieX.emplace_back(position.x);
			platforms[rNum]->pixieCount++;
			pixiesOnMap++;
			pixieCount--;

		}
	}
	fruits.shrink_to_fit();
}

// Start pixie mode
void NCL::CSC3222::FruitWizardGame::StartPixieMode()
{
	pixieMode = true;
	std::cout << "[PixieMode] Starting pixie mode." << std::endl;
	SpawnPixies(8);
}

void NCL::CSC3222::FruitWizardGame::SpawnTheKing()
{
	king = new Froggo(platforms, player);
	king->SetPosition(Vector2(360, 288));
	AddNewObject(king);
	kingSpawned = true;
}

// End pixie mode
void NCL::CSC3222::FruitWizardGame::EndPixieMode()
{
	pixieMode = false;
	std::cout << "[PixieMode] Ending pixie mode." << std::endl;

	int pixiesToDelete = pixiesOnMap;
	std::cout << "[PixieMode] Deleting " << pixiesOnMap << "x pixie." << std::endl;

	for (int i = 0; i < platforms.size(); i++) {
		platforms[i]->pixieCount = 0;
		platforms[i]->pixieX.clear();
	}

	for (int i = 0; i < pixies.size(); i++) {
		pixies[i]->Delete();
	}

	pixiesOnMap = 0;
	pixies.clear();
}

// Update game state on player death
void NCL::CSC3222::FruitWizardGame::OnDeath()
{
	// Respawn fruits
	int fruitsToRespawn = fruitOnMap;
	std::cout << "[OnDeath] Deleting " << fruitsToRespawn << "x fruit." << std::endl;

	for (int i = 0; i < platforms.size(); i++) {
		platforms[i]->fruitCount = 0;
		platforms[i]->fruitX.clear();
	}

	for (int i = 0; i < fruits.size(); i++) {
		fruits[i]->Delete();
	}

	fruitOnMap = 0;
	fruits.clear();
	SpawnFruit(fruitsToRespawn);

	// Reposition guards
	int guardsToRespawn = guardsOnMap;
	std::cout << "[OnDeath] Deleting " << guardsOnMap << "x guard." << std::endl;

	for (int i = 0; i < platforms.size(); i++) {
		platforms[i]->guardCount = 0;
	}

	for (int i = 0; i < guards.size(); i++) {
		guards[i]->Delete();
	}

	guardsOnMap = 0;
	guards.clear();
	SpawnGuards(guardsToRespawn);

	// Reposition the king
	if (kingSpawned) {
		king->SetPosition(Vector2(360, 288));
	}

	// End pixie mode
	if (pixieMode) {
		EndPixieMode();
	}
}

// Add a new SimObject to the game
void FruitWizardGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}
