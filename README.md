# Enies Lobby Basic Test Tool - CO1027 Assignment 2

Tool này mô phỏng cấu trúc testcase giống file `asignment_1_testcase.zip`, nhưng dùng cho bài Assignment 2 `eniesLobby`.

## Cách dùng nhanh

1. Giải nén các file trong tool này vào thư mục bài tập, cùng cấp với:

```txt
eniesLobby.h
eniesLobby.cpp
main.h
main.cpp
```

2. Chạy toàn bộ testcase:

```bash
bash run_tests.sh
```

3. Chạy riêng một testcase, ví dụ testcase 3:

```bash
bash run_tests.sh 3
```

Trên Windows CMD có MinGW:

```bat
run_tests.bat
run_tests.bat 3
```

## Nội dung testcase

- Testcase 1: `str()`, `isStrawHat()`, `isCP9()` của 7 nhân vật StrawHat.
- Testcase 2: `str()`, `isStrawHat()`, `isCP9()` của 7 nhân vật CP9.
- Testcase 3: attack/special của Luffy, Nami, Franky, Lucci, Kaku, Kalifa.
- Testcase 4: attack/special của Zoro, Sanji, Chopper, Usopp, Jabra, Blueno, Kumadori, Fukurou.
- Testcase 5: full battle bằng `EniesLobbyBattle`, tool tự tạo file `tc5_input.txt`.

## Lưu ý

Tool này chỉ phục vụ test local. Khi nộp bài, chỉ nộp 2 file theo đề:

```txt
eniesLobby.h
eniesLobby.cpp
```
