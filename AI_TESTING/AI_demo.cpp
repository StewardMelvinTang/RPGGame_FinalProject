#include <iostream>
#include <list>
using namespace std;

enum ItemType {
    ATTACKING,
    HEALING,
    DEFENDING
};

enum ItemName {
    FIREBALL,
    HEAL_POTION,
    FORCE_FIELD
}

struct Item {
    ItemName name;
    ItemType type;
    int weight; // weight will be used according to the item type (ex. if it's a healing type item, weight is the healing value)
};

class Player {
public:
    Player() {
        basicAttack = 10;
        maxHp = 100;
        hp = maxHp;
        
        Item item1(FIREBALL, ATTACKING, 15);
        Item item2(FIREBALL, ATTACKING, 15);
        Item item3(HEAL_POTION, HEALING, 20);
        Item item4(FORCE_FIELD, DEFENDING, 12);
        Item item5(FORCE_FIELD, DEFENDING, 12);

        items.push_back(item1);
        items.push_back(item2);
        items.push_back(item3);
        items.push_back(item4);
        items.push_back(item5);
    }

    list<Item> items;
    int basicAttack;
    int hp;
    int maxHp;
};

class Enemy {
public:
    Enemy() {
        basicAttack = 12;
        maxHp = 80;
        hp = maxHp;

        Item item1(FIREBALL, ATTACKING, 15);
        Item item2(FIREBALL, ATTACKING, 15);
        Item item3(FIREBALL, ATTACKING, 15);
        Item item4(HEAL_POTION, HEALING, 20);

        items.push_back(item1);
        items.push_back(item2);
        items.push_back(item3);
        items.push_back(item4);
    }

    list<Item> items;
    int basicAttack;
    int hp;
    int maxHp;
};

// evaluate the current position (represent them numerically)
int evaluateScenarioValue(const Player& player, const Enemy& enemy) {
    // positive: player is in advantage, negative: disadvantage
    int value = 0;

    // hp
    value += (player.hp - enemy.hp);

    // count player's items type
    int playerAttackCount = 0;
    int playerDefendCount = 0;
    int playerHealCount = 0;
    for(auto& ite : player.items) {
        if(ite.type == ATTACKING) playerAttackCount++;
        if(ite.type == DEFENDING) playerDefendCount++;
        if(ite.type == HEALING) playerHealCount++;
    }

    // count enemy's items type
    int enemyAttackCount = 0;
    int enemyDefendCount = 0;
    int enemyHealCount = 0;
    for(auto& ite : enemy.items) {
        if(ite.type == ATTACKING) enemyAttackCount++;
        if(ite.type == DEFENDING) enemyDefendCount++;
        if(ite.type == HEALING) enemyHealCount++;
    }

    // eval items (here we prioritize the number of attacking items left)
    value += (playerAttackCount - enemyAttackCount) * 10;


}

int main() {
    Player player;
    Enemy enemy;
}