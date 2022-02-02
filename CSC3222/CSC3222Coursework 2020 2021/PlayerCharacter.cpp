#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
#include "Guard.h"
#include "Froggo.h"

using namespace NCL;
using namespace CSC3222;

Vector4 runFrames[] = {
	Vector4(64,160, 32, 32),
	Vector4(96,160, 32, 32),
	Vector4(128,160, 32, 32),
	Vector4(160,160, 32, 32),
	Vector4(192,160, 32, 32),
	Vector4(224,160, 32, 32),
};

Vector4 attackFrames[] = {
	Vector4(128,288, 32, 30),
	Vector4(64,224, 32, 32),
	Vector4(160,288, 32, 30),
	Vector4(96,224, 32, 32),
	Vector4(192,288, 32, 29),
	Vector4(64,256, 32, 32)
};

Vector4 idleFrames[] = {
	Vector4(64,128, 32, 32),
	Vector4(96,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(160,128, 32, 32),
	Vector4(128,128, 32, 32),
	Vector4(224,128, 32, 32)
};

Vector4 fallFrames[] = {
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(64,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32),
	Vector4(96,320, 32, 32)
};

Vector4 deathFrames[] = {
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32),
	Vector4(96,352, 32, 32),
	Vector4(128,352, 32, 32)
};

Vector4 ladderFrames[] = {//Not an ideal spritesheet for ladders!
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
	Vector4(64,224, 32, 32),
};


PlayerCharacter::PlayerCharacter() : SimObject() {
	currentAnimState = PlayerState::Right;
	flipAnimFrame = false;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount = 6;
	inverseMass = 0.0125f;
	collider = new CollisionVolume(8.0f, position, CollisionVolume::ColliderType::Player, this);
	playerAction = new PlayerAction();
}

PlayerCharacter::~PlayerCharacter() {

}

bool PlayerCharacter::UpdateObject(float dt) {
	float moveSpeed = 100000.0f;
	float ladderSpeedFactor = 0.5f;
	Vector4* animSource = idleFrames;

	if (!onLadder) {
		AddImpulse(Vector2(0.0f, -980.0f));
	}

	if (dying) {
		if (currentAnimState == PlayerState::Die) {
			animSource = deathFrames;
			blinkingDuration++;
			if (blinkingDuration == 200) {
				currentAnimState = PlayerState::Idle;
				playerAction->loseLife = true;
				blinkingDuration = 0;
			}
		}

		if (!(currentAnimState == PlayerState::Die)) {
			currentAnimState = PlayerState::Die;
		}
		
	}
	else if (currentAnimState == PlayerState::Attack) {
		animSource = attackFrames;
		if (currentanimFrame >= 5) {
			currentAnimState = PlayerState::Idle;
		}
	}
	else if (onLadder && (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP) || Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			currentAnimState == PlayerState::Climb;
			animSource = ladderFrames;
			AddForce(Vector2(0.0f, moveSpeed * ladderSpeedFactor));
			
		}
		else if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			currentAnimState == PlayerState::Climb;
			animSource = ladderFrames;
			AddForce(Vector2(0.0f, -moveSpeed * ladderSpeedFactor));
		}
		else {
			currentAnimState == PlayerState::Climb;
			animSource = ladderFrames;
		}
	}
	else {
		currentAnimState = PlayerState::Idle;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Left;
			AddForce(Vector2(-moveSpeed, 0.0f));
			flipAnimFrame = true;
		}
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			animSource = runFrames;
			currentAnimState = PlayerState::Right;
			AddForce(Vector2(moveSpeed, 0.0f));
			flipAnimFrame = false;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
			playerAction->castSpell = true;
			currentAnimState = PlayerState::Attack;
			currentanimFrame = 0;
		}
		if (GetPosition().y + 0.5f < playerHeightOld) {
			currentAnimState = PlayerState::Fall;
			animSource = fallFrames;
		}
	}
	collider->SetPos(position);
	animFrameData = animSource[currentanimFrame];

	onLadder = false;
	playerHeightOld = GetPosition().y;
	return true;
}

// Check the collision logic to apply against the colliding object
bool PlayerCharacter::CollisionLogic(SimObject* buddy)
{
	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Guard) {
		Guard* guard = dynamic_cast<Guard*>(buddy);
		if (guard->IsStunned()) {

		}
		else {
			dying = true;
		}
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::King) {
		Froggo* froggo = dynamic_cast<Froggo*>(buddy);
		if (froggo->IsStunned()) {

		}
		else {
			dying = true;
		}
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Fruit) {
		playerAction->collectedFruit++;
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Pixie) {
		playerAction->collectedPixie = true;
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::PixieDust) {
		playerAction->collectedPixieDust = true;
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Spell) {
		return false;
	}

	return true;
}

// Set if the player is on a ladder or not
void NCL::CSC3222::PlayerCharacter::SetOnLadder()
{
	onLadder = true;
}