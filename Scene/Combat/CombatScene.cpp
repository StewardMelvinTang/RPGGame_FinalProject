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
#include "CombatScene.hpp"


void CombatScene::Initialize() {
    
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    //Button Size
    int Btn_w = 230;
    int Btn_h = 65;
	AddNewObject(new Engine::Image("bg/blackscreen_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));

	// AddNewObject(new Engine::Label("You Died!", "pixel-font.ttf", 150, halfW, halfH - 175, 255, 255, 255, 255, 0.5, 0.5)); // 0.5 is the anchor for both x and y
    

    Engine::ImageButton* attack;
    attack = new Engine::ImageButton("btn/btn_attack.png", "btn/btn_attack_hover.png", halfW - (Btn_w * 1.5/2), halfH - (Btn_h * 1.5/2) + 50, Btn_w * 1.5, Btn_h * 1.5);
    attack->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(attack);

    Engine::ImageButton* items;
    items = new Engine::ImageButton("btn/btn_items.png", "btn/btn_items_hover.png", halfW - (Btn_w * 1.5/2), halfH - (Btn_h * 1.5/2) + 150, Btn_w * 1.5, Btn_h * 1.5);
    items->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(items);

    Engine::ImageButton* escape;
    escape = new Engine::ImageButton("btn/btn_escape.png", "btn/btn_escape_hover.png", halfW - (Btn_w * 1.5/2), halfH - (Btn_h * 1.5/2) + 250, Btn_w * 1.5, Btn_h * 1.5);
    escape->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(escape);

    // cout << "Initializeed\n";
}

void CombatScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene(backMenuLevel);
}

void CombatScene::Terminate() {

}

void CombatScene::Update(float deltaTime) {
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

void CombatScene::InitLoadingScreen(std::string nextScene, float duration){
    // this->duration = duration;
    // this->nextScene = nextScene;
    // this->totalDur = duration;
}
