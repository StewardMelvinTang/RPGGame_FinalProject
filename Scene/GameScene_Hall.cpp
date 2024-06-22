#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <cstdlib>  // For std::rand and std::srand
#include <ctime>    // For std::time
#include <string>
#include <memory>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Resources.hpp"
#include "UI/Dialog/DialogScreen.hpp"

#include "PlayerCharacter/PlayerCharacter.hpp"

#include "GameScene_Hall.hpp"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "Scene/Combat/CombatScene.hpp"
// #include "GameScene_Hall.hpp"
using namespace std;


// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4
#define KEYBOARD_ESC 59


bool GameSceneHall::DebugMode = false;
const std::vector<Engine::Point> GameSceneHall::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int GameSceneHall::MapWidth = 25, GameSceneHall::MapHeight = 13;
const int GameSceneHall::BlockSize = 64;

bool mapAllInitialized = false;

Engine::Point GameSceneHall::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void GameSceneHall::Initialize() {
	Engine::GameEngine::currentActiveScene = "gamescene_hall";

	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	SpeedMult = 1;
	isGamePaused = false;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// * Group Initialization
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(ItemGroup = new Group());
	AddNewObject(BlockGroup = new Group());
	AddNewControlObject(UIGroup = new Group());
	AddNewControlObject(CharacterSpriteGroup = new Group());

	ReadMap();
	ConstructUI();

	// * Load Player Data from Profile Based Saving System
	playerEntryData = Engine::GameEngine::GetInstance().GetCurrentActivePlayer();
	Engine::Point spawnPoint = Engine::GameEngine::GetInstance().GridToXYPosition(10, 5, BlockSize);
	if (playerEntryData.x != -1 && playerEntryData.y != -1) {
		spawnPoint.y = playerEntryData.y; spawnPoint.x = playerEntryData.x;
		// cout << "Player Will Spawn at " << playerEntryData.y << " X : " << playerEntryData.x << endl;
	}
	playerChar = new PlayerCharacter(spawnPoint.x, spawnPoint.y , 3.0, 100, 50, BlockSize, Engine::GameEngine::currentActiveScene, playerEntryData);
	cout << "INITIALIZED WITH NAME " << playerEntryData.name << endl;

	for (int i = 0; i < MapHeight; i++){
		for (int j = 0; j < MapWidth; j++){
			cout << mapItems[i][j];
		}
		cout << endl;
	}

	mapAllInitialized = true;
}


void GameSceneHall::Terminate() {
	// AudioHelper::StopBGM(bgmId);
	// AudioHelper::StopSample(deathBGMInstance);
	// deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	cout << "Terminated\n";
	IScene::Terminate();

}
void GameSceneHall::Update(float deltaTime) {
    if (playerChar != nullptr) {
		playerChar->Update(deltaTime);
		if (!mapAllInitialized) return;
		Engine::Point playerPos = playerChar->GetPlayerPositionAtMap();
		if (mapItems[playerPos.y][playerPos.x] != ITEM_BLANK){
			playerChar->OverlapWithItem(mapItems[playerPos.y][playerPos.x], playerPos.y, playerPos.x);
			for (auto & item : ItemGroup->GetObjects()){
				if (item->Position.y == playerPos.y * BlockSize && item->Position.x == playerPos.x * BlockSize){
					ItemGroup->RemoveObject(item->GetObjectIterator());
					mapItems[playerPos.y][playerPos.x] = ITEM_BLANK;
					break;
				}
			}
		}
	}
}
void GameSceneHall::Draw() const {
	IScene::Draw();
    if (playerChar != nullptr && (!activeDialog || activeDialog->Enabled == false) && !isGamePaused){
		playerChar->Draw();
	} 
}
void GameSceneHall::OnMouseDown(int button, int mx, int my) {
	IScene::OnMouseDown(button, mx, my);
}
void GameSceneHall::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
}

void GameSceneHall::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
}
void GameSceneHall::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (playerChar != nullptr) playerChar->SetMovementState(keyCode, true);

	if (keyCode == 27){ // * Debug Spawn Dialog
		AddNewControlObject(activeDialog = new Engine::DialogScreen("This is a test dialog. steven ganteng 3D roblox playerqwdqwdqdqwd", "Arthur", 2.0f, playerChar));
		activeDialog->SetOnClickCallback(bind(&GameSceneHall::DestroyCurrentActiveDialog, this, activeDialog));
		cout << "Dialog Screen Initialized\n";
	}

	if (keyCode == 28 && playerChar){
		playerChar->SetCurrentHP(playerChar->GetCurrentHP() - 20);
	}

	if (keyCode == KEYBOARD_ESC){ // * Pause Menu
		ToogleGamePaused(!isGamePaused);
	}

	if (keyCode == 29){
		playerChar->CheckPointSave(mapItems, mapBlocks);
	}

	if (keyCode == 30){
		// * debug : add EXP
		playerChar->AddEXP(500);
	}

	if (keyCode == 6){
		CombatScene *Player = dynamic_cast<CombatScene *>(Engine::GameEngine::GetInstance().GetScene("combat-scene"));
		Player->playerChar_combat = this->playerChar;
		Engine::GameEngine::GetInstance().ChangeScene("combat-scene");
	}
}

void GameSceneHall::OnKeyUp(int keyCode) {
	if (playerChar != nullptr) playerChar->SetMovementState(keyCode, false);
}

int GameSceneHall::ClampMapPos(int pos, int max){
	if (pos >= max) {
		return max;
	} else return pos;
}
void GameSceneHall::ConstructGenerativeGrassTile(int locX, int locY){
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return;
	int tileRandom = rand() % 10; // 10 % Grasses, 10% Flower, 60% Default Grass Tile
	string grassTilePath = "play/grass_1.png";
	switch (tileRandom) {
		case 0:  // * Grasses
			grassTilePath = "play/grass_2.png";
		break;
		case 1: // * Flowers
			grassTilePath = "play/grass_3.png";
		break;
		default:
			grassTilePath = "play/grass_1.png";
		break;
	}

	TileMapGroup->AddNewObject(new Engine::Image(grassTilePath, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
}
void GameSceneHall::ConstructGenerativePathTile(int locX, int locY){

	// cout << "Curr Loc Y : " << locY << " Loc X : " << locX << " + Y : " << ClampMapPos(locY + 1, MapHeight - 1) << " : " << (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR ? " FLOOR " : " DIRT ") << endl;
	if (locY == MapHeight - 1 && mapState[locY][locX] == TILE_DIRT){
		int topBottomTileRand = rand() % 2;
		if (topBottomTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image("play/dirtTopBottom_1.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image("play/dirtTopBottom_2.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	else if (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR && mapState[locY][locX] != TILE_CORNERBTMLEFT && mapState[locY][locX] != TILE_CORNERBTMRIGHT && mapState[locY][locX] != TILE_CORNERTOPLEFT && mapState[locY][locX] != TILE_CORNERTOPRIGHT) { // * Bottom is a floor, creating different tiles
		// cout << "Bottom is a floor Loc Y : " << locY << " Loc X : " << locX;
		int topBottomTileRand = rand() % 2;
		if (topBottomTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image("play/dirtTopBottom_1.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image("play/dirtTopBottom_2.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	} else if ((mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_DIRT || mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_CORNERBTMLEFT || mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_CORNERBTMRIGHT)  && mapState[locY][locX] != TILE_CORNERBTMLEFT && mapState[locY][locX] != TILE_CORNERBTMRIGHT && mapState[locY][locX] != TILE_CORNERTOPLEFT && mapState[locY][locX] != TILE_CORNERTOPRIGHT){

		int leftRightTileRand = rand() % 2;
		if (leftRightTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image("play/dirtLeftRight_1.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image("play/dirtLeftRight_2.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	} else {
		string imgPathFile = "play/dirt.png";
		switch(mapState[locY][locX]){
			case TILE_CORNERTOPRIGHT:
				imgPathFile = "play/dirtCorner_TopRight.png";
			break;
			case TILE_CORNERBTMRIGHT:
				imgPathFile = "play/dirtCorner_BottomRight.png";
			break;
			case TILE_CORNERTOPLEFT:
				imgPathFile = "play/dirtCorner_TopLeft.png";
			break;
			case TILE_CORNERBTMLEFT:
				imgPathFile = "play/dirtCorner_BottomLeft.png";
			break;
			case TILE_MAR:
				imgPathFile = "play/newgrass.png";
			break;
			default:
				imgPathFile = "play/dirt.png";
			break;
		}

		TileMapGroup->AddNewObject(new Engine::Image(imgPathFile, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	
}

void GameSceneHall::ConstructBlock(int locX, int locY, BlockType block) {
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return;
	string blockImgPath = "play/Base_blocks.png";
	switch (block){
		case BLANK: blockImgPath = ""; break;
		case BASE_BLOCK: blockImgPath = "play/Base_blocks.png"; break;
		case BLOCK_CHEST: blockImgPath = "play/chest_closed.png"; break;
	}
	if (blockImgPath.empty()) return;
	BlockGroup->AddNewObject(new Engine::Image(blockImgPath, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	
}

void GameSceneHall::ConstructItem(int locX, int locY, ItemType item){
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return;
	string itemImgPath = "play/Base_blocks.png";
	switch (item){
		case ITEM_BLANK: itemImgPath = ""; break;
		case ITEM_POTION: itemImgPath = "play/potion.png"; break;
		case ITEM_MISSILE: itemImgPath = "play/missile.png"; break;
		case ITEM_SHIELD: itemImgPath = "play/shield.png"; break;
	}

	cout << itemImgPath << endl;
	if (itemImgPath.empty()) return;
	ItemGroup->AddNewObject(new Engine::Image(itemImgPath, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
}

void GameSceneHall::ReadMap() {
	std::string filename = std::string("Resource/map/") + Engine::GameEngine::currentActiveScene + "_path.txt";
	// Read map file.
    cout << filename << endl;
	char c;
	std::vector<int> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(TILE_DIRT); break;
		case '1': mapData.push_back(TILE_FLOOR); break;
		case '3': mapData.push_back(TILE_CORNERTOPRIGHT); break; //3 - 6 Means Path corner. inserting (path = false)
		case '4': mapData.push_back(TILE_CORNERBTMRIGHT); break; //3 - 6 Means Path corner. inserting (path = false)
		case '5': mapData.push_back(TILE_CORNERTOPLEFT); break; //3 - 6 Means Path corner. inserting (path = false)
		case '6': mapData.push_back(TILE_CORNERBTMLEFT); break; //3 - 6 Means Path corner. inserting (path = false)
		case '9': mapData.push_back(TILE_MAR); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted. 1 ");
			break;
		default: throw std::ios_base::failure("Map data is corrupted: unrecognized tile. 2 ");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted. 3 ");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			// mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			switch(num){
				case 0: mapState[i][j] = TILE_DIRT; break;
				case 1: mapState[i][j] = TILE_FLOOR; break;
				case 2: mapState[i][j] = TILE_OCCUPIED; break;
				case 3: mapState[i][j] = TILE_CORNERTOPRIGHT; break;
				case 4: mapState[i][j] = TILE_CORNERBTMRIGHT; break;
				case 5: mapState[i][j] = TILE_CORNERTOPLEFT; break;
				case 6: mapState[i][j] = TILE_CORNERBTMLEFT; break;
				case 8: mapState[i][j] = TILE_BLOCK; break;
				case 9: mapState[i][j] = TILE_MAR; break;
			}
		}
	}
	
	// reset dep
	mapData.clear();
	
	// read the blocks
	// try to read blocks item first
    filename = "resource/map/" + Engine::GameEngine::currentActivePlayerName + "_" + Engine::GameEngine::currentActiveScene + "_ItemData.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << " for reading." << std::endl;
		filename = std::string("Resource/map/") + Engine::GameEngine::currentActiveScene + "_blocks.txt";
		cout << "LOADED SCENE ITEM BLOCK FROM SCENE DEFAULT " << filename << endl;
    } else {
		cout << "LOADED SCENE ITEM BLOCK DATA FROM PLAYER " << filename << endl;
	}

	fin.open(filename, std::ifstream::in);
	while(fin >> c) {
		switch(c) {
			case '0': mapData.push_back(0); break; // Blank
			case '1': mapData.push_back(1); break; // Wall
			case '2': mapData.push_back(2); break; // Chest
			case '3': mapData.push_back(3); break; // Potion
			case '4': mapData.push_back(4); break; // Missile
			case '5': mapData.push_back(5); break; // Shield
			case '\n':
			case '\r':
				if (static_cast<int>(mapData.size()) / MapWidth != 0)
					throw std::ios_base::failure("Map block data is corrupted. 1 ");
				break;
			default: throw std::ios_base::failure("Map block data is corrupted: unknown block type");
		}
	}
	fin.close();

	// Validate block data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map block data is corrupted: width and height does not match");

	// Store blocks in 2d array.
	mapBlocks = std::vector<std::vector<BlockType>>(MapHeight, std::vector<BlockType>(MapWidth));
	mapItems = std::vector<std::vector<ItemType>>(MapHeight, std::vector<ItemType>(MapWidth));

	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			switch(num){
				case 0: mapBlocks[i][j] = BLANK; break;
				case 1: mapBlocks[i][j] = BASE_BLOCK; break;
				case 2: mapBlocks[i][j] = BLOCK_CHEST; break;
				case 3: mapItems[i][j] = ITEM_POTION; break;
				case 4: mapItems[i][j] = ITEM_MISSILE; break;
				case 5: mapItems[i][j] = ITEM_SHIELD; break;
			}
		}
	}

	// * construct Map Blocks
	for (int i = 0; i < MapHeight; i++){
		for (int j = 0; j < MapWidth; j++){
			if (mapState[i][j] == TILE_FLOOR)ConstructGenerativeGrassTile(j , i);
			else ConstructGenerativePathTile(j , i);
			if(mapBlocks[i][j]) ConstructBlock(j, i, mapBlocks[i][j]);
			if(mapItems[i][j]) ConstructItem(j, i, mapItems[i][j]);
		}
	}
}

void GameSceneHall::ConstructUI() {
	
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	
}

void GameSceneHall::DestroyCurrentActiveDialog(IControl * currActiveDialog){
	if (currActiveDialog == nullptr) return;
	RemoveControl(currActiveDialog->controlIterator);
	currActiveDialog = nullptr;
}

void GameSceneHall::ToogleGamePaused(bool newState){
	if (isGameOver == true || !playerChar) return;
	this->isGamePaused = newState;
	std::cout << "Paused Function Called!\n";
	if (newState){
		AddNewObject(IMG_PauseMenuBG = new Engine::Image("bg/PauseMenu_bg.png", 0, 0, 1600, 832));
		AddNewControlObject(BTNPause_Resume = new Engine::ImageButton("btn/btn_resumegame_normal.png", "btn/btn_resumegame_hover.png", 681, 234, 238, 57));
		AddNewControlObject(BTNPause_LoadCP = new Engine::ImageButton("btn/btn_loadcheckpoint_normal.png", "btn/btn_loadcheckpoint_hover.png", 681, 311, 238, 57));
		AddNewControlObject(BTNPause_BackMenu = new Engine::ImageButton("btn/btn_mainmenu_normal.png", "btn/btn_mainmenu_hover.png", 681, 388, 238, 57));

		BTNPause_Resume->SetOnClickCallback(std::bind(&GameSceneHall::OnClickBTNResume, this));
		BTNPause_LoadCP->SetOnClickCallback(std::bind(&GameSceneHall::OnClickBTNLoadCheckpoint, this));
		BTNPause_BackMenu->SetOnClickCallback(std::bind(&GameSceneHall::OnClickBTNBackMenu, this));
	} else {
		if (IMG_PauseMenuBG) RemoveObject(IMG_PauseMenuBG->GetObjectIterator());
		if (BTNPause_LoadCP) RemoveObject(BTNPause_LoadCP->GetObjectIterator());
		if (BTNPause_BackMenu) RemoveObject(BTNPause_BackMenu->GetObjectIterator());
		if (BTNPause_Resume) RemoveObject(BTNPause_Resume->GetObjectIterator());
		IMG_PauseMenuBG = nullptr; BTNPause_BackMenu = nullptr; BTNPause_LoadCP = nullptr; BTNPause_Resume = nullptr;
	}
	
}

void GameSceneHall::OnClickBTNResume(){
	ToogleGamePaused(false);
}
void GameSceneHall::OnClickBTNBackMenu(){
	Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void GameSceneHall::OnClickBTNLoadCheckpoint(){

}

