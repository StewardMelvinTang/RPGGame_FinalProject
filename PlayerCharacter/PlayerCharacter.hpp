#ifndef PLAYERCHARACTER_HPP
#define PLAYERCHARACTER_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"

class Bullet;
class PlayScene;

enum Enum_Direction{
    DIRECTION_LEFT, // 0
    DIRECTION_RIGHT, // 1
    DIRECTION_UP, // 2
    DIRECTION_DOWN, // 3
    DIRECTION_UPLEFT, // 4
    DIRECTION_UPRIGHT,// 5
    DIRECTION_DOWNLEFT,// 6
    DIRECTION_DOWNRIGHT// 7
};

class PlayerCharacter : public Engine::Sprite {
protected:
	float speedMultiplier = 1.0f;
	int gold;

    float x = 0, y = 0;
    float size = 64;
    float speed = 2.0;
    float currentHP = 100, maxHP = 100;

    Enum_Direction directionFacing = DIRECTION_DOWN;
    IObject * charSpriteObj = nullptr;
public:


	PlayerCharacter(float x, float y, float speed, float hp, int money, int blockSize);
    ~PlayerCharacter();
	void Update(float deltaTime) override;
	void Draw() const override;

	// virtual void VirtualUpdate(float deltatime) = 0;
    void UpdateCharacterDirection();
    bool CollisionCheck(float newX, float newY);
    void SetMovementState(int keycode, bool keyDown);

    Engine::Point GetPlayerPositionAtMap();


    // * Player HUD (UI) RESOURCES
    IObject * HP_BarBG = nullptr;
    IObject * HP_BarFILL = nullptr;
    Engine::Label * TXT_HPVal = nullptr;
    void ConstructPlayerHUD();
    void DrawPlayerHUD() const;
    void DestroyPlayerHUD();


    float GetCurrentHP() {return currentHP;}
    void SetCurrentHP(float newVal, bool shouldClamp = true);
};
#endif // ENEMY_HPP
