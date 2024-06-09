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

#include "PlayerCharacter.hpp"

// * Keyboard Shortcut Redefinition (for easier use)
#define KEYBOARD_W 23
#define KEYBOARD_S 19
#define KEYBOARD_A 1
#define KEYBOARD_D 4

bool keys[4] = {false, false, false, false}; // W, S, A, D (input holding 

PlayerCharacter::PlayerCharacter(float x, float y, float speed, float hp, int money) : Engine::Sprite("char/char_idle_down.png", x, y){
    this->x = x; this->y = y; // Set Position in screen
    this->speed = speed; this->gold = money;

}
void PlayerCharacter::Update(float deltaTime) {

}
void PlayerCharacter::Draw() const {
    if (charSpriteObj){
        charSpriteObj->Draw();
    }
}

void PlayerCharacter::UpdateCharacterDirection(){
    if (keys[0] && CollisionCheck()) { // W key
        this->y -= this->speed;
        this->directionFacing = DIRECTION_UP;
    }
    if (keys[1]&& CollisionCheck()) { // S key
        this->y += this->speed;
        this->directionFacing = DIRECTION_DOWN;
    }
    if (keys[2]&& CollisionCheck()) { // A key
        this->x -= this->speed;
        this->directionFacing = DIRECTION_LEFT;
    }
    if (keys[3]&& CollisionCheck()) { // D key
        this->x += this->speed;
        this->directionFacing = DIRECTION_RIGHT;
    }

    // * Update Sprite Based on DIRECTION ENUM
    string charSpritePath = "char/char_idle_down.png";
    switch (this->directionFacing){
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
    }
    charSpriteObj = new Engine::Image(charSpritePath, x, y, size, size);
}
bool PlayerCharacter::CollisionCheck(){
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    if (x < 0 || x > w || y < 0 || y > h) {
        if (x <= 0) x = 0;
        if (y <= 0) y = 0;
        if (x >= w) x = w;
        if (y >= h) y = h;
        return false;
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
}