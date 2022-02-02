#include "Froggo.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 froggoIdleFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoWalkFrames[] = {
	Vector4(0  ,(96 * 5) + 32, 80, 64),
	Vector4(80 ,(96 * 5) + 32, 80, 64),
	Vector4(160,(96 * 5) + 32, 80, 64),
	Vector4(240,(96 * 5) + 32, 80, 64),
	Vector4(320,(96 * 5) + 32, 80, 64),
	Vector4(400,(96 * 5) + 32, 80, 64),
};

Vector4 froggoLadderFrames[] = {
	Vector4(0  ,(96 * 4) + 32, 80, 64),
	Vector4(80 ,(96 * 4) + 32, 80, 64),
	Vector4(160,(96 * 4) + 32, 80, 64),
	Vector4(240,(96 * 4) + 32, 80, 64),
	Vector4(320,(96 * 4) + 32, 80, 64),
	Vector4(400,(96 * 4) + 32, 80, 64),
};

Vector4 froggoStunFrames[] = {
	Vector4(0  ,(96 * 8) + 32, 80, 64),
	Vector4(80, (96 * 8) + 32, 80, 64),
	Vector4(160,(96 * 8) + 32, 80, 64),
	Vector4(240,(96 * 8) + 32, 80, 64),
	Vector4(320,(96 * 8) + 32, 80, 64),
	Vector4(400,(96 * 8) + 32, 80, 64),
};

Vector4 froggoAttackFrames[] = {
	Vector4(0  ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(80 ,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(160,(96 * 1) + 32, 80, 64),
	Vector4(240,(96 * 1) + 32, 80, 64),
};

Vector4 froggoJumpFrames[] = {
	Vector4(0  ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(80 ,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(160,(96 * 7) + 32, 80, 64),
	Vector4(240,(96 * 7) + 32, 80, 64),
};

Froggo::Froggo(std::vector<Platform*> platforms, PlayerCharacter* player) : SimObject() {
	currentAnimState = KingState::Idle;
	texture			 = texManager->GetTexture("FruitWizard\\frogman_crown.png");
	animFrameCount	 = 6;
	inverseMass = 0.01f;

	currentPlatform = platforms[7];
	playerPlatform = platforms[0];
	collider = new CollisionVolume(18.0f, 18.0f, Vector2(position.x, position.y), CollisionVolume::ColliderType::King, this);
	this->player = player;
	this->platforms = platforms;
}

Froggo::~Froggo() {
}

bool Froggo::UpdateObject(float dt) {
	if (deleteMe) {
		return false;
	}

	float moveSpeed = 500.0f;
	float gravity = -1000.0f;
	Vector4* animSource = froggoIdleFrames;

	Pathfinding();

	if (stunned) {
		currentAnimState == KingState::Stun;
		animSource = froggoStunFrames;
		stunnedFor++;
		if (stunnedFor == 500) {
			stunned = false;
			currentAnimState = KingState::Idle;
			stunnedFor = 0;
		}
	}
	else {
		if (currentAnimState == KingState::Idle) {
			animSource = froggoIdleFrames;
			AddImpulse(Vector2(0, gravity)); // Gravity
		}
		if (currentAnimState == KingState::Right) {
			animSource = froggoWalkFrames;
			flipAnimFrame = true;
			AddImpulse(Vector2(moveSpeed, 0));
			AddImpulse(Vector2(0, gravity)); // Gravity

			if (onLadder) {
				SetPosition(Vector2(position.x, currentPlatform->y + 1));
			}
		}
		else if (currentAnimState == KingState::Left) {
			animSource = froggoWalkFrames;
			flipAnimFrame = false;
			AddImpulse(Vector2(-moveSpeed, 0));
			AddImpulse(Vector2(0, gravity)); // Gravity

			if (onLadder) {
				SetPosition(Vector2(position.x, currentPlatform->y + 1));
			}
		}
		else if (currentAnimState == KingState::Up) {
			animSource = froggoLadderFrames;
			AddImpulse(Vector2(0, moveSpeed / 2));
		}
		else if (currentAnimState == KingState::Down) {
			animSource = froggoLadderFrames;
			AddImpulse(Vector2(0, -moveSpeed / 2));
			//AddImpulse(Vector2(0, gravity)); // Gravity
		}
		else if (currentAnimState == KingState::JumpRight) {
			animSource = froggoJumpFrames;
			flipAnimFrame = false;
			AddImpulse(Vector2(moveSpeed * 2, 0));
		}
		else if (currentAnimState == KingState::JumpLeft) {
			animSource = froggoJumpFrames;
			flipAnimFrame = true;
			AddImpulse(Vector2(-moveSpeed * 2, 0));
		}
	}

	movingToLadder = false;
	animFrameData = animSource[currentanimFrame];
	collider->SetPos(Vector2(position.x, position.y));
	return true;
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::Froggo::CollisionLogic(SimObject* buddy)
{
	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Player) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Guard) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::King) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Fruit) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Pixie) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::PixieDust) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Spell) {
		if (!stunned) {
			int impulseStrength = (buddy->GetPosition().x > GetPosition().x) ? -80000 : 80000;
			AddImpulse(Vector2(impulseStrength, impulseStrength / 2));
			stunned = true;
			return true;
		}

		return false;
	}

	return true;
}

// Set if froggo is on a ladder or not
void NCL::CSC3222::Froggo::SetOnLadder()
{
	onLadder = true;
}

// A custom placeholder pathfinding algorithm for until I implement A* pathfinding
void NCL::CSC3222::Froggo::Pathfinding() {

	// Find froggo's platform
	bool currentKingSafeX = (currentPlatform->startX <= position.x) && (currentPlatform->endX >= position.x);
	bool currentKingSafeY = ((currentPlatform->y - 20) <= position.y) && ((currentPlatform->y + 20) >= position.y);

	if (!currentKingSafeX || !currentKingSafeY) {
		for (Platform* p : platforms) {

			bool pKingSafeX = (p->startX <= position.x) && (p->endX >= position.x);
			bool pKingSafeY = ((p->y - 20) <= position.y) && ((p->y + 20) >= position.y);

			if (pKingSafeX && pKingSafeY) {
				currentPlatform = p;
				currentLadderSearchX = 0;
				std::cout << "[Pathfinding] King's platform set to " << p->startX << "-" << p->y << std::endl;
			}
		}
	}

	// Find players platform
	Vector2 playerPosition = player->GetPosition();
	bool currentPlayerSafeX = (playerPlatform->startX <= playerPosition.x) && (playerPlatform->endX >= playerPosition.x);
	bool currentPlayerSafeY = ((playerPlatform->y - 20) <= playerPosition.y) && ((playerPlatform->y + 20) >= playerPosition.y);

	if (!currentPlayerSafeX || !currentPlayerSafeY) {
		for (Platform* p : platforms) {

			bool pPlayerSafeX = (p->startX <= playerPosition.x) && (p->endX >= playerPosition.x);
			bool pPlayerSafeY = ((p->y - 20) <= playerPosition.y) && ((p->y + 20) >= playerPosition.y);

			if (pPlayerSafeX && pPlayerSafeY){
				playerPlatform = p;
				std::cout << "[Pathfinding] Player's platform set to " << p->startX << "-" << p->y << std::endl;
			}
		}
	}

	// Determine which direction the player is in
	bool xToPlayer = (playerPosition.x > position.x) ? true : false; // True = Right
	bool yToPlayer = (playerPlatform->y > position.y) ? true : false; // True = Up

	// Figure out where to go to get to the player
	if (currentPlatform == playerPlatform) {
		currentAnimState = (xToPlayer) ? KingState::Right : KingState::Left;
	}
	else {
		if (playerPlatform->y != currentPlatform->y) { // If at a different elevation

			// Check if the king is already moving towards a ladder
			movingToLadder = (((currentLadderSearchX - 4) <= position.x) && ((currentLadderSearchX + 4) >= position.x)) ? false : true;
			if (onLadder && !movingToLadder) { // If at a ladder, climb (yToPlayer) it
				currentAnimState = (ladderDirection) ? KingState::Up : KingState::Down;
			}
			else { // Else, Find the closest ladder to the player and walk towards it
				float closestLadder = -1;
				if (yToPlayer) { // Player is above, find a ladder

					// If theres a ladder that goes up, go to the closest one to the player
					for (float xup : currentPlatform->ladderUpX) {
						if (closestLadder < 0 || abs(playerPosition.x - xup) < abs(playerPosition.x - closestLadder)) {
							closestLadder = xup;
							ladderDirection = true;
						}
					}

					if (closestLadder < 0.0f) { // Otherwise find the closest one that goes down
						for (float xdown : currentPlatform->ladderDownX) {
							if (closestLadder < 0 || abs(playerPosition.x - xdown) < abs(playerPosition.x - closestLadder)) {
								closestLadder = xdown;
								ladderDirection = false;
							}
						}
					}
				}
				else { // Player is below, find a ladder 
					// If theres a ladder that goes down, go to the closest one to the player
					for (float ydown : currentPlatform->ladderDownX) {
						if (closestLadder < 0 || abs(playerPosition.x - ydown) < abs(playerPosition.x - closestLadder)) {
							closestLadder = ydown;
							ladderDirection = false;
						}
					}

					if (closestLadder < 0) { // Otherwise find the closest one that goes up
						for (float yup : currentPlatform->ladderUpX) {
							if (closestLadder < 0 || abs(playerPosition.x - yup) < abs(playerPosition.x - closestLadder)) {
								closestLadder = yup;
								ladderDirection = true;
							}
						}
					}
				}

				// Move towards the closestLadder
				currentLadderSearchX = closestLadder;
				movingToLadder = (currentLadderSearchX > 0) ? true : false;
				if (movingToLadder) {
					currentAnimState = (position.x < closestLadder) ? KingState::Right : KingState::Left;
				}
				else {
					currentAnimState = KingState::Idle;
				}
			}
		}
		else if ((playerPlatform->startX != currentPlatform->startX) || (playerPlatform->endX != currentPlatform->endX)) { // Same elevation different platform
			currentAnimState = (xToPlayer) ? KingState::Right : KingState::Left;

			// If king is over a ledge, remove gravity and play the jump animation
			if (position.x < currentPlatform->startX || position.x > currentPlatform->endX) {
				currentAnimState = (xToPlayer) ? KingState::JumpRight : KingState::JumpLeft;
			}
		}
	}
}
