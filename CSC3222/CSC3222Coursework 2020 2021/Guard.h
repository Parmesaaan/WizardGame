#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"
#include "PlayerCharacter.h"

namespace NCL::CSC3222 {
	class Guard : public SimObject	{
	public:
		Guard(Platform* initialPlatform, std::vector<Platform*> platforms, PlayerCharacter* player);
		~Guard();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;
		void SetOnLadder();
		bool IsStunned() {
			return stunned;
		}
	protected:
		enum class GuardState {
			Left,
			Right,
			Idle,
			Stun,
			Attack
		};

		GuardState		currentAnimState;
		PlayerCharacter* player;
		Platform*		currentPlatform;
		std::vector<Platform*> platforms;

		bool playerIsClose = false;
		bool playerDirection = false;

		bool onLadder = false;
		bool stunned = false;
		float stunnedFor = 0;
	};
}