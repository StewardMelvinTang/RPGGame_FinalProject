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

    int health_weight = 25;
    int missile_weight = 45;

    void UseHealth();
    void UseMissile();
    void UseShield();
    //SetEnemy
    void UpdateEnemyHP(int dmg);
public:
    //bools
    bool playerturn;
    bool enemydead;
    bool displayitems;
    bool playerdead;
    bool enemy1;
    bool enemy2;
    bool enemy3;//melvin
    bool enemy4;
    bool boss;
    bool isUsingHealth;
    bool isUsingMissile;
    bool IsUsingShield;
    bool isAuto;

    bool isBoss;
    float boss_healing_amount;
    float boss_missile_amount;
    float boss_shield_amount;

    float boss_maxHp;
    float boss_currentHp;

    float boss_attack_weight;
    float boss_healing_weight;
    float boss_missile_weight;

    void BossUseHealth();
    void BossUseMissile();
    void BossUseShield();
    
    const float delayDuration = 2.0f;
    bool isPlayerATK;
    //more bools
    bool Health_out;
    bool Missile_out;
    bool Shield_out;
    int temp;
    float temp2;
    int targethealth;
    float currDelay;

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
    
    //Difficulty
    //Easy
    float Enemy_ATK_Easy = Enemy_ATK * 0.7;
    float Enemy_maxATK_Easy = Enemy_maxATK * 0.7;
    float Enemy_currentHP_Easy = Enemy_currentHP;
    float Enemy_maxHP_Easy = Enemy_maxHP;
    //Hard
    float Enemy_ATK_Hard = Enemy_ATK * 1.7;
    float Enemy_maxATK_Hard = Enemy_maxATK * 1.7;
    float Enemy_currentHP_Hard = Enemy_currentHP * 1.5;
    float Enemy_maxHP_Hard = Enemy_maxHP * 1.5;
    
    
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


    Engine::Image * No_Healing = nullptr;
    Engine::Image * No_Shield = nullptr;
    Engine::Image * No_Missile = nullptr;
    //masd
    // * AI for base enemy
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
                if(a.move == ATTACK) return false;
                if(b.move == ATTACK) return true;

                // then we the next priority is attack item;
                if(a.move == USE_MISSILE) return false;
                if(b.move == USE_MISSILE) return true;

                // then shield item
                if(a.move == USE_SHIELD) return false;
                if(b.move == USE_SHIELD) return true;

                // then healing item
                if(a.move == USE_HEALING) return false;
                if(b.move == USE_HEALING) return true;
                else return true;
            }
            else return a.scenarioValue < b.scenarioValue;
        }
    };

    std::vector<State> generateMoves(const State& s, bool isInit);
    float evaluateScenarioValue(const State& s);
    std::string hashState(const State& s);
    Move search(int depth);

    // * AI for boss enemy
    struct BossState {
        float playerHp;
        float maxHp;
        float attackW;
        float shieldW;
        float healingW;
        float missileW;
        float shieldCount;
        float healingCount;
        float missileCount;
        bool isShieldActive;

        float enemyHp;
        float EmaxHp;
        float EattackW;
        float EshieldW;
        float EhealingW;
        float EmissileW;
        float EshieldCount;
        float EhealingCount;
        float EmissileCount;
        bool EisShieldActive;

        Move move;
        float scenarioValue;
    };

    void invertBossState(BossState& s) {
        std::swap(s.playerHp, s.enemyHp);
        std::swap(s.maxHp, s.EmaxHp);
        std::swap(s.attackW, s.EattackW);
        std::swap(s.shieldW, s.EshieldW);
        std::swap(s.healingW, s.EhealingW);
        std::swap(s.missileW, s.EmissileW);
        std::swap(s.shieldCount, s.EshieldCount);
        std::swap(s.healingCount, s.EhealingCount);
        std::swap(s.missileCount, s.EmissileCount);
        std::swap(s.isShieldActive, s.EisShieldActive);
    }

    void processMove(BossState& s, Move move);

    struct CompareBossScenarioValue {
        bool operator() (const BossState& a, const BossState& b) {
            if(a.scenarioValue == b.scenarioValue) {
                // we prioritize using basic attack to conserve item
                if(a.move == ATTACK) return false;
                if(b.move == ATTACK) return true;

                // then we the next priority is attack item;
                if(a.move == USE_MISSILE) return false;
                if(b.move == USE_MISSILE) return true;

                // then shield item
                if(a.move == USE_SHIELD) return false;
                if(b.move == USE_SHIELD) return true;

                // then healing item
                if(a.move == USE_HEALING) return false;
                if(b.move == USE_HEALING) return true;
                else return true;
            }
            else return a.scenarioValue < b.scenarioValue;
        }
    };

    std::vector<BossState> generateBossMoves(const BossState& s, bool isInit, int depth);
    std::string hashBossState(const BossState& s);
    Move bossSearch(int depth, BossState& init);
    float evaluateBossScenarioValue(const BossState& s);
    void processMove(BossState& s, Move move, bool isPlayer, float lastDMG);

    //Empty Function
    void Empty();
    void RemoveReplace();

    void CheckState();

    void VirtualDraw() const override;
    Engine::ImageButton* Extend = nullptr;
    Engine::ImageButton* Auto_btn = nullptr;
};

#endif // LOSESCENE_HPP
