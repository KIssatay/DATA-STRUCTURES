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
	cout << "  Data Structure: Array" << endl;
	cout << "========================================" << endl;

	// Load data into array
	ResidentArray *data = new ResidentArray();
	if (!data->loadCSV("dataset1-cityA.csv")) {
		cout << "No data loaded." << endl;
		delete data;
		return 1;
	}

	cout << "\nLoaded " << data->getSize() << " records.\n" << endl;

	// Step 1: Show initial analysis (Carbon Emission & Age Groups)
	cout << "\n========================================" << endl;
	cout << "  Initial Analysis" << endl;
	cout << "========================================" << endl;

	// Age breakdown
	cout << "\n== Age Group Categorization ==" << endl;
	data->ageGroupReport();

	// Emission analysis
	cout << "\n== Carbon Emission Analysis ==" << endl;
	data->emissionReport();

	// Step 2: Interactive menu for sorting
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
			
			cout << "\n== Sorting by " << data->keyName(fieldChoice) << " ==" << endl;
			ResidentArray sorted;
			data->copyTo(sorted);
			sorted.sortBy(fieldChoice);
			sorted.showRecords(10);
			cout << "... (showing first 10 records)" << endl;
		}
		
		// Step 3: Interactive menu for searching
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
			cout << "  [1] Search by Age Group" << endl;
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
				cout << "\nSelect age group:" << endl;
				for (int g = 0; g < NUM_GROUPS; g++) {
					cout << "  [" << (g+1) << "] " << GROUP_NAMES[g] << endl;
				}
				cout << "\nYour choice: ";
				
				int groupChoice;
				cin >> groupChoice;
				clearInput();
				
				if (groupChoice >= 1 && groupChoice <= NUM_GROUPS) {
					data->searchAgeGroup(groupChoice - 1);
				} else {
					cout << "\nInvalid age group." << endl;
				}
			}
			else if (searchType == 2) {
				cout << "\nSelect transport mode:" << endl;
				for (int m = 0; m < NUM_MODES; m++) {
					cout << "  [" << (m+1) << "] " << MODE_NAMES[m] << endl;
				}
				cout << "\nYour choice: ";
				
				int modeChoice;
				cin >> modeChoice;
				clearInput();
				
				if (modeChoice >= 1 && modeChoice <= NUM_MODES) {
					data->searchTransport(modeChoice - 1);
				} else {
					cout << "\nInvalid transport mode." << endl;
				}
			}
			else if (searchType == 3) {
				cout << "\nEnter minimum distance (km): ";
				double minDist;
				cin >> minDist;
				clearInput();
				
				data->searchDistance(minDist);
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

	// Step 4: Run full hypothesis tests
	cout << "\n========================================" << endl;
	cout << "  Running Full Hypothesis Testing" << endl;
	cout << "========================================" << endl;
	data->runAllTests("dataset1-cityA.csv");

	// Insights
	cout << "\n== Insights & Recommendations ==" << endl;
	data->insights();

	// Performance summary
	PerfLogger::printSummary();

	// Cleanup
	delete data;

	cout << "\n========================================" << endl;
	cout << "  Analysis Complete - Array" << endl;
	cout << "========================================" << endl;
	
	return 0;
}
