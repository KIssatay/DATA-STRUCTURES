
// Output order follows assignment sections:
//   PART 1 (Array):
//     analyzeAgeGroups / emissionAnalysis / sorting / searching
//     performanceAnalysis / H2 / H3 / H4 / insights
//
//   PART 2 (Linked List):
//     [Section 4a / 4b]         Age group categorisation
//     [Section 5a / 5b / 5c / 5d]  Emission analysis tables
//     [Section 9a / 9b / 9c]    Insights and recommendations
//     [Section 6]               Sorting experiments
//     [Section 7]               Searching experiments
//     [Section 8]               Performance summary table
//
// Compile:
//   g++ main.cpp residentList.cpp ResidentArray.cpp -o program
// ============================================================

#include "resident.hpp"

// ============================================================
// PERFORMANCE TABLE ROW PRINTERS  (linked list side)
//
// These two small helpers print one row each in the Section 8
// performance summary table. They live in main.cpp because the
// table is assembled here from results collected across all
// experiments. They are not methods of ResidentList because
// they do not access any list data.
// ============================================================

// printSortRow
// Prints one row of the sorting comparison table.
// Columns: operation label | algorithm | complexity
//          | comparison count | execution time in microseconds
void printSortRow(string label, string algorithm,
                  string complexity, int compCount, long timeUs)
{
    cout << padStr(label,              36)
         << padStr(algorithm,          14)
         << padStr(complexity,         14)
         << padStr(intStr(compCount),  14)
         << padStr(intStr((int)timeUs), 10) << endl;
}

// printSearchRow
// Prints one row of the search comparison table.
// Slightly narrower label column than the sort table.
void printSearchRow(string label, string algorithm,
                    string complexity, int compCount, long timeUs)
{
    cout << padStr(label,              32)
         << padStr(algorithm,          18)
         << padStr(complexity,         12)
         << padStr(intStr(compCount),  14)
         << padStr(intStr((int)timeUs), 10) << endl;
}

// ============================================================
// runSortingExperiments  [Section 6]
//
// Runs Bubble Sort and Merge Sort on three fields:
//   Age, Daily Distance, Monthly Emission.
//
// Each experiment:
//   - Reloads a fresh copy of the CSV so the starting order is
//     always the original unsorted CSV order
//   - Records comparison count and execution time in microseconds
//   - Displays the first 8 sorted rows as a visual check
//
// All results are written back through the out-parameters so
// main can pass them directly to the Section 8 summary table.
//
// Proves H1: Merge Sort O(n log n) always uses far fewer
//   comparisons than Bubble Sort O(n^2) on the same data.
// ============================================================
void runSortingExperiments(
    string filename,
    int&  bubbleAgeComp,  long& bubbleAgeTime,
    int&  mergeAgeComp,   long& mergeAgeTime,
    int&  bubbleDistComp, long& bubbleDistTime,
    int&  mergeDistComp,  long& mergeDistTime,
    int&  bubbleEmitComp, long& bubbleEmitTime,
    int&  mergeEmitComp,  long& mergeEmitTime)
{
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 6]  SORTING EXPERIMENTS                             " << endl;
    cout << "  Bubble Sort O(n^2)  vs  Merge Sort O(n log n)               " << endl;
    cout << "  Each run reloads the CSV to start from the same order.      " << endl;
    cout << "  Proves H1: Merge Sort always uses fewer comparisons.        " << endl;
    cout << "===============================================================" << endl;

    clock_t startTime;
    clock_t endTime;
    int     comp = 0;  // reused for each experiment; final value saved separately

    // ----------------------------------------------------------
    // [S1] Bubble Sort by Age
    // ----------------------------------------------------------
    cout << endl << "[S1] Bubble Sort by Age" << endl;
    ResidentList listBubbleAge;
    listBubbleAge.loadFromCSV(filename);
    startTime     = clock();
    listBubbleAge.bubbleSort("age", comp);
    endTime       = clock();
    bubbleAgeComp = comp;
    bubbleAgeTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << bubbleAgeComp
         << " | Time: "          << bubbleAgeTime << " us" << endl;
    listBubbleAge.displayList(8);

    // ----------------------------------------------------------
    // [S2] Merge Sort by Age
    // ----------------------------------------------------------
    cout << endl << "[S2] Merge Sort by Age" << endl;
    ResidentList listMergeAge;
    listMergeAge.loadFromCSV(filename);
    startTime    = clock();
    listMergeAge.mergeSort("age", comp);
    endTime      = clock();
    mergeAgeComp = comp;
    mergeAgeTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << mergeAgeComp
         << " | Time: "          << mergeAgeTime << " us" << endl;
    listMergeAge.displayList(8);

    // ----------------------------------------------------------
    // [S3] Bubble Sort by Daily Distance
    // ----------------------------------------------------------
    cout << endl << "[S3] Bubble Sort by Daily Distance" << endl;
    ResidentList listBubbleDist;
    listBubbleDist.loadFromCSV(filename);
    startTime      = clock();
    listBubbleDist.bubbleSort("distance", comp);
    endTime        = clock();
    bubbleDistComp = comp;
    bubbleDistTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << bubbleDistComp
         << " | Time: "          << bubbleDistTime << " us" << endl;

    // ----------------------------------------------------------
    // [S4] Merge Sort by Daily Distance
    // ----------------------------------------------------------
    cout << "[S4] Merge Sort by Daily Distance" << endl;
    ResidentList listMergeDist;
    listMergeDist.loadFromCSV(filename);
    startTime     = clock();
    listMergeDist.mergeSort("distance", comp);
    endTime       = clock();
    mergeDistComp = comp;
    mergeDistTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << mergeDistComp
         << " | Time: "          << mergeDistTime << " us" << endl;

    // ----------------------------------------------------------
    // [S5] Bubble Sort by Monthly Emission
    // ----------------------------------------------------------
    cout << "[S5] Bubble Sort by Monthly Emission" << endl;
    ResidentList listBubbleEmit;
    listBubbleEmit.loadFromCSV(filename);
    startTime      = clock();
    listBubbleEmit.bubbleSort("emission", comp);
    endTime        = clock();
    bubbleEmitComp = comp;
    bubbleEmitTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << bubbleEmitComp
         << " | Time: "          << bubbleEmitTime << " us" << endl;

    // ----------------------------------------------------------
    // [S6] Merge Sort by Monthly Emission
    // ----------------------------------------------------------
    cout << "[S6] Merge Sort by Monthly Emission" << endl;
    ResidentList listMergeEmit;
    listMergeEmit.loadFromCSV(filename);
    startTime     = clock();
    listMergeEmit.mergeSort("emission", comp);
    endTime       = clock();
    mergeEmitComp = comp;
    mergeEmitTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << mergeEmitComp
         << " | Time: "          << mergeEmitTime << " us" << endl;
}

// ============================================================
// runH2OrderSensitivityTest  [Section 6 / H2]
//
// Tests whether Bubble Sort comparison counts change when the
// input is already sorted versus when it is in random order.
// Merge Sort should produce the same count in both conditions
// because its divide-and-merge structure depends only on list
// size, not on the values stored in the nodes.
//
// Already-sorted input: uses listMergeAgeRef (sorted in S2).
// Random-order input: reloads a fresh copy from the CSV.
//
// Proves H2: Bubble Sort is highly sensitive to input order.
//            Merge Sort is completely unaffected by input order.
// ============================================================
void runH2OrderSensitivityTest(
    string         filename,
    ResidentList&  listMergeAgeRef,
    int&  h2BubbleSortedComp, long& h2BubbleSortedTime,
    int&  h2MergeSortedComp,  long& h2MergeSortedTime,
    int&  h2BubbleRandomComp, long& h2BubbleRandomTime,
    int&  h2MergeRandomComp,  long& h2MergeRandomTime)
{
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 6 / H2]  DATA ORDER SENSITIVITY TEST               " << endl;
    cout << "  Proves H2: Bubble Sort comparison count changes with order. " << endl;
    cout << "             Merge Sort count stays the same regardless.      " << endl;
    cout << "===============================================================" << endl;

    clock_t startTime;
    clock_t endTime;
    int     comp = 0;

    // ----------------------------------------------------------
    // [H2-C1] Already sorted input
    // listMergeAgeRef was sorted by Merge Sort in S2.
    // Bubble Sort needs only one clean pass -> ~n-1 comparisons.
    // ----------------------------------------------------------
    cout << endl << "[H2-C1] Already sorted input (reusing list from S2)" << endl;

    cout << "  Bubble Sort (expect ~177 comparisons - one pass, no swaps):" << endl;
    startTime           = clock();
    listMergeAgeRef.bubbleSort("age", comp);
    endTime             = clock();
    h2BubbleSortedComp  = comp;
    h2BubbleSortedTime  = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << h2BubbleSortedComp
         << " | Time: " << h2BubbleSortedTime << " us" << endl;

    cout << "  Merge Sort (always same count - structure-driven):" << endl;
    startTime           = clock();
    listMergeAgeRef.mergeSort("age", comp);
    endTime             = clock();
    h2MergeSortedComp   = comp;
    h2MergeSortedTime   = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << h2MergeSortedComp
         << " | Time: " << h2MergeSortedTime << " us" << endl;

    // ----------------------------------------------------------
    // [H2-C2] Random / unsorted input (fresh reload from CSV)
    // Bubble Sort approaches O(n^2) worst-case here.
    // Merge Sort count should match H2-C1 exactly.
    // ----------------------------------------------------------
    cout << endl << "[H2-C2] Random order (original CSV) - fresh reload" << endl;

    ResidentList listH2Bubble;
    listH2Bubble.loadFromCSV(filename);
    cout << "  Bubble Sort (approaches O(n^2) on random data):" << endl;
    startTime           = clock();
    listH2Bubble.bubbleSort("age", comp);
    endTime             = clock();
    h2BubbleRandomComp  = comp;
    h2BubbleRandomTime  = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << h2BubbleRandomComp
         << " | Time: " << h2BubbleRandomTime << " us" << endl;

    ResidentList listH2Merge;
    listH2Merge.loadFromCSV(filename);
    cout << "  Merge Sort (same count as sorted - input order irrelevant):" << endl;
    startTime           = clock();
    listH2Merge.mergeSort("age", comp);
    endTime             = clock();
    h2MergeRandomComp   = comp;
    h2MergeRandomTime   = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
    cout << "     Comparisons: " << h2MergeRandomComp
         << " | Time: " << h2MergeRandomTime << " us" << endl;

    // ----------------------------------------------------------
    // [H2-C3] Theoretical worst-case note
    // Printed for documentation; no sort is re-run here.
    // ----------------------------------------------------------
    cout << endl << "[H2-C3] Theoretical worst case (reverse sorted input):" << endl;
    cout << "     Bubble Sort max     : n*(n-1)/2 = "
         << (178 * 177 / 2) << " comparisons" << endl;
    cout << "     Merge Sort always   : ~1335 comparisons  [ n * log2(n) ]" << endl;
    cout << "     Bubble/Merge ratio  : ~" << (178 * 177 / 2 / 1335)
         << "x more comparisons for Bubble Sort" << endl;
}

// ============================================================
// runSearchingExperiments  [Section 7]
//
// Runs five Linear Searches on an unsorted list and five
// Sentinel Searches on pre-sorted lists.
//
// Linear Search visits every node regardless of a match —
// comparison count always equals total records. This is the
// O(n) baseline against which Sentinel is measured.
//
// Sentinel Search stops as soon as the current node's value
// exceeds the target range upper bound. This early exit
// reduces comparisons when the range ends before the last node.
//
// Why Binary Search is not used on a linked list:
//   To reach the midpoint of a linked list we must walk n/2
//   pointer hops — O(n/2). This makes Binary Search O(n log n)
//   on a linked list, which is WORSE than Linear Search O(n).
//   Sentinel Search is the maximum optimisation achievable here.
//
// Proves H3: Sentinel < Linear for mid-list ranges.
//            Sentinel = Linear when range extends to the tail.
// ============================================================
void runSearchingExperiments(
    string filename,
    int&  linearBicycleComp,  long& linearBicycleTime,
    int&  linearBusComp,      long& linearBusTime,
    int&  linearAge1825Comp,  long& linearAge1825Time,
    int&  linearStudentComp,  long& linearStudentTime,
    int&  linearDist1012Comp, long& linearDist1012Time,
    int&  sentAge1825Comp,    long& sentAge1825Time,
    int&  sentAge617Comp,     long& sentAge617Time,
    int&  sentDist1012Comp,   long& sentDist1012Time,
    int&  sentDist59Comp,     long& sentDist59Time,
    int&  sentEmit2050Comp,   long& sentEmit2050Time)
{
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 7]  SEARCHING EXPERIMENTS                          " << endl;
    cout << "  Linear Search on unsorted data                              " << endl;
    cout << "  vs  Sentinel Search on pre-sorted data                      " << endl;
    cout << "  Proves H3: Sentinel stops early; Binary not viable on list. " << endl;
    cout << "===============================================================" << endl;

    clock_t startTime;
    clock_t endTime;

    // ----------------------------------------------------------
    // [7b / L1-L5] Linear Search — unsorted list
    // One fresh list is loaded and reused for all five searches
    // because linear search does not require any particular order.
    // ----------------------------------------------------------
    ResidentList searchList;
    searchList.loadFromCSV(filename);

    // [L1] Transport mode = Bicycle (exact match)
    cout << endl << "[L1] Linear Search - transport = Bicycle" << endl;
    startTime         = clock();
    searchList.linearSearch("transport", "Bicycle", 0, 0, linearBicycleComp);
    endTime           = clock();
    linearBicycleTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [L2] Transport contains "Bus" (matches School Bus and any other bus)
    cout << endl << "[L2] Linear Search - transport contains Bus" << endl;
    startTime     = clock();
    searchList.linearSearch("transport", "Bus", 0, 0, linearBusComp);
    endTime       = clock();
    linearBusTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [L3] Age range 18 to 25 (University Students group)
    cout << endl << "[L3] Linear Search - age range 18 to 25" << endl;
    startTime         = clock();
    searchList.linearSearch("age", "", 18, 25, linearAge1825Comp);
    endTime           = clock();
    linearAge1825Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [L4] Age group label contains "Student"
    cout << endl << "[L4] Linear Search - age group label contains Student" << endl;
    startTime         = clock();
    searchList.linearSearch("agegroup", "Student", 0, 0, linearStudentComp);
    endTime           = clock();
    linearStudentTime = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [L5] Daily distance in range 10 to 12 km
    cout << endl << "[L5] Linear Search - daily distance range 10 to 12 km" << endl;
    startTime          = clock();
    searchList.linearSearch("distance", "", 10, 12, linearDist1012Comp);
    endTime            = clock();
    linearDist1012Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // ----------------------------------------------------------
    // [7b / SS1-SS5] Sentinel Search — pre-sorted lists
    // Each group of sentinels requires a list sorted by that field.
    // We use Merge Sort (H1's best algorithm) to pre-sort.
    // ----------------------------------------------------------
    int dummy = 0;

    // --- Sort by Age, run SS1 and SS2 ---
    ResidentList sortedByAge;
    sortedByAge.loadFromCSV(filename);
    cout << endl << "  Pre-sorting by Age (Merge Sort) for sentinel search..." << endl;
    startTime = clock();
    sortedByAge.mergeSort("age", dummy);
    endTime   = clock();
    cout << "  Sort comparisons: " << dummy
         << " | Time: "
         << (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC
         << " us" << endl;

    // [SS1] Age 18 to 25 — range ends before last node, early exit expected
    cout << endl << "[SS1] Sentinel Search - age 18 to 25 (sorted by age)" << endl;
    startTime       = clock();
    sortedByAge.sentinelSearch("age", 18, 25, sentAge1825Comp);
    endTime         = clock();
    sentAge1825Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [SS2] Age 6 to 17 — narrow range, exits very early
    cout << endl << "[SS2] Sentinel Search - age 6 to 17 (sorted by age)" << endl;
    startTime      = clock();
    sortedByAge.sentinelSearch("age", 6, 17, sentAge617Comp);
    endTime        = clock();
    sentAge617Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // --- Sort by Distance, run SS3 and SS4 ---
    ResidentList sortedByDist;
    sortedByDist.loadFromCSV(filename);
    cout << endl << "  Pre-sorting by Distance (Merge Sort) for sentinel search..." << endl;
    startTime = clock();
    sortedByDist.mergeSort("distance", dummy);
    endTime   = clock();
    cout << "  Sort comparisons: " << dummy
         << " | Time: "
         << (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC
         << " us" << endl;

    // [SS3] Distance 10 to 12 km
    cout << endl << "[SS3] Sentinel Search - distance 10 to 12 km (sorted by distance)" << endl;
    startTime        = clock();
    sortedByDist.sentinelSearch("distance", 10, 12, sentDist1012Comp);
    endTime          = clock();
    sentDist1012Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // [SS4] Distance 5 to 9 km
    cout << endl << "[SS4] Sentinel Search - distance 5 to 9 km" << endl;
    startTime      = clock();
    sortedByDist.sentinelSearch("distance", 5, 9, sentDist59Comp);
    endTime        = clock();
    sentDist59Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;

    // --- Sort by Emission, run SS5 ---
    ResidentList sortedByEmit;
    sortedByEmit.loadFromCSV(filename);
    cout << endl << "  Pre-sorting by Emission (Merge Sort) for sentinel search..." << endl;
    startTime = clock();
    sortedByEmit.mergeSort("emission", dummy);
    endTime   = clock();
    cout << "  Sort comparisons: " << dummy
         << " | Time: "
         << (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC
         << " us" << endl;

    // [SS5] Monthly emission 20 to 50 kg CO2
    cout << endl << "[SS5] Sentinel Search - emission 20 to 50 kg CO2" << endl;
    startTime        = clock();
    sortedByEmit.sentinelSearch("emission", 20, 50, sentEmit2050Comp);
    endTime          = clock();
    sentEmit2050Time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
}

// ============================================================
// printSection8Summary  [Section 8]
//
// Assembles every sort and search result collected during
// Sections 6 and 7 into one clean performance comparison table.
// This is the primary evidence table for all four hypotheses.
//
// Also prints the H4 amortised cost note showing at what point
// it becomes cheaper to pre-sort once and use sentinel searches
// rather than running multiple linear searches.
// ============================================================
void printSection8Summary(
    int bubbleAgeComp,  long bubbleAgeTime,
    int mergeAgeComp,   long mergeAgeTime,
    int bubbleDistComp, long bubbleDistTime,
    int mergeDistComp,  long mergeDistTime,
    int bubbleEmitComp, long bubbleEmitTime,
    int mergeEmitComp,  long mergeEmitTime,
    int h2BubbleSortedComp, long h2BubbleSortedTime,
    int h2MergeSortedComp,  long h2MergeSortedTime,
    int h2BubbleRandomComp, long h2BubbleRandomTime,
    int h2MergeRandomComp,  long h2MergeRandomTime,
    int linearBicycleComp,  long linearBicycleTime,
    int linearBusComp,      long linearBusTime,
    int linearAge1825Comp,  long linearAge1825Time,
    int linearStudentComp,  long linearStudentTime,
    int linearDist1012Comp, long linearDist1012Time,
    int sentAge1825Comp,    long sentAge1825Time,
    int sentAge617Comp,     long sentAge617Time,
    int sentDist1012Comp,   long sentDist1012Time,
    int sentDist59Comp,     long sentDist59Time,
    int sentEmit2050Comp,   long sentEmit2050Time,
    int totalRecords)
{
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 8]  PERFORMANCE SUMMARY - Singly Linked List        " << endl;
    cout << "  Primary metric: comparison count (dataset fits in CPU cache; " << endl;
    cout << "  timing values may be 0 us. Comparisons confirm behaviour.)  " << endl;
    cout << "===============================================================" << endl;

    int tableWidth = 88;

    // ----------------------------------------------------------
    // [Section 6] Sorting results — H1 evidence
    // ----------------------------------------------------------
    printLine(tableWidth);
    cout << padStr("Operation",    36)
         << padStr("Algorithm",    14)
         << padStr("Complexity",   14)
         << padStr("Comparisons",  14)
         << padStr("Time (us)",    10) << endl;
    printLine(tableWidth);

    printSortRow("Sort Age     [Bubble]",
                 "Bubble Sort", "O(n^2)",
                 bubbleAgeComp,  bubbleAgeTime);
    printSortRow("Sort Age     [Merge]",
                 "Merge Sort",  "O(n logn)",
                 mergeAgeComp,   mergeAgeTime);
    printSortRow("Sort Dist    [Bubble]",
                 "Bubble Sort", "O(n^2)",
                 bubbleDistComp, bubbleDistTime);
    printSortRow("Sort Dist    [Merge]",
                 "Merge Sort",  "O(n logn)",
                 mergeDistComp,  mergeDistTime);
    printSortRow("Sort Emit    [Bubble]",
                 "Bubble Sort", "O(n^2)",
                 bubbleEmitComp, bubbleEmitTime);
    printSortRow("Sort Emit    [Merge]",
                 "Merge Sort",  "O(n logn)",
                 mergeEmitComp,  mergeEmitTime);

    printLine(tableWidth);

    // ----------------------------------------------------------
    // [Section 6 / H2] Data order sensitivity — H2 evidence
    // ----------------------------------------------------------
    printSortRow("H2 Sorted    [Bubble]",
                 "Bubble Sort", "O(n)*",
                 h2BubbleSortedComp, h2BubbleSortedTime);
    printSortRow("H2 Sorted    [Merge]",
                 "Merge Sort",  "O(n logn)",
                 h2MergeSortedComp,  h2MergeSortedTime);
    printSortRow("H2 Random    [Bubble]",
                 "Bubble Sort", "O(n^2)",
                 h2BubbleRandomComp, h2BubbleRandomTime);
    printSortRow("H2 Random    [Merge]",
                 "Merge Sort",  "O(n logn)",
                 h2MergeRandomComp,  h2MergeRandomTime);

    printLine(tableWidth);

    // ----------------------------------------------------------
    // [Section 7] Linear Search — H3 evidence baseline
    // ----------------------------------------------------------
    printSearchRow("Linear: Bicycle",
                   "Linear", "O(n)",
                   linearBicycleComp,  linearBicycleTime);
    printSearchRow("Linear: Bus",
                   "Linear", "O(n)",
                   linearBusComp,      linearBusTime);
    printSearchRow("Linear: age 18-25",
                   "Linear", "O(n)",
                   linearAge1825Comp,  linearAge1825Time);
    printSearchRow("Linear: Student",
                   "Linear", "O(n)",
                   linearStudentComp,  linearStudentTime);
    printSearchRow("Linear: dist 10-12",
                   "Linear", "O(n)",
                   linearDist1012Comp, linearDist1012Time);

    printLine(tableWidth);

    // ----------------------------------------------------------
    // [Section 7] Sentinel Search — H3 evidence early exit
    // ----------------------------------------------------------
    printSearchRow("Sentinel: age 18-25",
                   "Sentinel", "O(n)*",
                   sentAge1825Comp,    sentAge1825Time);
    printSearchRow("Sentinel: age 6-17",
                   "Sentinel", "O(n)*",
                   sentAge617Comp,     sentAge617Time);
    printSearchRow("Sentinel: dist 10-12",
                   "Sentinel", "O(n)*",
                   sentDist1012Comp,   sentDist1012Time);
    printSearchRow("Sentinel: dist 5-9",
                   "Sentinel", "O(n)*",
                   sentDist59Comp,     sentDist59Time);
    printSearchRow("Sentinel: emit 20-50",
                   "Sentinel", "O(n)*",
                   sentEmit2050Comp,   sentEmit2050Time);

    printLine(tableWidth);
    cout << "  * Bubble O(n)*   on sorted data : one clean pass, no swaps -> early exit" << endl;
    cout << "  * Sentinel O(n)* on sorted list : stops once value > upper bound" << endl;
    cout << "  Total records: " << totalRecords << endl;

    // ----------------------------------------------------------
    // [Section 8 / H4] Amortised pre-sort cost analysis
    // ----------------------------------------------------------
    int breakEven = (totalRecords > 0) ? (mergeAgeComp / totalRecords) : 0;

    cout << endl;
    cout << "  [Section 8 / H4 - Amortised Sort Cost]" << endl;
    printLine(tableWidth);
    cout << "  Pre-sort cost (Merge Sort, one time)  : ~"
         << mergeAgeComp << " comparisons" << endl;
    cout << "  Linear Search cost per query          : ~"
         << totalRecords << " comparisons (visits all nodes)" << endl;
    cout << "  Break-even point                      : ~"
         << breakEven << " repeated searches on the same field" << endl;
    cout << "  Below break-even : multiple Linear Searches cheaper (no sort needed)" << endl;
    cout << "  Above break-even : sort once + Sentinel is the cheaper strategy" << endl;
    printLine(tableWidth);

    // ----------------------------------------------------------
    // [Section 8] Array vs Linked List trade-off summary
    // ----------------------------------------------------------
    cout << endl;
    cout << "  [Section 8 - Array vs Linked List Trade-Off Summary]" << endl;
    printLine(tableWidth);
    cout << "  Metric             Array                 Linked List (this program)" << endl;
    printLine(tableWidth);
    cout << "  Sort extra mem     O(n) temp buffer       O(log n) stack only" << endl;
    cout << "  Best sort          Quick Sort O(n logn)   Merge Sort O(n logn)" << endl;
    cout << "  Best search        Binary O(log n)        Sentinel O(n) early stop" << endl;
    cout << "  Random access      O(1)  array[i]         O(n)  walk n hops" << endl;
    cout << "  Insert at tail     O(1) amortised          O(1) with tail pointer" << endl;
    cout << "  Cache locality     High (contiguous)       Lower (heap pointers)" << endl;
    printLine(tableWidth);
}

// ============================================================
// MAIN FUNCTION
//
// Execution order:
//   PART 1 (Array):
//     runDisplay(manager)  — calls all teammate's methods
//
//   PART 2 (Linked List):
//     1. Load data from CSV
//     2. Preview first 10 records
//     3. sectionFourAgeGroups()       [4a / 4b]
//     4. sectionFiveEmissions()       [5a / 5b / 5c / 5d]
//     5. sectionNineInsights()        [9a / 9b / 9c]
//     6. runSortingExperiments()      [Section 6]
//     7. runH2OrderSensitivityTest()  [Section 6 / H2]
//     8. runSearchingExperiments()    [Section 7]
//     9. printSection8Summary()       [Section 8]
// ============================================================

int main()
{
    // Ask for the CSV path once — both programs use the same file
    string filename = "dataset2-cityB.csv";
    cout << "========================================" << endl;
    cout << "  City B - Carbon Emission Analysis" << endl;
    cout << "  Data Structures: Array + Linked List" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Loading dataset: " << filename << endl;

    // ----------------------------------------------------------
    // PART 1 — Array Implementation (teammate)
    // ----------------------------------------------------------
    cout << endl;
    cout << "========================================" << endl;
    cout << "  PART 1 - ARRAY IMPLEMENTATION" << endl;
    cout << "========================================" << endl;

    ResidentArrayManager manager;
    manager.loadCSV(filename);
    runDisplay(manager);

    // ----------------------------------------------------------
    // PART 2 — Singly Linked List Implementation (your code)
    // ----------------------------------------------------------
    cout << endl;
    cout << "========================================" << endl;
    cout << "  PART 2 - SINGLY LINKED LIST" << endl;
    cout << "========================================" << endl;

    // Step 1: Load the CSV into the linked list
    ResidentList list;
    list.loadFromCSV(filename);

    // Step 2: Preview — first 10 records as a loading sanity check
    cout << endl << "--- Preview: First 10 Records After Loading ---" << endl;
    list.displayList(10);

    // Step 3: [Section 4a / 4b]  Age group categorisation
    // Must run before Sections 5 and 9 — populates the internal
    // cross-section data store used by both of those functions.
    list.sectionFourAgeGroups();

    // Step 4: [Section 5a / 5b / 5c / 5d]  Emission analysis
    list.sectionFiveEmissions();

    // Step 5: [Section 6]  Sorting experiments
    // Results stored in local variables for the Section 8 table.
    int  bubbleAgeComp  = 0; long bubbleAgeTime  = 0;
    int  mergeAgeComp   = 0; long mergeAgeTime   = 0;
    int  bubbleDistComp = 0; long bubbleDistTime = 0;
    int  mergeDistComp  = 0; long mergeDistTime  = 0;
    int  bubbleEmitComp = 0; long bubbleEmitTime = 0;
    int  mergeEmitComp  = 0; long mergeEmitTime  = 0;

    runSortingExperiments(filename,
        bubbleAgeComp,  bubbleAgeTime,
        mergeAgeComp,   mergeAgeTime,
        bubbleDistComp, bubbleDistTime,
        mergeDistComp,  mergeDistTime,
        bubbleEmitComp, bubbleEmitTime,
        mergeEmitComp,  mergeEmitTime);

    // Step 6: [Section 6 / H2]  Data order sensitivity test
    // Build a pre-sorted list here and hand it to the test
    // function so it can run Bubble Sort on already-sorted data.
    ResidentList listSortedForH2;
    int          dummy = 0;
    listSortedForH2.loadFromCSV(filename);
    listSortedForH2.mergeSort("age", dummy);

    int  h2BubbleSortedComp = 0; long h2BubbleSortedTime = 0;
    int  h2MergeSortedComp  = 0; long h2MergeSortedTime  = 0;
    int  h2BubbleRandomComp = 0; long h2BubbleRandomTime = 0;
    int  h2MergeRandomComp  = 0; long h2MergeRandomTime  = 0;

    runH2OrderSensitivityTest(filename, listSortedForH2,
        h2BubbleSortedComp, h2BubbleSortedTime,
        h2MergeSortedComp,  h2MergeSortedTime,
        h2BubbleRandomComp, h2BubbleRandomTime,
        h2MergeRandomComp,  h2MergeRandomTime);

    // Step 7: [Section 7]  Searching experiments
    // Results stored in local variables for the Section 8 table.
    int  linearBicycleComp  = 0; long linearBicycleTime  = 0;
    int  linearBusComp      = 0; long linearBusTime      = 0;
    int  linearAge1825Comp  = 0; long linearAge1825Time  = 0;
    int  linearStudentComp  = 0; long linearStudentTime  = 0;
    int  linearDist1012Comp = 0; long linearDist1012Time = 0;
    int  sentAge1825Comp    = 0; long sentAge1825Time    = 0;
    int  sentAge617Comp     = 0; long sentAge617Time     = 0;
    int  sentDist1012Comp   = 0; long sentDist1012Time   = 0;
    int  sentDist59Comp     = 0; long sentDist59Time     = 0;
    int  sentEmit2050Comp   = 0; long sentEmit2050Time   = 0;

    runSearchingExperiments(filename,
        linearBicycleComp,  linearBicycleTime,
        linearBusComp,      linearBusTime,
        linearAge1825Comp,  linearAge1825Time,
        linearStudentComp,  linearStudentTime,
        linearDist1012Comp, linearDist1012Time,
        sentAge1825Comp,    sentAge1825Time,
        sentAge617Comp,     sentAge617Time,
        sentDist1012Comp,   sentDist1012Time,
        sentDist59Comp,     sentDist59Time,
        sentEmit2050Comp,   sentEmit2050Time);

    // Step 8: [Section 8]  Full performance summary table
    // Feeds all collected results in — nothing is recalculated.
    printSection8Summary(
        bubbleAgeComp,  bubbleAgeTime,
        mergeAgeComp,   mergeAgeTime,
        bubbleDistComp, bubbleDistTime,
        mergeDistComp,  mergeDistTime,
        bubbleEmitComp, bubbleEmitTime,
        mergeEmitComp,  mergeEmitTime,
        h2BubbleSortedComp, h2BubbleSortedTime,
        h2MergeSortedComp,  h2MergeSortedTime,
        h2BubbleRandomComp, h2BubbleRandomTime,
        h2MergeRandomComp,  h2MergeRandomTime,
        linearBicycleComp,  linearBicycleTime,
        linearBusComp,      linearBusTime,
        linearAge1825Comp,  linearAge1825Time,
        linearStudentComp,  linearStudentTime,
        linearDist1012Comp, linearDist1012Time,
        sentAge1825Comp,    sentAge1825Time,
        sentAge617Comp,     sentAge617Time,
        sentDist1012Comp,   sentDist1012Time,
        sentDist59Comp,     sentDist59Time,
        sentEmit2050Comp,   sentEmit2050Time,
        list.getSize());

    // Step 9: [Section 9a / 9b / 9c]  Insights and recommendations
    list.sectionNineInsights();

    return 0;
}
