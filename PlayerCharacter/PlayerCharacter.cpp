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
#include "PlayerCharacter.hpp"

// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4

bool keys[4] = {false, false, false, false}; // W, S, A, D (input holding)
int w,h;

PlayerCharacter::PlayerCharacter(float x, float y, float speed, float hp, int money, int blockSize) : Engine::Sprite("char/char_idle_down.png", x, y){
    this->x = x; this->y = y; // Set Position in screen
    this->speed = speed; this->money = money;
    charSpriteObj = new Engine::Image("char/char_idle_down.png", x, y, size, size);
    

    w = Engine::GameEngine::GetInstance().GetScreenSize().x / blockSize;
    h = Engine::GameEngine::GetInstance().GetScreenSize().y / blockSize;

    ConstructPlayerHUD();
}

void PlayerCharacter::ConstructPlayerHUD(){
    // * Size 282 x 40, 282 * (maxHP / currHP) to track progressbar percent
    HP_BarBG = new Engine::Image("bg/progressbar_bg.png", 16, 770, 282, 40 );
    HP_BarFILL = new Engine::Image("bg/progressbar_fill.png", 16, 770, 282, 40);
    TXT_HPVal = new Engine::Label("HP: " + to_string(static_cast<int>(round(currentHP))), "pixel-font.ttf", 30, 20, 790, 255, 255, 255, 255, 0.0, 0.5);
}

void PlayerCharacter::DrawPlayerHUD() const{
    if (HP_BarBG) HP_BarBG->Draw();

    if (HP_BarFILL) {
        HP_BarFILL->Draw(); 
        HP_BarFILL->Size.x = 282 * (currentHP / maxHP); 
    }

    if (TXT_HPVal) {
        TXT_HPVal->Draw();
    }
}

void PlayerCharacter::DestroyPlayerHUD(){
    delete HP_BarBG; delete HP_BarFILL; delete TXT_HPVal;
}

void PlayerCharacter::Update(float deltaTime) {
    UpdateCharacterDirection();
    // cout << "Char Pos Y : " << GetPlayerPositionAtMap().y << " X : " << GetPlayerPositionAtMap().x << endl;
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

void PlayerCharacter::UpdateCharacterDirection() {
    bool moved = false;
    float newX = this->x;
    float newY = this->y;

    // Check for diagonal movement first to ensure correct priority
    if (keys[0] && keys[2]) { // W & A key
        newX -= this->speed * 0.6;
        newY -= this->speed * 0.6;
        if (CollisionCheck(newX, newY)) {
            this->x = newX;
            this->y = newY;
            this->directionFacing = DIRECTION_UPLEFT;
            moved = true;
        }
    }
    else if (keys[0] && keys[3]) { // W & D key
        newX += this->speed * 0.6;
        newY -= this->speed * 0.6;
        if (CollisionCheck(newX, newY)) {
            this->x = newX;
            this->y = newY;
            this->directionFacing = DIRECTION_UPRIGHT;
            moved = true;
        }
    }
    else if (keys[1] && keys[2]) { // S & A key
        newX -= this->speed * 0.6;
        newY += this->speed * 0.6;
        if (CollisionCheck(newX, newY)) {
            this->x = newX;
            this->y = newY;
            this->directionFacing = DIRECTION_DOWNLEFT;
            moved = true;
        }
    }
    else if (keys[1] && keys[3]) { // S & D key
        newX += this->speed * 0.6;
        newY += this->speed * 0.6;
        if (CollisionCheck(newX, newY)) {
            this->x = newX;
            this->y = newY;
            this->directionFacing = DIRECTION_DOWNRIGHT;
            moved = true;
        }
    }
    // Single Key Directions
    else if (keys[0]) { // W key
        newY -= this->speed;
        if (CollisionCheck(this->x, newY)) {
            this->y = newY;
            this->directionFacing = DIRECTION_UP;
            moved = true;
        }
    }
    else if (keys[1]) { // S key
        newY += this->speed;
        if (CollisionCheck(this->x, newY)) {
            this->y = newY;
            this->directionFacing = DIRECTION_DOWN;
            moved = true;
        }
    }
    else if (keys[2]) { // A key
        newX -= this->speed;
        if (CollisionCheck(newX, this->y)) {
            this->x = newX;
            this->directionFacing = DIRECTION_LEFT;
            moved = true;
        }
    }
    else if (keys[3]) { // D key
        newX += this->speed;
        if (CollisionCheck(newX, this->y)) {
            this->x = newX;
            this->directionFacing = DIRECTION_RIGHT;
            moved = true;
        }
    }

    // Update Sprite Based on DIRECTION ENUM if character moved
    if (moved) {
        std::string charSpritePath;
        switch (this->directionFacing) {
            case DIRECTION_DOWN:
                charSpritePath = "char/char_idle_down.png";
                break;
            case DIRECTION_UP:
                charSpritePath = "char/char_idle_up.png";
                break;
            case DIRECTION_RIGHT:
                charSpritePath = "char/char_idle_right.png";
                break;
            case DIRECTION_LEFT:
                charSpritePath = "char/char_idle_left.png";
                break;
            case DIRECTION_UPLEFT:
                charSpritePath = "char/char_idle_upleft.png";
                break;
            case DIRECTION_UPRIGHT:
                charSpritePath = "char/char_idle_upright.png";
                break;
            case DIRECTION_DOWNLEFT:
                charSpritePath = "char/char_idle_downleft.png";
                break;
            case DIRECTION_DOWNRIGHT:
                charSpritePath = "char/char_idle_downright.png";
                break;
        }
        if (charSpriteObj) delete charSpriteObj; 
        charSpriteObj = new Engine::Image(charSpritePath, x, y, size, size);
    }
}

bool PlayerCharacter::CollisionCheck(float newX, float newY) {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x - size;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y - size;
    if (newX < 0 || newX > w || newY < 0 || newY > h) {
        if (newX <= 0) newX = 0;
        if (newY <= 0) newY = 0;
        if (newX >= w) newX = w;
        if (newY >= h) newY = h;
        return false;
    }
    return true;
}

// * Make sure to include this in all scene OnKeyDown and OnKeyUp
void PlayerCharacter::SetMovementState(int keycode, bool keyDown) {
    switch(keycode) {
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
}

Engine::Point PlayerCharacter::GetPlayerPositionAtMap(){
    return Engine::Point(round(x / w), round(y / h));
}

void PlayerCharacter::OnPlayerDead(){
    cout << "You Died\n";
    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("start-scene", 5.0f);
    Engine::GameEngine::GetInstance().ChangeScene("loading-scene");
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