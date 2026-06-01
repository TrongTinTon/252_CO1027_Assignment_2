g++ -o tester.exe main_test.cpp eniesLobby.cpp -I . -std=c++11
if %errorlevel% neq 0 exit /b %errorlevel%
tester.exe %*
