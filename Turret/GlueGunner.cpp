#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Bullet/FireBullet.hpp"
#include "Bullet/GlueBullet.hpp"
#include "Engine/Group.hpp"
#include "GlueGunner.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int GlueGunnerTurret::Price = 55;
GlueGunnerTurret::GlueGunnerTurret(float x, float y) :
	// TODO: [CUSTOM-TOOL] You can imitate the 2 files: 'MachineGunTurret.hpp', 'MachineGunTurret.cpp' to create a new turret.
	Turret("play/tower-base-glue.png", "play/turret-glue.png", x, y, 200, Price, 0.5) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}
void GlueGunnerTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new GlueBullet(Position + normalized * 36, diff, rotation, this));
	AudioHelper::PlayAudio("gluegunsfx.wav");
}
