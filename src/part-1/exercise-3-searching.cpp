#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <algorithm>

#define max 111438

using namespace std;

struct registrations {
    string Direction, Date, Weekday, Country, Commodity, Transport_Mode, Measure;
    unsigned int Year;
    unsigned long long Value, Cumulative;
} reg[max];

int Convert(string date1) {
    int day = 0, month = 0, year = 0;

    if (sscanf(date1.c_str(), "%d/%d/%d", &day, &month, &year) != 3)
        return -1;

    /*
     * Convert day/month/year into yyyymmdd so dates can be compared
     * as simple integers.
     */
    return year * 10000 + month * 100 + day;
}

bool compareDates(string date1, string date2) {
    return Convert(date1) < Convert(date2);
}

void QuickSort(int left, int right) {
    int l, r;
    string pivot;

    l = left;
    r = right;
    pivot = reg[(l + r) / 2].Date;

    /*
     * The records are sorted by Date before applying Binary Search
     * or Interpolation Search.
     */
    do {
        while (compareDates(pivot, reg[r].Date))
            --r;

        while (compareDates(reg[l].Date, pivot))
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

int binarySearch(int left, int right, string tar) {
    int target = Convert(tar);

    while (left <= right) {
        int middle = (left + right) / 2;
        int currentDate = Convert(reg[middle].Date);

        if (currentDate == target)
            return middle;
        else if (target < currentDate)
            right = middle - 1;
        else
            left = middle + 1;
    }

    return -1;
}

int interpolationSearch(int size, const string& tar) {
    int target = Convert(tar);
    int low = 0;
    int high = size - 1;

    /*
     * Interpolation Search estimates the possible position of the target
     * date inside the sorted date range.
     */
    while (low <= high) {
        int lowDate = Convert(reg[low].Date);
        int highDate = Convert(reg[high].Date);

        if (target < lowDate || target > highDate)
            return -1;

        if (lowDate == highDate) {
            if (lowDate == target)
                return low;
            return -1;
        }

        int pos = low + static_cast<int>(
            ((long long)(target - lowDate) * (high - low)) / (highDate - lowDate)
        );

        if (pos < low)
            pos = low;

        if (pos > high)
            pos = high;

        int posDate = Convert(reg[pos].Date);

        if (posDate == target)
            return pos;

        if (posDate < target)
            low = pos + 1;
        else
            high = pos - 1;
    }

    return -1;
}

int findFirstDateMatch(int index, int target) {
    /*
     * The dataset may contain many records with the same date.
     * Move backwards to show the first matching record in sorted order.
     */
    while (index > 0 && Convert(reg[index - 1].Date) == target) {
        index--;
    }

    return index;
}

void printRecord(int index) {
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

    cout << setw(10) << reg[index].Direction
         << setw(15) << reg[index].Year
         << setw(15) << reg[index].Date
         << setw(15) << reg[index].Weekday
         << setw(35) << reg[index].Country
         << setw(40) << reg[index].Commodity
         << setw(25) << reg[index].Transport_Mode
         << setw(15) << reg[index].Measure
         << setw(15) << reg[index].Value
         << setw(15) << reg[index].Cumulative << endl;
}

int main() {
    int lines = 0, choice = -1, loc = -1;
    char ch;
    string temp, com, x;

    ifstream file;
    file.open("data/covid-19-trade-data.csv");

    if (!file.is_open()) {
        cout << "Error: could not open data/covid-19-trade-data.csv" << endl;
        return 1;
    }

    getline(file, temp);

    while (lines < max) {
        if (!getline(file, reg[lines].Direction, ','))
            break;

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

            while (ch != '"' && file) {
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

    QuickSort(0, lines - 1);

    cout << "Total records loaded: " << lines << endl;
    cout << "Data sorted by date before searching." << endl;

    cout << "Choose one of the following search algorithms by typing its number:" << endl
         << "1. BinarySearch" << endl
         << "2. InterpolationSearch" << endl;

    cin >> choice;

    while (choice != 1 && choice != 2) {
        cout << "Wrong input, try again: ";
        cin >> choice;
    }

    cout << "Give a date to search, for example 1/1/2020: ";
    cin >> x;

    int targetDate = Convert(x);

    if (targetDate == -1) {
        cout << "Invalid date format. Please use day/month/year, for example 1/1/2020." << endl;
        return 1;
    }

    if (choice == 1)
        loc = binarySearch(0, lines - 1, x);

    if (choice == 2)
        loc = interpolationSearch(lines, x);

    if (loc == -1) {
        cout << "The date you gave was not found." << endl;
    } else {
        loc = findFirstDateMatch(loc, targetDate);

        cout << "\nA matching record was found for date " << reg[loc].Date << "." << endl;
        cout << "Note: the dataset may contain multiple records with the same date." << endl;
        cout << "The first matching record in the sorted dataset is shown below." << endl;

        printRecord(loc);
    }

    file.close();
    return 0;
}