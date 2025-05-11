#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#define NOMINMAX
#include <Windows.h>
using namespace std;

// Class
struct Node {
    // 0 : Black Stone, 1 : White Stone.
    int Which_Stone, x, y;
    Node* next;
    Node* prdv;

    Node(int W, int X, int Y);
    ~Node();
};
class LinkedListStack {
private:
    Node* top; // 스택의 맨 위 노드를 가리키는 포인터
    int Size{};

public:
    LinkedListStack();
    ~LinkedListStack();

    void push(int Which_Stone, int x, int y);
    Node* pop();
    Node* peek();
    bool isEmpty();
    void clear();
    int size() const;
};

// Variables
int HContinousBoard[20][20]{}, VContinousBoard[20][20]{};
int HBContinousMax{}, HWContinousMax{}, HTContinousMax{}, HStoneSum[4][20]{};
int VBContinousMax{}, VWContinousMax{}, VTContinousMax{}, VStoneSum[4][20]{};
int LHDBContinousMax{}, LHDWContinousMax{}, LHDTContinousMax{}, LHDStoneSum[4][38]{};
int LLDBContinousMax{}, LLDWContinousMax{}, LLDTContinousMax{}, LLDStoneSum[4][38]{};
int CountBlackStone{}, CountWhiteStone{}, TContinousMax{};
string map[20][20]{}, player[2]{ " ○"," ●" };   // 1. black, 2. white
string MARKER{ "@" };                           // 승리 마커
// ** Windows 콘솔 색상 정의 **
const WORD COLOR_DEFAULT = 0x07;                // 흰색 배경에 회색 글꼴
const WORD COLOR_YELLOW = 0x0E;                 // 검은색 배경에 밝은 노란색 글꼴
const WORD COLOR_RED = 0x0C;                    // 검은색 배경에 밝은 빨간색 글꼴
const WORD COLOR_GREEN = 0x0A;                  // 검은색 배경에 밝은 초록색 글꼴
const WORD COLOR_TEAL = 0x0B;                   // 검은색 배경에 밝은 청록색 글꼴
// ** Stack **
LinkedListStack History;
LinkedListStack UndoStack;

// Define Functions
void setConsoleColor(WORD color);
void gotoxy(int x, int y);
void Initialize();
void ShowBoard();
int PlaceBStone();
int PlaceWStone();
int CheckBoard();
int Horizonal_Checker();
int Vertical_Checker();
int LHDiagonal_Checker();
int LLDiagonal_Checker();
void SaveFileClear();
int LoadFile();
int SaveFile();
int Redo();
int Undo();
void SelectCurrentState();


// Main
int main()
{
    int PlayerTurn{ 0 };

    Initialize();
    // 세이브 파일 선택
    while (true) {
        int CommandNum{};


        cout << "--------** Select Menu **--------\n"
            << "\t1. New_Game\n"
            << "\t2. Load_Game\n"
            << "\t0. Exit\n";
        if (!(cin >> CommandNum)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "입력이 잘못 되었습니다.n";
            continue;
        }
        else if (1 == CommandNum) {
            SaveFileClear();
            break;
        }
        else if (2 == CommandNum) {
            if (1 == LoadFile()) {
                cout << "파일을 로드하는데 실패했습니다.\n";
            }
            break;
        }
        else if (0 == CommandNum) {
            return 0;
        }
        else {
            cout << "틀린 커멘드 입니다. 다시 입력해주세요.\n";
            continue;
        }
    }
    // 게임 진행 사이클.
    bool ExitFlag{ false };
    while (!ExitFlag) {
        system("cls");
        ShowBoard();

        TContinousMax = max(HTContinousMax, VTContinousMax);
        for (int i = 0; i < 2; ++i) {
            for (int j = 1; j < 20; ++j) {
                if (HStoneSum[i][j] == TContinousMax && 0 == i) {
                    cout << "가로줄에서 가장 긴 연속된 흑돌의 갯수 : " << HTContinousMax << "\n\n";
                    break;
                }
                else if (HStoneSum[i][j] == TContinousMax && 1 == i) {
                    cout << "세로줄에서 가장 긴 연속된 백돌의 갯수 : " << VTContinousMax << "\n\n";
                    break;
                }
            }
        }

        while (true) {
            int ActionCommandNum;

            cout << "--------** Action Menu **--------\n"
                << "\t1. Place Stone\n"
                << "\t2. Modify State\n"
                << "\t0. Finish Game\n";

            if (!(cin >> ActionCommandNum)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "입력이 잘못 되었습니다. 다시 입력해주세요.\n";
                continue;
            }
            // 돌 놓기 사이클
            else if (1 == ActionCommandNum) {
                cout << "흑돌 : " << player[0] << ", 백돌 : " << player[1] << endl;
                cout << "입력은 (세로축, 가로축) 입니다.\n";
                cout << "입력에 0이 입력되면 프로그램이 종료됩니다.\n";

                if (0 == PlayerTurn) {
                    PlayerTurn = PlaceBStone();
                    if (PlayerTurn == -1) {
                        system("cls");
                        break;
                    }
                    if (1 == CheckBoard()) {
                        setConsoleColor(COLOR_TEAL);
                        cout << "\n흑돌 승리!\n";
                        setConsoleColor(COLOR_DEFAULT);
                        break;
                    }
                    PlayerTurn = 1;
                    break;
                }
                else {
                    PlayerTurn = PlaceWStone();
                    if (PlayerTurn == -1) {
                        system("cls");
                        break;
                    }
                    if (2 == CheckBoard()) {
                        setConsoleColor(COLOR_TEAL);
                        cout << "\n백돌 승리!\n";
                        setConsoleColor(COLOR_DEFAULT);
                        break;
                    }
                    PlayerTurn = 0;
                    break;
                }
            }
            // Undo, Redo 사이클
            else if (2 == ActionCommandNum) {
                while (true) {
                    int ModifyCommandNum;

                    cout << "-------- ** Modify Menu **--------\n"
                        << "\t1. Undo\n"    
                        << "\t2. Redo\n"
                        << "\t3. Select Current State\n"
                        << "3번은 Undo 및 Redo 한 상태를 결정하는 커멘드입니다. *주의하세요 되돌릴 수 없습니다.*\n";

                    if (!(cin >> ModifyCommandNum)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "입력이 잘못 되었습니다. 다시 입력해주세요.\n";
                        continue;
                    }
                    else if (1 == ModifyCommandNum) {
                        Undo();
                    }
                    else if (2 == ModifyCommandNum) {
                        Redo();
                    }
                    else if (3 == ModifyCommandNum) {
                        SelectCurrentState();
                        if (History.size() % 2 == 0) {
                            PlayerTurn = 0;
                        }
                        else {
                            PlayerTurn = 1;
                        }
                        break;
                    }
                    else {
                        cout << "잘못된 커멘드 입력입니다. 다시 입력해주세요.\n";
                        continue;
                    }
                }
            }
            else if (0 == ActionCommandNum) {
                ExitFlag = true;
                break;
            }
            else {
                cout << "잘못된 커멘드 입력입니다. 다시 입력해주세요.\n";
                continue;
            }
        }
    }
    ShowBoard();
    cout << "총 " << CountBlackStone + CountWhiteStone << "착수\n";
    cout << "흑돌의 갯수 : " << CountBlackStone << endl;
    cout << "백돌의 갯수 : " << CountWhiteStone << endl;

    if (0 == SaveFile()) {
        cout << "저장되었습니다.\n";
    }
    else {
        cout << "저장 실패 했습니다.\n";
    }
}

// Console Functions
void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
void gotoxy(int x, int y) {
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// Function Prepare for Game
void Initialize()
{
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            map[i][j] = "+";
        }
    }
    return;
}
void ShowBoard()
{
    cout << right << setw(108) << "**가로줄에 대해 최대 연속된 돌의 갯수 v  v\n";
    cout << right << setw(98) << "**가로줄에 대해 흑,백돌의 합 v  v\n";
    cout << "   |";
    for (int j = 1; j < 20; ++j)
        cout << setw(2) << j << setw(2) << " |";
    cout << setw(2) << " 흑 백 흑 백";
    cout << endl;

    for (int i = 1; i < 20; ++i) {
        cout << setw(2) << i << setw(2) << "|";
        for (int j = 1; j < 20; ++j) {
            if (map[i][j] == player[0] || map[i][j] == player[1]) {
                if (HContinousBoard[i][j] == HTContinousMax) {
                    setConsoleColor(COLOR_GREEN);
                    cout << setw(2) << map[i][j];
                    setConsoleColor(COLOR_DEFAULT);
                    cout << setw(2) << "|";
                    continue;
                }
                setConsoleColor(COLOR_YELLOW);
                cout << setw(2) << map[i][j];
                setConsoleColor(COLOR_DEFAULT);
            }
            else if (map[i][j] == MARKER) {
                setConsoleColor(COLOR_RED);
                cout << setw(2) << map[i][j];
                setConsoleColor(COLOR_DEFAULT);
            }
            else cout << setw(2) << map[i][j];
            cout << setw(2) << "|";
        }
        // 각 가로 줄에 대해 몇개의 돌이 놓여져 있는지 출력
        cout << setw(3) << HStoneSum[0][i] << setw(3) << HStoneSum[1][i];
        // 각 가로 줄에 대해 최대 몇개의 연속된 돌이 놓여져 있는지 출력
        cout << setw(3) << HStoneSum[2][i] << setw(3) << HStoneSum[3][i];

        cout << endl << "  ";
        for (int j = 1; j < 40; ++j) cout << "ㅡ";
        cout << endl;
    }
    // 각 세로 줄에 대해 몇개의 돌이 놓여져 있는지 출력
    for (int i = 0; i < 4; ++i) {
        if (i % 2 == 0) cout << "흑 :";
        else cout << "백 :";
        for (int j = 1; j < 20; ++j) {
            cout << setw(2) << VStoneSum[i][j] << setw(2) << " |";
        }
        cout << endl;
    }
    // 좌상우하 대각선에 대해 최대 몇개의 연속된 돌이 놓여져 있는지 출력
    cout << "\n좌상우하 대각선에 대해 최대 몇개의 연속된 돌 (19, 1) ~ (1, 19)\n";
    for (int i = 2; i < 4; ++i) {
        if (i % 2 == 0) cout << "흑 :";
        else cout << "백 :";
        for (int j = 1; j < 38; ++j) {
            cout << LHDStoneSum[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    // 좌하우상 대각선에 대해 최대 몇개의 연속된 돌이 놓여져 있는지 출력
    cout << "좌하우상 대각선에 대해 최대 몇개의 연속된 돌 (1, 1) ~ (19, 19)\n";
    for (int i = 2; i < 4; ++i) {
        if (i % 2 == 0) cout << "흑 :";
        else cout << "백 :";
        for (int j = 1; j < 38; ++j) {
            cout << LLDStoneSum[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\n\n";
}

// PlaceStones
int PlaceBStone()
{
    int x, y;

    while (true) {
        cout << "흑돌 차례 : ";
        if (!(cin >> x >> y)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "잘못된 입력입니다. 다시 입력하세요.\n";
            continue;
        }
        if (x == 0 || y == 0) return -1;
        if (x > 19 || x < 0 || y > 19 || y < 0) {
            cout << "입력 범위가 잘못 되었습니다. 다시 입력하세요.\n";
            continue;
        }
        if (map[x][y] != "+") {
            cout << "돌이 이미 놓여져 있습니다.\n";
            continue;
        }
        break;
    }

    History.push(0, x, y);

    map[x][y] = player[0];
    CountBlackStone++;
    VStoneSum[0][y]++;
    HStoneSum[0][x]++;

    return 1;
}
int PlaceWStone()
{
    int x, y;

    while (true) {
        cout << "백돌 차례 : ";
        if (!(cin >> x >> y)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "잘못된 입력입니다. 다시 입력하세요.\n";
            continue;
        }
        if (x > 19 || x < 0 || y > 19 || y < 0) {
            cout << "입력 범위가 잘못 되었습니다. 다시 입력하세요.\n";
            continue;
        }
        if (x == 0 || y == 0) return -1;
        if (map[x][y] != "+") {
            cout << "돌이 이미 놓여져 있습니다.\n";
            continue;
        }
        break;
    }

    History.push(1, x, y);

    map[x][y] = player[1];
    CountWhiteStone++;
    VStoneSum[1][y]++;
    HStoneSum[1][x]++;

    return 1;
}

// Board Checking Function
int CheckBoard()
{
    system("cls");
    int Status{};
    //1. 가로
    Status = Horizonal_Checker();
    if (Status == 1)
        return 1;
    else if (Status == 2)
        return 2;

    //2. 좌상우하
    Status = LHDiagonal_Checker();
    if (Status == 1)
        return 1;
    else if (Status == 2)
        return 2;

    //3. 세로
    Status = Vertical_Checker();
    if (Status == 1)
        return 1;
    else if (Status == 2)
        return 2;

    //4. 좌하우상
    Status = LLDiagonal_Checker();
    if (Status == 1)
        return 1;
    else if (Status == 2)
        return 2;

    return 0;
}
int Horizonal_Checker()
{
    int continuous_counter{ 1 };
    int SLBM{}, SLWM{};

    for (int i = 1; i < 20; i++) {
        SLBM = 0; SLWM = 0;
        for (int j = 1; j < 20; j++) {
            if (map[i][j] == player[0]) {                           //흑돌일때
                continuous_counter = 1;
                for (int k = 1; k < 5; k++) {
                    if (j + k < 20 && map[i][j + k] == player[0]) continuous_counter++;
                    else break;
                }
                SLBM = max(continuous_counter, SLBM);
                for (int l = 0; l < continuous_counter; l++)
                    HContinousBoard[i][j + l] = SLBM;
                if (5 == continuous_counter) {
                    for (int k = 0; k < 5; ++k)
                        map[i][j + k] = MARKER;
                    return 1;
                }
            }

            else if (map[i][j] == player[1]) {                      //백돌일때
                continuous_counter = 1;
                for (int k = 1; k < 5; k++) {
                    if (j + k < 20 && map[i][j + k] == player[1]) continuous_counter++;
                    else break;
                }
                SLWM = max(continuous_counter, SLWM);
                for (int l = 0; l < continuous_counter; l++)
                    HContinousBoard[i][j + l] = SLWM;
                if (5 == continuous_counter) {
                    for (int k = 0; k < 5; ++k)
                        map[i][j + k] = MARKER;
                    return 2;
                }
            }
        }

        if (SLBM > SLWM) {
            HStoneSum[2][i] = SLBM;
            HStoneSum[3][i] = -1;
        }
        else if (SLBM < SLWM) {
            HStoneSum[3][i] = SLWM;
            HStoneSum[2][i] = -1;
        }
        else {
            HStoneSum[2][i] = SLBM;
            HStoneSum[3][i] = SLWM;
        }

        HBContinousMax = max(HBContinousMax, SLBM);
        HWContinousMax = max(HWContinousMax, SLWM);
        HTContinousMax = max(HBContinousMax, HWContinousMax);
    }

    return 0;
}
int Vertical_Checker()
{
    int continuous_counter{ 1 };
    int SLBM{}, SLWM{};
    for (int i = 1; i < 20; i++) {
        SLBM = 0; SLWM = 0;
        for (int j = 1; j < 20; j++) {
            if (map[j][i] == player[0]) {                           //흑돌일때
                continuous_counter = 1;
                for (int k = 1; k < 5; k++) {
                    if (j + k < 20 && map[j + k][i] == player[0]) continuous_counter++;
                    else break;
                }
                SLBM = max(continuous_counter, SLBM);
                if (5 == continuous_counter) {
                    for (int k = 0; k < 5; ++k)
                        map[j + k][i] = MARKER;
                    return 1;
                }
            }

            else if (map[i][j] == player[1]) {                      //백돌일때
                continuous_counter = 1;
                for (int k = 1; k < 5; k++) {
                    if (j + k < 20 && map[j + k][i] == player[1]) continuous_counter++;
                    else break;
                }
                SLWM = max(continuous_counter, SLWM);
                if (5 == continuous_counter) {
                    for (int k = 0; k < 5; ++k)
                        map[j + k][i] = MARKER;
                    return 2;
                }
            }
        }

        if (SLBM > SLWM) {
            VStoneSum[2][i] = SLBM;
            VStoneSum[3][i] = -1;
        }
        else if (SLBM < SLWM) {
            VStoneSum[3][i] = SLWM;
            VStoneSum[2][i] = -1;
        }
        else {
            VStoneSum[2][i] = SLBM;
            VStoneSum[3][i] = SLWM;
        }

        VBContinousMax = max(VBContinousMax, SLBM);
        VWContinousMax = max(VWContinousMax, SLWM);
        VTContinousMax = max(VBContinousMax, VWContinousMax);
    }

    return 0;
}
int LHDiagonal_Checker()
{
    int continuous_counter{ 1 }, MaxTemp{}, DiagonalCount{ 1 };
    int SLBM{}, SLWM{};

    for (DiagonalCount = 1; DiagonalCount < 38; ++DiagonalCount) {      //37개의 대각선을 구분지어서 하기 위함.
        SLBM = 0, SLWM = 0;
        if (DiagonalCount < 19) {                   //왼쪽 벽에서 시작하는 대각선
            int rightside = 1; SLBM = 0, SLWM = 0;
            for (int leftside = 19; leftside > 0; --leftside) {
                if (map[leftside][rightside] == player[0]) {     //흑돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (leftside + k < 20 && rightside + k < 20 && map[leftside + k][rightside + k] == player[0])
                            continuous_counter++;
                        else break;
                    }
                    SLBM = max(continuous_counter, SLBM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[leftside + k][rightside + k] = MARKER;
                        return 1;
                    }
                }
                else if (map[leftside][rightside] == player[1]) {     //백돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (leftside + k < 20 && rightside + k < 20 && map[leftside + k][rightside + k] == player[1])
                            continuous_counter++;
                        else break;
                    }
                    SLWM = max(continuous_counter, SLWM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[leftside + k][rightside + k] = MARKER;
                        return 1;
                    }
                }
            }
        }
        else if (DiagonalCount == 19) {             // 주 대각선
            int bothside = 1; SLBM = 0, SLWM = 0;
            for (bothside = 1; bothside < 20; ++bothside) {
                if (map[bothside][bothside] == player[0]) {     //흑돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (bothside + k < 20 && map[bothside + k][bothside + k] == player[0])
                            continuous_counter++;
                        else break;
                    }
                    SLBM = max(continuous_counter, SLBM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[bothside + k][bothside + k] = MARKER;
                        return 1;
                    }
                }
                else if (map[bothside][bothside] == player[1]) {     //백돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (bothside + k < 20 && map[bothside + k][bothside + k] == player[1])
                            continuous_counter++;
                        else break;
                    }
                    SLWM = max(continuous_counter, SLWM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[bothside + k][bothside + k] = MARKER;
                        return 1;
                    }
                }
            }
        }
        else {                    // 위쪽 벽에서 시작하는 대각선
            int leftside = 1; SLBM = 0, SLWM = 0;
            for (int rightside = 2; leftside < 20; ++leftside) {
                if (map[leftside][rightside] == player[0]) {     //흑돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (leftside + k < 20 && rightside + k < 20 && map[leftside + k][rightside + k] == player[0])
                            continuous_counter++;
                        else break;
                    }
                    SLBM = max(continuous_counter, SLBM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[leftside + k][rightside + k] = MARKER;
                        return 1;
                    }
                }
                else if (map[leftside][rightside] == player[1]) {     //백돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (leftside + k < 20 && rightside + k < 20 && map[leftside + k][rightside + k] == player[1])
                            continuous_counter++;
                        else break;
                    }
                    SLWM = max(continuous_counter, SLWM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[leftside + k][rightside + k] = MARKER;
                        return 1;
                    }
                }
            }
        }
        if (SLBM > SLWM) {
            LHDStoneSum[2][DiagonalCount] = SLBM;
            LHDStoneSum[3][DiagonalCount] = -1;
        }
        else if (SLBM < SLWM) {
            LHDStoneSum[3][DiagonalCount] = SLWM;
            LHDStoneSum[2][DiagonalCount] = -1;
        }
        else {
            LHDStoneSum[2][DiagonalCount] = SLBM;
            LHDStoneSum[3][DiagonalCount] = SLWM;
        }

        LHDBContinousMax = max(LHDBContinousMax, SLBM);
        LHDWContinousMax = max(LHDWContinousMax, SLWM);
        LHDTContinousMax = max(LHDBContinousMax, LHDWContinousMax);
    }

    return 0;
}
int LLDiagonal_Checker() {
    int continuous_counter{ 1 }, MaxTemp{}, DiagonalCount{ 1 };
    int SLBM{}, SLWM{};

    for (DiagonalCount = 1; DiagonalCount < 38; ++DiagonalCount) { // 37개의 대각선을 구분지어서 하기 위함.
        SLBM = 0, SLWM = 0;
        if (DiagonalCount < 19) { // 왼쪽 벽에서 시작하는 우상향 대각선
            int rightside = DiagonalCount;
            for (int leftside = 19; leftside > 0; --leftside) {
                if (rightside > 0 && rightside < 20) {
                    if (map[leftside][rightside] == player[0]) { // 흑돌일때
                        continuous_counter = 1;
                        for (int k = 1; k < 5; k++) {
                            if (leftside + k < 20 && rightside - k > 0 && map[leftside + k][rightside - k] == player[0])
                                continuous_counter++;
                            else break;
                        }
                        SLBM = max(continuous_counter, SLBM);
                        if (5 == continuous_counter) {
                            for (int k = 0; k < 5; ++k)
                                map[leftside + k][rightside - k] = MARKER;
                            return 1;
                        }
                    }
                    else if (map[leftside][rightside] == player[1]) { // 백돌일때
                        continuous_counter = 1;
                        for (int k = 1; k < 5; k++) {
                            if (leftside + k < 20 && rightside - k > 0 && map[leftside + k][rightside - k] == player[1])
                                continuous_counter++;
                            else break;
                        }
                        SLWM = max(continuous_counter, SLWM);
                        if (5 == continuous_counter) {
                            for (int k = 0; k < 5; ++k)
                                map[leftside + k][rightside - k] = MARKER;
                            return 2;
                        }
                    }
                }
                SLBM = 0, SLWM = 0;
            }
        }
        else if (DiagonalCount == 19) { // 주 반대각선 (좌하 - 우상)
            for (int bothside = 1; bothside < 20; ++bothside) {
                if (map[bothside][20 - bothside] == player[0]) { // 흑돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (bothside + k < 20 && 20 - bothside - k > 0 && map[bothside + k][20 - bothside - k] == player[0])
                            continuous_counter++;
                        else break;
                    }
                    SLBM = max(continuous_counter, SLBM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[bothside + k][20 - bothside - k] = MARKER;
                        return 1;
                    }
                }
                else if (map[bothside][20 - bothside] == player[1]) { // 백돌일때
                    continuous_counter = 1;
                    for (int k = 1; k < 5; k++) {
                        if (bothside + k < 20 && 20 - bothside - k > 0 && map[bothside + k][20 - bothside - k] == player[1])
                            continuous_counter++;
                        else break;
                    }
                    SLWM = max(continuous_counter, SLWM);
                    if (5 == continuous_counter) {
                        for (int k = 0; k < 5; ++k)
                            map[bothside + k][20 - bothside - k] = MARKER;
                        return 2;
                    }
                }
                SLBM = 0, SLWM = 0;
            }
        }
        else { // 위쪽 벽에서 시작하는 우상향 대각선
            int leftside = DiagonalCount - 18; SLBM = 0, SLWM = 0;
            for (int rightside = 19; leftside < 20; ++leftside) {
                if (rightside > 0 && rightside < 20) {
                    if (map[leftside][rightside] == player[0]) { // 흑돌일때
                        continuous_counter = 1;
                        for (int k = 1; k < 5; k++) {
                            if (leftside + k < 20 && rightside - k > 0 && map[leftside + k][rightside - k] == player[0])
                                continuous_counter++;
                            else break;
                        }
                        SLBM = max(continuous_counter, SLBM);
                        if (5 == continuous_counter) {
                            for (int k = 0; k < 5; ++k)
                                map[leftside + k][rightside - k] = MARKER;
                            return 1;
                        }
                    }
                    else if (map[leftside][rightside] == player[1]) { // 백돌일때
                        continuous_counter = 1;
                        for (int k = 1; k < 5; k++) {
                            if (leftside + k < 20 && rightside - k > 0 && map[leftside + k][rightside - k] == player[1])
                                continuous_counter++;
                            else break;
                        }
                        SLWM = max(continuous_counter, SLWM);
                        if (5 == continuous_counter) {
                            for (int k = 0; k < 5; ++k)
                                map[leftside + k][rightside - k] = MARKER;
                            return 2;
                        }
                    }
                }
                SLBM = 0, SLWM = 0;
                rightside--;
            }
        }

        if (SLBM > SLWM) {
            LLDStoneSum[2][DiagonalCount] = SLBM;
            LLDStoneSum[3][DiagonalCount] = -1;
        }
        else if (SLBM < SLWM) {
            LLDStoneSum[3][DiagonalCount] = SLWM;
            LLDStoneSum[2][DiagonalCount] = -1;
        }
        else {
            LLDStoneSum[2][DiagonalCount] = SLBM;
            LLDStoneSum[3][DiagonalCount] = SLWM;
        }

        LLDBContinousMax = max(LLDBContinousMax, SLBM);
        LLDWContinousMax = max(LLDWContinousMax, SLWM);
        LLDTContinousMax = max(LLDBContinousMax, LLDWContinousMax);
    }

    return 0;
}

// File Function
void SaveFileClear() {
    // 덮어쓰기 모드로 열면 기존 내용 삭제. -> 여기서 작동하는 플래그 : ios::trunc (디폴트)
    // 추가모드로 열려면 ios::app 플래그를 추가해 열자.
    ofstream out("SaveFile.txt", ios::trunc);
    if (!out) {
        cerr << "Error at opening SaveFile.txt\n";
        return;
    }

    out.close();
    return;
}
int LoadFile() {
    constexpr int MaxBufferSize = 64;
    constexpr int MaxFieldsSize = 16;
    ifstream in("SaveFile.txt");
    if (!in) {
        cerr << "Error at opening SaveFile.txt\n";
        return -1;
    }
    
    char buffer[MaxBufferSize]{};
    while (in.getline(buffer, MaxBufferSize)) {
        char* context{};
        char* token{};
        char fields[3][MaxFieldsSize]{};
        int index{};

        token = strtok_s(buffer, " ", &context);
        while (index < 2 && nullptr != token) {
            strcpy_s(fields[index++], MaxFieldsSize, token);
            token = strtok_s(nullptr, " ", &context);
        }
        if (index == 2 && token != nullptr) {
            strcpy_s(fields[index++], MaxFieldsSize, token);
        }

        if (index == 3) {
            int WhichStone{ atoi(fields[0]) }, x{ atoi(fields[1]) }, y{ atoi(fields[2]) };

            History.push(WhichStone, x, y);
            
            if (0 == WhichStone) {
                map[x][y] = player[0];
            }
            else {
                map[x][y] = player[1];
            }
        }
    }



    in.close();
    return 0;
}
int SaveFile() {
    ofstream out("SaveFile.txt");
    if (!out) {
        cerr << "Error at opening SaveFile.txt\n";
        return -1;
    }
    if (!History.isEmpty()) {
        // 1. History 스택을 새 스택에 뒤집어서 저장
        LinkedListStack newStack;
        while (!History.isEmpty()) {
            Node* temp = History.pop();
            newStack.push(temp->Which_Stone, temp->x, temp->y);
            delete temp;
        }

        // 2. 새 스택에서 하나씩 빼서 "<Which_Stone> <x> <y>\n"으로 한줄씩 저장.
        while (!newStack.isEmpty()) {
            Node* temp = newStack.pop();
            out << temp->Which_Stone << " " << temp->x << " " << temp->y << endl;
            delete temp;
        }
    }

    out.close();
    cout << "\n저장 완료: SaveFile.txt\n";
    return 0;
}
int Undo() {
    if (History.isEmpty()) {
        return -1;
    }
    Node* temp = History.pop();
    UndoStack.push(temp->Which_Stone, temp->x, temp->y);

    map[temp->x][temp->y] = "+";

    delete temp;

    system("cls");
    ShowBoard();

    return 0;
}
int Redo() {
    if (UndoStack.isEmpty()) {
        return -1;
    }
    Node* temp = UndoStack.pop();
    History.push(temp->Which_Stone, temp->x, temp->y);

    if (0 == temp->Which_Stone) {
        map[temp->x][temp->y] = player[0];
    }
    else {
        map[temp->x][temp->y] = player[1];
    }

    delete temp;

    system("cls");
    ShowBoard();

    return 0;
}
void SelectCurrentState() {
    UndoStack.clear();
}

// Node Function
Node::Node(int W, int X, int Y) : next(nullptr), prdv(nullptr) {
    Which_Stone = W; x = X; y = Y;
}
Node::~Node() {}

// LinkedListStack
LinkedListStack::LinkedListStack() : top(nullptr) {}
LinkedListStack::~LinkedListStack() {
    while (!isEmpty()) {
        pop();
    }
}
void LinkedListStack::push(int W, int X, int Y) {
    Node* newNode = new Node(W, X, Y);

    newNode->next = top;
    if (top) {
        top->prdv = newNode;
    }
    top = newNode;

    Size++;
}
Node* LinkedListStack::pop() {
    if (isEmpty()) {
        cerr << "Stack is empty\n";
        return nullptr;
    }

    Node* poppedNode = top;
    top = top->next;
    if (top != nullptr) {
        top->prdv = nullptr;
    }

    poppedNode->next = nullptr; // 분리
    return poppedNode;
}
Node* LinkedListStack::peek() {
    return top;
}
bool LinkedListStack::isEmpty() {
    return top == nullptr;
}
void LinkedListStack::clear() {
    while (!isEmpty()) {
        delete pop();
        Size--;
    }
}
int LinkedListStack::size() const {
    return Size;
}