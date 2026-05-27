#include "resident.hpp"
#include <chrono>

using namespace std;
using namespace chrono;

typedef high_resolution_clock HRC;
typedef HRC::time_point TP;

inline TP now() { return HRC::now(); }
inline double elapsedMs(TP start) {
    return duration<double, milli>(HRC::now() - start).count();
}

int main(int argc, char* argv[]) {
    const char* csvFile = (argc > 1) ? argv[1] : "dataset3-cityC.csv";

    cout << "\n";
    hRule(70);
    cout << "  City C - Carbon Emission Analysis\n";
    cout << "  Data Structures: Array + Linked List\n";
    hRule(70);

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

    boxTitle("RAW DATA – ARRAY");
    masterArr.display();

    boxTitle("RAW DATA – LINKED LIST");
    masterList.display();

    const int MAX_TIMING = 30;
    TimingRow rows[MAX_TIMING];
    int rowIndex = 0;

    // Measure data loading performance
    {
        ResidentArray testArr;
        TP start = now();
        testArr.loadCSV(csvFile);
        double ms = elapsedMs(start);
        rows[rowIndex].label = "Load CSV Data";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = testArr.memUsage();
    }
    {
        ResidentList testList;
        TP start = now();
        testList.loadCSV(csvFile);
        double ms = elapsedMs(start);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = testList.memUsage();
        rowIndex++;
    }

    // Measure buildFromArray performance
    {
        rows[rowIndex].label = "Build List from Array";
        rows[rowIndex].arrayMs = 0.0;  // Not applicable for array
        rows[rowIndex].arrayMem = masterArr.memUsage();
        
        ResidentList testList;
        TP start = now();
        testList.buildFromArray(masterArr);
        double ms = elapsedMs(start);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = testList.memUsage();
        rowIndex++;
    }

    // Measure emission analysis performance
    {
        TP start = now();
        masterArr.emissionByAgeGroup();
        double ms = elapsedMs(start);
        rows[rowIndex].label = "Emission Analysis – Age Group";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();
        
        start = now();
        masterList.emissionByAgeGroup();
        ms = elapsedMs(start);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    {
        TP start = now();
        masterArr.emissionByMode();
        double ms = elapsedMs(start);
        rows[rowIndex].label = "Emission Analysis – Transport Mode";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();
        
        start = now();
        masterList.emissionByMode();
        ms = elapsedMs(start);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    {
        TP start = now();
        masterArr.emissionCrossAnalysis();
        double ms = elapsedMs(start);
        rows[rowIndex].label = "Emission Analysis – Cross Analysis";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();
        
        start = now();
        masterList.emissionCrossAnalysis();
        ms = elapsedMs(start);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    runSortingExperiments(masterArr, rows, rowIndex);
    runSearchingExperiments(masterArr, masterList, rows, rowIndex);

    printTimingTable(rows, rowIndex);
    displayComplexityAnalysis();
    displayInsightsAndRecommendations();

    cout << "\n";
    hRule(70);
    cout << "  Analysis complete.\n";
    hRule(70);
    cout << "\n";

    return 0;
}
