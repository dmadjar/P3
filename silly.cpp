//
//  main.cpp
//  Project3
//
//  Created by Daniel Madjar on 10/27/22.
//
#include "TableEntry.h"
#include "xcode_redirect.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
using namespace std;


struct Column {
    string columnName;
    char columnType; //Not sure if string∫
    uint32_t index;
};

class Table {
    
public:
    string tableName;
    
    unordered_map<string, Column> nameToIndex; // column name to what index it is in the vector<Columns>
    unordered_map<TableEntry, vector<uint32_t>> hashIndex;
    
    map<TableEntry, vector<uint32_t>> bstIndex;
    
    vector<vector<TableEntry>> data;
    
    vector<Column> columns;
    
    string columnNameIndex;
    
    uint32_t isHash; // 0 for neither, 1 for hash, 2 for bst
    
    Table() : tableName(""), columns({}), data({}) {}
    
    Table(string name, uint32_t size) : tableName(name), columns({}), data({}), isHash(0) {
        columns.resize(size);
    }
    
    void insert(string tableName) {
        string blank;
        uint32_t size;
        
        cin >> size;
        cin >> blank;
        
        uint32_t startN = static_cast<uint32_t>(data.size());
        uint32_t endN = startN + size - 1;
        
        for(uint32_t i = 0; i < size; i++) {
            vector<TableEntry> row;
            row.reserve(columns.size());
            
            for(uint32_t j = 0; j < columns.size(); j++) {
                if (columns[j].columnType == 'd') { //double
                    double x;
                    cin >> x;
                   
                    row.emplace_back(x);
                } else if (columns[j].columnType == 'b') { //bool
                    string x;
                    cin >> x;
                    
                    if (x[0] == 't') {
                        row.emplace_back(true);
                    } else {
                        row.emplace_back(false);
                    }
                } else if (columns[j].columnType == 'i') { //int
                    int x;
                    cin >> x;
                   
                    row.emplace_back(x);
                } else { // string
                    string x;
                    cin >> x;
                    
                    row.emplace_back(x);
                }
            }
            
            data.emplace_back(row);
        }
        
        if (!bstIndex.empty()) {
            bstIndex.clear();
            uint32_t colIndex = nameToIndex[columnNameIndex].index;
            deleteGenerate(colIndex);
        } else if (!hashIndex.empty()) {
            hashIndex.clear();
            uint32_t colIndex = nameToIndex[columnNameIndex].index;
            deleteGenerate(colIndex);
        }
        
        cout << "Added " << size << " rows to " << tableName << " from position " << startN << " to " << endN <<"\n";
    }
    
    void print(string command) {
        uint32_t numCols;
        vector<uint32_t> columnIndexes;
        cin >> numCols;
        
        for(uint32_t i = 0; i < numCols; i++) {
            cin >> command;
            
            if (!columnIsFound(command)) {
                cout << "Error during GENERATE: " << command << " does not name a column in " << tableName << "\n";
                return;
            }
            
            columnIndexes.push_back(nameToIndex[command].index);
            cout << command << " ";
        }
        
        cout << "\n";
        cin >> command;
        
        if (command[0] == 'A') {
            uint32_t i = 0;
            for(; i < data.size(); i++) {
                for(uint32_t j = 0; j < numCols; j++) {
                    cout << data[i][columnIndexes[j]] << " ";
                }
                
                cout << "\n";
            }
            
            cout << "Printed " << i << " matching rows from " << tableName << "\n";
        } else {
            string columnName;
            char op;
            cin >> columnName;
            
            if (!columnIsFound(columnName)) {
                cout << "Error during GENERATE: " << columnName << " does not name a column in " << tableName << "\n";
                return;
            }
            
            cin >> op;
            string type;
            
            type = nameToIndex[columnName].columnType; //incorrectly thought it was a string instead of a bool
            
           if (type[0] == 'd') { //double
                double x;
                cin >> x;
               
                TableEntry value{x};
                printWhere(columnIndexes, value, columnName, op);
            } else if (type[0] == 'b') { //bool
                string x;
                cin >> x;
                
                if (x[0] == 't') {
                    TableEntry value{true};
                    printWhere(columnIndexes, value, columnName, op);
                } else {
                    TableEntry value{false};
                    printWhere(columnIndexes, value, columnName, op);
                }
            } else if (type[0] == 'i') { //int
                int x;
                cin >> x;
                
                TableEntry value{x};
                printWhere(columnIndexes, value, columnName, op);
            } else { // string
                string x;
                cin >> x;
                
                TableEntry value{x};
                printWhere(columnIndexes, value, columnName, op);
            }
        }
    }
    
    void printWhere(vector<uint32_t> &columnIndexes, TableEntry value, string columnName, char op) {
        uint32_t index = nameToIndex[columnName].index;
        
        if (isHash == 0 || (isHash == 1 && columnNameIndex != columnName) || (isHash == 1 && columnNameIndex == columnName)) {
            uint32_t i = 0;
            uint32_t count = 0;
            for(; i < data.size(); i++) {
                if (checkOperator(value, op, data[i][index])) {
                    for(uint32_t k = 0; k < columnIndexes.size(); k++) {
                        cout << data[i][columnIndexes[k]] << " ";
                    }
                    
                    count++;
                    cout << "\n";
                }
            }
            
            cout << "Printed " << count << " matching rows from " << tableName << "\n";
        } else if (isHash == 2 && columnNameIndex == columnName) {
            uint32_t count = 0;
            
            for(auto it = bstIndex.begin(); it != bstIndex.end(); it++) {
               
                for(uint32_t j = 0; j < it->second.size(); j++) {
                    if (checkOperator(value, op, data[it->second[j]][index])) {
                        for(uint32_t k = 0; k < columnIndexes.size(); k++) {
                            cout << data[it->second[j]][columnIndexes[k]] << " ";
                        }
                        
                        count++;
                        cout << "\n";
                    }
                }
            }
            
            cout << "Printed " << count << " matching rows from " << tableName << "\n";
        }
    }
    
    void generateIndex() {
        string indexType;
        string colName;
        string blank;
        
        cin >> indexType;
        cin >> blank;
        cin >> blank;
        cin >> colName;
        
        if (!columnIsFound(colName)) {
            cout << "Error during GENERATE: " << colName << " does not name a column in " << tableName << "\n";
            return;
        }
        
        uint32_t index = nameToIndex[colName].index;
        
        if (!bstIndex.empty() || !hashIndex.empty()) {
            bstIndex.clear();
            hashIndex.clear();
        }
        
        if (!data.empty()) {
            if (indexType[0] == 'h') {
                for(uint32_t i = 0; i < data.size(); i++) {
                    hashIndex[data[i][index]].push_back(i);
                }
                
                columnNameIndex = colName;
                isHash = 1;
            } else {
                for(uint32_t i = 0; i < data.size(); i++) {
                    bstIndex[data[i][index]].push_back(i);
                }
                
                columnNameIndex = colName;
                isHash = 2;
            }
        }
        
        cout << "Created " << indexType << " index for table " << tableName << " on column " << colName << "\n";
    }
    
    void generalGenerate(uint32_t colIndex, unordered_map<TableEntry, vector<uint32_t>> &mp) {
        for(uint32_t i = 0; i < data.size(); i++) {
            mp[data[i][colIndex]].push_back(i);
        }
    }
    
    void deleteGenerate(uint32_t colIndex) {
        if (!data.empty()) {
            if (isHash == 1) {
                for(uint32_t i = 0; i < data.size(); i++) {
                    hashIndex[data[i][colIndex]].push_back(i);
                }
            } else {
                for(uint32_t i = 0; i < data.size(); i++) {
                    bstIndex[data[i][colIndex]].push_back(i);
                }
            }
        }
    }
    
    bool columnIsFound(string columnName) {
        if (nameToIndex.find(columnName) != nameToIndex.end()) {
            return true;
        } else {
            return false;
        }
    }
    
    bool checkOperator(TableEntry value, char op, TableEntry columnValue) {
        if (op == '>') {
            return columnValue > value;
        } else if (op == '<') {
            return columnValue < value;
        } else {
            return columnValue == value;
        }
    }
    
    void deleteRows() {
        string colName;
        char op;
        
        cin >> colName;
        cin >> op;
        
        char type = nameToIndex[colName].columnType;
        uint32_t col = nameToIndex[colName].index;
        
        if (type == 'd') { //double
             double x;
             cin >> x;
            
             TableEntry value{x};
             deleteHelper(value, col, op);
        } else if (type == 'b') { //bool
             string x;
             cin >> x;
             
             if (x[0] == 't') {
                 TableEntry value{true};
                 deleteHelper(value, col, op);
             } else {
                 TableEntry value{false};
                 deleteHelper(value, col, op);
             }
         } else if (type == 'i') { //int
             int x;
             cin >> x;
             
             TableEntry value{x};
             deleteHelper(value, col, op);
         } else { // string
             string x;
             cin >> x;
             
             TableEntry value{x};
             deleteHelper(value, col, op);
         }
        
        if (isHash != 0) {
            bstIndex.clear();
            hashIndex.clear();
            deleteGenerate(col);
        }
    }
    
    void deleteHelper(TableEntry value, uint32_t index, char op) {
        if (op == '<') {
            LessThan l(value, index);
            finalHelper(l);
        } else if (op == '>') {
            GreaterThan l(value, index);
            finalHelper(l);
        } else {
            EqualTo l(value, index);
            finalHelper(l);
        }
    }
    
    template<class Compare>
    void finalHelper(Compare compare) {
        uint32_t oldSize = static_cast<uint32_t>(size(data));
        
        data.erase(remove_if(data.begin(), data.end(), compare), data.end());
        
        uint32_t newSize = static_cast<uint32_t>(size(data));;
        
        cout << "Deleted " << (oldSize - newSize) << " rows from " << tableName << "\n";
    }
    
    class LessThan {
    private:
        const TableEntry value;
        const uint32_t col;
        
    public:
        LessThan(const TableEntry value, const uint32_t col) : value(value), col(col) {}
        
        bool operator()(const vector<TableEntry> &row) const {
            return row[col] < value;
        }
    };
    
    class GreaterThan {
    private:
        const TableEntry value;
        const uint32_t col;
    public:
        GreaterThan(const TableEntry value, const uint32_t col) : value(value),
        col(col) {}
        
        bool operator()(const vector<TableEntry> &row) const {
            return row[col] > value;
        }
    };
    
    class EqualTo {
    private:
        const TableEntry value;
        const uint32_t col;
        
    public:
        EqualTo(const TableEntry value, const uint32_t col) : value(value), col(col) {}
        
        bool operator()(const vector<TableEntry> &row) const {
            return row[col] == value;
        }
    };
};

class Database {
private:
    unordered_map<string, Table> tables;
    
public:
    
    void readInput() {
        string command;
        
        do {
            cout << "% ";
            cin >> command;
            
            processCommand(command);
            
        } while (command != "QUIT");
        
        cout << "Thanks for being silly!\n";
    }
    
    void processCommand(string command) {
        if (command[0] == 'Q') {
            return;
        } else if (command[0] == '#') {
            getline(cin, command);
        } else if (command[0] == 'C') {
            create(command);
        } else if (command[0] == 'R') {
            remove(command);
        } else if (command[0] == 'I') {
            string tableName;
            cin >> command;
            cin >> tableName;
            
            if (!tableAlreadyExists(tableName)) {
                cout << "Error during CREATE: Cannot create already existing table "
                << tableName << "\n";
                return;
            }
            
            tables[tableName].insert(tableName);
        } else if (command[0] == 'P') {
            string tableName;
            cin >> command;
            cin >> tableName;
            
            if (!tableAlreadyExists(tableName)) {
                cout << "Error during CREATE: Cannot create already existing table "
                << tableName << "\n";
                return;
            }
            
            tables[tableName].print(command);
            
        } else if (command[0] == 'G') {
            string tableName;
            cin >> command;
            cin >> tableName;
            
            if (!tableAlreadyExists(tableName)) {
                cout << "Error during CREATE: Cannot create already existing table "
                << tableName << "\n";
                return;
            }
            
            tables[tableName].generateIndex();
        } else if (command[0] == 'D') {
            string tableName;
            cin >> command;
            cin >> tableName;
            cin >> command;
            
            tables[tableName].deleteRows();
        } else if (command[0] == 'J') {
            string tableOne;
            string tableTwo;
            
            cin >> tableOne;
            cin >> command; //and
            cin >> tableTwo;
            
            cin >> command; //where
            
            join(tables[tableOne], tables[tableTwo]);
        }
    }
    
    void create(string column) {
        uint32_t size;
        string tableName;
        cin >> tableName;
        
        if (tableAlreadyExists(tableName)) {
            cout << "Error during CREATE: Cannot create already existing table "
            << tableName << "\n";
            return;
        }
        
        cin >> size;
        
        tables[tableName] = Table(tableName, size);
        uint32_t doubleSize = size * 2;
        
        vector<string> typeName;
        typeName.reserve(doubleSize);
        
        for(uint32_t i = 0; i < doubleSize; i++) {
            cin >> column;
            typeName.push_back(column);
        }
        
        cout << "New table " << tableName << " with column(s) ";
        
        for(uint32_t i = 0; i < size; i++) {
            Column c;
            c.columnType = typeName[i][0];
            c.columnName = typeName[i + size];
            c.index = i;
            tables[tableName].columns[i] = c;
            tables[tableName].nameToIndex[c.columnName] = c;
            cout << c.columnName << " ";
        }
        
        cout << "created\n";
    }
    
    void remove(string command) {
        cin >> command; // tableName
        
        if (!tableAlreadyExists(command)) {
            cout << "Error during REMOVE: " << command << " does not name a table in the database\n";
            return;
        } else {
            tables.erase(command);
            cout << "Table " << command << " deleted\n";
        }
    }
    
    
    bool tableAlreadyExists(string tableName) {
        if (tables.find(tableName) != tables.end()) {
            return true;
        } else {
            return false;
        }
    }
    
    void join(Table &tableOne, Table &tableTwo) {
        unordered_map<TableEntry, vector<uint32_t>> generalHash;
        
        string tableOneColumn;
        string tableTwoColumn;
        string blank;
        
        cin >> tableOneColumn;
        cin >> blank;
        cin >> tableTwoColumn;
        
        uint32_t colOneIndex = tableOne.nameToIndex[tableOneColumn].index;

        uint32_t colTwoIndex = tableTwo.nameToIndex[tableTwoColumn].index;
        
        cin >> blank; //and
        cin >> blank; //print
        
        uint32_t numCols;
        cin >> numCols;
        
        vector<pair<string, uint32_t>> nameToTable;
        nameToTable.reserve(numCols);
        for(uint32_t i = 0; i < numCols; i++) {
            string colName;
            uint32_t whichTable;
            cin >> colName;
            cin >> whichTable;
            nameToTable.push_back(make_pair(colName, whichTable));
            cout << colName <<  " ";
        }
        
        cout << "\n";
        
        if (tableTwo.isHash != 1 || tableTwo.columnNameIndex != tableTwoColumn) {
            tableTwo.generalGenerate(colTwoIndex, generalHash);
            joinCustomIndex(generalHash, tableOne, tableTwo, nameToTable, colOneIndex);
        } else if (tableTwo.isHash == 1 && tableTwo.columnNameIndex == tableTwoColumn) {
            joinIndex(nameToTable, tableOne, tableTwo, colOneIndex);
        }
    }
    
    void joinCustomIndex(unordered_map<TableEntry, vector<uint32_t>> &generalHash, Table &tableOne, Table &tableTwo, vector<pair<string, uint32_t>> &nameToTable, uint32_t colOneIndex) {
        uint32_t rowsPrinted = 0;
        
        for(uint32_t i = 0; i < tableOne.data.size(); i++) {
            if (generalHash.find(tableOne.data[i][colOneIndex]) != generalHash.end()) {
                for(uint32_t j = 0; j < generalHash[tableOne.data[i][colOneIndex]].size(); j++) {
                    for(uint32_t k = 0; k < nameToTable.size(); k++) {
                        string columnName =  nameToTable[k].first;
                        uint32_t table = nameToTable[k].second;
                        
                        if (table == 1) {
                            uint32_t colIndex = tableOne.nameToIndex[columnName].index;
                            cout << tableOne.data[i][colIndex] << " ";
                        } else if (table == 2) {
                            uint32_t colIndex = tableTwo.nameToIndex[columnName].index;
                            cout << tableTwo.data[generalHash[tableOne.data[i][colOneIndex]][j]][colIndex] << " ";
                        }
                    }
                    
                    rowsPrinted++;
                    cout << "\n";
                }
            }
        }
        
        cout << "Printed " << rowsPrinted << " rows from joining " << tableOne.tableName << " to " << tableTwo.tableName << "\n";
    }
    
    void joinIndex(vector<pair<string, uint32_t>> &nameToTable, Table &tableOne, Table &tableTwo, uint32_t colOneIndex) {
        uint32_t rowsPrinted = 0;
        
        for(uint32_t i = 0; i < tableOne.data.size(); i++) {
            if (tableTwo.hashIndex.find(tableOne.data[i][colOneIndex]) != tableTwo.hashIndex.end()) {
                for(uint32_t j = 0; j < tableTwo.hashIndex[tableOne.data[i][colOneIndex]].size(); j++) {
                    for(uint32_t k = 0; k < nameToTable.size(); k++) {
                        string columnName =  nameToTable[k].first;
                        uint32_t table = nameToTable[k].second;
                        
                        if (table == 1) {
                            uint32_t colIndex = tableOne.nameToIndex[columnName].index;
                            cout << tableOne.data[i][colIndex] << " ";
                        } else if (table == 2) {
                            uint32_t colIndex = tableTwo.nameToIndex[columnName].index;
                            cout << tableTwo.data[tableTwo.hashIndex[tableOne.data[i][colOneIndex]][j]][colIndex] << " ";
                        }
                    }
                    
                    rowsPrinted++;
                    cout << "\n";
                }
            }
        }
        
        cout << "Printed " << rowsPrinted << " rows from joining " << tableOne.tableName << " to " << tableTwo.tableName << "\n";
    }
};

int main(int argc, char * argv[]) {
    // insert code here...
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines
    
    Database start;
    
    start.readInput();
    
    return 0;
}
