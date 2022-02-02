#include "Pixie.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
#include "Guard.h"

using namespace NCL;
using namespace CSC3222;

Vector4 pixieFrames[] = {
	Vector4(32 * 1,32 * 4, 32, 32),
	Vector4(32 * 2,32 * 4, 32, 32),
	Vector4(32 * 3,32 * 4, 32, 32),
	Vector4(32 * 4,32 * 4, 32, 32),
};

Pixie::Pixie(Vector2 position, FlockingInfo* flockingInfo) : SimObject() {
	texture			= texManager->GetTexture("FruitWizard\\super_random_sprites.png");
	animFrameCount	= 4;

	inverseMass = 0.1f;
	collider = new CollisionVolume(8.0f, position, CollisionVolume::ColliderType::Pixie, this);
	SetPosition(position);
	this->flockingInfo = flockingInfo;
}

Pixie::~Pixie() {
}

bool Pixie::UpdateObject(float dt) {
	if (deleteMe) {
		return false;
	}

	Flocking(flockingInfo, dt);

	float distanceToPlayer = (position - flockingInfo->player->GetPosition()).Length();
	Vector2 directionToPlayer = (flockingInfo->player->GetPosition() - position).Normalised();

	// Move closer to the player, stronger if farther
	AddImpulse(directionToPlayer * distanceToPlayer / 120);

	// Move closer to the player, stronger if closer
	AddImpulse(directionToPlayer * (1/ distanceToPlayer) * 600);

	animFrameData = pixieFrames[currentanimFrame];
	collider->SetPos(position);
	return true;
}

// The property of members in a flock to broadly travel in the same direction as each other
Vector2 NCL::CSC3222::Pixie::Alignment(std::vector<Pixie*>* pixies)
{
	Vector2 direction = this->velocity;
	float aligmentThreshold = 40;

	for (Pixie* buddy : *pixies) {

		if (buddy == this) {
			continue;
		}

		float distance = (this->position - buddy->GetPosition()).Length();

		if (distance > aligmentThreshold) {
			continue;
		}

		direction += buddy->GetVelocity();
	}

	return direction.Normalised();
}

// Make Boids try and move away from their neighbours, such that collisions can’t happen
Vector2 NCL::CSC3222::Pixie::Separation(std::vector<Pixie*>* pixies)
{
	Vector2 direction;
	float separationThreshold = 20;

	for (Pixie* buddy : *pixies) {

		if (buddy == this) {
			continue;
		}

		float distance = (this->position - buddy->GetPosition()).Length();

		if (distance > separationThreshold) {
			continue;
		}

		float strength = 1.0f - (distance / separationThreshold);
		direction += (this->position - buddy->GetPosition()).Normalised() * strength;
	}

	return direction.Normalised();
}

// Prevents Boids moving too far away from their flock
Vector2 NCL::CSC3222::Pixie::Cohesion(std::vector<Pixie*>* pixies)
{
	Vector2 average = this->position;
	float count = 1;
	float cohesionThreshold = 20;

	for (Pixie* buddy : *pixies) {
		if (buddy == this) {
			continue;
		}
		float distance = (this->position - buddy->GetPosition()).Length();

		if (distance > cohesionThreshold) {
			continue;
		}
		average += buddy->GetPosition();
		count++;
	}
	average /= count;
	Vector2 direction = average - this->position;
	return direction.Normalised();
}

// Boids of one type (pixies) try to avoid Boids of another (guard)
Vector2 NCL::CSC3222::Pixie::Avoidance(std::vector<Guard*>* guards)
{
	Vector2 direction;
	float avoidanceThreshold = 50;

	for (Guard* guard : *guards) {
		if (guard->GetCollider()->GetColliderType() == this->GetCollider()->GetColliderType()) {
			continue;
		}
		float distance = (this->position - guard->GetPosition()).Length();

		if (distance > avoidanceThreshold) {
			continue;
		}
		direction = (this->position - guard->GetPosition()).Normalised();
	}

	return direction.Normalised();
}

// Flocking implemented via the Boids algorithm
void NCL::CSC3222::Pixie::Flocking(FlockingInfo* flockingInfo, float dt)
{
	std::vector<Pixie*>* pixies = flockingInfo->pixies;
	std::vector<Guard*>* guards = flockingInfo->guards;

	for (Pixie* pixie : *pixies) {
		Vector2 move;
		move += Alignment(pixies) * 20;
		move += Separation(pixies) * 20;
		move += Cohesion(pixies) * 60;
		move += Avoidance(guards) * 2000;

		pixie->SetVelocity(pixie->GetVelocity() + move * dt);
	}
}

// Check the collision logic to apply against the colliding object
bool NCL::CSC3222::Pixie::CollisionLogic(SimObject* buddy)
{
	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Player) {
		deleteMe = true;
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Guard) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::King) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Fruit) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Pixie) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::PixieDust) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Spell) {
	}

	if (buddy->GetCollider()->GetColliderType() == CollisionVolume::ColliderType::Ladder) {
	}

	return false;
}