#include "GameSimsPhysics.h"
#include "SimObject.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include "PlayerCharacter.h"
#include "Guard.h"
#include "Froggo.h"

using namespace NCL;
using namespace CSC3222;

GameSimsPhysics::GameSimsPhysics() {
	timeRemaining = 0.0f;
}

GameSimsPhysics::~GameSimsPhysics() {

}

void GameSimsPhysics::Update(float dt) {
	timeRemaining += dt;
	const float subTimeDelta = 1.0 / 240;

	RemoveNulls();

	while (timeRemaining > subTimeDelta) {
		IntegrateAcceleration(dt);
		CollisionDetection(dt);
		IntegrateVelocity(dt);
		timeRemaining -= subTimeDelta;
	}

	ResetForces();
}

// Add a rigidbody to the game
void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

// Remove a rigidbody from the game
void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

// Add a collider to the game
void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

// Remove a collider from the game
void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

// Integrate the acceleration of all rigidbodies
void GameSimsPhysics::IntegrateAcceleration(float dt) {
	for (RigidBody* body : allBodies) {
		Vector2 acceleration = body->force * body->inverseMass;
		body->velocity += acceleration * dt;
		body->velocity *= 0.9f;
	}
}

// Remove null rigidbodies (I know this is ugly but it was a last second hotfix to remove crashes on death)
void GameSimsPhysics::RemoveNulls() {
	for (RigidBody* body : allBodies) {
		if (&body) {

		}
		else {
			RemoveRigidBody(body);
			std::cout << "[Physics] A null object was removed." << std::endl;
		}
	}
}

// Integrate the velocity of all rigidbodies
void GameSimsPhysics::IntegrateVelocity(float dt) {
	for (RigidBody* body : allBodies) {
		body->position = body->position + body->velocity * dt;
	}
}

/* COLLISIONS WILL BE TOTALLY REWORKED. TEMPORARY SOLUTION SO I CAN WORK ON OTHER THINGS */

// Detect collisions between objects in the game
void GameSimsPhysics::CollisionDetection(float dt) {
	for (int i = 0; i < allColliders.size(); i++) {
		for (int j = i + 1; j < allColliders.size(); j++) {
			if (allColliders[i]->GetPos() != Vector2(0, 0) && allColliders[j]->GetPos() != Vector2(0, 0)) // Objects spawn on top of each other initially, so this skips that initial check when it would return false collisions.
			{

				CollisionInfo* collisionInfo = new CollisionInfo();
				collisionInfo->collider1 = allColliders[i];
				collisionInfo->collider2 = allColliders[j];

				if (collisionInfo->collider1->GetVolumeType() == collisionInfo->collider2->GetVolumeType()) {
					if (collisionInfo->collider1->GetVolumeType() == CollisionVolume::VolumeType::AABB) { // They are AABBs
						if (collisionInfo->collider1->CheckAABB(collisionInfo)) {
							CollisionResolution(collisionInfo, dt);
						}
					}
					else { // They are Bounding Spheres
						if (collisionInfo->collider1->CheckBoundingSphere(collisionInfo)) {
							CollisionResolution(collisionInfo, dt);
						}
					}
				}
				else {
					if (collisionInfo->collider1->GetVolumeType() == CollisionVolume::VolumeType::AABB) { // collider1 is an AABB and collider2 is a Bounding Sphere
						if (collisionInfo->collider2->CheckBoundingSphereAABB(collisionInfo)) {
							CollisionResolution(collisionInfo, dt);
						}
					}
					else {	// collider1 is a Bounding Sphere and collider2 is an AABB
						if (collisionInfo->collider1->CheckBoundingSphereAABB(collisionInfo)) {
							CollisionResolution(collisionInfo, dt);
						}
					}
				}

				delete collisionInfo;
			}
		}
	}
}

// Disentangles, conserves momentum and applies collision logic to colliding game objects
void GameSimsPhysics::CollisionResolution(CollisionInfo* collisionInfo, float dt) {
	CollisionVolume::ColliderType colType1 = collisionInfo->collider1->GetColliderType();
	CollisionVolume::ColliderType colType2 = collisionInfo->collider2->GetColliderType();

	// Stop walls and ladders from colliding with each other
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Terrain &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Terrain) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Terrain) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Terrain &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder))
	{
		return;
	}

	SimObject* simObject1 = collisionInfo->collider1->GetParent();
	SimObject* simObject2 = collisionInfo->collider2->GetParent();



	// Ladder/player has special interaction
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Player &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Player))
	{
		PlayerCharacter* pc;

		if (collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Player) {
			pc = dynamic_cast<PlayerCharacter*>(simObject1);
		}
		else {
			pc = dynamic_cast<PlayerCharacter*>(simObject2);
		}

		pc->SetOnLadder();

		// Print collision info to console
		//CollisionDebug(collisionInfo);
		return;
	}

	// Ladder/guard has special interaction
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Guard &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Guard))
	{
		Guard* guard;

		if (collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Player) {
			guard = dynamic_cast<Guard*>(simObject1);
		}
		else {
			guard = dynamic_cast<Guard*>(simObject2);
		}

		guard->SetOnLadder();

		// Print collision info to console
		//CollisionDebug(collisionInfo);
		return;
	}

	// Ladder/king has special interaction
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::King &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
			collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::King))
	{
		Froggo* king;

		if (collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Player) {
			king = dynamic_cast<Froggo*>(simObject1);
		}
		else {
			king = dynamic_cast<Froggo*>(simObject2);
		}

		king->SetOnLadder();

		// Print collision info to console
		//CollisionDebug(collisionInfo);
		return;
	}

	// Ladder/spell has special interaction
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Spell &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
			collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Spell))
	{
		return;
	}

	// Fairy ignores ladders
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Pixie &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Ladder) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Ladder &&
			collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Pixie))
	{
		return;
	}

	// Fairy ignores terrain
	if ((collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Pixie &&
		collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Terrain) ||
		(collisionInfo->collider1->GetColliderType() == CollisionVolume::ColliderType::Terrain &&
			collisionInfo->collider2->GetColliderType() == CollisionVolume::ColliderType::Pixie))
	{
		return;
	}

	// Get info for calculations
	float obj1InverseMass = (simObject1) ? simObject1->RigidBody::inverseMass : 0.0f;
	float obj1Elasticity = (simObject1) ? simObject1->RigidBody::elasticity : 1.0f;
	Vector2 obj1Velocity = (simObject1) ? simObject1->RigidBody::velocity : Vector2(0, 0);

	float obj2InverseMass = (simObject2) ? simObject2->RigidBody::inverseMass : 0.0f;
	float obj2Elasticity = (simObject2) ? simObject2->RigidBody::elasticity : 1.0f;
	Vector2 obj2Velocity = (simObject2) ? simObject2->RigidBody::velocity : Vector2(0, 0);

	// Calculate impulse amounts
	float totalMass = obj1InverseMass + obj2InverseMass;
	float e = 1; // Average of both objects elasticities?
	float dot = Vector2::Dot((obj2Velocity - obj1Velocity), collisionInfo->collisionNormal);
	float j = ((-1 * (1 - e)) * dot) / (obj1InverseMass + obj2InverseMass);

	// Check if colliders have SimObjects attached to them before applying anything
	if (simObject1) {
		if (simObject2) {
			if (simObject1->CollisionLogic(simObject2)) {
				simObject1->SetPosition(simObject1->GetPosition() - (collisionInfo->collisionNormal * collisionInfo->penetration * (obj1InverseMass / totalMass)));
				simObject1->AddImpulse(simObject1->GetVelocity() - (collisionInfo->collisionNormal * obj1InverseMass * j));
			}
		}
		else {
			simObject1->SetPosition(simObject1->GetPosition() - (collisionInfo->collisionNormal * collisionInfo->penetration * (obj1InverseMass / totalMass)));
			simObject1->AddImpulse(simObject1->GetVelocity()- (collisionInfo->collisionNormal * obj1InverseMass * j));
		}
	}

	if (simObject2) {
		if (simObject1) {
			if (simObject2->CollisionLogic(simObject1)) {
				simObject2->SetPosition(simObject2->GetPosition() + (collisionInfo->collisionNormal * collisionInfo->penetration * (obj2InverseMass / totalMass)));
				simObject2->AddImpulse(simObject2->GetVelocity() + (collisionInfo->collisionNormal * obj1InverseMass * j));
			}
		}
		else {
			simObject2->SetPosition(simObject2->GetPosition() + (collisionInfo->collisionNormal * collisionInfo->penetration * (obj2InverseMass / totalMass)));
			simObject2->AddImpulse(simObject2->GetVelocity() + (collisionInfo->collisionNormal * obj1InverseMass * j));
		}
	}


	// Print collision info to console
	// CollisionDebug(collisionInfo);
}

// Print information about collisions to the console
void GameSimsPhysics::CollisionDebug(CollisionInfo* collisionInfo) {
	std::cout << "Collision:" << std::endl;

	if (collisionInfo->collider1->GetVolumeType() == CollisionVolume::VolumeType::AABB) {
		std::cout << "Shape 1: AABB | pos =  (" << collisionInfo->collider1->GetPos().x << "," << collisionInfo->collider1->GetPos().y << ") | width = " << collisionInfo->collider1->GetWidth() << " | height = " << collisionInfo->collider1->GetHeight() << std::endl;
	}
	else {
		std::cout << "Shape 1: BoundingSphere | pos =  (" << collisionInfo->collider1->GetPos().x << "," << collisionInfo->collider1->GetPos().y << ") | radius = " << collisionInfo->collider1->GetWidth() << std::endl;
	}

	if (collisionInfo->collider2->GetVolumeType() == CollisionVolume::VolumeType::AABB) {
		std::cout << "Shape 2: AABB | pos =  (" << collisionInfo->collider2->GetPos().x << "," << collisionInfo->collider2->GetPos().y << ") | width = " << collisionInfo->collider2->GetWidth() << " | height = " << collisionInfo->collider2->GetHeight() << std::endl;
	}
	else {
		std::cout << "Shape 2: BoundingSphere | pos =  (" << collisionInfo->collider2->GetPos().x << "," << collisionInfo->collider2->GetPos().y << ") | radius = " << collisionInfo->collider2->GetWidth() << std::endl;
	}

	std::cout << "Penetration: " << collisionInfo->penetration << std::endl;
	std::cout << "Normal: (" << collisionInfo->collisionNormal.x << "," << collisionInfo->collisionNormal.y << ")" << std::endl << std::endl;
}

// Reset all forces on all rigidbodies
void GameSimsPhysics::ResetForces() {
	for (RigidBody* body : allBodies) {
		body->force = Vector2(0.0f, 0.0f);
	}
}
