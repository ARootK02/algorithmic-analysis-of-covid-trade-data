#include <iostream>
#include <fstream>
#include <sstream>
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

int findFirstDateMatch(int index, int target) {
    while (index > 0 && Convert(reg[index - 1].Date) == target) {
        index--;
    }

    return index;
}

int binaryInterpolationSearch(int size, string targetDate) {
    int target = Convert(targetDate);

    if (target == -1)
        return -1;

    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int leftDate = Convert(reg[left].Date);
        int rightDate = Convert(reg[right].Date);

        if (target < leftDate || target > rightDate)
            return -1;

        int middle = left + (right - left) / 2;
        int middleDate = Convert(reg[middle].Date);

        if (middleDate == target)
            return findFirstDateMatch(middle, target);

        int position = middle;

        if (rightDate != leftDate) {
            position = left + static_cast<int>(
                ((long long)(target - leftDate) * (right - left)) / (rightDate - leftDate)
            );
        }

        if (position < left)
            position = left;

        if (position > right)
            position = right;

        int positionDate = Convert(reg[position].Date);

        if (positionDate == target)
            return findFirstDateMatch(position, target);

        if (target < positionDate) {
            right = position - 1;
        } else {
            left = position + 1;
        }

        /*
            Binary fallback:
            If interpolation gives a poor position, the loop still narrows the
            search interval safely. The middle position is also checked above,
            which makes this a hybrid binary/interpolation search.
        */
    }

    return -1;
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
    int lines = 0;
    int loc = -1;
    char ch;
    string temp, com, x;

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

    QuickSort(0, lines - 1);

    cout << "Total records loaded: " << lines << endl;
    cout << "Data sorted by date before searching." << endl;
    cout << "Binary Interpolation Search" << endl;

    cout << "Give a date to search, for example 1/1/2020: ";
    cin >> x;

    if (Convert(x) == -1) {
        cout << "Invalid date format. Please use day/month/year, for example 1/1/2020." << endl;
        return 1;
    }

    loc = binaryInterpolationSearch(lines, x);

    if (loc == -1) {
        cout << "The date you gave was not found." << endl;
    } else {
        cout << "\nA matching record was found for date " << reg[loc].Date << "." << endl;
        cout << "Note: the dataset may contain multiple records with the same date." << endl;
        cout << "The first matching record in the sorted dataset is shown below." << endl;

        printRecord(loc);
    }

    file.close();
    return 0;
}