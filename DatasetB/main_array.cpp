#include "resident.hpp"
#include <iostream>
#include <limits>
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
	cout << "  Data Structure: Array" << endl;
	cout << "========================================" << endl;
	cout << endl;
	cout << "Loading dataset: " << filename << endl;

	ResidentArrayManager manager;
	manager.loadCSV(filename);
	
	cout << "\nDataset loaded successfully.\n" << endl;

	// Step 1: Show initial analysis
	cout << "\n========================================" << endl;
	cout << "  Initial Analysis" << endl;
	cout << "========================================" << endl;

	manager.analyzeAgeGroups();
	manager.emissionAnalysis();

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
			manager.sortingExperiment();
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
			manager.searchingExperiment();
		}
		
		// Ask if user wants to continue
		cout << "\n========================================" << endl;
		cout << "\nWould you like to:" << endl;
		cout << "  [1] Perform another sort/search" << endl;
		cout << "  [2] Continue to insights & performance analysis" << endl;
		cout << "\nYour choice: ";
		
		int continueChoice;
		cin >> continueChoice;
		clearInput();
		
		if (continueChoice == 2) {
			continueProgram = false;
		}
	}

	// Step 3: Run full analysis
	cout << "\n========================================" << endl;
	cout << "  Running Full Analysis" << endl;
	cout << "========================================" << endl;

	manager.performanceAnalysis();
	manager.performanceTest();
	manager.testDataOrderEffect();
	manager.hypothesis3_test();
	manager.hypothesis4_test();
	manager.insights();

	cout << "\n========================================" << endl;
	cout << "  Analysis Complete - Array" << endl;
	cout << "========================================" << endl;

	return 0;
}
