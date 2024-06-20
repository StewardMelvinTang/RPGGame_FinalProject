//
// Created by Hsuan on 2024/4/10.
//

#ifndef INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include <string>
#include <iostream>
#include "Engine/GameEngine.hpp"




class StartScene final : public Engine::IScene {

struct PlayerEntryComponents
{
    IObject * IMG_Avatar;
    IObject * PlayerName, * PlayerDifficulty, * PlayerMoney;
    Engine::ImageButton * PlayButton;
};

private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
    std::string saveFileName = "Resource/save.txt";
    std::string oldName = "PLAYER";
    // std::string playerName = "Player A";
    IObject * TXT_Name;
    // * Create New Player
    IObject * TXT_Profile;
    IObject * IMG_AddNewPlayerBG;
    IObject * NameText, * DoneText, *DiffText;
    IObject * IMG_Avatar;
    Engine::ImageButton * BTN_Back, * BTN_Done, * NameTextBox, * AvatarLeft, *AvatarRight, *DiffLeft, *DiffRight;
    
    // * Profile List Components
    Engine::ImageButton * BTN_BackPrfl, * BTN_CreateNewProfile;
    IObject * IMG_ProfileBG, * TXT_AddNew;
    std::vector<PlayerEntryComponents> playerEntryComps;
    void InitializeProfileListMenu();
    void DestroyProfileListMenu();

    int currAvatarID = 0;
    std::string avatarPath[6] = {"avatar/avatar_1.png", "avatar/avatar_2.png", "avatar/avatar_3.png", "avatar/avatar_4.png", "avatar/avatar_5.png", "avatar/avatar_6.png"};
    int currDifficulty = 1; // 0 = easy, 1 = normal, 2 = hard

    explicit StartScene() = default;
    void Initialize() override;
    void Terminate() override;
    void VirtualDraw() const override;
    void PlayOnClick();
    void SettingsOnClick(int stage);

    void InitializeAudioEngine();

    void OnExitGameBtn();
    void CreditBtn();
    void ScoreBoardBtn();
    void AddNewPlayerBtn();

    void LoadSettings();
    void SaveSettings();

    

    void InitializeCreatePlayerMenu();
    void Profile_BackBtnClick(int screen);
    void Profile_DoneBtnClick();
    void RecordPlayerName();
    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int mx, int my) override;
    void Profile_AddNewPlayerBtnClick();

    void ChangeAvatar(int mode);
    void ChangeDifficulty(int mode);

    void EmptyFunction() {}

    void PlayProfileBtn(std::vector<PlayerEntry> & entries, int id);
    
};
#endif //INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
