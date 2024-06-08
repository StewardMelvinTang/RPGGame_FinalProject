#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

class Enemy : public Engine::Sprite {
protected:
 	std::vector<Engine::Point> path;
	float speed;
	float speedMultiplier = 1.0f;
	float hp;
	int money;
	PlayScene* getPlayScene();
	virtual void OnExplode();

	float glueSlowCooldown = 5.0f;
public:
	float reachEndTime;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;
	Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money);
 	void Hit(float damage, std::string turretName = "");
 	// void Hit(float damage, );
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	void Draw() const override;

	virtual void VirtualUpdate(float deltatime) = 0;
};
#endif // ENEMY_HPP
