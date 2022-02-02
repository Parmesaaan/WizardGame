#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	class PixieDust :	public SimObject	{
	public:
		PixieDust(Vector2 position);
		~PixieDust();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;
	protected:
	};
}
