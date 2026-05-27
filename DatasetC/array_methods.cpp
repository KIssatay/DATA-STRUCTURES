#include "resident.hpp"
#include <fstream>
#include <cstdlib>
#include <chrono>

// Constructor - initializes an empty array
ResidentArray::ResidentArray() {
    capacity = 64;
    size = 0;
    data = new Resident[capacity];
}

// Destructor - frees allocated memory
ResidentArray::~ResidentArray() {
    delete[] data;
    data = nullptr;
    size = 0;
    capacity = 0;
}

// Adds a resident to the array, doubling capacity if needed
void ResidentArray::append(const Resident& r) {
    // Step 1: check if the array is full
    if (size >= capacity) {
        // Step 2: double the capacity to make room for more elements
        capacity *= 2;
        Resident* newData = new Resident[capacity];
        
        // Step 3: copy all existing residents to the new larger array
        for (int i = 0; i < size; i++) newData[i] = data[i];
        
        // Step 4: free the old array and switch to the new one
        delete[] data;
        data = newData;
    }
    
    // Step 5: add the new resident at the end and increment size
    data[size++] = r;
}

void ResidentArray::deepCopy(const ResidentArray& src) {
    for (int i = 0; i < src.size; i++) append(src.data[i]);
}

size_t ResidentArray::memUsage() const {
    return sizeof(Resident) * capacity;
}

// Removes trailing spaces and carriage returns from a string
static string trimToken(const string& str) {
    // Step 1: find where the actual content starts (skip leading spaces)
    size_t start = 0;
    while (start < str.length() && str[start] == ' ') start++;
    
    // Step 2: find where the actual content ends (skip trailing whitespace)
    size_t end = str.length();
    while (end > start && (str[end-1] == '\r' || str[end-1] == ' ' || str[end-1] == '\n')) {
        end--;
    }
    
    // Step 3: return the trimmed substring
    return str.substr(start, end - start);
}

// Parses one line from the CSV file into a Resident struct
static bool parseLine(const string& line, Resident& r) {
    size_t start = 0;
    int field = 0;
    
    // Step 1: walk through the line and split it at each comma
    for (size_t i = 0; i <= line.length(); i++) {
        if (i == line.length() || line[i] == ',') {
            // Step 2: extract the field between start and i
            string token = trimToken(line.substr(start, i - start));
            
            // Step 3: store the field in the appropriate struct member
            if (field == 0) {
                r.residentID = token;
            } else if (field == 1) {
                r.age = atoi(token.c_str());
            } else if (field == 2) {
                r.modeOfTransport = token;
            } else if (field == 3) {
                r.dailyDistance = atof(token.c_str());
            } else if (field == 4) {
                r.carbonFactor = atof(token.c_str());
            } else if (field == 5) {
                r.avgDaysPerMonth = atoi(token.c_str());
                
                // Step 4: calculate derived fields
                r.monthlyEmission = computeEmission(r.dailyDistance, r.carbonFactor, r.avgDaysPerMonth);
                r.ageGroup = assignAgeGroup(r.age);
                return true;
            }
            
            field++;
            start = i + 1;
        }
    }
    
    return false;
}

// Loads resident data from a CSV file into the array
int ResidentArray::loadCSV(const char* fname) {
    // Step 1: try to open the file
    ifstream file(fname);
    if (!file.is_open()) return -1;

    string line;
    bool skipHeader = true;
    int loaded = 0;

    // Step 2: read the file line by line
    while (getline(file, line)) {
        // Step 3: skip the first line (column headers)
        if (skipHeader) {
            skipHeader = false;
            continue;
        }
        
        // Step 4: skip empty lines
        if (line.empty() || line[0] == '\r') continue;
        
        // Step 5: parse the line and add it to the array
        Resident r;
        if (parseLine(line, r)) {
            append(r);
            loaded++;
        }
    }
    
    file.close();
    return loaded;
}

void ResidentArray::display() const {
    cout << "[ARRAY – showing " << size << " of " << size << " records]\n";
    printHeader();
    for (int i = 0; i < size; i++) printRow(data[i]);
    cout << "\n";
}

// Merges two sorted portions of an array back together
static void mergeArr(Resident* arr, int left, int mid, int right, int key) {
    // Step 1: calculate the sizes of the two halves
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    // Step 2: create temporary arrays to hold each half
    Resident* leftArr = new Resident[leftSize];
    Resident* rightArr = new Resident[rightSize];
    
    // Step 3: copy data into the temporary arrays
    for (int i = 0; i < leftSize; i++) leftArr[i] = arr[left + i];
    for (int j = 0; j < rightSize; j++) rightArr[j] = arr[mid + 1 + j];

    // Step 4: merge the two halves back into the original array in sorted order
    int i = 0, j = 0, k = left;
    while (i < leftSize && j < rightSize)
        arr[k++] = (cmpKey(leftArr[i], rightArr[j], key) <= 0) ? leftArr[i++] : rightArr[j++];
    
    // Step 5: copy any remaining elements from the left half
    while (i < leftSize) arr[k++] = leftArr[i++];
    
    // Step 6: copy any remaining elements from the right half
    while (j < rightSize) arr[k++] = rightArr[j++];

    // Step 7: free the temporary arrays
    delete[] leftArr;
    delete[] rightArr;
}

// Recursively sorts an array using merge sort
static void mergeSortArr(Resident* arr, int left, int right, int key) {
    // Step 1: base case - if there's one or zero elements, it's already sorted
    if (left >= right) return;
    
    // Step 2: find the middle point to split the array
    int mid = left + (right - left) / 2;
    
    // Step 3: recursively sort the left half
    mergeSortArr(arr, left, mid, key);
    
    // Step 4: recursively sort the right half
    mergeSortArr(arr, mid + 1, right, key);
    
    // Step 5: merge the two sorted halves back together
    mergeArr(arr, left, mid, right, key);
}

void ResidentArray::mergeSort(int key) {
    if (size > 1) mergeSortArr(data, 0, size - 1, key);
}

// Sorts an array using insertion sort
void ResidentArray::insertionSort(int key) {
    // Step 1: start from the second element (first is already "sorted")
    for (int i = 1; i < size; i++) {
        // Step 2: save the current element we're trying to insert
        Resident current = data[i];
        int j = i - 1;
        
        // Step 3: shift larger elements to the right to make space
        while (j >= 0 && cmpKey(data[j], current, key) > 0) {
            data[j + 1] = data[j];
            j--;
        }
        
        // Step 4: insert the current element in its correct position
        data[j + 1] = current;
    }
}

// Searches for all residents in a specific age group
int ResidentArray::linSearchAgeGroup(const string& group,
                      Resident* results, int maxResults) const {
    int count = 0;
    for (int i = 0; i < size && count < maxResults; i++)
        if (data[i].ageGroup == group)
            results[count++] = data[i];
    return count;
}

// Searches for all residents using a specific transport mode
int ResidentArray::linSearchMode(const string& mode,
                  Resident* results, int maxResults) const {
    int count = 0;
    for (int i = 0; i < size && count < maxResults; i++)
        if (data[i].modeOfTransport == mode)
            results[count++] = data[i];
    return count;
}

// Searches for residents traveling more than a threshold distance
int ResidentArray::linSearchDist(double threshold,
                  Resident* results, int maxResults) const {
    int count = 0;
    for (int i = 0; i < size && count < maxResults; i++)
        if (data[i].dailyDistance > threshold)
            results[count++] = data[i];
    return count;
}

// Jump search for residents within an age range (array must be sorted by age)
int ResidentArray::jumpSearchAge(int minAge, int maxAge,
                  Resident* results, int maxResults) const {
    int n = size;
    if (n == 0) return 0;
    int stepSize = (int)sqrt((double)n);
    int previous = 0;

    // Step 1: jump ahead in blocks until we find one that might contain minAge
    while (data[iMin(previous + stepSize, n) - 1].age < minAge) {
        previous += stepSize;
        if (previous >= n) return 0;
    }

    // Step 2: do a linear search within the block to find the first match
    while (previous < n && data[previous].age < minAge) previous++;

    // Step 3: collect all residents in the age range
    int count = 0;
    while (previous < n && data[previous].age <= maxAge && count < maxResults)
        results[count++] = data[previous++];
    return count;
}

// Jump search for residents above a distance threshold (array must be sorted by distance)
int ResidentArray::jumpSearchDist(double threshold,
                   Resident* results, int maxResults) const {
    int n = size;
    if (n == 0) return 0;
    int stepSize = (int)sqrt((double)n);
    int previous = 0;

    // Step 1: jump ahead until we find a block that might contain values above threshold
    while (data[iMin(previous + stepSize, n) - 1].dailyDistance <= threshold) {
        previous += stepSize;
        if (previous >= n) return 0;
    }

    // Step 2: linear search within the block to find the first match
    while (previous < n && data[previous].dailyDistance <= threshold) previous++;

    // Step 3: collect all residents above the threshold
    int count = 0;
    while (previous < n && count < maxResults)
        results[count++] = data[previous++];
    return count;
}

static const string AGE_GROUPS[] = {
    "Children & Teenagers",
    "Young Adults",
    "Working Adults (Early)",
    "Working Adults (Late)",
    "Senior Citizens"
};
static const int N_GROUPS = 5;

static const string MODES[] = {
    "Car", "Bicycle", "Bus", "Carpool", "Walking", "School Bus"
};
static const int N_MODES = 6;

static int modeIndex(const string& mode) {
    for (int i = 0; i < N_MODES; i++)
        if (MODES[i] == mode) return i;
    return -1;
}

struct GroupStat {
    int    count;
    double totalEmission;
    int    modeCounts[6];
};

// Analyzes and displays carbon emissions grouped by age category
void ResidentArray::emissionByAgeGroup() const {
    // Step 1: initialize statistics for each age group
    GroupStat groups[N_GROUPS];
    for (int g = 0; g < N_GROUPS; g++) {
        groups[g].count = 0;
        groups[g].totalEmission = 0.0;
        for (int m = 0; m < N_MODES; m++) groups[g].modeCounts[m] = 0;
    }

    // Step 2: go through all residents and accumulate stats for their age group
    for (int i = 0; i < size; i++) {
        for (int g = 0; g < N_GROUPS; g++) {
            if (data[i].ageGroup == AGE_GROUPS[g]) {
                groups[g].count++;
                groups[g].totalEmission += data[i].monthlyEmission;
                int modeIdx = modeIndex(data[i].modeOfTransport);
                if (modeIdx >= 0) groups[g].modeCounts[modeIdx]++;
                break;
            }
        }
    }

    // Step 3: print the table header
    boxTitle("CARBON EMISSION ANALYSIS BY AGE GROUP");
    cout << left
         << setw(26) << "Age Group"
         << setw(7)  << "Count"
         << setw(20) << "Total CO2 (kg/mth)"
         << setw(20) << "Avg CO2/Resident"
         << setw(18) << "Preferred Mode"
         << "\n";
    hRule(91);

    // Step 4: print stats for each age group
    for (int g = 0; g < N_GROUPS; g++) {
        double average = (groups[g].count > 0) ? groups[g].totalEmission / groups[g].count : 0.0;

        // Step 5: find the most popular transport mode for this group
        int bestMode = 0;
        for (int m = 1; m < N_MODES; m++)
            if (groups[g].modeCounts[m] > groups[g].modeCounts[bestMode]) bestMode = m;
        string preferred = (groups[g].count > 0) ? MODES[bestMode] : "-";

        cout << left << fixed << setprecision(2)
             << setw(26) << AGE_GROUPS[g]
             << setw(7)  << groups[g].count
             << setw(20) << groups[g].totalEmission
             << setw(20) << average
             << setw(18) << preferred
             << "\n";
    }
    cout << "\n";
}

// Analyzes and displays carbon emissions grouped by transport mode
void ResidentArray::emissionByMode() const {
    // Step 1: initialize totals for each transport mode
    double totalEmissions[N_MODES] = {0};
    int    counts[N_MODES] = {0};

    // Step 2: accumulate emissions for each mode
    for (int i = 0; i < size; i++) {
        int modeIdx = modeIndex(data[i].modeOfTransport);
        if (modeIdx >= 0) {
            totalEmissions[modeIdx] += data[i].monthlyEmission;
            counts[modeIdx]++;
        }
    }

    // Step 3: calculate the grand total across all modes
    double grandTotal = 0.0;
    for (int m = 0; m < N_MODES; m++) grandTotal += totalEmissions[m];

    // Step 4: print the table header
    boxTitle("CARBON EMISSION ANALYSIS BY TRANSPORT MODE");
    cout << left
         << setw(14) << "Mode"
         << setw(8)  << "Count"
         << setw(22) << "Total CO2 (kg/mth)"
         << setw(20) << "Avg CO2/Resident"
         << setw(12) << "Share (%)"
         << "\n";
    hRule(76);

    // Step 5: print stats for each transport mode
    for (int m = 0; m < N_MODES; m++) {
        double average = (counts[m] > 0) ? totalEmissions[m] / counts[m] : 0.0;
        double share = (grandTotal > 0.0) ? totalEmissions[m] / grandTotal * 100.0 : 0.0;
        cout << left << fixed << setprecision(2)
             << setw(14) << MODES[m]
             << setw(8)  << counts[m]
             << setw(22) << totalEmissions[m]
             << setw(20) << average
             << setw(12) << share
             << "\n";
    }
    
    // Step 6: print the grand total
    cout << fixed << setprecision(2)
         << "\n  City C TOTAL Monthly CO2 Emissions: " << grandTotal << " kg\n\n";
}

// Creates a 2D matrix showing emissions by age group and transport mode
void ResidentArray::emissionCrossAnalysis() const {
    // Step 1: initialize the matrix with zeros
    double matrix[N_GROUPS][N_MODES];
    for (int g = 0; g < N_GROUPS; g++)
        for (int m = 0; m < N_MODES; m++)
            matrix[g][m] = 0.0;

    // Step 2: fill the matrix by going through all residents
    for (int i = 0; i < size; i++) {
        // Step 3: find which age group this resident belongs to
        int groupIdx = -1;
        for (int g = 0; g < N_GROUPS; g++) {
            if (data[i].ageGroup == AGE_GROUPS[g]) {
                groupIdx = g;
                break;
            }
        }
        
        // Step 4: find which transport mode they use
        int modeIdx = modeIndex(data[i].modeOfTransport);
        
        // Step 5: add their emissions to the appropriate cell
        if (groupIdx >= 0 && modeIdx >= 0) {
            matrix[groupIdx][modeIdx] += data[i].monthlyEmission;
        }
    }

    // Step 6: print the table header
    boxTitle("CROSS-ANALYSIS: AGE GROUP vs TRANSPORT MODE (Total CO2 kg/mth)");
    cout << left << setw(26) << "Age Group";
    for (int m = 0; m < N_MODES; m++) cout << setw(12) << MODES[m];
    cout << "\n";
    hRule(26 + 12 * N_MODES);

    // Step 7: print each row of the matrix
    for (int g = 0; g < N_GROUPS; g++) {
        cout << left << setw(26) << AGE_GROUPS[g];
        for (int m = 0; m < N_MODES; m++) {
            cout << fixed << setprecision(1) << setw(12) << matrix[g][m];
        }
        cout << "\n";
    }
    cout << "\n";
}
// Display and timing functions moved from display.cpp
using namespace chrono;

typedef high_resolution_clock HRC;
typedef HRC::time_point TP;

inline TP now() { return HRC::now(); }
inline double elapsedMs(TP start) {
    return duration<double, milli>(HRC::now() - start).count();
}

// Displays search results with timing information
void showSearchResults(const string& title, Resident* results, int count,
                       const string& structure, double milliseconds, int maxShow) {
    // Step 1: print the search summary line
    cout << "  [" << structure << "]  " << title
         << " -> " << count << " match(es) found in "
         << fixed << setprecision(4) << milliseconds << " ms\n";
    
    // Step 2: if there are results, display them in a table
    if (count > 0) {
        int limit = iMin(maxShow, count);
        printHeader();
        
        // Step 3: print up to maxShow results
        for (int i = 0; i < limit; i++) printRow(results[i]);
        
        // Step 4: if there are more results, show how many were omitted
        if (count > maxShow)
            cout << "  ... (" << count - maxShow << " more) ...\n";
        cout << "\n";
    }
}

// Prints the performance comparison table
void printTimingTable(TimingRow* rows, int numRows) {
    boxTitle("PERFORMANCE COMPARISON TABLE");
    
    // Step 1: print the table header
    cout << left
         << setw(38) << "Operation"
         << setw(15) << "Array (ms)"
         << setw(15) << "List (ms)"
         << setw(15) << "Array (B)"
         << setw(15) << "List (B)"
         << setw(12) << "Faster"
         << "\n";
    hRule(110);
    
    // Step 2: print each timing result row
    for (int i = 0; i < numRows; i++) {
        // Step 3: determine which structure was faster
        string faster = (rows[i].arrayMs <= rows[i].listMs) ? "Array" : "List";
        
        cout << left << fixed << setprecision(4)
             << setw(38) << rows[i].label
             << setw(15) << rows[i].arrayMs
             << setw(15) << rows[i].listMs
             << setw(15) << rows[i].arrayMem
             << setw(15) << rows[i].listMem
             << setw(12) << faster
             << "\n";
    }
    cout << "\n";
}

// Displays insights and recommendations for city planners
void displayInsightsAndRecommendations() {
    boxTitle("INSIGHTS & RECOMMENDATIONS FOR CITY PLANNERS");
    cout << "  1. HIGHEST EMITTERS\n";
    cout << "     Working Adults (Early, 26-45) form the largest commuter group and\n";
    cout << "     generate the most total CO2 due to frequent car use on longer routes.\n\n";
    
    cout << "  2. ZERO-EMISSION MODES\n";
    cout << "     Bicycle and Walking produce 0 kg CO2. Expanding cycling lanes and\n";
    cout << "     pedestrian paths in City C would directly cut emissions.\n\n";
    
    cout << "  3. LOW-EMISSION ALTERNATIVE\n";
    cout << "     Bus and Carpool share the per-km cost across multiple riders.\n";
    cout << "     Incentivising these modes for Car users could reduce emissions by\n";
    cout << "     roughly 30-70% per resident.\n\n";
    
    cout << "  4. DATA STRUCTURE CHOICE\n";
    cout << "     Arrays outperform linked lists for Jump Search and random access.\n";
    cout << "     Linked lists avoid costly memory reallocation for dynamic insertions.\n";
    cout << "     For this dataset (122 records), both perform comparably; at scale\n";
    cout << "     (10,000+ records), arrays are preferable for search-heavy workloads.\n\n";
    
    cout << "  5. SORTING ALGORITHM CHOICE\n";
    cout << "     Merge Sort is recommended for large, unsorted datasets (O(n log n)).\n";
    cout << "     Insertion Sort is efficient only for nearly-sorted or tiny datasets.\n\n";
    
    cout << "  6. AGE-SPECIFIC INTERVENTIONS\n";
    cout << "     Young Adults (18-25) show higher bicycle usage - expand bike-sharing.\n";
    cout << "     Working Adults (26-60) rely heavily on cars - promote carpooling.\n";
    cout << "     Senior Citizens (61+) use walking more - improve pedestrian safety.\n\n";
}

// Displays time and space complexity analysis
void displayComplexityAnalysis() {
    boxTitle("TIME & SPACE COMPLEXITY ANALYSIS");
    cout << left
         << setw(28) << "Algorithm"
         << setw(18) << "Time Complexity"
         << setw(22) << "Array Space"
         << setw(22) << "Linked List Space"
         << "\n";
    hRule(90);
    cout << setw(28) << "Merge Sort"
         << setw(18) << "O(n log n)"
         << setw(22) << "O(n) [temp arrays]"
         << setw(22) << "O(log n) [call stack]" << "\n";
    cout << setw(28) << "Insertion Sort"
         << setw(18) << "O(n^2)"
         << setw(22) << "O(1)"
         << setw(22) << "O(1)" << "\n";
    cout << setw(28) << "Linear Search"
         << setw(18) << "O(n)"
         << setw(22) << "O(1)"
         << setw(22) << "O(1)" << "\n";
    cout << setw(28) << "Jump Search"
         << setw(18) << "O(sqrt n)"
         << setw(22) << "O(1) [true O(sqrt n)]"
         << setw(22) << "O(n) [no random access]" << "\n";
    cout << "\n";
    cout << "  * Jump Search on a SINGLY LINKED LIST cannot achieve O(sqrt n)\n";
    cout << "    because advancing 'step' nodes still requires O(step) traversal.\n";
    cout << "    True benefit of Jump Search is only realised on arrays.\n\n";
    cout << "  * Merge Sort uses O(n) auxiliary space for arrays (temp L/R buffers)\n";
    cout << "    but only O(log n) recursive stack space for linked lists (in-place split).\n\n";
}

// Runs all sorting experiments and records timing data
void runSortingExperiments(const ResidentArray& masterArr, TimingRow* rows, int& rowIndex) {
    boxTitle("SORTING EXPERIMENTS");

    cout << ">>> Sorting by AGE\n\n";

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now();
        temp.mergeSort(0);
        double ms = elapsedMs(start);
        cout << "  Merge Sort (Array)        : " << fixed << setprecision(4) << ms << " ms\n";
        cout << "  Sample (first 10 rows, sorted by Age):\n";
        printHeader();
        for (int i = 0; i < iMin(10, temp.getSize()); i++) printRow(temp.getData()[i]);
        cout << "\n";
        rows[rowIndex].label = "Merge Sort – Age";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now();
        temp.mergeSort(0);
        double ms = elapsedMs(start);
        cout << "  Merge Sort (Linked List)  : " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now();
        temp.insertionSort(0);
        double ms = elapsedMs(start);
        cout << "  Insertion Sort (Array)    : " << fixed << setprecision(4) << ms << " ms\n";
        rows[rowIndex].label = "Insertion Sort – Age";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now();
        temp.insertionSort(0);
        double ms = elapsedMs(start);
        cout << "  Insertion Sort (Linked List): " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }

    cout << ">>> Sorting by DAILY DISTANCE\n\n";

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now(); temp.mergeSort(1); double ms = elapsedMs(start);
        cout << "  Merge Sort (Array)        : " << fixed << setprecision(4) << ms << " ms\n";
        cout << "  Sample (first 10 rows):\n"; printHeader();
        for (int i = 0; i < iMin(10, temp.getSize()); i++) printRow(temp.getData()[i]);
        cout << "\n";
        rows[rowIndex].label = "Merge Sort – Daily Distance";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now(); temp.mergeSort(1); double ms = elapsedMs(start);
        cout << "  Merge Sort (Linked List)  : " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now(); temp.insertionSort(1); double ms = elapsedMs(start);
        cout << "  Insertion Sort (Array)    : " << fixed << setprecision(4) << ms << " ms\n";
        rows[rowIndex].label = "Insertion Sort – Daily Distance";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now(); temp.insertionSort(1); double ms = elapsedMs(start);
        cout << "  Insertion Sort (Linked List): " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }

    cout << ">>> Sorting by MONTHLY CO2 EMISSION\n\n";

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now(); temp.mergeSort(2); double ms = elapsedMs(start);
        cout << "  Merge Sort (Array)        : " << fixed << setprecision(4) << ms << " ms\n";
        cout << "  Sample (first 10 rows):\n"; printHeader();
        for (int i = 0; i < iMin(10, temp.getSize()); i++) printRow(temp.getData()[i]);
        cout << "\n";
        rows[rowIndex].label = "Merge Sort – Monthly CO2";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now(); temp.mergeSort(2); double ms = elapsedMs(start);
        cout << "  Merge Sort (Linked List)  : " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }

    {
        ResidentArray temp;
        temp.deepCopy(masterArr);
        TP start = now(); temp.insertionSort(2); double ms = elapsedMs(start);
        cout << "  Insertion Sort (Array)    : " << fixed << setprecision(4) << ms << " ms\n";
        rows[rowIndex].label = "Insertion Sort – Monthly CO2";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = temp.memUsage();
    }
    {
        ResidentList temp;
        temp.buildFromArray(masterArr);
        TP start = now(); temp.insertionSort(2); double ms = elapsedMs(start);
        cout << "  Insertion Sort (Linked List): " << fixed << setprecision(4) << ms << " ms\n\n";
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = temp.memUsage();
        rowIndex++;
    }
}

// Runs all searching experiments and records timing data
void runSearchingExperiments(const ResidentArray& masterArr, const ResidentList& masterList, 
                             TimingRow* rows, int& rowIndex) {
    boxTitle("SEARCHING EXPERIMENTS");
    int maxResults = 500;
    Resident* buffer = new Resident[maxResults];

    cout << "--- LINEAR SEARCH on unsorted data ---\n\n";

    {
        string group = "Working Adults (Early)";
        TP start = now();
        int count = masterArr.linSearchAgeGroup(group, buffer, maxResults);
        double ms = elapsedMs(start);
        showSearchResults("Age Group = 'Working Adults (Early)'", buffer, count, "Array", ms);

        rows[rowIndex].label = "Linear Search – Age Group";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();

        start = now();
        count = masterList.linSearchAgeGroup(group, buffer, maxResults);
        ms = elapsedMs(start);
        showSearchResults("Age Group = 'Working Adults (Early)'", buffer, count, "Linked List", ms);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    cout << "  --- Search: Mode = 'Car' ---\n";
    {
        TP start = now();
        int count = masterArr.linSearchMode("Car", buffer, maxResults);
        double ms = elapsedMs(start);
        showSearchResults("Mode = 'Car'", buffer, count, "Array", ms);

        rows[rowIndex].label = "Linear Search – Mode (Car)";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();

        start = now();
        count = masterList.linSearchMode("Car", buffer, maxResults);
        ms = elapsedMs(start);
        showSearchResults("Mode = 'Car'", buffer, count, "Linked List", ms);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    cout << "  --- Search: DailyDistance > 15 km ---\n";
    {
        TP start = now();
        int count = masterArr.linSearchDist(15.0, buffer, maxResults);
        double ms = elapsedMs(start);
        showSearchResults("DailyDistance > 15 km", buffer, count, "Array", ms);

        rows[rowIndex].label = "Linear Search – Distance > 15km";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = masterArr.memUsage();

        start = now();
        count = masterList.linSearchDist(15.0, buffer, maxResults);
        ms = elapsedMs(start);
        showSearchResults("DailyDistance > 15 km", buffer, count, "Linked List", ms);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = masterList.memUsage();
        rowIndex++;
    }

    cout << "\n--- JUMP SEARCH on sorted data (sort first, then search) ---\n\n";

    ResidentArray sortedArr;
    sortedArr.deepCopy(masterArr);
    ResidentList sortedList;
    sortedList.buildFromArray(masterArr);
    sortedArr.mergeSort(0);
    sortedList.mergeSort(0);

    cout << "  --- Jump Search: Age 26–45 (Working Adults Early) ---\n";
    {
        TP start = now();
        int count = sortedArr.jumpSearchAge(26, 45, buffer, maxResults);
        double ms = elapsedMs(start);
        showSearchResults("Jump Search Age [26,45]", buffer, count, "Array (sorted)", ms);

        rows[rowIndex].label = "Jump Search – Age [26,45]";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = sortedArr.memUsage();

        start = now();
        count = sortedList.jumpSearchAge(26, 45, buffer, maxResults);
        ms = elapsedMs(start);
        showSearchResults("Jump Search Age [26,45]", buffer, count, "List  (sorted)", ms);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = sortedList.memUsage();
        rowIndex++;
    }

    ResidentArray sortedArrDist;
    sortedArrDist.deepCopy(masterArr);
    sortedArrDist.mergeSort(1);
    
    ResidentList sortedListDist;
    sortedListDist.buildFromArray(masterArr);
    sortedListDist.mergeSort(1);

    cout << "  --- Jump Search: DailyDistance > 15 km ---\n";
    {
        TP start = now();
        int count = sortedArrDist.jumpSearchDist(15.0, buffer, maxResults);
        double ms = elapsedMs(start);
        showSearchResults("Jump Search Distance > 15km", buffer, count, "Array (sorted)", ms);

        rows[rowIndex].label = "Jump Search – Distance > 15km";
        rows[rowIndex].arrayMs = ms;
        rows[rowIndex].arrayMem = sortedArrDist.memUsage();

        start = now();
        count = sortedListDist.jumpSearchDist(15.0, buffer, maxResults);
        ms = elapsedMs(start);
        showSearchResults("Jump Search Distance > 15km", buffer, count, "List  (sorted)", ms);
        rows[rowIndex].listMs = ms;
        rows[rowIndex].listMem = sortedListDist.memUsage();
        rowIndex++;
    }

    delete[] buffer;
}