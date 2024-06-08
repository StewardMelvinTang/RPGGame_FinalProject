#include <string>

#include "WarriorEnemy.hpp"

WarriorEnemy::WarriorEnemy(int x, int y) : Enemy("play/enemy-warrior.png", x, y, 10, 55, 4, 4) {
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
}

void WarriorEnemy::VirtualUpdate(float deltaTime) {
	// cout << "virtual update triggered for robocop enemy" << deltaTime << endl;
}
