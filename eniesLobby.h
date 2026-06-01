#ifndef ENIES_LOBBY_H
#define ENIES_LOBBY_H

#include "main.h"

class BattleContext;
class Building;
class Character;
struct TurnNode;

class Character
{
protected:
    string name;
    int hp;
    int maxHp;
    int atk;
    int def;
    int speed;
    int energy;
    bool alive;

public:
    bool killedThisTurn;
    bool isLowestHP_Enemies;

    Character();
    Character(string name, int hp, int atk, int def, int speed, int energy);
    virtual ~Character();

    virtual int attack(Character *target, BattleContext &context) = 0;
    virtual int specialSkill(Character *target, BattleContext &context) = 0;

    virtual int attack(Building *target, BattleContext &context);
    virtual int specialSkill(Building *target, BattleContext &context);

    virtual void endTurn(BattleContext &context);

    void receiveDamage(int damage);
    bool isAlive() const;
    string getName() const;
    int getHP() const;
    int getEnergy() const;

    int getMaxHP() const { return maxHp; }
    void setHP(int h)
    {
        hp = h;
        if (hp <= 0)
            alive = false;
    }
    void setAlive(bool a) { alive = a; }
    void setEnergy(int e) { energy = e; }
    int getAtk() const { return atk; }
    void setAtk(int a) { atk = a; }
    int getDef() const { return def; }
    void setDef(int d) { def = d; }
    int getSpeed() const { return speed; }
    void setSpeed(int s) { speed = s; }

    virtual bool isStrawHat() const;
    virtual bool isCP9() const;

    virtual string str() const = 0;
};

class StrawHat : public Character
{
protected:
    long long bounty;

public:
    StrawHat();
    StrawHat(string name, int hp, int atk, int def,
             int speed, int energy, long long bounty);

    virtual bool isStrawHat() const;
    virtual string str() const;
};

class Luffy : public StrawHat
{
public:
    Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Zoro : public StrawHat
{
public:
    Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Sanji : public StrawHat
{
public:
    Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Nami : public StrawHat
{
public:
    Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Chopper : public StrawHat
{
public:
    Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Usopp : public StrawHat
{
public:
    Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Franky : public StrawHat
{
public:
    Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    int attack(Building *target, BattleContext &context) override;
    int specialSkill(Building *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class CP9Agent : public Character
{
protected:
    int doriki;

public:
    CP9Agent();
    CP9Agent(string name, int hp, int atk, int def,
             int speed, int energy, int doriki);

    virtual bool isCP9() const;
    virtual string str() const;
};

class Lucci : public CP9Agent
{
public:
    Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Kaku : public CP9Agent
{
public:
    Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Jabra : public CP9Agent
{
public:
    Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Blueno : public CP9Agent
{
public:
    Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Kalifa : public CP9Agent
{
public:
    Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Kumadori : public CP9Agent
{
public:
    Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class Fukurou : public CP9Agent
{
public:
    Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki);
    int attack(Character *target, BattleContext &context) override;
    int specialSkill(Character *target, BattleContext &context) override;
    void endTurn(BattleContext &context) override;
};

class BattleContext
{
public:
    int turnCount;
    int morale;
    int alarmLevel;
    int rescueProgress;
    int escapeProgress;
    int busterCallTimer;
    bool mainGateDestroyed;
    bool robinRescued;
    bool bridgeOpened;
    bool battleEnded;
    string resultCode;

    BattleContext();
    void nextTurn();
};

class Building
{
protected:
    string name;
    int hp;
    int maxHP;
    bool destroyed;

public:
    Building(string name, int hp);
    virtual ~Building();

    void receiveDamage(int damage);
    bool isDestroyed() const;

    virtual void applyEffect(BattleContext &context) = 0;
    virtual void onDestroyed(BattleContext &context);

    virtual string str() const;
    string getName() const { return name; }
    int getHP() const { return hp; }
    int getMaxHP() const { return maxHP; }
    void setHP(int h)
    {
        hp = h;
        if (hp <= 0)
            destroyed = true;
    }
    void setDestroyed(bool d) { destroyed = d; }
};

class MainGate : public Building
{
public:
    MainGate(string name, int hp);
    void applyEffect(BattleContext &context) override;
    void onDestroyed(BattleContext &context) override;
};

class Courthouse : public Building
{
public:
    Courthouse(string name, int hp);
    void applyEffect(BattleContext &context) override;
    void onDestroyed(BattleContext &context) override;
};

class TowerOfJustice : public Building
{
public:
    TowerOfJustice(string name, int hp);
    void applyEffect(BattleContext &context) override;
};

class BridgeOfHesitation : public Building
{
public:
    BridgeOfHesitation(string name, int hp);
    void applyEffect(BattleContext &context) override;
};

class BusterCallShip : public Building
{
public:
    BusterCallShip(string name, int hp);
    void applyEffect(BattleContext &context) override;
    void onDestroyed(BattleContext &context) override;
};

struct TurnNode
{
    Character *data;
    TurnNode *next;
};

class EniesLobbyBattle
{
private:
    Character **strawHats;
    int strawHatCount;

    Character **cp9Agents;
    int cp9Count;

    Building **buildings;
    int buildingCount;

    TurnNode *turnOrder;
    BattleContext context;
    int maxTurns;

    Building *findBuilding(string name)
    {
        for (int i = 0; i < buildingCount; i++)
        {
            if (buildings[i]->getName() == name)
                return buildings[i];
        }
        return nullptr;
    }

public:
    EniesLobbyBattle(const string &filename);
    ~EniesLobbyBattle();

    void loadFromFile(const string &filename);
    void addStrawHat(Character *character);
    void addCP9Agent(Character *character);
    void addBuilding(Building *building);
    void buildTurnOrder();
    void runBattle();
    void processTurn(Character *character);
    void processBuildings();
    void checkEndCondition();

    string getResult() const;
};

#endif