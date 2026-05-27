#include "linkedlist_city.cpp"
#include "Resident.hpp"
#include <iostream>
using namespace std;

int main()
{
	// ----------------------------------------------------
	//  LINKED LIST SECTION
	// ----------------------------------------------------

	cout << "--------------------------------------------------------" << endl;
	cout << "  CITY A - CARBON EMISSION ANALYSIS" << endl;
	cout << "  DATA STRUCTURE: SINGLY LINKED LIST" << endl;
	cout << "--------------------------------------------------------" << endl;

	// Step 1: load csv data into linked list
	ResidentLinkedList llData;
	LoadCSVIntoList("dataset1-cityA.csv", &llData);

	if (llData.head == nullptr)
	{
		cout << "Failed to load data." << endl;
		return 1;
	}

	cout << "\nLoaded " << llData.getSize() << " records from City A." << endl;
	cout << "\n--- first 10 records (original order) ---" << endl;
	llData.DisplayList(10);

	// Step 2: run age group and emission analysis
	AnalyzeAgeGroups(&llData);
	AnalyzeEmissions(&llData);

	// Step 3: hypothesis 1 - merge sort experiment
	RunSortingExperiment(&llData);

	// Step 4: show sorted samples
	cout << "\n--- sample: merge sort by emission (first 10) ---" << endl;
	ResidentLinkedList llSortedByEmission;
	CopyEntireList(&llData, &llSortedByEmission);
	PerformMergeSort(&llSortedByEmission, 3);
	llSortedByEmission.DisplayList(10);

	cout << "\n--- sample: merge sort by age (first 10) ---" << endl;
	ResidentLinkedList llSortedByAge;
	CopyEntireList(&llData, &llSortedByAge);
	PerformMergeSort(&llSortedByAge, 1);
	llSortedByAge.DisplayList(10);

	// Step 5: hypothesis 1 - searching experiment
	RunSearchingExperiment(&llData);

	// Step 6: hypothesis 2 - csv loading time (linked list side only)
	MeasureLoadingTime("dataset1-cityA.csv");

	// Step 7: hypothesis 3 - memory usage (linked list side only)
	MeasureMemoryUsage(&llData);

	// Step 8: hypothesis 4 - linear search comparison (linked list side only)
	MeasureLinearSearchTime(&llData);

	// Step 9: hypothesis 5 - scalability test (linked list side only)
	MeasureScalability("dataset1-cityA.csv");

	// Step 10: personal hypothesis 6 - merge sort stability
	TestMergeSortStability(&llData);

	// Step 11: personal hypothesis 7 - exponential search position sensitivity
	TestExponentialSearchPosition(&llData);

	// Step 12: personal hypothesis 8 - sort field impact on time
	TestSortFieldImpact(&llData);

	// Step 13: personal hypothesis 9 - getNodeAt pointer traversal cost
	TestGetNodeAtCost(&llData);

	// Step 14: performance summary - consolidated timing and memory results
	PerformanceSummary(&llData);

	// Step 15: insights and recommendations based on emission data
	InsightsAndRecommendations(&llData);

	// ════════════════════════════════════════════════════════════
	//  ARRAY SECTION
	// ════════════════════════════════════════════════════════════

	cout << "\n========================================" << endl;
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

	// Run all hypothesis tests first
	data->runAllTests("dataset1-cityA.csv");

	// Summary tables & analysis
	cout << "\n========================================" << endl;
	cout << "  Summary Tables & Analysis" << endl;
	cout << "========================================" << endl;
	cout << "\nLoaded " << data->getSize() << " records.\n" << endl;
	data->showRecords(data->getSize());

	// Age breakdown
	cout << "\n== Age Group Categorization ==" << endl;
	data->ageGroupReport();

	// Emission analysis
	cout << "\n== Carbon Emission Analysis ==" << endl;
	data->emissionReport();

	// Sorting
	cout << "\n== Sorting ==" << endl;
	ResidentArray s1;
	data->copyTo(s1);
	s1.sortBy(BY_AGE);
	s1.showRecords(s1.getSize());

	ResidentArray s2;
	data->copyTo(s2);
	s2.sortBy(BY_DIST);
	s2.showRecords(s2.getSize());

	ResidentArray s3;
	data->copyTo(s3);
	s3.sortBy(BY_EMISSION);
	s3.showRecords(s3.getSize());

	// Searching
	cout << "\n== Searching ==" << endl;
	for (int g = 0; g < NUM_GROUPS; g++)
		data->searchAgeGroup(g);
	for (int m = 0; m < NUM_MODES; m++)
		data->searchTransport(m);
	data->searchDistance(15.0);
	data->searchDistance(20.0);

	// Insights
	cout << "\n== Insights & Recommendations ==" << endl;
	data->insights();

	// Performance summary
	PerfLogger::printSummary();

	// Cleanup
	delete data;

	cout << "========================================" << endl;
	cout << "  Done." << endl;
	cout << "========================================" << endl;
	return 0;
}
