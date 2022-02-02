#pragma once
#include "SimObject.h"
#include "FruitWizardGame.h"
#include "PlayerCharacter.h"

namespace NCL::CSC3222 {
	class Froggo : public SimObject {
	public:
		Froggo(std::vector<Platform*> platforms, PlayerCharacter* player);
		~Froggo();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;
		void SetOnLadder();
		bool IsStunned() {
			return stunned;
		}
	protected:
		enum class KingState {
			Idle,
			Left,
			Right,
			Up,
			Down,
			JumpLeft,
			JumpRight,
			Stun,
			Attack
		};

		void Pathfinding();

		bool onLadder = false;
		bool ladderDirection = false;
		bool stunned = false;
		float stunnedFor = 0;

		bool movingToLadder = false;
		float currentLadderSearchX = 0;

		KingState		currentAnimState;

		Platform* currentPlatform;
		std::vector<Platform*> platforms;

		PlayerCharacter* player;
		Platform* playerPlatform;
	};
}