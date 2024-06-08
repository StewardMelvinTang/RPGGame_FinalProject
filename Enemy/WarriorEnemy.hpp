#ifndef WARRIORENEMY_HPP
#define WARRIORENEMY_HPP
#include "Enemy.hpp"

class WarriorEnemy : public Enemy {
public:
	WarriorEnemy(int x, int y);
	void VirtualUpdate(float deltaTime) override;
};
#endif // SOLDIERENEMY_HPP
