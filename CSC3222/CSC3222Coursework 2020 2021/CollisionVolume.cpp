#include "CollisionVolume.h"

#ifndef wtypes_h
#include <wtypes.h>
#endif

#ifndef _WINDEF
#include <windef.h>
#endif

using namespace NCL;
using namespace CSC3222;

// Create a bounding box collider
CollisionVolume::CollisionVolume(float radius, Vector2 pos, ColliderType colliderType)
{
	this->width = radius;
	this->height = 0;
	this->pos = pos;
	this->volumeType = VolumeType::BoundingSphere;
	this->colliderType = colliderType;
}

// Create a bounding box collider with a parent
CollisionVolume::CollisionVolume(float radius, Vector2 pos, ColliderType colliderType, SimObject* parent)
{
	this->width = radius;
	this->height = 0;
	this->pos = pos;
	this->volumeType = VolumeType::BoundingSphere;
	this->colliderType = colliderType;
	this->parent = parent;
}

// Create an AABB collider
CollisionVolume::CollisionVolume(float width, float height, Vector2 pos, ColliderType colliderType)
{
	this->width = width;
	this->height = height;
	this->pos = pos;
	this->volumeType = VolumeType::AABB;
	this->colliderType = colliderType;
}

// Create an AABB collider with a parent
CollisionVolume::CollisionVolume(float width, float height, Vector2 pos, ColliderType colliderType, SimObject* parent)
{
	this->width = width;
	this->height = height;
	this->pos = pos;
	this->volumeType = VolumeType::AABB;
	this->colliderType = colliderType;
	this->parent = parent;
}

CollisionVolume::~CollisionVolume()
{

}

// Check if a collision has occured between two bounding spheres
bool NCL::CSC3222::CollisionVolume::CheckBoundingSphere(CollisionInfo* collisionInfo)
{
	CollisionVolume* buddy = collisionInfo->collider2;

	// Check if distance between center points is less than sum of both radii using Pythagoras theorem
	float dx = this->GetPos().x - buddy->GetPos().x;
	float dy = this->GetPos().y - buddy->GetPos().y;
	float dist = sqrt((dx * dx) + (dy * dy));

	bool collision = dist < (this->GetWidth() + buddy->GetWidth());

	if (collision) {
		collisionInfo->penetration = (collisionInfo->collider1->GetWidth() + collisionInfo->collider2->GetWidth()) - dist;
		collisionInfo->collisionNormal = (collisionInfo->collider2->GetPos() - collisionInfo->collider1->GetPos()).Normalised();
	}

	return collision;
}

// Check if a collision has occured between two AABBs
bool NCL::CSC3222::CollisionVolume::CheckAABB(CollisionInfo* collisionInfo)
{
	CollisionVolume* buddy = collisionInfo->collider2;

	bool collision = ((abs(this->GetPos().x - buddy->GetPos().x) * 2 < (this->GetWidth() + buddy->GetWidth())) &&
					  (abs(this->GetPos().y - buddy->GetPos().y) * 2 < (this->GetHeight() + buddy->GetWidth())));

	if (collision) {
		Vector2 faces[4] =
		{
			Vector2(-1, 0),	// Left
			Vector2(1, 0), // Right
			Vector2(0, -1), // Bottom
			Vector2(0, 1) // Top
		};

		Vector2 maxA = this->GetPos() + Vector2(this->GetWidth() / 2, this->GetHeight() / 2);
		Vector2 minA = this->GetPos() - Vector2(this->GetWidth() / 2, this->GetHeight() / 2);;

		Vector2 maxB = buddy->GetPos() + Vector2(buddy->GetWidth() / 2, buddy->GetHeight() / 2);
		Vector2 minB = buddy->GetPos() - Vector2(buddy->GetWidth() / 2, buddy->GetHeight() / 2);

		float distances[4] =
		{
			(maxB.x - minA.x), // Left
			(maxA.x - minB.x), // Right
			(maxB.y - minA.y), // Bottom
			(maxA.y - minB.y) // Top
		};

		float pen = FLT_MAX;
		Vector2 normalAxis;

		for (int i = 0; i < 4; i++) {
			if (distances[i] < pen) {
				pen = distances[i];
				normalAxis = faces[i];
			}
		}

		collisionInfo->penetration = pen;
		collisionInfo->collisionNormal = normalAxis;
	}

	return collision;
}

// Clamp a float
float Clamp(float value, float minValue, float maxValue) {
	value = min(value, maxValue);
	value = max(value, minValue);
	return value;
}

// Clamp a vector2
Vector2 Clamp(Vector2 value, Vector2 minValue, Vector2 maxValue) {
	value.x = min(value.x, minValue.x);
	value.y = min(value.y, minValue.y);

	value.x = max(value.x, minValue.x);
	value.y = max(value.y, minValue.y);
	return value;
}

// Check if a collision has occured between a bounding sphere and an AABB
bool NCL::CSC3222::CollisionVolume::CheckBoundingSphereAABB(CollisionInfo* collisionInfo)
{
	CollisionVolume* buddy;

	if (collisionInfo->collider1->GetVolumeType() == VolumeType::BoundingSphere) {
		buddy = collisionInfo->collider2;
	}
	else {
		buddy = collisionInfo->collider1;
	}

	// Define center points with offset
	float testX = buddy->GetPos().x - (buddy->GetWidth() / 2);
	float testY = buddy->GetPos().y - (buddy->GetHeight() / 2);

	// Closest coords to the bounding sphere from the AABB
	float closestX = Clamp(this->GetPos().x, testX, testX + buddy->GetWidth());
	float closestY = Clamp(this->GetPos().y, testY, testY + buddy->GetHeight());

	// Distance between center of circle and closest point
	float distX = this->GetPos().x - closestX;
	float distY = this->GetPos().y - closestY;
	float distSq = (distX * distX) + (distY * distY);

	// If this distance is larger than the radius of the bounding sphere, there is a collision
	bool collision = distSq < (this->GetWidth()* this->GetWidth());

	if (collision) {
		collisionInfo->penetration = this->GetWidth() - sqrt(distSq);
		collisionInfo->collisionNormal = (sqrt(distSq) == 0) ? Vector2(0, 0) : (this->GetPos() - Vector2(closestX, closestY)) / sqrt(distSq);

		return true;
	}

	return false;
}