# Algorithmic Analysis of COVID-19 Trade Data

## Overview

This repository contains a completed university programming project focused on the application of classic data structures and algorithms to a real-world trade dataset related to the economic effects of COVID-19.

The project implements and compares sorting, searching, binary search tree, and hashing techniques using C and C++. It was originally developed as part of the **Data Structures** course at the **Department of Computer Engineering and Informatics, University of Patras**, and was later cleaned, corrected, and reorganized for GitHub portfolio presentation.

---

## Project Information

* **Course:** Data Structures
* **University:** University of Patras
* **Department:** Computer Engineering and Informatics
* **Project Type:** Programming Project / Algorithmic Data Analysis
* **Year:** 2023
* **Programming Languages:** C, C++

---

## Author

* Antreas Kerkidis

---

## Project Objectives

The objective of this project was to apply fundamental data structures and algorithms to process, sort, search, and organize records from a real dataset.

The project focuses on:

* Sorting records using different algorithmic approaches
* Searching records efficiently after preprocessing and sorting
* Building and querying binary search trees
* Implementing hash tables with chaining
* Processing structured CSV data in C and C++
* Comparing algorithmic behavior on a large dataset

---

## Algorithms and Data Structures Implemented

The repository includes implementations of the following algorithms and data structures:

* Counting Sort
* Merge Sort
* Heap Sort
* Quick Sort
* Binary Search
* Interpolation Search
* Binary Interpolation Search
* Balanced Binary Search Tree based on date
* Balanced Binary Search Tree based on value
* Hash Table with chaining

---

## Dataset

The dataset used in this project is stored in the `data` folder:

```text
covid-19-trade-data.csv
```

The dataset contains **111,438 trade records** and includes fields such as:

* Direction
* Year
* Date
* Weekday
* Country
* Commodity
* Transport Mode
* Measure
* Value
* Cumulative

Dataset attribution:

> Stats NZ, “Effects of COVID-19 on trade”, licensed under Creative Commons Attribution 4.0 International.

---

## Repository Structure

```text
.
├── data/
│   └── covid-19-trade-data.csv
│
├── src/
│   ├── part-1/
│   │   ├── exercise-1-counting-merge-sort.cpp
│   │   ├── exercise-2-heap-quick-sort.cpp
│   │   ├── exercise-3-searching.cpp
│   │   └── exercise-4-binary-interpolation-search.cpp
│   │
│   └── part-2/
│       └── bst-and-hashing.c
│
├── .gitignore
└── README.md
```

---

## Repository Contents

### Part 1 — Sorting and Searching

The `src/part-1` folder contains four C++ programs.

#### Exercise 1 — Counting Sort and Merge Sort

Implements sorting by the `Value` field.

Counting Sort is included conceptually, but the program prevents it from running on the full dataset when the value range is too large, because the required count array would consume excessive memory. Merge Sort runs normally on the dataset.

#### Exercise 2 — Heap Sort and Quick Sort

Implements Heap Sort and Quick Sort based on the `Cumulative` field.

The corrected implementation swaps full records, ensuring that all fields remain correctly associated after sorting.

#### Exercise 3 — Binary Search and Interpolation Search

Sorts the dataset by date and searches for a user-provided date using Binary Search or Interpolation Search.

Because the dataset contains multiple records with the same date, the program returns the first matching record in the sorted dataset.

#### Exercise 4 — Binary Interpolation Search

Implements a hybrid Binary Interpolation Search approach for searching records by date.

The dataset is sorted before searching, and duplicate dates are handled consistently.

### Part 2 — Binary Search Trees and Hashing

The `src/part-2` folder contains a C program that implements:

* A balanced Binary Search Tree based on date
* A balanced Binary Search Tree based on value
* A hash table based on date

The program supports basic operations such as printing records, searching, updating values, and deleting records.

---

## How to Compile and Run

All commands should be executed from the main repository folder.

### Exercise 1

```bash
g++ -std=c++17 -Wall -Wextra src/part-1/exercise-1-counting-merge-sort.cpp -o exercise-1.exe
```

Windows PowerShell:

```powershell
.\exercise-1.exe
```

Linux/macOS:

```bash
./exercise-1.exe
```

### Exercise 2

```bash
g++ -std=c++17 -Wall -Wextra src/part-1/exercise-2-heap-quick-sort.cpp -o exercise-2.exe
```

Windows PowerShell:

```powershell
.\exercise-2.exe
```

Linux/macOS:

```bash
./exercise-2.exe
```

### Exercise 3

```bash
g++ -std=c++17 -Wall -Wextra src/part-1/exercise-3-searching.cpp -o exercise-3.exe
```

Windows PowerShell:

```powershell
.\exercise-3.exe
```

Linux/macOS:

```bash
./exercise-3.exe
```

### Exercise 4

```bash
g++ -std=c++17 -Wall -Wextra src/part-1/exercise-4-binary-interpolation-search.cpp -o exercise-4.exe
```

Windows PowerShell:

```powershell
.\exercise-4.exe
```

Linux/macOS:

```bash
./exercise-4.exe
```

### Part 2

```bash
gcc -std=c11 -Wall -Wextra src/part-2/bst-and-hashing.c -o part-2.exe
```

Windows PowerShell:

```powershell
.\part-2.exe
```

Linux/macOS:

```bash
./part-2.exe
```

---

## Example Inputs

For the search-based programs, an example existing date is:

```text
1/1/2020
```

An example non-existing date is:

```text
1/1/2030
```

For the hash table implementation in Part 2, an example number of buckets is:

```text
100
```

---

## Important Note

This repository preserves the educational purpose of the original coursework while improving the structure, documentation, and reliability of the implementation.

The algorithms are implemented manually for learning purposes. The project is not intended to serve as a production-ready data processing system.

The dataset contains multiple records with the same date and many records with duplicate values. For that reason, search operations return one matching record rather than every possible matching record.

Counting Sort is included as part of the coursework topic, but it is not practical for the full dataset because of the large range of values.

---

## Technologies Used

* C
* C++
* GCC / G++
* CSV file processing
* Data structures and algorithms

---

## Copyright

© 2026 Antreas Kerkidis.

This repository is published for educational and portfolio purposes. The source code remains the intellectual property of the author. The dataset remains the property of its original provider and is used according to its stated license.
