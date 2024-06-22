#ifndef GAMESCENE_HALL_HPP
#define GAMESCENE_HALL_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Dialog/DialogScreen.hpp"
#include "UI/Component/ImageButton.hpp"

#include "PlayerCharacter/PlayerCharacter.hpp"
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



class GameSceneHall final : public Engine::IScene {
private:
	ALLEGRO_SAMPLE_ID bgmId;
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
protected:
	int SpeedMult;
public:
	enum TileType {
		TILE_DIRT, // 0
		TILE_FLOOR, // 1
		TILE_OCCUPIED, // 2
		TILE_CORNERTOPRIGHT, // 3
		TILE_CORNERBTMRIGHT, // 4
		TILE_CORNERTOPLEFT, // 5
		TILE_CORNERBTMLEFT, // 6
		TILE_HOUSE, // 7
		TILE_BLOCK, // 8
		TILE_MAR // 9
	};

	static bool DebugMode;
	static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
	static const int BlockSize;
	static Engine::Point SpawnGridPoint;
	static const Engine::Point EndGridPoint;

	float ticks;
	float deathCountDown;
	bool isGameOver = false;

	PlayerEntry playerEntryData;

	// * Used for Map Tiling and Grouping
	Group* TileMapGroup;
	Group* BlockGroup;
	Group* ItemGroup;
	Group* GroundEffectGroup;
	Group* EffectGroup;
	Group* UIGroup;
    Group* CharacterSpriteGroup;
	Engine::Label* UIMoney;
	Engine::Label* UILives;
	Engine::Image* imgTarget;
	std::vector<std::vector<TileType>> mapState;
	std::vector<std::vector<BlockType>> mapBlocks;
	std::vector<std::vector<ItemType>> mapItems;
	std::vector<std::vector<int>> mapDistance;

	std::vector<Engine::Point> npcList;
	std::vector<Engine::Point> chestList;
	std::list<int> keyStrokes;

	static Engine::Point GetClientSize();

	PlayerCharacter * playerChar = nullptr;

	Engine::DialogScreen * activeDialog;

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
	void ConstructBlock(int, int, BlockType);
	void ConstructItem(int, int, ItemType);
	int ClampMapPos(int, int);

	void DestroyCurrentActiveDialog(IControl * currActiveDialog);

    bool isGamePaused = false;
	IObject * IMG_PauseMenuBG;
	Engine::ImageButton * BTNPause_Resume, * BTNPause_LoadCP, * BTNPause_BackMenu;
    void ToogleGamePaused(bool newState);
	void OnClickBTNResume();
	void OnClickBTNBackMenu();
	void OnClickBTNLoadCheckpoint();
};
#endif // PLAYSCENE_HPP
