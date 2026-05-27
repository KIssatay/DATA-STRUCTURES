#include "resident.hpp"
#include <chrono>
#include <iostream>
#include <limits>

using namespace std;
using namespace chrono;

typedef high_resolution_clock HRC;
typedef HRC::time_point TP;

inline TP now() { return HRC::now(); }
inline double elapsedMs(TP start) {
    return duration<double, milli>(HRC::now() - start).count();
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main(int argc, char* argv[]) {
    const char* csvFile = (argc > 1) ? argv[1] : "dataset3-cityC.csv";

    cout << "\n";
    hRule(70);
    cout << "  City C - Carbon Emission Analysis\n";
    cout << "  Data Structure: Linked List\n";
    hRule(70);

    // Load data into array first, then build list
    ResidentArray masterArr;
    cout << "\nLoading CSV: " << csvFile << " ...\n";
    int loaded = masterArr.loadCSV(csvFile);
    if (loaded < 0) {
        cerr << "ERROR: Cannot open '" << csvFile << "'.\n";
        cerr << "       Place the CSV file in the same directory as the executable.\n";
        return 1;
    }
    cout << "  Loaded " << loaded << " records successfully.\n\n";

    ResidentList masterList;
    masterList.buildFromArray(masterArr);

    // Step 1: Show initial analysis
    cout << "\n";
    hRule(70);
    cout << "  Initial Analysis\n";
    hRule(70);
    cout << "\n";

    boxTitle("EMISSION ANALYSIS – AGE GROUP");
    masterList.emissionByAgeGroup();

    boxTitle("EMISSION ANALYSIS – TRANSPORT MODE");
    masterList.emissionByMode();

    boxTitle("EMISSION ANALYSIS – CROSS ANALYSIS");
    masterList.emissionCrossAnalysis();

    // Step 2: Interactive menu for sorting and searching
    bool continueProgram = true;
    while (continueProgram) {
        cout << "\n";
        hRule(70);
        cout << "  Sorting Experiments\n";
        hRule(70);
        cout << "\nWould you like to sort the dataset?" << endl;
        cout << "  [1] Yes" << endl;
        cout << "  [2] No, skip to searching" << endl;
        cout << "\nYour choice: ";
        
        int sortChoice;
        cin >> sortChoice;
        
        if (cin.fail()) {
            clearInput();
            cout << "\nInvalid input. Please enter 1 or 2." << endl;
            continue;
        }
        clearInput();
        
        if (sortChoice == 1) {
            cout << "\nSelect sorting field:" << endl;
            cout << "  [1] Age" << endl;
            cout << "  [2] Daily Distance" << endl;
            cout << "  [3] Monthly Emission" << endl;
            cout << "\nYour choice: ";
            
            int fieldChoice;
            cin >> fieldChoice;
            
            if (cin.fail() || fieldChoice < 1 || fieldChoice > 3) {
                clearInput();
                cout << "\nInvalid choice. Please select 1, 2, or 3." << endl;
                continue;
            }
            clearInput();
            
            string fieldNames[3] = {"Age", "Distance", "Emission"};
            cout << "\n[Merge Sort by " << fieldNames[fieldChoice - 1] << "]\n";
            
            ResidentList sortList;
            sortList.buildFromArray(masterArr);
            TP start = now();
            sortList.mergeSort(fieldChoice - 1);
            double ms = elapsedMs(start);
            cout << "  Time: " << ms << " ms | Memory: " << sortList.memUsage() << " bytes\n";
            sortList.display();
        }
        
        // Searching menu
        cout << "\n";
        hRule(70);
        cout << "  Searching Experiments\n";
        hRule(70);
        cout << "\nWould you like to search the dataset?" << endl;
        cout << "  [1] Yes" << endl;
        cout << "  [2] No, continue to full analysis" << endl;
        cout << "\nYour choice: ";
        
        int searchChoice;
        cin >> searchChoice;
        
        if (cin.fail()) {
            clearInput();
            cout << "\nInvalid input. Please enter 1 or 2." << endl;
            continue;
        }
        clearInput();
        
        if (searchChoice == 1) {
            cout << "\nSelect search type:" << endl;
            cout << "  [1] Linear Search - Age Group" << endl;
            cout << "  [2] Linear Search - Transport Mode" << endl;
            cout << "  [3] Linear Search - Distance Threshold" << endl;
            cout << "  [4] Jump Search - Age Range (sorted)" << endl;
            cout << "\nYour choice: ";
            
            int searchType;
            cin >> searchType;
            clearInput();
            
            Resident results[200];
            
            if (searchType == 1) {
                cout << "\nSelect age group:" << endl;
                cout << "  [1] Children & Teenagers (6-17)" << endl;
                cout << "  [2] Young Adults (18-25)" << endl;
                cout << "  [3] Working Adults Early (26-45)" << endl;
                cout << "  [4] Working Adults Late (46-60)" << endl;
                cout << "  [5] Senior Citizens (61-100)" << endl;
                cout << "\nYour choice: ";
                
                int groupChoice;
                cin >> groupChoice;
                clearInput();
                
                string groups[5] = {"Children & Teenagers", "Young Adults", 
                                   "Working Adults Early", "Working Adults Late", 
                                   "Senior Citizens"};
                
                if (groupChoice >= 1 && groupChoice <= 5) {
                    cout << "\n[Linear Search - Age Group '" << groups[groupChoice - 1] << "']\n";
                    TP start = now();
                    int count = masterList.linSearchAgeGroup(groups[groupChoice - 1], results, 200);
                    double ms = elapsedMs(start);
                    cout << "  Found: " << count << " residents | Time: " << ms << " ms\n";
                }
            }
            else if (searchType == 2) {
                cout << "\nSelect transport mode:" << endl;
                cout << "  [1] Car" << endl;
                cout << "  [2] Bus" << endl;
                cout << "  [3] Bicycle" << endl;
                cout << "  [4] Walking" << endl;
                cout << "\nYour choice: ";
                
                int modeChoice;
                cin >> modeChoice;
                clearInput();
                
                string modes[4] = {"Car", "Bus", "Bicycle", "Walking"};
                
                if (modeChoice >= 1 && modeChoice <= 4) {
                    cout << "\n[Linear Search - Transport = " << modes[modeChoice - 1] << "]\n";
                    TP start = now();
                    int count = masterList.linSearchMode(modes[modeChoice - 1], results, 200);
                    double ms = elapsedMs(start);
                    cout << "  Found: " << count << " residents | Time: " << ms << " ms\n";
                }
            }
            else if (searchType == 3) {
                cout << "\nEnter minimum distance (km): ";
                double minDist;
                cin >> minDist;
                clearInput();
                
                cout << "\n[Linear Search - Distance > " << minDist << " km]\n";
                TP start = now();
                int count = masterList.linSearchDist(minDist, results, 200);
                double ms = elapsedMs(start);
                cout << "  Found: " << count << " residents | Time: " << ms << " ms\n";
            }
            else if (searchType == 4) {
                cout << "\nEnter age range:" << endl;
                cout << "  Minimum age: ";
                int minAge;
                cin >> minAge;
                cout << "  Maximum age: ";
                int maxAge;
                cin >> maxAge;
                clearInput();
                
                // Sort first
                ResidentList sortedList;
                sortedList.buildFromArray(masterArr);
                sortedList.mergeSort(0);
                
                cout << "\n[Jump Search - Age " << minAge << "-" << maxAge << " (sorted)]\n";
                TP start = now();
                int count = sortedList.jumpSearchAge(minAge, maxAge, results, 200);
                double ms = elapsedMs(start);
                cout << "  Found: " << count << " residents | Time: " << ms << " ms\n";
            }
        }
        
        // Ask if user wants to continue
        cout << "\n";
        hRule(70);
        cout << "\nWould you like to:" << endl;
        cout << "  [1] Perform another sort/search" << endl;
        cout << "  [2] Continue to full analysis" << endl;
        cout << "\nYour choice: ";
        
        int continueChoice;
        cin >> continueChoice;
        clearInput();
        
        if (continueChoice == 2) {
            continueProgram = false;
        }
    }

    // Step 3: Run full analysis
    cout << "\n";
    hRule(70);
    cout << "  Running Full Analysis\n";
    hRule(70);
    cout << "\n";

    boxTitle("RAW DATA – LINKED LIST");
    masterList.display();

    boxTitle("SORTING EXPERIMENTS – LINKED LIST");
    
    cout << "\n[1] Merge Sort by Age\n";
    ResidentList sortAge;
    sortAge.buildFromArray(masterArr);
    TP start = now();
    sortAge.mergeSort(0);
    double ms = elapsedMs(start);
    cout << "  Time: " << ms << " ms | Memory: " << sortAge.memUsage() << " bytes\n";
    sortAge.display();

    cout << "\n[2] Merge Sort by Distance\n";
    ResidentList sortDist;
    sortDist.buildFromArray(masterArr);
    start = now();
    sortDist.mergeSort(1);
    ms = elapsedMs(start);
    cout << "  Time: " << ms << " ms | Memory: " << sortDist.memUsage() << " bytes\n";

    cout << "\n[3] Merge Sort by Emission\n";
    ResidentList sortEmit;
    sortEmit.buildFromArray(masterArr);
    start = now();
    sortEmit.mergeSort(2);
    ms = elapsedMs(start);
    cout << "  Time: " << ms << " ms | Memory: " << sortEmit.memUsage() << " bytes\n";

    boxTitle("SEARCHING EXPERIMENTS – LINKED LIST");
    
    Resident results[200];
    
    cout << "\n[1] Linear Search - Age Group 'Working Adults Early'\n";
    start = now();
    int count1 = masterList.linSearchAgeGroup("Working Adults Early", results, 200);
    ms = elapsedMs(start);
    cout << "  Found: " << count1 << " residents | Time: " << ms << " ms\n";

    cout << "\n[2] Linear Search - Distance > 15 km\n";
    start = now();
    int count2 = masterList.linSearchDist(15.0, results, 200);
    ms = elapsedMs(start);
    cout << "  Found: " << count2 << " residents | Time: " << ms << " ms\n";

    cout << "\n[3] Linear Search - Transport = Car\n";
    start = now();
    int count3 = masterList.linSearchMode("Car", results, 200);
    ms = elapsedMs(start);
    cout << "  Found: " << count3 << " residents | Time: " << ms << " ms\n";

    cout << "\n";
    hRule(70);
    cout << "  Analysis Complete - Linked List\n";
    hRule(70);
    cout << "\n";

    return 0;
}
