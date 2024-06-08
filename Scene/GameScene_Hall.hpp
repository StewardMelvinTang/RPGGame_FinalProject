#ifndef GAMESCENE_HALL_HPP
#define GAMESCENE_HALL_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"

using namespace std;


// * HANDLE CURRENT PROFILE' PLAYER STATS
struct PlayerStats
{
    string name = "PLAYER";
    float currentHP =- 100, maxHP = 100;
    int gold = 0;

    string weaponName = "Sword";
    float attackDMG = 15.0f;
    float attackCooldown = 1.0f;
};

// * CHARACTER DRAWING DATA HANDLING
enum Enum_Direction{
    DIRECTION_LEFT, // 0
    DIRECTION_RIGHT, // 1
    DIRECTION_UP, // 2
    DIRECTION_DOWN // 3
};
struct Character{
public:
    float x = 0, y = 0;
    float size = 64;
    float speed = 2.0;

    Enum_Direction directionFacing = DIRECTION_DOWN;
};


class GameSceneHall final : public Engine::IScene {
private:
	enum TileType {
		TILE_DIRT, // 0
		TILE_FLOOR, // 1
		TILE_OCCUPIED, // 2
		TILE_CORNERTOPRIGHT, // 3
		TILE_CORNERBTMRIGHT, // 4
		TILE_CORNERTOPLEFT, // 5
		TILE_CORNERBTMLEFT, // 6
		TILE_HOUSE // 7
	};
	ALLEGRO_SAMPLE_ID bgmId;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
protected:
	int SpeedMult;
public:
	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
	static const int BlockSize;
	static Engine::Point SpawnGridPoint;
	static const Engine::Point EndGridPoint;

	float ticks;
	float deathCountDown;

    string currentMapID;

	// * Used for Map Tiling and Grouping
	Group* TileMapGroup;
	Group* GroundEffectGroup;
	Group* EffectGroup;
	Group* UIGroup;
    Group* CharacterSpriteGroup;
	Engine::Label* UIMoney;
	Engine::Label* UILives;
	Engine::Image* imgTarget;
	std::vector<std::vector<TileType>> mapState;
	std::vector<std::vector<int>> mapDistance;
	std::list<int> keyStrokes;

	static Engine::Point GetClientSize();


    // * Default Function Initialization
	explicit GameSceneHall() = default;
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Draw() const override;
	void OnMouseDown(int button, int mx, int my) override;
	void OnMouseMove(int mx, int my) override;
	void OnMouseUp(int button, int mx, int my) override;
	void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;

	void ReadMap();
	void ConstructUI();

    // * Generative Tile Maps
	void ConstructGenerativePathTile(int, int);
	void ConstructGenerativeGrassTile(int, int);
	int ClampMapPos(int, int);

    // * Character Initialization
    IObject * charSpriteObj;
    Character charSpriteData;
    void InitializeCharacter();
    void DrawCharacter() const;
    void UpdateCharacterDirection(Character &character, bool keys[4]);
    bool CollisionCheck(Character &character);

};
#endif // PLAYSCENE_HPP
