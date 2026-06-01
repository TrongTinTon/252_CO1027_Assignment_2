#ifndef TC_CHECKER_ENIES_LOBBY_H
#define TC_CHECKER_ENIES_LOBBY_H

#include "eniesLobby.h"

struct TestStats {
    int passed;
    int failed;
};

inline string boolToString(bool value) {
    return value ? "true" : "false";
}

inline void writeFile(const string& filename, const string& content) {
    ofstream fout(filename.c_str());
    fout << content;
    fout.close();
}

inline void expectString(const string& actual, const string& expected, const string& label, bool& ok) {
    if (actual != expected) {
        ok = false;
        cout << "\n  [FAIL] " << label << endl;
        cout << "    Output: " << actual << endl;
        cout << "    Expect: " << expected << endl;
    }
}

inline void expectInt(int actual, int expected, const string& label, bool& ok) {
    if (actual != expected) {
        ok = false;
        cout << "\n  [FAIL] " << label << endl;
        cout << "    Output: " << actual << endl;
        cout << "    Expect: " << expected << endl;
    }
}

inline void expectBool(bool actual, bool expected, const string& label, bool& ok) {
    if (actual != expected) {
        ok = false;
        cout << "\n  [FAIL] " << label << endl;
        cout << "    Output: " << boolToString(actual) << endl;
        cout << "    Expect: " << boolToString(expected) << endl;
    }
}

inline void expectCharacterState(Character* c,
                                 const string& expectedStr,
                                 bool expectedAlive,
                                 const string& label,
                                 bool& ok) {
    expectString(c->str(), expectedStr, label + " str()", ok);
    expectBool(c->isAlive(), expectedAlive, label + " isAlive()", ok);
}

#endif
