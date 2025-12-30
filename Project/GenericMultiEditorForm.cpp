#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <functional>
#include <vector>

using namespace std;

struct Field {
    string label;
    int len;
    string regexStr;
    // void* is a Pointer to Something we don't know what its type yet
    // we will cast it to specific type when we use it inside
    // function<> allows you to store a function in a variable like PHP
    function<void(void*, const char*)> setter;

    string oldValue = "";
};

void gotoxy(int x,int y);
void textattr(int i);
bool showForm(void* object, vector<Field>& fields, string errorMessage = "", int errorCount = 0, const int inputX = 18);
void display(int nChar, char* arr, int cursor, int xPos, int yPos, int len);
bool isCharAllowed(char ch, const string& regexStr);
char** multiLineEditor(int* xPos, int* yPos, int* len, char** str, string* regexStrs, int N, int errorCount);
//int displayMenu(const vector<string>& menu, const string& MenuTitle = "=======Menu======");
int displayMenu(const vector<string>& menu, const string& MenuTitle="=======Menu======", const string& MenuDescriptionTitle="",  const string& MenuDescription="",int YPositionOfESC = 3);

// Generic Form to fill an Object
bool showForm(void* object, vector<Field>& fields, string errorMessage, int errorCount, const int inputX) {
    int n = fields.size();
    const int labelX = 2;
    //const int inputX = 21;
    const int startY = 1;

    int* xPos = new int[n];
    int* yPos = new int[n];
    int* len  = new int[n];
    char** oldValues = new char*[n];
    string* regexStrs = new string[n];

    system("cls");

    for (int i = 0; i < n; ++i) {
        int y = startY + i * 2;

        gotoxy(labelX, y);
        cout << fields[i].label;

        xPos[i] = inputX;
        yPos[i] = y;
        len[i]  = fields[i].len;
        oldValues[i] = new char[len[i] + 1];
        regexStrs[i] = fields[i].regexStr;
        // Writes len[i] (+1 for null-terminator char) characters from fields[i].oldvalue if exist
        // to oldValues[i] using "%s" format (which means as string)
        int len = fields[i].oldValue.size();
        if (fields[i].len < len) len = fields[i].len;
        snprintf(oldValues[i], len+1, "%.*s", len, fields[i].oldValue.c_str());
    }

    if (!errorMessage.empty()) {
        gotoxy(0, yPos[n-1] + 2);
        cout << "\x1b[31m" << errorMessage << "\x1b[0m\n";
        gotoxy(xPos[0], yPos[0]);
    }

    char** values = multiLineEditor(xPos, yPos, len, oldValues, regexStrs, n,errorCount);
    if (values == nullptr) return false;

    for (int i = 0; i < n; ++i) {
        fields[i].setter(object, values[i]);
    }
    return true;
}

void gotoxy(int x,int y)
{
    COORD coord={0,0};
    coord.X=x;
    coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

void textattr(int i)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
}

char** multiLineEditor(int* xPos, int* yPos, int* len, char** str, string* regexStrs, int N, int errorCount){
    int index = 0;

    //char** str = new char*[N];
    char** current = new char*[N];
    char** first = new char*[N];
    char** last = new char*[N];
    int* cursor = new int[N]{};

    for(int i = 0; i < N; ++i){
        current[i] = first[i] = str[i];
        last[i] = str[i] + (strlen(str[i]));
        display((int)strlen(str[i]), str[i], cursor[i], xPos[i], yPos[i], len[i]);
    }

    gotoxy(0, yPos[N-1] + errorCount + 3);
    cout << "Press ESC to back.";
    gotoxy(xPos[0], yPos[0]);
    while(true){
        char ch = getch();
        switch(ch){
        case -32:{
            ch = getch();
            switch(ch){
            case 77: // Right Arrow
                if (current[index] < last[index]){
                    gotoxy(++cursor[index] + xPos[index], yPos[index]);
                    ++current[index];
                }
                break;
            case 75: // Left Arrow
                if (current[index] > first[index])
                {
                    gotoxy(--cursor[index] + xPos[index], yPos[index]);
                    --current[index];
                }
                break;
            case 80: // Down Arrow
                if (index < N - 1){
                    index++;
                    gotoxy(cursor[index] + xPos[index], yPos[index]);
                }
                break;
            case 72: // Up Arrow
                if (index > 0){
                    index--;
                    gotoxy(cursor[index] + xPos[index], yPos[index]);
                }
                break;
            case 71: // Home Key
                cursor[index] = 0;
                gotoxy(cursor[index] + xPos[index], yPos[index]);
                current[index] = first[index];
                break;
            case 79: // End Key
                cursor[index] = last[index] - first[index];
                gotoxy(cursor[index] + xPos[index], yPos[index]);
                current[index] = last[index];
                break;
            case 83: // Delete
                // AB|CD
                // AB|D
                if(current[index] < last[index]){
                    memmove(current[index], current[index] + 1, last[index] - current[index] - 1);
                    last[index]--;
                }
                display((last[index] - first[index]), str[index], cursor[index], xPos[index], yPos[index], len[index]);
                break;
            }
            break;
        }
        case 27: // Esc Key
            system("cls");
            return nullptr;
        case 8: // Backspace
        {
            // AB|CD
            // A|CD
            if(current[index] > first[index]){
                if(current[index] != last[index]){
                    memmove(current[index] - 1, current[index], last[index] - current[index]);
                }
                --cursor[index];
                --current[index];
                --last[index];
                display((last[index] - first[index]), str[index], cursor[index], xPos[index], yPos[index], len[index]);
            }
            break;
        }
        case '\t':
            if (index == N - 1){
                index = 0;
            } else if (index < N - 1){
                ++index;
            }
            gotoxy(cursor[index] + xPos[index], yPos[index]);
            break;
        case '\r': { // Enter
            for(int i = 0; i < N; ++i){
                *last[i] = '\0';
            }
            return str;
        }
        default:
            if((last[index] == &str[index][len[index]]) || !isCharAllowed(ch, regexStrs[index])){
                break;
            }

            if (current[index] < last[index]) {
                // AB|CB
                // ABD|CB
                memmove(current[index] + 1, current[index], last[index] - current[index]);
            }
            // Else
            // AB|
            // ABC|
            *current[index] = ch;
            ++cursor[index];
            // advance the pointers
            ++current[index];
            ++last[index];

            display((last[index] - first[index]), str[index], cursor[index], xPos[index], yPos[index], len[index]);
        }
    }


    return str;
}

bool isCharAllowed(char ch, const string& regexStr) {
    for (int i = 0; i < regexStr.size(); ++i) {
        // Handle range like A-Z and
        if (i + 2 < regexStr.size() && regexStr[i + 1] == '-') {
            if (ch >= regexStr[i] && ch <= regexStr[i + 2])
                return true;

            i += 2; // skip "A-Z"
        }
        // Handle single character like M or F
        else {
            if (ch == regexStr[i])
                return true;
        }
    }
    return false;
}

void display(int nChar, char* str, int cursor, int xPos, int yPos, int len){
    gotoxy(xPos, yPos);
    textattr(0x071);
    // Display nChar from str
    printf("%.*s", nChar, str);
    if (nChar < len)
        // and Fill the gap with Spaces
        cout << string(len - nChar, ' ');
    textattr(0x007);
    gotoxy(cursor + xPos, yPos);
}

// int displayMenu(const vector<string>& menu, const string& MenuTitle){
//     const int nOption = menu.size();
//     bool abort = false;
//     int selected = 0;
//     do{
//         system("cls");
//         cout << "\n" << MenuTitle << "\n";
//         gotoxy(0, nOption*2 + 3);
//         cout << "Press ESC to back.";

//         for(int i = 0; i < nOption; ++i){
//             gotoxy(2, 3+i*2);
//             if (i == selected){
//                 textattr(0x070);
//             } else {
//                 textattr(0x007);
//             }
//             cout << menu[i];
//         }
//         textattr(0x007); // reset colors
//         char ch = getch();
//         switch(ch){
//         case -32:{
//             ch = getch();
//             switch(ch){
//             case 72: // Up Arrow
//                 selected = (selected - 1 + nOption) % nOption;
//                 break;
//             case 80: // Down Arrow
//                 selected = (selected + 1) % nOption;
//                 break;
//             case 71: // Home Key
//                 selected = 0;
//                 break;
//             case 79: // End Key
//                 selected = nOption - 1;
//                 break;
//             }
//             break;
//         }
//         case 27: // Esc Key
//             return -1;
//         case '\r':// Enter
//             return (selected + 1);
//         }
//     } while(!abort);

//     return -1;
// }


int displayMenu(const vector<string>& menu, const string& MenuTitle,const string& MenuDescriptionTitle ,  const string& MenuDescription,int YPositionOfESC){
    const int nOption = menu.size();
    bool abort = false;
    int selected = 0;
    do{
        system("cls");
        cout << "\n" << MenuTitle << "\n";
        gotoxy(0, nOption*2 + YPositionOfESC);
        cout << "Press ESC to back.";

        gotoxy(2, 2);
        for(int i = 0; i < nOption; ++i){
            gotoxy(2, 3+i*2);
            if (i == selected){
                textattr(0x070);
            } else {
                textattr(0x007);
            }
            cout << menu[i];
        }

        textattr(0x007); // reset colors

        if (MenuDescriptionTitle != ""){
            cout<< "\n" + MenuDescriptionTitle<< endl;
            if (!MenuDescription.empty())
                cout << "\n" << MenuDescription << endl;
        }

        char ch = getch();
        switch(ch){
        case -32:{
            ch = getch();
            switch(ch){
            case 72: // Up Arrow
                selected = (selected - 1 + nOption) % nOption;
                break;
            case 80: // Down Arrow
                selected = (selected + 1) % nOption;
                break;
            case 71: // Home Key
                selected = 0;
                break;
            case 79: // End Key
                selected = nOption - 1;
                break;
            }
            break;
        }
        case 27: // Esc Key
            return -1;
        case '\r':// Enter
            return (selected + 1);
        }
    } while(!abort);

    return -1;
}


//Example

// struct Student {
//     int id;
//     char name[30];
//     char grade;
// };


// int main()
// {
//     vector<Field> studentFields = {
//         {
//             "ID:",
//             6, '0', '9',
//             [](void* o, const char* v) {
//                 ((Student*)o)->id = atoi(v);
//             }
//         },
//         {
//             "Name:",
//             20, 'A', 'z',
//             [](void* o, const char* v) {
//                 strcpy(((Student*)o)->name, v);
//             }
//         },
//         {
//             "Grade:",
//             1, 'A', 'F',
//             [](void* o, const char* v) {
//                 ((Student*)o)->grade = v[0];
//             }
//         }
//     };

//     Student s{};
//     showForm(&s, studentFields);
// }
