#pragma once
#include "SimObject.h"
#include "Guard.h"
#include "PlayerCharacter.h"

namespace NCL::CSC3222 {
	struct FlockingInfo {
		FlockingInfo(std::vector<Pixie*>* pixies, std::vector<Guard*>* guards, PlayerCharacter* player) {
			this->pixies = pixies;
			this->guards = guards;
			this->player = player;
		}

		std::vector<Pixie*>* pixies;
		std::vector<Guard*>* guards;
		PlayerCharacter* player;
	};

	class Pixie : public SimObject	{
	public:
		Pixie(Vector2 position, FlockingInfo* flockingInfo);
		~Pixie();

		bool UpdateObject(float dt) override;
		bool CollisionLogic(SimObject* buddy) override;

	protected:
		Vector2 Alignment(std::vector<Pixie*>* pixies);
		Vector2 Separation(std::vector<Pixie*>* pixies);
		Vector2 Cohesion(std::vector<Pixie*>* pixies);
		Vector2 Avoidance(std::vector<Guard*>* guards);

		void Flocking(FlockingInfo* flockingInfo, float dt);

		FlockingInfo* flockingInfo;
	};
}