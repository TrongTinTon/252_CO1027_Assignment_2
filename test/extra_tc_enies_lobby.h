#ifndef EXTRA_TC_ENIES_LOBBY_H
#define EXTRA_TC_ENIES_LOBBY_H

#include "test/tc_checker.h"

// =======================
// Testcase 1: StrawHat str() + faction flags
// =======================
bool tc1() {
    bool ok = true;

    Character* strawHats[7];
    strawHats[0] = new Luffy("Luffy", 120, 35, 20, 25, 50, 1500000000LL);
    strawHats[1] = new Zoro("Zoro", 110, 38, 25, 20, 45, 1111000000LL);
    strawHats[2] = new Sanji("Sanji", 100, 34, 18, 30, 45, 1032000000LL);
    strawHats[3] = new Nami("Nami", 80, 22, 12, 35, 60, 366000000LL);
    strawHats[4] = new Chopper("Chopper", 90, 20, 15, 22, 55, 1000LL);
    strawHats[5] = new Usopp("Usopp", 85, 24, 14, 28, 50, 500000000LL);
    strawHats[6] = new Franky("Franky", 130, 32, 28, 18, 50, 394000000LL);

    string expected[7] = {
        "StrawHat[name=Luffy, hp=120, atk=35, def=20, speed=25, energy=50, bounty=1500000000]",
        "StrawHat[name=Zoro, hp=110, atk=38, def=25, speed=20, energy=45, bounty=1111000000]",
        "StrawHat[name=Sanji, hp=100, atk=34, def=18, speed=30, energy=45, bounty=1032000000]",
        "StrawHat[name=Nami, hp=80, atk=22, def=12, speed=35, energy=60, bounty=366000000]",
        "StrawHat[name=Chopper, hp=90, atk=20, def=15, speed=22, energy=55, bounty=1000]",
        "StrawHat[name=Usopp, hp=85, atk=24, def=14, speed=28, energy=50, bounty=500000000]",
        "StrawHat[name=Franky, hp=130, atk=32, def=28, speed=18, energy=50, bounty=394000000]"
    };

    for (int i = 0; i < 7; i++) {
        expectString(strawHats[i]->str(), expected[i], string("TC1 StrawHat[") + char('0' + i) + "]", ok);
        expectBool(strawHats[i]->isStrawHat(), true, string("TC1 isStrawHat[") + char('0' + i) + "]", ok);
        expectBool(strawHats[i]->isCP9(), false, string("TC1 isCP9[") + char('0' + i) + "]", ok);
    }

    for (int i = 0; i < 7; i++) delete strawHats[i];
    return ok;
}

// =======================
// Testcase 2: CP9 str() + faction flags
// =======================
bool tc2() {
    bool ok = true;

    Character* cp9[7];
    cp9[0] = new Lucci("Lucci", 150, 40, 25, 30, 60, 4000);
    cp9[1] = new Kaku("Kaku", 130, 35, 22, 28, 55, 2200);
    cp9[2] = new Jabra("Jabra", 125, 33, 30, 24, 50, 2180);
    cp9[3] = new Blueno("Blueno", 110, 28, 24, 26, 45, 820);
    cp9[4] = new Kalifa("Kalifa", 95, 26, 18, 32, 50, 630);
    cp9[5] = new Kumadori("Kumadori", 115, 30, 20, 20, 45, 810);
    cp9[6] = new Fukurou("Fukurou", 100, 27, 16, 25, 40, 800);

    string expected[7] = {
        "CP9[name=Lucci, hp=150, atk=40, def=25, speed=30, energy=60, doriki=4000]",
        "CP9[name=Kaku, hp=130, atk=35, def=22, speed=28, energy=55, doriki=2200]",
        "CP9[name=Jabra, hp=125, atk=33, def=30, speed=24, energy=50, doriki=2180]",
        "CP9[name=Blueno, hp=110, atk=28, def=24, speed=26, energy=45, doriki=820]",
        "CP9[name=Kalifa, hp=95, atk=26, def=18, speed=32, energy=50, doriki=630]",
        "CP9[name=Kumadori, hp=115, atk=30, def=20, speed=20, energy=45, doriki=810]",
        "CP9[name=Fukurou, hp=100, atk=27, def=16, speed=25, energy=40, doriki=800]"
    };

    for (int i = 0; i < 7; i++) {
        expectString(cp9[i]->str(), expected[i], string("TC2 CP9[") + char('0' + i) + "]", ok);
        expectBool(cp9[i]->isStrawHat(), false, string("TC2 isStrawHat[") + char('0' + i) + "]", ok);
        expectBool(cp9[i]->isCP9(), true, string("TC2 isCP9[") + char('0' + i) + "]", ok);
    }

    for (int i = 0; i < 7; i++) delete cp9[i];
    return ok;
}

// =======================
// Testcase 3: Simple character attack group 1
// =======================
bool tc3() {
    bool ok = true;

    Character* strawHats[3];
    strawHats[0] = new Luffy("Luffy", 120, 35, 20, 25, 50, 1500000000LL);
    strawHats[1] = new Nami("Nami", 80, 22, 12, 35, 60, 366000000LL);
    strawHats[2] = new Franky("Franky", 130, 32, 28, 18, 50, 394000000LL);

    Character* cp9[3];
    cp9[0] = new Lucci("Lucci", 150, 40, 25, 30, 60, 4000);
    cp9[1] = new Kaku("Kaku", 130, 35, 22, 28, 55, 2200);
    cp9[2] = new Kalifa("Kalifa", 95, 26, 18, 32, 50, 630);

    BattleContext context;
    int damage;

    damage = strawHats[0]->specialSkill(cp9[0], context);
    expectInt(damage, 45, "TC3 Luffy special damage", ok);
    expectCharacterState(strawHats[0], "StrawHat[name=Luffy, hp=110, atk=50, def=20, speed=40, energy=30, bounty=1500000000]", true, "TC3 Luffy after special", ok);
    expectCharacterState(cp9[0], "CP9[name=Lucci, hp=105, atk=40, def=25, speed=30, energy=60, doriki=4000]", true, "TC3 Lucci after Luffy special", ok);

    damage = strawHats[1]->attack(cp9[1], context);
    expectInt(damage, 6, "TC3 Nami attack damage", ok);
    expectCharacterState(strawHats[1], "StrawHat[name=Nami, hp=80, atk=22, def=12, speed=35, energy=60, bounty=366000000]", true, "TC3 Nami after attack", ok);
    expectCharacterState(cp9[1], "CP9[name=Kaku, hp=124, atk=35, def=22, speed=28, energy=55, doriki=2200]", true, "TC3 Kaku after Nami attack", ok);

    damage = strawHats[2]->specialSkill(cp9[2], context);
    expectInt(damage, 21, "TC3 Franky special damage", ok);
    expectCharacterState(strawHats[2], "StrawHat[name=Franky, hp=130, atk=32, def=28, speed=18, energy=20, bounty=394000000]", true, "TC3 Franky after special", ok);
    expectCharacterState(cp9[2], "CP9[name=Kalifa, hp=74, atk=26, def=18, speed=32, energy=50, doriki=630]", true, "TC3 Kalifa after Franky special", ok);

    damage = cp9[0]->attack(strawHats[0], context);
    expectInt(damage, 220, "TC3 Lucci attack damage", ok);
    expectCharacterState(strawHats[0], "StrawHat[name=Luffy, hp=0, atk=50, def=20, speed=40, energy=30, bounty=1500000000]", false, "TC3 Luffy after Lucci attack", ok);

    damage = cp9[1]->specialSkill(strawHats[1], context);
    expectInt(damage, 69, "TC3 Kaku special damage", ok);
    expectCharacterState(cp9[1], "CP9[name=Kaku, hp=124, atk=35, def=22, speed=28, energy=35, doriki=2200]", true, "TC3 Kaku after special", ok);
    expectCharacterState(strawHats[1], "StrawHat[name=Nami, hp=11, atk=22, def=12, speed=35, energy=60, bounty=366000000]", true, "TC3 Nami after Kaku special", ok);

    damage = cp9[2]->specialSkill(strawHats[2], context);
    expectInt(damage, 9, "TC3 Kalifa special damage", ok);
    expectCharacterState(cp9[2], "CP9[name=Kalifa, hp=74, atk=26, def=18, speed=32, energy=32, doriki=630]", true, "TC3 Kalifa after special", ok);
    expectCharacterState(strawHats[2], "StrawHat[name=Franky, hp=121, atk=32, def=28, speed=12, energy=20, bounty=394000000]", true, "TC3 Franky after Kalifa special", ok);

    for (int i = 0; i < 3; i++) delete strawHats[i];
    for (int i = 0; i < 3; i++) delete cp9[i];
    return ok;
}

// =======================
// Testcase 4: Simple character attack group 2
// =======================
bool tc4() {
    bool ok = true;

    Character* strawHats[4];
    strawHats[0] = new Zoro("Zoro", 110, 38, 25, 20, 45, 1111000000LL);
    strawHats[1] = new Sanji("Sanji", 100, 34, 18, 30, 45, 1032000000LL);
    strawHats[2] = new Chopper("Chopper", 90, 20, 15, 22, 55, 1000LL);
    strawHats[3] = new Usopp("Usopp", 85, 24, 14, 28, 50, 500000000LL);

    Character* cp9[4];
    cp9[0] = new Jabra("Jabra", 125, 33, 30, 24, 50, 2180);
    cp9[1] = new Blueno("Blueno", 110, 28, 24, 26, 45, 820);
    cp9[2] = new Kumadori("Kumadori", 115, 30, 20, 20, 45, 810);
    cp9[3] = new Fukurou("Fukurou", 100, 27, 16, 25, 40, 800);

    BattleContext context;
    int damage;

    damage = strawHats[0]->specialSkill(cp9[0], context);
    expectInt(damage, 54, "TC4 Zoro special damage", ok);
    expectCharacterState(strawHats[0], "StrawHat[name=Zoro, hp=110, atk=38, def=25, speed=20, energy=30, bounty=1111000000]", true, "TC4 Zoro after special", ok);
    expectCharacterState(cp9[0], "CP9[name=Jabra, hp=71, atk=33, def=30, speed=24, energy=50, doriki=2180]", true, "TC4 Jabra after Zoro special", ok);

    damage = strawHats[1]->specialSkill(cp9[1], context);
    expectInt(damage, 48, "TC4 Sanji special damage", ok);
    expectCharacterState(strawHats[1], "StrawHat[name=Sanji, hp=100, atk=34, def=18, speed=30, energy=27, bounty=1032000000]", true, "TC4 Sanji after special", ok);
    expectCharacterState(cp9[1], "CP9[name=Blueno, hp=62, atk=28, def=16, speed=26, energy=45, doriki=820]", true, "TC4 Blueno after Sanji special", ok);

    damage = strawHats[2]->attack(cp9[2], context);
    expectInt(damage, 0, "TC4 Chopper attack damage", ok);
    expectCharacterState(strawHats[2], "StrawHat[name=Chopper, hp=90, atk=20, def=15, speed=22, energy=55, bounty=1000]", true, "TC4 Chopper after attack", ok);
    expectCharacterState(cp9[2], "CP9[name=Kumadori, hp=115, atk=30, def=20, speed=20, energy=45, doriki=810]", true, "TC4 Kumadori after Chopper attack", ok);

    damage = strawHats[3]->specialSkill(cp9[3], context);
    expectInt(damage, 4, "TC4 Usopp special damage", ok);
    expectCharacterState(strawHats[3], "StrawHat[name=Usopp, hp=85, atk=24, def=14, speed=28, energy=34, bounty=500000000]", true, "TC4 Usopp after special", ok);
    expectCharacterState(cp9[3], "CP9[name=Fukurou, hp=96, atk=27, def=16, speed=13, energy=40, doriki=800]", true, "TC4 Fukurou after Usopp special", ok);

    damage = cp9[0]->specialSkill(strawHats[0], context);
    expectInt(damage, 25, "TC4 Jabra special damage", ok);
    expectCharacterState(cp9[0], "CP9[name=Jabra, hp=71, atk=33, def=30, speed=24, energy=32, doriki=2180]", true, "TC4 Jabra after special", ok);
    expectCharacterState(strawHats[0], "StrawHat[name=Zoro, hp=85, atk=38, def=25, speed=20, energy=30, bounty=1111000000]", true, "TC4 Zoro after Jabra special", ok);

    damage = cp9[1]->specialSkill(strawHats[1], context);
    expectInt(damage, 39, "TC4 Blueno special damage", ok);
    expectCharacterState(cp9[1], "CP9[name=Blueno, hp=62, atk=28, def=16, speed=26, energy=30, doriki=820]", true, "TC4 Blueno after special", ok);
    expectCharacterState(strawHats[1], "StrawHat[name=Sanji, hp=61, atk=34, def=18, speed=30, energy=27, bounty=1032000000]", true, "TC4 Sanji after Blueno special", ok);

    damage = cp9[2]->specialSkill(strawHats[2], context);
    expectInt(damage, 96, "TC4 Kumadori special damage", ok);
    expectCharacterState(cp9[2], "CP9[name=Kumadori, hp=115, atk=30, def=20, speed=20, energy=29, doriki=810]", true, "TC4 Kumadori after special", ok);
    expectCharacterState(strawHats[2], "StrawHat[name=Chopper, hp=0, atk=20, def=15, speed=22, energy=55, bounty=1000]", false, "TC4 Chopper after Kumadori special", ok);

    damage = cp9[3]->specialSkill(strawHats[3], context);
    expectInt(damage, 22, "TC4 Fukurou special damage", ok);
    expectCharacterState(cp9[3], "CP9[name=Fukurou, hp=96, atk=27, def=16, speed=13, energy=26, doriki=800]", true, "TC4 Fukurou after special", ok);
    expectCharacterState(strawHats[3], "StrawHat[name=Usopp, hp=63, atk=24, def=14, speed=28, energy=34, bounty=500000000]", true, "TC4 Usopp after Fukurou special", ok);

    for (int i = 0; i < 4; i++) delete strawHats[i];
    for (int i = 0; i < 4; i++) delete cp9[i];
    return ok;
}

// =======================
// Testcase 5: Full battle through EniesLobbyBattle
// This writes its own input file. The target result matches the basic Excel testcase style.
// =======================
bool tc5() {
    bool ok = true;
    string filename = "tc5_input.txt";
    string content =
        "CONTEXT 15 50 50 0 93 6\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "STRAW_HAT Luffy 100 10 0 1 0 1\n"
        "CP9 Lucci 60 0 0 1 0 1\n";
    writeFile(filename, content);

    EniesLobbyBattle battle(filename);
    battle.runBattle();
    expectString(battle.getResult(),
                 "STRAW_HAT_WIN_BY_DEFEAT_CP9 6 20 50 50 0 93",
                 "TC5 full battle result",
                 ok);
    return ok;
}

#endif
