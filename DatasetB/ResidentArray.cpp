#include "resident.hpp"

ResidentArrayManager::ResidentArrayManager() {
    arrCount = 0;
    bubbleComparisons = 0;
    quickComparisons = 0;
}

void ResidentArrayManager::loadCSV(string filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "File not found!\n";
        return;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string temp;

        getline(ss, arr[arrCount].id, ',');
        getline(ss, temp, ','); arr[arrCount].age = stoi(temp);
        getline(ss, arr[arrCount].transport, ',');
        getline(ss, temp, ','); arr[arrCount].distance = stof(temp);
        getline(ss, temp, ','); arr[arrCount].carbonFactor = stof(temp);
        getline(ss, temp, ','); arr[arrCount].days = stoi(temp);

        arrCount++;
    }
    file.close();
}

void ResidentArrayManager::copyArray(ResidentArray dest[], ResidentArray src[], int n) {
        for (int i = 0; i < n; i++) dest[i] = src[i];
    }

    void ResidentArrayManager::shuffleArray(ResidentArray a[], int n) {
        for (int i = 0; i < n; i++) {
            int r = rand() % n;
            swap(a[i], a[r]);
        }
    }

    void ResidentArrayManager::bubbleSort(ResidentArray a[], int n) {
        bool swapped;
        for (int i = 0; i < n - 1; i++) {
            swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                bubbleComparisons++;
                if (a[j].age > a[j + 1].age) {
                    swap(a[j], a[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) break;
        }
    }

    int ResidentArrayManager::partition(ResidentArray a[], int low, int high) {
        float pivot = a[(low + high) / 2].carbonFactor;
        int i = low, j = high;

        while (i <= j) {
            while (a[i].carbonFactor < pivot) { quickComparisons++; i++; }
            while (a[j].carbonFactor > pivot) { quickComparisons++; j--; }

            if (i <= j) {
                swap(a[i], a[j]);
                i++; j--;
            }
        }
        return i;
    }

    void ResidentArrayManager::quickSort(ResidentArray a[], int low, int high) {
        if (low < high) {
            int pi = partition(a, low, high);
            quickSort(a, low, pi - 1);
            quickSort(a, pi, high);
        }
    }

    void ResidentArrayManager::linearSearchTransport(string target) {
        cout << "\nLinear Search (" << target << "):\n";
        for (int i = 0; i < arrCount; i++) {
            if (arr[i].transport == target)
                cout << arr[i].id << " " << arr[i].transport << endl;
        }
    }

    void ResidentArrayManager::binarySearchAge(int target) {
        int l = 0, r = arrCount - 1;
        while (l <= r) {
            int m = (l + r) / 2;
            if (arr[m].age == target) {
                cout << "Found: " << arr[m].id << endl;
                return;
            }
            else if (arr[m].age < target) l = m + 1;
            else r = m - 1;
        }
        cout << "Not found\n";
    }

    void ResidentArrayManager::jumpSearchEmission(float target) {
        int step = sqrt(arrCount);
        int prev = 0;

        while (arr[(step < arrCount ? step : arrCount) - 1].carbonFactor < target) {
            prev = step;
            step += sqrt(arrCount);
            if (prev >= arrCount) {
                cout << "Not found\n";
                return;
            }
        }

        for (int i = prev; i < (step < arrCount ? step : arrCount); i++) {
            if (arr[i].carbonFactor >= target)
                cout << arr[i].id << endl;
        }
    }

    void ResidentArrayManager::analyzeAgeGroups() {
    cout << "\n===== TASK 4 =====\n";

    int groups[5][2] = {{6,17},{18,25},{26,45},{46,60},{61,100}};

    for (int g = 0; g < 5; g++) {
        int minA = groups[g][0], maxA = groups[g][1];

        float total = 0;
        int count = 0;

        int bicycle=0, walking=0, schoolBus=0, carpool=0;

        for (int i = 0; i < arrCount; i++) {
            if (arr[i].age >= minA && arr[i].age <= maxA) {
                count++;

                float e = arr[i].distance * arr[i].carbonFactor * arr[i].days;
                total += e;

                if (arr[i].transport == "Bicycle") bicycle++;
                else if (arr[i].transport == "Walking") walking++;
                else if (arr[i].transport == "School Bus") schoolBus++;
                else if (arr[i].transport == "Carpool") carpool++;
            }
        }

        cout << "\nGroup " << minA << "-" << maxA << endl;

        if (count == 0) {
            cout << "No data\n";
            continue;
        }

        string mostUsed = "None";
        int maxCount = bicycle;

        mostUsed = "Bicycle";

        if (walking > maxCount) {
            maxCount = walking;
            mostUsed = "Walking";
        }
        if (schoolBus > maxCount) {
            maxCount = schoolBus;
            mostUsed = "School Bus";
        }
        if (carpool > maxCount) {
            maxCount = carpool;
            mostUsed = "Carpool";
        }

        cout << "Most Preferred Transport: " << mostUsed << endl;
        cout << "Total Emission: " << total << endl;
        cout << "Average Emission: " << total / count << endl;
    }
}


void ResidentArrayManager::emissionAnalysis() {
    cout << "\n===== TASK 5: CARBON EMISSION ANALYSIS =====\n";

    int groups[5][2] = {{6,17},{18,25},{26,45},{46,60},{61,100}};

    for (int g = 0; g < 5; g++) {
        int minA = groups[g][0];
        int maxA = groups[g][1];

        int bicycleC=0, walkingC=0, schoolBusC=0, carpoolC=0;
        float bicycleE=0, walkingE=0, schoolBusE=0, carpoolE=0;

        for (int i = 0; i < arrCount; i++) {
            if (arr[i].age >= minA && arr[i].age <= maxA) {

                float e = arr[i].distance * arr[i].carbonFactor * arr[i].days;

                if (arr[i].transport == "Bicycle") {
                    bicycleC++; bicycleE += e;
                }
                else if (arr[i].transport == "Walking") {
                    walkingC++; walkingE += e;
                }
                else if (arr[i].transport == "School Bus") {
                    schoolBusC++; schoolBusE += e;
                }
                else if (arr[i].transport == "Carpool") {
                    carpoolC++; carpoolE += e;
                }
            }
        }

        float totalGroup = bicycleE + walkingE + schoolBusE + carpoolE;

        cout << "\nAge Group: " << minA << "-" << maxA << endl;
        cout << "-------------------------------------------------------------\n";

        cout << left << setw(18) << "Mode of Transport"
             << setw(10) << "Count"
             << setw(22) << "Total Emission"
             << setw(20) << "Average" << endl;

        cout << "-------------------------------------------------------------\n";

        auto printRow = [&](string name, int count, float total) {
            float avg = (count > 0) ? total / count : 0;
            cout << left << setw(18) << name
                 << setw(10) << count
                 << setw(22) << total
                 << setw(20) << avg << endl;
        };

        printRow("Bicycle", bicycleC, bicycleE);
        printRow("Walking", walkingC, walkingE);
        printRow("School Bus", schoolBusC, schoolBusE);
        printRow("Carpool", carpoolC, carpoolE);

        cout << "-------------------------------------------------------------\n";
        cout << "Total Emission for Age Group: " << totalGroup << " kg CO2\n";
    }
}


void ResidentArrayManager::sortingExperiment() {
    cout << "\n===== TASK 6: SORTING EXPERIMENTS =====\n";

    ResidentArray ageArr[MAX], distArr[MAX], emisArr[MAX];

    copyArray(ageArr, arr, arrCount);
    copyArray(distArr, arr, arrCount);
    copyArray(emisArr, arr, arrCount);

    bubbleSort(ageArr, arrCount);

    for (int i = 0; i < arrCount - 1; i++) {
        for (int j = 0; j < arrCount - i - 1; j++) {
            if (distArr[j].distance > distArr[j+1].distance) {
                swap(distArr[j], distArr[j+1]);
            }
        }
    }

    quickSort(emisArr, 0, arrCount - 1);

    cout << "\n---------------- SORTED BY AGE ----------------\n";
    cout << left << setw(8) << "ID"
         << setw(6) << "Age"
         << setw(15) << "Transport"
         << setw(10) << "Dist"
         << setw(10) << "CO2"
         << setw(6) << "Days" << endl;

    for (int i = 0; i < 10 && i < arrCount; i++) {
        cout << setw(8) << ageArr[i].id
             << setw(6) << ageArr[i].age
             << setw(15) << ageArr[i].transport
             << setw(10) << ageArr[i].distance
             << setw(10) << ageArr[i].carbonFactor
             << setw(6) << ageArr[i].days << endl;
    }

    cout << "\n------------- SORTED BY DISTANCE --------------\n";
    for (int i = 0; i < 10 && i < arrCount; i++) {
        cout << setw(8) << distArr[i].id
             << setw(6) << distArr[i].age
             << setw(15) << distArr[i].transport
             << setw(10) << distArr[i].distance
             << setw(10) << distArr[i].carbonFactor
             << setw(6) << distArr[i].days << endl;
    }

    cout << "\n------------ SORTED BY EMISSION ---------------\n";
    for (int i = 0; i < 10 && i < arrCount; i++) {
        cout << setw(8) << emisArr[i].id
             << setw(6) << emisArr[i].age
             << setw(15) << emisArr[i].transport
             << setw(10) << emisArr[i].distance
             << setw(10) << emisArr[i].carbonFactor
             << setw(6) << emisArr[i].days << endl;
    }
}


void ResidentArrayManager::searchingExperiment() {
    cout << "\n===== TASK 7: SEARCHING EXPERIMENTS =====\n";

    cout << "\n--- Linear Search (Age 18–25) ---\n";

    cout << left << setw(8) << "ID"
         << setw(6) << "Age"
         << setw(15) << "Transport"
         << setw(10) << "Dist" << endl;

    for (int i = 0; i < arrCount; i++) {
        if (arr[i].age >= 18 && arr[i].age <= 25) {
            cout << setw(8) << arr[i].id
                 << setw(6) << arr[i].age
                 << setw(15) << arr[i].transport
                 << setw(10) << arr[i].distance << endl;
        }
    }

    cout << "\n--- Jump Search (Age 18–25 & School Bus) ---\n";

    ResidentArray temp[MAX];

    for (int i = 0; i < arrCount; i++) {
        temp[i] = arr[i];
    }

    for (int i = 0; i < arrCount - 1; i++) {
        for (int j = 0; j < arrCount - i - 1; j++) {
            if (temp[j].age > temp[j + 1].age) {
                swap(temp[j], temp[j + 1]);
            }
        }
    }

    int step = sqrt(arrCount);
    int prev = 0;
    bool found = false;

    while (temp[(step < arrCount ? step : arrCount) - 1].age < 18) {
        prev = step;
        step += sqrt(arrCount);

        if (prev >= arrCount) {
            cout << "No results found\n";
            return;
        }
    }

    for (int i = prev; i < (step < arrCount ? step : arrCount); i++) {
        if (temp[i].age >= 18 && temp[i].age <= 25 &&
            temp[i].transport == "School Bus") {

            cout << temp[i].id << " "
                 << temp[i].age << " "
                 << temp[i].transport << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "No results found\n";
    }
}


void ResidentArrayManager::performanceTest() {
    ResidentArray a[MAX], b[MAX];
    copyArray(a, arr, arrCount);
    copyArray(b, arr, arrCount);

    bubbleComparisons = 0;
    auto s1 = chrono::high_resolution_clock::now();
    bubbleSort(a, arrCount);
    auto e1 = chrono::high_resolution_clock::now();

    quickComparisons = 0;
    auto s2 = chrono::high_resolution_clock::now();
    quickSort(b, 0, arrCount - 1);
    auto e2 = chrono::high_resolution_clock::now();

    cout << "\n===== H1 =====\n";
    cout << "Bubble: " << chrono::duration<double, milli>(e1-s1).count()
         << " ms (" << bubbleComparisons << ")\n";
    cout << "Quick: " << chrono::duration<double, milli>(e2-s2).count()
         << " ms (" << quickComparisons << ")\n";
}


void ResidentArrayManager::testDataOrderEffect() {
    ResidentArray sorted[MAX], unsorted[MAX];
    copyArray(sorted, arr, arrCount);
    copyArray(unsorted, arr, arrCount);

    bubbleSort(sorted, arrCount);
    shuffleArray(unsorted, arrCount);

    auto s1 = chrono::high_resolution_clock::now();
    bubbleSort(sorted, arrCount);
    auto e1 = chrono::high_resolution_clock::now();

    auto s2 = chrono::high_resolution_clock::now();
    bubbleSort(unsorted, arrCount);
    auto e2 = chrono::high_resolution_clock::now();

    cout << "\n===== H2 =====\n";
    cout << "Sorted: " << chrono::duration<double, milli>(e1-s1).count() << " ms\n";
    cout << "Unsorted: " << chrono::duration<double, milli>(e2-s2).count() << " ms\n";
}


void ResidentArrayManager::hypothesis3_test() {
    cout << "\n===== H3 =====\n";

    auto s1 = chrono::high_resolution_clock::now();
    linearSearchTransport("Bicycle");
    auto e1 = chrono::high_resolution_clock::now();

    ResidentArray temp[MAX];
    copyArray(temp, arr, arrCount);
    bubbleSort(temp, arrCount);

    auto s2 = chrono::high_resolution_clock::now();
    binarySearchAge(15);
    auto e2 = chrono::high_resolution_clock::now();

    cout << "Linear: " << chrono::duration<double, milli>(e1-s1).count() << " ms\n";
    cout << "Binary: " << chrono::duration<double, milli>(e2-s2).count() << " ms\n";
}


void ResidentArrayManager::hypothesis4_test() {
    cout << "\n===== H4 =====\n";

    int sizes[] = {50, 100, 200};

    for (int s = 0; s < 3; s++) {
        int n = sizes[s];

        ResidentArray temp[MAX];
        for (int i = 0; i < n; i++) temp[i] = arr[i];

        auto start = chrono::high_resolution_clock::now();
        bubbleSort(temp, n);
        auto end = chrono::high_resolution_clock::now();

        cout << "Size " << n << ": "
             << chrono::duration<double, milli>(end-start).count()
             << " ms\n";
    }
}


void ResidentArrayManager::insights() {
    cout << "\n===== TASK 9: INSIGHTS =====\n";

    int groups[2][2] = {{6,17},{18,25}};

    for (int g = 0; g < 2; g++) {
        int minA = groups[g][0];
        int maxA = groups[g][1];

        float totalEmission = 0;

        int bicycle=0, walking=0, schoolBus=0, carpool=0;

        for (int i = 0; i < arrCount; i++) {
            if (arr[i].age >= minA && arr[i].age <= maxA) {

                float e = arr[i].distance * arr[i].carbonFactor * arr[i].days;
                totalEmission += e;

                if (arr[i].transport == "Bicycle") bicycle++;
                else if (arr[i].transport == "Walking") walking++;
                else if (arr[i].transport == "School Bus") schoolBus++;
                else if (arr[i].transport == "Carpool") carpool++;
            }
        }

        string mostUsed = "Bicycle";
        int maxCount = bicycle;

        if (walking > maxCount) { maxCount = walking; mostUsed = "Walking"; }
        if (schoolBus > maxCount) { maxCount = schoolBus; mostUsed = "School Bus"; }
        if (carpool > maxCount) { maxCount = carpool; mostUsed = "Carpool"; }

        cout << "\nAge Group " << minA << "-" << maxA << endl;
        cout << "----------------------------------\n";
        cout << "Total Emission: " << totalEmission << endl;
        cout << "Most Preferred Transport: " << mostUsed << endl;

    }

    float emission1 = 0, emission2 = 0;

    for (int i = 0; i < arrCount; i++) {
        float e = arr[i].distance * arr[i].carbonFactor * arr[i].days;

        if (arr[i].age >= 6 && arr[i].age <= 17)
            emission1 += e;

        if (arr[i].age >= 18 && arr[i].age <= 25)
            emission2 += e;
    }

    cout << "\n===== COMPARISON =====\n";

    if (emission1 > emission2)
        cout << "Age group 6-17 produces more emissions\n";
    else
        cout << "Age group 18-25 produces more emissions\n";
}


void ResidentArrayManager::performanceAnalysis() {
    cout << "\n===== TASK 8: PERFORMANCE ANALYSIS =====\n";

    ResidentArray temp[MAX];

    auto memoryUsageMB = [&](int n) {
        return (sizeof(ResidentArray) * n) / (1024.0 * 1024.0);
    };

    double mem = memoryUsageMB(arrCount);

    copyArray(temp, arr, arrCount);
    auto s1 = chrono::high_resolution_clock::now();
    bubbleSort(temp, arrCount);
    auto e1 = chrono::high_resolution_clock::now();

    copyArray(temp, arr, arrCount);
    auto s2 = chrono::high_resolution_clock::now();
    quickSort(temp, 0, arrCount - 1);
    auto e2 = chrono::high_resolution_clock::now();

    auto s3 = chrono::high_resolution_clock::now();
    for (int i = 0; i < arrCount; i++) {
        if (arr[i].transport == "Bicycle") {}
    }
    auto e3 = chrono::high_resolution_clock::now();

    copyArray(temp, arr, arrCount);
    bubbleSort(temp, arrCount);

    auto s4 = chrono::high_resolution_clock::now();
    int left = 0, right = arrCount - 1;
    int target = 20;

    while (left <= right) {
        int mid = (left + right) / 2;
        if (temp[mid].age == target) break;
        else if (temp[mid].age < target) left = mid + 1;
        else right = mid - 1;
    }
    auto e4 = chrono::high_resolution_clock::now();

    copyArray(temp, arr, arrCount);
    bubbleSort(temp, arrCount);

    auto s5 = chrono::high_resolution_clock::now();
    int step = sqrt(arrCount);
    int prev = 0;

    while (temp[(step < arrCount ? step : arrCount) - 1].age < 18) {
        prev = step;
        step += sqrt(arrCount);
        if (prev >= arrCount) break;
    }
    auto e5 = chrono::high_resolution_clock::now();

    cout << "\nAlgorithm name, Time (ms), Memory (MB)\n";

    cout << "Bubble Sort, "
         << chrono::duration<double, milli>(e1 - s1).count()
         << ", " << mem << endl;

    cout << "Quick Sort, "
         << chrono::duration<double, milli>(e2 - s2).count()
         << ", " << mem << endl;

    cout << "Linear Search, "
         << chrono::duration<double, milli>(e3 - s3).count()
         << ", " << mem << endl;

    cout << "Binary Search, "
         << chrono::duration<double, milli>(e4 - s4).count()
         << ", " << mem << endl;

    cout << "Jump Search, "
         << chrono::duration<double, milli>(e5 - s5).count()
         << ", " << mem << endl;
}
// Display function moved from displayArray.cpp
void runDisplay(ResidentArrayManager &m) {
    m.analyzeAgeGroups();
    m.emissionAnalysis();
    m.sortingExperiment();
    m.searchingExperiment();
    m.performanceAnalysis();

    m.performanceTest();
    m.testDataOrderEffect();
    m.hypothesis3_test();
    m.hypothesis4_test();

    m.insights();
}