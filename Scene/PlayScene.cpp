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
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/GlueGunner.hpp"
#include "UI/Animation/Plane.hpp"
#include "Enemy/PlaneEnemy.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/TankEnemy.hpp"
#include "Enemy/RobocopEnemy.hpp"
#include "Enemy/WarriorEnemy.hpp"
#include "Turret/TurretButton.hpp"
#include "WinScene.hpp"
#include "LoseScene.hpp"
using namespace std;

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
 Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 1);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
									ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	// TODO: [HACKATHON-3-BUG] (1/5): There's a bug in this file, which crashes the game when you lose. Try to find it.
	// TODO: [HACKATHON-3-BUG] (2/5): Find out the cheat code to test.
    // TODO: [HACKATHON-3-BUG] (2/5): It should generate a Plane, and add 10000 to the money, but it doesn't work now.
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = (MapId == 3 ? 1 : 10);
	money = (MapId == 1 ? 150 : MapId == 2 ? 215 : MapId == 3 ? 250 : 150);
	SpeedMult = 1;
	score = 0;
	towerBuilt = 0;
	enemyKilled = 0;

	std::srand(static_cast<unsigned int>(std::time(nullptr))); // * Initialize Srand
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(TowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();
	mapDistance = CalculateBFSDistance();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : EnemyGroup->GetObjects()) {
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					if (SpeedMult != 0)
						deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	if (SpeedMult == 0)
		AudioHelper::StopSample(deathBGMInstance);
	if (deathCountDown == -1 && lives > 0) {
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		// cout << " ENEMY WAVE DATA SIZE LENGTH : " << enemyWaveData.size() << endl;
		if (enemyWaveData.empty()) {
			if (EnemyGroup->GetObjects().empty()) {
				// Free resources.
				/*delete TileMapGroup;
				delete GroundEffectGroup;
				delete DebugIndicatorGroup;
				delete TowerGroup;
				delete EnemyGroup;
				delete BulletGroup;
				delete EffectGroup;
				delete UIGroup;
				delete imgTarget;*/
				Engine::GameEngine::GetInstance().ChangeScene("win");
				WinScene *scene = dynamic_cast<WinScene *>(Engine::GameEngine::GetInstance().GetScene("win"));
				if (scene != nullptr) {
					scene->score = this->score;
					scene->enemiesKilled = this->enemyKilled;
					scene->towersBuilt = this->towerBuilt;
					scene->stage = this->MapId;
				}
			}
			continue;
		}
		auto current = enemyWaveData.front();
		if (ticks < current.second)
			continue;
		ticks -= current.second;
		
		enemyWaveData.pop_front();
		const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
		Enemy* enemy;
		switch (current.first) {
		case 1:
			EnemyGroup->AddNewObject(enemy = new SoldierEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 2:
			EnemyGroup->AddNewObject(enemy = new PlaneEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 3:
			EnemyGroup->AddNewObject(enemy = new TankEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 4: // * ROBOCOP
			EnemyGroup->AddNewObject(enemy = new RobocopEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;
		case 5: // * WARRIOR
			EnemyGroup->AddNewObject(enemy = new WarriorEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
			break;			
		default:
			continue;
		}
		enemy->UpdatePath(mapDistance);
		// Compensate the time lost.
		enemy->Update(ticks);
	}
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}
void PlayScene::Draw() const {
	IScene::Draw();
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapDistance[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button & 1) {
		if (mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			if (!CheckSpaceValid(x, y)) {
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}
			// Purchase.
			EarnMoney(-preview->GetPrice());
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			UIGroup->RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
			TowerGroup->AddNewObject(preview);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			towerBuilt ++;
			AddScore(100);

			mapState[y][x] = TILE_OCCUPIED;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);

	if (keyCode == ALLEGRO_KEY_TAB) {
		
		DebugMode = !DebugMode;
	}
	else {
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_RIGHT && keyStrokes.size() == code.size()) {
			auto it = keyStrokes.begin();
			for (int c : code) {
				if (!((*it == c) ||
					(c == ALLEGRO_KEYMOD_SHIFT &&
					(*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			EffectGroup->AddNewObject(new Plane());
			EarnMoney(10000);
			AddScore(5000);
		}
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for MachineGunTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for LaserTurret.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E) {
		// Hotkey for MissileTurret.
		UIBtnClicked(2);
	} else if (keyCode == ALLEGRO_KEY_R) {
		UIBtnClicked(3); // * Glue Gunner Turret Hotkey
	}

	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}
void PlayScene::Hit() {
	lives--;
	AddScore(-150);
	UILives->Text = std::to_string(lives);
	if (lives <= 0) {
		// Engine::GameEngine::GetInstance().ChangeScene("lose");

		Engine::GameEngine::GetInstance().ChangeScene("lose");
		LoseScene *scene = dynamic_cast<LoseScene *>(Engine::GameEngine::GetInstance().GetScene("lose"));
		if (scene != nullptr) {
			scene->score = this->score;
			scene->stage = this->MapId;
			scene->kill = this->enemyKilled;
			scene->towerBuilt = this->towerBuilt;
		}
	}
}
int PlayScene::GetMoney() const {
	return money;
}
void PlayScene::EarnMoney(int money) {
	this->money += money;
	UIMoney->Text = std::to_string(this->money);
	AddScore(money * 2);
}

void PlayScene::AddScore(int val){
	if (val < 0){
		if (score - val >= 0){
			score -= val;
		} else {
			score = 0;
		}
	} else {
		score += val;
	}
}

int PlayScene::ClampMapPos(int pos, int max){
	if (pos >= max) {
		return max;
	} else return pos;
}


void PlayScene::ConstructGenerativeGrassTile(int locX, int locY){
	if (locX < 0 || locX >= MapWidth || locY < 0 || locY >= MapHeight) return; // ! Invalid
	int tileRandom = rand() % 10; // 10 % Grasses, 10% Flower, 60% Default Grass Tile
	string grassTilePath = (MapId == 3 ? "play/stage3/dirt_2.png" : "play/grass_1.png");
	switch (tileRandom) {
		case 0:  // * Grasses
			grassTilePath = (MapId == 3 ? "play/stage3/dirt_3.png" : "play/grass_2.png");
		break;
		case 1: // * Flowers
			grassTilePath = (MapId == 3 ? "play/stage3/dirt_1.png" : "play/grass_3.png");
		break;
		default:
			grassTilePath = (MapId == 3 ? "play/stage3/dirt_2.png" : "play/grass_1.png");
		break;
	}

	TileMapGroup->AddNewObject(new Engine::Image(grassTilePath, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
}

void PlayScene::ConstructGenerativePathTile(int locX, int locY){

	// cout << "Curr Loc Y : " << locY << " Loc X : " << locX << " + Y : " << ClampMapPos(locY + 1, MapHeight - 1) << " : " << (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR ? " FLOOR " : " DIRT ") << endl;
	if (locY == MapHeight - 1 && mapState[locY][locX] == TILE_DIRT){
		int topBottomTileRand = rand() % 2;
		if (topBottomTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_topdown_1.png" : "play/dirtTopBottom_1.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_topdown_1.png" : "play/dirtTopBottom_2.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	else if (mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_FLOOR && mapState[locY][locX] != TILE_CORNERBTMLEFT && mapState[locY][locX] != TILE_CORNERBTMRIGHT && mapState[locY][locX] != TILE_CORNERTOPLEFT && mapState[locY][locX] != TILE_CORNERTOPRIGHT) { // * Bottom is a floor, creating different tiles
		// cout << "Bottom is a floor Loc Y : " << locY << " Loc X : " << locX;
		int topBottomTileRand = rand() % 2;
		if (topBottomTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_topdown_1.png" : "play/dirtTopBottom_1.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_topdown_1.png" : "play/dirtTopBottom_2.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	} else if ((mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_DIRT || mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_CORNERBTMLEFT || mapState[ClampMapPos(locY + 1, MapHeight - 1)][locX] == TILE_CORNERBTMRIGHT)  && mapState[locY][locX] != TILE_CORNERBTMLEFT && mapState[locY][locX] != TILE_CORNERBTMRIGHT && mapState[locY][locX] != TILE_CORNERTOPLEFT && mapState[locY][locX] != TILE_CORNERTOPRIGHT){

		int leftRightTileRand = rand() % 2;
		if (leftRightTileRand == 0) TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_leftright.png" : "play/dirtLeftRight_1.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
		else TileMapGroup->AddNewObject(new Engine::Image((MapId == 3 ? "play/stage3/path_leftright2.png" : "play/dirtLeftRight_2.png"), locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	} else {
		string imgPathFile = (MapId == 3 ? "play/stage3/dirt_2.png" : "play/dirt.png");
		switch(mapState[locY][locX]){
			case TILE_CORNERTOPRIGHT:
				imgPathFile = (MapId == 3 ? "play/stage3/path_topright.png" : "play/dirtCorner_TopRight.png");
			break;
			case TILE_CORNERBTMRIGHT:
				imgPathFile = (MapId == 3 ? "play/stage3/path_bottomright.png" : "play/dirtCorner_BottomRight.png");
			break;
			case TILE_CORNERTOPLEFT:
				imgPathFile = (MapId == 3 ? "play/stage3/path_topleft.png" : "play/dirtCorner_TopLeft.png");
			break;
			case TILE_CORNERBTMLEFT:
				imgPathFile = (MapId == 3 ? "play/stage3/path_bottomleft.png" : "play/dirtCorner_BottomLeft.png");
			break;
			default:
				imgPathFile = (MapId == 3 ? "play/stage3/dirt_2.png" : "play/dirt.png");
			break;
		}

		TileMapGroup->AddNewObject(new Engine::Image(imgPathFile, locX * BlockSize, locY * BlockSize, BlockSize, BlockSize));
	}
	
}

void PlayScene::ReadMap() {
	std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
	// Read map file.
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
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
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
			// if (num)
			// 	TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			// else {
			// 	cout << (mapState[1][0] == TILE_FLOOR ? "Floor" : "Dirt") << endl;
			// 	// cout << "Generating Path" << endl;
			// 	ConstructGenerativePathTile(j , i);
			// 	// cout << "Path generated successfully\n";
			// 	// int dirtTextureRand = std::rand() % 3;
			// 	// switch (dirtTextureRand){
			// 	// 	case 0:
			// 	// 		TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			// 	// 	break;
			// 	// 	case 1:
			// 	// 		TileMapGroup->AddNewObject(new Engine::Image("play/dirt1.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			// 	// 	break;
			// 	// 	case 2:
			// 	// 		TileMapGroup->AddNewObject(new Engine::Image("play/dirt2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			// 	// 	break;

				
			// }
				
				// TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}

	bool findingSpawnPoint = true;

	for (int i = 0; i < MapHeight; i++){
		for (int j = 0; j < MapWidth; j++){
			if (mapState[i][j] == TILE_FLOOR){
				// TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
				ConstructGenerativeGrassTile(j , i);
			} else {
				if (findingSpawnPoint) {
					SpawnGridPoint = Engine::Point(-1, i);
					findingSpawnPoint = false;
				}
				ConstructGenerativePathTile(j , i);
			}
		}
	}
}
void PlayScene::ReadEnemyWave() {
    // TODO: [HACKATHON-3-BUG] (3/5): Trace the code to know how the enemies are created.
    // TODO: [HACKATHON-3-BUG] (3/5): There is a bug in these files, which let the game only spawn the first enemy, try to fix it.
    std::string filename = string("Resource/enemy") + to_string(MapId) + ".txt";
	// cout << "Reading Enemy Wave File name : " << filename << endl;
	// Read enemy file.
	float type, wait, repeat;
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> repeat) {
		for (int i = 0; i < repeat; i++){
			//if (type == 0) type = 1;
			enemyWaveData.emplace_back(type, wait);
		}

	}
	fin.close();
}
void PlayScene::ConstructUI() {
	
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int shift = 135 + 25;

	// Background
	UIGroup->AddNewObject(new Engine::Image((MapId == 3 ? "bg/PlayerHUDSTG3_Game.png" : "bg/PlayerHUD_Game.png"), 0, 0, 1600, 832));
	// UIGroup->AddNewObject(new Engine::I)
	// Text
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pixel-font.ttf", 48, 1306, 25, 255, 226, 182));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::to_string(money), "pixel-font.ttf", 35, 1359, 85, 255, 224, 78));
	UIGroup->AddNewObject(UILives = new Engine::Label(std::to_string(lives), "pixel-font.ttf", 35, 1359, 138, 255, 255, 255));


	TurretButton* btn; 
	// ? Button 1 : Machine Gun Turret
	btn = new TurretButton("play/tower_btn_normal.png", "play/tower_btn_hover.png",
		Engine::Sprite("play/tower-base.png", 1315, 193, 0, 0, 0, 0),
		Engine::Sprite("play/turret-1.png", 1315, 193 - 8, 0, 0, 0, 0)
		, 1310, 188, MachineGunTurret::Price);
	// Reference: Class Member Function Pointer and std::bind.
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 0));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label("$" + to_string(MachineGunTurret::Price), "pixel-font.ttf", 20, 1380, 235, 190, 190, 190, 255, 1.0f, 0.0f));

	// ? Button 2 : Laser Turret
	btn = new TurretButton("play/tower_btn_normal.png", "play/tower_btn_hover.png",
		Engine::Sprite("play/tower-base.png", 1405, 193, 0, 0, 0, 0),
		Engine::Sprite("play/turret-2.png", 1405, 193 - 8, 0, 0, 0, 0)
		, 1400, 188, LaserTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 1));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label("$" + to_string(LaserTurret::Price), "pixel-font.ttf", 20, 1470, 235, 190, 190, 190, 255, 1.0f, 0.0f));


	// ? Button 3 : Missile Turret
	btn = new TurretButton("play/tower_btn_normal.png", "play/tower_btn_hover.png",
		Engine::Sprite("play/tower-base.png", 1500, 193, 0, 0, 0, 0),
		Engine::Sprite("play/turret-3.png", 1500, 193, 0, 0, 0, 0)
		, 1494, 188, MissileTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 2));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label("$" + to_string(MissileTurret::Price), "pixel-font.ttf", 20, 1565, 235, 190, 190, 190, 255, 1.0f, 0.0f));

	// ? Button 4 : Glue Gunner
	btn = new TurretButton("play/tower_btn_normal.png", "play/tower_btn_hover.png",
		Engine::Sprite("play/tower-base-glue.png", 1315, 275, 0, 0, 0, 0),
		Engine::Sprite("play/turret-glue.png", 1315, 275 - 8, 0, 0, 0, 0)
		, 1310, 270, GlueGunnerTurret::Price);
	btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, 3));
	UIGroup->AddNewControlObject(btn);
	UIGroup->AddNewObject(new Engine::Label("$" + to_string(GlueGunnerTurret::Price), "pixel-font.ttf", 20, 1381, 317, 190, 190, 190, 255, 1.0f, 0.0f));
	
	Engine::ImageButton * ffButton;
    ffButton = new Engine::ImageButton("btn/fastForward_normal.png", "btn/fastForward_hover.png", 1303, 746, 70, 70);
    ffButton->SetOnClickCallback(std::bind(&PlayScene::FastForwardButtonPress, this));
    UIGroup->AddNewControlObject(ffButton);

	Engine::ImageButton * homeBtn;
    homeBtn = new Engine::ImageButton("btn/home_normal.png", "btn/home_hover.png", 1380, 746, 70, 70);
    homeBtn->SetOnClickCallback(std::bind(&PlayScene::BackHome, this));
    UIGroup->AddNewControlObject(homeBtn);

	// * Danger Indicator (Benjamin)
	// TODO : Change to red vignette
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::UIBtnClicked(int id) {
	if (preview)
		UIGroup->RemoveObject(preview->GetObjectIterator());
    // TODO: [CUSTOM-TURRET]: On callback, create the turret.
	if (id == 0 && money >= MachineGunTurret::Price)
		preview = new MachineGunTurret(0, 0);
	else if (id == 1 && money >= LaserTurret::Price)
		preview = new LaserTurret(0, 0);
	else if (id == 2 && money >= MissileTurret::Price)
		preview = new MissileTurret(0, 0);
	else if (id == 3 && money >= GlueGunnerTurret::Price)
		preview = new GlueGunnerTurret(0, 0);
	if (!preview)
		return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;

	cout << "Checking Space Valid\n";
	auto map00 = mapState[y][x];
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[SpawnGridPoint.y][SpawnGridPoint.x] == -1 || mapState[y][x] == 0)
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1)
			return false;
	}
	// All enemy have path to exit.
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto& it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}

bool PlayScene::PathFindingIsValid(int curr){
	if (curr == TILE_DIRT || curr == TILE_CORNERBTMLEFT || curr == TILE_CORNERBTMRIGHT || curr == TILE_CORNERTOPLEFT || curr == TILE_CORNERTOPRIGHT) return true;
	else return false;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	if (PathFindingIsValid(mapState[MapHeight - 1][MapWidth - 1]) == false) return map;
	que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
	map[MapHeight - 1][MapWidth - 1] = 0;
	while (!que.empty()) {
		Engine::Point p = que.front();
		que.pop();
		// TODO: [BFS PathFinding] (1/1): Implement a BFS starting from the most right-bottom block in the map.
		//               For each step you should assign the corresponding distance to the most right-bottom block.
		//               mapState[y][x] is TILE_DIRT if it is empty.

		int X = p.x - 1;
        int Y = p.y;

        if(X >= 0 && X < MapWidth && Y >= 0 && Y < MapHeight && PathFindingIsValid(mapState[Y][X]) && map[Y][X] == -1) {
            map[Y][X] = map[p.y][p.x] + 1;
            que.push(Engine::Point(X, Y));
        }

        X = p.x + 1;
        Y = p.y;

        if(X >= 0 && X < MapWidth && Y >= 0 && Y < MapHeight && PathFindingIsValid(mapState[Y][X]) && map[Y][X] == -1) {
            map[Y][X] = map[p.y][p.x] + 1;
            que.push(Engine::Point(X, Y));
        }

        X = p.x;
        Y = p.y - 1;

        if(X >= 0 && X < MapWidth && Y >= 0 && Y < MapHeight && PathFindingIsValid(mapState[Y][X]) && map[Y][X] == -1) {
            map[Y][X] = map[p.y][p.x] + 1;
            que.push(Engine::Point(X, Y));
        }

        X = p.x;
        Y = p.y + 1;

        if(X >= 0 && X < MapWidth && Y >= 0 && Y < MapHeight && PathFindingIsValid(mapState[Y][X]) && map[Y][X] == -1) {
            map[Y][X] = map[p.y][p.x] + 1;
            que.push(Engine::Point(X, Y));
        }
	}
	return map;
}


void PlayScene::FastForwardButtonPress(){
	fastForward = !fastForward;
	if (fastForward){
		SpeedMult = 3;
	} else SpeedMult = 1;
}

void PlayScene::BackHome(){
	Engine::GameEngine::GetInstance().ChangeScene("start-scene");
}