#ifndef GLUEGUNNERTURRET_HPP
#define GLUEGUNNERTURRET_HPP
#include "Turret.hpp"

class GlueGunnerTurret: public Turret {
public:
	static const int Price;
    GlueGunnerTurret(float x, float y);
	void CreateBullet() override;
};
#endif