#ifndef RESIDENT_H
#define RESIDENT_H

#include <string>
#include <cstring>

using namespace std;

// ----------------------------------------------------
//  LINKED LIST SIDE (ResidentData)
// ----------------------------------------------------

// Step 1: define the data that each resident holds from csv
struct ResidentData
{
	string ResidentID;
	int Age;
	string TransportMode;
	double DailyDistance;
	double EmissionFactor;
	int DaysPerMonth;
	double TotalEmission;
};

// Step 1.1: calculate monthly carbon emission for one resident
inline double calculateEmission(double distance, double factor, int days)
{
	return distance * factor * days;
}

// Step 1.2: return the age group name based on age
inline string getAgeGroupName(int age)
{
	if (age >= 6  && age <= 17)  return "Children & Teenagers (6-17)";
	if (age >= 18 && age <= 25)  return "University Students (18-25)";
	if (age >= 26 && age <= 45)  return "Working Adults Early (26-45)";
	if (age >= 46 && age <= 60)  return "Working Adults Late (46-60)";
	if (age >= 61 && age <= 100) return "Senior Citizens (61-100)";
	return "Unknown";
}

// Step 1.3: return age group as a number so we can use it as array index
inline int getAgeGroupIndex(int age)
{
	if (age >= 6  && age <= 17)  return 0;
	if (age >= 18 && age <= 25)  return 1;
	if (age >= 26 && age <= 45)  return 2;
	if (age >= 46 && age <= 60)  return 3;
	if (age >= 61 && age <= 100) return 4;
	return -1;
}

// ============================================================
//  ARRAY SIDE — constants, structs, classes
// ============================================================

// Limits
const int MAX_STR  = 20;   // Max string len
const int BUF_SIZE = 256;  // Read buffer

// Age groups
const int NUM_GROUPS = 5;
const int GROUP_MIN[] = {6, 18, 26, 46, 61};
const int GROUP_MAX[] = {17, 25, 45, 60, 100};
const char GROUP_NAMES[][35] = {
    "Children & Teenagers (6-17)", "University Students (18-25)",
    "Working Adults Early (26-45)", "Working Adults Late (46-60)",
    "Senior Citizens (61-100)"};

// Transport modes
const int NUM_MODES = 4;
const char MODE_NAMES[][MAX_STR] = {"Car", "Bus", "Bicycle", "Walking"};

// Sort keys
const int BY_AGE      = 1;
const int BY_DIST     = 2;
const int BY_EMISSION = 3;

// Data struct (array side)
struct Resident {
  char   id[MAX_STR];
  int    age;
  char   transport[MAX_STR];
  double distance;
  double factor;
  int    days;
  double emission; // dist * factor * days
};

// Main ADT
class ResidentArray {
private:
  Resident *data;   // Heap array
  int       size;   // Loaded count
  int       capacity; // Allocated slots

  void resize(int newCapacity);

  // Sorting helpers
  double sortVal(int index, int key) const;
  int    partition(int lo, int hi, int key);
  void   qsortRec(int lo, int hi, int key);

  // Static helpers
  static int nextField(const char *line, int pos, char *out, int limit);

public:
  // Constructor / Destructor
  ResidentArray();
  ~ResidentArray();

  // Getters
  int getSize()     const { return this->size; }
  int getCapacity() const { return this->capacity; }
  int getMemoryUsed() const { return this->capacity * sizeof(Resident); }
  const Resident &get(int index) const { return this->data[index]; }

  // Operations
  void push(const Resident &r);
  bool loadCSV(const char *file);
  void copyTo(ResidentArray &to) const;
  void showRecords(int n) const;

  // Sorting
  void sortBy(int key);

  // Searching
  void bSearchRange(int key, double lo, double hi, ResidentArray &out) const;
  void searchAgeGroup(int g) const;
  void searchTransport(int modeIdx) const;
  void searchDistance(double minDist) const;

  // Analysis
  void ageGroupReport() const;
  void emissionReport() const;
  void insights() const;

  // Hypothesis Testing
  void runAllTests(const char *csvFile);

  // Static Utility
  static const char *keyName(int key);
  static inline int whichGroup(int age) {
    for (int i = 0; i < NUM_GROUPS; i++)
      if (age >= GROUP_MIN[i] && age <= GROUP_MAX[i])
        return i;
    return -1;
  }
  static inline int whichMode(const char *t) {
    for (int i = 0; i < NUM_MODES; i++)
      if (strcmp(t, MODE_NAMES[i]) == 0)
        return i;
    return -1;
  }
};

// Performance log entry
struct LogEntry {
  char   type[50];
  char   detail[50];
  double timeUs;
  int    memoryBytes;
};

class PerfLogger {
private:
  static const int MAX_LOGS = 100;
  static LogEntry  perfLogs[100];
  static int       numLogs;

public:
  static bool silent;
  static void showPerf(const char *type, const char *detail, int records,
                       double ms, int bytes);
  static void printSummary();
};

#endif
