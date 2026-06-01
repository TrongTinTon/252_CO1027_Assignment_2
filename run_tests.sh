#!/bin/sh
g++ -o tester main_test.cpp eniesLobby.cpp -I . -std=c++11 && ./tester "$@"
