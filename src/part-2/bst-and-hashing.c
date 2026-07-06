#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_FIELD 256
#define INITIAL_CAPACITY 1024

typedef struct Record {
    char direction[MAX_FIELD];
    char year[8];
    char date[16];
    char weekday[MAX_FIELD];
    char country[MAX_FIELD];
    char commodity[MAX_FIELD];
    char transportMode[MAX_FIELD];
    char measure[MAX_FIELD];
    unsigned long long value;
    unsigned long long cumulative;
} Record;

typedef struct BSTNode {
    Record data;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct HashNode {
    Record data;
    struct HashNode* next;
} HashNode;

int dateToInt(const char* date) {
    int day = 0, month = 0, year = 0;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3)
        return -1;
    return year * 10000 + month * 100 + day;
}

void removeNewline(char* str) {
    str[strcspn(str, "\r\n")] = '\0';
}

void copyField(char* destination, const char* source, int size) {
    strncpy(destination, source, size - 1);
    destination[size - 1] = '\0';
}

int splitCSVLine(const char* line, char fields[][MAX_FIELD], int maxFields) {
    int field = 0;
    int i = 0;
    int j = 0;
    int insideQuotes = 0;

    while (line[i] != '\0' && field < maxFields) {
        char c = line[i];

        if (c == '"') {
            insideQuotes = !insideQuotes;
            if (j < MAX_FIELD - 1)
                fields[field][j++] = c;
        } else if (c == ',' && !insideQuotes) {
            fields[field][j] = '\0';
            field++;
            j = 0;
        } else {
            if (j < MAX_FIELD - 1)
                fields[field][j++] = c;
        }

        i++;
    }

    if (field < maxFields) {
        fields[field][j] = '\0';
        field++;
    }

    return field;
}

int parseRecord(char* line, Record* record) {
    char fields[10][MAX_FIELD];
    removeNewline(line);

    if (splitCSVLine(line, fields, 10) != 10)
        return 0;

    copyField(record->direction, fields[0], MAX_FIELD);
    copyField(record->year, fields[1], sizeof(record->year));
    copyField(record->date, fields[2], sizeof(record->date));
    copyField(record->weekday, fields[3], MAX_FIELD);
    copyField(record->country, fields[4], MAX_FIELD);
    copyField(record->commodity, fields[5], MAX_FIELD);
    copyField(record->transportMode, fields[6], MAX_FIELD);
    copyField(record->measure, fields[7], MAX_FIELD);

    record->value = strtoull(fields[8], NULL, 10);
    record->cumulative = strtoull(fields[9], NULL, 10);

    return 1;
}

Record* loadRecords(const char* filename, int* count) {
    FILE* file = fopen(filename, "r");
    char line[MAX_LINE];
    int capacity = INITIAL_CAPACITY;
    Record* records = (Record*)malloc(capacity * sizeof(Record));

    if (records == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    if (file == NULL) {
        printf("Failed to open %s\n", filename);
        free(records);
        return NULL;
    }

    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) {
        Record record;

        if (parseRecord(line, &record)) {
            if (*count >= capacity) {
                capacity *= 2;
                Record* temp = (Record*)realloc(records, capacity * sizeof(Record));

                if (temp == NULL) {
                    printf("Memory reallocation failed.\n");
                    free(records);
                    fclose(file);
                    return NULL;
                }

                records = temp;
            }

            records[*count] = record;
            (*count)++;
        }
    }

    fclose(file);
    return records;
}

void printHeader(void) {
    printf("%-10s %-6s %-12s %-12s %-30s %-40s %-15s %-10s %15s %15s\n",
           "Direction", "Year", "Date", "Weekday", "Country", "Commodity",
           "Transport", "Measure", "Value", "Cumulative");
}

void printRecord(Record record) {
    printf("%-10s %-6s %-12s %-12s %-30s %-40s %-15s %-10s %15llu %15llu\n",
           record.direction, record.year, record.date, record.weekday,
           record.country, record.commodity, record.transportMode, record.measure,
           record.value, record.cumulative);
}

int compareDateKey(const Record* a, const Record* b) {
    int dateA = dateToInt(a->date);
    int dateB = dateToInt(b->date);
    int cmp;

    if (dateA < dateB) return -1;
    if (dateA > dateB) return 1;

    cmp = strcmp(a->direction, b->direction);
    if (cmp != 0) return cmp;

    cmp = strcmp(a->country, b->country);
    if (cmp != 0) return cmp;

    cmp = strcmp(a->commodity, b->commodity);
    if (cmp != 0) return cmp;

    cmp = strcmp(a->transportMode, b->transportMode);
    if (cmp != 0) return cmp;

    cmp = strcmp(a->measure, b->measure);
    if (cmp != 0) return cmp;

    if (a->value < b->value) return -1;
    if (a->value > b->value) return 1;

    if (a->cumulative < b->cumulative) return -1;
    if (a->cumulative > b->cumulative) return 1;

    return 0;
}

int compareValueKey(const Record* a, const Record* b) {
    if (a->value < b->value) return -1;
    if (a->value > b->value) return 1;
    return compareDateKey(a, b);
}

int qsortCompareDate(const void* a, const void* b) {
    const Record* recordA = (const Record*)a;
    const Record* recordB = (const Record*)b;
    return compareDateKey(recordA, recordB);
}

int qsortCompareValue(const void* a, const void* b) {
    const Record* recordA = (const Record*)a;
    const Record* recordB = (const Record*)b;
    return compareValueKey(recordA, recordB);
}

BSTNode* createBSTNode(Record record) {
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    newNode->data = record;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

BSTNode* buildBalancedBST(Record records[], int left, int right) {
    if (left > right)
        return NULL;

    int mid = left + (right - left) / 2;
    BSTNode* root = createBSTNode(records[mid]);

    root->left = buildBalancedBST(records, left, mid - 1);
    root->right = buildBalancedBST(records, mid + 1, right);

    return root;
}

BSTNode* insertValueBST(BSTNode* root, Record record) {
    if (root == NULL)
        return createBSTNode(record);

    if (compareValueKey(&record, &root->data) < 0)
        root->left = insertValueBST(root->left, record);
    else
        root->right = insertValueBST(root->right, record);

    return root;
}

void inorderTraversalLimited(BSTNode* root, int* printed, int limit) {
    if (root == NULL || *printed >= limit)
        return;

    inorderTraversalLimited(root->left, printed, limit);

    if (*printed < limit) {
        printRecord(root->data);
        (*printed)++;
    }

    inorderTraversalLimited(root->right, printed, limit);
}

BSTNode* searchDateBST(BSTNode* root, const char* date) {
    int target = dateToInt(date);

    while (root != NULL) {
        int current = dateToInt(root->data.date);

        if (target == current)
            return root;

        if (target < current)
            root = root->left;
        else
            root = root->right;
    }

    return NULL;
}

BSTNode* searchValueBST(BSTNode* root, unsigned long long value) {
    while (root != NULL) {
        if (value == root->data.value)
            return root;

        if (value < root->data.value)
            root = root->left;
        else
            root = root->right;
    }

    return NULL;
}

BSTNode* findMinNode(BSTNode* root) {
    while (root != NULL && root->left != NULL)
        root = root->left;
    return root;
}

BSTNode* deleteDateBST(BSTNode* root, const char* date) {
    if (root == NULL)
        return NULL;

    int target = dateToInt(date);
    int current = dateToInt(root->data.date);

    if (target < current) {
        root->left = deleteDateBST(root->left, date);
    } else if (target > current) {
        root->right = deleteDateBST(root->right, date);
    } else {
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        }

        if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }

        BSTNode* temp = findMinNode(root->right);
        root->data = temp->data;
        root->right = deleteDateBST(root->right, temp->data.date);
    }

    return root;
}

BSTNode* deleteValueBST(BSTNode* root, unsigned long long value) {
    if (root == NULL)
        return NULL;

    if (value < root->data.value) {
        root->left = deleteValueBST(root->left, value);
    } else if (value > root->data.value) {
        root->right = deleteValueBST(root->right, value);
    } else {
        if (root->left == NULL) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        }

        if (root->right == NULL) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }

        BSTNode* temp = findMinNode(root->right);
        root->data = temp->data;
        root->right = deleteValueBST(root->right, temp->data.value);
    }

    return root;
}

void freeBST(BSTNode* root) {
    if (root == NULL)
        return;

    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

int computeHash(const char* date, int m) {
    int dateNumber = dateToInt(date);
    if (dateNumber < 0)
        dateNumber = 0;
    return dateNumber % m;
}

HashNode* createHashNode(Record record) {
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    newNode->data = record;
    newNode->next = NULL;
    return newNode;
}

void insertHashRecord(HashNode** hashTable, int m, Record record) {
    int index = computeHash(record.date, m);
    HashNode* newNode = createHashNode(record);

    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

HashNode* searchHash(HashNode** hashTable, int m, const char* date) {
    int index = computeHash(date, m);
    HashNode* current = hashTable[index];

    while (current != NULL) {
        if (strcmp(current->data.date, date) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

void deleteHashRecord(HashNode** hashTable, int m, const char* date) {
    int index = computeHash(date, m);
    HashNode* current = hashTable[index];
    HashNode* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->data.date, date) == 0) {
            if (previous == NULL)
                hashTable[index] = current->next;
            else
                previous->next = current->next;

            free(current);
            printf("Record deleted successfully.\n");
            return;
        }

        previous = current;
        current = current->next;
    }

    printf("Date not found.\n");
}

void printHashTableLimited(HashNode** hashTable, int m, int limit) {
    int printed = 0;

    for (int i = 0; i < m && printed < limit; i++) {
        HashNode* current = hashTable[i];

        while (current != NULL && printed < limit) {
            printRecord(current->data);
            printed++;
            current = current->next;
        }
    }
}

void freeHashTable(HashNode** hashTable, int m) {
    for (int i = 0; i < m; i++) {
        HashNode* current = hashTable[i];

        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(hashTable);
}

HashNode** buildHashTable(Record records[], int count, int m) {
    HashNode** hashTable = (HashNode**)malloc(m * sizeof(HashNode*));

    if (hashTable == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for (int i = 0; i < m; i++)
        hashTable[i] = NULL;

    for (int i = 0; i < count; i++)
        insertHashRecord(hashTable, m, records[i]);

    return hashTable;
}

BSTNode* dateBSTMenu(BSTNode* root) {
    int option;
    char date[16];
    BSTNode* node;

    while (1) {
        printf("\nDate BST menu:\n");
        printf("1. Print first 20 records using in-order traversal\n");
        printf("2. Search for Value from a given Date\n");
        printf("3. Change the Value from a given Date\n");
        printf("4. Delete one registration from a given Date\n");
        printf("5. Close this menu\n");
        printf("Option: ");
        scanf("%d", &option);

        if (option == 1) {
            int printed = 0;
            printHeader();
            inorderTraversalLimited(root, &printed, 20);
        } else if (option == 2) {
            printf("Enter Date, for example 1/1/2020: ");
            scanf("%15s", date);
            node = searchDateBST(root, date);

            if (node != NULL) {
                printHeader();
                printRecord(node->data);
            } else {
                printf("Date not found.\n");
            }
        } else if (option == 3) {
            printf("Enter Date to update Value: ");
            scanf("%15s", date);
            node = searchDateBST(root, date);

            if (node != NULL) {
                printf("Enter new Value: ");
                scanf("%llu", &node->data.value);
                printf("Value updated successfully.\n");
            } else {
                printf("Date not found.\n");
            }
        } else if (option == 4) {
            printf("Enter Date to delete: ");
            scanf("%15s", date);
            root = deleteDateBST(root, date);
            printf("Delete operation completed if date existed.\n");
        } else if (option == 5) {
            return root;
        } else {
            printf("Invalid option.\n");
        }
    }
}

BSTNode* valueBSTMenu(BSTNode* root) {
    int option;
    unsigned long long value;
    BSTNode* node;

    while (1) {
        printf("\nValue BST menu:\n");
        printf("1. Print first 20 records using in-order traversal\n");
        printf("2. Search for record from a given Value\n");
        printf("3. Change the Value from a given Value\n");
        printf("4. Delete one registration from a given Value\n");
        printf("5. Close this menu\n");
        printf("Option: ");
        scanf("%d", &option);

        if (option == 1) {
            int printed = 0;
            printHeader();
            inorderTraversalLimited(root, &printed, 20);
        } else if (option == 2) {
            printf("Enter Value to search for: ");
            scanf("%llu", &value);
            node = searchValueBST(root, value);

            if (node != NULL) {
                printHeader();
                printRecord(node->data);
            } else {
                printf("Value not found.\n");
            }
        } else if (option == 3) {
            printf("Enter Value to search for: ");
            scanf("%llu", &value);
            node = searchValueBST(root, value);

            if (node != NULL) {
                Record updated = node->data;
                root = deleteValueBST(root, value);
                printf("Enter new Value: ");
                scanf("%llu", &updated.value);
                root = insertValueBST(root, updated);
                printf("Value updated and reinserted successfully.\n");
            } else {
                printf("Value not found.\n");
            }
        } else if (option == 4) {
            printf("Enter Value to delete: ");
            scanf("%llu", &value);
            root = deleteValueBST(root, value);
            printf("Delete operation completed if value existed.\n");
        } else if (option == 5) {
            return root;
        } else {
            printf("Invalid option.\n");
        }
    }
}

void hashMenu(HashNode** hashTable, int m) {
    int option;
    char date[16];
    HashNode* node;

    while (1) {
        printf("\nHash table menu:\n");
        printf("1. Print first 20 records from hash table\n");
        printf("2. Search for Value from a given Date\n");
        printf("3. Change the Value from a given Date\n");
        printf("4. Delete one registration from a given Date\n");
        printf("5. Close this menu\n");
        printf("Option: ");
        scanf("%d", &option);

        if (option == 1) {
            printHeader();
            printHashTableLimited(hashTable, m, 20);
        } else if (option == 2) {
            printf("Enter Date, for example 1/1/2020: ");
            scanf("%15s", date);
            node = searchHash(hashTable, m, date);

            if (node != NULL) {
                printHeader();
                printRecord(node->data);
            } else {
                printf("Date not found.\n");
            }
        } else if (option == 3) {
            printf("Enter Date to update Value: ");
            scanf("%15s", date);
            node = searchHash(hashTable, m, date);

            if (node != NULL) {
                printf("Enter new Value: ");
                scanf("%llu", &node->data.value);
                printf("Value updated successfully.\n");
            } else {
                printf("Date not found.\n");
            }
        } else if (option == 4) {
            printf("Enter Date to delete: ");
            scanf("%15s", date);
            deleteHashRecord(hashTable, m, date);
        } else if (option == 5) {
            return;
        } else {
            printf("Invalid option.\n");
        }
    }
}

int main(void) {
    const char filename[] = "data/covid-19-trade-data.csv";
    int menuOption;
    int count = 0;
    Record* records = loadRecords(filename, &count);

    if (records == NULL)
        return 1;

    printf("Loaded %d records from the CSV file.\n", count);

    printf("Please choose an option:\n");
    printf("1. Build balanced BST based on Date\n");
    printf("2. Build balanced BST based on Value\n");
    printf("3. Build hash table based on Date\n");
    printf("Option: ");
    scanf("%d", &menuOption);

    if (menuOption == 1) {
        qsort(records, count, sizeof(Record), qsortCompareDate);
        BSTNode* dateBST = buildBalancedBST(records, 0, count - 1);
        printf("Balanced Date BST created with %d records.\n", count);
        dateBST = dateBSTMenu(dateBST);
        freeBST(dateBST);
    } else if (menuOption == 2) {
        qsort(records, count, sizeof(Record), qsortCompareValue);
        BSTNode* valueBST = buildBalancedBST(records, 0, count - 1);
        printf("Balanced Value BST created with %d records.\n", count);
        valueBST = valueBSTMenu(valueBST);
        freeBST(valueBST);
    } else if (menuOption == 3) {
        int m;
        printf("Enter the number of buckets: ");
        scanf("%d", &m);

        if (m <= 0) {
            printf("Invalid number of buckets.\n");
            free(records);
            return 1;
        }

        HashNode** hashTable = buildHashTable(records, count, m);

        if (hashTable == NULL) {
            free(records);
            return 1;
        }

        printf("Hash table created with %d records and %d buckets.\n", count, m);
        hashMenu(hashTable, m);
        freeHashTable(hashTable, m);
    } else {
        printf("Invalid option.\n");
    }

    free(records);
    return 0;
}
