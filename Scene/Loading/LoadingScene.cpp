#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"

using namespace std;
#include "LoadingScene.hpp"

float currentProgress = 0.0;

void LoadingScene::Initialize() {
    currentProgress = 0.0f;
    // duration = 0.0f;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	AddNewObject(new Engine::Image("bg/blackscreen_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    
	AddNewObject(new Engine::Label("Loading...", "pixel-font.ttf", 42, 25, h - 100, 255, 255, 255, 255, 0, 0));
    AddNewObject(new Engine::Image("bg/progressbar_bg.png", 25, h - 50, w - 25, 30));
    AddNewObject(loadingProgressBar = new Engine::Image("bg/pbLong_fill.png", 25, h - 50, w - 25, 30));
    // cout << "Initializeed\n";
}

void LoadingScene::Terminate() {

}

void LoadingScene::Update(float deltaTime) {
    if (duration > 0.0f) {
        currentProgress += deltaTime;
        float progressPercentage = currentProgress / totalDur;

        if (loadingProgressBar) {
            int progressBarWidth = (Engine::GameEngine::GetInstance().GetScreenSize().x - 50) * progressPercentage;
            loadingProgressBar->Size.x = progressBarWidth;
        }

        if (currentProgress >= totalDur) {
            cout << "Loading Complete\n";
            Engine::GameEngine::GetInstance().ChangeScene(nextScene);
        }
    }
}

void LoadingScene::InitLoadingScreen(std::string nextScene, float duration){
    this->duration = duration;
    this->nextScene = nextScene;
    this->totalDur = duration;
}
