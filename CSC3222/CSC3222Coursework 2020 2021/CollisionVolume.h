#pragma once
#include "../../Common/Vector2.h"
#include "../../Common/MeshGeometry.h"
#include "GameSimsPhysics.h"
#include "RigidBody.h"

namespace NCL {
	namespace CSC3222 {


		class CollisionVolume {
		public:
			enum class VolumeType {
				BoundingSphere,
				AABB
			};

			enum class ColliderType {
				Player,
				Guard,
				King,
				Fruit,
				Pixie,
				PixieDust,
				Spell,
				Ladder,
				Terrain
			};

			CollisionVolume(float radius, Vector2 pos, ColliderType colliderType);	// Bounding Sphere
			CollisionVolume(float radius, Vector2 pos, ColliderType colliderType, SimObject* parent);	// Bounding Sphere with Parent
			CollisionVolume(float width, float height, Vector2 pos, ColliderType colliderType); // AABB
			CollisionVolume(float width, float height, Vector2 pos, ColliderType colliderType, SimObject* parent); // AABB with Parent
			~CollisionVolume();

			float GetWidth() const {
				return width;
			}
			float GetHeight() const {
				return height;
			}

			Vector2 GetPos() const {
				return pos;
			}

			void SetPos(const Vector2& newPos) {
				pos = newPos;
			}

			VolumeType GetVolumeType() const {
				return volumeType;
			}

			ColliderType GetColliderType() const {
				return colliderType;
			}

			SimObject* GetParent() const {
				return parent;
			}

			bool CheckBoundingSphere(CollisionInfo* collisionInfo);
			bool CheckAABB(CollisionInfo* collisionInfo);
			bool CheckBoundingSphereAABB(CollisionInfo* collisionInfo);

		protected:
			float width;
			float height;

			Vector2 pos;
			SimObject* parent;
			VolumeType volumeType;
			ColliderType colliderType;
		};
	}
}
