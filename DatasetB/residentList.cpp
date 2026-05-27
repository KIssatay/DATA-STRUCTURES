
// Purpose of this file contains the implementation of the core data
//              structure operations for the ResidentList class.
//              This includes all helper/utility functions,
//              constructor/destructor, insert, load, sorting,
//              and searching.
//
//              Display and output functions have been separated
//              into displayList.cpp to keep this file focused
//              on data structure logic only.
//
//              Compile all three files together:
//                g++ main.cpp residentList.cpp displayList.cpp -o program


#include "resident.hpp"


// SECTION 1: HELPER FUNCTION IMPLEMENTATIONS
// Small utility functions used throughout the program to
// format output neatly in text-based tables.


// padStr: pads any text with spaces on the right until it
// reaches the desired column width. Used in every table row.
string padStr(string text, int width)
{
    while ((int)text.length() < width)
    {
        text = text + " ";
    }
    return text;
}

// intStr: converts an integer into a string so we can
// pass it into padStr (padStr only accepts strings)
string intStr(int number)
{
    // handle the special case of zero
    if (number == 0)
    {
        return "0";
    }

    string result   = "";
    bool isNegative = false;

    // handle negative numbers
    if (number < 0)
    {
        isNegative = true;
        number     = -number;
    }

    // build the string digit by digit from right to left
    while (number > 0)
    {
        char digit = '0' + (number % 10);
        result     = digit + result;
        number     = number / 10;
    }

    if (isNegative)
    {
        result = "-" + result;
    }

    return result;
}

// dblStr: converts a double into a string with exactly
// 2 decimal places so CO2 columns line up cleanly
string dblStr(double number)
{
    // round to 2 decimal places
    int wholePart   = (int)number;
    int decimalPart = (int)((number - wholePart) * 100 + 0.5);

    string result = intStr(wholePart) + ".";

    // always show 2 digits after the decimal point
    if (decimalPart < 10)
    {
        result = result + "0";
    }
    result = result + intStr(decimalPart);

    return result;
}

// printLine: prints a row of dashes as a table separator
// Used above and below every table header
void printLine(int length)
{
    for (int i = 0; i < length; i++)
    {
        cout << "-";
    }
    cout << endl;
}

// ageGroupLabel: takes an age and returns the group name as text
// Used in display, search results, and emission analysis
// Without this we would repeat the same if-else chain everywhere
string ageGroupLabel(int age)
{
    if      (age >= 6  && age <= 17)  return "Children & Teenagers";
    else if (age >= 18 && age <= 25)  return "University Students";
    else if (age >= 26 && age <= 45)  return "Working Adults (Early)";
    else if (age >= 46 && age <= 60)  return "Working Adults (Late)";
    else if (age >= 61 && age <= 100) return "Senior Citizens";
    else                              return "Unknown";
}

// toLower: converts a string to all lowercase letters
// Used so search comparisons are not case-sensitive
string toLower(string text)
{
    string result = "";
    for (int i = 0; i < (int)text.length(); i++)
    {
        char c = text[i];
        // if the character is uppercase, shift it to lowercase
        if (c >= 'A' && c <= 'Z')
        {
            c = c + 32;
        }
        result = result + c;
    }
    return result;
}

// strContains: checks if query appears inside text
// ignoring capital letters (case-insensitive)
// Example: searching "bus" also matches "School Bus"
bool strContains(string text, string query)
{
    string lowerText  = toLower(text);
    string lowerQuery = toLower(query);

    // slide through every possible starting position in text
    for (int i = 0; i <= (int)lowerText.length() - (int)lowerQuery.length(); i++)
    {
        bool match = true;
        // check each character of query at position i
        for (int j = 0; j < (int)lowerQuery.length(); j++)
        {
            if (lowerText[i + j] != lowerQuery[j])
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            return true;
        }
    }
    return false;
}

// ============================================================
// SECTION 2: RESIDENTLIST PRIVATE HELPER IMPLEMENTATIONS
// Used only internally by the class to power Merge Sort
// ============================================================

// splitList: cuts the linked list into two equal halves
// slow moves one step, fast moves two steps at a time.
// When fast falls off the end, slow is at the midpoint.
void ResidentList::splitList(Resident* source,
                             Resident*& frontHalf,
                             Resident*& backHalf)
{
    Resident* slow = source;
    Resident* fast = source->nextAddress;

    // advance fast by 2 and slow by 1 each step
    while (fast != nullptr)
    {
        fast = fast->nextAddress;
        if (fast != nullptr)
        {
            slow = slow->nextAddress;
            fast = fast->nextAddress;
        }
    }

    // slow is now at the midpoint — cut the list here
    frontHalf         = source;
    backHalf          = slow->nextAddress;
    slow->nextAddress = nullptr;  // this cut breaks the chain
}

// mergeTwoLists: combines two already-sorted halves into one
// sorted list by re-linking nextAddress pointers.
// No new nodes are created — only pointer assignments.
// This is why Merge Sort suits linked lists: arrays need a
// separate O(n) buffer to hold the merged result. Linked lists
// just re-wire pointers and need only O(log n) stack space.
Resident* ResidentList::mergeTwoLists(Resident* a, Resident* b,
                                      string sortField, int& compCount)
{
    // if one half is empty, the other is the result
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    Resident* result = nullptr;
    bool aIsSmaller  = false;

    compCount++;  // every call to this function is one comparison

    // pick the smaller front node depending on the sort field
    if (sortField == "age")
    {
        aIsSmaller = (a->age <= b->age);
    }
    else if (sortField == "distance")
    {
        aIsSmaller = (a->dailyDistance <= b->dailyDistance);
    }
    else if (sortField == "emission")
    {
        aIsSmaller = (a->monthlyEmission <= b->monthlyEmission);
    }

    if (aIsSmaller)
    {
        result              = a;
        result->nextAddress = mergeTwoLists(a->nextAddress, b,
                                            sortField, compCount);
    }
    else
    {
        result              = b;
        result->nextAddress = mergeTwoLists(a, b->nextAddress,
                                            sortField, compCount);
    }

    return result;
}

// mergeSortHelper: the recursive engine of Merge Sort
// step 1 - base case: 0 or 1 node is already sorted
// step 2 - call splitList to divide into two halves
// step 3 - call itself recursively on each half
// step 4 - call mergeTwoLists to combine the sorted halves
void ResidentList::mergeSortHelper(Resident*& listHead,
                                   string sortField, int& compCount)
{
    // base case: a list of 0 or 1 node is already sorted
    if (listHead == nullptr || listHead->nextAddress == nullptr)
    {
        return;
    }

    Resident* frontHalf = nullptr;
    Resident* backHalf  = nullptr;

    // divide the list at the midpoint
    splitList(listHead, frontHalf, backHalf);

    // conquer: recursively sort each half
    mergeSortHelper(frontHalf, sortField, compCount);
    mergeSortHelper(backHalf,  sortField, compCount);

    // combine: merge the two now-sorted halves
    listHead = mergeTwoLists(frontHalf, backHalf, sortField, compCount);
}

// ============================================================
// SECTION 3: RESIDENTLIST PUBLIC FUNCTION IMPLEMENTATIONS
// ============================================================

// constructor: start with an empty list
// Also zeroes out the cross-section data store used by
// sectionFourAgeGroups, sectionFiveEmissions, and sectionNineInsights.
// sectionFourRan is set false so the guard in Sections 5 and 9
// can detect if sectionFourAgeGroups was skipped by accident.
ResidentList::ResidentList()
{
    head             = nullptr;
    tail             = nullptr;
    size             = 0;
    datasetTotalEmit = 0.0;
    sectionFourRan   = false;

    for (int g = 0; g < 5; g++)
    {
        savedCount[g]    = 0;
        savedTotal[g]    = 0.0;
        savedAvg[g]      = 0.0;
        savedTopMode[g]  = "";
        savedTopCount[g] = 0;
        for (int m = 0; m < 4; m++)
        {
            savedModeTotal[g][m]  = 0.0;
            savedModeCounts[g][m] = 0;
        }
    }
}

// destructor: free all nodes from memory when done
ResidentList::~ResidentList()
{
    Resident* temp = head;
    while (temp != nullptr)
    {
        Resident* next = temp->nextAddress;
        delete temp;
        temp = next;
    }
}

// getHead: returns the head pointer
Resident* ResidentList::getHead()
{
    return head;
}

// getSize: returns how many residents are in the list
int ResidentList::getSize()
{
    return size;
}

// ----------------------------------------------------------
// createNewResident: allocates memory for one new node and
// fills in all fields. Also calculates monthlyEmission here
// so we never have to recalculate it elsewhere.
// Sets nextAddress = nullptr because this node is not yet
// connected to anything.
// ----------------------------------------------------------
Resident* ResidentList::createNewResident(string id, int age,
                                          string transport,
                                          double distance,
                                          double carbonFactor,
                                          int avgDays)
{
    Resident* newResident = new Resident;

    newResident->residentID           = id;
    newResident->age                  = age;
    newResident->modeOfTransport      = transport;
    newResident->dailyDistance        = distance;
    newResident->carbonEmissionFactor = carbonFactor;
    newResident->avgDaysPerMonth      = avgDays;

    // calculate monthly emission once here so we never repeat the formula
    newResident->monthlyEmission = distance * carbonFactor * avgDays;

    newResident->nextAddress = nullptr;  // not linked yet

    return newResident;
}

// ----------------------------------------------------------
// insertToEnd: creates a new node and attaches it at the end
// of the chain. This preserves the original CSV row order.
// If the list is empty the new node becomes both head and tail.
// ----------------------------------------------------------
void ResidentList::insertToEnd(string id, int age, string transport,
                               double distance, double carbonFactor,
                               int avgDays)
{
    Resident* newResident = createNewResident(id, age, transport,
                                              distance, carbonFactor,
                                              avgDays);
    if (head == nullptr)
    {
        // list is empty — new node is both the head and the tail
        head = newResident;
        tail = newResident;
    }
    else
    {
        // link after the current tail and move tail forward
        tail->nextAddress = newResident;
        tail              = newResident;
    }

    size++;
}

// ----------------------------------------------------------
// loadFromCSV: opens the file, skips the header row, reads
// one row at a time using getline with comma as separator,
// then calls insertToEnd for each valid row.
// The \r removal handles Windows-style CRLF line endings that
// would otherwise corrupt the last field of each row.
// ----------------------------------------------------------
void ResidentList::loadFromCSV(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open file: " << filename << endl;
        return;
    }

    string headerLine = "";
    getline(file, headerLine);  // skip the column header row

    string id        = "";
    string ageStr    = "";
    string transport = "";
    string distStr   = "";
    string carbonStr = "";
    string daysStr   = "";

    while (file.good())
    {
        getline(file, id, ',');

        if (id == "" || id == "\r")
        {
            break;  // reached end of file or empty line
        }

        // remove Windows carriage return \r if present
        if (!id.empty() && id[id.length() - 1] == '\r')
        {
            id = id.substr(0, id.length() - 1);
        }

        getline(file, ageStr,    ',');
        getline(file, transport, ',');
        getline(file, distStr,   ',');
        getline(file, carbonStr, ',');
        getline(file, daysStr);

        // remove \r from the last field (Windows line endings)
        if (!daysStr.empty() && daysStr[daysStr.length() - 1] == '\r')
        {
            daysStr = daysStr.substr(0, daysStr.length() - 1);
        }

        // convert string fields to their correct numeric types
        int    age      = stoi(ageStr);
        double distance = stod(distStr);
        double carbon   = stod(carbonStr);
        int    days     = stoi(daysStr);

        insertToEnd(id, age, transport, distance, carbon, days);
    }

    file.close();
    cout << "Loaded successfully. Total records: " << size << endl;
}

// ============================================================
// SECTION 4: SORTING FUNCTION IMPLEMENTATIONS
// ============================================================

// ----------------------------------------------------------
// bubbleSort: sorts the list by swapping data in adjacent nodes
//
// Algorithm steps:
//   1. Start at head, compare the first two adjacent nodes
//   2. If they are in the wrong order, swap ALL their data fields
//   3. Move to the next pair and repeat until the end (one pass)
//   4. If any swap happened during the pass, do another pass
//   5. If a full pass had zero swaps the list is sorted — stop
//
// Why we swap data and not pointers:
//   Swapping pointers in a singly linked list requires tracking
//   the previous node which is significantly more complex code.
//   Swapping data fields gives the identical sorted result with
//   much simpler logic.
//
// Time complexity:
//   O(n^2) worst/average case (random or reverse sorted data)
//   O(n) best case (already sorted — one clean pass, no swaps)
// ----------------------------------------------------------
void ResidentList::bubbleSort(string sortField, int& compCount)
{
    compCount = 0;

    if (head == nullptr || head->nextAddress == nullptr)
    {
        return;  // list with 0 or 1 node is already sorted
    }

    bool swapHappened = true;

    // keep doing passes until a full pass has no swaps
    while (swapHappened)
    {
        swapHappened      = false;
        Resident* current = head;

        while (current->nextAddress != nullptr)
        {
            Resident* nextNode = current->nextAddress;
            bool shouldSwap    = false;

            compCount++;  // count every comparison regardless of outcome

            // decide if these two adjacent nodes are in the wrong order
            if (sortField == "age")
            {
                shouldSwap = (current->age > nextNode->age);
            }
            else if (sortField == "distance")
            {
                shouldSwap = (current->dailyDistance > nextNode->dailyDistance);
            }
            else if (sortField == "emission")
            {
                shouldSwap = (current->monthlyEmission > nextNode->monthlyEmission);
            }

            if (shouldSwap)
            {
                // swap all data fields between current and nextNode
                string tempID     = current->residentID;
                int    tempAge    = current->age;
                string tempTrans  = current->modeOfTransport;
                double tempDist   = current->dailyDistance;
                double tempCarbon = current->carbonEmissionFactor;
                int    tempDays   = current->avgDaysPerMonth;
                double tempEmit   = current->monthlyEmission;

                current->residentID           = nextNode->residentID;
                current->age                  = nextNode->age;
                current->modeOfTransport      = nextNode->modeOfTransport;
                current->dailyDistance        = nextNode->dailyDistance;
                current->carbonEmissionFactor = nextNode->carbonEmissionFactor;
                current->avgDaysPerMonth      = nextNode->avgDaysPerMonth;
                current->monthlyEmission      = nextNode->monthlyEmission;

                nextNode->residentID           = tempID;
                nextNode->age                  = tempAge;
                nextNode->modeOfTransport      = tempTrans;
                nextNode->dailyDistance        = tempDist;
                nextNode->carbonEmissionFactor = tempCarbon;
                nextNode->avgDaysPerMonth      = tempDays;
                nextNode->monthlyEmission      = tempEmit;

                swapHappened = true;
            }

            current = current->nextAddress;
        }
    }

    // after sorting, update the tail pointer by walking to the last node
    Resident* temp = head;
    while (temp->nextAddress != nullptr)
    {
        temp = temp->nextAddress;
    }
    tail = temp;
}

// ----------------------------------------------------------
// mergeSort: sorts the list using divide and conquer
//
// Algorithm steps:
//   1. mergeSortHelper checks if list has 0 or 1 node (base case)
//   2. splitList cuts the list at the midpoint using slow/fast pointers
//   3. calls itself on the front half recursively
//   4. calls itself on the back half recursively
//   5. mergeTwoLists combines the two sorted halves by re-linking pointers
//
// Why comparison count is always the same value:
//   The splitting and merging structure depends only on n (list size).
//   It never depends on the values inside the nodes. Whether the data
//   is random, sorted, or reverse sorted, the same number of
//   merge comparisons are made every time.
//
// Memory advantage over array Merge Sort:
//   Array Merge Sort needs O(n) extra buffer space to hold the merged
//   result before copying back. Linked list Merge Sort only re-wires
//   nextAddress pointers — the extra space needed is only O(log n)
//   for the recursion call stack.
//
// Time complexity:
//   O(n log n) always — best, worst, and average case are identical
// ----------------------------------------------------------
void ResidentList::mergeSort(string sortField, int& compCount)
{
    compCount = 0;
    mergeSortHelper(head, sortField, compCount);

    // after sorting, walk to the last node and update the tail pointer
    if (head != nullptr)
    {
        Resident* temp = head;
        while (temp->nextAddress != nullptr)
        {
            temp = temp->nextAddress;
        }
        tail = temp;
    }
}


// SECTION 5: SEARCHING FUNCTION IMPLEMENTATIONS
// =================================================================

// ----------------------------------------------------------
// linearSearch: walks every single node from head to tail
//
// Why comparisons always equal total records:
//   It visits every node regardless of whether it has found
//   a match. Even if the first node matches, it still checks
//   all remaining nodes. This is the defining characteristic
//   of linear search — no early exit based on order.
//
// Supports four search modes:
//   "transport"  — string match (case-insensitive, partial)
//   "age"        — numeric range [rangeMin, rangeMax]
//   "distance"   — numeric range [rangeMin, rangeMax]
//   "agegroup"   — matches the age group label string
//
// Time complexity: O(n) always
// ----------------------------------------------------------
void ResidentList::linearSearch(string searchField, string query,
                                double rangeMin, double rangeMax,
                                int& compCount)
{
    compCount      = 0;
    int matchCount = 0;

    int lineWidth = 86;
    cout << endl;
    cout << "  Linear Search | Field: [" << searchField << "] Query: [";
    if (query != "")
        cout << query;
    else
        cout << dblStr(rangeMin) << "-" << dblStr(rangeMax);
    cout << "]" << endl;

    printLine(lineWidth);
    cout << padStr("ID",        8)
         << padStr("Age",       6)
         << padStr("Transport", 14)
         << padStr("Dist(km)",  10)
         << padStr("CO2/Month", 12)
         << padStr("Age Group", 24) << endl;
    printLine(lineWidth);

    Resident* temp = head;

    while (temp != nullptr)
    {
        compCount++;       // count every node we visit
        bool matched = false;

        // check the correct field based on what we are searching
        if (searchField == "transport")
        {
            matched = strContains(temp->modeOfTransport, query);
        }
        else if (searchField == "age")
        {
            matched = (temp->age >= (int)rangeMin &&
                       temp->age <= (int)rangeMax);
        }
        else if (searchField == "distance")
        {
            matched = (temp->dailyDistance >= rangeMin &&
                       temp->dailyDistance <= rangeMax);
        }
        else if (searchField == "agegroup")
        {
            matched = strContains(ageGroupLabel(temp->age), query);
        }

        if (matched)
        {
            cout << padStr(temp->residentID,              8)
                 << padStr(intStr(temp->age),              6)
                 << padStr(temp->modeOfTransport,         14)
                 << padStr(dblStr(temp->dailyDistance),   10)
                 << padStr(dblStr(temp->monthlyEmission), 12)
                 << padStr(ageGroupLabel(temp->age),      24) << endl;
            matchCount++;
        }

        temp = temp->nextAddress;  // always move to next node
    }

    printLine(lineWidth);
    cout << "  Matches: " << matchCount
         << " | Comparisons: " << compCount
         << " (visited every node - no early exit)" << endl;
}

// ----------------------------------------------------------
// sentinelSearch: searches a SORTED list and stops early
//
// IMPORTANT: the list must already be sorted by the same field
//            before calling this function. If the list is not
//            sorted, results will be incomplete and incorrect.
//
// How it works:
//   Walk from head. At each node check the value of sortField.
//   - If value > rangeMax: STOP immediately (sentinel condition).
//     Because the list is sorted, every remaining node is also
//     above rangeMax, so there is no point continuing.
//   - If value >= rangeMin: this node is in range — print it.
//   - Otherwise: below rangeMin — skip and continue.
//
// Why not Binary Search on a linked list:
//   Binary Search needs to jump to the middle using an index.
//   Arrays do this in O(1) — array[89] is immediate.
//   A linked list has no index — to reach node 89 we must
//   walk 89 pointer hops, which costs O(n/2).
//   This makes Binary Search O(n log n) on a linked list —
//   WORSE than just scanning linearly at O(n).
//   Sentinel Search is therefore the maximum optimisation
//   achievable for searching a singly linked list.
//
// When Sentinel helps:
//   Only when the target range ends BEFORE the last node in
//   the sorted list. If the range includes the tail (e.g.
//   age 18-25 when the oldest resident is 25), Sentinel gives
//   no benefit and behaves identically to Linear Search.
//
// Time complexity: O(n) worst case, O(k) best case where k is
//   the position of the first node that exceeds rangeMax.
// ----------------------------------------------------------
void ResidentList::sentinelSearch(string sortField,
                                  double rangeMin, double rangeMax,
                                  int& compCount)
{
    compCount      = 0;
    int matchCount = 0;

    int lineWidth = 86;
    cout << endl;
    cout << "  Sentinel Search | Field: [" << sortField << "]"
         << " | Range: " << dblStr(rangeMin)
         << " to " << dblStr(rangeMax) << endl;
    cout << "  (List must be sorted by this field for sentinel to work)"
         << endl;

    printLine(lineWidth);
    cout << padStr("ID",        8)
         << padStr("Age",       6)
         << padStr("Transport", 14)
         << padStr("Dist(km)",  10)
         << padStr("CO2/Month", 12)
         << padStr("Age Group", 24) << endl;
    printLine(lineWidth);

    Resident* temp = head;

    while (temp != nullptr)
    {
        compCount++;
        double currentValue = 0;

        // read the value of the field we are searching on
        if (sortField == "age")
        {
            currentValue = temp->age;
        }
        else if (sortField == "distance")
        {
            currentValue = temp->dailyDistance;
        }
        else if (sortField == "emission")
        {
            currentValue = temp->monthlyEmission;
        }

        // sentinel condition: we have passed the upper bound
        // everything after this in a sorted list is also above it
        if (currentValue > rangeMax)
        {
            break;  // early exit — this is what makes sentinel faster
        }

        // within range — print this resident
        if (currentValue >= rangeMin)
        {
            cout << padStr(temp->residentID,              8)
                 << padStr(intStr(temp->age),              6)
                 << padStr(temp->modeOfTransport,         14)
                 << padStr(dblStr(temp->dailyDistance),   10)
                 << padStr(dblStr(temp->monthlyEmission), 12)
                 << padStr(ageGroupLabel(temp->age),      24) << endl;
            matchCount++;
        }

        temp = temp->nextAddress;
    }

    printLine(lineWidth);
    cout << "  Matches: " << matchCount
         << " | Comparisons: " << compCount
         << " (stopped early once past upper bound)" << endl;
}
// ============================================================
// DISPLAY FUNCTIONS MOVED FROM displayList.cpp
// ============================================================

// displayList
//
// Walks from head to tail and prints every node as one formatted
// table row. Accepts a limit so callers can show a preview or
// the full list.
//
// Why we cannot jump to row N directly:
//   A linked list has no index. To reach any node we must follow
//   nextAddress pointers from head one hop at a time. This is why
//   every walk costs O(n) time regardless of where we stop.
//
// Parameters:
//   limit = 0  -> print all records
//   limit > 0  -> print only the first N records then stop
void ResidentList::displayList(int limit)
{
    int lineWidth = 86;
    cout << endl;
    printLine(lineWidth);
    cout << padStr("ID",        8)
         << padStr("Age",       6)
         << padStr("Transport", 14)
         << padStr("Dist(km)",  10)
         << padStr("CO2/Month", 12)
         << padStr("Age Group", 24) << endl;
    printLine(lineWidth);

    Resident* temp = head;  // always start from the head node
    int       count = 0;

    while (temp != nullptr)
    {
        cout << padStr(temp->residentID,              8)
             << padStr(intStr(temp->age),              6)
             << padStr(temp->modeOfTransport,         14)
             << padStr(dblStr(temp->dailyDistance),   10)
             << padStr(dblStr(temp->monthlyEmission), 12)
             << padStr(ageGroupLabel(temp->age),      24) << endl;

        count++;

        // stop early if a limit was given and we have reached it
        if (limit > 0 && count >= limit)
        {
            cout << "  Showing " << limit
                 << " of " << size << " records." << endl;
            break;
        }

        temp = temp->nextAddress;  // follow the chain to the next node
    }

    printLine(lineWidth);
}

// sectionFourAgeGroups  [Section 4a / 4b]
void ResidentList::sectionFourAgeGroups()
{
    // ----------------------------------------------------------
    // [4a] The 5 official age groups from the assignment spec
    // ----------------------------------------------------------
    int    groupMin[5]  = {  6,  18,  26,  46,  61 };
    int    groupMax[5]  = { 17,  25,  45,  60, 100 };
    string groupName[5] = {
        "6-17   Children & Teenagers",
        "18-25  University Students / Young Adults",
        "26-45  Working Adults (Early Career)",
        "46-60  Working Adults (Late Career)",
        "61-100 Senior Citizens / Retirees"
    };

    // short labels used in the Section 5c cross-group table
    string groupShort[5] = { "6-17", "18-25", "26-45", "46-60", "61-100" };

    // City B transport modes (University Town)
    string modes[4] = { "Bicycle", "Walking", "School Bus", "Carpool" };

    // reset the cross-section data store before filling it
    datasetTotalEmit = 0.0;
    for (int g = 0; g < 5; g++)
    {
        savedCount[g]    = 0;
        savedTotal[g]    = 0.0;
        savedAvg[g]      = 0.0;
        savedTopMode[g]  = "";
        savedTopCount[g] = 0;
        for (int m = 0; m < 4; m++)
        {
            savedModeTotal[g][m]  = 0.0;
            savedModeCounts[g][m] = 0;
        }
    }

    // print the section header
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 4a / 4b]                                            " << endl;
    cout << "        AGE GROUP CATEGORISATION - City B                     " << endl;
    cout << "        University Town | " << size << " Residents            " << endl;
    cout << "===============================================================" << endl;

    // ----------------------------------------------------------
    // [4b] Process each age group one at a time
    // ----------------------------------------------------------
    for (int g = 0; g < 5; g++)
    {
        int    groupCount     = 0;
        double groupTotalEmit = 0.0;
        int    modeCounts[4]  = { 0, 0, 0, 0 };
        double modeTotals[4]  = { 0.0, 0.0, 0.0, 0.0 };

        // walk the full list; collect every resident in this age group
        Resident* temp = head;
        while (temp != nullptr)
        {
            if (temp->age >= groupMin[g] && temp->age <= groupMax[g])
            {
                groupCount++;
                groupTotalEmit += temp->monthlyEmission;

                // tally per-mode counts and emission totals
                for (int m = 0; m < 4; m++)
                {
                    if (temp->modeOfTransport == modes[m])
                    {
                        modeCounts[m]++;
                        modeTotals[m] += temp->monthlyEmission;
                    }
                }
            }
            temp = temp->nextAddress;
        }

        // accumulate into the dataset grand total [5a]
        datasetTotalEmit += groupTotalEmit;

        // find the most preferred mode (highest resident count) [4b]
        int preferredIndex = 0;
        for (int m = 1; m < 4; m++)
        {
            if (modeCounts[m] > modeCounts[preferredIndex])
            {
                preferredIndex = m;
            }
        }

        // average CO2 per resident in this group [4b]
        double avgEmit = (groupCount > 0) ? (groupTotalEmit / groupCount) : 0.0;

        // save everything for Section 5 and Section 9
        savedCount[g]    = groupCount;
        savedTotal[g]    = groupTotalEmit;
        savedAvg[g]      = avgEmit;
        savedTopMode[g]  = (groupCount > 0) ? modes[preferredIndex] : "N/A";
        savedTopCount[g] = (groupCount > 0) ? modeCounts[preferredIndex] : 0;
        for (int m = 0; m < 4; m++)
        {
            savedModeTotal[g][m]  = modeTotals[m];
            savedModeCounts[g][m] = modeCounts[m];
        }

        // ----------------------------------------------------------
        // Print the per-group table — assignment sample format
        // ----------------------------------------------------------
        int colMode  = 20;
        int colCount =  8;
        int colTotal = 24;
        int colAvg   = 20;
        int lineWidth = colMode + colCount + colTotal + colAvg;

        cout << endl;
        cout << "Age Group: " << groupName[g] << endl;
        printLine(lineWidth);
        cout << padStr("Mode of Transport",       colMode)
             << padStr("Count",                   colCount)
             << padStr("Total Emission (kg CO2)", colTotal)
             << padStr("Average per Resident",    colAvg) << endl;
        printLine(lineWidth);

        for (int m = 0; m < 4; m++)
        {
            double modeAvg = (modeCounts[m] > 0)
                             ? (modeTotals[m] / modeCounts[m])
                             : 0.0;

            cout << padStr(modes[m],              colMode)
                 << padStr(intStr(modeCounts[m]), colCount)
                 << padStr(dblStr(modeTotals[m]), colTotal)
                 << padStr(dblStr(modeAvg),       colAvg) << endl;
        }

        printLine(lineWidth);

        // group summary below the table
        if (groupCount > 0)
        {
            cout << "Total Emission for Age Group : "
                 << dblStr(groupTotalEmit) << " kg CO2" << endl;
            cout << "Most Preferred Transport     : "
                 << modes[preferredIndex]
                 << " (" << modeCounts[preferredIndex] << " residents)" << endl;
            cout << "Average CO2 per Resident     : "
                 << dblStr(avgEmit) << " kg" << endl;
        }
        else
        {
            // City B is a university town — senior and working-adult
            // groups have zero residents. This is expected, not an error.
            cout << "No residents found in this age group." << endl;
            cout << "(City B is a university town — this age group" << endl;
            cout << " is not represented in this dataset.)" << endl;
        }
    }

    // mark that this function has run so Section 5 and 9 can proceed
    sectionFourRan = true;
}

// sectionFiveEmissions
//   5a - Calculate total carbon emissions per dataset
//   5b - Determine carbon emissions per mode of transport
//   5c - Compare emissions across datasets and age groups
//   5d - Display results as text-based tables in the console
//
// Depends on sectionFourAgeGroups having run first to populate
// the savedModeTotal, savedModeCounts, and datasetTotalEmit
// private members. If called out of order, prints a warning.
void ResidentList::sectionFiveEmissions()
{
    // guard: Section 4 must run first to populate the saved data
    if (!sectionFourRan)
    {
        cout << "  Warning: call sectionFourAgeGroups() before sectionFiveEmissions()." << endl;
        return;
    }

    string modes[4] = { "Bicycle", "Walking", "School Bus", "Carpool" };

    string groupShort[5] = { "6-17", "18-25", "26-45", "46-60", "61-100" };

    // ----------------------------------------------------------
    // [5a / 5d] Dataset grand total CO2
    // ----------------------------------------------------------
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 5a / 5d]                                            " << endl;
    cout << "        CITY B DATASET - TOTAL CARBON EMISSION                " << endl;
    cout << "===============================================================" << endl;
    cout << "  Total residents in dataset : " << size << endl;
    cout << "  Grand total CO2 emission   : " << dblStr(datasetTotalEmit)
         << " kg CO2" << endl;
    cout << "===============================================================" << endl;

    // ----------------------------------------------------------
    // [5b / 5d] CO2 by transport mode — all residents combined
    // Sum each mode's totals across all five age groups.
    // ----------------------------------------------------------
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 5b / 5d]                                            " << endl;
    cout << "        CO2 BY TRANSPORT MODE - All Residents (City B)        " << endl;
    cout << "===============================================================" << endl;

    // aggregate across all groups
    double allModeTotals[4] = { 0.0, 0.0, 0.0, 0.0 };
    int    allModeCounts[4] = {   0,   0,   0,   0 };
    for (int g = 0; g < 5; g++)
    {
        for (int m = 0; m < 4; m++)
        {
            allModeTotals[m] += savedModeTotal[g][m];
            allModeCounts[m] += savedModeCounts[g][m];
        }
    }

    int colM = 16, colC = 10, colT = 20, colA = 22, colP = 12;
    printLine(80);
    cout << padStr("Transport",        colM)
         << padStr("Residents",        colC)
         << padStr("Total CO2 (kg)",   colT)
         << padStr("Avg per Resident", colA)
         << padStr("% of Total",       colP) << endl;
    printLine(80);

    for (int m = 0; m < 4; m++)
    {
        double mAvg = (allModeCounts[m] > 0)
                      ? (allModeTotals[m] / allModeCounts[m])
                      : 0.0;

        double pct = (datasetTotalEmit > 0.0)
                     ? ((allModeTotals[m] / datasetTotalEmit) * 100.0)
                     : 0.0;

        // format percentage to one decimal place using snprintf
        char pctBuf[16];
        snprintf(pctBuf, sizeof(pctBuf), "%.1f%%", pct);

        cout << padStr(modes[m],                colM)
             << padStr(intStr(allModeCounts[m]), colC)
             << padStr(dblStr(allModeTotals[m]), colT)
             << padStr(dblStr(mAvg),             colA)
             << padStr(string(pctBuf),           colP) << endl;
    }

    printLine(80);
    cout << padStr("TOTAL",                    colM)
         << padStr(intStr(size),               colC)
         << padStr(dblStr(datasetTotalEmit),   colT) << endl;
    printLine(80);
    cout << "  Note: Bicycle and Walking emit 0 kg CO2 (zero-emission modes)." << endl;
    cout << "  School Bus and Carpool carry non-zero carbon emission factors." << endl;

    // ----------------------------------------------------------
    // [5c / 5d] Cross-group comparison table
    // All 5 age groups side by side so the reader can compare
    // which group emits most and which transport mode dominates.
    // ----------------------------------------------------------
    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 5c / 5d]                                            " << endl;
    cout << "        CROSS-GROUP EMISSION COMPARISON - City B              " << endl;
    cout << "===============================================================" << endl;

    int cAge = 10, cRes = 12, cTot = 18, cAvg = 20, cTop = 18;
    printLine(78);
    cout << padStr("Age Group",        cAge)
         << padStr("Residents",        cRes)
         << padStr("Total CO2 (kg)",   cTot)
         << padStr("Avg CO2/Resident", cAvg)
         << padStr("Top Transport",    cTop) << endl;
    printLine(78);

    for (int g = 0; g < 5; g++)
    {
        cout << padStr(groupShort[g],         cAge)
             << padStr(intStr(savedCount[g]), cRes)
             << padStr(dblStr(savedTotal[g]), cTot)
             << padStr(dblStr(savedAvg[g]),   cAvg)
             << padStr(savedTopMode[g],       cTop) << endl;
    }

    printLine(78);
}

// sectionNineInsights  [Section 9a / 9b / 9c]
void ResidentList::sectionNineInsights()
{
    // guard: Section 4 must run first to populate the saved data
    if (!sectionFourRan)
    {
        cout << "  Warning: call sectionFourAgeGroups() before sectionNineInsights()." << endl;
        return;
    }

    string modes[4]      = { "Bicycle", "Walking", "School Bus", "Carpool" };
    string groupShort[5] = { "6-17", "18-25", "26-45", "46-60", "61-100" };
    string groupFull[5]  = {
        "Children & Teenagers",
        "University Students / Young Adults",
        "Working Adults (Early Career)",
        "Working Adults (Late Career)",
        "Senior Citizens / Retirees"
    };

    cout << endl;
    cout << "===============================================================" << endl;
    cout << " [Section 9a / 9b / 9c]                                       " << endl;
    cout << "        INSIGHTS AND RECOMMENDATIONS - City B                 " << endl;
    cout << "===============================================================" << endl;

    // ----------------------------------------------------------
    // [9a] Mode preference comparison across all active groups
    // Shows exactly how many residents in each group use each mode
    // so city planners can spot which mode dominates per group.
    // ----------------------------------------------------------
    cout << endl;
    cout << "  [9a] Mode Preference Comparison Across Age Groups" << endl;

    // build dynamic column widths: one column per mode
    int cLabel = 10;
    int cMode  = 14;
    int rowWidth = cLabel + (cMode * 4);

    printLine(rowWidth);
    cout << padStr("Age Group", cLabel);
    for (int m = 0; m < 4; m++)
    {
        cout << padStr(modes[m], cMode);
    }
    cout << endl;
    printLine(rowWidth);

    for (int g = 0; g < 5; g++)
    {
        cout << padStr(groupShort[g], cLabel);
        for (int m = 0; m < 4; m++)
        {
            cout << padStr(intStr(savedModeCounts[g][m]), cMode);
        }
        cout << endl;
    }

    printLine(rowWidth);
    cout << "  Counts show how many residents in each group use that mode." << endl;

    // ----------------------------------------------------------
    // [9b] Identify the highest-emitting age group
    // Also flag groups with zero residents (expected for City B).
    // ----------------------------------------------------------
    cout << endl;
    cout << "  [9b] Highest-Emitting Age Group Analysis" << endl;
    printLine(70);

    // find the group with the largest total CO2
    int highestGroup = 0;
    for (int g = 1; g < 5; g++)
    {
        if (savedTotal[g] > savedTotal[highestGroup])
        {
            highestGroup = g;
        }
    }

    // find which mode produces the most CO2 across all residents
    double allModeTotals[4] = { 0.0, 0.0, 0.0, 0.0 };
    for (int g = 0; g < 5; g++)
    {
        for (int m = 0; m < 4; m++)
        {
            allModeTotals[m] += savedModeTotal[g][m];
        }
    }
    int highestMode = 0;
    for (int m = 1; m < 4; m++)
    {
        if (allModeTotals[m] > allModeTotals[highestMode])
        {
            highestMode = m;
        }
    }

    // count how many groups actually have residents
    int activeGroups = 0;
    for (int g = 0; g < 5; g++)
    {
        if (savedCount[g] > 0) activeGroups++;
    }

    if (savedCount[highestGroup] > 0)
    {
        cout << "  Highest-emitting group : " << groupShort[highestGroup]
             << " " << groupFull[highestGroup] << endl;
        cout << "  Total CO2 for group    : "
             << dblStr(savedTotal[highestGroup]) << " kg CO2" << endl;
        cout << "  Average per resident   : "
             << dblStr(savedAvg[highestGroup]) << " kg CO2" << endl;
        cout << "  Top transport in group : " << savedTopMode[highestGroup]
             << " (" << savedTopCount[highestGroup] << " residents)" << endl;
    }

    cout << endl;
    cout << "  Highest-emitting transport mode (dataset-wide) : "
         << modes[highestMode] << endl;
    cout << "  Total CO2 from that mode                       : "
         << dblStr(allModeTotals[highestMode]) << " kg CO2" << endl;
    cout << endl;
    cout << "  Active age groups in City B   : " << activeGroups << " of 5" << endl;
    cout << "  (City B is a university town — only younger groups exist)" << endl;

    // count zero-emission riders (Bicycle + Walking)
    int zeroBicycle = 0, zeroWalking = 0;
    for (int g = 0; g < 5; g++)
    {
        zeroBicycle += savedModeCounts[g][0];  // index 0 = Bicycle
        zeroWalking += savedModeCounts[g][1];  // index 1 = Walking
    }
    int totalZeroEmission = zeroBicycle + zeroWalking;

    cout << endl;
    cout << "  Zero-emission riders (Bicycle) : " << zeroBicycle << endl;
    cout << "  Zero-emission riders (Walking) : " << zeroWalking << endl;
    cout << "  Total zero-emission riders     : " << totalZeroEmission
         << " of " << size << " residents ("
         << dblStr((size > 0) ? ((double)totalZeroEmission / size * 100.0) : 0.0)
         << "%)" << endl;

    printLine(70);

    // ----------------------------------------------------------
    // [9c] Practical recommendations for city planners
    // Based directly on what the emission and mode data show.
    // ----------------------------------------------------------
    cout << endl;
    cout << "  [9c] Recommendations for City Planners" << endl;
    printLine(70);
    cout << endl;
    cout << "  1. PROTECT ZERO-EMISSION COMMUTING" << endl;
    cout << "     Bicycle and Walking together account for "
         << dblStr((size > 0) ? ((double)totalZeroEmission / size * 100.0) : 0.0)
         << "% of all commuters" << endl;
    cout << "     and produce 0 kg CO2. Expanding protected cycling lanes" << endl;
    cout << "     and pedestrian paths for short distances (<5 km) will" << endl;
    cout << "     maintain and grow this zero-emission proportion." << endl;
    cout << endl;
    cout << "  2. SHIFT SCHOOL BUS RIDERS TO CARPOOL" << endl;
    cout << "     School Bus and Carpool are the only emission-producing" << endl;
    cout << "     modes in City B. Carpool distributes its CO2 cost across" << endl;
    cout << "     more passengers per trip, lowering the per-resident" << endl;
    cout << "     emission figure. Incentivising carpool formation among" << endl;
    cout << "     university students will reduce average CO2 per commuter." << endl;
    cout << endl;
    cout << "  3. TARGET THE HIGHEST-EMITTING GROUP FIRST" << endl;
    cout << "     Age group " << groupShort[highestGroup]
         << " (" << groupFull[highestGroup] << ")" << endl;
    cout << "     contributes the most total CO2 in this dataset." << endl;
    cout << "     Focused awareness campaigns and subsidised cycling" << endl;
    cout << "     equipment for this group will have the largest impact." << endl;
    cout << endl;
    cout << "  4. DATA-DRIVEN POLICY FOR FUTURE DATASETS" << endl;
    cout << "     City B only covers ages 6-25. Expanding data collection" << endl;
    cout << "     to working-adult and senior age groups would allow a full" << endl;
    cout << "     city-wide comparison across all five age categories." << endl;
    printLine(70);
}