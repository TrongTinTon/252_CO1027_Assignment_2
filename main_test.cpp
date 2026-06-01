#include "eniesLobby.h"
#include "test/extra_tc_enies_lobby.h"

bool (*testcase[])() = {
    0,
    tc1, tc2, tc3, tc4, tc5
};

int parseIndex(const char* s) {
    int x = 0;
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] >= '0' && s[i] <= '9') {
            x = x * 10 + (s[i] - '0');
        }
        i++;
    }
    return x;
}

bool runOne(int i) {
    int total = (int)(sizeof(testcase) / sizeof(testcase[0])) - 1;
    if (i < 1 || i > total) {
        cout << "Invalid testcase index. Valid range: 1.." << total << endl;
        return false;
    }

    cout << "Testcase " << i << ": ";
    bool ok = testcase[i]();
    cout << (ok ? "Pass" : "Fail") << endl;
    return ok;
}

void runAll() {
    int passed = 0;
    int total = (int)(sizeof(testcase) / sizeof(testcase[0])) - 1;

    for (int i = 1; i <= total; i++) {
        if (runOne(i)) passed++;
    }

    cout << "==============================" << endl;
    cout << "Passed: " << passed << "/" << total << endl;
    cout << "Score : " << fixed << setprecision(2) << ((double)passed / total) * 10.0 << "/10.00" << endl;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        runAll();
    } else {
        runOne(parseIndex(argv[1]));
    }
    return 0;
}
