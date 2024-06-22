#ifndef PLAYERCHARACTER_HPP
#define PLAYERCHARACTER_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Group.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/IScene.hpp"
#include "Engine/GameEngine.hpp"

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

enum BlockType {
    BLANK,
    BASE_BLOCK,
    BLOCK_CHEST,
};

enum ItemType{
    ITEM_BLANK,
    ITEM_POTION,
    ITEM_MISSILE,
    ITEM_SHIELD,
};

class PlayerCharacter : public Engine::Sprite {
protected:
	float speedMultiplier = 1.0f;
	int money = 0;

    float x = 0, y = 0;
    float size = 64;
    float collission_radius = 32; // think of this as a hitbox
    float speed = 2.0;
    float currentHP = 100, maxHP = 100;
    float attackDamage = 5;
    int currentEXP = 0, maxEXP = 50;
	int playerLevel = 1;

    Enum_Direction directionFacing = DIRECTION_DOWN;
    IObject * charSpriteObj = nullptr;

    std::string currentScene = "gamescene_hall";


public:
    int healthPotion = 0;
    int missile = 2;
    int shield = 0;
	PlayerCharacter(float x, float y, float speed, float hp, int money, int blockSize, std::string mapID, PlayerEntry entry);
    ~PlayerCharacter();
	void Update(float deltaTime) override;
	void Draw() const override;

    void InitializeProfile(PlayerEntry entry);

	// virtual void VirtualUpdate(float deltatime) = 0;
    std::string currentMapID;

    // * Player Functionalities
    void UpdateCharacterDirection();
    bool CollisionCheck(float x, float y, Enum_Direction dir);
    void SetMovementState(int keycode, bool keyDown);
    void OnPlayerDead();

    // * helper function & getter setter
    Engine::Point GetPlayerPositionAtMap();
    void OverlapWithItem(ItemType, int, int);
    float GetCurrentHP() {return currentHP;}
    void SetCurrentHP(float newVal, bool shouldClamp = true);
    float GetPlayerSpeed() {return speed;}
    void SetPlayerSpeed(float newSpeed) {speed = newSpeed;}
    float GetMaxHP() {return maxHP;}
    void SetMaxHP(float newMaxHP) {maxHP = newMaxHP;}
    int GetMoney() {return money;}
    void SetMoney(int newAmount) {money = newAmount;}
    void AddEXP(int amount);
    int GetCurrentEXP() { return currentEXP;}
    int GetCurrentLevel() { return playerLevel;}

    // * Player HUD (UI) RESOURCES
    IObject * LevelUpBG = nullptr;
    IObject * HP_BarBG = nullptr, * EXP_BarBG = nullptr;
    IObject * HP_BarFILL = nullptr, * EXP_BarFILL = nullptr;
    Engine::Label * TXT_HPVal = nullptr, * TXT_EXPVal = nullptr;
    IObject * HotBarBG = nullptr;
    IObject * IMG_Potion = nullptr, * IMG_Missile = nullptr, * IMG_Shield = nullptr;
    IObject * TXT_Potion = nullptr,*  TXT_Missile = nullptr, * TXT_Shield = nullptr;
    void ConstructPlayerHUD();
    void DrawPlayerHUD() const;
    void DestroyPlayerHUD();

    void CheckPointSave(std::vector<std::vector<ItemType>> itemData, std::vector<std::vector<BlockType>> blockData);
    void LoadPlayerEntryData(std::string ); // * To Load from profile lists
    void SaveSceneItemBlockData(std::vector<std::vector<ItemType>>, std::vector<std::vector<BlockType>>);
};
#endif // ENEMY_HPP
