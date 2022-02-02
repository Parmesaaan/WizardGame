#include "Fruit.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

Fruit::Fruit(Vector2 position) : SimObject() {
	texture = texManager->GetTexture("FruitWizard\\Fruit.png");

	int fruitID = rand() % 16;

	animFrameData = Vector4((fruitID / 4) * 16.0f, (fruitID % 4) * 16.0f, 16.0f, 16.0f);
	collider = new CollisionVolume(5.0f, position, CollisionVolume::ColliderType::Fruit, this);

	SetPosition(position);

}

Fruit::~Fruit() {
}

bool Fruit::UpdateObject(float dt) {
	if (deleteMe) {
		return false;
	}

	collider->SetPos(position);
	return true;
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::Fruit::CollisionLogic(SimObject* buddy)
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

	return true;
}

