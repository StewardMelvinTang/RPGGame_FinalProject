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
// int y = items->Position.y;
void CombatScene::Initialize() {
    Enemy_maxHP = 100;
    Enemy_currentHP = 100;
    playerturn = true;
    playerdead = false;
    IsUsingShield = false;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    //Button Size
    int Btn_w = 230;
    int Btn_h = 65;
	AddNewObject(new Engine::Image("bg/battlearena_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    
	// AddNewObject(new Engine::Label("Combat Scene!", "pixel-font.ttf", 150, halfW, halfH - 175, 255, 255, 255, 255, 0.5, 0.5)); // 0.5 is the anchor for both x and y
    // AddNewObject(new Engine::Image("btn/buttonSquare_beige_pressed.png", halfW/4 , halfH*2, 600, 200, 0.5, 0.5));
    Engine::ImageButton* attack;
    attack = new Engine::ImageButton("btn/btn_attack.png", "btn/btn_attack_hover.png", 865, 593, 270, 64);
    attack->SetOnClickCallback(bind(&CombatScene::AttackOnClick, this)); 
    AddNewControlObject(attack);

    Engine::ImageButton* items;
    items = new Engine::ImageButton("btn/btn_items.png", "btn/btn_items_hover.png", 865, 664, 274, 64);
    items->SetOnClickCallback(bind(&CombatScene::ItemsOnClick, this)); 
    AddNewControlObject(items);

    Engine::ImageButton* escape;
    escape = new Engine::ImageButton("btn/btn_escape.png", "btn/btn_escape_hover.png", 865, 733, 274, 64);
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
    HP_BarBG = new Engine::Image("bg/progressbar_bg.png", 20, 305, 282, 40);
    HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", 20, 305, 282, 40);
    HP_BarFILL->Size.x = 282 * (currentHP / maxHP);
    TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(currentHP))), "pixel-font.ttf", 30, 28, 327, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(HP_BarBG);
    AddNewObject(HP_BarFILL);
    AddNewObject(TXT_HPVal);

    //Player Attack Values:

    ATK_BarBG = new Engine::Image("bg/progressbar_bg.png", 20, 353, 282, 40 );
    ATK_BarFILL = new Engine::Image("bg/atkbar_fill.png", 20, 353, ATK_FILL, 40);
    TXT_ATKVal = new Engine::Label("ATK: " + to_string(static_cast<int>(round(playerATK))), "pixel-font.ttf", 30, 23 , 375, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(ATK_BarBG);
    AddNewObject(ATK_BarFILL);
    AddNewObject(TXT_ATKVal);

    //<-----ENEMY VALUES:----->
    //Enemy Health Values:
    Enemy_HP_BarBG = new Engine::Image("bg/progressbar_bg.png", 1303, 34, 282, 40);
    Enemy_HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", 1303, 34, 282, 40);
    Enemy_TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(Enemy_currentHP))), "pixel-font.ttf", 30, 1310, 37, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(Enemy_HP_BarBG);
    AddNewObject(Enemy_HP_BarFILL);
    AddNewObject(Enemy_TXT_HPVal);

    //Enemy Attack Values:

    Enemy_ATK_BarBG = new Engine::Image("bg/progressbar_bg.png", 1303, 82, 282, 40 );
    Enemy_ATK_BarFILL = new Engine::Image("bg/atkbar_fill.png", 1303, 82, ATK_FILL, 40);
    Enemy_TXT_ATKVal = new Engine::Label("ATK: " + to_string(static_cast<int>(round(Enemy_ATK))), "pixel-font.ttf", 30, 1310, 85, 255, 255, 255, 255, 0.0, 0.5);
    AddNewObject(Enemy_ATK_BarBG);
    AddNewObject(Enemy_ATK_BarFILL);
    AddNewObject(Enemy_TXT_ATKVal);

    

    //Sprite and Visuals
    // Platform = new Engine::Image("bg/buttonSquare_blue.png", x + 1150, items->Position.y, 282, 160, 0.5, 0.5);
    // AddNewObject(Platform);

    std::string avatarFilePath = "avatar/avatar_" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "_combat.png";
    PlayerSprite = new Engine::Image(avatarFilePath, 104, 440, 413, 392, 0, 0);
    AddNewObject(PlayerSprite);

    EnemySprite = new Engine::Image("enemy/Enemy1Sprite.png", x + 1150, Enemy_ATK_BarBG->Position.y, 282*4, 160*4, 0.5, 0.5);
    AddNewObject(EnemySprite);
    // cout << "Initializeed\n";

    //Items
    Healing_Amount = playerChar_combat->healthPotion;
    Missile_Amount = playerChar_combat->missile;
    Shield_Amount = playerChar_combat->shield;

    Healing = new Engine::ImageButton("play/healthpotion.png", "play/healthpotion.png", x + 750, items->Position.y, Btn_h*0.96);
    Missile = new Engine::ImageButton("play/missile.png", "play/missile.png", x + 850, items->Position.y, Btn_h*0.96);
    Shield = new Engine::ImageButton("play/shield.png", "play/shield.png", x + 950, items->Position.y, Btn_h*0.96);
    
    // Healing_num = new Engine::Label("x " + to_string(static_cast<int>(round(Healing_Amount))), "pixel-font.ttf", 30, x + 8, y + 22, 255, 255, 255, 255, 0.0, 0.5);
    // Missile_num = new Engine::Label("x " + to_string(static_cast<int>(round(Missile_Amount))), "pixel-font.ttf", 30, x + 8, y + 22, 255, 255, 255, 255, 0.0, 0.5);
    // Shield_num = new Engine::Label("x " + to_string(static_cast<int>(round(Shield_Amount))), "pixel-font.ttf", 30, x + 8, y + 22, 255, 255, 255, 255, 0.0, 0.5);
    Healing->SetOnClickCallback(bind(&CombatScene::UseHealth, this)); 
    Missile->SetOnClickCallback(bind(&CombatScene::UseMissile, this)); 
    Shield->SetOnClickCallback(bind(&CombatScene::UseShield, this));
    
    AddNewControlObject(Healing, true);
    AddNewControlObject(Missile, true);
    AddNewControlObject(Shield, true);
}

void CombatScene::EscapeOnClick(){
    int num = rand() % 50;
    if (num % 2 == 0){
        CombatScene::Terminate();
        Engine::GameEngine::GetInstance().ChangeScene(backGameHall);
        return;
    }
    playerturn = false;
    return;
}

void CombatScene::AttackOnClick(){
    if(!playerturn){
        return;
    }
    UpdateEnemyHP();
    CombatScene::CheckState();
    playerturn = false;
}
void CombatScene::UpdateEnemyHP(){
    Enemy_currentHP -= playerATK;
    if (Enemy_TXT_HPVal) Enemy_TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(Enemy_currentHP)));
    if (Enemy_HP_BarFILL) {
        std::cout<<"Enemy_Curr_HP: " << currentHP << endl;
        std::cout<<"Enemy_Max_HP: " << maxHP << endl;
        Enemy_HP_BarFILL->Size.x = 282 * (Enemy_currentHP / Enemy_maxHP);
    }
}
void CombatScene::ItemsOnClick(){
    if(!playerturn) return;
    displayitems = true;

    // AddNewObject(Healing_num);
    // AddNewObject(Missile_num);
    // AddNewObject(Shield_num);

    // Healing = new Engine::ImageButton("play/healthpotion.png", "play/healthpotion.png", x + 750, y, Btn_h*0.96);
    // Missile = new Engine::ImageButton("play/missile.png", "play/missile.png", x + 850, y, Btn_h*0.96);
    // Shield = new Engine::ImageButton("play/shield.png", "play/shield.png", x + 950, y, Btn_h*0.96);
    

    // if (!Healing || !Missile || !Shield){
    //     cout << "ONE OF HEALING/MISSILE/SHIELD BUTTON IS MISSING\n";
    // }
    // if (Healing) AddNewControlObject(Healing);
    // if (Missile) AddNewControlObject(Missile);
    // if (Shield) AddNewControlObject(Shield); 
    // if (Healing) Healing->SetOnClickCallback(bind(&CombatScene::UseHealth, this)); 
    // if (Missile) Missile->SetOnClickCallback(bind(&CombatScene::UseMissile, this)); 
    // if (Shield) Shield->SetOnClickCallback(bind(&CombatScene::UseShield, this)); 
    //display items beside the UI
}
void CombatScene::RemoveReplace(){
    std::cout << "Removing Buttons on Items!../n" << endl;
    displayitems = false;
    // RemoveObject(Healing_num->GetObjectIterator());
    // RemoveObject(Missile_num->GetObjectIterator());
    // RemoveObject(Shield_num->GetObjectIterator());
    // Healing->SetOnClickCallback(bind(&CombatScene::Empty, this)); 
    // Missile->SetOnClickCallback(bind(&CombatScene::Empty, this)); 
    // Shield->SetOnClickCallback(bind(&CombatScene::Empty, this)); 
    // RemoveObject(Healing->GetObjectIterator());
    // RemoveObject(Missile->GetObjectIterator());
    // RemoveObject(Shield->GetObjectIterator());

}
void CombatScene::UseHealth(){
    std::cout << "Healing Amount: " << Healing_Amount << endl;
    if(Healing_Amount <= 0){
        std::cout << "Ran out of healing!...\n";
        RemoveReplace();
        return;
    }
    else if(currentHP + 15 != maxHP){
        SetPlayerHP(currentHP + 15);
        Healing_Amount -= 1;
        std::cout << "Using Healing!...\n";
    }
    RemoveReplace();
    playerturn = false;
}
void CombatScene::UseMissile(){
    std::cout << "Missile Amount: " << Missile_Amount << endl;
    if(Missile_Amount <= 0){
        std::cout << "Ran out of Missiles!...\n";
        RemoveReplace();
        return;
    }
    Enemy_currentHP -= 35;
    UpdateEnemyHP();
    Missile_Amount -= 1;
    std::cout << "Used Missile!...\n";
    RemoveReplace();
    playerturn = false;
    CombatScene::CheckState();
}
void CombatScene::UseShield(){
    std::cout << "Shield Amount: " << Shield_Amount << endl;
    if(Shield_Amount <= 0){
        std::cout << "Ran out of Shields!...\n";
        RemoveReplace();
        return;
    } 
    IsUsingShield = true;
    Shield_Amount -= 1;
    std::cout << "Using Shield!...\n";
    RemoveReplace();
    playerturn = false;
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

void CombatScene::VirtualDraw() const {
    if (displayitems){
        if (Healing) Healing->Draw();
        if (Missile) Missile->Draw();
        if (Shield) Shield->Draw();
    }
}

void CombatScene::UpdateHP(){
    if (HP_BarFILL) {
        // std::cout<<"Curr_HP: " << currentHP << endl;
        // std::cout<<"Max_HP: " << maxHP << endl;
        HP_BarFILL->Size.x = 282 * (currentHP / maxHP);
    }
    // Update the HP label text
    if (TXT_HPVal) {
        TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(currentHP)));
    }
}
void CombatScene::Terminate() {
    IScene::Terminate();
    cout << "Terminated\n";
}
void CombatScene::CheckState(){
    if(currentHP <= 0){
        Engine::GameEngine::GetInstance().ChangeScene("death-scene");
    }

    if(Enemy_currentHP <= 0){
        Engine::GameEngine::GetInstance().ChangeScene(backGameHall);
    }
}
void CombatScene::Update(float deltaTime) {
    if(!playerturn){
        if(IsUsingShield){
            SetPlayerHP(currentHP - (Enemy_ATK/2));
            IsUsingShield = false;
            playerturn = true;
            std::cout << "Shield Used!...\n";
            return;
        }
        int counter = currentHP - Enemy_ATK;
        // std::cout << "counter: " << counter << endl;
        int temp;
        //fix later 
        while(currentHP > counter){
            temp = currentHP - 0.05*deltaTime;
            SetPlayerHP(temp);
            if(currentHP <= counter){
                break;
            }
        }
        std::cout<<"END! Curr_HP: " << currentHP << endl;
        playerturn = true;
    }

    CombatScene::CheckState();
}
void CombatScene::Empty(){
    // Haha
}

