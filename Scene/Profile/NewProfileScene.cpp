#include <functional>
#include <string>
#include <iostream>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
// NEW
#include "Scene/GameScene_Hall.hpp"
#include "Scene/Loading/LoadingScene.hpp"

using namespace std;
#include "NewProfileScene.hpp"



void NewProfileScene::Initialize() {
    
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
    AddNewObject(new Engine::Image("bg/ProfileScene_bg.png", 0 ,0 , 1600, 832, 0.0, 0.0));
    currDifficulty = 1;
    Engine::GameEngine::currentActivePlayerName = "PLAYER";
    CreateNewProfile();
}

void NewProfileScene::CreateNewProfile(){
    recordingPlayerName = false;
    IMG_AddNewPlayerBG = new Engine::Image("bg/createnewprofile_bg.png", 0, 0, 1600, 832, 0, 0);
    AddNewObject(IMG_AddNewPlayerBG);
    BTN_Back = new Engine::ImageButton("btn/back_normal.png", "btn/back_hover.png", 570, 105, 70, 70);
    BTN_Back->SetOnClickCallback(bind(&NewProfileScene::Profile_BackBtnClick, this));
    AddNewControlObject(BTN_Back);

    BTN_Done = new Engine::ImageButton("btn/btn_long.png", "btn/btn_long_hover.png", 696, 635, 214, 55);
    BTN_Done->SetOnClickCallback(bind(&NewProfileScene::Profile_DoneBtnClick, this));
    AddNewControlObject(BTN_Done);

    // * Draw Text Box
    NameTextBox = new Engine::ImageButton("btn/textbox.png", "btn/textbox_hover.png", 619, 422, 362, 49);
    NameTextBox->SetOnClickCallback(bind(&NewProfileScene::RecordPlayerName, this));
    AddNewControlObject(NameTextBox);
    NameText = new Engine::Label((!Engine::GameEngine::currentActivePlayerName.empty() ? Engine::GameEngine::currentActivePlayerName : "PLAYER"), "pixel-font.ttf", 24, 630, 437,  255, 255, 255, 255, 0, 0);
    DoneText = new Engine::Label("Done", "pixel-font.ttf", 32, 766, 650,  255, 255, 255, 255, 0, 0);
    AddNewObject(NameText);
    AddNewObject(DoneText);

    // * Draw Avatar Selection
    IMG_Avatar = new Engine::Image("avatar/avatar_1.png", 720, 218, 160, 160);
    AddNewObject(IMG_Avatar);
    AvatarRight = new Engine::ImageButton("btn/nextBtn_normal.png", "btn/nextBtn_hover.png", 895, 281, 36, 34);
    AddNewControlObject(AvatarRight);
    AvatarLeft = new Engine::ImageButton("btn/prevBtn_normal.png", "btn/prevBtn_hover.png", 672, 281, 36, 34);
    AddNewControlObject(AvatarLeft);
    AvatarRight->SetOnClickCallback(std::bind(&NewProfileScene::ChangeAvatar, this, 1));
    AvatarLeft->SetOnClickCallback(std::bind(&NewProfileScene::ChangeAvatar, this, -1));

    // * Draw Difficulty Selection
    DiffText = new Engine::Label("Normal", "pixel-font.ttf", 30, 800, 530,  255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(DiffText);
    DiffRight = new Engine::ImageButton("btn/nextBtn_normal.png", "btn/nextBtn_hover.png", 895, 510, 36, 34);
    AddNewControlObject(DiffRight);
    DiffRight->SetOnClickCallback(std::bind(&NewProfileScene::ChangeDifficulty, this, 1));
    DiffLeft = new Engine::ImageButton("btn/prevBtn_normal.png", "btn/prevBtn_hover.png", 672, 510, 36, 34);
    AddNewControlObject(DiffLeft);
    DiffLeft->SetOnClickCallback(std::bind(&NewProfileScene::ChangeDifficulty, this, -1));
    

}

void NewProfileScene::Profile_BackBtnClick(){
    Terminate();
    Engine::GameEngine::GetInstance().ChangeScene("profile-scene");
}

void NewProfileScene::Profile_DoneBtnClick(){

    recordingPlayerName = false;

    if (Engine::GameEngine::currentActivePlayerName.empty()) {
        Engine::GameEngine::currentActivePlayerName = "PLAYER";
        Engine::GameEngine::GetInstance().currentActivePlayerEntry.name = "PLAYER";
    }
   
    // Engine::GameEngine::GetInstance().WriteSaveFile({Engine::GameEngine::currentActivePlayerName, AudioHelper::BGMVolume, AudioHelper::SFXVolume});
    LoadingScene* loadingScene = dynamic_cast<LoadingScene*>(Engine::GameEngine::GetInstance().GetScene("loading-scene"));
    loadingScene->InitLoadingScreen("gamescene_hall", 2.5f);

    auto oldData = Engine::GameEngine::GetInstance().LoadProfileBasedSaving(); 
    PlayerEntry entry;
    entry.name = Engine::GameEngine::currentActivePlayerName;
    entry.difficulty = (currDifficulty == 0 ? "Easy" : currDifficulty == 1 ? "Normal" : currDifficulty == 2 ? "Hard" : "Normal");
    entry.avatarID = currAvatarID;
    Engine::GameEngine::GetInstance().WriteProfileBasedSaving(oldData, entry);
    Engine::GameEngine::GetInstance().SetCurrentActivePlayer(entry.name,  entry);
    if (bgmInstance){
        AudioHelper::StopSample(bgmInstance);
        bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    }
    Engine::GameEngine::GetInstance().ChangeScene("loading-scene");
}

void NewProfileScene::RecordPlayerName(){
        // cout << "Recording Player Name " << recordingPlayerName << endl;
        recordingPlayerName = true;
        RemoveObject(NameText->GetObjectIterator());
        Engine::GameEngine::currentActivePlayerName.clear();
        NameText = new Engine::Label(Engine::GameEngine::currentActivePlayerName, "pixel-font.ttf",  24, 630, 437,  255, 255, 255, 255, 0, 0);
        AddNewObject(NameText);
        cout << "Recording Player Name " << recordingPlayerName << endl;
}
void NewProfileScene::ChangeAvatar(int mode){
    if (IMG_Avatar) {
        if (mode == 1) {
            if (currAvatarID + 1 == 6) currAvatarID = -1;
            currAvatarID++;
            // std::cout << "currAvatarID: " << currAvatarID << "\n";
        } else if (mode == -1){
            if (currAvatarID - 1 < 0) currAvatarID = 6;
            currAvatarID--;
            // std::cout << "currAvatarID: " << currAvatarID << "\n";
        }
        RemoveObject(IMG_Avatar->GetObjectIterator());
        IMG_Avatar = new Engine::Image(avatarPath[currAvatarID], 720, 218, 160, 160);
        AddNewObject(IMG_Avatar);

    }
}
void NewProfileScene::ChangeDifficulty(int mode){ //  1 for inc, -1 for dec
    if (mode == 1){
        if (currDifficulty + 1 > 2) currDifficulty = -1;
        currDifficulty++;
        // std::cout << "curr: " << currDifficulty << "\n";
    } else if (mode == -1){
        if (currDifficulty - 1 < 0) currDifficulty = 3;
        currDifficulty--;
        // std::cout << "curr: " << currDifficulty << "\n";
    }

    RemoveObject(DiffText->GetObjectIterator());
    DiffText = new Engine::Label((currDifficulty == 0 ? "Easy" : currDifficulty == 1 ? "Normal" : currDifficulty == 2 ? "Hard" : "Normal"), "pixel-font.ttf", 30, 800, 530,  255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(DiffText);
}
// Record Player Functions:
void NewProfileScene::OnKeyDown(int keyCode){
    IScene::OnKeyDown(keyCode);
    if(keyCode == ALLEGRO_KEY_ESCAPE){
        Engine::GameEngine::GetInstance().ChangeScene("profile-scene");
    }
    if(recordingPlayerName){
        if (keyCode == 59 || keyCode == ALLEGRO_KEY_ENTER) { // * Escape
            recordingPlayerName = false;
            return;
        }
        if (keyCode == 63 && !Engine::GameEngine::currentActivePlayerName.empty()) { // * Backspace
            Engine::GameEngine::currentActivePlayerName.pop_back();
            RemoveObject(NameText->GetObjectIterator());
            NameText = new Engine::Label(Engine::GameEngine::currentActivePlayerName, "pixel-font.ttf",  24, 630, 437,  255, 255, 255, 255, 0, 0);
            AddNewObject(NameText);
            std::cout <<"name now: "<< Engine::GameEngine::currentActivePlayerName << "\n";
            return;
        }
        if (keyCode < 0 || keyCode > 36 || Engine::GameEngine::currentActivePlayerName.length() >= 12) return;
        else if(keyCode >= 0 && keyCode <= 36){
            std::cout <<"keycode supposed to appear!\n";
            std::cout <<"name now: "<< Engine::GameEngine::currentActivePlayerName << "\n";
            Engine::GameEngine::currentActivePlayerName += '@' + keyCode;
            RemoveObject(NameText->GetObjectIterator());
            NameText = new Engine::Label(Engine::GameEngine::currentActivePlayerName, "pixel-font.ttf",  24, 630, 437,  255, 255, 255, 255, 0, 0);
            AddNewObject(NameText);
        }
    }
}
void NewProfileScene::OnMouseDown(int button, int mx, int my){
    IScene::OnMouseDown(button, mx, my);
}
void NewProfileScene::Terminate() {
    // AudioHelper::StopBGM(bgmId);
	// AudioHelper::StopSample(deathBGMInstance);
	// deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
	cout << "Terminated_NewProfileScene!\n";
}

