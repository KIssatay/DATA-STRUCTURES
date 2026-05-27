# Carbon Emission Analysis — Assignment 3

C++ program that analyzes monthly carbon emissions of city residents using two data structures: **Array** and **Singly Linked List**.

## Datasets

| Dataset | City | File |
|---------|------|------|
| DatasetA | City A | `dataset1-cityA.csv` |
| DatasetB | City B | `dataset2-cityB.csv` |
| DatasetC | City C | `dataset3-cityC.csv` |

Each record contains: ResidentID, Age, TransportMode, DailyDistance, EmissionFactor, DaysPerMonth.

## Features

- Load CSV data into an **array** or **linked list**
- Analyze emissions by age group
- Compare sorting performance
- Big O Notation
- Run analysis for all three cities

## Build & Run

### Dataset A
```bash
cd DatasetA
g++ -o cityA_list  main_linkedlist.cpp
g++ -o cityA_array main_array.cpp
./cityA_list
./cityA_array
```

### Dataset B
```bash
cd DatasetB
g++ -o cityB_list  main_linkedlist.cpp
g++ -o cityB_array main_array.cpp
./cityB_list
./cityB_array
```

### Dataset C
```bash
cd DatasetC
g++ -o cityC_list  main_linkedlist.cpp
g++ -o cityC_array main_array.cpp
./cityC_list
./cityC_array
```

## Project Structure

```
├── DatasetA/
│   ├── dataset1-cityA.csv
│   ├── Resident.hpp
│   ├── ArrayFunctions.cpp
│   ├── linkedlist_city.cpp
│   ├── main_array.cpp
│   └── main_linkedlist.cpp
├── DatasetB/
│   ├── dataset2-cityB.csv
│   ├── resident.hpp
│   ├── ResidentArray.cpp
│   ├── residentList.cpp
│   ├── main_array.cpp
│   └── main_linkedlist.cpp
└── DatasetC/
    ├── dataset3-cityC.csv
    ├── resident.hpp
    ├── array_methods.cpp
    ├── list_methods.cpp
    ├── main_array.cpp
    └── main_linkedlist.cpp
```

## Requirements

- C++17 or later
- g++ compiler
