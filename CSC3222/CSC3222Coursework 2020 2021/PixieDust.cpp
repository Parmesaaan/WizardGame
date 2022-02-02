#include "PixieDust.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Vector4 animFrames[] = {
	Vector4(64 , 192, 32, 32),
	Vector4(96, 192, 32, 32),
	Vector4(128, 192, 32, 32),
	Vector4(160, 192, 32, 32)
};

PixieDust::PixieDust(Vector2 position) : SimObject() {
	animFrameCount = 4;
	texture = texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");

	collider = new CollisionVolume(8.0f, position, CollisionVolume::ColliderType::PixieDust, this);
	SetPosition(position);
}

PixieDust::~PixieDust() {
}

bool PixieDust::UpdateObject(float dt) {
	if (deleteMe) {
		return false;
	}

	animFrameData = animFrames[currentanimFrame];
	collider->SetPos(position);
	return true;
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::PixieDust::CollisionLogic(SimObject* buddy)
{
	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Player) {
		deleteMe = true;
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
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Ladder) {
		return false;
	}

	return true;
}
