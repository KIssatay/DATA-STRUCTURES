#include <iostream>
#include <cstdlib>
#include <limits>
#include <string>
using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader() {
    cout << "========================================\n";
    cout << "  Carbon Emission Analysis - Array\n";
    cout << "========================================\n";
}

bool compileAndRun(const char* folder, const char* sources, const char* output) {
    cout << "\nCompiling " << folder << "...\n";
    
    string cmd = "cd \"";
    cmd += folder;
    cmd += "\" && g++ ";
    cmd += sources;
    cmd += " -o ";
    cmd += output;
    cmd += " -std=c++17 2>&1";
    
    int result = system(cmd.c_str());
    
    if (result != 0) {
        cout << "Compilation failed.\n";
        return false;
    }
    
    cout << "Running analysis...\n\n";
    
    string runCmd = "cd \"";
    runCmd += folder;
    runCmd += "\" && ./";
    runCmd += output;
    
    system(runCmd.c_str());
    return true;
}

int main() {
    string input;
    char choice;
    
    while (true) {
        clearScreen();
        printHeader();
        
        cout << "\nSelect dataset:\n";
        cout << "  [A] City A - dataset1-cityA.csv (200 records)\n";
        cout << "  [B] City B - dataset2-cityB.csv (178 records)\n";
        cout << "  [C] City C - dataset3-cityC.csv (122 records)\n";
        cout << "  [Q] Quit\n";
        cout << "\nYour choice: ";
        
        cin >> input;
        
        if (input.length() != 1) {
            cout << "\nPlease enter only one letter.\n";
            pressEnter();
            continue;
        }
        
        choice = static_cast<char>(toupper(static_cast<unsigned char>(input[0])));
        
        switch (choice) {
            case 'A':
                clearScreen();
                printHeader();
                cout << "\nCity A analysis\n";
                cout << "----------------------------------------\n";
                compileAndRun("DatasetA", "main_array.cpp ArrayFunctions.cpp", "cityA_array");
                pressEnter();
                break;
                
            case 'B':
                clearScreen();
                printHeader();
                cout << "\nCity B analysis\n";
                cout << "----------------------------------------\n";
                compileAndRun("DatasetB", "main_array.cpp ResidentArray.cpp", "cityB_array");
                pressEnter();
                break;
                
            case 'C':
                clearScreen();
                printHeader();
                cout << "\nCity C analysis\n";
                cout << "----------------------------------------\n";
                compileAndRun("DatasetC", "main_array.cpp array_methods.cpp list_methods.cpp", "cityC_array");
                pressEnter();
                break;
                
            case 'Q':
                clearScreen();
                cout << "\nThank you!\n\n";
                return 0;
                
            default:
                cout << "\nInvalid choice. Please select A, B, C, or Q.\n";
                pressEnter();
        }
    }
    
    return 0;
}
