//SeyedAlirezaKhosravi 40231015

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Record
{
    int studentNumber;
    char generalCourseName [33];
    char generalCourseInstructor [33];
    int generalCourseScore;
    char coreCourseName [33];
    char coreCourseInstructor [33];
    int coreCourseScore;
    struct Record * next;
    struct Record * prev;
} Record;

typedef struct RBNode {
    int key; // for key
    char color;
    Record * record;
    struct RBNode * l;
    struct RBNode * r;
    struct RBNode * p;                 
} RBNode;

typedef struct RBTree{
    RBNode * root;
    RBNode * nil;
} RBTree;

typedef struct Table
{
    char tableName [33];
    Record * start;
    Record * end;
    RBTree * index;
    struct Table * next;
} Table;

typedef struct hashMap
{
    Table * tables [352];
} hashMap;

hashMap hashTables;

int hashFunction(char tableName[33]);
Table *createTable(char name[33]);
Table *findTable(char table_name[33]);
void addTable(Table *table);
void removeTable(char name[33]);
void createIndex(char table_name[33]);
void insertRBNode(RBTree *tree, Record *record);
void leftRotate(RBTree *tree, RBNode *x);
void rightRotate(RBTree *tree, RBNode *y);
void RBinsert_fixup(RBTree *tree, RBNode *z);
Record *createRecord(int studentNumber, char generalCourseName[33], char generalCourseInstructor[33], int generalCourseScore, char coreCourseName[33], char coreCourseInstructor[33], int coreCourseScore);
void addRecord(char table_name[33], Record record);
void deleteRecord(char table_name[33], char column_name[33], char value[33]);
void updateRecord(char table_name[33], char column_name[33], char value[33], char new_value[33]);
Record *split(Record *start);
Record *merge(Record *first, Record *second);
Record *mergeSort(Record *start);
void selectRecord(char table_name[33], char column_name[33], char value[33], int sorted);
void print_records(Record *start);
void deleteRecords(Record *start);
void printRBTree (RBNode *node, int level);

int hashFunction (char tableName [33]) {
    int hash_code = 0;
    for (int i = 0 ; tableName[i] != '\0' ; i++) {
        hash_code += tableName[i];
    }

    hash_code = hash_code % 352;

    return hash_code;
}

Table * createTable(char name [33]) {
    if (findTable(name)) {
        printf("Table '%s' already exists.\n", name);
        return NULL;
    }

    Table *newTable = (Table *)malloc(sizeof(Table));
    if (!newTable) {
        printf("Failed to create table");
        return NULL;
    }

    RBTree * RBtree = (RBTree *)malloc(sizeof(RBTree));
    RBNode * nilNode = (RBNode *)malloc(sizeof(RBNode));
    nilNode->key = 0;
    nilNode->color = 'b';
    nilNode->l = nilNode->r = nilNode->p = NULL;
    RBtree->nil = nilNode;
    RBtree->root = RBtree->nil;

    strncpy(newTable->tableName, name, 32);
    newTable->tableName[32] = '\0'; 
    newTable->start = NULL;   
    newTable->end = NULL;
    newTable->next = NULL;
    newTable->index = RBtree; // null nist

    printf("Table '%s' created successfully.\n", name);
    return newTable;
}

Table * findTable(char table_name [33]) {
    int index = hashFunction(table_name);
    Table *current = hashTables.tables[index];
    while (current) {
        if (strcmp(current->tableName, table_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void addTable(Table *table) {
    int index = hashFunction(table->tableName);
    table->next = hashTables.tables[index];
    hashTables.tables[index] = table;
    printf("Table '%s' added to Hash Map.\n", table->tableName);
}

void removeTable(char name [33]) {
    int index = hashFunction(name);
    Table *current = hashTables.tables[index];
    Table *prev = NULL;

    while (current && strcmp(current->tableName, name) != 0) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        printf("Table '%s' not found.\n", name);
        return;
    }

    if (prev) {
        prev->next = current->next;
    } else {
        hashTables.tables[index] = current->next;
    }

    free(current->index->nil);
    free(current->index);
    free(current);
    printf("Table '%s' removed from Hash Map.\n", name);
}

void createIndex (char table_name [33]) {
    Table * current_table = findTable(table_name);
    if (current_table == NULL) {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Record * record = current_table->start;
    while(record) {
        insertRBNode(current_table->index , record);
        record = record->next;
    }

    printf("Index created for table '%s'.\n", table_name);
    printRBTree(current_table->index->root, 0);
}

void leftRotate(RBTree * tree, RBNode * x) {
    RBNode * y = x->r;
    x->r = y->l;
    if (y->l != tree->nil) {
        y->l->p = x;
    }
    y->p = x->p;
    if (x->p == NULL) {
        tree->root = y;
    } else if (x == x->p->l) {
        x->p->l = y;
    } else {
        x->p->r = y;
    }
    y->l = x;
    x->p = y;
}

void rightRotate(RBTree * tree, RBNode * y) {
    RBNode *x = y->l;
    y->l = x->r;
    if (x->r != tree->nil) {
        x->r->p = y;
    }
    x->p = y->p;
    if (y->p == NULL) {
        tree->root = x;
    } else if (y == y->p->r) {
        y->p->r = x;
    } else {
        y->p->l = x;
    }
    x->r = y;
    y->p = x;
}

void insertRBNode(RBTree * tree, Record * record) {
    RBNode *new_node = (RBNode *)malloc(sizeof(RBNode));
    new_node->key = record->studentNumber;
    new_node->record = record;
    new_node->color = 'r';
    new_node->p = tree->nil;
    new_node->l = tree->nil;
    new_node->r = tree->nil;

    RBNode *y = tree->nil;
    RBNode *x = tree->root;

    while (x != tree->nil) {
        y = x;
        if (new_node->key < x->key) {
            x = x->l;
        } else {
            x = x->r;
        }
    }

    new_node->p = y;
    if (y == tree->nil) {
        tree->root = new_node;
    } else if (new_node->key < y->key) {
        y->l = new_node;
    } else {
        y->r = new_node;
    }

    RBinsert_fixup(tree, new_node);
}

void RBinsert_fixup(RBTree *tree, RBNode *z) {
    while (z->p->color == 'r') {
        if (z->p == z->p->p->l) {
            RBNode *y = z->p->p->r;
            if (y->color == 'r') {
                z->p->color = 'b';
                y->color = 'b';        // case 1
                z->p->p->color = 'r';
                z = z->p->p;
            } else {
                if (z == z->p->r) {
                    z = z->p;                    // case 2
                    leftRotate(tree, z);
                }
                z->p->color = 'b';
                z->p->p->color = 'r';          // case 3
                rightRotate(tree, z->p->p);
            }
        } else {
            RBNode *y = z->p->p->l;
            if (y->color == 'r') {
                z->p->color = 'b';
                y->color = 'b';
                z->p->p->color = 'r';
                z = z->p->p;
            } else {
                if (z == z->p->l) {
                    z = z->p;
                    rightRotate(tree, z);
                }
                z->p->color = 'b';
                z->p->p->color = 'r';
                leftRotate(tree, z->p->p);
            }
        }
    }
    tree->root->color = 'b';
}

Record * createRecord (int studentNumber , char generalCourseName [33], char generalCourseInstructor [33], int generalCourseScore, char coreCourseName [33], char coreCourseInstructor [33], int coreCourseScore) {
    Record * my_record = (Record *) malloc(sizeof(Record));
    my_record->studentNumber = studentNumber;
    strcpy(my_record->generalCourseInstructor, generalCourseInstructor);
    strcpy(my_record->generalCourseName, generalCourseName);
    my_record->generalCourseScore = generalCourseScore;
    strcpy(my_record->coreCourseInstructor, coreCourseInstructor);
    strcpy(my_record->coreCourseName, coreCourseName);
    my_record->coreCourseScore = coreCourseScore;
    my_record->next = NULL;
    my_record->prev = NULL;
    return my_record;
}

void addRecord (char table_name [33], Record record) {
    if(record.studentNumber == 0){
        printf("The record was not added to the table.\n");
        return;
    }
    Table * current_tb = findTable(table_name);
    if (current_tb == NULL) {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Record * current = current_tb->start;
    while (current) {
        if (current->studentNumber == record.studentNumber) {
            printf("Record with student number %d already exists in table '%s'.\n", record.studentNumber, table_name);
            return;
        }
        current = current->next;
    }

    if (record.generalCourseScore < 0 || record.generalCourseScore > 20) {
        printf("General course score must be between 0 and 20.\n");
        return;
    }

    if (record.coreCourseScore < 0 || record.coreCourseScore > 20) {
        printf("Core course score must be between 0 and 20.\n");
        return;
    }

    Record * new_record = (Record *) malloc(sizeof(Record));
    * new_record = record;
    new_record->prev = current_tb->end;
    new_record->next = NULL;
    
    if(current_tb->end)
        current_tb->end->next = new_record;
    else
        current_tb->start = new_record;

    current_tb->end = new_record;

    if(current_tb->index->nil != current_tb->index->root)
        insertRBNode(current_tb->index, new_record);
    
    printf("Record added to table '%s'.\n", table_name);
}

void deleteRecord(char table_name[33], char column_name[33] , char value [33]) {
    Table * current_table = findTable(table_name);
    int number = atoi(value);
    int match = 0;
    int finded = 0;
    if (current_table == NULL) {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Record * record = current_table->start;
    while (record) {
        if (strcmp(column_name, "student-number") == 0 && record->studentNumber == number)
            match = 1;
        else if (strcmp(column_name, "general-course-name") == 0 && strcmp(record->generalCourseName, value) == 0)
            match = 1;
        else if (strcmp(column_name, "general-course-instructor") == 0 && strcmp(record->generalCourseInstructor, value) == 0)
            match = 1;
        else if (strcmp(column_name, "general-course-score") == 0 && record->generalCourseScore == number)
            match = 1;
        else if (strcmp(column_name, "core-course-name") == 0 && strcmp(record->coreCourseName, value) == 0)
            match = 1;
        else if (strcmp(column_name, "core-course-instructor") == 0 && strcmp(record->coreCourseInstructor, value) == 0)
            match = 1;
        else if (strcmp(column_name, "core-course-score") == 0 && record->coreCourseScore == number)
            match = 1;

        if(match) {
            finded = 1;
            if (record->prev)
                record->prev->next = record->next;
            else
                current_table->start = record->next;
            if (record->next)
                record->next->prev = record->prev;
            else
                current_table->end = record->prev;

            Record * del_rec = record;
            record = record->next;
            free(del_rec);
            printf("Record with %s=%s deleted from table '%s'.\n", column_name, value, table_name);


            if(current_table->index->nil != current_table->index->root) {
                free(current_table->index->nil);
                free(current_table->index);
                current_table->index = (RBTree *) malloc(sizeof(RBTree));
                RBNode * nil_node = (RBNode * ) malloc(sizeof(RBNode));
                nil_node->l = NULL;
                nil_node->r = NULL;
                nil_node->color = 'b';
                current_table->index->nil = nil_node;
                current_table->index->root = nil_node;
                createIndex(table_name);
                printf("Index recreated for table '%s' after record deletion.\n", table_name);
            }
        }
        match = 0; 
        record = record->next;
    }
    if(!finded)
        printf("Record with %s=%s not found in table '%s'.\n", column_name, value, table_name); ///
}

void updateRecord (char table_name[33], char column_name[33], char value[33], char new_value[33]) {
    Table * current_table = findTable(table_name);
    int first_number = atoi(value);
    int second_number = atoi(new_value);
    int finded = 0;
    if (current_table == NULL) {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Record * record = current_table->start;
    while (record) {
        if (strcmp(column_name, "student-number") == 0 && record->studentNumber == first_number){
            Record * current = current_table->start;
            while (current) {
                if (current->studentNumber == first_number) {
                    printf("Record with student number %d already exists in table '%s'.\n",current->studentNumber, table_name);
                    return;
                }
            current = current->next;
            }
            record->studentNumber = second_number;
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            return;
        }
        else if (strcmp(column_name, "general-course-name") == 0 && strcmp(record->generalCourseName, value) == 0){
            strcpy(record->generalCourseName, new_value);
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        else if (strcmp(column_name, "general-course-instructor") == 0 && strcmp(record->generalCourseInstructor, value) == 0){
            strcpy(record->generalCourseInstructor, new_value);
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        else if (strcmp(column_name, "general-course-score") == 0 && record->generalCourseScore == first_number){
            record->generalCourseScore = second_number;
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        else if (strcmp(column_name, "core-course-name") == 0 && strcmp(record->coreCourseName, value) == 0){
            strcpy(record->coreCourseName, new_value);
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        else if (strcmp(column_name, "core-course-instructor") == 0 && strcmp(record->coreCourseInstructor, value) == 0){
            strcpy(record->coreCourseInstructor, new_value);
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        else if (strcmp(column_name, "core-course-score") == 0 && record->coreCourseScore == first_number){
            record->coreCourseScore = second_number;
            printf("Record with %s=%s updated to %s in table '%s'.\n", column_name, value, new_value, table_name);
            finded = 1;
        }
        record = record->next;
    }
    
    if(!finded)
        printf("Record with %s=%s not found in table '%s'.\n", column_name, value, table_name);
}

Record * split (Record * start) {
    Record * aux_node1 = start;
    Record * aux_node2 = start;

    while(aux_node2 && aux_node2->next && aux_node2->next->next) {
        aux_node2 = aux_node2->next->next;
        aux_node1 = aux_node1->next;
    }

    Record * mid_node = aux_node1->next;
    aux_node1->next = NULL;
    if (mid_node != NULL) {
        mid_node->prev = NULL;
    }
    return mid_node;
}

Record * merge(Record * first, Record * second) {
    if (first == NULL)
        return second;
    if (second == NULL) 
        return first;
    
    if(first->studentNumber > second->studentNumber){
        second->next = merge(second->next, first);
        if (second->next != NULL) {
            second->next->prev = second;
        }
        second->prev = NULL;
        return second;
    }

    else {
        first->next = merge(first->next, second);
        if (first->next != NULL) {
            first->next->prev = first;
        }
        first->prev = NULL;
        return first;
    }
}

Record * mergeSort (Record * start) {
    if (start == NULL || start->next == NULL) {
        return start;
    }

    Record * mid = split(start);

    start = mergeSort(start);
    mid = mergeSort(mid);

    return merge(start, mid);
}

void selectRecord (char table_name [33], char column_name [33], char value [33], int sorted) {
    int val_int = atoi(value);
    int match = 0;
    Table * cur_table = findTable(table_name);
    if (cur_table == NULL) {
        printf("Table '%s' not found.\n", table_name);
        return;
    }

    Record * record = cur_table->start;
    Record * selected_records_start = NULL;
    Record * selected_records_end = NULL;

    while (record) {
        if (strcmp(column_name, "student-number") == 0 && record->studentNumber == val_int)
            match = 1;
        else if (strcmp(column_name, "general-course-name") == 0 && strcmp(record->generalCourseName, value) == 0)
            match = 1;
        else if (strcmp(column_name, "general-course-instructor") == 0 && strcmp(record->generalCourseInstructor, value) == 0)
            match = 1;
        else if (strcmp(column_name, "general-course-score") == 0 && record->generalCourseScore == val_int)
            match = 1;
        else if (strcmp(column_name, "core-course-name") == 0 && strcmp(record->coreCourseName, value) == 0)
            match = 1;
        else if (strcmp(column_name, "core-course-instructor") == 0 && strcmp(record->coreCourseInstructor, value) == 0)
            match = 1;
        else if (strcmp(column_name, "core-course-score") == 0 && record->coreCourseScore == val_int)
            match = 1;

        if (match) {
            Record * new_record = (Record *) malloc(sizeof(Record));
            *new_record = *record;
            new_record->prev = new_record->next = NULL;

            if (selected_records_start == NULL) {
                selected_records_start = selected_records_end =  new_record;
            }
            else {
                selected_records_end->next = new_record;
                new_record->prev = selected_records_end;
                selected_records_end = new_record;
            }
        }
        match = 0;
        record = record->next;
    }
    
    if(!selected_records_start) {
        printf("Record with %s=%s not found in table '%s'.\n", column_name, value, table_name);
        return;
    }
    if (sorted)
        selected_records_start = mergeSort(selected_records_start);

    print_records(selected_records_start);
    deleteRecords(selected_records_start);
}

void print_records (Record * start) {
    Record * record = start;
    while(record) {
        printf("Student Number: %d, General Course: %s, General Instructor: %s, General Score: %d, Core Course: %s, Core Instructor: %s, Core Score: %d\n",
                record->studentNumber, record->generalCourseName, record->generalCourseInstructor, record->generalCourseScore,
               record->coreCourseName, record->coreCourseInstructor, record->coreCourseScore);
        record = record->next;
    }
}

void deleteRecords (Record * start) {
    Record * record = start;
    while (record) {
        Record * temp = record;
        record = record->next;
        free(temp); 
    }
}

void printRBTree(RBNode *node, int level) {
    if (node == NULL || node->key == 0) {
        return;
    }

    printRBTree(node->l, level + 1);

    printf("%d (%c)\n", node->key, node->color == 'r' ? 'R' : 'B');

    printRBTree(node->r, level + 1);
}

int main() {
    char input[300];
    printf("Enter commands (type 'EXIT' to quit):\n");

    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "EXIT") == 0) {
            break;
        }

        char tableName[33], columnName[33], value[33], newValue[33];
        if (sscanf(input, "CREATE TABLE %s", tableName) == 1) {
            Table *table = createTable(tableName);
            if (table) {
                addTable(table);
            }
        } else if (sscanf(input, "DELETE TABLE %s", tableName) == 1) {
            removeTable(tableName);
        } else if (sscanf(input, "CREATE INDEX %s", tableName) == 1) {
            createIndex(tableName);
        } else if (strncmp(input, "ADD ", 4) == 0) {
    sscanf(input + 4, "%s", tableName);

    Table *currentTable = findTable(tableName);
    if (!currentTable) {
        printf("Table '%s' not found.\n", tableName);
        continue;
    }

    Record newRecord = {0};
    char *token = strtok(input + 4 + strlen(tableName), " "); 

    while (token) {
        strncpy(columnName, token, 32);
        columnName[32] = '\0';
        token = strtok(NULL, " "); 

        if (!token) {
            printf("Missing value for column '%s'.\n", columnName);
            break;
        }

        strncpy(value, token, 32);
        value[32] = '\0';
        token = strtok(NULL, " ");

        if (strcmp(columnName, "student-number") == 0) {
            newRecord.studentNumber = atoi(value);
        } else if (strcmp(columnName, "general-course-name") == 0) {
            strncpy(newRecord.generalCourseName, value, 32);
        } else if (strcmp(columnName, "general-course-instructor") == 0) {
            strncpy(newRecord.generalCourseInstructor, value, 32);
        } else if (strcmp(columnName, "general-course-score") == 0) {
            newRecord.generalCourseScore = atoi(value);
        } else if (strcmp(columnName, "core-course-name") == 0) {
            strncpy(newRecord.coreCourseName, value, 32);
        } else if (strcmp(columnName, "core-course-instructor") == 0) {
            strncpy(newRecord.coreCourseInstructor, value, 32);
        } else if (strcmp(columnName, "core-course-score") == 0) {
            newRecord.coreCourseScore = atoi(value);
        } else {
            printf("Unknown column: %s\n", columnName);
            break;
        }
    }

    addRecord(tableName, newRecord); 
}
 else if (sscanf(input, "DELETE %s %s %s", tableName, columnName, value) == 3) {
            deleteRecord(tableName, columnName, value);
        } else if (sscanf(input, "UPDATE %s %s %s %s", tableName, columnName, value, newValue) == 4) {
            updateRecord(tableName, columnName, value, newValue);
        } else if (strstr(input, "SORTED") != NULL && sscanf(input, "SELECT %s %s %s", tableName, columnName, value) == 3) {
            selectRecord(tableName, columnName, value, 1);
        } else if (sscanf(input, "SELECT %s %s %s", tableName, columnName, value) == 3) {
            selectRecord(tableName, columnName, value, 0);
        } else {
            printf("Invalid command. Try again.\n");
        }
    }

    printf("Exiting program.\n");
    return 0;
}
