// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreBoardScene.hpp"
#include "Scene/GameScene_Hall.hpp"
#include "Scene/ForestSceneUp.hpp"
#include "Scene/VillageScene.hpp"
#include "Scene/BossArenaScene.hpp"
//Loading Scene
#include "Scene/Loading/LoadingScene.hpp"
//Death Scene
#include "Scene/Death/DeathScene.hpp"
//Combat Scene
#include "Scene/Combat/CombatScene.hpp"  
//Profile Scene
#include "Scene/Profile/ProfileScene.hpp"  
#include "Scene/Profile/NewProfileScene.hpp"  
int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

	// game.AddNewScene("play", new PlayScene());
	// game.AddNewScene("lose", new LoseScene());	
	// game.AddNewScene("win", new WinScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("gamescene_hall", new GameSceneHall());
	game.AddNewScene("forestscene_up", new ForestScene());
	game.AddNewScene("villagescene", new VillageScene());
	game.AddNewScene("bossarenascene", new BossArenaScene());

	game.AddNewScene("stage-select", new StageSelectScene());

	game.AddNewScene("start-scene", new StartScene());
	game.AddNewScene("score-board", new ScoreBoardScene());
	//Loading Scene
	game.AddNewScene("loading-scene", new LoadingScene());
	//Death Scene
	game.AddNewScene("death-scene", new DeathScene());
	//Combat Scene
	game.AddNewScene("combat-scene", new CombatScene());
	//Profile Scene
	game.AddNewScene("profile-scene", new ProfileScene());
	game.AddNewScene("newprofile-scene", new NewProfileScene());

	game.Start("start-scene", 60, 1600, 832); // 1600 / blockSize = 25
	
	//game.Start("settings", 60, 1600, 832);
	return 0;
}
