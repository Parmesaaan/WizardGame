#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include "../../Common/MeshGeometry.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class SimObject;
		class CollisionVolume;

		struct CollisionInfo {
			CollisionVolume* collider1;
			CollisionVolume* collider2;

			Vector2 collisionNormal;
			float penetration;
		};

		class GameSimsPhysics {
		public:
			GameSimsPhysics();
			~GameSimsPhysics();

			void Update(float dt);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);

		protected:
			void IntegrateAcceleration(float dt);
			void IntegrateVelocity(float dt);
			void CollisionDetection(float dt);
			void CollisionResolution(CollisionInfo* collisionInfo, float dt);
			void ResetForces();

			void RemoveNulls();

			void CollisionDebug(CollisionInfo* collisionInfo);

			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;

			float timeRemaining;
		};
	}
}

