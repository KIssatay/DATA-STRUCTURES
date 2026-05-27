#include "Resident.hpp"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// Perf logs
const int PerfLogger::MAX_LOGS;
LogEntry PerfLogger::perfLogs[100];
int PerfLogger::numLogs = 0;
bool PerfLogger::silent = false;

// Read CSV
int ResidentArray::nextField(const char *line, int pos, char *out, int limit) {
  int i = 0;
  while (true) {
    if (line[pos] == ',' || line[pos] == '\0' || line[pos] == '\n' ||
        line[pos] == '\r' || i >= limit - 1)
      break;
    out[i] = line[pos];
    i++;
    pos++;
  }
  out[i] = '\0';
  if (line[pos] == ',')
    pos++;
  return pos;
}

// ADT Impl
ResidentArray::ResidentArray() {
  this->capacity = 10;
  this->size = 0;
  this->data = new Resident[this->capacity];
}
ResidentArray::~ResidentArray() { delete[] this->data; }

void ResidentArray::resize(int newCapacity) {
  Resident *newData = new Resident[newCapacity];
  for (int i = 0; i < this->size; i++)
    newData[i] = this->data[i];
  delete[] this->data;
  this->data = newData;
  this->capacity = newCapacity;
}

bool ResidentArray::loadCSV(const char *file) {
  ifstream fin(file);
  if (!fin.is_open()) {
    cout << "Can't open " << file << endl;
    return false;
  }
  this->size = 0;
  char line[BUF_SIZE];
  fin.getline(line, BUF_SIZE); // Skip header
  char tmp[BUF_SIZE];
  while (fin.getline(line, BUF_SIZE)) {
    if (line[0] == '\0' || line[0] == '\r')
      continue;
    Resident r;
    int p = 0;
    p = nextField(line, p, r.id, MAX_STR);
    p = nextField(line, p, tmp, BUF_SIZE);
    r.age = atoi(tmp);
    p = nextField(line, p, r.transport, MAX_STR);
    p = nextField(line, p, tmp, BUF_SIZE);
    r.distance = atof(tmp);
    p = nextField(line, p, tmp, BUF_SIZE);
    r.factor = atof(tmp);
    p = nextField(line, p, tmp, BUF_SIZE);
    r.days = atoi(tmp);
    r.emission = r.distance * r.factor * r.days;

    if (this->size == this->capacity)
      this->resize(this->capacity * 2);
    this->data[this->size] = r;
    this->size++;
  }
  fin.close();
  return true;
}

void ResidentArray::copyTo(ResidentArray &to) const {
  if (to.capacity < this->capacity)
    to.resize(this->capacity);
  to.size = this->size;
  for (int i = 0; i < this->size; i++)
    to.data[i] = this->data[i];
}

void ResidentArray::showRecords(int n) const {
  if (n > this->size)
    n = this->size;
  cout << left << setw(8) << "ID " << setw(6) << "Age " << setw(10) << "Mode "
       << right << setw(10) << "Dist(km) " << setw(10) << "Factor " << setw(8)
       << "Days " << setw(14) << "Emission " << endl;
  cout << "------------------------------------------------------------------"
       << endl;
  for (int i = 0; i < n; i++) {
    const Resident &r = this->data[i];
    cout << left << setw(8) << r.id << setw(6) << r.age << setw(10)
         << r.transport << right << setw(8) << fixed << setprecision(1)
         << r.distance << setw(10) << setprecision(2) << r.factor << setw(8)
         << r.days << setw(14) << setprecision(2) << r.emission << endl;
  }
  if (n < this->size)
    cout << "( " << n << " of " << this->size << " shown) " << endl;
}

// Quick Sort
double ResidentArray::sortVal(int index, int key) const {
  const Resident &r = this->data[index];
  if (key == BY_AGE)
    return r.age;
  if (key == BY_DIST)
    return r.distance;
  if (key == BY_EMISSION)
    return r.emission;
  return 0.0;
}

const char *ResidentArray::keyName(int key) {
  if (key == BY_AGE)
    return "Age";
  if (key == BY_DIST)
    return "Distance";
  if (key == BY_EMISSION)
    return "Emission";
  return "Unknown";
}

int ResidentArray::partition(int lo, int hi, int key) {
  double pivot = this->sortVal(hi, key);
  int i = lo - 1;
  for (int j = lo; j < hi; j++) {
    if (this->sortVal(j, key) <= pivot) {
      i++;
      Resident tmp = this->data[i];
      this->data[i] = this->data[j];
      this->data[j] = tmp;
    }
  }
  Resident tmp = this->data[i + 1];
  this->data[i + 1] = this->data[hi];
  this->data[hi] = tmp;
  return i + 1;
}

void ResidentArray::qsortRec(int lo, int hi, int key) {
  if (lo < hi) {
    int p = this->partition(lo, hi, key);
    this->qsortRec(lo, p - 1, key);
    this->qsortRec(p + 1, hi, key);
  }
}

void ResidentArray::sortBy(int key) {
  auto t1 = chrono::high_resolution_clock::now();
  this->qsortRec(0, this->size - 1, key);
  auto t2 = chrono::high_resolution_clock::now();
  double ms = chrono::duration<double, milli>(t2 - t1).count();
  PerfLogger::showPerf("Quick Sort", ResidentArray::keyName(key), this->size,
                       ms, this->getMemoryUsed());
}

// Binary Search
void ResidentArray::bSearchRange(int key, double lo, double hi,
                                 ResidentArray &out) const {
  out.size = 0;
  int left = 0, right = this->size - 1, start = this->size;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (this->sortVal(mid, key) >= lo) {
      start = mid;
      right = mid - 1;
    } else
      left = mid + 1;
  }
  for (int i = start; i < this->size; i++) {
    if (this->sortVal(i, key) <= hi) {
      if (out.size == out.capacity)
        out.resize(out.capacity * 2);
      out.data[out.size++] = this->data[i];
    } else
      break;
  }
}

void ResidentArray::searchAgeGroup(int g) const {
  if (g < 0 || g >= NUM_GROUPS)
    return;
  cout << "\n>> Binary Search: " << GROUP_NAMES[g] << endl;
  ResidentArray sorted;
  this->copyTo(sorted);
  sorted.sortBy(BY_AGE);
  auto t1 = chrono::high_resolution_clock::now();
  ResidentArray results;
  sorted.bSearchRange(BY_AGE, GROUP_MIN[g], GROUP_MAX[g], results);
  auto t2 = chrono::high_resolution_clock::now();
  double ms = chrono::duration<double, milli>(t2 - t1).count();
  cout << "Found " << results.size << " residents" << endl;
  results.showRecords(results.size);
  PerfLogger::showPerf("Binary Search", GROUP_NAMES[g], this->size, ms,
                       sorted.getMemoryUsed());
}

void ResidentArray::searchTransport(int modeIdx) const {
  if (modeIdx < 0 || modeIdx >= NUM_MODES)
    return;
  cout << "\n>> Search: " << MODE_NAMES[modeIdx] << " users" << endl;
  auto t1 = chrono::high_resolution_clock::now();
  ResidentArray results;
  results.size = 0;
  for (int i = 0; i < this->size; i++) {
    if (strcmp(this->data[i].transport, MODE_NAMES[modeIdx]) == 0) {
      if (results.size == results.capacity)
        results.resize(results.capacity * 2);
      results.data[results.size++] = this->data[i];
    }
  }
  auto t2 = chrono::high_resolution_clock::now();
  double ms = chrono::duration<double, milli>(t2 - t1).count();
  cout << "Found " << results.size << " residents" << endl;
  results.showRecords(results.size);
  PerfLogger::showPerf("Linear Search", MODE_NAMES[modeIdx], this->size, ms,
                       this->getMemoryUsed());
}

void ResidentArray::searchDistance(double minDist) const {
  cout << "\n>> Binary Search: distance >= " << fixed << setprecision(0)
       << minDist << " km " << endl;
  ResidentArray sorted;
  this->copyTo(sorted);
  sorted.sortBy(BY_DIST);
  double maxDist = sorted.data[sorted.size - 1].distance;
  auto t1 = chrono::high_resolution_clock::now();
  ResidentArray results;
  sorted.bSearchRange(BY_DIST, minDist, maxDist, results);
  auto t2 = chrono::high_resolution_clock::now();
  double ms = chrono::duration<double, milli>(t2 - t1).count();
  cout << "Found " << results.size << " residents" << endl;
  results.showRecords(results.size);
  char detailBuf[30];
  strcpy(detailBuf, "dist >= ");
  int distInt = (int)minDist, d_pos = 8;
  if (distInt >= 10) {
    detailBuf[d_pos++] = '0' + (distInt / 10);
    distInt %= 10;
  }
  detailBuf[d_pos++] = '0' + distInt;
  detailBuf[d_pos++] = ' ';
  detailBuf[d_pos++] = 'k';
  detailBuf[d_pos++] = 'm';
  detailBuf[d_pos] = '\0';
  PerfLogger::showPerf("Binary Search", detailBuf, this->size, ms,
                       sorted.getMemoryUsed());
}

// Analysis
void ResidentArray::ageGroupReport() const {
  int count[NUM_GROUPS] = {0};
  double totalEm[NUM_GROUPS] = {0};
  int modePref[NUM_GROUPS][NUM_MODES] = {{0}};
  for (int i = 0; i < this->size; i++) {
    int g = whichGroup(this->data[i].age);
    int m = whichMode(this->data[i].transport);
    if (g < 0)
      continue;
    count[g]++;
    totalEm[g] += this->data[i].emission;
    if (m >= 0)
      modePref[g][m]++;
  }
  cout << left << setw(35) << "Age Group " << setw(8) << "Count " << setw(16)
       << "Preferred Mode " << right << setw(16) << "Total Emission "
       << setw(14) << "Avg Emission " << endl;
  cout << "--------------------------------------------------------------------"
       << endl;
  for (int g = 0; g < NUM_GROUPS; g++) {
    int best = 0;
    for (int m = 1; m < NUM_MODES; m++)
      if (modePref[g][m] > modePref[g][best])
        best = m;
    double avg = count[g] > 0 ? totalEm[g] / count[g] : 0.0;
    cout << left << setw(35) << GROUP_NAMES[g] << setw(8) << count[g]
         << setw(16) << (count[g] > 0 ? MODE_NAMES[best] : "N/A ") << right
         << setw(14) << fixed << setprecision(2) << totalEm[g] << setw(14)
         << avg << endl;
  }
}

void ResidentArray::emissionReport() const {
  double total = 0;
  for (int i = 0; i < this->size; i++)
    total += this->data[i].emission;
  cout << "Total emission: " << fixed << setprecision(2) << total << " kg CO2 "
       << endl;
  cout << "Residents: " << this->size << endl;
  cout << "Average: " << total / this->size << " kg CO2 per person " << endl;
  cout << "\nBy transport mode:" << endl;
  double modeEm[NUM_MODES] = {0};
  int modeCnt[NUM_MODES] = {0};
  for (int i = 0; i < this->size; i++) {
    int m = whichMode(this->data[i].transport);
    if (m >= 0) {
      modeEm[m] += this->data[i].emission;
      modeCnt[m]++;
    }
  }
  cout << left << setw(12) << "Mode " << setw(8) << "Count " << right
       << setw(16) << "Total " << setw(14) << "Average " << endl;
  cout << "--------------------------------------------------" << endl;
  for (int m = 0; m < NUM_MODES; m++) {
    if (modeCnt[m] == 0)
      continue;
    cout << left << setw(12) << MODE_NAMES[m] << setw(8) << modeCnt[m] << right
         << setw(14) << fixed << setprecision(2) << modeEm[m] << setw(14)
         << modeEm[m] / modeCnt[m] << endl;
  }
  cout << "\nEmission by age group and transport: " << endl;
  double cross[NUM_GROUPS][NUM_MODES] = {{0}};
  for (int i = 0; i < this->size; i++) {
    int g = whichGroup(this->data[i].age);
    int m = whichMode(this->data[i].transport);
    if (g >= 0 && m >= 0)
      cross[g][m] += this->data[i].emission;
  }
  cout << left << setw(35) << "Age Group ";
  for (int m = 0; m < NUM_MODES; m++)
    cout << right << setw(12) << MODE_NAMES[m];
  cout << endl
       << "--------------------------------------------------------------------"
       << endl;
  for (int g = 0; g < NUM_GROUPS; g++) {
    bool any = false;
    for (int m = 0; m < NUM_MODES; m++)
      if (cross[g][m] > 0)
        any = true;
    if (!any)
      continue;
    cout << left << setw(35) << GROUP_NAMES[g];
    for (int m = 0; m < NUM_MODES; m++)
      cout << right << setw(12) << fixed << setprecision(2) << cross[g][m];
    cout << endl;
  }
}

void ResidentArray::insights() const {
  double groupEm[NUM_GROUPS] = {0};
  int groupCnt[NUM_GROUPS] = {0};
  int carUse[NUM_GROUPS] = {0}, bikeUse[NUM_GROUPS] = {0};
  int carIdx = whichMode("Car"), bikeIdx = whichMode("Bicycle");
  for (int i = 0; i < this->size; i++) {
    int g = whichGroup(this->data[i].age);
    if (g < 0)
      continue;
    groupEm[g] += this->data[i].emission;
    groupCnt[g]++;
    int m = whichMode(this->data[i].transport);
    if (m == carIdx)
      carUse[g]++;
    if (m == bikeIdx)
      bikeUse[g]++;
  }
  cout << "1) Executive Data Summary: " << endl;
  cout << left << setw(30) << "Age Group " << right << setw(15) << "Total CO2 "
       << setw(15) << "Avg CO2 " << setw(10) << "Cars " << setw(10) << "Bikes "
       << endl;
  cout << "--------------------------------------------------------------------"
       << endl;
  for (int g = 0; g < NUM_GROUPS; g++) {
    if (groupCnt[g] == 0)
      continue;
    cout << left << setw(30) << GROUP_NAMES[g] << right << setw(15) << fixed
         << setprecision(2) << groupEm[g] << setw(15)
         << groupEm[g] / groupCnt[g] << setw(10) << carUse[g] << setw(10)
         << bikeUse[g] << endl;
  }
  int worst = 0;
  for (int g = 1; g < NUM_GROUPS; g++)
    if (groupEm[g] > groupEm[worst])
      worst = g;
  cout << "\n2) Key Takeaway: " << endl;
  cout << "   The highest emitting group is " << GROUP_NAMES[worst] << " ("
       << fixed << setprecision(2) << groupEm[worst] << " kg CO2 ";
  if (groupCnt[worst] > 0)
    cout << ", avg " << groupEm[worst] / groupCnt[worst] << " per person ";
  cout << "). " << endl;
  cout << "\n3) Recommendations: " << endl;
  cout << "   - More cycling lanes for " << GROUP_NAMES[2]
       << " who mostly drive short-medium distances " << endl;
  cout << "   - Better bus coverage for " << GROUP_NAMES[3]
       << " to reduce car reliance " << endl;
  cout << "   - Rush-hour congestion pricing to shift commuters to public "
          "transport "
       << endl;
  cout << "   - E-bike subsidies for short-distance commuters to cut emissions "
       << endl;
}

// ========================================
// 🟢 FIX 1: showPerf (Prevent benchmark logs from filling buffer)
// ========================================
void PerfLogger::showPerf(const char *type, const char *detail, int records,
                          double ms, int bytes) {
  // 🟢 Move silent check to TOP to skip benchmark iterations entirely
  if (silent)
    return;

  if (numLogs < MAX_LOGS) {
    LogEntry &log = perfLogs[numLogs++];
    log.timeUs = ms * 1000.0;
    log.memoryBytes = bytes;
    strcpy(log.type, type);
    strcpy(log.detail, detail);
  }

  cout << "\n  [Performance] " << endl;
  cout << "  Operation :  " << type << "  " << detail << endl;
  cout << "  Structure : Array " << endl;
  cout << "  Records   :  " << records << endl;
  if (ms < 1.0)
    cout << "  Time      :  " << fixed << setprecision(3) << ms * 1000 << " us "
         << endl;
  else
    cout << "  Time      :  " << fixed << setprecision(3) << ms << " ms "
         << endl;
  cout << "  Memory    :  " << bytes << " bytes " << endl;
}

// ========================================
// 🟢 FIX 2: printSummary (Fix trailing space mismatch)
// ========================================
void PerfLogger::printSummary() {
  cout << "\n=================================================================="
          "=="
       << endl;
  cout << "  System Performance Logs  & Averages " << endl;
  cout << "===================================================================="
       << endl;
  cout << left << setw(20) << "Operation Type " << setw(35)
       << "Parameter/Detail " << right << setw(12) << "Time (us) " << setw(13)
       << "Memory (B) " << endl;
  cout << "--------------------------------------------------------------------"
       << endl;
  for (int i = 0; i < numLogs; i++) {
    cout << left << setw(20) << perfLogs[i].type << setw(35)
         << perfLogs[i].detail << right << setw(12) << fixed << setprecision(3)
         << perfLogs[i].timeUs << setw(13) << perfLogs[i].memoryBytes << endl;
  }

  cout << "\n------------------------------------------------------------------"
          "--"
       << endl;
  cout << "  Averages by Operation Type " << endl;
  cout << "--------------------------------------------------------------------"
       << endl;
  cout << left << setw(25) << "Operation Type " << right << setw(15) << "Runs "
       << setw(20) << "Avg Time (us) " << setw(20) << "Avg Memory (B) " << endl;
  cout << "--------------------------------------------------------------------"
       << endl;

  // 🟢 FIX: Removed trailing spaces so strcmp() matches exactly
  const char *types[] = {"Quick Sort", "Binary Search", "Linear Search"};

  for (int t = 0; t < 3; t++) {
    int count = 0;
    double totalTime = 0;
    long long totalMem = 0;
    for (int i = 0; i < numLogs; i++) {
      // Now strcmp will successfully match "Quick Sort" == "Quick Sort"
      if (strcmp(perfLogs[i].type, types[t]) == 0) {
        count++;
        totalTime += perfLogs[i].timeUs;
        totalMem += perfLogs[i].memoryBytes;
      }
    }
    if (count > 0) {
      cout << left << setw(25) << types[t] << right << setw(15) << count
           << setw(20) << fixed << setprecision(3) << (totalTime / count)
           << setw(20) << (totalMem / count) << endl;
    }
  }
  cout << "===================================================================="
          "\n"
       << endl;
}

void ResidentArray::push(const Resident &r) {
  if (this->size == this->capacity)
    this->resize(this->capacity * 2);
  this->data[this->size] = r;
  this->size++;
}

// ============================================================================
//  HYPOTHESIS TESTING FRAMEWORK
// ============================================================================
class HypothesisTester {
public:
  static int linearSearch(ResidentArray *arr, int key, double target) {
    for (int i = 0; i < arr->getSize(); i++) {
      if (arr->get(i).age == target || arr->get(i).distance == target ||
          arr->get(i).emission == target)
        return i;
    }
    return -1;
  }
  static int linearCount(ResidentArray *arr, int key, double lo, double hi) {
    int count = 0;
    for (int i = 0; i < arr->getSize(); i++) {
      double v = (key == BY_AGE)    ? arr->get(i).age
                 : (key == BY_DIST) ? arr->get(i).distance
                                    : arr->get(i).emission;
      if (v >= lo && v <= hi)
        count++;
    }
    return count;
  }
  static double avgSortTime(ResidentArray *src, int key, int runs) {
    double total = 0;
    for (int r = 0; r < runs; r++) {
      ResidentArray *tmp = new ResidentArray();
      src->copyTo(*tmp);
      auto t1 = chrono::high_resolution_clock::now();
      tmp->sortBy(key);
      auto t2 = chrono::high_resolution_clock::now();
      total += chrono::duration<double, micro>(t2 - t1).count();
      delete tmp;
    }
    return total / runs;
  }
  static int linearCountMode(ResidentArray *arr, const char *mode) {
    int count = 0;
    for (int i = 0; i < arr->getSize(); i++)
      if (strcmp(arr->get(i).transport, mode) == 0)
        count++;
    return count;
  }
  static double avgLinearSearchTime(ResidentArray *arr, const char *mode,
                                    int runs) {
    double total = 0;
    for (int r = 0; r < runs; r++) {
      auto t1 = chrono::high_resolution_clock::now();
      linearCountMode(arr, mode);
      auto t2 = chrono::high_resolution_clock::now();
      total += chrono::duration<double, micro>(t2 - t1).count();
    }
    return total / runs;
  }
  static double avgLoadTime(const char *file, int runs) {
    double total = 0;
    for (int r = 0; r < runs; r++) {
      ResidentArray *tmp = new ResidentArray();
      auto t1 = chrono::high_resolution_clock::now();
      tmp->loadCSV(file);
      auto t2 = chrono::high_resolution_clock::now();
      total += chrono::duration<double, micro>(t2 - t1).count();
      delete tmp;
    }
    return total / runs;
  }
  static void scaleData(ResidentArray *src, ResidentArray *dst, int times) {
    for (int t = 0; t < times; t++)
      for (int i = 0; i < src->getSize(); i++)
        dst->push(src->get(i));
  }
};

void ResidentArray::runAllTests(const char *csvFile) {
  bool originalSilent = PerfLogger::silent;
  PerfLogger::silent = true;
  cout << "\n=================================================================="
          "==\n ";
  cout << "  HYPOTHESIS TESTING - ARRAY IMPLEMENTATION\n ";
  cout << "===================================================================="
          "\n ";
  cout << "Dataset: " << csvFile << " (" << this->getSize() << " records)\n ";
  cout << "Test runs per measurement: 100 iterations (averaged)\n ";
  cout << "===================================================================="
          "\n\n ";

  int runs = 100;
  int keys[] = {BY_AGE, BY_DIST, BY_EMISSION};
  const char *names[] = {"Age", "Distance", "Emission"};

  cout << "HYPOTHESIS 1.1: Quick Sort Performance\n ";
  cout << "Expectation: Quick sort degrades on already-sorted data (worst-case "
          "O(n²))\n ";
  cout << "--------------------------------------------------------------------"
          "\n ";
  cout << left << setw(20) << "Sort Key " << right << setw(18)
       << "Unsorted (microseconds) " << setw(20) << "Pre-sorted (microseconds) "
       << setw(12) << "Ratio " << endl;
  cout << "--------------------------------------------------------------------"
          "\n ";
  double qsUnsorted[3], qsSorted[3];
  for (int k = 0; k < 3; k++) {
    qsUnsorted[k] = HypothesisTester::avgSortTime(this, keys[k], runs);
    ResidentArray *sorted = new ResidentArray();
    this->copyTo(*sorted);
    sorted->sortBy(keys[k]);
    qsSorted[k] = HypothesisTester::avgSortTime(sorted, keys[k], runs);
    delete sorted;
    cout << left << setw(20) << names[k] << right << setw(18) << fixed
         << setprecision(2) << qsUnsorted[k] << setw(20) << qsSorted[k]
         << setw(10) << setprecision(2) << (qsSorted[k] / qsUnsorted[k]) << "x "
         << endl;
  }
  cout << "\n Verdict: Ratio > 1 confirms Quick Sort degrades on pre-sorted "
          "data\n\n";

  cout << "HYPOTHESIS 1.2: Binary Search vs Linear Search\n ";
  cout << "Expectation: Binary search O(log n) faster than linear scan O(n)\n ";
  cout << "Test: Find all residents aged 26-45 (Working Adults Early)\n ";
  cout << "--------------------------------------------------------------------"
          "\n ";
  ResidentArray *sortedByAge = new ResidentArray();
  this->copyTo(*sortedByAge);
  sortedByAge->sortBy(BY_AGE);
  int testMin = GROUP_MIN[2], testMax = GROUP_MAX[2];
  double bsTotal = 0;
  int bsCount = 0;
  for (int r = 0; r < runs; r++) {
    ResidentArray *results = new ResidentArray();
    auto t1 = chrono::high_resolution_clock::now();
    sortedByAge->bSearchRange(BY_AGE, testMin, testMax, *results);
    auto t2 = chrono::high_resolution_clock::now();
    bsTotal += chrono::duration<double, micro>(t2 - t1).count();
    bsCount = results->getSize();
    delete results;
  }
  double bsAvg = bsTotal / runs;
  double lsTotal = 0;
  int lsCount = 0;
  for (int r = 0; r < runs; r++) {
    auto t1 = chrono::high_resolution_clock::now();
    lsCount = HypothesisTester::linearCount(this, BY_AGE, testMin, testMax);
    auto t2 = chrono::high_resolution_clock::now();
    lsTotal += chrono::duration<double, micro>(t2 - t1).count();
  }
  double lsAvg = lsTotal / runs;
  cout << left << setw(30) << "Search Method " << right << setw(18)
       << "Time (microseconds) " << setw(15) << "Records Found " << endl;
  cout << "--------------------------------------------------------------------"
          "\n ";
  cout << left << setw(30) << "Binary Search (sorted) " << right << setw(18)
       << fixed << setprecision(2) << bsAvg << setw(15) << bsCount << endl;
  cout << left << setw(30) << "Linear Scan (unsorted) " << right << setw(18)
       << lsAvg << setw(15) << lsCount << endl;
  cout << "--------------------------------------------------------------------"
          "\n ";
  cout << " Speedup: " << fixed << setprecision(2) << lsAvg / bsAvg
       << "x faster with binary search\n ";
  ResidentArray *wrongResults = new ResidentArray();
  this->bSearchRange(BY_AGE, testMin, testMax, *wrongResults);
  if (wrongResults->getSize() != lsCount)
    cout << " Binary search on unsorted data returns "
         << wrongResults->getSize() << " (incorrect) vs " << lsCount
         << " (correct)\n ";
  cout << endl;
  delete wrongResults;
  delete sortedByAge;

  cout << "HYPOTHESIS 1.3: Memory Utilization\n";
  cout
      << "Expectation: Fixed-size array wastes memory due to over-allocation\n";
  cout
      << "--------------------------------------------------------------------";
  int sizeofResident = sizeof(Resident);
  int actualUsed = this->getSize() * sizeofResident;
  int capacity = this->getCapacity();
  int allocated = capacity * sizeofResident;
  cout << left << setw(30) << "Metric " << right << setw(20) << "Value "
       << endl;
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(30) << "Records loaded " << right << setw(20)
       << this->getSize() << endl;
  cout << left << setw(30) << "Array capacity " << right << setw(20) << capacity
       << endl;
  cout << left << setw(30) << "Bytes per record " << right << setw(20)
       << sizeofResident << endl;
  cout << left << setw(30) << "Memory allocated " << right << setw(20)
       << allocated << endl;
  cout << left << setw(30) << "Memory actually used " << right << setw(20)
       << actualUsed << endl;
  cout << left << setw(30) << "Wasted memory " << right << setw(20)
       << (allocated - actualUsed) << endl;
  cout << left << setw(30) << "Utilization % " << right << setw(19) << fixed
       << setprecision(1) << (100.0 * this->getSize() / capacity) << "% "
       << endl;
  cout << "\n Verdict: " << (allocated - actualUsed)
       << " bytes wasted per copy\n\n ";

  cout << "HYPOTHESIS 2: CSV Load Time\n ";
  cout << "Expectation: Array loads faster (O(1) index write vs O(n) pointer "
          "traversal)\n ";
  cout
      << "--------------------------------------------------------------------";
  double loadAvg = HypothesisTester::avgLoadTime(csvFile, runs);
  cout << left << setw(30) << "Structure " << right << setw(20)
       << "Load Time (microseconds) " << setw(15) << "Records " << endl;
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(30) << "Array " << right << setw(20) << fixed
       << setprecision(2) << loadAvg << setw(15) << this->getSize() << endl;
  cout << "\n Baseline established for comparison with linked list\n\n ";

  cout << "HYPOTHESIS 3: Memory Footprint\n ";
  cout << "Expectation: Array ~1600 bytes smaller (no next pointers: 200 "
          "records × 8 bytes)\n ";
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(40) << "Component " << right << setw(20) << "Bytes "
       << endl;
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(40) << "Array (allocated capacity) " << right << setw(20)
       << allocated << endl;
  cout << left << setw(40) << "Array (actual data only) " << right << setw(20)
       << actualUsed << endl;
  cout << left << setw(40) << "Expected linked list overhead " << right
       << setw(20) << (this->getSize() * 8) << endl;
  cout << "\n Array saves ~ " << (this->getSize() * 8)
       << " bytes vs linked list (no next pointers)\n\n ";

  cout << "HYPOTHESIS 4: Linear Search for Transport Mode\n ";
  cout << "Expectation: Similar performance on both structures (~200 string "
          "comparisons)\n ";
  cout << "Test: Count all 'Car' users\n ";
  cout
      << "--------------------------------------------------------------------";
  double carSearchTime =
      HypothesisTester::avgLinearSearchTime(this, "Car", runs);
  int carCount = HypothesisTester::linearCountMode(this, "Car");
  cout << left << setw(30) << "Structure " << right << setw(20)
       << "Search Time (microseconds) " << setw(15) << "Cars Found " << endl;
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(30) << "Array " << right << setw(20) << fixed
       << setprecision(2) << carSearchTime << setw(15) << carCount << endl;
  cout << "\n Baseline for O(n) string search comparison\n\n ";

  cout << "HYPOTHESIS 5: Sort Scaling (200 vs 2000 records)\n ";
  cout << "Expectation: Quick sort scales O(n log n), but gap widens at larger "
          "n\n ";
  cout
      << "--------------------------------------------------------------------";
  ResidentArray *big = new ResidentArray();
  HypothesisTester::scaleData(this, big, 10);
  double qs200 = HypothesisTester::avgSortTime(this, BY_EMISSION, runs);
  double qs2000 = HypothesisTester::avgSortTime(big, BY_EMISSION, runs);
  double scale = qs2000 / qs200;
  cout << left << setw(30) << "Dataset " << right << setw(15) << "Records "
       << setw(20) << "Sort Time (microseconds)) " << setw(15)
       << "Scale Factor " << endl;
  cout
      << "--------------------------------------------------------------------";
  cout << left << setw(30) << "Array (200 records) " << right << setw(15) << 200
       << setw(20) << fixed << setprecision(2) << qs200 << setw(14) << "1.00x "
       << endl;
  cout << left << setw(30) << "Array (2000 records) " << right << setw(15)
       << 2000 << setw(20) << qs2000 << setw(13) << setprecision(2) << scale
       << "x " << endl;
  cout << "\n Scaling factor: " << fixed << setprecision(2) << scale
       << "x (theoretical O(n log n) = ~13.3x)\n ";
  delete big;

  cout << "\n=================================================================="
          "==\n";
  cout << "  HYPOTHESIS TESTING COMPLETE\n";
  cout
      << "====================================================================";
  PerfLogger::silent = originalSilent;
}