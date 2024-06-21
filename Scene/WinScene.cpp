#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "WinScene.hpp"
#include "LoseScene.hpp"
#include "PlayScene.hpp"
#include "ScoreBoardScene.hpp"
#include <vector>

using namespace std;

void WinScene::Initialize() {
	ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;

	AddNewObject(new Engine::Image("bg/VictoryScreen_Game.png", 0 ,0 , 1600, 832, 0.0, 0.0));
	// AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 -10, 255, 255, 255, 255, 0.5, 0.5));
	AddNewObject(new Engine::Label(to_string(score), "pixel-font.ttf", 42, halfW, 350, 255 , 162, 0, 255, 0.5, 0.5));
	AddNewObject(new Engine::Label(to_string(enemiesKilled), "pixel-font.ttf", 42, halfW, 425, 255, 255, 255, 255, 0.5, 0.5));
	AddNewObject(new Engine::Label(to_string(towersBuilt), "pixel-font.ttf", 42, halfW, 495, 255, 255, 255, 255, 0.5, 0.5));

	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("btn/restart_normal.png", "btn/restart_hover.png", 648, 551, 70, 70);
	btn->SetOnClickCallback(std::bind(&WinScene::RestartBtn, this));
	AddNewControlObject(btn);

	btn = new Engine::ImageButton("btn/home_normal.png", "btn/home_hover.png", 751, 538, 100, 100);
	btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 0));
	AddNewControlObject(btn);

	btn = new Engine::ImageButton("btn/scoreboard_normal.png", "btn/scoreboard_hover.png", 881, 551, 70, 70);
	btn->SetOnClickCallback(std::bind(&WinScene::ScoreBoardBtn, this));
	AddNewControlObject(btn);


	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
	bgmId = AudioHelper::PlayAudio("win.wav");

	// * Upload To Leaderboard
	vector<scoreBoardData> oldSb = Engine::GameEngine::GetInstance().LoadScoreBoard();
	scoreBoardData entry; bool shouldSave = false, foundName = false;
	for (scoreBoardData dt : oldSb){
		if (dt.playerName == Engine::GameEngine::currentActivePlayerName){
			// * found
			foundName = true;
			if (dt.highestScore >= score) return; // * dont have to update scoreBoard
			shouldSave = true;
			cout << "Updating old score " << dt.highestScore << " To be new score : " << score << endl;
			dt.highestScore = score;
			dt.saveTime = Engine::GameEngine::GetInstance().GetCurrentDateTime();
			entry = dt;
		}
	}
	if (foundName == false){
		cout << "Cannot find name... creating one\n";
		entry.highestScore = score;
		entry.playerName = Engine::GameEngine::currentActivePlayerName;
		entry.saveTime = Engine::GameEngine::GetInstance().GetCurrentDateTime();
		shouldSave = true;
	}

	if (shouldSave) Engine::GameEngine::GetInstance().WriteScoreBoard(oldSb, entry);
}
void WinScene::Terminate() {
	IScene::Terminate();
	AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
	ticks += deltaTime;
	if (ticks > 4 && ticks < 100 &&
		dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
		ticks = 100;
		bgmId = AudioHelper::PlayBGM("happy.ogg");
	}
}
void WinScene::BackOnClick(int stage) {
	// Change to select scene.
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}


void WinScene::RestartBtn(){
	PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void WinScene::ScoreBoardBtn(){
	ScoreBoardScene* scene = dynamic_cast<ScoreBoardScene*>(Engine::GameEngine::GetInstance().GetScene("score-board"));
    Engine::GameEngine::GetInstance().ChangeScene("score-board");
	if (scene){
		scene->backMenuLevel = "win";
	}
}
