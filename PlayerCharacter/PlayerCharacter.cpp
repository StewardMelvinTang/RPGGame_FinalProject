#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
#include "Engine/AudioHelper.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/Loading/LoadingScene.hpp"
#include "Scene/GameScene_Hall.hpp"
#include "PlayerCharacter.hpp"
#include "Engine/Collider.hpp"
#include <fstream>
#include <sstream>
#include <ostream>

// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4
#define KEYBOARD_F 6

bool keys[4] = {false, false, false, false}; // W, S, A, D (input holding)

PlayerCharacter::PlayerCharacter(float x, float y, float speed, float hp, int money, int blockSize, string mapID, PlayerEntry entry): 
    Engine::Sprite("char/1/char_idle_down.png", x, y),
    currentMapID(mapID)    
{
    InitializeProfile(entry);
    this->x = x; this->y = y; // Set Position in screen
    this->speed = speed; this->money = money;

    charSpriteObj = new Engine::Image("char/" + to_string(entry.avatarID + 1) + "/" + idleSprites[0], x, y, size, size);
    
    ConstructPlayerHUD();
}
void PlayerCharacter::InitializeProfile(PlayerEntry entry){
    maxHP = entry.maxHP;
    currentHP = entry.currentHP;
    speed = entry.speed;
    attackDamage = entry.atkDMG;
    currentEXP = entry.currentEXP;
    maxEXP = entry.maxEXP;
    playerLevel = entry.playerLevel;
    healthPotion = entry.healthPotion;
    missile = entry.missile;
    shield = entry.shield;
}

void PlayerCharacter::ConstructPlayerHUD(){
    // * Size 282 x 40, 282 * (maxHP / currHP) to track progressbar percent

    LevelUpBG = new Engine::Image("bg/levelup_screen.png", 0, -300, 1600, 832);
    ChestObtainBG = new Engine::Image("bg/chestobtain_01.png", -400, 0, 1600, 832);
    HP_BarBG = new Engine::Image("bg/progressbar_bg.png", 16, 770, 282, 40 );
    HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", 16, 770, 282, 40);
    TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(currentHP))), "pixel-font.ttf", 30, 20, 790, 255, 255, 255, 255, 0.0, 0.5);

    EXP_BarBG = new Engine::Image("bg/progressbar_bg.png", 16, 720, 282, 40 );
    EXP_BarFILL = new Engine::Image("bg/progressbar_fill_white.png", 16, 720, 282, 40);

    std::string expString = "LVL " + to_string(playerLevel) + " " + to_string(currentEXP) + "/" + to_string(maxEXP);
    TXT_EXPVal = new Engine::Label(expString, "pixel-font.ttf", 30, 20, 740, 255, 255, 255, 255, 0.0, 0.5);

    HotBarBG = new Engine::Image("bg/inventory_hotbar_bg.png", 694, 761, 210, 64);
    IMG_Potion = new Engine::Image("play/healthpotion.png", 697, 760, 64, 64);
    TXT_Potion = new Engine::Label("x" + to_string(this->healthPotion), "pixel-font.ttf", 27, 756, 795, 35, 240, 35, 255, 1.0f);

    IMG_Missile = new Engine::Image("play/missile.png", 697 + 69, 760, 64, 64);
    TXT_Missile = new Engine::Label("x" + to_string(this->missile), "pixel-font.ttf", 27, 756 + 69, 795, 35, 240, 35, 255, 1.0f);

    IMG_Shield = new Engine::Image("play/shield.png", 697 + 138, 760, 64, 64);
    TXT_Shield = new Engine::Label("x" + to_string(this->shield), "pixel-font.ttf", 27, 756 + 138, 795, 35, 240, 35, 255, 1.0f);

    TXT_Interaction = new Engine::Label("Press [F] to Interact", "pixel-font.ttf", 35, 800, 700, 255, 255, 255, 255, 0.5, 0.5);
}

void PlayerCharacter::DrawPlayerHUD() const{
    if (LevelUpBG && LevelUpBG->Position.y > -300){
        LevelUpBG->Draw();
        LevelUpBG->Position.y -= 2.5f;
    }

    if (ChestObtainBG && ChestObtainBG->Position.x > 0 && chestBGdelay > 0.0f){
        chestBGdelay -= 0.01f;
        ChestObtainBG->Draw();
        // cout << "chest bg delay before animation : " << chestBGdelay << endl;
    }
    if (ChestObtainBG && ChestObtainBG->Position.x > -400 && chestBGdelay <= 0.0f){
        ChestObtainBG->Draw();
        ChestObtainBG->Position.x -= 4.0f;
    } else if (ChestObtainBG->Position.x <= -400 && chestBGdelay <= 0.0f){
        chestBGdelay = 1.5f;
    }


    if (HP_BarBG) HP_BarBG->Draw();

    if (HP_BarFILL) {
        HP_BarFILL->Draw(); 
        HP_BarFILL->Size.x = 282 * (currentHP / maxHP); 
    }

    if (TXT_HPVal) {
        TXT_HPVal->Draw();
    }

    if (EXP_BarBG){
        EXP_BarBG->Draw();
    }
    if (EXP_BarFILL){
        EXP_BarFILL->Draw();
        EXP_BarFILL->Size.x = 282 * static_cast<float>(currentEXP) / maxEXP;
    }
    if (TXT_EXPVal){
        TXT_EXPVal->Draw();
    }


    // * Draw Inventory
    if (HotBarBG) {
        HotBarBG->Draw();
    }

    if (IMG_Potion && TXT_Potion){
        IMG_Potion->Draw(); TXT_Potion->Draw();
    }
    if (IMG_Missile && TXT_Missile){
        IMG_Missile->Draw(); TXT_Missile->Draw();
    }
    if (IMG_Shield && TXT_Shield){
        IMG_Shield->Draw(); TXT_Shield->Draw();
    }
    

    if (TXT_Interaction && canInteract) {
        TXT_Interaction->Draw();
    }
}

void PlayerCharacter::DestroyPlayerHUD(){
    delete HP_BarBG; delete HP_BarFILL; delete TXT_HPVal;
}

void PlayerCharacter::Update(float deltaTime) {
    UpdateCharacterDirection(deltaTime);
}

void PlayerCharacter::OverlapWithItem(ItemType itemType, int posY, int posX){
    if (itemType == ITEM_BLANK) return;
    switch (itemType){
        case ITEM_BLANK : return;
        case ITEM_MISSILE : 
            missile++;
            UpdateItemHotBar();
        break;
        case ITEM_POTION : 
            healthPotion ++;
            UpdateItemHotBar();
        break;

        case ITEM_SHIELD : 
            shield++;
            UpdateItemHotBar();
        break;
    };
}

PlayerCharacter::~PlayerCharacter(){
    DestroyPlayerHUD();
}

void PlayerCharacter::Draw() const {
    if (charSpriteObj) {
        charSpriteObj->Draw();
    }

    DrawPlayerHUD(); // CALL DRAW PLAYER HUD IN SCENE
}

void PlayerCharacter::UpdateCharacterDirection(float deltaTime) {
    bool moved = false;
    float newX = this->x;
    float newY = this->y;

    // Check for diagonal movement first to ensure correct priority
    if (keys[0] && keys[2]) { // W & A key
        newX -= this->speed * 0.6;
        newY -= this->speed * 0.6;
        this->directionFacing = DIRECTION_UP;
        if (CollisionCheck(newX, newY, DIRECTION_UP)) {
            this->x = newX;
            this->y = newY;
            moved = true;
        }
    } else if (keys[0] && keys[3]) { // W & D key
        newX += this->speed * 0.6;
        newY -= this->speed * 0.6;
        this->directionFacing = DIRECTION_UP;
        if (CollisionCheck(newX, newY, DIRECTION_UP)) {
            this->x = newX;
            this->y = newY;
            moved = true;
        }
    } else if (keys[1] && keys[2]) { // S & A key
        newX -= this->speed * 0.6;
        newY += this->speed * 0.6;
        this->directionFacing = DIRECTION_DOWN;
        if (CollisionCheck(newX, newY, DIRECTION_DOWN)) {
            this->x = newX;
            this->y = newY;
            moved = true;
        }
    } else if (keys[1] && keys[3]) { // S & D key
        newX += this->speed * 0.6;
        newY += this->speed * 0.6;
        this->directionFacing = DIRECTION_DOWN;
        if (CollisionCheck(newX, newY, DIRECTION_DOWN)) {
            this->x = newX;
            this->y = newY;
            moved = true;
        }
    }
    // Single Key Directions
    else if (keys[0]) { // W key
        newY -= this->speed;
        this->directionFacing = DIRECTION_UP;
        if (CollisionCheck(this->x, newY, DIRECTION_UP)) {
            this->y = newY;
            moved = true;
        }
    } else if (keys[1]) { // S key
        newY += this->speed;
        this->directionFacing = DIRECTION_DOWN;
        if (CollisionCheck(this->x, newY, DIRECTION_DOWN)) {
            this->y = newY;
            moved = true;
        }
    } else if (keys[2]) { // A key
        newX -= this->speed;
        this->directionFacing = DIRECTION_LEFT;
        if (CollisionCheck(newX, this->y, DIRECTION_LEFT)) {
            this->x = newX;
            moved = true;
        }
    } else if (keys[3]) { // D key
        newX += this->speed;
        this->directionFacing = DIRECTION_RIGHT;
        if (CollisionCheck(newX, this->y, DIRECTION_RIGHT)) {
            this->x = newX;
            moved = true;
        }
    }

    // Update Sprite Based on DIRECTION ENUM if character moved
    if (moved) {
        timeSinceLastFrame += deltaTime;
        if (timeSinceLastFrame >= frameTime) {
            timeSinceLastFrame = 0.0f;
            currentFrame[directionFacing] = (currentFrame[directionFacing] + 1) % walkSprites[directionFacing].size();
        }
        std::string charSpritePath = "char/" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "/" + walkSprites[directionFacing][currentFrame[directionFacing]];
        if (charSpriteObj) delete charSpriteObj;
        charSpriteObj = new Engine::Image(charSpritePath, x, y, size, size);
    }
}


bool PlayerCharacter::CollisionCheck(float newX, float newY, Enum_Direction dir) {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x - size;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y - size;

    GameSceneHall* currScene = dynamic_cast<GameSceneHall*>(Engine::GameEngine::GetInstance().GetScene("gamescene_hall"));
    std::list<IObject*> blocks = currScene->BlockGroup->GetObjects();

    if (newX < 0 || newX > w || newY < 0 || newY > h) {
        if (newX <= 0) newX = 0;
        if (newY <= 0) newY = 0;
        if (newX >= w) newX = w;
        if (newY >= h) newY = h;
        return false;
    }
    else {
        // check for every block position, using the collider helper function to detect collision
        Engine::Point player_hitbox_rect_a((size/2) - (collission_radius/2), (size/2) - (collission_radius/2));
        Engine::Point player_hitbox_rect_b((size/2) + (collission_radius/2), (size/2) + (collission_radius/2));

        Engine::Point curr_rect_a = Engine::Point(newX, newY) + player_hitbox_rect_a;
        Engine::Point curr_rect_b = Engine::Point(newX, newY) + player_hitbox_rect_b;

        for(auto& ite : blocks) {
            if(Engine::Collider::IsRectOverlap(curr_rect_a, curr_rect_b, ite->Position, ite->Position + ite->Size))
                return false;
        }
    }
    return true;
}

// * Make sure to include this in all scene OnkeyDown and OnKeyUp
void PlayerCharacter::SetMovementState(int keycode, bool keyDown){
    switch(keycode){
        case KEYBOARD_W:
            keys[0] = keyDown;
            break;
        case KEYBOARD_S:
            keys[1] = keyDown;
            break;
        case KEYBOARD_A:
            keys[2] = keyDown;
            break;
        case KEYBOARD_D:
            keys[3] = keyDown;
            break;
    }

    isMoving = keys[0] || keys[1] || keys[2] || keys[3];

    if (isMoving == false){
        // * set back to idle animation
        switch (directionFacing){
            case DIRECTION_UPLEFT:
            case DIRECTION_UPRIGHT:
            case DIRECTION_UP:
            if (charSpriteObj) delete charSpriteObj;
            charSpriteObj = new Engine::Image("char/" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "/" + idleSprites[1], x, y, size, size);
            break;

            case DIRECTION_DOWNLEFT:
            case DIRECTION_DOWNRIGHT:
            case DIRECTION_DOWN:
            if (charSpriteObj) delete charSpriteObj;
            charSpriteObj = new Engine::Image("char/" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "/" + idleSprites[0], x, y, size, size);
            break;

            case DIRECTION_LEFT:
            if (charSpriteObj) delete charSpriteObj;
            charSpriteObj = new Engine::Image("char/" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "/" + idleSprites[2], x, y, size, size);
            break;

            case DIRECTION_RIGHT:
            if (charSpriteObj) delete charSpriteObj;
            charSpriteObj = new Engine::Image("char/" + to_string(Engine::GameEngine::GetInstance().GetCurrentActivePlayer().avatarID + 1) + "/" + idleSprites[3], x, y, size, size);
            break;
        }
    }
}

Engine::Point PlayerCharacter::GetPlayerPositionAtMap(){
    return Engine::Point(round(x / 64), round(y / 64));
}

void PlayerCharacter::OnPlayerDead(){
    cout << "You Died\n";
    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("start-scene", 5.0f);
    // Engine::GameEngine::GetInstance().ChangeScene("loading-scene");
    // Change After Death Scene is created:
    Engine::GameEngine::GetInstance().ChangeScene("death-scene");
}

void PlayerCharacter::SetCurrentHP(float newVal, bool shouldClamp){
    if (shouldClamp == false){
        this->currentHP = newVal;
    } else {
        if (newVal < 0) newVal = 0;
        else if (newVal > maxHP) newVal = maxHP;
        this->currentHP = newVal;
    }

    // * Update Health Text
    if (TXT_HPVal){
        TXT_HPVal->Text = "HP: " + to_string(static_cast<int>(round(currentHP)));
    }

    if (this->currentHP <= 0){
        // ! Dead
        OnPlayerDead();
    }
}

// * Should be called from scenes.
void PlayerCharacter::CheckPointSave(std::vector<std::vector<ItemType>> itemData, std::vector<std::vector<BlockType>> blockData){
    // * we have to get the old playerdata from gameengine and set it back
    PlayerEntry entry = Engine::GameEngine::GetInstance().GetCurrentActivePlayer();
    entry.atkDMG = this->attackDamage;
    entry.currentHP = this->currentHP;
    entry.maxHP = this->maxHP;
    entry.lastScene = Engine::GameEngine::currentActiveScene;
    entry.money = this->money;
    entry.speed = this->speed;
    entry.currentEXP = this->currentEXP;
    entry.maxEXP = this->maxEXP;
    entry.playerLevel = this->playerLevel;
    entry.x = this->x;
    entry.y = this->y;

    entry.healthPotion = this->healthPotion;
    entry.missile = this->missile;
    entry.shield = this->shield;

    // * update the current active player
    Engine::GameEngine::GetInstance().SetCurrentActivePlayer(Engine::GameEngine::GetInstance().currentActivePlayerName, entry);
     
    // * Write to save file
    auto oldEntries = Engine::GameEngine::GetInstance().LoadProfileBasedSaving();
    Engine::GameEngine::GetInstance().WriteProfileBasedSaving(oldEntries, entry);

    SaveSceneItemBlockData(itemData, blockData);

    cout << "Checkpoint Reached, Player Data Saved!!!!\n";
}

void PlayerCharacter::AddEXP(int amount){
    currentEXP += amount;

    while (currentEXP >= maxEXP) {
        // Level up
        currentEXP -= maxEXP;
        maxEXP = static_cast<int>(maxEXP * 1.25);
        playerLevel++;

        // Update stats and UI for each level up
        attackDamage *= 1.1f;
        maxHP += 10;

        std::string expString = "LVL " + std::to_string(playerLevel) + " " + std::to_string(currentEXP) + "/" + std::to_string(maxEXP);
        TXT_EXPVal = new Engine::Label(expString, "pixel-font.ttf", 30, 20, 740, 255, 255, 255, 255, 0.0, 0.5);

        if (LevelUpBG) LevelUpBG->Position.y = 0;
    }

    // Update the final EXP string after all potential level ups
    std::string finalExpString = "LVL " + std::to_string(playerLevel) + " " + std::to_string(currentEXP) + "/" + std::to_string(maxEXP);
    TXT_EXPVal = new Engine::Label(finalExpString, "pixel-font.ttf", 30, 20, 740, 255, 255, 255, 255, 0.0, 0.5);
}

void PlayerCharacter::SaveSceneItemBlockData(std::vector<std::vector<ItemType>> itemData, std::vector<std::vector<BlockType>> blockData) {
    std::string filename = "resource/map/" + Engine::GameEngine::currentActivePlayerName + "_" + Engine::GameEngine::currentActiveScene + "_ItemData.txt";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << filename << " for writing." << std::endl;
        return;
    }

    // Initialize parseString with the correct dimensions
    std::vector<std::vector<std::string>> parseString(itemData.size(), std::vector<std::string>(itemData[0].size(), "0"));

    for (size_t i = 0; i < itemData.size(); ++i) {
        if (itemData[i].size() != blockData[i].size()) {
            std::cerr << "Error: itemData and blockData have different column counts at row " << i << "." << std::endl;
            return;
        }

        for (size_t j = 0; j < itemData[i].size(); ++j) {
            // Initialize with "0"
            parseString[i][j] = "0";
            
            // Parse item data
            if (itemData[i][j] == ITEM_POTION) {
                parseString[i][j] = '3';
            } else if (itemData[i][j] == ITEM_MISSILE) {
                parseString[i][j] = '4';
            } else if (itemData[i][j] == ITEM_SHIELD) {
                parseString[i][j] = '5';
            } else if (blockData[i][j] == BLANK) {
                parseString[i][j] = '0';
            } else if (blockData[i][j] == BLOCK_CHEST) {
                parseString[i][j] = '2';
            } else if (blockData[i][j] == BASE_BLOCK) {
                parseString[i][j] = '1';
            } else if (blockData[i][j] == CHEST_OPENED){
                parseString[i][j] = '6';
            } else if (blockData[i][j] == NPC_INSPECTOR){
                parseString[i][j] = '7';
            }

            file << parseString[i][j];
        }
        file << std::endl;
    }

    file.close();
    std::cout << "Item and block data saved to " << filename << std::endl;
}

void PlayerCharacter::UpdateItemHotBar(){
    if (IMG_Missile) delete IMG_Missile;
    IMG_Missile = new Engine::Image("play/missile.png", 697 + 69, 760, 64, 64);
    if (TXT_Missile) delete TXT_Missile;
    TXT_Missile = new Engine::Label("x" + to_string(this->missile), "pixel-font.ttf", 27, 756 + 69, 795, 35, 240, 35, 255, 1.0f);
    if (IMG_Potion) delete IMG_Potion;
    IMG_Potion = new Engine::Image("play/healthpotion.png", 697, 760, 64, 64);
    if (TXT_Potion) delete TXT_Potion;
    TXT_Potion = new Engine::Label("x" + to_string(this->healthPotion), "pixel-font.ttf", 27, 756, 795, 35, 240, 35, 255, 1.0f);
    if (IMG_Shield) delete IMG_Shield;
    IMG_Shield = new Engine::Image("play/shield.png", 697 + 138, 760, 64, 64);
    if (TXT_Shield) delete TXT_Shield;
    TXT_Shield = new Engine::Label("x" + to_string(this->shield), "pixel-font.ttf", 27, 756 + 138, 795, 35, 240, 35, 255, 1.0f);
}