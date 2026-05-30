#include "eniesLobby.h"

void clampContext(BattleContext& ctx) {
    if (ctx.morale < 0) ctx.morale = 0; else if (ctx.morale > 100) ctx.morale = 100;
    if (ctx.alarmLevel < 0) ctx.alarmLevel = 0; else if (ctx.alarmLevel > 100) ctx.alarmLevel = 100;
    if (ctx.rescueProgress < 0) ctx.rescueProgress = 0; else if (ctx.rescueProgress > 100) ctx.rescueProgress = 100;
    if (ctx.escapeProgress < 0) ctx.escapeProgress = 0; else if (ctx.escapeProgress > 100) ctx.escapeProgress = 100;
    if (ctx.busterCallTimer < 0) ctx.busterCallTimer = 0;
}

void clampCharacter(Character* c) {
    if (c->getHP() < 0) { c->setHP(0); c->setAlive(false); }
    else if (c->getHP() > c->getMaxHP()) c->setHP(c->getMaxHP());
    if (c->getEnergy() < 0) c->setEnergy(0); else if (c->getEnergy() > 100) c->setEnergy(100);
}

void clampBuilding(Building* b) {
    if (b->getHP() < 0) { b->setHP(0); b->setDestroyed(true); }
    else if (b->getHP() > b->getMaxHP()) b->setHP(b->getMaxHP());
}

int applyDamage(Character* attacker, Character* target, int raw_damage, BattleContext& context, int customMorale = -999) {
    if (!target->isAlive()) return 0;
    
    int actual_damage = raw_damage - target->getDef();
    if (actual_damage < 0) actual_damage = 0;
    
    int hp_before = target->getHP();
    target->receiveDamage(raw_damage); 
    
    if (hp_before > 0 && target->getHP() == 0) {
        attacker->killedThisTurn = true;
        if (customMorale != -999) {
            context.morale += customMorale;
        } else {
            if (attacker->isStrawHat()) context.morale += 5;
            else if (attacker->isCP9()) context.morale -= 5;
        }
    }
    
    clampContext(context);
    clampCharacter(target);
    return actual_damage;
}

int applyDamageBuilding(Character* attacker, Building* target, int raw_damage, BattleContext& context) {
    if (target->isDestroyed()) return 0;
    
    int actual_damage = raw_damage;
    int hp_before = target->getHP();
    target->receiveDamage(raw_damage);
    
    if (hp_before > 0 && target->isDestroyed()) {
        target->onDestroyed(context);
    }
    clampBuilding(target);
    return actual_damage;
}

/* BattleContext */
BattleContext::BattleContext() {
    turnCount = 0; morale = 0; alarmLevel = 0; rescueProgress = 0; escapeProgress = 0; busterCallTimer = 0;
    mainGateDestroyed = false; robinRescued = false; bridgeOpened = false; battleEnded = false; resultCode = "";
}
void BattleContext::nextTurn() { turnCount++; }

/* Character */
Character::Character() {
    name = ""; hp = 0; maxHp = 0; atk = 0; def = 0; speed = 0; energy = 0; alive = false;
    killedThisTurn = false; isLowestHP_Enemies = false;
}
Character::Character(string name, int hp, int atk, int def, int speed, int energy) {
    this->name = name; this->hp = hp; this->maxHp = hp; this->atk = atk;
    this->def = def; this->speed = speed; this->energy = energy;
    this->alive = (hp > 0);
    this->killedThisTurn = false; this->isLowestHP_Enemies = false;
}
Character::~Character() {}
int Character::attack(Building* target, BattleContext& context) { return 0; }
int Character::specialSkill(Building* target, BattleContext& context) { return 0; }
void Character::endTurn(BattleContext& context) { }

void Character::receiveDamage(int damage) {
    int actual_damage = damage - def;
    if (actual_damage < 0) actual_damage = 0;
    hp -= actual_damage;
    if (hp <= 0) { hp = 0; alive = false; }
}

bool Character::isAlive() const { return alive; }
string Character::getName() const { return name; }
int Character::getHP() const { return hp; }
int Character::getEnergy() const { return energy; }
bool Character::isStrawHat() const { return false; }
bool Character::isCP9() const { return false; }

/* StrawHat */
StrawHat::StrawHat() : Character() { bounty = 0; }
StrawHat::StrawHat(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : Character(name, hp, atk, def, speed, energy) { this->bounty = bounty; }
bool StrawHat::isStrawHat() const { return true; }
string StrawHat::str() const {
    ostringstream oss;
    oss << "StrawHat[name=" << name << ", hp=" << hp << ", atk=" << atk 
        << ", def=" << def << ", speed=" << speed << ", energy=" << energy 
        << ", bounty=" << bounty << "]";
    return oss.str();
}

/* Luffy */
Luffy::Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Luffy::attack(Character* target, BattleContext& context) {
    int dmg = atk;
    if (hp > ceil(0.3 * maxHp) && hp <= ceil(0.5 * maxHp)) dmg = ceil(atk * 1.15);
    else if (hp <= ceil(0.3 * maxHp)) dmg = ceil(atk * 1.30);
    return applyDamage(this, target, dmg, context);
}
int Luffy::specialSkill(Character* target, BattleContext& context) {
    energy -= 20;
    int dmg = atk * 2;
    speed += 15; atk += 15; context.alarmLevel += 10;
    hp -= ceil(0.08 * maxHp);
    if(hp <= 0) { hp = 0; alive = false; }
    int actual = applyDamage(this, target, dmg, context);
    clampCharacter(this); clampContext(context);
    return actual;
}
int Luffy::attack(Building* target, BattleContext& context) {
    int dmg = atk;
    if (hp > ceil(0.3 * maxHp) && hp <= ceil(0.5 * maxHp)) dmg = ceil(atk * 1.15);
    else if (hp <= ceil(0.3 * maxHp)) dmg = ceil(atk * 1.30);
    return applyDamageBuilding(this, target, dmg, context);
}
int Luffy::specialSkill(Building* target, BattleContext& context) {
    energy -= 20;
    int dmg = atk * 2;
    speed += 15; atk += 15; context.alarmLevel += 10;
    hp -= ceil(0.08 * maxHp);
    if(hp <= 0) { hp = 0; alive = false; }
    int actual = applyDamageBuilding(this, target, dmg, context);
    clampCharacter(this); clampContext(context);
    return actual;
}
void Luffy::endTurn(BattleContext& context) {
    if (hp <= ceil(0.3 * maxHp)) context.morale += 3;
    if (killedThisTurn) energy += 5;
    clampContext(context); clampCharacter(this);
}

/* Zoro */
Zoro::Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Zoro::attack(Character* target, BattleContext& context) {
    int dmg = atk + ceil(0.2 * def);
    if (target->getHP() < ceil(0.4 * target->getMaxHP())) dmg = ceil(dmg * 1.15);
    return applyDamage(this, target, dmg, context);
}
int Zoro::specialSkill(Character* target, BattleContext& context) {
    energy -= 15;
    int dmg = ceil(atk * 2.2);
    if (target->getHP() < ceil(0.5 * target->getMaxHP())) dmg = ceil(dmg * 1.5);
    return applyDamage(this, target, dmg, context, 4);
}
int Zoro::attack(Building* target, BattleContext& context) {
    int dmg = atk + ceil(0.2 * def);
    return applyDamageBuilding(this, target, dmg, context);
}
int Zoro::specialSkill(Building* target, BattleContext& context) {
    energy -= 15;
    int dmg = ceil(atk * 2.2);
    if (target->getHP() < ceil(0.5 * target->getMaxHP())) dmg = ceil(dmg * 1.5);
    return applyDamageBuilding(this, target, dmg, context);
}
void Zoro::endTurn(BattleContext& context) {
    if (killedThisTurn) { context.morale += 6; atk += ceil(atk * 0.05); clampContext(context); }
}

/* Sanji */
Sanji::Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Sanji::attack(Character* target, BattleContext& context) {
    int dmg = atk + ceil(0.5 * speed);
    if (target->getDef() < def) dmg = ceil(dmg * 1.10);
    return applyDamage(this, target, dmg, context);
}
int Sanji::specialSkill(Character* target, BattleContext& context) {
    energy -= 18;
    int dmg = ceil(atk * 2.1);
    int actual = applyDamage(this, target, dmg, context);
    target->setDef(target->getDef() - (target->getName() == "Jabra" ? 12 : 8));
    if(target->getDef() < 0) target->setDef(0);
    return actual;
}
int Sanji::attack(Building* target, BattleContext& context) {
    int dmg = atk + ceil(0.5 * speed);
    return applyDamageBuilding(this, target, dmg, context);
}
int Sanji::specialSkill(Building* target, BattleContext& context) {
    energy -= 18;
    int dmg = ceil(atk * 2.1);
    return applyDamageBuilding(this, target, dmg, context);
}
void Sanji::endTurn(BattleContext& context) {
    if (killedThisTurn) { context.morale += 8; atk += ceil(atk * 0.10); clampContext(context); }
}

/* Nami */
Nami::Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Nami::attack(Character* target, BattleContext& context) {
    int originalDef = target->getDef();
    target->setDef(ceil(originalDef * 0.7)); 
    int actual = applyDamage(this, target, atk, context);
    target->setDef(originalDef);
    return actual;
}
int Nami::specialSkill(Character* target, BattleContext& context) {
    energy -= 20;
    int dmg = atk + 40;
    target->setSpeed(target->getSpeed() - 10);
    if(target->getSpeed() < 0) target->setSpeed(0);
    context.busterCallTimer += 1; context.alarmLevel -= 5;
    clampContext(context);
    return applyDamage(this, target, dmg, context);
}
int Nami::attack(Building* target, BattleContext& context) {
    return applyDamageBuilding(this, target, ceil(atk * 0.5), context);
}
int Nami::specialSkill(Building* target, BattleContext& context) {
    energy -= 20;
    int dmg = ceil((atk + 40) * 1.5);
    context.busterCallTimer += 1; context.alarmLevel -= 5;
    clampContext(context);
    return applyDamageBuilding(this, target, dmg, context);
}
void Nami::endTurn(BattleContext& context) {
    if (killedThisTurn) { energy += 6; clampCharacter(this); }
}

/* Chopper */
Chopper::Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Chopper::attack(Character* target, BattleContext& context) {
    return applyDamage(this, target, atk, context);
}
int Chopper::specialSkill(Character* target, BattleContext& context) {
    energy -= 15;
    int heal = 35 + ceil(0.5 * atk);
    target->setHP(target->getHP() + heal);
    clampCharacter(target);
    if (target->getName() == "Luffy") { context.morale += 5; clampContext(context); }
    return 0; 
}
int Chopper::attack(Building* target, BattleContext& context) {
    return applyDamageBuilding(this, target, atk, context);
}
int Chopper::specialSkill(Building* target, BattleContext& context) {
    energy -= 15;
    return 0;
}
void Chopper::endTurn(BattleContext& context) {}

/* Usopp */
Usopp::Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Usopp::attack(Character* target, BattleContext& context) {
    int dmg = atk;
    if (target->getSpeed() < 50) dmg = ceil(dmg * 1.2);
    return applyDamage(this, target, dmg, context);
}
int Usopp::specialSkill(Character* target, BattleContext& context) {
    energy -= 16;
    int dmg = ceil(atk * 0.8);
    target->setSpeed(target->getSpeed() - 12);
    if(target->getSpeed() < 0) target->setSpeed(0);
    context.escapeProgress += 8;
    clampContext(context);
    return applyDamage(this, target, dmg, context);
}
int Usopp::attack(Building* target, BattleContext& context) { 
    return applyDamageBuilding(this, target, ceil(atk * 0.5), context); 
}
int Usopp::specialSkill(Building* target, BattleContext& context) {
    energy -= 16;
    int dmg = ceil(atk * 0.8);
    context.escapeProgress += 8;
    clampContext(context);
    return applyDamageBuilding(this, target, dmg, context);
}
void Usopp::endTurn(BattleContext& context) {
    context.morale += 10; clampContext(context);
}

/* Franky */
Franky::Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}
int Franky::attack(Character* target, BattleContext& context) {
    int dmg = atk + ceil(0.3 * def);
    if (target->isCP9()) dmg = ceil(dmg * 1.1);
    return applyDamage(this, target, dmg, context);
}
int Franky::specialSkill(Character* target, BattleContext& context) {
    if (energy >= 30) {
        energy -= 30;
        int dmg = ceil(atk * 1.2);
        return applyDamage(this, target, dmg, context);
    } else {
        energy -= 20;
        int dmg = ceil(atk * 1.8);
        target->setSpeed(target->getSpeed() - 8);
        if(target->getSpeed() < 0) target->setSpeed(0);
        if (target->getName() == "Lucci") dmg = ceil(dmg * 1.2);
        return applyDamage(this, target, dmg, context);
    }
}
int Franky::attack(Building* target, BattleContext& context) {
    int dmg = atk + ceil(0.3 * def);
    return applyDamageBuilding(this, target, dmg, context);
}
int Franky::specialSkill(Building* target, BattleContext& context) {
    if (energy >= 30) {
        energy -= 30;
        int hp_before = target->getHP();
        applyDamageBuilding(this, target, target->getHP(), context); 
        return hp_before;
    } else {
        energy -= 20;
        int dmg = ceil(atk * 1.8);
        return applyDamageBuilding(this, target, dmg, context);
    }
}
void Franky::endTurn(BattleContext& context) {
    if (hp > ceil(0.7 * maxHp)) def += 5;
    if (hp < ceil(0.3 * maxHp)) atk += ceil(atk * 0.1);
}

/* CP9Agent */
CP9Agent::CP9Agent() : Character() { doriki = 0; }
CP9Agent::CP9Agent(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : Character(name, hp, atk, def, speed, energy) { this->doriki = doriki; }
bool CP9Agent::isCP9() const { return true; }
string CP9Agent::str() const {
    ostringstream oss;
    oss << "CP9[name=" << name << ", hp=" << hp << ", atk=" << atk 
        << ", def=" << def << ", speed=" << speed << ", energy=" << energy 
        << ", doriki=" << doriki << "]";
    return oss.str();
}

/* Lucci */
Lucci::Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Lucci::attack(Character* target, BattleContext& context) {
    int dmg = atk + ceil(doriki / 20.0);
    if (target->getHP() < ceil(0.5 * target->getMaxHP())) dmg = ceil(dmg * 1.2);
    return applyDamage(this, target, dmg, context);
}
int Lucci::specialSkill(Character* target, BattleContext& context) {
    energy -= 25;
    int dmg = ceil(atk * 2.8);
    int originalDef = target->getDef();
    target->setDef(ceil(originalDef * 0.5)); 
    int actual = applyDamage(this, target, dmg, context, -10);
    target->setDef(originalDef);
    return actual;
}
void Lucci::endTurn(BattleContext& context) {
    if (hp < ceil(0.4 * maxHp)) atk += ceil(atk * 0.05);
}

/* Kaku */
Kaku::Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Kaku::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Kaku::specialSkill(Character* target, BattleContext& context) {
    energy -= 20;
    int total = 0;
    total += applyDamage(this, target, ceil(atk * 1.2), context);
    if (target->isAlive()) total += applyDamage(this, target, atk, context);
    if (target->isAlive()) total += applyDamage(this, target, ceil(atk * 0.8), context);
    return total;
}
void Kaku::endTurn(BattleContext& context) {}

/* Jabra */
Jabra::Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Jabra::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Jabra::specialSkill(Character* target, BattleContext& context) {
    energy -= 18;
    int dmg = ceil(atk * 1.5);
    if (hp < ceil(0.3 * maxHp)) dmg = ceil(dmg * 1.25);
    return applyDamage(this, target, dmg, context, -5);
}
void Jabra::endTurn(BattleContext& context) {}

/* Blueno */
Blueno::Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Blueno::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Blueno::specialSkill(Character* target, BattleContext& context) {
    energy -= 15;
    int dmg = ceil(atk * 1.3);
    if (hp > ceil(0.5 * maxHp)) dmg += 20; else dmg += 40;
    return applyDamage(this, target, dmg, context);
}
void Blueno::endTurn(BattleContext& context) {}

/* Kalifa */
Kalifa::Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Kalifa::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Kalifa::specialSkill(Character* target, BattleContext& context) {
    energy -= 18;
    int dmg = ceil(atk * 1.4);
    context.morale -= (target->getName() == "Nami" ? 12 : 8);
    target->setSpeed(target->getSpeed() - 6);
    if(target->getSpeed() < 0) target->setSpeed(0);
    return applyDamage(this, target, dmg, context);
}
void Kalifa::endTurn(BattleContext& context) {}

/* Kumadori */
Kumadori::Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Kumadori::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Kumadori::specialSkill(Character* target, BattleContext& context) {
    energy -= 16;
    int dmg = 30 + ceil(doriki / 10.0);
    if (hp < ceil(0.4 * maxHp)) dmg += 25;
    return applyDamage(this, target, dmg, context);
}
void Kumadori::endTurn(BattleContext& context) {}

/* Fukurou */
Fukurou::Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}
int Fukurou::attack(Character* target, BattleContext& context) { return applyDamage(this, target, atk, context); }
int Fukurou::specialSkill(Character* target, BattleContext& context) {
    energy -= 14;
    int dmg = ceil(atk * 1.3);
    if (target->isLowestHP_Enemies) dmg += 20;
    return applyDamage(this, target, dmg, context, -6);
}
void Fukurou::endTurn(BattleContext& context) {}

/* Building */
Building::Building(string name, int hp) {
    this->name = name; this->hp = hp; this->maxHP = hp; this->destroyed = (hp <= 0);
}
Building::~Building() {}
void Building::receiveDamage(int damage) { hp -= damage; if (hp <= 0) { hp = 0; destroyed = true; } }
bool Building::isDestroyed() const { return destroyed; }
void Building::onDestroyed(BattleContext& context) { }
string Building::str () const {
    ostringstream oss;
    oss << "Building[name=" << name << ", hp=" << hp << ", maxHP=" << maxHP 
        << ", destroyed=" << (destroyed ? "true" : "false") << "]";
    return oss.str();
}

/* Building subclasses */
MainGate::MainGate(string name, int hp) : Building(name, hp) {}
void MainGate::applyEffect(BattleContext& context) {}
void MainGate::onDestroyed(BattleContext& context) {
    context.mainGateDestroyed = true;
    context.rescueProgress += 20; context.morale += 5;
    clampContext(context);
}

Courthouse::Courthouse(string name, int hp) : Building(name, hp) {}
void Courthouse::applyEffect(BattleContext& context) {
    if (!destroyed) { context.alarmLevel += 5; clampContext(context); }
}
void Courthouse::onDestroyed(BattleContext& context) {
    context.alarmLevel -= 20; clampContext(context);
}

TowerOfJustice::TowerOfJustice(string name, int hp) : Building(name, hp) {}
void TowerOfJustice::applyEffect(BattleContext& context) {
    if (!destroyed && context.mainGateDestroyed && !context.robinRescued) {
        context.rescueProgress += 5;
        if (context.rescueProgress >= 100) { context.robinRescued = true; context.morale += 10; }
        clampContext(context);
    }
}

BridgeOfHesitation::BridgeOfHesitation(string name, int hp) : Building(name, hp) {}
void BridgeOfHesitation::applyEffect(BattleContext& context) {
    if (!destroyed && context.robinRescued) {
        context.bridgeOpened = true;
        context.escapeProgress += 5;
        if (context.escapeProgress >= 100) { context.battleEnded = true; context.resultCode = "STRAW_HAT_WIN"; }
        clampContext(context);
    }
}

BusterCallShip::BusterCallShip(string name, int hp) : Building(name, hp) {}
void BusterCallShip::applyEffect(BattleContext& context) {
    if (!destroyed) {
        context.busterCallTimer -= 1;
        if (context.busterCallTimer <= 0) { context.battleEnded = true; context.resultCode = "BUSTER_CALL"; }
        clampContext(context);
    }
}
void BusterCallShip::onDestroyed(BattleContext& context) {
    context.busterCallTimer += 3; clampContext(context);
}

/* EniesLobbyBattle */
EniesLobbyBattle::EniesLobbyBattle(const string& filename) {
    strawHats = new Character*[7]; strawHatCount = 0;
    cp9Agents = new Character*[7]; cp9Count = 0;
    buildings = new Building*[5]; buildingCount = 0;
    turnOrder = nullptr;
    loadFromFile(filename);
}

EniesLobbyBattle::~EniesLobbyBattle() {
    for (int i = 0; i < strawHatCount; i++) delete strawHats[i];
    delete[] strawHats;
    for (int i = 0; i < cp9Count; i++) delete cp9Agents[i];
    delete[] cp9Agents;
    for (int i = 0; i < buildingCount; i++) delete buildings[i];
    delete[] buildings;

    while (turnOrder) {
        TurnNode* tmp = turnOrder;
        turnOrder = turnOrder->next;
        delete tmp;
    }
}

void EniesLobbyBattle::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return;
    string type;
    while (file >> type) {
        if (type == "CONTEXT") {
            file >> context.morale >> context.alarmLevel >> context.rescueProgress 
                 >> context.escapeProgress >> context.busterCallTimer >> maxTurns;
        } else if (type == "STRAW_HAT") {
            string name; int hp, atk, def, speed, energy; long long bounty;
            file >> name >> hp >> atk >> def >> speed >> energy >> bounty;
            Character* c = nullptr;
            if (name == "Luffy") c = new Luffy(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Zoro") c = new Zoro(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Sanji") c = new Sanji(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Nami") c = new Nami(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Chopper") c = new Chopper(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Usopp") c = new Usopp(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Franky") c = new Franky(name, hp, atk, def, speed, energy, bounty);
            if (c) addStrawHat(c);
        } else if (type == "CP9") {
            string name; int hp, atk, def, speed, energy, doriki;
            file >> name >> hp >> atk >> def >> speed >> energy >> doriki;
            Character* c = nullptr;
            if (name == "Lucci") c = new Lucci(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kaku") c = new Kaku(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Jabra") c = new Jabra(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Blueno") c = new Blueno(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kalifa") c = new Kalifa(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kumadori") c = new Kumadori(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Fukurou") c = new Fukurou(name, hp, atk, def, speed, energy, doriki);
            if (c) addCP9Agent(c);
        } else if (type == "BUILDING") {
            string name; int hp;
            file >> name >> hp;
            Building* b = nullptr;
            if (name == "MainGate") b = new MainGate(name, hp);
            else if (name == "Courthouse") b = new Courthouse(name, hp);
            else if (name == "TowerOfJustice") b = new TowerOfJustice(name, hp);
            else if (name == "BridgeOfHesitation") b = new BridgeOfHesitation(name, hp);
            else if (name == "BusterCallShip") b = new BusterCallShip(name, hp);
            if (b) addBuilding(b);
        }
    }
    buildTurnOrder();
}

void EniesLobbyBattle::addStrawHat(Character* character) { if (strawHatCount < 7) strawHats[strawHatCount++] = character; }
void EniesLobbyBattle::addCP9Agent(Character* character) { if (cp9Count < 7) cp9Agents[cp9Count++] = character; }
void EniesLobbyBattle::addBuilding(Building* building) { if (buildingCount < 5) buildings[buildingCount++] = building; }

void EniesLobbyBattle::buildTurnOrder() {
    TurnNode* tail = nullptr;
    for (int i = 0; i < strawHatCount; i++) {
        TurnNode* node = new TurnNode{strawHats[i], nullptr};
        if (!turnOrder) turnOrder = node; else tail->next = node;
        tail = node;
    }
    for (int i = 0; i < cp9Count; i++) {
        TurnNode* node = new TurnNode{cp9Agents[i], nullptr};
        if (!turnOrder) turnOrder = node; else tail->next = node;
        tail = node;
    }
}

void EniesLobbyBattle::runBattle() {
    while (!context.battleEnded && context.turnCount < maxTurns) {
        if (!turnOrder) break;
        
        TurnNode* current = turnOrder;
        turnOrder = turnOrder->next;
        current->next = nullptr;
        
        // Loại bỏ các nhân vật đã bị hạ (skip hoàn toàn turn)
        if (!current->data->isAlive()) {
            delete current;
            continue;
        }
        
        processTurn(current->data);
        
        if (!turnOrder) turnOrder = current;
        else {
            TurnNode* temp = turnOrder;
            while (temp->next) temp = temp->next;
            temp->next = current;
        }
        
        processBuildings();
        context.nextTurn();
        checkEndCondition();
    }
    if (!context.battleEnded && context.turnCount >= maxTurns) {
        context.battleEnded = true; context.resultCode = "TIME_OUT";
    }
}

void EniesLobbyBattle::processTurn(Character* character) {
    character->killedThisTurn = false;
    Character* targetCharacter = nullptr;
    Building* targetBuilding = nullptr;
    bool isHealing = false;

    int minSHHp = 999999;
    for(int i = 0; i < strawHatCount; i++) {
        if(strawHats[i]->isAlive() && strawHats[i]->getHP() < minSHHp) minSHHp = strawHats[i]->getHP();
    }
    for(int i = 0; i < strawHatCount; i++) {
        strawHats[i]->isLowestHP_Enemies = (strawHats[i]->isAlive() && strawHats[i]->getHP() == minSHHp);
    }
    
    Character* firstCP9 = nullptr;
    for (int i = 0; i < cp9Count; i++) {
        if (cp9Agents[i]->isAlive()) { firstCP9 = cp9Agents[i]; break; }
    }
    
    if (character->isStrawHat()) {
        if (character->getName() == "Chopper" && character->getEnergy() >= 15) {
            for (int i = 0; i < strawHatCount; i++) {
                if (strawHats[i]->isAlive() && strawHats[i]->getHP() == minSHHp) {
                    targetCharacter = strawHats[i];
                    break;
                }
            }
            isHealing = true;
        } else {
            Building* mg = findBuilding("MainGate");
            Building* ch = findBuilding("Courthouse");
            Building* bcs = findBuilding("BusterCallShip");
            Building* boh = findBuilding("BridgeOfHesitation");

            if (mg && !mg->isDestroyed()) targetBuilding = mg;
            else if (context.mainGateDestroyed && context.alarmLevel >= 50 && ch && !ch->isDestroyed()) targetBuilding = ch;
            else if (context.busterCallTimer <= 5 && bcs && !bcs->isDestroyed()) targetBuilding = bcs;
            else if (!context.robinRescued) targetCharacter = firstCP9;
            else {
                if (boh && !boh->isDestroyed()) targetBuilding = boh;
                else targetCharacter = firstCP9;
            }
        }
    } else if (character->isCP9()) {
        for (int i = 0; i < strawHatCount; i++) {
            if (strawHats[i]->isAlive()) { targetCharacter = strawHats[i]; break; }
        }
    }
    
    string n = character->getName();
    bool useSpecial = false;
    
    if (n == "Luffy") useSpecial = (character->getEnergy() >= 20 && character->getHP() >= ceil(0.15 * character->getMaxHP()));
    else if (n == "Zoro") useSpecial = (character->getEnergy() >= 15);
    else if (n == "Sanji") useSpecial = (character->getEnergy() >= 18);
    else if (n == "Nami") useSpecial = (character->getEnergy() >= 20);
    else if (n == "Chopper") useSpecial = (character->getEnergy() >= 15);
    else if (n == "Usopp") useSpecial = (character->getEnergy() >= 16);
    else if (n == "Franky") useSpecial = (character->getEnergy() >= 20);
    else if (n == "Lucci") useSpecial = (character->getEnergy() >= 25);
    else if (n == "Kaku") useSpecial = (character->getEnergy() >= 20);
    else if (n == "Jabra") useSpecial = (character->getEnergy() >= 18);
    else if (n == "Blueno") useSpecial = (character->getEnergy() >= 15);
    else if (n == "Kalifa") useSpecial = (character->getEnergy() >= 18);
    else if (n == "Kumadori") useSpecial = (character->getEnergy() >= 16);
    else if (n == "Fukurou") useSpecial = (character->getEnergy() >= 14);

    if (targetCharacter) {
        if (useSpecial || isHealing) character->specialSkill(targetCharacter, context);
        else character->attack(targetCharacter, context);
        clampCharacter(targetCharacter);
    } 
    else if (targetBuilding) {
        if (useSpecial) character->specialSkill(targetBuilding, context);
        else character->attack(targetBuilding, context);
        clampBuilding(targetBuilding);
    }
    
    character->endTurn(context);
}

void EniesLobbyBattle::processBuildings() {
    for (int i = 0; i < buildingCount; i++) {
        buildings[i]->applyEffect(context);
    }
}

void EniesLobbyBattle::checkEndCondition() {
    if (context.battleEnded) return;
    if (context.robinRescued && context.escapeProgress >= 100) { context.battleEnded = true; context.resultCode = "STRAW_HAT_WIN"; return; }
    if (context.busterCallTimer <= 0) { context.battleEnded = true; context.resultCode = "BUSTER_CALL"; return; }
    
    bool shAlive = false;
    for (int i = 0; i < strawHatCount; i++) if (strawHats[i]->isAlive()) shAlive = true;
    if (!shAlive) { context.battleEnded = true; context.resultCode = "CP9_WIN"; return; }
    
    bool cp9Alive = false;
    for (int i = 0; i < cp9Count; i++) if (cp9Agents[i]->isAlive()) cp9Alive = true;
    if (!cp9Alive) { context.battleEnded = true; context.resultCode = "STRAW_HAT_WIN_BY_DEFEAT_CP9"; return; }
}

string EniesLobbyBattle::getResult() const {
    ostringstream oss;
    oss << context.resultCode << " " << context.turnCount << " " << context.morale << " " 
        << context.alarmLevel << " " << context.rescueProgress << " " 
        << context.escapeProgress << " " << context.busterCallTimer;
    return oss.str();
}