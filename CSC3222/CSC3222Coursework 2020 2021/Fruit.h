#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	class Fruit : public SimObject	{
	public:
		Fruit(Vector2 position);
		~Fruit();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;
	protected:
	};
}