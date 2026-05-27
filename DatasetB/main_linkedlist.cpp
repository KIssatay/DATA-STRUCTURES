#include "resident.hpp"
#include <iostream>
#include <limits>
#include <ctime>
using namespace std;

void clearInput() {
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
	string filename = "dataset2-cityB.csv";
	
	cout << "========================================" << endl;
	cout << "  City B - Carbon Emission Analysis" << endl;
	cout << "  Data Structure: Linked List" << endl;
	cout << "========================================" << endl;
	cout << endl;
	cout << "Loading dataset: " << filename << endl;

	// Load the CSV into the linked list
	ResidentList list;
	list.loadFromCSV(filename);
	
	cout << "\nLoaded " << list.getSize() << " records.\n" << endl;

	// Step 1: Show initial analysis
	cout << "\n========================================" << endl;
	cout << "  Initial Analysis" << endl;
	cout << "========================================" << endl;

	list.sectionFourAgeGroups();
	list.sectionFiveEmissions();

	// Step 2: Interactive menu for sorting and searching
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
			cout << "  [3] Monthly Emission" << endl;
			cout << "\nYour choice: ";
			
			int fieldChoice;
			cin >> fieldChoice;
			
			if (cin.fail() || fieldChoice < 1 || fieldChoice > 3) {
				clearInput();
				cout << "\nInvalid choice. Please select 1, 2, or 3." << endl;
				continue;
			}
			clearInput();
			
			string fieldNames[3] = {"age", "distance", "emission"};
			string displayNames[3] = {"Age", "Daily Distance", "Monthly Emission"};
			
			cout << "\nSelect sorting algorithm:" << endl;
			cout << "  [1] Bubble Sort" << endl;
			cout << "  [2] Merge Sort" << endl;
			cout << "\nYour choice: ";
			
			int algoChoice;
			cin >> algoChoice;
			clearInput();
			
			if (algoChoice == 1 || algoChoice == 2) {
				ResidentList sortList;
				sortList.loadFromCSV(filename);
				int comp = 0;
				
				clock_t startTime = clock();
				if (algoChoice == 1) {
					cout << "\n== Bubble Sort by " << displayNames[fieldChoice - 1] << " ==" << endl;
					sortList.bubbleSort(fieldNames[fieldChoice - 1], comp);
				} else {
					cout << "\n== Merge Sort by " << displayNames[fieldChoice - 1] << " ==" << endl;
					sortList.mergeSort(fieldNames[fieldChoice - 1], comp);
				}
				clock_t endTime = clock();
				long time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
				
				cout << "Comparisons: " << comp << " | Time: " << time << " us" << endl;
				sortList.displayList(sortList.getSize());
			}
		}
		
		// Searching menu
		cout << "\n========================================" << endl;
		cout << "  Searching Experiments" << endl;
		cout << "========================================" << endl;
		cout << "\nWould you like to search the dataset?" << endl;
		cout << "  [1] Yes" << endl;
		cout << "  [2] No, continue to insights" << endl;
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
			cout << "  [1] Linear Search - Transport Mode" << endl;
			cout << "  [2] Linear Search - Age Range" << endl;
			cout << "  [3] Sentinel Search - Age Range (sorted)" << endl;
			cout << "\nYour choice: ";
			
			int searchType;
			cin >> searchType;
			clearInput();
			
			if (searchType == 1) {
				cout << "\nSelect transport mode:" << endl;
				cout << "  [1] Bicycle" << endl;
				cout << "  [2] Bus (includes School Bus)" << endl;
				cout << "  [3] Walking" << endl;
				cout << "  [4] Carpool" << endl;
				cout << "\nYour choice: ";
				
				int modeChoice;
				cin >> modeChoice;
				clearInput();
				
				string modes[4] = {"Bicycle", "Bus", "Walking", "Carpool"};
				if (modeChoice >= 1 && modeChoice <= 4) {
					int comp = 0;
					clock_t startTime = clock();
					list.linearSearch("transport", modes[modeChoice - 1], 0, 0, comp);
					clock_t endTime = clock();
					long time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
					cout << "\nComparisons: " << comp << " | Time: " << time << " us" << endl;
				}
			}
			else if (searchType == 2) {
				cout << "\nEnter age range:" << endl;
				cout << "  Minimum age: ";
				int minAge;
				cin >> minAge;
				cout << "  Maximum age: ";
				int maxAge;
				cin >> maxAge;
				clearInput();
				
				int comp = 0;
				clock_t startTime = clock();
				list.linearSearch("age", "", minAge, maxAge, comp);
				clock_t endTime = clock();
				long time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
				cout << "\nComparisons: " << comp << " | Time: " << time << " us" << endl;
			}
			else if (searchType == 3) {
				cout << "\nEnter age range:" << endl;
				cout << "  Minimum age: ";
				int minAge;
				cin >> minAge;
				cout << "  Maximum age: ";
				int maxAge;
				cin >> maxAge;
				clearInput();
				
				// Sort first
				ResidentList sortedList;
				sortedList.loadFromCSV(filename);
				int dummy = 0;
				cout << "\nPre-sorting by age..." << endl;
				sortedList.mergeSort("age", dummy);
				
				int comp = 0;
				clock_t startTime = clock();
				sortedList.sentinelSearch("age", minAge, maxAge, comp);
				clock_t endTime = clock();
				long time = (long)(endTime - startTime) * 1000000 / CLOCKS_PER_SEC;
				cout << "\nComparisons: " << comp << " | Time: " << time << " us" << endl;
			}
		}
		
		// Ask if user wants to continue
		cout << "\n========================================" << endl;
		cout << "\nWould you like to:" << endl;
		cout << "  [1] Perform another sort/search" << endl;
		cout << "  [2] Continue to insights & recommendations" << endl;
		cout << "\nYour choice: ";
		
		int continueChoice;
		cin >> continueChoice;
		clearInput();
		
		if (continueChoice == 2) {
			continueProgram = false;
		}
	}

	// Step 3: Insights and recommendations
	list.sectionNineInsights();

	cout << "\n========================================" << endl;
	cout << "  Analysis Complete - Linked List" << endl;
	cout << "========================================" << endl;

	return 0;
}
