#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

#define max 111438

using namespace std;

struct registrations {
    string Direction, Date, Weekday, Country, Commodity, Transport_Mode, Measure;
    unsigned int Year;
    unsigned long long Value, Cumulative;
} reg[max];

void heapify(int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && reg[l].Cumulative > reg[largest].Cumulative)
        largest = l;

    if (r < n && reg[r].Cumulative > reg[largest].Cumulative)
        largest = r;

    if (largest != i) {
        swap(reg[i], reg[largest]);
        heapify(n, largest);
    }
}

void heapSort(int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(n, i);

    for (int i = n - 1; i >= 0; i--) {
        swap(reg[0], reg[i]);
        heapify(i, 0);
    }
}

void QuickSort(int left, int right) {
    int l, r;
    unsigned long long pivot;

    l = left;
    r = right;
    pivot = reg[(l + r) / 2].Cumulative;

    do {
        while (reg[r].Cumulative > pivot)
            --r;

        while (reg[l].Cumulative < pivot)
            ++l;

        if (l <= r) {
            swap(reg[l], reg[r]);
            ++l;
            --r;
        }
    } while (r >= l);

    if (left < r)
        QuickSort(left, r);

    if (l < right)
        QuickSort(l, right);
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

    getline(file, temp); // Skip header

    while (lines < max) {
        if (!getline(file, reg[lines].Direction, ','))
            break;

        getline(file, temp, ',');
        reg[lines].Year = static_cast<unsigned int>(stoul(temp));

        getline(file, reg[lines].Date, ',');
        getline(file, reg[lines].Weekday, ',');
        getline(file, reg[lines].Country, ',');

        file.get(ch);
        com = ch;

        if (ch == '"') {
            file.get(ch);
            while (ch != '"' && file) {
                com = com + ch;
                file.get(ch);
            }
            com = com + ch;
            file.get(ch); // consume comma after closing quote
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

    cout << "Choose one of the following sorts by typing its number:" << endl
         << "1. HeapSort" << endl
         << "2. QuickSort" << endl;

    cin >> choice;

    while (choice != 1 && choice != 2) {
        cout << "Wrong input try again: ";
        cin >> choice;
    }

    if (choice == 1) {
        heapSort(lines);
    }

    if (choice == 2) {
        QuickSort(0, lines - 1);
    }

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