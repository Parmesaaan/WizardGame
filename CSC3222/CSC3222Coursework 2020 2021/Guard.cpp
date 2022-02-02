#include "Guard.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 guardIdleFrames[] = {
	Vector4(384,32, 64, 32),
	Vector4(448,32, 64, 32),
	Vector4(512,32, 64, 32),
	Vector4(576,32, 64, 32),
	Vector4(448,64, 64, 32),
	Vector4(512,64, 64, 32),
};

Vector4 guardWalkFrames[] = {
	Vector4(384,96, 64, 32),
	Vector4(448,96, 64, 32),
	Vector4(512,96, 64, 32),
	Vector4(576,96, 64, 32),
	Vector4(640,96, 64, 32),
	Vector4(704,96, 64, 32),
};

Vector4 guardAttackFrames[] = {
	Vector4(384,128, 64, 32),
	Vector4(448,128, 64, 32),
	Vector4(512,128, 64, 32),
	Vector4(576,128, 64, 32),
	Vector4(640,128, 64, 32),
	Vector4(704,128, 64, 32),
};

Vector4 guardStunFrames[] = {
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
	Vector4(448,160, 64, 32),
};

Guard::Guard(Platform* initialPlatform, std::vector<Platform*> platforms, PlayerCharacter* player) : SimObject() {
	currentAnimState = GuardState::Right;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount = 6;
	inverseMass = 0.025f;
	this->currentPlatform = initialPlatform;
	this->platforms = platforms;
	SetPosition(Vector2(currentPlatform->startX, currentPlatform->y + 1));
	collider = new CollisionVolume(18.0f, 18.0f, Vector2(position.x, position.y + 1), CollisionVolume::ColliderType::Guard, this);
	this->player = player;
}

Guard::~Guard() {

}

bool Guard::UpdateObject(float dt) {
	if (deleteMe) {
		return false;
	}

	float moveSpeed = 200.0f;	// 750.0f
	Vector4* animSource = guardIdleFrames;

	// Reallocate platform if not on currentPlatform anymore
	if (position.y < currentPlatform->y - 10 || position.y > currentPlatform->y + 10) {
		for (Platform* p : platforms) {
			if ((position.y >= p->y - 10 && position.y <= p->y + 10) &&
				(position.x >= p->startX && position.x <= p->endX)) {
				currentPlatform = p;
			}
		}
	}

	// Get information on the player
	float xDistanceToPlayer = abs(position.x - player->GetPosition().x);
	float yDistanceToPlayer = abs(position.y - player->GetPosition().y);
	if (xDistanceToPlayer < (4 * 16) &&
		yDistanceToPlayer < 10) {
		playerIsClose = true;
		if (player->GetPosition().x <= position.x) {
			playerDirection = false;
		}
		else {
			playerDirection = true;
		}
	}
	else {
		playerIsClose = false;
	}

	// Attack player if he's close
	if (xDistanceToPlayer < (2 * 16) &&
		yDistanceToPlayer < 10 &&
		!(player->dying) &&
		!stunned)
	{
		currentAnimState = GuardState::Attack;
		flipAnimFrame = playerDirection;
	}

	// Ignore ladders
	if (!onLadder) {
		AddImpulse(Vector2(0.0f, -200.0f));
	}
	else {
		SetPosition(Vector2(position.x, currentPlatform->y));
	}

	if (currentAnimState == GuardState::Attack) {
		animSource = guardAttackFrames;
		if (currentanimFrame >= 4) {
			if (xDistanceToPlayer < (2 * 16) &&
				yDistanceToPlayer < 10)
			{
				player->dying = true;
			}
		}

		if (currentanimFrame >= 5) {
			currentAnimState = GuardState::Idle;
		}
	}
	else if (stunned) {
		currentAnimState == GuardState::Stun;
		animSource = guardStunFrames;
		stunnedFor++;
		if (stunnedFor == 500) {
			stunned = false;
			currentAnimState = GuardState::Idle;
			stunnedFor = 0;
		}
	} 
	else {
		if (currentAnimState == GuardState::Idle) {
			if (!(player->dying)) {
				if (!(xDistanceToPlayer < (2 * 16) &&
					yDistanceToPlayer < 10)) {
					currentAnimState = (playerDirection) ? GuardState::Right : GuardState::Left;
				}
			}
		}
		if (currentAnimState == GuardState::Left) {
			animSource = guardWalkFrames;
			flipAnimFrame = false;
			if (position.x <= currentPlatform->startX) {
				currentAnimState = GuardState::Right;
			}
			else {
				if (playerIsClose) {
					if (playerDirection) {
						currentAnimState = GuardState::Right;
					}
				}
				AddImpulse(Vector2(-moveSpeed, 0.0f));
			}
		}
		if (currentAnimState == GuardState::Right) {
			animSource = guardWalkFrames;
			flipAnimFrame = true;
			if (position.x >= currentPlatform->endX) {
				currentAnimState = GuardState::Left;
			}
			else {
				if (playerIsClose) {
					if (!playerDirection) {
						currentAnimState = GuardState::Left;
					}
				}
				AddImpulse(Vector2(moveSpeed, 0.0f));
			}
		}
	}

	onLadder = false;
	collider->SetPos(Vector2(position.x, position.y + 1));
	animFrameData = animSource[currentanimFrame];

	return true;
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::Guard::CollisionLogic(SimObject* buddy)
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
			int impulseStrength = (buddy->GetPosition().x > GetPosition().x) ? -20000 : 20000;
			AddImpulse(Vector2(impulseStrength, impulseStrength/2));
			stunned = true;
			return true;
		}

		return false;
	}

	return true;
}

void NCL::CSC3222::Guard::SetOnLadder()
{
	onLadder = true;
}