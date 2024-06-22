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
};

class Item {
public:
    Item(ItemName _name, ItemType _type, int _weight):
        name(_name),
        type(_type),
        weight(_weight)
    {}

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
        Item item6(FIREBALL, ATTACKING, 15);
        Item item7(FIREBALL, ATTACKING, 15);
        Item item8(FIREBALL, ATTACKING, 15);
        Item item9(FIREBALL, ATTACKING, 15);
        Item item10(FIREBALL, ATTACKING, 15);

        items.push_back(item1);
        items.push_back(item2);
        items.push_back(item3);
        items.push_back(item4);
        items.push_back(item5);
        items.push_back(item6);
        items.push_back(item7);
        items.push_back(item8);
        items.push_back(item9);
        items.push_back(item10);
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
        maxHp = 180;
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

    // eval the values based on the current remaining skills (only a rough estimation for now)
    int playerAttackNet = playerAttackCount - enemyDefendCount;
    int enemyAttackNet = enemyAttackCount - playerDefendCount;
    int playerHealNet = (player.maxHp - player.hp) * playerHealCount;
    int enemyHealNet = (enemy.maxHp - enemy.hp) * enemyHealCount;

    value += (playerAttackNet - enemyAttackNet) * 15; // 15 is the average skills damage
    value += (playerHealNet - enemyHealNet) * 10; // 10 is the average healing

    return value;
}

int search(int depth, const Player& player, const Enemy& enemy) {
    if(depth == 0) return evaluateScenarioValue(player, enemy);


}

int main() {
    Player player;
    Enemy enemy;
}