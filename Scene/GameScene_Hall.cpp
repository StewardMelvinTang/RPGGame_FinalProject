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

#include "GameScene_Hall.hpp"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
using namespace std;


// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4

bool keys[4] = {false, false, false, false}; // W, S, A, D (input holding status)

bool GameSceneHall::DebugMode = false;
const std::vector<Engine::Point> GameSceneHall::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int GameSceneHall::MapWidth = 25, GameSceneHall::MapHeight = 13;
const int GameSceneHall::BlockSize = 64;
Engine::Point GameSceneHall::SpawnGridPoint = Engine::Point(-1, 1);
const Engine::Point GameSceneHall::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);

Engine::Point GameSceneHall::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void GameSceneHall::Initialize() {
    
    currentMapID = "hall";
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	SpeedMult = 1;
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // * Initialize Srand
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	// AddNewObject(GroundEffectGroup = new Group());
	// AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	AddNewControlObject(CharacterSpriteGroup = new Group());

	ReadMap();
	ConstructUI();
    InitializeCharacter();
	// imgTarget = new Engine::Image("play/target.png", 0, 0);
	// imgTarget->Visible = false;
	// UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene
	// deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	// Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.

    cout << "MAp Initialized\n";
	bgmId = AudioHelper::PlayBGM("play.ogg");
    cout << "Audio Played\n";

}

void GameSceneHall::InitializeCharacter(){
    charSpriteObj = new Engine::Image("char/char_idle_down.png", charSpriteData.x, charSpriteData.y, charSpriteData.size, charSpriteData.size);
}

void GameSceneHall::DrawCharacter() const{
    if (!charSpriteObj) return;
    charSpriteObj->Draw();
}

bool GameSceneHall::CollisionCheck(Character &character){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    if (character.x < 0 || character.x > w || character.y < 0 || character.y > h) {
        if (character.x <= 0) {
            character.x = 0;
        }
        if (character.y <= 0) {
            character.y = 0;
        }

        if (character.x >= w){
            character.x = w;
        }

        if (character.y >= h){
            character.y = h;
        }

        return false;
    }

    return true;
}

void GameSceneHall::UpdateCharacterDirection(Character &character, bool keys[4]){
    if (keys[0] && CollisionCheck(character)) { // W key
        character.y -= character.speed;
        character.directionFacing = DIRECTION_UP;
        //character.rotation = 0;
    }
    if (keys[1]&& CollisionCheck(character)) { // S key
        character.y += character.speed;
        character.directionFacing = DIRECTION_DOWN;
        //character.rotation = ALLEGRO_PI; // 180 degrees
    }
    if (keys[2]&& CollisionCheck(character)) { // A key
        character.x -= character.speed;
        character.directionFacing = DIRECTION_LEFT;
        //character.rotation = -ALLEGRO_PI / 2; // -90 degrees
    }
    if (keys[3]&& CollisionCheck(character)) { // D key
        character.x += character.speed;
        character.directionFacing = DIRECTION_RIGHT;
        //character.rotation = ALLEGRO_PI / 2; // 90 degrees
    }

    // * Update Sprite Based on DIRECTION ENUM
    string charSpritePath = "char/char_idle_down.png";
    switch (character.directionFacing){
        case DIRECTION_DOWN:
            charSpritePath = "char/char_idle_down.png";
        break;

        case DIRECTION_UP:
            charSpritePath = "char/char_idle_up.png";
        break;

        case DIRECTION_RIGHT:
            charSpritePath = "char/char_idle_right.png";
        break;

        case DIRECTION_LEFT:
            charSpritePath = "char/char_idle_left.png";
        break;
    }
    charSpriteObj = new Engine::Image(charSpritePath, character.x, character.y, character.size, character.size);
}

void GameSceneHall::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void GameSceneHall::Update(float deltaTime) {
    UpdateCharacterDirection(charSpriteData, keys);
}
void GameSceneHall::Draw() const {
	IScene::Draw();
    DrawCharacter();
    
}
void GameSceneHall::OnMouseDown(int button, int mx, int my) {
	IScene::OnMouseDown(button, mx, my);
}
void GameSceneHall::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	// imgTarget->Visible = true;
	// imgTarget->Position.x = x * BlockSize;
	// imgTarget->Position.y = y * BlockSize;
}
void GameSceneHall::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
}
void GameSceneHall::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);

    switch(keyCode){
        case KEYBOARD_W:
            keys[0] = true;
        break;
        case KEYBOARD_S:
            keys[1] = true;
        break;
        case KEYBOARD_A:
            keys[2] = true;
        break;
        case KEYBOARD_D:
            keys[3] = true;
        break;
    }

}

void GameSceneHall::OnKeyUp(int keyCode) {
    switch(keyCode){
        case KEYBOARD_W:
            keys[0] = false;
        break;
        case KEYBOARD_S:
            keys[1] = false;
        break;
        case KEYBOARD_A:
            keys[2] = false;
        break;
        case KEYBOARD_D:
            keys[3] = false;
        break;
    }
}

int GameSceneHall::ClampMapPos(int pos, int max){
	if (pos >= max) {
		return max;
	} else return pos;
}
void GameSceneHall::ConstructGenerativeGrassTile(int locX, int locY){
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return; // ! Invalid
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
			default:
				imgPathFile = "play/dirt.png";
			break;
		}

		TileMapGroup->AddNewObject(new Engine::Image(imgPathFile, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	
}

void GameSceneHall::ReadMap() {
	std::string filename = std::string("Resource/map") + currentMapID + ".txt";
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
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted. 1 ");
			break;
		default: throw std::ios_base::failure("Map data is corrupted. 2 ");
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
			}
		}
	}


	for (int i = 0; i < MapHeight; i++){
		for (int j = 0; j < MapWidth; j++){
			if (mapState[i][j] == TILE_FLOOR){
				// TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
				ConstructGenerativeGrassTile(j , i);
			} else {
				ConstructGenerativePathTile(j , i);
			}
		}
	}
}

void GameSceneHall::ConstructUI() {
	
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;

	// Background
	//UIGroup->AddNewObject(new Engine::Image((MapId == 3 ? "bg/PlayerHUDSTG3_Game.png" : "bg/PlayerHUD_Game.png"), 0, 0, 1600, 832));
	// UIGroup->AddNewObject(new Engine::I)
	// Text
	// UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pixel-font.ttf", 48, 1306, 25, 255, 226, 182));
	// UIGroup->AddNewObject(UIMoney = new Engine::Label(std::to_string(money), "pixel-font.ttf", 35, 1359, 85, 255, 224, 78));
	// UIGroup->AddNewObject(UILives = new Engine::Label(std::to_string(lives), "pixel-font.ttf", 35, 1359, 138, 255, 255, 255));


	
}

