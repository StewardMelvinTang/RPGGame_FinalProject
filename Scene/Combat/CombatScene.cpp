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
    Enemy_maxHP = 100;
    Enemy_currentHP = 100;
    playerturn = true;
    playerdead = false;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    //Button Size
    int Btn_w = 230;
    int Btn_h = 65;
	// AddNewObject(new Engine::Image("bg/Combat_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    
	// AddNewObject(new Engine::Label("Combat Scene!", "pixel-font.ttf", 150, halfW, halfH - 175, 255, 255, 255, 255, 0.5, 0.5)); // 0.5 is the anchor for both x and y
    // AddNewObject(new Engine::Image("btn/buttonSquare_beige_pressed.png", halfW/4 , halfH*2, 600, 200, 0.5, 0.5));
    Engine::ImageButton* attack;
    attack = new Engine::ImageButton("btn/btn_attack.png", "btn/btn_attack_hover.png", (halfW - (Btn_w*0.96)*3.07), halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    attack->SetOnClickCallback(bind(&CombatScene::AttackOnClick, this)); 
    AddNewControlObject(attack);

    Engine::ImageButton* items;
    items = new Engine::ImageButton("btn/btn_items.png", "btn/btn_items_hover.png", (halfW - (Btn_w*0.96)*3.07) + Btn_w*0.96*1.05, halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    items->SetOnClickCallback(bind(&CombatScene::ItemsOnClick, this)); 
    AddNewControlObject(items);

    Engine::ImageButton* escape;
    escape = new Engine::ImageButton("btn/btn_escape.png", "btn/btn_escape_hover.png", (halfW - (Btn_w*0.96)*3.07) + (Btn_w*0.96)*2.10, halfH - (Btn_h*0.96) + 350, Btn_w*0.96, Btn_h*0.96);
    escape->SetOnClickCallback(bind(&CombatScene::EscapeOnClick, this)); 
    AddNewControlObject(escape);

    //set currentHP and maxHP
    currentHP = playerChar_combat->GetCurrentHP();
    maxHP = playerChar_combat->GetMaxHP();
    std::cout<<"Curr_HP: " << currentHP << endl;
    std::cout<<"Max_HP: " << maxHP << endl;

    //Player Health Values:
    int x = (halfW - (Btn_w*0.96)*3.07);
    int y = halfH - (Btn_h*0.96) + 275;
    HP_BarBG = new Engine::Image("bg/progressbar_bg.png", x, y, 282, 40 );
    HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", x, y, 282, 40);
    HP_BarFILL->Size.x = 282 * (currentHP / maxHP);
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

    //<-----ENEMY VALUES:----->
    //Enemy Health Values:
    Enemy_HP_BarBG = new Engine::Image("bg/progressbar_bg.png", x, y-550, 282, 40);
    Enemy_HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", x, y-550, 282, 40);
    Enemy_TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(Enemy_currentHP))), "pixel-font.ttf", 30, x + 8, y - 528, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(Enemy_HP_BarBG);
    AddNewObject(Enemy_HP_BarFILL);
    AddNewObject(Enemy_TXT_HPVal);

    //Enemy Attack Values:

    Enemy_ATK_BarBG = new Engine::Image("bg/progressbar_bg.png", x + 325, y-550, 282, 40 );
    Enemy_ATK_BarFILL = new Engine::Image("bg/atkbar_fill.png", x + 325, y-550, ATK_FILL, 40);
    Enemy_TXT_ATKVal = new Engine::Label("ATK: " + to_string(static_cast<int>(round(Enemy_ATK))), "pixel-font.ttf", 30, x + 333, y - 528, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(Enemy_ATK_BarBG);
    AddNewObject(Enemy_ATK_BarFILL);
    AddNewObject(Enemy_TXT_ATKVal);



    //Sprite and Visuals
    Platform = new Engine::Image("bg/buttonSquare_blue.png", x + 1150, items->Position.y, 282, 160, 0.5, 0.5);
    AddNewObject(Platform);
    PlayerSprite = new Engine::Image("char/char_idle_up.png", x + 1150, items->Position.y, 282, 160, 0.5, 0.5);
    AddNewObject(PlayerSprite);

    EnemySprite = new Engine::Image("enemy/Enemy1Sprite.png", x + 1150, Enemy_ATK_BarBG->Position.y, 282*4, 160*4, 0.5, 0.5);
    AddNewObject(EnemySprite);
    // cout << "Initializeed\n";
}

void CombatScene::EscapeOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene(backGameHall);
}

void CombatScene::AttackOnClick(){
    if(!playerturn){
        return;
    }

    Enemy_currentHP -= playerATK;
    Enemy_TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(Enemy_currentHP)));
    if (Enemy_HP_BarFILL) {
        std::cout<<"Enemy_Curr_HP: " << currentHP << endl;
        std::cout<<"Enemy_Max_HP: " << maxHP << endl;
        Enemy_HP_BarFILL->Size.x = 282 * (Enemy_currentHP / Enemy_maxHP);
    }
    playerturn = false;
    //Enemy Attacks
    SetPlayerHP(currentHP - Enemy_ATK);
    playerturn = true;
    if(currentHP <= 0){
        Engine::GameEngine::GetInstance().ChangeScene("death-scene");
    }

    if(Enemy_currentHP <= 0){
        Engine::GameEngine::GetInstance().ChangeScene(backGameHall);
    }
}
void CombatScene::ItemsOnClick(){
    if(!playerturn) return;
    displayitems = true;
    //display items beside the UI
}

void CombatScene::SetPlayerHP(float val){
    this->currentHP = val;
    std::cout << "Current HP: " << currentHP << "\n";
    UpdateHP();
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
        // CombatScene::UpdateHP();
    }
    std::cout << "OnKeyDown!\n";
}
void CombatScene::UpdateHP(){
    if (HP_BarFILL) {
        std::cout<<"Curr_HP: " << currentHP << endl;
        std::cout<<"Max_HP: " << maxHP << endl;
        HP_BarFILL->Size.x = 282 * (currentHP / maxHP);
    }
    // Update the HP label text
    if (TXT_HPVal) {
        TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(currentHP)));
    }
}
void CombatScene::Terminate() {
    IScene::Terminate();
}

void CombatScene::Update(float deltaTime) {
    
}

