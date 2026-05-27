#ifndef RESIDENT_HPP
#define RESIDENT_HPP

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <string>
#include <chrono>

using namespace std;

// Resident struct - holds all data for one resident
struct Resident {
    string residentID;
    int    age;
    string modeOfTransport;
    double dailyDistance;
    double carbonFactor;
    int    avgDaysPerMonth;
    double monthlyEmission;
    string ageGroup;
};

// Node struct for linked list
struct Node {
    Resident data;
    Node*    next;
};

// ResidentArray class - manages dynamic array of residents
class ResidentArray {
private:
    Resident* data;
    int       size;
    int       capacity;

public:
    // Constructor and destructor
    ResidentArray();
    ~ResidentArray();
    
    // Core array operations
    void append(const Resident& r);
    void deepCopy(const ResidentArray& src);
    size_t memUsage() const;
    int loadCSV(const char* fname);
    void display() const;
    
    // Sorting methods
    void mergeSort(int key);
    void insertionSort(int key);
    
    // Search methods
    int linSearchAgeGroup(const string& grp, Resident* res, int maxRes) const;
    int linSearchMode(const string& mode, Resident* res, int maxRes) const;
    int linSearchDist(double threshold, Resident* res, int maxRes) const;
    int jumpSearchAge(int minAge, int maxAge, Resident* res, int maxRes) const;
    int jumpSearchDist(double threshold, Resident* res, int maxRes) const;
    
    // Analysis methods
    void emissionByAgeGroup() const;
    void emissionByMode() const;
    void emissionCrossAnalysis() const;
    
    // Getters for accessing private members
    int getSize() const { return size; }
    Resident* getData() const { return data; }
    
    // Friend function for list conversion
    friend class ResidentList;
};

// ResidentList class - manages singly linked list of residents
class ResidentList {
private:
    Node* head;
    int   size;

public:
    // Constructor and destructor
    ResidentList();
    ~ResidentList();
    
    // Core list operations
    void append(const Resident& r);
    void buildFromArray(const ResidentArray& arr);
    int loadCSV(const char* fname);
    size_t memUsage() const;
    void display() const;
    
    // Sorting methods
    void mergeSort(int key);
    void insertionSort(int key);
    
    // Search methods
    int linSearchAgeGroup(const string& grp, Resident* res, int maxRes) const;
    int linSearchMode(const string& mode, Resident* res, int maxRes) const;
    int linSearchDist(double threshold, Resident* res, int maxRes) const;
    int jumpSearchAge(int minAge, int maxAge, Resident* res, int maxRes) const;
    int jumpSearchDist(double threshold, Resident* res, int maxRes) const;
    
    // Analysis methods
    void emissionByAgeGroup() const;
    void emissionByMode() const;
    void emissionCrossAnalysis() const;
    
    // Getters
    int getSize() const { return size; }
};

// Timing struct for performance comparison
struct TimingRow {
    string label;
    double arrayMs;
    double listMs;
    size_t arrayMem;
    size_t listMem;
};

// Utility functions (inline)
inline double computeEmission(double dist, double factor, int days) {
    return dist * factor * static_cast<double>(days);
}

inline string assignAgeGroup(int age) {
    if      (age >=  6 && age <= 17) return "Children & Teenagers";
    else if (age >= 18 && age <= 25) return "Young Adults";
    else if (age >= 26 && age <= 45) return "Working Adults (Early)";
    else if (age >= 46 && age <= 60) return "Working Adults (Late)";
    else if (age >= 61 && age <=100) return "Senior Citizens";
    else                              return "Unknown";
}

inline void hRule(int w = 100) {
    for (int i = 0; i < w; i++) cout << '-';
    cout << '\n';
}

inline void boxTitle(const char* msg) {
    cout << "\n";
    hRule(70);
    cout << "  " << msg << "\n";
    hRule(70);
    cout << "\n";
}

inline void printHeader() {
    cout << left
         << setw(8)  << "ID"
         << setw(5)  << "Age"
         << setw(24) << "Age Group"
         << setw(13) << "Transport"
         << setw(10) << "Dist(km)"
         << setw(9)  << "CO2/km"
         << setw(7)  << "Days"
         << setw(16) << "Monthly CO2(kg)"
         << "\n";
    hRule(92);
}

inline void printRow(const Resident& r) {
    cout << left
         << setw(8)  << r.residentID
         << setw(5)  << r.age
         << setw(24) << r.ageGroup
         << setw(13) << r.modeOfTransport
         << fixed << setprecision(2)
         << setw(10) << r.dailyDistance
         << setw(9)  << r.carbonFactor
         << setw(7)  << r.avgDaysPerMonth
         << setw(16) << r.monthlyEmission
         << "\n";
}

inline int iMin(int a, int b) { return (a < b) ? a : b; }

inline int cmpKey(const Resident& a, const Resident& b, int key) {
    if (key == 0) return a.age - b.age;
    if (key == 1) {
        if (a.dailyDistance < b.dailyDistance) return -1;
        if (a.dailyDistance > b.dailyDistance) return  1;
        return 0;
    }
    if (a.monthlyEmission < b.monthlyEmission) return -1;
    if (a.monthlyEmission > b.monthlyEmission) return  1;
    return 0;
}

// Display and timing functions
void showSearchResults(const string& title, Resident* results, int count,
                       const string& structure, double milliseconds, int maxShow = 8);
void printTimingTable(TimingRow* rows, int numRows);
void displayInsightsAndRecommendations();
void displayComplexityAnalysis();
void runSortingExperiments(const ResidentArray& masterArr, TimingRow* rows, int& rowIndex);
void runSearchingExperiments(const ResidentArray& masterArr, const ResidentList& masterList, 
                             TimingRow* rows, int& rowIndex);

#endif
