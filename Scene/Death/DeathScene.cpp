#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"


using namespace std;
#include "DeathScene.hpp"


void DeathScene::Initialize() {
    
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    //Button Size
    int Btn_w = 518;
    int Btn_h = 147;
	AddNewObject(new Engine::Image("bg/blackscreen_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));

	AddNewObject(new Engine::Label("You Died!", "pixel-font.ttf", 150, halfW, halfH - 175, 255, 255, 255, 255, 0.5, 0.5)); // 0.5 is the anchor for both x and y
    


    Engine::ImageButton* back;
    back = new Engine::ImageButton("btn/btn_restart.png", "btn/btn_restart_hover.png", halfW - (Btn_w/3), halfH - (Btn_h/3) + 250, Btn_w/1.5, Btn_h/1.5);
    back->SetOnClickCallback(bind(&DeathScene::BackOnClick, this)); 
    AddNewControlObject(back);


    bgmId = AudioHelper::PlayBGM("GameSceneHall_Theme.ogg"); 
    // cout << "Initializeed\n";
}

void DeathScene::BackOnClick(){
    Terminate();
    Engine::GameEngine::GetInstance().ChangeScene(backMenuLevel);
}

void DeathScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
	// AudioHelper::StopSample(deathBGMInstance);
	// deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	cout << "Terminated_DeathScene\n";
	IScene::Terminate();
}

void DeathScene::Update(float deltaTime) {
    // if (duration > 0.0f) {
    //     currentProgress += deltaTime;
    //     float progressPercentage = currentProgress / totalDur;

    //     if (loadingProgressBar) {
    //         int progressBarWidth = (Engine::GameEngine::GetInstance().GetScreenSize().x - 50) * progressPercentage;
    //         loadingProgressBar->Size.x = progressBarWidth;
    //     }

    //     if (currentProgress >= totalDur) {
    //         cout << "Loading Complete\n";
    //         Engine::GameEngine::GetInstance().ChangeScene(nextScene);
    //     }
    // }
}

void DeathScene::InitLoadingScreen(std::string nextScene, float duration){
    // this->duration = duration;
    // this->nextScene = nextScene;
    // this->totalDur = duration;
}
