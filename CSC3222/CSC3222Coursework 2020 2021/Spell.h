#pragma once
#include "SimObject.h"
#include "PlayerCharacter.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class Spell : public SimObject {
		public:
			Spell(bool direction, PlayerCharacter* player);
			~Spell();

			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;
			bool CollisionLogic(SimObject* buddy) override;

		protected:
			int bounces = 0;
			PlayerCharacter* player;
		};
	}
}

