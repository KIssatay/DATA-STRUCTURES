#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "Resident.hpp"

using namespace std;


//Step 1: define the node structure for singly linked list
// i chose singly linked list because the assignment requires comparing
// array vs linked list. each node holds resident data and a pointer
// to the next node. if i used doubly linked list, i would have a prev
// pointer too but that uses more memory and the assignment says singly
class ResidentNode
{
public:
	ResidentData info;
	ResidentNode* nextAddress;
};


//Step 2: define the linked list class with head, tail, and size
// i added tail pointer following the lecture slides part 3 because
// it makes InsertToEndOfList O(1) instead of O(n). without tail
// i would need to walk the entire list every time i add a new node.
// for 200 csv records that would be 200+199+198+... = ~20000 pointer
// follows just for loading data, which is unnecessary
class ResidentLinkedList
{
public:
	ResidentNode* head;
	ResidentNode* tail;
	int size;

	//Step 2.1: constructor sets empty list
	ResidentLinkedList()
	{
		head = nullptr;
		tail = nullptr;
		size = 0;
	}

	//Step 2.2: create a new node and fill it with resident data
	// i made this separate like the lecturer did with CreateNewFlower
	// so i can reuse it in different insert functions if needed
	ResidentNode* createNewNode(ResidentData data)
	{
		ResidentNode* newnode = new ResidentNode;
		newnode->info = data;
		newnode->nextAddress = nullptr;
		return newnode;
	}

	//Step 2.3: insert new node at the end of the list
	// i insert at end to keep the same order as the csv file.
	// if i used InsertToFrontOfList instead, the list would be reversed
	// and the first record in csv would be the last node.
	// that would still work for sorting but would make debugging harder
	// because the print output wouldnt match the csv file order.
	// with tail pointer this is O(1) - just one pointer assignment
	void InsertToEndOfList(ResidentData data)
	{
		//2.3.1 create a new node first using create new node function
		ResidentNode* newnode = createNewNode(data);

		//2.3.2 if list is empty, new node becomes both head and tail
		if (head == nullptr)
		{
			head = newnode;
			tail = newnode;
		}
		else
		{
			//2.3.3 attach new node after current tail, then update tail
			tail->nextAddress = newnode;
			tail = newnode;
		}
		//2.3.4 size increase
		size++;
	}

	//Step 2.4: get total number of nodes in the list
	int getSize()
	{
		return size;
	}

	//Step 2.5: get pointer to node at a given position
	// on an array this would be arr[index] which is O(1), but on linked
	// list every access costs O(n) because we have to follow pointers
	// one by one. this is the main reason why binary search performs
	// badly on linked list - it calls getNodeAt for every midpoint
	ResidentNode* getNodeAt(int index)
	{
		//2.5.1 start from head and walk forward index times
		ResidentNode* current = head;
		for (int i = 0; i < index && current != nullptr; i++)
		{
			current = current->nextAddress;
		}
		return current;
	}

	//Step 2.6: display the list as a formatted table
	void DisplayList(int maxRows = 10)
	{
		//2.6.1 print table header
		cout << left
			 << setw(8) << "ID"
			 << setw(6) << "Age"
			 << setw(12) << "Transport"
			 << setw(10) << "Dist"
			 << setw(10) << "EmFactor"
			 << setw(8) << "Days"
			 << setw(14) << "Emission"
			 << endl;
		cout << string(68, '-') << endl;

		//2.6.2 loop through nodes and print each row
		int row = 0;
		ResidentNode* current = head;
		while (current != nullptr && row < maxRows)
		{
			cout << left
				 << setw(8) << current->info.ResidentID
				 << setw(6) << current->info.Age
				 << setw(12) << current->info.TransportMode
				 << setw(10) << fixed << setprecision(1) << current->info.DailyDistance
				 << setw(10) << fixed << setprecision(2) << current->info.EmissionFactor
				 << setw(8) << current->info.DaysPerMonth
				 << setw(14) << fixed << setprecision(2) << current->info.TotalEmission
				 << endl;
			current = current->nextAddress;
			row++;
		}

		//2.6.3 if the list has more rows, show remaining count
		if (current != nullptr)
		{
			int remaining = 0;
			while (current != nullptr)
			{
				remaining++;
				current = current->nextAddress;
			}
			cout << "... (" << remaining << " more rows)" << endl;
		}
		cout << endl;
	}

	//Step 2.7: delete all nodes and free memory
	void ClearList()
	{
		while (head != nullptr)
		{
			ResidentNode* temp = head;
			head = head->nextAddress;
			delete temp;
		}
		tail = nullptr;
		size = 0;
	}

	//Step 2.8: destructor calls clear automatically when object is destroyed
	~ResidentLinkedList()
	{
		ClearList();
	}
};


//  CSV READER  ---------------------------------------------------

//Step 3: read csv file and build linked list from it
void LoadCSVIntoList(string filepath, ResidentLinkedList* list)
{
	//3.1 open the file
	ifstream file(filepath);
	if (!file.is_open())
	{
		cout << "Cannot open file: " << filepath << endl;
		return;
	}

	//3.2 skip the header line
	string line;
	getline(file, line);

	//3.3 read each line and parse into resident data
	while (getline(file, line))
	{
		//3.3.1 remove carriage return if file is from windows
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}
		if (line.empty()) continue;

		//3.3.2 split line by commas using stringstream
		stringstream ss(line);
		string token;
		ResidentData resident;

		getline(ss, resident.ResidentID, ',');
		getline(ss, token, ','); resident.Age = stoi(token);
		getline(ss, resident.TransportMode, ',');
		getline(ss, token, ','); resident.DailyDistance = stod(token);
		getline(ss, token, ','); resident.EmissionFactor = stod(token);
		getline(ss, token, ','); resident.DaysPerMonth = stoi(token);

		//3.3.3 calculate total monthly emission
		resident.TotalEmission = calculateEmission(
			resident.DailyDistance,
			resident.EmissionFactor,
			resident.DaysPerMonth
		);

		//3.3.4 add resident to the linked list
		list->InsertToEndOfList(resident);
	}
	file.close();
}



//  DEEP COPY  ---------------------------------------------------


//Step 4: create an independent copy of a linked list
// i need to copy the list before each sorting experiment because
// merge sort changes the pointer connections between nodes.
// if i sort the original, the next experiment would receive
// already sorted data and the timing results would be wrong
void CopyEntireList(ResidentLinkedList* source, ResidentLinkedList* destination)
{
	//4.1 clear destination if it has old data
	destination->ClearList();

	//4.2 walk through source and copy each node
	ResidentNode* current = source->head;
	while (current != nullptr)
	{
		destination->InsertToEndOfList(current->info);
		current = current->nextAddress;
	}
}


//  COMPARE HELPER  ---------------------------------------------------

//Step 5: check if two nodes need to be swapped for ascending order
// i use int mode to decide which field to compare.
// i considered making three separate compare functions like
// shouldSwapByAge, shouldSwapByDistance, shouldSwapByEmission
// but that would mean i also need three separate sort functions.
// using mode parameter keeps it flexible - one sort function
// can sort by any field just by changing the mode value
bool shouldSwap(ResidentNode* first, ResidentNode* second, int mode)
{
	if (mode == 1) return first->info.Age > second->info.Age;
	if (mode == 2) return first->info.DailyDistance > second->info.DailyDistance;
	return first->info.TotalEmission > second->info.TotalEmission;
}


//  MERGE SORT FOR LINKED LIST  ---------------------------------------------------
// 
// i chose merge sort for linked list based on my hypothesis.
// the key idea is that merge sort on linked list only needs to
// reassign pointers during the merge step - it does not copy
// or shift any data in memory. on an array, merge sort needs
// a temporary array to hold elements during merging which costs
// O(n) extra memory. on linked list that extra memory is zero
// because we just relink existing nodes.
//
// i also considered bubble sort but it would be O(n^2) which is
// much slower. quick sort was another option but it needs to
// access the last element for pivot which costs O(n) on linked
// list every time. merge sort finds the middle using slow/fast
// pointer which is also O(n) but it only happens once per level,
// not repeatedly like quick sort pivot selection.
//
// my prediction: merge sort on linked list should be faster than
// merge sort on array because array merge sort wastes time copying
// elements into temporary arrays, while linked list just moves pointers

//Step 6: split list into two halves by counting nodes
// to split the list i first count how many nodes there are,
// then walk to the middle position and cut the list there.
// this uses two simple loops - first a counting while loop,
// then a for loop to walk to the middle. both are basic
// traversal patterns we learned in class.
// total cost is O(n) because we go through the list twice
ResidentNode* SplitListInHalf(ResidentNode* head)
{
	//6.1 count total number of nodes in this portion of the list
	int count = 0;
	ResidentNode* current = head;
	while (current != nullptr)
	{
		count++;
		current = current->nextAddress;
	}

	//6.2 find the middle position
	int mid = count / 2;

	//6.3 walk to the node just before the middle
	current = head;
	for (int i = 1; i < mid; i++)
	{
		current = current->nextAddress;
	}

	//6.4 cut the list at the middle point
	ResidentNode* secondHalf = current->nextAddress;
	current->nextAddress = nullptr;
	return secondHalf;
}

//Step 7: merge two sorted lists into one by relinking pointers
// this is where linked list really wins over array.
// i create a dummy node on the stack (not with new) so i dont need
// to handle the empty result case separately. then i compare the heads
// of left and right and attach the smaller one. no data copying happens
// here - only the nextAddress pointers get reassigned.
// on array this same merge step would need a temporary array of size n
// to hold the merged result, then copy everything back. that extra
// allocation and copying is what makes array merge sort slower
ResidentNode* MergeTwoSortedLists(ResidentNode* left, ResidentNode* right, int mode)
{
	//7.1 create a dummy node on stack to simplify merging
	ResidentNode dummy;
	dummy.nextAddress = nullptr;
	ResidentNode* tail = &dummy;

	//7.2 compare heads of both lists and pick the smaller one
	while (left != nullptr && right != nullptr)
	{
		if (!shouldSwap(left, right, mode))
		{
			//7.2.1 left node is smaller or equal, attach it to result
			tail->nextAddress = left;
			left = left->nextAddress;
		}
		else
		{
			//7.2.2 right node is smaller, attach it to result
			tail->nextAddress = right;
			right = right->nextAddress;
		}
		tail = tail->nextAddress;
	}

	//7.3 attach whatever nodes are remaining
	if (left != nullptr)
	{
		tail->nextAddress = left;
	}
	else
	{
		tail->nextAddress = right;
	}

	return dummy.nextAddress;
}

//Step 8: recursive merge sort function
// the recursion splits the list in half, sorts each half, then merges.
// base case is when list has 0 or 1 element - already sorted.
// recursion depth is log(n) - for 200 records thats about 8 levels.
// each level processes all n elements during merge.
// total time: O(n log n), space: O(log n) for recursion stack only,
// no extra arrays needed unlike array merge sort which needs O(n)
ResidentNode* MergeSortRecursive(ResidentNode* head, int mode)
{
	//8.1 base case: empty list or single node is already sorted
	if (head == nullptr || head->nextAddress == nullptr)
	{
		return head;
	}

	//8.2 find middle and split into two halves
	ResidentNode* secondHalf = SplitListInHalf(head);

	//8.3 sort the left half
	ResidentNode* leftSorted = MergeSortRecursive(head, mode);

	//8.4 sort the right half
	ResidentNode* rightSorted = MergeSortRecursive(secondHalf, mode);

	//8.5 merge two sorted halves back together
	return MergeTwoSortedLists(leftSorted, rightSorted, mode);
}

//Step 9: wrapper that updates the list head and tail after sorting
void PerformMergeSort(ResidentLinkedList* list, int mode)
{
	//9.1 run recursive merge sort starting from head
	list->head = MergeSortRecursive(list->head, mode);

	//9.2 after sorting, tail pointer is wrong because merge sort
	// rearranged all the pointers. walk to the end to fix tail
	ResidentNode* current = list->head;
	if (current != nullptr)
	{
		while (current->nextAddress != nullptr)
		{
			current = current->nextAddress;
		}
	}
	list->tail = current;
}


//  LINEAR SEARCH (baseline for comparison)  ---------------------------------------------------


//Step 10: search for first resident with matching age
// linear search is my baseline - the simplest possible search.
// it walks through every node one by one. time complexity is O(n).
// i need this to compare against exponential search and show
// whether the more complex algorithm actually gives any benefit
ResidentNode* LinearSearchByAge(ResidentLinkedList* list, int targetAge)
{
	//10.1 walk from head and compare each node
	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		if (current->info.Age == targetAge)
		{
			return current;
		}
		current = current->nextAddress;
	}
	return nullptr;
}

//Step 11: find all residents using a specific transport mode
int LinearSearchByTransport(ResidentLinkedList* list, string mode, ResidentLinkedList* result)
{
	int count = 0;
	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		if (current->info.TransportMode == mode)
		{
			//11.1 copy matching resident into result list
			result->InsertToEndOfList(current->info);
			count++;
		}
		current = current->nextAddress;
	}
	return count;
}

//Step 12: find all residents with distance above a threshold
int LinearSearchByDistanceAbove(ResidentLinkedList* list, double threshold, ResidentLinkedList* result)
{
	int count = 0;
	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		if (current->info.DailyDistance > threshold)
		{
			result->InsertToEndOfList(current->info);
			count++;
		}
		current = current->nextAddress;
	}
	return count;
}



//  EXPONENTIAL SEARCH FOR LINKED LIST  ---------------------------------------------------

// i chose exponential search because my hypothesis says it is
// more list-friendly than binary search. here is my reasoning:
//
// binary search on array is O(log n) because arr[mid] is O(1).
// but on linked list, reaching the midpoint requires walking
// from head which is O(n). so binary search on linked list
// becomes O(n log n) - worse than linear search O(n).
//
// exponential search has two phases:
// phase 1 (doubling): check positions 1, 2, 4, 8, 16, 32...
//   this is like a forward pointer walk which is natural for
//   linked list - we just follow nextAddress pointers forward.
//   if the target is near the beginning (like age 27 in our data),
//   the doubling stops early and we only searched a small portion.
//
// phase 2 (bounded binary search): once we find the range where
//   the target could be, we do binary search only within that range,
//   not the whole list. the range is smaller so getNodeAt costs less.
//
// my prediction: for targets near the start of sorted list,
// exponential search should be faster than linear because it
// narrows the range quickly. for targets near the end it will
// be similar to linear search because the doubling phase has
// to go through most of the list anyway.

//Step 13: binary search within a bounded range (helper for exponential search)
ResidentNode* BinarySearchInRange(ResidentLinkedList* list, int low, int high, int targetAge)
{
	while (low <= high)
	{
		//13.1 find the middle index
		int mid = low + (high - low) / 2;

		//13.2 walk to middle node (this costs O(n) on linked list)
		// this getNodeAt call is why binary search is slow on linked list.
		// each call walks from head to position mid, costing O(mid) time.
		// on array this would be just arr[mid] which is O(1)
		ResidentNode* midNode = list->getNodeAt(mid);
		if (midNode == nullptr)
		{
			return nullptr;
		}

		//13.3 compare and narrow the range
		if (midNode->info.Age == targetAge)
		{
			return midNode;
		}
		else if (midNode->info.Age < targetAge)
		{
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}
	return nullptr;
}

//Step 14: exponential search with doubling phase then bounded binary search
ResidentNode* ExponentialSearchByAge(ResidentLinkedList* list, int targetAge)
{
	//14.1 check if list is empty
	if (list->head == nullptr)
	{
		return nullptr;
	}

	//14.2 check the first element
	if (list->head->info.Age == targetAge)
	{
		return list->head;
	}

	int totalSize = list->size;
	int bound = 1;

	//14.3 doubling phase: walk forward 1, 2, 4, 8... positions
	// this is the part that works well on linked list.
	// we walk forward through the list doubling our position each time.
	// if target age is 27 and list starts at 26, the doubling stops
	// at bound=2 and we only search positions 1 to 2 instead of all 200
	ResidentNode* walker = list->head->nextAddress;
	while (bound < totalSize && walker != nullptr && walker->info.Age <= targetAge)
	{
		bound *= 2;
		//14.3.1 jump to the new bound position
		int jumpTo = (bound < totalSize) ? bound : totalSize - 1;
		walker = list->getNodeAt(jumpTo);
	}

	//14.4 narrow down the search range
	int low = bound / 2;
	int high = (bound < totalSize) ? bound : totalSize - 1;

	//14.5 do binary search only within the found range
	// instead of searching the entire list [0, 199]
	return BinarySearchInRange(list, low, high, targetAge);
}


//  AGE GROUP ANALYSIS  ---------------------------------------------------


//Step 15: analyze residents by age group and transport mode
// the assignment requires categorizing residents into 5 age groups
// and for each group finding: most preferred transport, total emission,
// average emission per resident. i use fixed arrays of size 5 and 4
// because the number of groups and transport types is known and fixed.
// i considered using a linked list for counters too but arrays are
// simpler when the size is known in advance
void AnalyzeAgeGroups(ResidentLinkedList* list)
{
	//15.1 define group names matching the assignment requirement
	string groupNames[5] = {
		"Children & Teenagers (6-17)",
		"University Students (18-25)",
		"Working Adults Early (26-45)",
		"Working Adults Late (46-60)",
		"Senior Citizens (61-100)"
	};

	//15.2 create arrays to count residents and track emissions
	int groupCount[5] = {0};
	double groupEmission[5] = {0.0};
	string transportNames[4] = {"Car", "Bus", "Bicycle", "Walking"};
	int transportCount[5][4] = {{0}};
	double transportEmission[5][4] = {{0.0}};

	//15.3 walk through entire list and fill the counters - single pass O(n)
	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		int groupIndex = getAgeGroupIndex(current->info.Age);
		if (groupIndex >= 0)
		{
			groupCount[groupIndex]++;
			groupEmission[groupIndex] += current->info.TotalEmission;

			//15.3.1 find which transport mode this resident uses
			for (int t = 0; t < 4; t++)
			{
				if (current->info.TransportMode == transportNames[t])
				{
					transportCount[groupIndex][t]++;
					transportEmission[groupIndex][t] += current->info.TotalEmission;
					break;
				}
			}
		}
		current = current->nextAddress;
	}

	//15.4 print results for each age group
	cout << "\nAge group analysis\n";
	cout << "----------------------------------------\n";

	for (int g = 0; g < 5; g++)
	{
		if (groupCount[g] == 0)
		{
			cout << "\nAge group: " << groupNames[g] << endl;
			cout << "  No residents in this group" << endl;
			continue;
		}

		cout << "\nAge group: " << groupNames[g] << endl;
		cout << string(65, '-') << endl;
		cout << left << setw(15) << "Transport"
			 << setw(10) << "Count"
			 << setw(22) << "Total emission(kgCO2)"
			 << setw(18) << "Avg per resident" << endl;
		cout << string(65, '-') << endl;

		//15.4.1 find the most preferred transport in this group
		int mostPreferred = 0;
		for (int t = 1; t < 4; t++)
		{
			if (transportCount[g][t] > transportCount[g][mostPreferred])
			{
				mostPreferred = t;
			}
		}

		//15.4.2 print each transport row
		for (int t = 0; t < 4; t++)
		{
			if (transportCount[g][t] == 0) continue;
			double average = transportEmission[g][t] / transportCount[g][t];
			cout << left << setw(15) << transportNames[t]
				 << setw(10) << transportCount[g][t]
				 << setw(22) << fixed << setprecision(2) << transportEmission[g][t]
				 << setw(18) << fixed << setprecision(2) << average << endl;
		}

		cout << string(65, '-') << endl;
		cout << "Total emission: " << fixed << setprecision(2) << groupEmission[g] << " kg CO2" << endl;
		cout << "Average per кesident: " << fixed << setprecision(2) << groupEmission[g] / groupCount[g] << " kg CO2" << endl;
		cout << "Most preferred Transport: " << transportNames[mostPreferred]
			 << " (" << transportCount[g][mostPreferred] << ")" << endl;
	}
}



//  CARBON EMISSION ANALYSIS ---------------------------------------------------


//Step 16: analyze total emissions by transport mode
void AnalyzeEmissions(ResidentLinkedList* list)
{
	string transportNames[4] = {"Car", "Bus", "Bicycle", "Walking"};
	int transportCount[4] = {0};
	double transportEmission[4] = {0.0};
	double totalEmissionAll = 0.0;

	//16.1 walk through list and sum up emissions per transport
	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		for (int t = 0; t < 4; t++)
		{
			if (current->info.TransportMode == transportNames[t])
			{
				transportCount[t]++;
				transportEmission[t] += current->info.TotalEmission;
				break;
			}
		}
		totalEmissionAll += current->info.TotalEmission;
		current = current->nextAddress;
	}

	//16.2 print the emission table
	cout << "\nCarbon emission by transport\n";
	cout << "----------------------------------------\n";
	cout << left << setw(15) << "Transport"
		 << setw(10) << "Count"
		 << setw(22) << "Total Emission(kgCO2)"
		 << setw(18) << "Avg per Resident" << endl;
	cout << string(65, '-') << endl;

	for (int t = 0; t < 4; t++)
	{
		double average = (transportCount[t] > 0) ? transportEmission[t] / transportCount[t] : 0;
		cout << left << setw(15) << transportNames[t]
			 << setw(10) << transportCount[t]
			 << setw(22) << fixed << setprecision(2) << transportEmission[t]
			 << setw(18) << fixed << setprecision(2) << average << endl;
	}
	cout << string(65, '-') << endl;
	cout << "Total Emission (City A): " << fixed << setprecision(2) << totalEmissionAll << " kg CO2" << endl;
	cout << "Total Residents: " << list->size << endl;
	cout << "Average per Resident: " << fixed << setprecision(2) << totalEmissionAll / list->size << " kg CO2" << endl;
}


//  SORTING EXPERIMENT  ---------------------------------------------------
// 

//Step 17: run merge sort on all three fields and measure time
// i run merge sort three times - once per field (age, distance, emission).
// each time i make a fresh copy because sorting changes the node order.
// the results will be compared with my teammates array merge sort
// to test our hypothesis that linked list merge sort is faster
// because it avoids the temporary array allocation
void RunSortingExperiment(ResidentLinkedList* original)
{
	cout << "\nSorting experiment - merge sort\n";
	cout << "----------------------------------------\n";
	cout << left << setw(22) << "Algorithm"
		 << setw(18) << "Sort By"
		 << setw(18) << "Time (microsec)" << endl;
	cout << string(58, '-') << endl;

	string fieldNames[3] = {"Age", "Distance", "Emission"};

	for (int field = 1; field <= 3; field++)
	{
		//17.1 make a fresh copy so original stays unchanged
		ResidentLinkedList copy;
		CopyEntireList(original, &copy);

		//17.2 start the timer
		auto startTime = chrono::high_resolution_clock::now();

		//17.3 run merge sort on the copy
		PerformMergeSort(&copy, field);

		//17.4 stop the timer and calculate duration
		auto endTime = chrono::high_resolution_clock::now();
		long long duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();

		cout << left << setw(22) << "Merge Sort"
			 << setw(18) << fieldNames[field - 1]
			 << setw(18) << duration << endl;
	}
}


//  SEARCHING EXPERIMENT  ---------------------------------------------------

//Step 18: run search algorithms and measure time
// i compare linear search vs exponential search on sorted data.
// linear search is my baseline - O(n), simple, works on any list.
// exponential search is my hypothesis algorithm - should perform
// better when the target is in the first portion of the sorted list.
// i also test searching by transport and distance using linear search
// because those are multi-result searches where exponential does not apply
void RunSearchingExperiment(ResidentLinkedList* original)
{
	cout << "\nSearching experiment\n";
	cout << "----------------------------------------\n";

	//18.1 sort by age first because exponential search needs sorted data
	ResidentLinkedList sortedList;
	CopyEntireList(original, &sortedList);
	PerformMergeSort(&sortedList, 1);

	int targetAge = 35;

	cout << "\n--- search for age = " << targetAge << " (sorted list) ---" << endl;
	cout << left << setw(28) << "Algorithm"
		 << setw(18) << "Time (microsec)"
		 << setw(15) << "Found?" << endl;
	cout << string(61, '-') << endl;

	//18.2 linear search as baseline
	auto startTime1 = chrono::high_resolution_clock::now();
	ResidentNode* result1 = LinearSearchByAge(&sortedList, targetAge);
	auto endTime1 = chrono::high_resolution_clock::now();
	long long duration1 = chrono::duration_cast<chrono::microseconds>(endTime1 - startTime1).count();
	cout << left << setw(28) << "Linear Search"
		 << setw(18) << duration1
		 << setw(15) << (result1 ? result1->info.ResidentID : "Not found") << endl;

	//18.3 exponential search (our hypothesis algorithm)
	auto startTime2 = chrono::high_resolution_clock::now();
	ResidentNode* result2 = ExponentialSearchByAge(&sortedList, targetAge);
	auto endTime2 = chrono::high_resolution_clock::now();
	long long duration2 = chrono::duration_cast<chrono::microseconds>(endTime2 - startTime2).count();
	cout << left << setw(28) << "Exponential Search (LL)"
		 << setw(18) << duration2
		 << setw(15) << (result2 ? result2->info.ResidentID : "Not found") << endl;

	//18.4 search all residents by transport mode
	// these always use linear because we need to find ALL matching
	// residents, not just one - exponential search finds only one
	cout << "\n--- search transport = 'Car' (linear) ---" << endl;
	ResidentLinkedList carResults;
	auto startTime3 = chrono::high_resolution_clock::now();
	int carCount = LinearSearchByTransport(original, "Car", &carResults);
	auto endTime3 = chrono::high_resolution_clock::now();
	long long duration3 = chrono::duration_cast<chrono::microseconds>(endTime3 - startTime3).count();
	cout << "Found " << carCount << " residents | Time: " << duration3 << " microsec" << endl;

	//18.5 search all residents by distance threshold
	cout << "\n--- search distance > 15.0 km (linear) ---" << endl;
	ResidentLinkedList farResults;
	auto startTime4 = chrono::high_resolution_clock::now();
	int farCount = LinearSearchByDistanceAbove(original, 15.0, &farResults);
	auto endTime4 = chrono::high_resolution_clock::now();
	long long duration4 = chrono::duration_cast<chrono::microseconds>(endTime4 - startTime4).count();
	cout << "Found " << farCount << " residents | Time: " << duration4 << " microsec" << endl;
}


//  HYPOTHESIS 2: CSV LOADING TIME
//  linked list side only - teammate will ----------------------------------------------------
//  measure array loading time separately


//Step 19: measure how long it takes to load csv into linked list
// hypothesis: array loading is faster because arr[i] = data is O(1)
// while linked list calls new for every node which accesses the heap.
// even with tail pointer making InsertToEndOfList O(1) for pointer work,
// the memory allocation overhead of calling new 200 times is unavoidable.
// i run the load 50 times and take the average to reduce random noise
// from the operating system and other processes
void MeasureLoadingTime(string filepath)
{
	cout << "\nHypothesis 2: CSV loading time\n";
	cout << "----------------------------------------\n";

	int runs = 50;
	long long totalTime = 0;

	for (int i = 0; i < runs; i++)
	{
		ResidentLinkedList fresh;
		auto startTime = chrono::high_resolution_clock::now();
		LoadCSVIntoList(filepath, &fresh);
		auto endTime = chrono::high_resolution_clock::now();
		totalTime += chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
	}

	long long averageTime = totalTime / runs;

	cout << "Runs: " << runs << endl;
	cout << "Average loading time: " << averageTime << " microseconds" << endl;
	cout << "Records loaded per run: 200" << endl;
	cout << "Each insert calls 'new' once per node (heap allocation)" << endl;
	cout << "Compare this with teammate's array loading time" << endl;
}


//  HYPOTHESIS 3: MEMORY USAGE
//  linked list side only - teammate will  ---------------------------------------------------
//  calculate array memory separately


//Step 20: calculate memory used by linked list
// hypothesis: linked list uses more memory than array because each
// node has an extra nextAddress pointer (8 bytes on 64-bit system).
// for 200 nodes thats 1600 bytes of memory that stores no actual
// resident data - just addresses pointing to other nodes.
// array does not have this overhead because elements sit next to
// each other in contiguous memory with no pointers between them
void MeasureMemoryUsage(ResidentLinkedList* list)
{
	cout << "\nHypothesis 3: Memory usage\n";
	cout << "----------------------------------------\n";

	//20.1 calculate sizes using sizeof
	int dataSize = sizeof(ResidentData);
	int nodeSize = sizeof(ResidentNode);
	int pointerSize = sizeof(ResidentNode*);
	int totalNodes = list->size;

	//20.2 total memory = node size * number of nodes
	int totalMemory = nodeSize * totalNodes;

	//20.3 pointer overhead = pointer size * number of nodes
	// this is the extra memory that linked list uses compared to array
	int pointerOverhead = pointerSize * totalNodes;

	//20.4 pure data = data size * number of nodes
	// this is what array would use - same data, no pointers
	int pureDataMemory = dataSize * totalNodes;

	cout << "Size of ResidentData (pure data): " << dataSize << " bytes" << endl;
	cout << "Size of ResidentNode (data + pointer): " << nodeSize << " bytes" << endl;
	cout << "Size of one pointer (nextAddress): " << pointerSize << " bytes" << endl;
	cout << "Total nodes: " << totalNodes << endl;
	cout << string(50, '-') << endl;
	cout << "Total linked list memory: " << totalMemory << " bytes" << endl;
	cout << "Pointer overhead: " << pointerOverhead << " bytes" << endl;
	cout << "Pure data memory (array equivalent): " << pureDataMemory << " bytes" << endl;
	cout << "Extra memory vs array: " << totalMemory - pureDataMemory << " bytes" << endl;
	cout << "Overhead percentage: " << fixed << setprecision(1)
		 << ((double)(totalMemory - pureDataMemory) / pureDataMemory) * 100 << "%" << endl;
}


//  HYPOTHESIS 4: LINEAR SEARCH TIME
//  linked list side only - teammate will  ---------------------------------------------------
//  measure array linear search separately

//Step 21: measure linear search time for transport mode
// hypothesis: linear search time will be approximately the same on
// array and linked list because both perform exactly 200 string
// comparisons. any small difference comes from cache locality -
// array elements sit next to each other in memory so the cpu can
// preload them, while linked list nodes are scattered across heap.
// i run 100 times and average to get consistent results
void MeasureLinearSearchTime(ResidentLinkedList* list)
{
	cout << "\nHypothesis 4: Linear search time\n";
	cout << "----------------------------------------\n";

	int runs = 100;
	long long totalTime = 0;

	for (int i = 0; i < runs; i++)
	{
		ResidentLinkedList tempResult;
		auto startTime = chrono::high_resolution_clock::now();
		LinearSearchByTransport(list, "Car", &tempResult);
		auto endTime = chrono::high_resolution_clock::now();
		totalTime += chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
	}

	long long averageTime = totalTime / runs;

	cout << "Search target: transport = 'Car'" << endl;
	cout << "Runs: " << runs << endl;
	cout << "Average search time: " << averageTime << " microseconds" << endl;
	cout << "Both structures do exactly 200 string comparisons" << endl;
	cout << "Compare this with teammate's array linear search time" << endl;
}


//  HYPOTHESIS 5: SCALABILITY TEST
//  linked list side only - teammate will  ---------------------------------------------------
//  test array scalability separately
// 

//Step 22: test merge sort with 10x larger dataset
// hypothesis: when we scale from 200 to 2000 records, the speed
// difference between array and linked list merge sort will grow
// because array merge sort allocates temporary arrays during merge
// phase. with 2000 elements these allocations become 10x bigger.
// linked list merge sort does not allocate any extra memory at any
// scale - it only reassigns existing pointers.
// i load the same csv 10 times into one list to simulate 2000 records.
// theoretical scaling for O(n log n): 2000*log2(2000) / 200*log2(200)
// = 2000*11 / 200*7.6 = 22000 / 1520 = ~14.5x
void MeasureScalability(string filepath)
{
	cout << "\nHypothesis 5: Scalability test\n";
	cout << "----------------------------------------\n";

	//22.1 measure merge sort on original 200 records
	ResidentLinkedList small;
	LoadCSVIntoList(filepath, &small);

	ResidentLinkedList smallCopy;
	CopyEntireList(&small, &smallCopy);
	auto s1 = chrono::high_resolution_clock::now();
	PerformMergeSort(&smallCopy, 1);
	auto e1 = chrono::high_resolution_clock::now();
	long long time200 = chrono::duration_cast<chrono::microseconds>(e1 - s1).count();

	//22.2 build 10x larger dataset by loading csv 10 times
	ResidentLinkedList large;
	for (int i = 0; i < 10; i++)
	{
		LoadCSVIntoList(filepath, &large);
	}

	//22.3 measure merge sort on 2000 records
	ResidentLinkedList largeCopy;
	CopyEntireList(&large, &largeCopy);
	auto s2 = chrono::high_resolution_clock::now();
	PerformMergeSort(&largeCopy, 1);
	auto e2 = chrono::high_resolution_clock::now();
	long long time2000 = chrono::duration_cast<chrono::microseconds>(e2 - s2).count();

	cout << "Merge Sort on 200 records: " << time200 << " microseconds" << endl;
	cout << "Merge Sort on 2000 records: " << time2000 << " microseconds" << endl;

	//22.4 avoid division by zero if time200 is 0
	if (time200 > 0)
	{
		cout << "Actual scaling factor: " << fixed << setprecision(1)
			 << (double)time2000 / time200 << "x" << endl;
	}
	else
	{
		cout << "200 records too fast to measure in microseconds" << endl;
		cout << "2000 records time gives a better baseline" << endl;
	}
	cout << "Theoretical O(n log n) scaling: ~14.5x" << endl;
	cout << "Compare this ratio with teammate's array scaling ratio" << endl;
}


//  PERSONAL HYPOTHESIS 6: MERGE SORT STABILITY  --------------------------------------------------- 

//Step 23: test if merge sort preserves original order for equal values
// hypothesis: when sorting by age, residents who share the same age
// (for example age 29 appears 13 times in our dataset) should keep
// their original csv order after sorting. this is called a stable sort.
// it happens naturally in our merge step because at step 7.2.1 we pick
// the left node when values are equal (shouldSwap returns false for equal).
// this means the node that was earlier in the original list stays earlier
// after sorting - no extra code or memory needed for stability
void TestMergeSortStability(ResidentLinkedList* original)
{
	cout << "\nPersonal hypothesis 6: Merge sort stability\n";
	cout << "----------------------------------------\n";

	//23.1 first collect all residents with a specific age from original list
	// i pick age 29 because it has 13 duplicates - good sample
	int testAge = 29;
	cout << "Testing with age = " << testAge << endl;
	cout << "\nOriginal order (before sorting):" << endl;

	ResidentNode* current = original->head;
	int originalCount = 0;
	string originalOrder[20];
	while (current != nullptr)
	{
		if (current->info.Age == testAge)
		{
			cout << "  " << current->info.ResidentID << endl;
			originalOrder[originalCount] = current->info.ResidentID;
			originalCount++;
		}
		current = current->nextAddress;
	}

	//23.2 sort a copy by age and collect the same age group again
	ResidentLinkedList sorted;
	CopyEntireList(original, &sorted);
	PerformMergeSort(&sorted, 1);

	cout << "\nAfter merge sort by age:" << endl;
	current = sorted.head;
	int sortedCount = 0;
	string sortedOrder[20];
	while (current != nullptr)
	{
		if (current->info.Age == testAge)
		{
			cout << "  " << current->info.ResidentID << endl;
			sortedOrder[sortedCount] = current->info.ResidentID;
			sortedCount++;
		}
		current = current->nextAddress;
	}

	//23.3 compare the two orders
	bool isStable = true;
	for (int i = 0; i < originalCount && i < sortedCount; i++)
	{
		if (originalOrder[i] != sortedOrder[i])
		{
			isStable = false;
			break;
		}
	}

	cout << "\nTotal residents with age " << testAge << ": " << originalCount << endl;
	if (isStable)
	{
		cout << "Result: STABLE - original order is preserved after sorting" << endl;
		cout << "This confirms our hypothesis. The merge step picks the left" << endl;
		cout << "node when values are equal, keeping earlier nodes first." << endl;
	}
	else
	{
		cout << "Result: NOT STABLE - order changed after sorting" << endl;
	}
}


//  PERSONAL HYPOTHESIS 7: EXPONENTIAL SEARCH  ---------------------------------------------------
//  POSITION SENSITIVITY

//Step 24: test if exponential search speed depends on target position
// hypothesis: searching for age 27 (near the start of sorted list)
// will be faster than searching for age 55 (near the end) because
// the doubling phase stops early for small targets.
// for age 27: sorted list starts at 26, doubling goes 1,2,4 and stops
//   because position 4 already has age > 27. bounded binary search
//   operates on range [2, 4] which is tiny.
// for age 55: doubling goes 1,2,4,8,16,32,64,128 before passing age 55.
//   bounded binary search operates on range [64, 128] which is much bigger
//   and each getNodeAt call walks 64-128 nodes from head.
void TestExponentialSearchPosition(ResidentLinkedList* original)
{
	cout << "\nPersonal hypothesis 7: Exponential search position\n";
	cout << "----------------------------------------\n";

	//24.1 sort by age first
	ResidentLinkedList sorted;
	CopyEntireList(original, &sorted);
	PerformMergeSort(&sorted, 1);

	int earlyAge = 27;
	int lateAge = 55;
	int runs = 1000;

	//24.2 measure exponential search for early age (near start)
	long long totalEarly = 0;
	for (int i = 0; i < runs; i++)
	{
		auto s = chrono::high_resolution_clock::now();
		ExponentialSearchByAge(&sorted, earlyAge);
		auto e = chrono::high_resolution_clock::now();
		totalEarly += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	//24.3 measure exponential search for late age (near end)
	long long totalLate = 0;
	for (int i = 0; i < runs; i++)
	{
		auto s = chrono::high_resolution_clock::now();
		ExponentialSearchByAge(&sorted, lateAge);
		auto e = chrono::high_resolution_clock::now();
		totalLate += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	long long avgEarly = totalEarly / runs;
	long long avgLate = totalLate / runs;

	cout << "Runs per test: " << runs << endl;
	cout << "Search for age " << earlyAge << " (near start): " << avgEarly << " microseconds avg" << endl;
	cout << "Search for age " << lateAge << " (near end):   " << avgLate << " microseconds avg" << endl;

	if (avgLate > avgEarly && avgEarly > 0)
	{
		cout << "Late target is " << fixed << setprecision(1)
			 << (double)avgLate / avgEarly << "x slower than early target" << endl;
		cout << "This confirms our hypothesis - exponential search is faster" << endl;
		cout << "for targets near the beginning because the doubling phase" << endl;
		cout << "stops early and the bounded range is smaller." << endl;
	}
	else
	{
		cout << "Difference too small to measure at this dataset size" << endl;
	}
}


//  PERSONAL HYPOTHESIS 8: SORT FIELD IMPACT  ---------------------------------------------------
// 

//Step 25: test if merge sort time differs by sort field
// hypothesis: sorting by emission will be slightly different than
// sorting by age because the data distribution is different.
// age has 33 unique values spread across 200 records.
// emission has many duplicates - 80 residents have emission = 0
// (bicycle and walking), and the rest cluster around a few values.
// more duplicates means more equal comparisons in the merge step
// which could affect branching patterns in the cpu
void TestSortFieldImpact(ResidentLinkedList* original)
{
	cout << "\nPersonal hypothesis 8: Sort field impact on time\n";
	cout << "----------------------------------------\n";

	int runs = 200;
	string fieldNames[3] = {"Age", "Distance", "Emission"};
	long long totals[3] = {0, 0, 0};

	//25.1 run each sort field many times and average
	for (int field = 1; field <= 3; field++)
	{
		for (int i = 0; i < runs; i++)
		{
			ResidentLinkedList copy;
			CopyEntireList(original, &copy);

			auto s = chrono::high_resolution_clock::now();
			PerformMergeSort(&copy, field);
			auto e = chrono::high_resolution_clock::now();

			totals[field - 1] += chrono::duration_cast<chrono::microseconds>(e - s).count();
		}
	}

	cout << "Runs per field: " << runs << endl;
	cout << left << setw(15) << "Sort Field"
		 << setw(20) << "Avg Time (microsec)"
		 << setw(20) << "Unique Values" << endl;
	cout << string(55, '-') << endl;

	// age has 33 unique values, distance has ~17, emission has ~25 but 80 are zero
	string uniqueInfo[3] = {"33 unique", "~17 unique", "~25 unique (80 zeros)"};

	for (int f = 0; f < 3; f++)
	{
		cout << left << setw(15) << fieldNames[f]
			 << setw(20) << totals[f] / runs
			 << setw(20) << uniqueInfo[f] << endl;
	}

	cout << "\nThe field with most duplicates may show slightly different" << endl;
	cout << "timing due to how the cpu handles equal vs unequal comparisons" << endl;
}



//  PERSONAL HYPOTHESIS 9: getNodeAt COST  ---------------------------------------------------

//Step 26: count total pointer follows in exponential search vs linear search
// hypothesis: exponential search follows MORE pointers total than linear
// search even though it checks fewer nodes logically. this is because
// each getNodeAt call starts from head and re-walks nodes it already passed.
// linear search for position 100 follows exactly 100 pointers.
// exponential search calls getNodeAt(1), getNodeAt(2), getNodeAt(4),
// getNodeAt(8)... and each one starts from head. so getNodeAt(8) walks
// through nodes 0,1,2,3,4,5,6,7,8 even though nodes 0-3 were already
// visited by earlier getNodeAt calls.
void TestGetNodeAtCost(ResidentLinkedList* original)
{
	cout << "\nPersonal hypothesis 9: getNodeAt pointer traversal cost\n";
	cout << "----------------------------------------\n";

	//26.1 sort by age first
	ResidentLinkedList sorted;
	CopyEntireList(original, &sorted);
	PerformMergeSort(&sorted, 1);

	int targetAge = 35;

	//26.2 count pointer follows for linear search
	// linear search follows exactly one pointer per node checked
	int linearFollows = 0;
	ResidentNode* current = sorted.head;
	while (current != nullptr)
	{
		linearFollows++;
		if (current->info.Age == targetAge)
		{
			break;
		}
		current = current->nextAddress;
	}

	//26.3 simulate exponential search and count all pointer follows
	// including the redundant ones inside getNodeAt
	int exponentialFollows = 0;

	// doubling phase
	int totalSize = sorted.size;
	int bound = 1;
	ResidentNode* walker = sorted.head->nextAddress;
	exponentialFollows++; // head->nextAddress is one follow

	while (bound < totalSize && walker != nullptr && walker->info.Age <= targetAge)
	{
		bound *= 2;
		int jumpTo = (bound < totalSize) ? bound : totalSize - 1;
		// getNodeAt(jumpTo) walks jumpTo pointers from head
		exponentialFollows += jumpTo;
		walker = sorted.getNodeAt(jumpTo);
	}

	// bounded binary search phase
	int low = bound / 2;
	int high = (bound < totalSize) ? bound : totalSize - 1;
	while (low <= high)
	{
		int mid = low + (high - low) / 2;
		// getNodeAt(mid) walks mid pointers from head
		exponentialFollows += mid;
		ResidentNode* midNode = sorted.getNodeAt(mid);
		if (midNode == nullptr) break;
		if (midNode->info.Age == targetAge) break;
		else if (midNode->info.Age < targetAge) low = mid + 1;
		else high = mid - 1;
	}

	cout << "Target age: " << targetAge << endl;
	cout << "Linear search: " << linearFollows << " pointer follows" << endl;
	cout << "Exponential search: " << exponentialFollows << " pointer follows" << endl;

	if (exponentialFollows > linearFollows)
	{
		cout << "Exponential search followed " << fixed << setprecision(1)
			 << (double)exponentialFollows / linearFollows
			 << "x more pointers than linear search" << endl;
		cout << "\nThis confirms our hypothesis. Even though exponential search" << endl;
		cout << "logically checks fewer nodes, each getNodeAt call restarts" << endl;
		cout << "from head and re-walks previously visited nodes." << endl;
		cout << "On array this would not happen because arr[mid] is O(1)." << endl;
	}
	else
	{
		cout << "Exponential search used fewer or equal pointer follows" << endl;
		cout << "This may happen when target is very close to the beginning" << endl;
	}
}


//  PERFORMANCE SUMMARY 
//  consolidated results from all experiments  ---------------------------------------------------
// 

//Step 27: print a single summary table of all timing and memory results
// this collects everything into one place so it is easy to compare
// with the array side and discuss in the documentation
void PerformanceSummary(ResidentLinkedList* original)
{
	cout << "\nPerformance summary\n";
	cout << "----------------------------------------\n";

	//27.1 loading time
	int loadRuns = 50;
	long long loadTotal = 0;
	for (int i = 0; i < loadRuns; i++)
	{
		ResidentLinkedList fresh;
		auto s = chrono::high_resolution_clock::now();
		LoadCSVIntoList("dataset1-cityA.csv", &fresh);
		auto e = chrono::high_resolution_clock::now();
		loadTotal += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	//27.2 merge sort timing for each field
	string fieldNames[3] = {"Age", "Distance", "Emission"};
	long long sortTimes[3] = {0, 0, 0};
	int sortRuns = 100;
	for (int field = 1; field <= 3; field++)
	{
		for (int i = 0; i < sortRuns; i++)
		{
			ResidentLinkedList copy;
			CopyEntireList(original, &copy);
			auto s = chrono::high_resolution_clock::now();
			PerformMergeSort(&copy, field);
			auto e = chrono::high_resolution_clock::now();
			sortTimes[field - 1] += chrono::duration_cast<chrono::microseconds>(e - s).count();
		}
	}

	//27.3 search timing
	ResidentLinkedList sorted;
	CopyEntireList(original, &sorted);
	PerformMergeSort(&sorted, 1);

	int searchRuns = 100;

	long long linearTime = 0;
	for (int i = 0; i < searchRuns; i++)
	{
		auto s = chrono::high_resolution_clock::now();
		LinearSearchByAge(&sorted, 35);
		auto e = chrono::high_resolution_clock::now();
		linearTime += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	long long expTime = 0;
	for (int i = 0; i < searchRuns; i++)
	{
		auto s = chrono::high_resolution_clock::now();
		ExponentialSearchByAge(&sorted, 35);
		auto e = chrono::high_resolution_clock::now();
		expTime += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	long long transportTime = 0;
	for (int i = 0; i < searchRuns; i++)
	{
		ResidentLinkedList temp;
		auto s = chrono::high_resolution_clock::now();
		LinearSearchByTransport(original, "Car", &temp);
		auto e = chrono::high_resolution_clock::now();
		transportTime += chrono::duration_cast<chrono::microseconds>(e - s).count();
	}

	//27.4 memory
	int nodeSize = sizeof(ResidentNode);
	int dataSize = sizeof(ResidentData);
	int pointerSize = sizeof(ResidentNode*);
	int totalMem = nodeSize * original->size;
	int pointerOverhead = pointerSize * original->size;

	//27.5 print consolidated table
	cout << "\n--- TIMING RESULTS (averaged) ---" << endl;
	cout << left << setw(35) << "Operation"
		 << right << setw(15) << "Time (us)"
		 << setw(15) << "Complexity" << endl;
	cout << string(65, '-') << endl;

	cout << left << setw(35) << "CSV Loading (200 records)"
		 << right << setw(15) << loadTotal / loadRuns
		 << setw(15) << "O(n)" << endl;

	for (int f = 0; f < 3; f++)
	{
		string label = "Merge Sort by " + fieldNames[f];
		cout << left << setw(35) << label
			 << right << setw(15) << sortTimes[f] / sortRuns
			 << setw(15) << "O(n log n)" << endl;
	}

	cout << left << setw(35) << "Linear Search (age=35)"
		 << right << setw(15) << linearTime / searchRuns
		 << setw(15) << "O(n)" << endl;

	cout << left << setw(35) << "Exponential Search (age=35)"
		 << right << setw(15) << expTime / searchRuns
		 << setw(15) << "O(n)" << endl;

	cout << left << setw(35) << "Linear Search (transport=Car)"
		 << right << setw(15) << transportTime / searchRuns
		 << setw(15) << "O(n)" << endl;

	cout << "\n--- MEMORY RESULTS ---" << endl;
	cout << left << setw(35) << "Total memory used:"
		 << right << setw(15) << totalMem << " bytes" << endl;
	cout << left << setw(35) << "Pure data (without pointers):"
		 << right << setw(15) << dataSize * original->size << " bytes" << endl;
	cout << left << setw(35) << "Pointer overhead:"
		 << right << setw(15) << pointerOverhead << " bytes" << endl;
	cout << left << setw(35) << "Overhead percentage:"
		 << right << setw(14) << fixed << setprecision(1)
		 << ((double)pointerOverhead / (dataSize * original->size)) * 100 << "%" << endl;

	//27.6 analysis: when does linked list win or lose
	cout << "\n--- WHEN LINKED LIST PERFORMS BETTER ---" << endl;
	cout << "  1. Merge Sort: linked list only reassigns pointers during merge." << endl;
	cout << "     No temporary arrays needed. Space complexity O(log n) vs" << endl;
	cout << "     array merge sort which needs O(n) extra memory for temp arrays." << endl;
	cout << "  2. Insertion/Deletion: adding or removing a node is O(1) once" << endl;
	cout << "     the position is known. Array needs to shift all elements" << endl;
	cout << "     after the deletion point which costs O(n)." << endl;
	cout << "  3. Dynamic size: linked list grows and shrinks as needed." << endl;
	cout << "     Array allocates fixed MAX_SIZE slots even if only 200 are used." << endl;

	cout << "\n--- WHEN LINKED LIST PERFORMS WORSE ---" << endl;
	cout << "  1. Random access: getNodeAt(i) is O(n) because we walk from head." << endl;
	cout << "     Array does arr[i] in O(1). This makes binary search O(n log n)" << endl;
	cout << "     on linked list vs O(log n) on array." << endl;
	cout << "  2. Memory overhead: each node wastes " << pointerSize << " bytes on the nextAddress" << endl;
	cout << "     pointer. For " << original->size << " nodes that is " << pointerOverhead << " extra bytes." << endl;
	cout << "  3. Cache locality: array elements are contiguous in memory so" << endl;
	cout << "     CPU cache can preload them. Linked list nodes are scattered" << endl;
	cout << "     across heap memory causing more cache misses." << endl;
	cout << "  4. Loading time: each node requires a separate heap allocation" << endl;
	cout << "     with 'new' which is slower than writing to a pre-allocated array slot." << endl;
}


//  INSIGHTS AND RECOMMENDATIONS  ---------------------------------------------------

//Step 28: analyze carbon emissions and provide city planning recommendations
// this function uses the data already in the linked list to identify
// which age groups pollute most, which prefer cars vs bicycles,
// and what the city can do about it
void InsightsAndRecommendations(ResidentLinkedList* list)
{
	cout << "\nInsights and recommendations\n";
	cout << "----------------------------------------\n";

	//28.1 build summary table: age group x transport mode
	string groupNames[5] = {
		"Children & Teenagers (6-17)",
		"University Students (18-25)",
		"Working Adults Early (26-45)",
		"Working Adults Late (46-60)",
		"Senior Citizens (61-100)"
	};
	string transportNames[4] = {"Car", "Bus", "Bicycle", "Walking"};

	int groupCount[5] = {0};
	double groupEmission[5] = {0.0};
	int carUsers[5] = {0};
	int bikeUsers[5] = {0};
	int busUsers[5] = {0};
	int walkUsers[5] = {0};
	double totalEmission = 0.0;

	ResidentNode* current = list->head;
	while (current != nullptr)
	{
		int gi = getAgeGroupIndex(current->info.Age);
		if (gi >= 0)
		{
			groupCount[gi]++;
			groupEmission[gi] += current->info.TotalEmission;
			totalEmission += current->info.TotalEmission;

			if (current->info.TransportMode == "Car") carUsers[gi]++;
			else if (current->info.TransportMode == "Bus") busUsers[gi]++;
			else if (current->info.TransportMode == "Bicycle") bikeUsers[gi]++;
			else if (current->info.TransportMode == "Walking") walkUsers[gi]++;
		}
		current = current->nextAddress;
	}

	//28.2 print cross-table: age groups vs transport with emission
	cout << "\n--- EMISSION BY AGE GROUP AND TRANSPORT PREFERENCE ---" << endl;
	cout << left << setw(32) << "Age Group"
		 << right << setw(8) << "Count"
		 << setw(14) << "Total CO2"
		 << setw(12) << "Avg CO2"
		 << setw(8) << "Car"
		 << setw(8) << "Bus"
		 << setw(8) << "Bike"
		 << setw(8) << "Walk" << endl;
	cout << string(98, '-') << endl;

	int highestEmitter = -1;
	double highestEmission = 0;
	int mostCarUsers = -1;
	int mostCarCount = 0;
	int mostBikeUsers = -1;
	int mostBikeCount = 0;

	for (int g = 0; g < 5; g++)
	{
		if (groupCount[g] == 0) continue;

		double avg = groupEmission[g] / groupCount[g];

		cout << left << setw(32) << groupNames[g]
			 << right << setw(8) << groupCount[g]
			 << setw(14) << fixed << setprecision(2) << groupEmission[g]
			 << setw(12) << fixed << setprecision(2) << avg
			 << setw(8) << carUsers[g]
			 << setw(8) << busUsers[g]
			 << setw(8) << bikeUsers[g]
			 << setw(8) << walkUsers[g] << endl;

		if (groupEmission[g] > highestEmission)
		{
			highestEmission = groupEmission[g];
			highestEmitter = g;
		}
		if (carUsers[g] > mostCarCount)
		{
			mostCarCount = carUsers[g];
			mostCarUsers = g;
		}
		if (bikeUsers[g] > mostBikeCount)
		{
			mostBikeCount = bikeUsers[g];
			mostBikeUsers = g;
		}
	}

	cout << string(98, '-') << endl;
	cout << left << setw(32) << "TOTAL"
		 << right << setw(8) << list->size
		 << setw(14) << fixed << setprecision(2) << totalEmission
		 << setw(12) << fixed << setprecision(2) << totalEmission / list->size
		 << endl;

	//28.3 key findings
	cout << "\n--- KEY FINDINGS ---" << endl;

	if (highestEmitter >= 0)
	{
		cout << "  1. Highest emitting group: " << groupNames[highestEmitter]
			 << " with " << fixed << setprecision(2) << highestEmission << " kg CO2 total"
			 << " (avg " << highestEmission / groupCount[highestEmitter] << " per person)" << endl;
	}

	if (mostCarUsers >= 0)
	{
		cout << "  2. Most car users: " << groupNames[mostCarUsers]
			 << " with " << mostCarCount << " car commuters" << endl;
	}

	if (mostBikeUsers >= 0)
	{
		cout << "  3. Most bicycle users: " << groupNames[mostBikeUsers]
			 << " with " << mostBikeCount << " cyclists" << endl;
	}

	// car emission percentage
	double carEmission = 0;
	current = list->head;
	while (current != nullptr)
	{
		if (current->info.TransportMode == "Car")
			carEmission += current->info.TotalEmission;
		current = current->nextAddress;
	}
	cout << "  4. Cars produce " << fixed << setprecision(1)
		 << (carEmission / totalEmission) * 100 << "% of all emissions"
		 << " (" << fixed << setprecision(2) << carEmission << " out of "
		 << totalEmission << " kg CO2)" << endl;

	//28.4 recommendations for city planners
	cout << "\n--- RECOMMENDATIONS FOR CITY PLANNERS ---" << endl;
	cout << "\n  For " << groupNames[2] << " (26-45):" << endl;
	cout << "    - This group has the most car users and highest total emission." << endl;
	cout << "    - Build dedicated cycling lanes on commuter routes to offer a" << endl;
	cout << "      practical alternative for short-medium distances (8-15 km)." << endl;
	cout << "    - Introduce rush-hour congestion pricing to make driving less" << endl;
	cout << "      attractive and fund public transport improvements." << endl;

	cout << "\n  For " << groupNames[3] << " (46-60):" << endl;
	cout << "    - Many in this group walk (short distances) but car users have" << endl;
	cout << "      high individual emissions due to longer distances." << endl;
	cout << "    - Expand bus routes and increase frequency during commute hours" << endl;
	cout << "      so bus becomes a realistic alternative to car." << endl;
	cout << "    - Offer subsidized monthly bus passes for this age bracket." << endl;

	cout << "\n  General recommendations:" << endl;
	cout << "    - E-bike purchase subsidies for commuters traveling 5-15 km daily" << endl;
	cout << "      would shift car users to zero-emission transport." << endl;
	cout << "    - Bicycle and walking already produce zero emissions. The city" << endl;
	cout << "      should protect and expand existing pedestrian and cycling" << endl;
	cout << "      infrastructure to maintain these healthy transport habits." << endl;
	cout << "    - Bus emission factor is 0.05 vs car 0.21 (4.2x lower)." << endl;
	cout << "      Even shifting 20% of car commuters to bus would reduce" << endl;
	cout << "      total city emissions significantly." << endl;
}