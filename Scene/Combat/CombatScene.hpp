#ifndef COMBATSCENE_HPP
#define COMBATSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class CombatScene final : public Engine::IScene {
public:
    std::string nextScene = "";
    std::string prevScene = "";
    std::string backMenuLevel = "start-scene";
    std::string backGameHall = "gamescene_hall";
    
    // float player_hp;
    // float enemy_hp;

	explicit CombatScene() = default;
	void Initialize() override;
	void Terminate() override;
    void Update(float deltaTime) override;
    void InitLoadingScreen(std::string nextScene, float duration);
    // New Inits
    void BackOnClick();
    // Attack, Items, Escape
    void AttackOnClick();
    void ItemsOnClick();
    void EscapeOnClick();
	// SetPlayerHp
    void SetPlayerHP(float val);
    void UpdateHP();
    // keydown & up
    void OnKeyDown(int keyCode) override;
    // void OnKeyUp(int keyCode) override;
private:
    //bools
    bool playerturn;
    bool enemydead;
    bool displayitems;
    bool playerdead;
    //Player health values
    IObject * HP_BarBG = nullptr;
    IObject * HP_BarFILL = nullptr;
    Engine::Label * TXT_HPVal = nullptr;
    float currentHP = 100, maxHP = 100; // remove value when combat can be entered
    

    //Player attack values
    IObject * ATK_BarBG = nullptr;
    IObject * ATK_BarFILL = nullptr;
    Engine::Label * TXT_ATKVal = nullptr;
    float playerATK = 20, maxATK = 100;
    int ATK_FILL = 282 * (playerATK / maxATK);
};

#endif // LOSESCENE_HPP
