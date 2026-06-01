#include "eniesLobby.h"

int clampValue(int value, int low, int high)
{
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

int ceilDivInt(long long a, long long b)
{
    if (b <= 0)
        return 0;
    if (a <= 0)
        return 0;
    return (int)((a + b - 1) / b);
}

int ceilPercentValue(int value, int percent)
{
    return ceilDivInt((long long)value * percent, 100);
}

bool hpGreaterPercent(int hp, int maxHp, int percent)
{
    return (long long)hp * 100 > (long long)maxHp * percent;
}

bool hpGreaterEqualPercent(int hp, int maxHp, int percent)
{
    return (long long)hp * 100 >= (long long)maxHp * percent;
}

bool hpLessPercent(int hp, int maxHp, int percent)
{
    return (long long)hp * 100 < (long long)maxHp * percent;
}

bool hpLessEqualPercent(int hp, int maxHp, int percent)
{
    return (long long)hp * 100 <= (long long)maxHp * percent;
}

int effectiveDefenseAfterIgnore(int defense, int ignoredPercent)
{
    // If a skill ignores X% defense, the remaining defense is (100-X)%.
    // The assignment rounds calculated integer values up, so use ceil on the
    // remaining defense rather than subtracting ceil(ignored defense).
    int remainingPercent = 100 - ignoredPercent;
    int effective = ceilPercentValue(defense, remainingPercent);
    return clampValue(effective, 0, defense);
}

void clampContext(BattleContext &ctx)
{
    ctx.morale = clampValue(ctx.morale, 0, 100);
    ctx.alarmLevel = clampValue(ctx.alarmLevel, 0, 100);
    ctx.rescueProgress = clampValue(ctx.rescueProgress, 0, 100);
    ctx.escapeProgress = clampValue(ctx.escapeProgress, 0, 100);
    if (ctx.busterCallTimer < 0)
        ctx.busterCallTimer = 0;
}

void clampCharacter(Character *c)
{
    if (!c)
        return;
    if (c->getHP() <= 0)
    {
        c->setHP(0);
        c->setAlive(false);
    }
    else if (c->getHP() > c->getMaxHP())
    {
        c->setHP(c->getMaxHP());
    }
    c->setEnergy(clampValue(c->getEnergy(), 0, 100));
    if (c->getDef() < 0)
        c->setDef(0);
    if (c->getSpeed() < 0)
        c->setSpeed(0);
}

void clampBuilding(Building *b)
{
    if (!b)
        return;
    if (b->getHP() <= 0)
    {
        b->setHP(0);
        b->setDestroyed(true);
    }
    else if (b->getHP() > b->getMaxHP())
    {
        b->setHP(b->getMaxHP());
    }
}

int applyDamage(Character *attacker, Character *target, int rawDamage,
                BattleContext &context, int customMorale = -999999)
{
    if (!attacker || !target || !target->isAlive())
        return 0;
    if (rawDamage < 0)
        rawDamage = 0;

    int actualDamage = rawDamage - target->getDef();
    if (actualDamage < 0)
        actualDamage = 0;

    int hpBefore = target->getHP();
    target->receiveDamage(rawDamage);
    clampCharacter(target);

    if (hpBefore > 0 && target->getHP() == 0)
    {
        attacker->killedThisTurn = true;
        if (customMorale != -999999)
        {
            context.morale += customMorale;
        }
        else if (attacker->isStrawHat())
        {
            context.morale += 5;
        }
        else if (attacker->isCP9())
        {
            context.morale -= 5;
        }
        clampContext(context);
    }
    return actualDamage;
}

int applyDamageWithTemporaryDefense(Character *attacker, Character *target, int rawDamage,
                                    int temporaryDefense, BattleContext &context,
                                    int customMorale = -999999)
{
    if (!target)
        return 0;
    int originalDef = target->getDef();
    target->setDef(temporaryDefense);
    int actual = applyDamage(attacker, target, rawDamage, context, customMorale);
    target->setDef(originalDef);
    clampCharacter(target);
    return actual;
}

int applyDamageBuilding(Character *attacker, Building *target, int rawDamage, BattleContext &context)
{
    if (!target || target->isDestroyed())
        return 0;
    if (rawDamage < 0)
        rawDamage = 0;

    int hpBefore = target->getHP();
    target->receiveDamage(rawDamage);
    clampBuilding(target);

    if (hpBefore > 0 && target->isDestroyed())
    {
        target->onDestroyed(context);
        clampContext(context);
    }
    return rawDamage;
}

/* BattleContext */
BattleContext::BattleContext()
{
    turnCount = 0;
    morale = 0;
    alarmLevel = 0;
    rescueProgress = 0;
    escapeProgress = 0;
    busterCallTimer = 0;
    mainGateDestroyed = false;
    robinRescued = false;
    bridgeOpened = false;
    battleEnded = false;
    resultCode = "";
}

void BattleContext::nextTurn()
{
    turnCount++;
}

/* Character */
Character::Character()
{
    name = "";
    hp = 0;
    maxHp = 0;
    atk = 0;
    def = 0;
    speed = 0;
    energy = 0;
    alive = false;
    killedThisTurn = false;
    isLowestHP_Enemies = false;
}

Character::Character(string name, int hp, int atk, int def, int speed, int energy)
{
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->alive = (hp > 0);
    this->killedThisTurn = false;
    this->isLowestHP_Enemies = false;
}

Character::~Character() {}

int Character::attack(Building *target, BattleContext &context)
{
    return 0;
}

int Character::specialSkill(Building *target, BattleContext &context)
{
    return 0;
}

void Character::endTurn(BattleContext &context) {}

void Character::receiveDamage(int damage)
{
    if (damage < 0)
        damage = 0;
    int actualDamage = damage - def;
    if (actualDamage < 0)
        actualDamage = 0;
    hp -= actualDamage;
    if (hp <= 0)
    {
        hp = 0;
        alive = false;
    }
}

bool Character::isAlive() const
{
    return alive;
}

string Character::getName() const
{
    return name;
}

int Character::getHP() const
{
    return hp;
}

int Character::getEnergy() const
{
    return energy;
}

bool Character::isStrawHat() const
{
    return false;
}

bool Character::isCP9() const
{
    return false;
}

/* StrawHat */
StrawHat::StrawHat() : Character()
{
    bounty = 0;
}

StrawHat::StrawHat(string name, int hp, int atk, int def,
                   int speed, int energy, long long bounty)
    : Character(name, hp, atk, def, speed, energy)
{
    this->bounty = bounty;
}

bool StrawHat::isStrawHat() const
{
    return true;
}

string StrawHat::str() const
{
    ostringstream oss;
    oss << "StrawHat[name=" << name
        << ", hp=" << hp
        << ", atk=" << atk
        << ", def=" << def
        << ", speed=" << speed
        << ", energy=" << energy
        << ", bounty=" << bounty << "]";
    return oss.str();
}

/* Luffy */
Luffy::Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Luffy::attack(Character *target, BattleContext &context)
{
    int dmg = atk;
    if (!hpGreaterPercent(hp, maxHp, 50))
    {
        if (hpGreaterPercent(hp, maxHp, 30))
            dmg = ceilPercentValue(atk, 115);
        else
            dmg = ceilPercentValue(atk, 130);
    }
    return applyDamage(this, target, dmg, context);
}

int Luffy::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 20 || !hpGreaterEqualPercent(hp, maxHp, 15))
        return 0;

    energy -= 20;
    int dmg = ceilPercentValue(atk, 200);
    speed += 15;
    atk += 15;
    context.alarmLevel += 10;
    hp -= ceilPercentValue(maxHp, 8);
    if (hp <= 0)
    {
        hp = 0;
        alive = false;
    }

    int actual = applyDamage(this, target, dmg, context);
    clampCharacter(this);
    clampContext(context);
    return actual;
}

int Luffy::attack(Building *target, BattleContext &context)
{
    int dmg = atk;
    if (!hpGreaterPercent(hp, maxHp, 50))
    {
        if (hpGreaterPercent(hp, maxHp, 30))
            dmg = ceilPercentValue(atk, 115);
        else
            dmg = ceilPercentValue(atk, 130);
    }
    return applyDamageBuilding(this, target, dmg, context);
}

int Luffy::specialSkill(Building *target, BattleContext &context)
{
    if (energy < 20 || !hpGreaterEqualPercent(hp, maxHp, 15))
        return 0;

    energy -= 20;
    int dmg = ceilPercentValue(atk, 200);
    speed += 15;
    atk += 15;
    context.alarmLevel += 10;
    hp -= ceilPercentValue(maxHp, 8);
    if (hp <= 0)
    {
        hp = 0;
        alive = false;
    }

    int actual = applyDamageBuilding(this, target, dmg, context);
    clampCharacter(this);
    clampContext(context);
    return actual;
}

void Luffy::endTurn(BattleContext &context)
{
    if (hpLessEqualPercent(hp, maxHp, 30))
        context.morale += 3;
    if (killedThisTurn)
        energy += 5;
    clampContext(context);
    clampCharacter(this);
}

/* Zoro */
Zoro::Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Zoro::attack(Character *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(def, 20);
    if (target && hpLessPercent(target->getHP(), target->getMaxHP(), 40))
        dmg = ceilPercentValue(dmg, 115);
    return applyDamage(this, target, dmg, context);
}

int Zoro::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 15)
        return 0;
    energy -= 15;

    int dmg = ceilPercentValue(atk, 220);
    if (target && hpLessPercent(target->getHP(), target->getMaxHP(), 50))
        dmg = ceilPercentValue(dmg, 150);

    int hpBefore = target ? target->getHP() : 0;
    int actual = applyDamage(this, target, dmg, context, 4);
    if (target && hpBefore > 0 && target->getHP() == 0)
    {
        energy += 8;
        clampCharacter(this);
    }
    return actual;
}

int Zoro::attack(Building *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(def, 20);
    return applyDamageBuilding(this, target, dmg, context);
}

int Zoro::specialSkill(Building *target, BattleContext &context)
{
    if (energy < 15)
        return 0;
    energy -= 15;

    int dmg = ceilPercentValue(atk, 220);
    if (target && target->getHP() * 100 < target->getMaxHP() * 50)
        dmg = ceilPercentValue(dmg, 150);
    return applyDamageBuilding(this, target, dmg, context);
}

void Zoro::endTurn(BattleContext &context)
{
    if (killedThisTurn)
    {
        context.morale += 6;
        atk += ceilPercentValue(atk, 5);
    }
    clampContext(context);
    clampCharacter(this);
}

/* Sanji */
Sanji::Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Sanji::attack(Character *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(speed, 50);
    if (target && target->getDef() < def)
        dmg = ceilPercentValue(dmg, 110);
    return applyDamage(this, target, dmg, context);
}

int Sanji::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 18)
        return 0;
    energy -= 18;

    int dmg = ceilPercentValue(atk, 210);
    int actual = applyDamage(this, target, dmg, context);
    if (target)
    {
        target->setDef(target->getDef() - (target->getName() == "Jabra" ? 12 : 8));
        clampCharacter(target);
    }
    return actual;
}

int Sanji::attack(Building *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(speed, 50);
    return applyDamageBuilding(this, target, dmg, context);
}

int Sanji::specialSkill(Building *target, BattleContext &context)
{
    if (energy < 18)
        return 0;
    energy -= 18;
    int dmg = ceilPercentValue(atk, 210);
    return applyDamageBuilding(this, target, dmg, context);
}

void Sanji::endTurn(BattleContext &context)
{
    if (killedThisTurn)
    {
        context.morale += 8;
        atk += ceilPercentValue(atk, 10);
    }
    clampContext(context);
    clampCharacter(this);
}

/* Nami */
Nami::Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Nami::attack(Character *target, BattleContext &context)
{
    if (!target)
        return 0;
    int effectiveDef = effectiveDefenseAfterIgnore(target->getDef(), 30);
    return applyDamageWithTemporaryDefense(this, target, atk, effectiveDef, context);
}

int Nami::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 20)
        return 0;
    energy -= 20;

    int dmg = atk + 40;
    if (target)
    {
        target->setSpeed(target->getSpeed() - 10);
        clampCharacter(target);
    }
    context.busterCallTimer += 1;
    context.alarmLevel -= 5;
    clampContext(context);
    return applyDamage(this, target, dmg, context);
}

int Nami::attack(Building *target, BattleContext &context)
{
    return applyDamageBuilding(this, target, ceilPercentValue(atk, 50), context);
}

int Nami::specialSkill(Building *target, BattleContext &context)
{
    if (energy < 20)
        return 0;
    energy -= 20;

    int dmg = ceilPercentValue(atk + 40, 150);
    context.busterCallTimer += 1;
    context.alarmLevel -= 5;
    clampContext(context);
    return applyDamageBuilding(this, target, dmg, context);
}

void Nami::endTurn(BattleContext &context)
{
    if (killedThisTurn)
        energy += 6;
    clampCharacter(this);
}

/* Chopper */
Chopper::Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Chopper::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Chopper::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 15 || !target || !target->isStrawHat() || !target->isAlive())
        return 0;
    energy -= 15;

    int heal = 35 + ceilPercentValue(atk, 50);
    target->setHP(target->getHP() + heal);
    clampCharacter(target);
    if (target->getName() == "Luffy")
    {
        context.morale += 5;
        clampContext(context);
    }
    clampCharacter(this);
    return 0;
}

int Chopper::attack(Building *target, BattleContext &context)
{
    return applyDamageBuilding(this, target, atk, context);
}

int Chopper::specialSkill(Building *target, BattleContext &context)
{
    return 0;
}

void Chopper::endTurn(BattleContext &context) {}

/* Usopp */
Usopp::Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Usopp::attack(Character *target, BattleContext &context)
{
    int dmg = atk;
    if (target && target->getSpeed() < 50)
        dmg = ceilPercentValue(dmg, 120);
    return applyDamage(this, target, dmg, context);
}

int Usopp::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 16)
        return 0;
    energy -= 16;

    int dmg = ceilPercentValue(atk, 80);
    if (target)
    {
        target->setSpeed(target->getSpeed() - 12);
        clampCharacter(target);
    }
    context.escapeProgress += 8;
    clampContext(context);
    return applyDamage(this, target, dmg, context);
}

int Usopp::attack(Building *target, BattleContext &context)
{
    return applyDamageBuilding(this, target, ceilPercentValue(atk, 50), context);
}

int Usopp::specialSkill(Building *target, BattleContext &context)
{
    if (energy < 16)
        return 0;
    energy -= 16;

    int dmg = ceilPercentValue(atk, 80);
    context.escapeProgress += 8;
    clampContext(context);
    return applyDamageBuilding(this, target, dmg, context);
}

void Usopp::endTurn(BattleContext &context)
{
    context.morale += 10;
    clampContext(context);
}

/* Franky */
Franky::Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty)
    : StrawHat(name, hp, atk, def, speed, energy, bounty) {}

int Franky::attack(Character *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(def, 30);
    if (target && target->isCP9())
        dmg = ceilPercentValue(dmg, 110);
    return applyDamage(this, target, dmg, context);
}

int Franky::specialSkill(Character *target, BattleContext &context)
{
    if (!target)
        return 0;

    // Franky has two special skills. Testcases call specialSkill() directly,
    // so choose Coup de Vent first when enough energy is available.
    if (energy >= 30)
    {
        energy -= 30;
        int dmg = ceilPercentValue(atk, 120);
        int actual = applyDamage(this, target, dmg, context);
        clampCharacter(this);
        return actual;
    }

    if (energy >= 20)
    {
        energy -= 20;
        int dmg = ceilPercentValue(atk, 180);
        target->setSpeed(target->getSpeed() - 8);
        clampCharacter(target);
        if (target->getName() == "Lucci")
            dmg = ceilPercentValue(dmg, 120);
        int actual = applyDamage(this, target, dmg, context);
        clampCharacter(this);
        return actual;
    }

    return 0;
}

int Franky::attack(Building *target, BattleContext &context)
{
    int dmg = atk + ceilPercentValue(def, 30);
    return applyDamageBuilding(this, target, dmg, context);
}

int Franky::specialSkill(Building *target, BattleContext &context)
{
    if (!target)
        return 0;
    if (energy >= 30)
    {
        energy -= 30;
        int dmg = ceilPercentValue(atk, 120);
        int hpBefore = target->getHP();
        applyDamageBuilding(this, target, hpBefore, context);
        clampCharacter(this);
        return dmg;
    }
    if (energy >= 20)
    {
        energy -= 20;
        int dmg = ceilPercentValue(atk, 180);
        int actual = applyDamageBuilding(this, target, dmg, context);
        clampCharacter(this);
        return actual;
    }
    return 0;
}

void Franky::endTurn(BattleContext &context)
{
    if (hpGreaterPercent(hp, maxHp, 70))
        def += 5;
    if (hpLessPercent(hp, maxHp, 30))
        atk += ceilPercentValue(atk, 10);
    clampCharacter(this);
}

/* CP9Agent */
CP9Agent::CP9Agent() : Character()
{
    doriki = 0;
}

CP9Agent::CP9Agent(string name, int hp, int atk, int def,
                   int speed, int energy, int doriki)
    : Character(name, hp, atk, def, speed, energy)
{
    this->doriki = doriki;
}

bool CP9Agent::isCP9() const
{
    return true;
}

string CP9Agent::str() const
{
    ostringstream oss;
    oss << "CP9[name=" << name
        << ", hp=" << hp
        << ", atk=" << atk
        << ", def=" << def
        << ", speed=" << speed
        << ", energy=" << energy
        << ", doriki=" << doriki << "]";
    return oss.str();
}

/* Lucci */
Lucci::Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Lucci::attack(Character *target, BattleContext &context)
{
    int dmg = atk + ceilDivInt(doriki, 20);
    if (target && hpLessPercent(target->getHP(), target->getMaxHP(), 50))
        dmg = ceilPercentValue(dmg, 120);
    return applyDamage(this, target, dmg, context);
}

int Lucci::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 25)
        return 0;
    energy -= 25;

    int dmg = ceilPercentValue(atk, 280);
    if (!target)
        return 0;
    int effectiveDef = effectiveDefenseAfterIgnore(target->getDef(), 50);
    return applyDamageWithTemporaryDefense(this, target, dmg, effectiveDef, context, -15);
}

void Lucci::endTurn(BattleContext &context)
{
    if (hpLessPercent(hp, maxHp, 40))
        atk += ceilPercentValue(atk, 5);
    clampCharacter(this);
}

/* Kaku */
Kaku::Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kaku::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Kaku::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 20)
        return 0;
    energy -= 20;

    int total = 0;
    total += applyDamage(this, target, ceilPercentValue(atk, 120), context);
    if (target && target->isAlive())
        total += applyDamage(this, target, atk, context);
    if (target && target->isAlive())
        total += applyDamage(this, target, ceilPercentValue(atk, 80), context);
    return total;
}

void Kaku::endTurn(BattleContext &context) {}

/* Jabra */
Jabra::Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Jabra::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Jabra::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 18)
        return 0;
    energy -= 18;

    int dmg = ceilPercentValue(atk, 150);
    if (hpLessPercent(hp, maxHp, 30))
        dmg = ceilPercentValue(dmg, 125);
    return applyDamage(this, target, dmg, context, -10);
}

void Jabra::endTurn(BattleContext &context) {}

/* Blueno */
Blueno::Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Blueno::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Blueno::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 15)
        return 0;
    energy -= 15;

    int dmg = ceilPercentValue(atk, 130);
    if (hpGreaterPercent(hp, maxHp, 50))
        dmg += 20;
    else
        dmg += 40;
    return applyDamage(this, target, dmg, context);
}

void Blueno::endTurn(BattleContext &context) {}

/* Kalifa */
Kalifa::Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kalifa::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Kalifa::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 18)
        return 0;
    energy -= 18;

    int dmg = ceilPercentValue(atk, 140);
    context.morale -= (target && target->getName() == "Nami" ? 12 : 8);
    if (target)
    {
        target->setSpeed(target->getSpeed() - 6);
        clampCharacter(target);
    }
    clampContext(context);
    return applyDamage(this, target, dmg, context);
}

void Kalifa::endTurn(BattleContext &context) {}

/* Kumadori */
Kumadori::Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Kumadori::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Kumadori::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 16)
        return 0;
    energy -= 16;

    int dmg = 30 + ceilDivInt(doriki, 10);
    if (hpLessPercent(hp, maxHp, 40))
        dmg += 25;
    return applyDamage(this, target, dmg, context);
}

void Kumadori::endTurn(BattleContext &context) {}

/* Fukurou */
Fukurou::Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki)
    : CP9Agent(name, hp, atk, def, speed, energy, doriki) {}

int Fukurou::attack(Character *target, BattleContext &context)
{
    return applyDamage(this, target, atk, context);
}

int Fukurou::specialSkill(Character *target, BattleContext &context)
{
    if (energy < 14)
        return 0;
    energy -= 14;

    int dmg = ceilPercentValue(atk, 130);
    if (target && target->isLowestHP_Enemies)
        dmg += 20;
    return applyDamage(this, target, dmg, context, -11);
}

void Fukurou::endTurn(BattleContext &context) {}

/* Building */
Building::Building(string name, int hp)
{
    this->name = name;
    this->hp = hp;
    this->maxHP = hp;
    this->destroyed = (hp <= 0);
}

Building::~Building() {}

void Building::receiveDamage(int damage)
{
    if (damage < 0)
        damage = 0;
    hp -= damage;
    if (hp <= 0)
    {
        hp = 0;
        destroyed = true;
    }
}

bool Building::isDestroyed() const
{
    return destroyed;
}

void Building::onDestroyed(BattleContext &context) {}

string Building::str() const
{
    ostringstream oss;
    oss << "Building[name=" << name
        << ", hp=" << hp
        << ", maxHP=" << maxHP
        << ", destroyed=" << (destroyed ? "true" : "false") << "]";
    return oss.str();
}

/* Building subclasses */
MainGate::MainGate(string name, int hp) : Building(name, hp) {}

void MainGate::applyEffect(BattleContext &context) {}

void MainGate::onDestroyed(BattleContext &context)
{
    context.mainGateDestroyed = true;
    context.rescueProgress += 20;
    context.morale += 5;
    clampContext(context);
}

Courthouse::Courthouse(string name, int hp) : Building(name, hp) {}

void Courthouse::applyEffect(BattleContext &context)
{
    if (!destroyed)
    {
        context.alarmLevel += 5;
        clampContext(context);
    }
}

void Courthouse::onDestroyed(BattleContext &context)
{
    context.alarmLevel -= 20;
    clampContext(context);
}

TowerOfJustice::TowerOfJustice(string name, int hp) : Building(name, hp) {}

void TowerOfJustice::applyEffect(BattleContext &context)
{
    if (!destroyed && context.mainGateDestroyed && !context.robinRescued)
    {
        context.rescueProgress += 5;
        if (context.rescueProgress >= 100)
        {
            context.robinRescued = true;
            context.morale += 10;
        }
        clampContext(context);
    }
}

BridgeOfHesitation::BridgeOfHesitation(string name, int hp) : Building(name, hp) {}

void BridgeOfHesitation::applyEffect(BattleContext &context)
{
    if (!destroyed && context.robinRescued)
    {
        context.bridgeOpened = true;
        context.escapeProgress += 5;
        if (context.escapeProgress >= 100)
        {
            context.battleEnded = true;
            context.resultCode = "STRAW_HAT_WIN";
        }
        clampContext(context);
    }
}

BusterCallShip::BusterCallShip(string name, int hp) : Building(name, hp) {}

void BusterCallShip::applyEffect(BattleContext &context)
{
    if (!destroyed)
    {
        context.busterCallTimer -= 1;
        if (context.busterCallTimer <= 0)
        {
            context.battleEnded = true;
            context.resultCode = "BUSTER_CALL";
        }
        clampContext(context);
    }
}

void BusterCallShip::onDestroyed(BattleContext &context)
{
    context.busterCallTimer += 3;
    clampContext(context);
}

/* EniesLobbyBattle */
EniesLobbyBattle::EniesLobbyBattle(const string &filename)
{
    strawHats = new Character *[7];
    strawHatCount = 0;
    cp9Agents = new Character *[7];
    cp9Count = 0;
    buildings = new Building *[5];
    buildingCount = 0;
    turnOrder = nullptr;
    maxTurns = 0;
    loadFromFile(filename);
}

EniesLobbyBattle::~EniesLobbyBattle()
{
    for (int i = 0; i < strawHatCount; i++)
        delete strawHats[i];
    delete[] strawHats;

    for (int i = 0; i < cp9Count; i++)
        delete cp9Agents[i];
    delete[] cp9Agents;

    for (int i = 0; i < buildingCount; i++)
        delete buildings[i];
    delete[] buildings;

    while (turnOrder)
    {
        TurnNode *tmp = turnOrder;
        turnOrder = turnOrder->next;
        delete tmp;
    }
}

void EniesLobbyBattle::loadFromFile(const string &filename)
{
    ifstream file(filename.c_str());
    if (!file.is_open())
        return;

    string type;
    while (file >> type)
    {
        if (type == "CONTEXT")
        {
            file >> context.morale >> context.alarmLevel >> context.rescueProgress >> context.escapeProgress >> context.busterCallTimer >> maxTurns;
            clampContext(context);
        }
        else if (type == "STRAW_HAT")
        {
            string name;
            int hp, atk, def, speed, energy;
            long long bounty;
            file >> name >> hp >> atk >> def >> speed >> energy >> bounty;

            Character *c = nullptr;
            if (name == "Luffy")
                c = new Luffy(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Zoro")
                c = new Zoro(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Sanji")
                c = new Sanji(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Nami")
                c = new Nami(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Chopper")
                c = new Chopper(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Usopp")
                c = new Usopp(name, hp, atk, def, speed, energy, bounty);
            else if (name == "Franky")
                c = new Franky(name, hp, atk, def, speed, energy, bounty);
            if (c)
                addStrawHat(c);
        }
        else if (type == "CP9")
        {
            string name;
            int hp, atk, def, speed, energy, doriki;
            file >> name >> hp >> atk >> def >> speed >> energy >> doriki;

            Character *c = nullptr;
            if (name == "Lucci")
                c = new Lucci(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kaku")
                c = new Kaku(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Jabra")
                c = new Jabra(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Blueno")
                c = new Blueno(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kalifa")
                c = new Kalifa(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Kumadori")
                c = new Kumadori(name, hp, atk, def, speed, energy, doriki);
            else if (name == "Fukurou")
                c = new Fukurou(name, hp, atk, def, speed, energy, doriki);
            if (c)
                addCP9Agent(c);
        }
        else if (type == "BUILDING")
        {
            string name;
            int hp;
            file >> name >> hp;

            Building *b = nullptr;
            if (name == "MainGate")
                b = new MainGate(name, hp);
            else if (name == "Courthouse")
                b = new Courthouse(name, hp);
            else if (name == "TowerOfJustice")
                b = new TowerOfJustice(name, hp);
            else if (name == "BridgeOfHesitation")
                b = new BridgeOfHesitation(name, hp);
            else if (name == "BusterCallShip")
                b = new BusterCallShip(name, hp);
            if (b)
                addBuilding(b);
        }
    }

    buildTurnOrder();
}

void EniesLobbyBattle::addStrawHat(Character *character)
{
    if (character && strawHatCount < 7)
        strawHats[strawHatCount++] = character;
}

void EniesLobbyBattle::addCP9Agent(Character *character)
{
    if (character && cp9Count < 7)
        cp9Agents[cp9Count++] = character;
}

void EniesLobbyBattle::addBuilding(Building *building)
{
    if (building && buildingCount < 5)
        buildings[buildingCount++] = building;
}

void EniesLobbyBattle::buildTurnOrder()
{
    while (turnOrder)
    {
        TurnNode *tmp = turnOrder;
        turnOrder = turnOrder->next;
        delete tmp;
    }

    TurnNode *tail = nullptr;
    for (int i = 0; i < strawHatCount; i++)
    {
        TurnNode *node = new TurnNode;
        node->data = strawHats[i];
        node->next = nullptr;
        if (!turnOrder)
            turnOrder = node;
        else
            tail->next = node;
        tail = node;
    }

    for (int i = 0; i < cp9Count; i++)
    {
        TurnNode *node = new TurnNode;
        node->data = cp9Agents[i];
        node->next = nullptr;
        if (!turnOrder)
            turnOrder = node;
        else
            tail->next = node;
        tail = node;
    }
}

void EniesLobbyBattle::runBattle()
{
    while (!context.battleEnded && context.turnCount < maxTurns)
    {
        if (!turnOrder)
            break;

        TurnNode *current = turnOrder;
        turnOrder = turnOrder->next;
        current->next = nullptr;

        if (current->data && current->data->isAlive())
        {
            processTurn(current->data);
        }

        if (!turnOrder)
        {
            turnOrder = current;
        }
        else
        {
            TurnNode *temp = turnOrder;
            while (temp->next)
                temp = temp->next;
            temp->next = current;
        }

        processBuildings();
        context.nextTurn();
        checkEndCondition();
    }

    if (!context.battleEnded && context.turnCount >= maxTurns)
    {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
    }
}

void EniesLobbyBattle::processTurn(Character *character)
{
    if (!character || !character->isAlive())
        return;

    character->killedThisTurn = false;
    Character *targetCharacter = nullptr;
    Building *targetBuilding = nullptr;
    bool isHealing = false;

    int minSHHp = INT_MAX;
    for (int i = 0; i < strawHatCount; i++)
    {
        if (strawHats[i]->isAlive() && strawHats[i]->getHP() < minSHHp)
        {
            minSHHp = strawHats[i]->getHP();
        }
    }
    for (int i = 0; i < strawHatCount; i++)
    {
        strawHats[i]->isLowestHP_Enemies = (strawHats[i]->isAlive() && strawHats[i]->getHP() == minSHHp);
    }

    Character *firstCP9 = nullptr;
    for (int i = 0; i < cp9Count; i++)
    {
        if (cp9Agents[i]->isAlive())
        {
            firstCP9 = cp9Agents[i];
            break;
        }
    }

    if (character->isStrawHat())
    {
        if (character->getName() == "Chopper" && character->getEnergy() >= 15)
        {
            for (int i = 0; i < strawHatCount; i++)
            {
                if (strawHats[i]->isAlive() && strawHats[i]->getHP() == minSHHp)
                {
                    targetCharacter = strawHats[i];
                    break;
                }
            }
            isHealing = true;
        }
        else
        {
            Building *mg = findBuilding("MainGate");
            Building *ch = findBuilding("Courthouse");
            Building *bcs = findBuilding("BusterCallShip");
            Building *boh = findBuilding("BridgeOfHesitation");

            if (mg && !mg->isDestroyed())
                targetBuilding = mg;
            else if (context.mainGateDestroyed && context.alarmLevel >= 50 && ch && !ch->isDestroyed())
                targetBuilding = ch;
            else if (context.busterCallTimer <= 5 && bcs && !bcs->isDestroyed())
                targetBuilding = bcs;
            else if (!context.robinRescued)
                targetCharacter = firstCP9;
            else
            {
                if (boh && !boh->isDestroyed())
                    targetBuilding = boh;
                else
                    targetCharacter = firstCP9;
            }
        }
    }
    else if (character->isCP9())
    {
        for (int i = 0; i < strawHatCount; i++)
        {
            if (strawHats[i]->isAlive())
            {
                targetCharacter = strawHats[i];
                break;
            }
        }
    }

    string n = character->getName();
    bool useSpecial = false;

    if (n == "Luffy")
        useSpecial = (character->getEnergy() >= 20 && hpGreaterEqualPercent(character->getHP(), character->getMaxHP(), 15));
    else if (n == "Zoro")
        useSpecial = (character->getEnergy() >= 15);
    else if (n == "Sanji")
        useSpecial = (character->getEnergy() >= 18);
    else if (n == "Nami")
        useSpecial = (character->getEnergy() >= 20);
    else if (n == "Chopper")
        useSpecial = (character->getEnergy() >= 15);
    else if (n == "Usopp")
        useSpecial = (character->getEnergy() >= 16);
    else if (n == "Franky")
        useSpecial = (character->getEnergy() >= 20);
    else if (n == "Lucci")
        useSpecial = (character->getEnergy() >= 25);
    else if (n == "Kaku")
        useSpecial = (character->getEnergy() >= 20);
    else if (n == "Jabra")
        useSpecial = (character->getEnergy() >= 18);
    else if (n == "Blueno")
        useSpecial = (character->getEnergy() >= 15);
    else if (n == "Kalifa")
        useSpecial = (character->getEnergy() >= 18);
    else if (n == "Kumadori")
        useSpecial = (character->getEnergy() >= 16);
    else if (n == "Fukurou")
        useSpecial = (character->getEnergy() >= 14);

    if (targetCharacter)
    {
        if (useSpecial || isHealing)
            character->specialSkill(targetCharacter, context);
        else
            character->attack(targetCharacter, context);
        clampCharacter(targetCharacter);
    }
    else if (targetBuilding)
    {
        if (useSpecial)
            character->specialSkill(targetBuilding, context);
        else
            character->attack(targetBuilding, context);
        clampBuilding(targetBuilding);
    }

    character->endTurn(context);
    clampCharacter(character);
    clampContext(context);
}

void EniesLobbyBattle::processBuildings()
{
    for (int i = 0; i < buildingCount; i++)
    {
        buildings[i]->applyEffect(context);
        clampContext(context);
    }
}

void EniesLobbyBattle::checkEndCondition()
{
    if (context.robinRescued && context.escapeProgress >= 100)
    {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN";
        return;
    }

    if (context.busterCallTimer <= 0)
    {
        context.battleEnded = true;
        context.resultCode = "BUSTER_CALL";
        return;
    }

    bool shAlive = false;
    for (int i = 0; i < strawHatCount; i++)
    {
        if (strawHats[i]->isAlive())
        {
            shAlive = true;
            break;
        }
    }
    if (!shAlive)
    {
        context.battleEnded = true;
        context.resultCode = "CP9_WIN";
        return;
    }

    bool cp9Alive = false;
    for (int i = 0; i < cp9Count; i++)
    {
        if (cp9Agents[i]->isAlive())
        {
            cp9Alive = true;
            break;
        }
    }
    if (!cp9Alive)
    {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN_BY_DEFEAT_CP9";
        return;
    }

    if (context.turnCount >= maxTurns)
    {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
        return;
    }
}

string EniesLobbyBattle::getResult() const
{
    ostringstream oss;
    oss << context.resultCode << " " << context.turnCount << " " << context.morale << " "
        << context.alarmLevel << " " << context.rescueProgress << " "
        << context.escapeProgress << " " << context.busterCallTimer;
    return oss.str();
}
