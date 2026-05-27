#include "resident.hpp"
#include <fstream>
#include <cstdlib>
#include <chrono>

// Constructor - initializes an empty list
ResidentList::ResidentList() {
    head = nullptr;
    size = 0;
}

// Destructor - frees all nodes
ResidentList::~ResidentList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    size = 0;
}

// Adds a resident to the end of the linked list
void ResidentList::append(const Resident& r) {
    // Step 1: create a new node and fill it with data
    Node* newNode = new Node();
    newNode->data = r;
    newNode->next = nullptr;
    
    // Step 2: if the list is empty, make this the first node
    if (!head) {
        head = newNode;
    } else {
        // Step 3: otherwise, walk to the end of the list
        Node* current = head;
        while (current->next) current = current->next;
        
        // Step 4: attach the new node at the end
        current->next = newNode;
    }
    size++;
}

// Builds a linked list from an array
void ResidentList::buildFromArray(const ResidentArray& arr) {
    // Step 1: copy each resident from the array into the list
    for (int i = 0; i < arr.getSize(); i++)
        append(arr.getData()[i]);
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

// Loads resident data from a CSV file into the linked list
int ResidentList::loadCSV(const char* fname) {
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
        
        // Step 5: parse the line and add it to the list
        Resident r;
        if (parseLine(line, r)) {
            append(r);
            loaded++;
        }
    }
    
    file.close();
    return loaded;
}

size_t ResidentList::memUsage() const {
    return sizeof(Node) * size;
}

void ResidentList::display() const {
    cout << "[LINKED LIST – showing " << size << " of " << size << " records]\n";
    printHeader();
    Node* current = head;
    while (current) {
        printRow(current->data);
        current = current->next;
    }
    cout << "\n";
}

// Finds the middle node using slow/fast pointer technique
static Node* listMiddle(Node* head) {
    // Step 1: set up two pointers - slow moves one step, fast moves two steps
    Node* slow = head;
    Node* fast = head ? head->next : nullptr;
    
    // Step 2: move both pointers until fast reaches the end
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    // Step 3: when fast reaches the end, slow is at the middle
    return slow;
}

// Merges two sorted linked lists into one sorted list
static Node* listMerge(Node* left, Node* right, int key) {
    // Step 1: if one list is empty, return the other
    if (!left) return right;
    if (!right) return left;
    
    // Step 2: pick the smaller head node and recursively merge the rest
    if (cmpKey(left->data, right->data, key) <= 0) {
        left->next = listMerge(left->next, right, key);
        return left;
    }
    right->next = listMerge(left, right->next, key);
    return right;
}

// Recursively sorts a linked list using merge sort
static Node* mergeSortNode(Node* head, int key) {
    // Step 1: base case - if list is empty or has one node, it's already sorted
    if (!head || !head->next) return head;
    
    // Step 2: find the middle and split the list into two halves
    Node* middle = listMiddle(head);
    Node* rightHalf = middle->next;
    middle->next = nullptr;

    // Step 3: recursively sort both halves
    Node* leftSorted = mergeSortNode(head, key);
    Node* rightSorted = mergeSortNode(rightHalf, key);
    
    // Step 4: merge the two sorted halves back together
    return listMerge(leftSorted, rightSorted, key);
}

void ResidentList::mergeSort(int key) {
    head = mergeSortNode(head, key);
}

// Sorts a linked list using insertion sort
void ResidentList::insertionSort(int key) {
    Node* sortedHead = nullptr;
    Node* current = head;

    // Step 1: take each node from the original list one by one
    while (current) {
        Node* next = current->next;

        // Step 2: insert the current node into the sorted list at the right position
        if (!sortedHead || cmpKey(current->data, sortedHead->data, key) <= 0) {
            // Insert at the beginning
            current->next = sortedHead;
            sortedHead = current;
        } else {
            // Step 3: find the correct position in the sorted list
            Node* temp = sortedHead;
            while (temp->next && cmpKey(temp->next->data, current->data, key) < 0)
                temp = temp->next;
            
            // Step 4: insert the node after temp
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    
    // Step 5: update the list head to point to the sorted list
    head = sortedHead;
}

// Searches for all residents in a specific age group
int ResidentList::linSearchAgeGroup(const string& group,
                          Resident* results, int maxResults) const {
    int count = 0;
    for (Node* current = head; current && count < maxResults; current = current->next)
        if (current->data.ageGroup == group)
            results[count++] = current->data;
    return count;
}

// Searches for all residents using a specific transport mode
int ResidentList::linSearchMode(const string& mode,
                      Resident* results, int maxResults) const {
    int count = 0;
    for (Node* current = head; current && count < maxResults; current = current->next)
        if (current->data.modeOfTransport == mode)
            results[count++] = current->data;
    return count;
}

// Searches for residents traveling more than a threshold distance
int ResidentList::linSearchDist(double threshold,
                      Resident* results, int maxResults) const {
    int count = 0;
    for (Node* current = head; current && count < maxResults; current = current->next)
        if (current->data.dailyDistance > threshold)
            results[count++] = current->data;
    return count;
}

// Jump search for residents within an age range (list must be sorted by age)
int ResidentList::jumpSearchAge(int minAge, int maxAge,
                      Resident* results, int maxResults) const {
    int n = size;
    if (n == 0) return 0;
    int stepSize = (int)sqrt((double)n);

    Node* blockStart = head;
    Node* current = head;

    // Step 1: jump forward in blocks until we find one that might contain minAge
    while (current) {
        // Step 2: move to the end of the current block
        Node* blockEnd = current;
        for (int s = 0; s < stepSize - 1 && blockEnd->next; s++)
            blockEnd = blockEnd->next;

        // Step 3: if this block's last element is >= minAge, we found the right block
        if (blockEnd->data.age >= minAge) break;

        // Step 4: move to the next block
        blockStart = blockEnd->next;
        if (!blockStart) return 0;
        current = blockStart;
    }

    // Step 5: do a linear search within the block to find the first match
    Node* scanner = blockStart;
    while (scanner && scanner->data.age < minAge) scanner = scanner->next;

    // Step 6: collect all residents in the age range
    int count = 0;
    while (scanner && scanner->data.age <= maxAge && count < maxResults) {
        results[count++] = scanner->data;
        scanner = scanner->next;
    }
    return count;
}

// Jump search for residents above a distance threshold (list must be sorted by distance)
int ResidentList::jumpSearchDist(double threshold,
                      Resident* results, int maxResults) const {
    int n = size;
    if (n == 0) return 0;
    int stepSize = (int)sqrt((double)n);

    Node* blockStart = head;
    Node* current = head;

    // Step 1: jump forward in blocks until we find one that might contain values above threshold
    while (current) {
        // Step 2: move to the end of the current block
        Node* blockEnd = current;
        for (int s = 0; s < stepSize - 1 && blockEnd->next; s++)
            blockEnd = blockEnd->next;

        // Step 3: if this block's last element is > threshold, we found the right block
        if (blockEnd->data.dailyDistance > threshold) break;

        // Step 4: move to the next block
        blockStart = blockEnd->next;
        if (!blockStart) return 0;
        current = blockStart;
    }

    // Step 5: do a linear search within the block to find the first match
    Node* scanner = blockStart;
    while (scanner && scanner->data.dailyDistance <= threshold) scanner = scanner->next;

    // Step 6: collect all residents above the threshold
    int count = 0;
    while (scanner && count < maxResults) {
        results[count++] = scanner->data;
        scanner = scanner->next;
    }
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
void ResidentList::emissionByAgeGroup() const {
    // Step 1: initialize statistics for each age group
    GroupStat groups[N_GROUPS];
    for (int g = 0; g < N_GROUPS; g++) {
        groups[g].count = 0;
        groups[g].totalEmission = 0.0;
        for (int m = 0; m < N_MODES; m++) groups[g].modeCounts[m] = 0;
    }

    // Step 2: go through all residents and accumulate stats for their age group
    Node* current = head;
    while (current) {
        for (int g = 0; g < N_GROUPS; g++) {
            if (current->data.ageGroup == AGE_GROUPS[g]) {
                groups[g].count++;
                groups[g].totalEmission += current->data.monthlyEmission;
                int modeIdx = modeIndex(current->data.modeOfTransport);
                if (modeIdx >= 0) groups[g].modeCounts[modeIdx]++;
                break;
            }
        }
        current = current->next;
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
void ResidentList::emissionByMode() const {
    // Step 1: initialize totals for each transport mode
    double totalEmissions[N_MODES] = {0};
    int    counts[N_MODES] = {0};

    // Step 2: accumulate emissions for each mode
    Node* current = head;
    while (current) {
        int modeIdx = modeIndex(current->data.modeOfTransport);
        if (modeIdx >= 0) {
            totalEmissions[modeIdx] += current->data.monthlyEmission;
            counts[modeIdx]++;
        }
        current = current->next;
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
void ResidentList::emissionCrossAnalysis() const {
    // Step 1: initialize the matrix with zeros
    double matrix[N_GROUPS][N_MODES];
    for (int g = 0; g < N_GROUPS; g++)
        for (int m = 0; m < N_MODES; m++)
            matrix[g][m] = 0.0;

    // Step 2: fill the matrix by going through all residents
    Node* current = head;
    while (current) {
        // Step 3: find which age group this resident belongs to
        int groupIdx = -1;
        for (int g = 0; g < N_GROUPS; g++) {
            if (current->data.ageGroup == AGE_GROUPS[g]) {
                groupIdx = g;
                break;
            }
        }
        
        // Step 4: find which transport mode they use
        int modeIdx = modeIndex(current->data.modeOfTransport);
        
        // Step 5: add their emissions to the appropriate cell
        if (groupIdx >= 0 && modeIdx >= 0) {
            matrix[groupIdx][modeIdx] += current->data.monthlyEmission;
        }
        
        current = current->next;
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
