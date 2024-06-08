#include <string>
#include <iostream>
#include <random>
// #include <string>
using namespace std;
#include "RobocopEnemy.hpp"
// #include "PlayScene.hpp"\#include "Scene/PlayScene.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

float cooldown = 10.f;
int maxHealth = 0.f;
RobocopEnemy::RobocopEnemy(int x, int y) : Enemy("play/enemy-robocop.png", x, y, 10, 50, 8, 10) {
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
	maxHealth = this->hp;
	cooldown = 10.f;
}

void RobocopEnemy::VirtualUpdate(float deltaTime) {
	// cout << "virtual update triggered for robocop enemy" << deltaTime << endl;
	if (cooldown >= 0.0f){
		cooldown -= 0.5f * deltaTime;
		cout << cooldown << " Before the next regenerate\n";
	} else if (cooldown >= -1.f && cooldown <= 0.5f){
		cout << "Regenerate 5 hp\n";
		this->hp += 5;
		if (this->hp > maxHealth )this->hp = maxHealth;

		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/health-regen.png", 5, this->Position.x, this->Position.y));

		cooldown = 10.f;
	}
}
