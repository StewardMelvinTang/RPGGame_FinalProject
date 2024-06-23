#include <functional>
#include <queue>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
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

    EnemySprite = new Engine::Image("enemy/enemy_enemy1.png", 1235, 290, 572, 319, 0.5, 0.5);
    AddNewObject(EnemySprite);
    // cout << "Initializeed\n";

    //Items
    Healing_Amount = playerChar_combat->healthPotion;
    Missile_Amount = playerChar_combat->missile;
    Shield_Amount = playerChar_combat->shield;

    Healing = new Engine::ImageButton("btn/btn_health.png", "btn/btn_health.png", 1154, 605, 223, 58);
    Missile = new Engine::ImageButton("btn/btn_missile.png", "btn/btn_missile.png", 1154, 668 , 223, 57);
    Shield = new Engine::ImageButton("btn/btn_shield.png", "btn/btn_shield.png", 1154, 730, 223, 57);
    
    Healing_num = new Engine::Label("x " + to_string(static_cast<int>(round(Healing_Amount))), "pixel-font.ttf", 22, 1338, 642, 119, 255, 119, 255, 0.0, 0.5);
    Missile_num = new Engine::Label("x " + to_string(static_cast<int>(round(Missile_Amount))), "pixel-font.ttf", 22, 1338, 705, 119, 255, 119, 255, 0.0, 0.5);
    Shield_num = new Engine::Label("x " + to_string(static_cast<int>(round(Shield_Amount))), "pixel-font.ttf", 22, 1338, 767, 119, 255, 119, 255, 0.0, 0.5);
    Healing->SetOnClickCallback(bind(&CombatScene::UseHealth, this)); 
    Missile->SetOnClickCallback(bind(&CombatScene::UseMissile, this)); 
    Shield->SetOnClickCallback(bind(&CombatScene::UseShield, this));
    
    AddNewControlObject(Healing, true);
    AddNewControlObject(Missile, true);
    AddNewControlObject(Shield, true);

    Extend = new Engine::ImageButton("play/extension.png","play/extension.png", 1133 , 593 , 253, 205);
    AddNewControlObject(Extend, true);

    Auto_btn = new Engine::ImageButton("btn/btn_auto.png","btn/btn_auto.png", 1395 , 736 , 156, 60);
    AddNewControlObject(Auto_btn);
}

void CombatScene::EscapeOnClick(){
    displayitems = false;
    RemoveReplace();
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
    displayitems = false;
    RemoveReplace();
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
    
    Healing->SetOnClickCallback(bind(&CombatScene::UseHealth, this)); 
    Missile->SetOnClickCallback(bind(&CombatScene::UseMissile, this)); 
    Shield->SetOnClickCallback(bind(&CombatScene::UseShield, this));
}
void CombatScene::RemoveReplace(){
    std::cout << "Removing Buttons on Items!../n" << endl;
    displayitems = false;
    Healing->SetOnClickCallback(bind(&CombatScene::Empty, this)); 
    Missile->SetOnClickCallback(bind(&CombatScene::Empty, this)); 
    Shield->SetOnClickCallback(bind(&CombatScene::Empty, this));
}
void CombatScene::UseHealth(){
    std::cout << "Healing Amount: " << Healing_Amount << endl;
    if(Healing_Amount <= 0){
        std::cout << "Ran out of healing!...\n";
        RemoveReplace();
        return;
    }
    else if(currentHP + health_weight != maxHP){
        SetPlayerHP(currentHP + health_weight);
        Healing_Amount -= 1;
        Healing_num->Text = "x " + to_string(static_cast<int>(round(Healing_Amount)));
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
    Enemy_currentHP -= missile_weight;
    UpdateEnemyHP();
    Missile_Amount -= 1;
    Missile_num->Text = "x " + to_string(static_cast<int>(round(Missile_Amount)));
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
    Shield_num->Text = "x " + to_string(static_cast<int>(round(Shield_Amount)));
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
        if (Extend) Extend->Draw();
        if (Healing) Healing->Draw();
        if (Missile) Missile->Draw();
        if (Shield) Shield->Draw();
        // if (Auto_btn) Auto_btn->Draw();
        if (Healing_num) Healing_num->Draw();
        if (Missile_num) Missile_num->Draw();
        if (Shield_num) Shield_num->Draw();
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
        // while(currentHP > counter){
        //     temp = currentHP - 0.05*deltaTime;
        //     SetPlayerHP(temp);
        //     if(currentHP <= counter){
        //         break;
        //     }
        // }
        SetPlayerHP(currentHP - Enemy_ATK);
        std::cout<<"END! Curr_HP: " << currentHP << endl;
        playerturn = true;
    }

    CombatScene::CheckState();
}
void CombatScene::Empty(){
    // Haha
}

// * ============= ai thing, kinda cool ??
float CombatScene::evaluateScenarioValue(const State& s) {
    if(s.enemyHp == 0) return 999999999.0f;
    else if(s.playerHp == 0) return -999999999.0f;

    // positive: player is in advantage, negative: disadvantage
    float value = 0;

    // hp
    value += (s.playerHp - s.enemyHp);

    // eval the values based on the current remaining skills (only a rough estimation for now)
    // 0 is the current placeholder because the boss is not coded yet
    int playerAttackNet = s.missileCount - 0;
    int enemyAttackNet = 0 - s.shieldCount;
    float playerHealNet = (maxHP - s.playerHp) * s.healingCount;
    float enemyHealNet = (Enemy_maxHP - s.enemyHp) * 0;

    // net heuristic value
    value += (playerAttackNet - enemyAttackNet) * 15; // 15 is the average skills damage
    value += (playerHealNet - enemyHealNet) * 10; // 10 is the average healing

    return value;
}

// helper hash function (hash the current state)
string CombatScene::hashState(const State& s) {
    std::stringstream input;
    input << s.playerHp << s.enemyHp << s.shieldCount << s.healingCount << s.missileCount;

    string res;
    getline(input, res);

    return res;
}

std::vector<CombatScene::State> CombatScene::generateMoves(const CombatScene::State& s, bool isInit) {
    vector<State> moves;
    
    // move order priority: missile -> basic attack -> shield -> heal

    // missile 
    if(s.missileCount > 0) {
        State newState = s;

        // attack the enemy
        newState.enemyHp = max(0.0f, s.enemyHp - missile_weight);
        newState.missileCount--;
        newState.move = isInit ? USE_MISSILE : s.move;

        // enemy attacks
        newState.playerHp = max(0.0f, s.playerHp - Enemy_ATK);

        // evaluate the scenario value
        newState.scenarioValue = evaluateScenarioValue(newState);

        moves.push_back(newState);
    }

    // basic attack
    {
        State newState = s;

        // attack the enemy
        newState.enemyHp = max(0.0f, s.enemyHp - playerATK);
        newState.move = isInit ? ATTACK : s.move;

        // enemy attacks
        newState.playerHp = max(0.0f, s.playerHp - Enemy_ATK);

        // evaluate the scenario value
        newState.scenarioValue = evaluateScenarioValue(newState);

        moves.push_back(newState);
    }

    if(s.shieldCount > 0) {
        State newState = s;

        // enemy attacks
        newState.playerHp = max(0.0f, s.playerHp - (Enemy_ATK/2.0f));
        newState.move = isInit ? USE_SHIELD : s.move;
        
        // eval
        newState.scenarioValue = evaluateScenarioValue(newState);

        moves.push_back(newState);
    }

    if(s.healingCount > 0) {
        State newState = s;

        // heal player
        newState.playerHp = min(maxHP, newState.playerHp + health_weight);
        newState.healingCount--;
        newState.move = isInit ? USE_HEALING : s.move;

        // enemy attacks
        newState.playerHp = max(0.0f, s.playerHp - Enemy_ATK);
        
        // eval
        newState.scenarioValue = evaluateScenarioValue(newState);

        moves.push_back(newState);
    }

    return moves;
}

CombatScene::Move CombatScene::search(int depth) {
    State init{currentHP, Shield_Amount, Healing_Amount, Missile_Amount, Enemy_currentHP, ATTACK, INT32_MIN};
    // the initial last move and scenario can be ignored

    set<string> explored;

    priority_queue<State, vector<State>, CompareScenarioValue> pq;

    for(State& initMoves : generateMoves(init, true)) 
        pq.push(initMoves);

    while(!pq.empty() && depth--) {
        const State& curr = pq.top(); pq.pop();
        explored.insert(hashState(curr));

        for(State& next : generateMoves(curr, false)) {
            // possible transposition usage here
            if(explored.find(hashState(next)) != explored.end()) continue;

            // possible floating point error
            if(next.enemyHp <= 0) return next.move;
            else if(next.playerHp <= 0) continue;

            // alpha beta pruning here?
            pq.push(next);
        }
    }

    if(pq.empty()) return ESCAPE;
    else return pq.top().move;
}