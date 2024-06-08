#ifndef ROBOCOPENEMY_HPP
#define ROBOCOPENEMY_HPP
#include "Enemy.hpp"

class RobocopEnemy : public Enemy {
public:
	RobocopEnemy(int x, int y);

	void VirtualUpdate(float deltaTime) override;
};
#endif // SOLDIERENEMY_HPP
