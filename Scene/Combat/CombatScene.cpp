#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"

#include "CombatScene.hpp"

//HP value
#include "PlayerCharacter/PlayerCharacter.hpp"

using namespace std;
// To do:
// pass hp value to gamehall scene after finish fight
// Escape function has rng 33% might work
// attack has flat damage
// items consist of healing and attacking , display on the right of the image as button
// display enemy and player sprite
// turn-based combat


// ---Do if extra time---:
// implement beat the monster algorithm for 'auto' function
// make animation

void CombatScene::Initialize() {
    playerdead = false;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    //Button Size
    int Btn_w = 230;
    int Btn_h = 65;
	AddNewObject(new Engine::Image("bg/Combat_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    
	// AddNewObject(new Engine::Label("Combat Scene!", "pixel-font.ttf", 150, halfW, halfH - 175, 255, 255, 255, 255, 0.5, 0.5)); // 0.5 is the anchor for both x and y

    Engine::ImageButton* attack;
    attack = new Engine::ImageButton("btn/btn_attack.png", "btn/btn_attack_hover.png", (halfW - (Btn_w*0.96)*3.07), halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    attack->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(attack);

    Engine::ImageButton* items;
    items = new Engine::ImageButton("btn/btn_items.png", "btn/btn_items_hover.png", (halfW - (Btn_w*0.96)*3.07) + Btn_w*0.96*1.05, halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    items->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(items);

    Engine::ImageButton* escape;
    escape = new Engine::ImageButton("btn/btn_escape.png", "btn/btn_escape_hover.png", (halfW - (Btn_w*0.96)*3.07) + (Btn_w*0.96)*2.10, halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    escape->SetOnClickCallback(bind(&CombatScene::BackOnClick, this)); 
    AddNewControlObject(escape);

    //Player Health Values:
    int x = (halfW - (Btn_w*0.96)*3.07);
    int y = halfH - (Btn_h*0.96) + 275;
    HP_BarBG = new Engine::Image("bg/progressbar_bg.png", x, y, 282, 40 );
    HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", x, y, 282, 40);
    TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(currentHP))), "pixel-font.ttf", 30, x + 8, y + 22, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(HP_BarBG);
    AddNewObject(HP_BarFILL);
    AddNewObject(TXT_HPVal);

    //Player Attack Values:

    ATK_BarBG = new Engine::Image("bg/progressbar_bg.png", x + 325, y, 282, 40 );
    ATK_BarFILL = new Engine::Image("bg/atkbar_fill.png", x + 325, y, ATK_FILL, 40);
    TXT_ATKVal = new Engine::Label("ATK: " + to_string(static_cast<int>(round(playerATK))), "pixel-font.ttf", 30, x + 333, y + 22, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(ATK_BarBG);
    AddNewObject(ATK_BarFILL);
    AddNewObject(TXT_ATKVal);
    // cout << "Initializeed\n";
}

void CombatScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene(backGameHall);
}

void CombatScene::SetPlayerHP(float val){
    this->currentHP = val;
    std::cout << "Current HP: " << currentHP << "\n";
    if(this->currentHP <= 0){
        // playerdead = true;
        Engine::GameEngine::GetInstance().ChangeScene("death-scene");
    }
}
void CombatScene::OnKeyDown(int keycode){
    Engine::IScene::OnKeyDown(keycode);
    if(keycode == ALLEGRO_KEY_P){
        std::cout << "Setting Player HP!\n";
        CombatScene::SetPlayerHP(this->currentHP - 20);
        CombatScene::UpdateHP();
    }
}
void CombatScene::UpdateHP(){
    if (HP_BarFILL) {
        HP_BarFILL->Size.x = 282 * (currentHP / maxHP);
    }
    // Update the HP label text
    if (TXT_HPVal) {
        TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(currentHP)));
    }
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
