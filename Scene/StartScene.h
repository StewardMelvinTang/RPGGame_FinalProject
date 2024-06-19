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
class StartScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
    std::string saveFileName = "Resource/save.txt";
    std::string oldName = "PLAYER";
    // std::string playerName = "Player A";
    IObject * TXT_Name;
    // * Profile Room
    IObject * TXT_Profile;
    IObject * IMG_ProfileBG;
    IObject * NameText, * DoneText;
    Engine::ImageButton * BTN_Back, * BTN_Done, * NameTextBox;

    explicit StartScene() = default;
    void Initialize() override;
    void Terminate() override;
    void VirtualDraw() const override;
    void PlayOnClick(int stage);
    void SettingsOnClick(int stage);

    void InitializeAudioEngine();

    void OnExitGameBtn();
    void CreditBtn();
    void ScoreBoardBtn();
    void ProfileBtn();

    void LoadSettings();
    void SaveSettings();

    void InitializeProfileMenu();
    void Profile_BackBtnClick();
    void Profile_DoneBtnClick();
    void RecordPlayerName();
    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int mx, int my) override;
};
#endif //INC_2024_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
