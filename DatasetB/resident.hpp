
#ifndef RESIDENT_HPP
#define RESIDENT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <cstdlib>
using namespace std;


// ============================================================
// SECTION 1: HELPER FUNCTION PROTOTYPES
// Defined in residentList.cpp.
// Declared here so main.cpp and displayList.cpp can both use them.
// ============================================================

string padStr(string text, int width);
string intStr(int number);
string dblStr(double number);
void   printLine(int length);
string ageGroupLabel(int age);
string toLower(string text);
bool   strContains(string text, string query);


// ============================================================
// SECTION 2: RESIDENT STRUCT (THE NODE)
// One Resident = one row from the CSV file.
// nextAddress is the chain link that connects nodes together.
// Without it we would have isolated variables, not a linked list.
// ============================================================

struct Resident
{
    string residentID;
    int    age;
    string modeOfTransport;
    double dailyDistance;
    double carbonEmissionFactor;
    int    avgDaysPerMonth;
    double monthlyEmission;
    Resident* nextAddress;
};


// ============================================================
// SECTION 3: RESIDENTLIST CLASS DECLARATION
// ============================================================

class ResidentList
{
private:
    Resident* head;
    Resident* tail;
    int       size;

    void splitList(Resident* source, Resident*& frontHalf, Resident*& backHalf);
    Resident* mergeTwoLists(Resident* a, Resident* b, string sortField, int& compCount);
    void mergeSortHelper(Resident*& listHead, string sortField, int& compCount);

    int    savedCount[5];
    double savedTotal[5];
    double savedAvg[5];
    string savedTopMode[5];
    int    savedTopCount[5];
    double savedModeTotal[5][4];
    int    savedModeCounts[5][4];
    double datasetTotalEmit;
    bool   sectionFourRan;

public:
    ResidentList();
    ~ResidentList();
    Resident* getHead();
    int getSize();

    Resident* createNewResident(string id, int age, string transport,
                                double distance, double carbonFactor, int avgDays);
    void insertToEnd(string id, int age, string transport,
                     double distance, double carbonFactor, int avgDays);
    void loadFromCSV(string filename);
    void displayList(int limit);

    void bubbleSort(string sortField, int& compCount);
    void mergeSort(string sortField, int& compCount);

    void linearSearch(string searchField, string query,
                      double rangeMin, double rangeMax, int& compCount);
    void sentinelSearch(string sortField, double rangeMin,
                        double rangeMax, int& compCount);

    void sectionFourAgeGroups();
    void sectionFiveEmissions();
    void sectionNineInsights();
};


// ============================================================
// SECTION 4: RESIDENTARRAY STRUCT (ARRAY ELEMENT)
// ============================================================

struct ResidentArray {
    string id;
    int age;
    string transport;
    float distance;
    float carbonFactor;
    int days;
};

const int MAX = 1000;


// ============================================================
// SECTION 5: RESIDENTARRAYMANAGER CLASS DECLARATION
// ============================================================

class ResidentArrayManager {

private:
    ResidentArray arr[MAX];
    int arrCount;
    long long bubbleComparisons;
    long long quickComparisons;

public:
    ResidentArrayManager();

    void loadCSV(string filename);
    void copyArray(ResidentArray dest[], ResidentArray src[], int n);
    void shuffleArray(ResidentArray a[], int n);

    void bubbleSort(ResidentArray a[], int n);
    int partition(ResidentArray a[], int low, int high);
    void quickSort(ResidentArray a[], int low, int high);

    void linearSearchTransport(string target);
    void binarySearchAge(int target);
    void jumpSearchEmission(float target);

    void analyzeAgeGroups();
    void emissionAnalysis();
    void sortingExperiment();
    void searchingExperiment();

    void performanceTest();
    void testDataOrderEffect();
    void hypothesis3_test();
    void hypothesis4_test();

    void insights();
    void performanceAnalysis();
};

// Display function
void runDisplay(ResidentArrayManager &m);

#endif
