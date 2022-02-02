#include "Spell.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"

#include "../../Common/Maths.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"

using namespace NCL;
using namespace CSC3222;

Vector4 activeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(384,416, 32, 32),
};

Vector4 explodeFrames[] = {
	Vector4(512,384, 32, 32),
	Vector4(384,416, 32, 32),
	Vector4(416,416, 32, 32),
	Vector4(448,416, 32, 32),
	Vector4(480,416, 32, 32),
	Vector4(512,416, 32, 32),	
};

Spell::Spell(bool direction, PlayerCharacter* player) : SimObject()	{
	texture		= texManager->GetTexture("FruitWizard\\mini_fantasy_sprites_oga_ver.png");
	animFrameCount = 6;
	collider = new CollisionVolume(8.0f, position, CollisionVolume::ColliderType::Spell, this);
	this->player = player;
	SetPosition(player->GetPosition());
	inverseMass = 0.25;

	int projectileSpeed = 4000;
	if (direction) { // Right
		AddImpulse(Vector2(-projectileSpeed, 0));
	}
	else { // Left
		AddImpulse(Vector2(projectileSpeed, 0));
	}
}

Spell::~Spell()	{
}

void Spell::DrawObject(GameSimsRenderer &r) {
	Vector4	texData = explodeFrames[currentanimFrame];
	Vector2 texPos	= Vector2(texData.x, texData.y);
	Vector2 texSize = Vector2(texData.z, texData.w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}

bool Spell::UpdateObject(float dt) {
	if (bounces >= 3 || ((!(GetVelocity().x < -0.5) && !(GetVelocity().x > 0.5)) && (!(GetVelocity().y < -0.5) && !(GetVelocity().y > 0.5)))) {
		deleteMe = true;
		std::cout << "[Player] Miss!" << std::endl;
	}

	if (deleteMe) {
		return false;
	}

	animFrameData = explodeFrames[currentanimFrame];
	collider->SetPos(position);
	return true;
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::Spell::CollisionLogic(SimObject* buddy)
{
	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Player) {
		return false;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Guard) {
		player->GetPlayerAction()->damagedGuard = true;
		deleteMe = true;
		return true;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::King) {
		return true;
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

	bounces++;
	return true;
}
