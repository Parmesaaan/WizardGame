#pragma once
#include "SimObject.h"

namespace NCL::CSC3222 {
	struct PlayerAction {
		bool loseLife = false;
		int collectedFruit = 0;
		bool collectedPixieDust = false;
		bool collectedPixie = false;
		bool damagedGuard = false;
		bool castSpell = false;
	};

	class PlayerCharacter : public SimObject	{
	public:
		enum class PlayerState {
			Left,
			Right,
			Attack,
			Fall,
			Die,
			Idle,
			Climb
		};

		PlayerCharacter();
		~PlayerCharacter();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;
		void SetOnLadder();

		PlayerAction* GetPlayerAction() const {
			return playerAction;
		}

		PlayerState GetPlayerState() const {
			return currentAnimState;
		}

		bool dying = false;

	protected:
		PlayerState	currentAnimState;
		PlayerAction* playerAction;

		int blinkingDuration = 0;
		bool onLadder = false;
		float playerHeightOld;
	};
}