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

#include "Scene/ForestSceneUp.hpp"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "Scene/Combat/CombatScene.hpp"
#include "Scene/Loading/LoadingScene.hpp"
#include "Scene/GameScene_Hall.hpp"
using namespace std;


// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4
#define KEYBOARD_ESC 59
#define KEYBOARD_F 6


const std::vector<Engine::Point> ForestScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int ForestScene::MapWidth = 25, ForestScene::MapHeight = 13;
const int ForestScene::BlockSize = 64;

bool forest_mapAllInitialized = false;

Engine::Point ForestScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void ForestScene::Initialize() {
	Engine::GameEngine::currentActiveScene = "forestscene_up";
	mapState.clear();
    mapItems.clear();
	mapBlocks.clear();
	keyStrokes.clear();
	ticks = 0;
	SpeedMult = 1;
	isGamePaused = false;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// * Group Initialization

	AddNewObject(TileMapGroup = new Group());
	AddNewObject(new Engine::Image("maps/forestsceneup_overlay.png", 0, 0, 1600, 832));
	AddNewObject(ItemGroup = new Group());
	AddNewObject(BlockGroup = new Group());


	AddNewControlObject(UIGroup = new Group());
	AddNewControlObject(CharacterSpriteGroup = new Group());

	ReadMap();
	ConstructUI();


	playerEntryData = Engine::GameEngine::GetInstance().GetCurrentActivePlayer();
	Engine::Point spawnPoint = Engine::GameEngine::GetInstance().GridToXYPosition(10, 6, BlockSize);
	if (playerEntryData.x != -1 && playerEntryData.y != -1 && playerEntryData.lastScene == Engine::GameEngine::currentActiveScene) {
		spawnPoint.y = playerEntryData.y; spawnPoint.x = playerEntryData.x;
	}

    if (lastScene == "gamescene_hall"){
        spawnPoint.y = 11; spawnPoint.x = 20;
    }

	if (playerChar == nullptr){
		playerChar = new PlayerCharacter(spawnPoint.x, spawnPoint.y , 3.0, 100, 50, BlockSize, Engine::GameEngine::currentActiveScene, playerEntryData);
	} else {
		playerChar->x = spawnPoint.x * BlockSize;
		playerChar->y = spawnPoint.y * BlockSize;
        playerChar->UpdateSprite();
	}

    if (playerChar) playerChar->canMove = true;
	forest_mapAllInitialized = true;
}


void ForestScene::Terminate() {
	IScene::Terminate();

}
void ForestScene::Update(float deltaTime) {
    if (playerChar != nullptr) {
		playerChar->Update(deltaTime);
		if (!forest_mapAllInitialized) return;
		Engine::Point playerPos = playerChar->GetPlayerPositionAtMap();


		// * Back into gamescene hall
		if (playerPos.y == 11 && playerPos.x == 20 && playerChar->directionFacing == DIRECTION_DOWN){
			// playerChar->canMove = false;
            playerChar->ResetMovementInput();
			playerChar->CheckPointSave(mapItems, mapBlocks);
			GameSceneHall * scene = dynamic_cast<GameSceneHall*>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
			scene->playerChar = this->playerChar;
			scene->lastScene = "forestscene_up";
			Engine::GameEngine::GetInstance().ChangeScene("gamescene_hall");
		}

        if (!mapItems.empty()) {
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


		// * Detect if Player is near NPC or Chest
		if (!npcList.empty()){
            for (auto & npc : npcList){
                if ((playerPos.y >= npc.y - 1 && playerPos.y <= npc.y + 1) && (playerPos.x >= npc.x - 1 && playerPos.x <= npc.x + 1)){
                    playerChar->canInteract = true;
                    playerChar->objToInteract_PosX = npc.x;
                    playerChar->objToInteract_PosY = npc.y;
                } else {
                    playerChar->canInteract = false;
                    playerChar->objToInteract_PosX = -1;
                    playerChar->objToInteract_PosY = -1;
                }
            }
        }

		if (playerChar->canInteract) return;

		if (chestList.empty()) return;
		bool foundChest = false;
		for (auto & chest : chestList){
			if (playerPos.y == chest.y + 1 && playerPos.x == chest.x && playerChar->directionFacing == DIRECTION_UP && mapBlocks[chest.y][chest.x] == BLOCK_CHEST){
				playerChar->canInteract = true;
				playerChar->objToInteract_PosX = chest.x;
				playerChar->objToInteract_PosY = chest.y;
				foundChest = true;
			} 
		}
		if (foundChest == false) {
			playerChar->canInteract = false;
			playerChar->objToInteract_PosX = -1;
			playerChar->objToInteract_PosY = -1;
		}
	}
}
void ForestScene::Draw() const {
	IScene::Draw();
    if (playerChar != nullptr && (!activeDialog || activeDialog->Enabled == false) && !isGamePaused){
		playerChar->Draw();
	} 
}
void ForestScene::OnMouseDown(int button, int mx, int my) {
	IScene::OnMouseDown(button, mx, my);
}
void ForestScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
}

void ForestScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
}
void ForestScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (playerChar != nullptr && playerChar->canMove) playerChar->SetMovementState(keyCode, true);

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

	if (keyCode == 3){
		CombatScene *Player = dynamic_cast<CombatScene *>(Engine::GameEngine::GetInstance().GetScene("combat-scene"));
		Player->playerChar_combat = this->playerChar;
		Engine::GameEngine::GetInstance().ChangeScene("combat-scene");
	}

	if (keyCode == KEYBOARD_F){
		// * Interaction
		if (playerChar->canInteract == true && activeDialog == nullptr) {
			cout << "Interacted!\n";
			if (playerChar->objToInteract_PosY == npcList[0].y && playerChar->objToInteract_PosX == npcList[0].x){
				AddNewControlObject(activeDialog = new Engine::DialogScreen("Ah, " + Engine::GameEngine::currentActivePlayerName +", just the person I was hoping to see. Welcome to your new home, \nmy child...", "Old Man", 2.0f, playerChar, 1));
				activeDialog->SetOnClickCallback(bind(&ForestScene::OnDialogDone, this, activeDialog));
			}

			else if (playerChar->objToInteract_PosY == chestList[0].y && playerChar->objToInteract_PosX == chestList[0].x){
				mapBlocks[chestList[0].y][chestList[0].x] = CHEST_OPENED;
				cout << "Chest opened up\n";
				for (auto & block : BlockGroup->GetObjects()){
					if (block->Position.y == chestList[0].y * BlockSize && block->Position.x == chestList[0].x * BlockSize){

						BlockGroup->RemoveObject(block->GetObjectIterator());
						ConstructBlock(chestList[0].x, chestList[0].y, CHEST_OPENED);
						break;
					}
				}
				if (playerChar->ChestObtainBG) delete playerChar->ChestObtainBG;
				playerChar->ChestObtainBG = new Engine::Image("bg/chestobtain_01.png", 1, 0, 1600, 832);
				playerChar->chestBGdelay = 1.5f;

				playerChar->healthPotion++;
				playerChar->shield++;
				playerChar->AddEXP(40);
			} 
		}
	}
}

void ForestScene::OnKeyUp(int keyCode) {
	if (playerChar != nullptr) playerChar->SetMovementState(keyCode, false);
}

int ForestScene::ClampMapPos(int pos, int max){
	if (pos >= max) {
		return max;
	} else return pos;
}
void ForestScene::ConstructGenerativeGrassTile(int locX, int locY){
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
void ForestScene::ConstructGenerativePathTile(int locX, int locY){

	// cout << "Curr Loc Y : " << locY << " Loc X : " << locX << " + Y : " << ClampMapPos(locY + 1, MapHeight - 1) << " : " << (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR ? " FLOOR " : " DIRT ") << endl;
	if (locY == MapHeight - 1 && mapState[locY][locX] == TILE_DIRT){
		int topBottomTileRand = rand() % 2;
		if (topBottomTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image("play/dirtLeftRight_1.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image("play/dirtLeftRight_2.png", locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	else if (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR && mapState[locY][locX] != TILE_CORNERBTMLEFT && mapState[locY][locX] != TILE_CORNERBTMRIGHT && mapState[locY][locX] != TILE_CORNERTOPLEFT && mapState[locY][locX] != TILE_CORNERTOPRIGHT) { // * Bottom is a floor, creating different tiles
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

void ForestScene::ConstructBlock(int locX, int locY, BlockType block) {
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return;
	string blockImgPath = "play/Base_blocks.png";
	switch (block){
		case BLANK: blockImgPath = ""; break;
		case BASE_BLOCK: blockImgPath = "play/block_transparant.png"; break;
		case BLOCK_CHEST: blockImgPath = "play/chest_closed.png"; break;
		case CHEST_OPENED: blockImgPath = "play/chest_opened.png"; break;
		case NPC_INSPECTOR: blockImgPath = "char/npc/npc_idle_1.png"; break;
	}
	if (blockImgPath.empty()) return;
	BlockGroup->AddNewObject(new Engine::Image(blockImgPath, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	
}

void ForestScene::ConstructItem(int locX, int locY, ItemType item){
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

void ForestScene::ReadMap() {
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
	// if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
	// 	throw std::ios_base::failure("Map data is corrupted. 3 ");
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
			case '6' : mapData.push_back(6); break; // Chest (Opened)
			case '7' : mapData.push_back(7); break; //NPC_Inspector
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
				case 0: 
					mapBlocks[i][j] = BLANK; 
					break;
				case 1: 
					mapBlocks[i][j] = BASE_BLOCK; 
					break;
				case 2: {
					mapBlocks[i][j] = BLOCK_CHEST; 
					Engine::Point pChest; 
					pChest.x = j; 
					pChest.y = i;
					chestList.push_back(pChest);
					break;
				}
				case 3: 
					mapItems[i][j] = ITEM_POTION; 
					break;
				case 4: 
					mapItems[i][j] = ITEM_MISSILE; 
					break;
				case 5: 
					mapItems[i][j] = ITEM_SHIELD; 
					break;
				case 6: 
					mapBlocks[i][j] = CHEST_OPENED; 
					break;
				case 7: {
					mapBlocks[i][j] = NPC_INSPECTOR; 
					Engine::Point p; 
					p.x = j; 
					p.y = i;
					npcList.push_back(p);
					break;
				}
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

void ForestScene::ConstructUI() {
	
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;
	
}

void ForestScene::OnDialogDone(IControl * currActiveDialog){
	if (currActiveDialog == nullptr) return;
	RemoveControl(currActiveDialog->controlIterator);

	if (activeDialog->dialogID == 1){
		AddNewControlObject(activeDialog = new Engine::DialogScreen("Dark times are upon us. The Shadow King has awoken in the depths of the Forgotten Grove, \nand his darkness spreads through our beloved forest. The prophecy foretold of a hero \nwho would rise to challenge him. I believe that hero is you.", "Old Man", 3.0f, playerChar, 2));
		activeDialog->SetOnClickCallback(bind(&ForestScene::OnDialogDone, this, activeDialog));
	} else if (activeDialog->dialogID == 2){
		AddNewControlObject(activeDialog = new Engine::DialogScreen("Me? But what can I do against such evil?", Engine::GameEngine::currentActivePlayerName, 1.0f, playerChar, 3));
		activeDialog->SetOnClickCallback(bind(&ForestScene::OnDialogDone, this, activeDialog));
	} else if (activeDialog->dialogID == 3){
		AddNewControlObject(activeDialog = new Engine::DialogScreen("You have a brave heart and a strong spirit, " + Engine::GameEngine::currentActivePlayerName + ". The journey ahead will be \nfraught with danger, but you must find the courage within yourself. \nYour first task is to gather the resources hidden within the forest. These resources hold \nthe power to weaken the monsters", "Old Man", 3.0f, playerChar, 4));
		activeDialog->SetOnClickCallback(bind(&ForestScene::OnDialogDone, this, activeDialog));		
	} else if (activeDialog->dialogID == 4){
		AddNewControlObject(activeDialog = new Engine::DialogScreen("I understand, Old Man. I won’t let you down.", Engine::GameEngine::currentActivePlayerName, 1.0f, playerChar, 5));
		activeDialog->SetOnClickCallback(bind(&ForestScene::OnDialogDone, this, activeDialog));
	}
	
	
	
	
	else activeDialog = nullptr;
}

void ForestScene::ToogleGamePaused(bool newState){
	if (isGameOver == true || !playerChar) return;
	this->isGamePaused = newState;
	std::cout << "Paused Function Called!\n";
	if (newState){
		AddNewObject(IMG_PauseMenuBG = new Engine::Image("bg/PauseMenu_bg.png", 0, 0, 1600, 832));
		AddNewControlObject(BTNPause_Resume = new Engine::ImageButton("btn/btn_resumegame_normal.png", "btn/btn_resumegame_hover.png", 681, 234, 238, 57));
		AddNewControlObject(BTNPause_LoadCP = new Engine::ImageButton("btn/btn_loadcheckpoint_normal.png", "btn/btn_loadcheckpoint_hover.png", 681, 311, 238, 57));
		AddNewControlObject(BTNPause_BackMenu = new Engine::ImageButton("btn/btn_mainmenu_normal.png", "btn/btn_mainmenu_hover.png", 681, 388, 238, 57));

		BTNPause_Resume->SetOnClickCallback(std::bind(&ForestScene::OnClickBTNResume, this));
		BTNPause_LoadCP->SetOnClickCallback(std::bind(&ForestScene::OnClickBTNLoadCheckpoint, this));
		BTNPause_BackMenu->SetOnClickCallback(std::bind(&ForestScene::OnClickBTNBackMenu, this));
	} else {
		if (IMG_PauseMenuBG) RemoveObject(IMG_PauseMenuBG->GetObjectIterator());
		if (BTNPause_LoadCP) RemoveObject(BTNPause_LoadCP->GetObjectIterator());
		if (BTNPause_BackMenu) RemoveObject(BTNPause_BackMenu->GetObjectIterator());
		if (BTNPause_Resume) RemoveObject(BTNPause_Resume->GetObjectIterator());
		IMG_PauseMenuBG = nullptr; BTNPause_BackMenu = nullptr; BTNPause_LoadCP = nullptr; BTNPause_Resume = nullptr;
	}
	
}

void ForestScene::OnClickBTNResume(){
	ToogleGamePaused(false);
}
void ForestScene::OnClickBTNBackMenu(){
    if (playerChar) {
        delete playerChar;
        playerChar = nullptr;
    }
	Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}
void ForestScene::OnClickBTNLoadCheckpoint(){

}

