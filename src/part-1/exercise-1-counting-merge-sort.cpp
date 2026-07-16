#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#define max 111438
#define COUNTING_SORT_LIMIT 10000000

using namespace std;

struct registrations {
    string Direction, Date, Weekday, Country, Commodity, Transport_Mode, Measure;
    unsigned int Year;
    unsigned long long Value, Cumulative;
} reg[max];

void countingSort(int size) {
    unsigned long long maxVal = 0;

    for (int i = 0; i < size; i++) {
        if (reg[i].Value > maxVal)
            maxVal = reg[i].Value;
    }

    /*
     * Counting Sort is only practical when the key range is small.
     * The Value field can be very large in this dataset, so the count
     * array may require too much memory.
     */
    if (maxVal > COUNTING_SORT_LIMIT) {
        cout << "Counting Sort is not practical for this dataset." << endl;
        cout << "Maximum Value found: " << maxVal << endl;
        cout << "Counting Sort would require an extremely large count array." << endl;
        return;
    }

    long* count = new long[maxVal + 1]();
    struct registrations* output = new struct registrations[size];

    for (int i = 0; i < size; i++)
        count[reg[i].Value]++;

    for (unsigned long long int i = 1; i <= maxVal; i++)
        count[i] += count[i - 1];

    for (int i = size - 1; i >= 0; i--) {
        output[count[reg[i].Value] - 1] = reg[i];
        count[reg[i].Value]--;
    }

    for (int i = 0; i < size; i++)
        reg[i] = output[i];

    delete[] count;
    delete[] output;
}

void merge(struct registrations reg[], int const left, int const mid, int const right) {
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    struct registrations* leftArray = new struct registrations[subArrayOne];
    struct registrations* rightArray = new struct registrations[subArrayTwo];

    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = reg[left + i];

    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = reg[mid + 1 + j];

    auto indexOfSubArrayOne = 0;
    auto indexOfSubArrayTwo = 0;
    int indexOfMergedArray = left;

    /*
     * Merge Sort orders the records by the Value field.
     */
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne].Value <= rightArray[indexOfSubArrayTwo].Value) {
            reg[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        } else {
            reg[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }

        indexOfMergedArray++;
    }

    while (indexOfSubArrayOne < subArrayOne) {
        reg[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }

    while (indexOfSubArrayTwo < subArrayTwo) {
        reg[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }

    delete[] leftArray;
    delete[] rightArray;
}

void mergeSort(struct registrations reg[], int const begin, int const end) {
    if (begin >= end)
        return;

    auto mid = begin + (end - begin) / 2;

    mergeSort(reg, begin, mid);
    mergeSort(reg, mid + 1, end);
    merge(reg, begin, mid, end);
}

int main() {
    int lines = 0, choice = -1;
    char ch;
    string temp, com;
    ifstream file;

    file.open("data/covid-19-trade-data.csv");

    if (!file.is_open()) {
        cout << "Error: could not open data/covid-19-trade-data.csv" << endl;
        return 1;
    }

    getline(file, temp);

    while (lines < max) {
        getline(file, reg[lines].Direction, ',');
        getline(file, temp, ',');
        reg[lines].Year = static_cast<unsigned int>(stoul(temp));
        getline(file, reg[lines].Date, ',');
        getline(file, reg[lines].Weekday, ',');
        getline(file, reg[lines].Country, ',');

        /*
         * The Commodity field may contain commas inside quotation marks,
         * so it is parsed separately from the other CSV fields.
         */
        file.get(ch);
        com = ch;

        if (ch == '"') {
            file.get(ch);

            while (ch != '"') {
                com = com + ch;
                file.get(ch);
            }

            com = com + ch;
            file.get(ch);
        } else {
            getline(file, temp, ',');
            com = com + temp;
        }

        reg[lines].Commodity = com;
        getline(file, reg[lines].Transport_Mode, ',');
        getline(file, reg[lines].Measure, ',');
        getline(file, temp, ',');
        reg[lines].Value = stoull(temp);
        getline(file, temp);
        reg[lines].Cumulative = stoull(temp);

        lines++;
    }

    cout << "Choose one of the following sorts by typing its number:" << endl;
    cout << "1. CountingSort" << endl;
    cout << "2. MergeSort" << endl;
    cin >> choice;

    while (choice != 1 && choice != 2) {
        cout << "Wrong input, try again: ";
        cin >> choice;
    }

    if (choice == 1) {
        countingSort(lines);
        return 0;
    }

    if (choice == 2)
        mergeSort(reg, 0, lines - 1);

    cout << "\nTotal records loaded: " << lines << endl;
    cout << "Showing first 20 sorted records:" << endl;

    cout << setw(10) << "Direction"
         << setw(15) << "Year"
         << setw(15) << "Date"
         << setw(15) << "Weekday"
         << setw(35) << "Country"
         << setw(40) << "Commodity"
         << setw(25) << "Transport_Mode"
         << setw(15) << "Measure"
         << setw(15) << "Value"
         << setw(15) << "Cumulative" << endl;

    /*
     * Only the first records are printed to keep the terminal output readable.
     */
    int displayLimit = 20;

    if (lines < displayLimit)
        displayLimit = lines;

    for (int i = 0; i < displayLimit; i++) {
        cout << setw(10) << reg[i].Direction
             << setw(15) << reg[i].Year
             << setw(15) << reg[i].Date
             << setw(15) << reg[i].Weekday
             << setw(35) << reg[i].Country
             << setw(40) << reg[i].Commodity
             << setw(25) << reg[i].Transport_Mode
             << setw(15) << reg[i].Measure
             << setw(15) << reg[i].Value
             << setw(15) << reg[i].Cumulative << endl;
    }

    file.close();
    return 0;
}