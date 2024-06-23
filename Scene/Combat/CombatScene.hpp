#ifndef COMBATSCENE_HPP
#define COMBATSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
#include "PlayerCharacter/PlayerCharacter.hpp"
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
    // void BackOnClick();
    // Attack, Items, Escape
    void AttackOnClick();
    void ItemsOnClick();
    void EscapeOnClick();
    void AutoOnClick();

	// SetPlayerHp
    void SetPlayerHP(float val);
    void UpdateHP();

    // keydown & up
    void OnKeyDown(int keyCode) override;
    // void OnKeyUp(int keyCode) override;
    PlayerCharacter * playerChar_combat = nullptr;

    int health_weight = 15;
    int missile_weight = 35;

    void enemyATK();
    void UseHealth();
    void UseMissile();
    void UseShield();
    //SetEnemy
    void UpdateEnemyHP();
private:
    //bools
    bool playerturn;
    bool enemydead;
    bool displayitems;
    bool playerdead;
    bool enemy1;
    bool enemy2;
    bool enemy3;
    bool enemy4;
    bool boss;
    bool IsUsingShield;
    bool isAuto;

    //Player health values
    IObject * HP_BarBG = nullptr;
    IObject * HP_BarFILL = nullptr;
    Engine::Label * TXT_HPVal = nullptr;
    float currentHP, maxHP; // remove value when combat can be entered
    

    //Player attack values
    IObject * ATK_BarBG = nullptr;
    IObject * ATK_BarFILL = nullptr;
    Engine::Label * TXT_ATKVal = nullptr;
    float playerATK = 20, maxATK = 100;
    int ATK_FILL = 282 * (playerATK / maxATK);

    //<-----ENEMY VALUES:----->
    //Enemy health values
    IObject * Enemy_HP_BarBG = nullptr;
    IObject * Enemy_HP_BarFILL = nullptr;
    Engine::Label * Enemy_TXT_HPVal = nullptr;
    float Enemy_currentHP = 100, Enemy_maxHP = 100;

    //Enemy attack values
    IObject * Enemy_ATK_BarBG = nullptr;
    IObject * Enemy_ATK_BarFILL = nullptr;
    Engine::Label * Enemy_TXT_ATKVal = nullptr;
    float Enemy_ATK = 20, Enemy_maxATK = 100;
    int Enemy_ATK_FILL = 282 * (Enemy_ATK / Enemy_maxATK);


    //Sprites and Visuals
    IObject * Platform = nullptr;
    IObject * PlayerSprite = nullptr;
    IObject * EnemySprite = nullptr;

    //Items
    float Healing_Amount;
    float Missile_Amount;
    float Shield_Amount;
    Engine::ImageButton* Healing = nullptr;
    Engine::ImageButton* Missile = nullptr;
    Engine::ImageButton* Shield = nullptr;
    Engine::Label * Healing_num = nullptr;
    Engine::Label * Missile_num = nullptr;
    Engine::Label * Shield_num = nullptr;

    // * AI
    enum Move {
        USE_MISSILE,
        USE_HEALING,
        USE_SHIELD,
        ATTACK,
        ESCAPE
    };

    struct State {
        float playerHp;
        float shieldCount;
        float healingCount;
        float missileCount;
        float enemyHp;
        Move move;
        float scenarioValue;
    };

    struct CompareScenarioValue {
        bool operator() (const State& a, const State& b) {
            if(a.scenarioValue == b.scenarioValue) {
                // we prioritize using basic attack to conserve item
                if(b.move == ATTACK) return true;
                if(a.move == ATTACK) return false;

                // then we the next priority is attack item;
                if(b.move == USE_MISSILE) return true;
                if(a.move == USE_MISSILE) return false;

                // then shield item
                if(b.move == USE_SHIELD) return true;
                if(a.move == USE_SHIELD) return false;

                // then healing item
                if(b.move == USE_HEALING) return true;
                if(a.move == USE_HEALING) return false;
                else return false;
            }
            else return b.scenarioValue > a.scenarioValue;
        }
    };

    std::vector<State> generateMoves(const State& s, bool isInit);
    float evaluateScenarioValue(const State& s);
    std::string hashState(const State& s);
    Move search(int depth);

    //Empty Function
    void Empty();
    void RemoveReplace();

    void CheckState();

    void VirtualDraw() const override;
    Engine::ImageButton* Extend = nullptr;
    Engine::ImageButton* Auto_btn = nullptr;
};

#endif // LOSESCENE_HPP
