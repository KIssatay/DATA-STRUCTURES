#include "linkedlist_city.cpp"
#include "Resident.hpp"
#include <iostream>
#include <limits>
using namespace std;

void clearInput() {
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
	cout << "========================================" << endl;
	cout << "  City A - Carbon Emission Analysis" << endl;
	cout << "  Data Structure: Linked List" << endl;
	cout << "========================================" << endl;

	// Step 1: load csv data into linked list
	ResidentLinkedList llData;
	LoadCSVIntoList("dataset1-cityA.csv", &llData);

	if (llData.head == nullptr)
	{
		cout << "Failed to load data." << endl;
		return 1;
	}

	cout << "\nLoaded " << llData.getSize() << " records from City A." << endl;

	// Step 2: Show initial analysis (Carbon Emission & Age Groups)
	cout << "\n========================================" << endl;
	cout << "  Initial Analysis" << endl;
	cout << "========================================" << endl;

	AnalyzeAgeGroups(&llData);
	AnalyzeEmissions(&llData);

	// Step 3: Interactive menu for sorting
	bool continueProgram = true;
	while (continueProgram) {
		cout << "\n========================================" << endl;
		cout << "  Sorting Experiments" << endl;
		cout << "========================================" << endl;
		cout << "\nWould you like to sort the dataset?" << endl;
		cout << "  [1] Yes" << endl;
		cout << "  [2] No, skip to searching" << endl;
		cout << "\nYour choice: ";
		
		int sortChoice;
		cin >> sortChoice;
		
		if (cin.fail()) {
			clearInput();
			cout << "\nInvalid input. Please enter 1 or 2." << endl;
			continue;
		}
		clearInput();
		
		if (sortChoice == 1) {
			cout << "\nSelect sorting field:" << endl;
			cout << "  [1] Age" << endl;
			cout << "  [2] Daily Distance" << endl;
			cout << "  [3] Carbon Emission" << endl;
			cout << "\nYour choice: ";
			
			int fieldChoice;
			cin >> fieldChoice;
			
			if (cin.fail() || fieldChoice < 1 || fieldChoice > 3) {
				clearInput();
				cout << "\nInvalid choice. Please select 1, 2, or 3." << endl;
				continue;
			}
			clearInput();
			
			string fieldNames[3] = {"Age", "Daily Distance", "Carbon Emission"};
			cout << "\n== Sorting by " << fieldNames[fieldChoice - 1] << " ==" << endl;
			
			ResidentLinkedList sorted;
			CopyEntireList(&llData, &sorted);
			PerformMergeSort(&sorted, fieldChoice);
			sorted.DisplayList(sorted.getSize());  // Show all records
			cout << "\nTotal records: " << sorted.getSize() << endl;
		}
		
		// Step 4: Interactive menu for searching
		cout << "\n========================================" << endl;
		cout << "  Searching Experiments" << endl;
		cout << "========================================" << endl;
		cout << "\nWould you like to search the dataset?" << endl;
		cout << "  [1] Yes" << endl;
		cout << "  [2] No, continue to full analysis" << endl;
		cout << "\nYour choice: ";
		
		int searchChoice;
		cin >> searchChoice;
		
		if (cin.fail()) {
			clearInput();
			cout << "\nInvalid input. Please enter 1 or 2." << endl;
			continue;
		}
		clearInput();
		
		if (searchChoice == 1) {
			cout << "\nSelect search type:" << endl;
			cout << "  [1] Search by Age" << endl;
			cout << "  [2] Search by Transport Mode" << endl;
			cout << "  [3] Search by Distance Threshold" << endl;
			cout << "\nYour choice: ";
			
			int searchType;
			cin >> searchType;
			
			if (cin.fail() || searchType < 1 || searchType > 3) {
				clearInput();
				cout << "\nInvalid choice. Please select 1, 2, or 3." << endl;
				continue;
			}
			clearInput();
			
			if (searchType == 1) {
				cout << "\nEnter age to search: ";
				int targetAge;
				cin >> targetAge;
				clearInput();
				
				// Sort first for exponential search
				ResidentLinkedList sorted;
				CopyEntireList(&llData, &sorted);
				PerformMergeSort(&sorted, 1);
				
				cout << "\n--- Linear Search ---" << endl;
				auto s1 = chrono::high_resolution_clock::now();
				ResidentNode* result1 = LinearSearchByAge(&sorted, targetAge);
				auto e1 = chrono::high_resolution_clock::now();
				long long t1 = chrono::duration_cast<chrono::microseconds>(e1 - s1).count();
				
				if (result1) {
					cout << "Found: " << result1->info.ResidentID 
						 << " (Age: " << result1->info.Age << ")" << endl;
				} else {
					cout << "Not found" << endl;
				}
				cout << "Time: " << t1 << " microseconds" << endl;
				
				cout << "\n--- Exponential Search ---" << endl;
				auto s2 = chrono::high_resolution_clock::now();
				ResidentNode* result2 = ExponentialSearchByAge(&sorted, targetAge);
				auto e2 = chrono::high_resolution_clock::now();
				long long t2 = chrono::duration_cast<chrono::microseconds>(e2 - s2).count();
				
				if (result2) {
					cout << "Found: " << result2->info.ResidentID 
						 << " (Age: " << result2->info.Age << ")" << endl;
				} else {
					cout << "Not found" << endl;
				}
				cout << "Time: " << t2 << " microseconds" << endl;
			}
			else if (searchType == 2) {
				cout << "\nSelect transport mode:" << endl;
				cout << "  [1] Car" << endl;
				cout << "  [2] Bus" << endl;
				cout << "  [3] Bicycle" << endl;
				cout << "  [4] Walking" << endl;
				cout << "\nYour choice: ";
				
				int modeChoice;
				cin >> modeChoice;
				clearInput();
				
				string modes[4] = {"Car", "Bus", "Bicycle", "Walking"};
				if (modeChoice >= 1 && modeChoice <= 4) {
					ResidentLinkedList results;
					auto s = chrono::high_resolution_clock::now();
					int count = LinearSearchByTransport(&llData, modes[modeChoice - 1], &results);
					auto e = chrono::high_resolution_clock::now();
					long long t = chrono::duration_cast<chrono::microseconds>(e - s).count();
					
					cout << "\nFound " << count << " residents using " << modes[modeChoice - 1] << endl;
					cout << "Time: " << t << " microseconds" << endl;
					results.DisplayList(results.getSize());  // Show all results
				} else {
					cout << "\nInvalid transport mode." << endl;
				}
			}
			else if (searchType == 3) {
				cout << "\nEnter minimum distance (km): ";
				double minDist;
				cin >> minDist;
				clearInput();
				
				ResidentLinkedList results;
				auto s = chrono::high_resolution_clock::now();
				int count = LinearSearchByDistanceAbove(&llData, minDist, &results);
				auto e = chrono::high_resolution_clock::now();
				long long t = chrono::duration_cast<chrono::microseconds>(e - s).count();
				
				cout << "\nFound " << count << " residents with distance > " << minDist << " km" << endl;
				cout << "Time: " << t << " microseconds" << endl;
				results.DisplayList(results.getSize());  // Show all results
			}
		}
		
		// Ask if user wants to continue or finish
		cout << "\n========================================" << endl;
		cout << "\nWould you like to:" << endl;
		cout << "  [1] Perform another sort/search" << endl;
		cout << "  [2] Continue to full analysis & hypothesis testing" << endl;
		cout << "\nYour choice: ";
		
		int continueChoice;
		cin >> continueChoice;
		clearInput();
		
		if (continueChoice == 2) {
			continueProgram = false;
		}
	}

	// Step 5: Run full hypothesis testing
	cout << "\n========================================" << endl;
	cout << "  Running Full Hypothesis Testing" << endl;
	cout << "========================================" << endl;

	RunSortingExperiment(&llData);
	RunSearchingExperiment(&llData);
	MeasureLoadingTime("dataset1-cityA.csv");
	MeasureMemoryUsage(&llData);
	MeasureLinearSearchTime(&llData);
	MeasureScalability("dataset1-cityA.csv");
	TestMergeSortStability(&llData);
	TestExponentialSearchPosition(&llData);
	TestSortFieldImpact(&llData);
	TestGetNodeAtCost(&llData);
	PerformanceSummary(&llData);
	InsightsAndRecommendations(&llData);

	cout << "\n========================================" << endl;
	cout << "  Analysis Complete - Linked List" << endl;
	cout << "========================================" << endl;

	return 0;
}
