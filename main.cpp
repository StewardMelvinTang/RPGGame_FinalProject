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
//Loading Scene
#include "Scene/Loading/LoadingScene.hpp"
//Death Scene
#include "Scene/Death/DeathScene.hpp"
                                           
int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("gamescene_hall", new GameSceneHall());

	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("start-scene", new StartScene());
	//Loading Scene
	game.AddNewScene("loading-scene", new LoadingScene());
	
	game.AddNewScene("score-board", new ScoreBoardScene());
	//Death Scene
	game.AddNewScene("death-scene", new DeathScene());
	
	game.Start("gamescene_hall", 60, 1600, 832); // 1600 / blockSize = 25
	
	//game.Start("settings", 60, 1600, 832);
	return 0;
}
