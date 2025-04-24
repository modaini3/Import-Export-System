#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <limits>
#include <vector>

using namespace std;

// Constants
const int MAX_CASES = 100;
const int MAX_MANAGERS = 20;
const int MAX_ACTIONS = 50;
const int MAX_ASSIGNED_MANAGERS = 5;
const string DATA_FILE = "IE.txt";

// Data Structures
struct Action {
    string description;
    string date;
    string time;
    string manager;
};

struct Manager {
    string name;
    string department;
    string password;
    bool active = true;
};

struct Case {
    int id;
    string title;
    string description;
    string creationDate;
    string creationTime;
    string source;
    string status = "Open";
    string assignedManagers[MAX_ASSIGNED_MANAGERS];
    int assignedManagerCount = 0;
    Action actions[MAX_ACTIONS];
    int actionCount = 0;
};

// Global Variables
Case cases[MAX_CASES];
Manager managers[MAX_MANAGERS];
int caseCount = 0;
int managerCount = 0;
string currentUser;
bool currentUserIsManager = false;
int nextCaseId = 1000;

// Function Prototypes
void login();
void mainMenu();
void caseManagementMenu();
void managerManagementMenu();
void reportMenu();
void addCase();
void viewCases(bool brief = false);
bool viewCaseDetails(int caseId);
void assignManagerToCase();
void addActionToCase();
void exportCase();
void closeCase();
void editCase();
void deleteCase();
void addManager();
void viewManagers();
void editManager();
void toggleManagerStatus();
void deleteManager();
void generateReport();
void saveData();
void loadData();
void clearInputBuffer();
string getCurrentDate();
string getCurrentTime();
int findCaseIndex(int caseId);
int findManagerIndex(const string& managerName);
bool isManagerAssignedToCase(const Case& c, const string& managerName);
void printCaseHeader();
void printCaseSummary(const Case& c);
bool validateAdminLogin(const string& username, const string& password);
void addNewAdmin();
void adminMainMenu();

int main() {
    loadData();
    login();
    saveData();
    return 0;
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isManagerAssignedToCase(const Case& c, const string& managerName) {
    for (int i = 0; i < c.assignedManagerCount; i++) {
        if (c.assignedManagers[i] == managerName) {
            return true;
        }
    }
    return false;
}

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, 11, "%Y-%m-%d", ltm);
    return string(buffer);
}

string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[9];
    strftime(buffer, 9, "%H:%M:%S", ltm);
    return string(buffer);
}

bool validateAdminLogin(const string& username, const string& password) {
    ifstream credFile("Admins.txt");
    if (!credFile) {
        cerr << "Error: Admin credentials file not found!" << endl;
        return false;
    }

    string line;
    while (getline(credFile, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != string::npos) {
            string storedUsername = line.substr(0, colonPos);
            string storedPassword = line.substr(colonPos + 1);
            if (username == storedUsername && password == storedPassword) {
                return true;
            }
        }
    }
    return false;
}

void addNewAdmin() {
    if (currentUserIsManager) {
        cout << "Only admins can add other admins." << endl;
        return;
    }

    ofstream credFile("Admins.txt", ios::app);
    if (!credFile) {
        cerr << "Error opening credentials file!" << endl;
        return;
    }

    string username, password;
    cout << "Enter new admin username: ";
    getline(cin, username);
    cout << "Enter new admin password: ";
    getline(cin, password);

    if (validateAdminLogin(username, password)) {
        cout << "Admin already exists!" << endl;
        return;
    }

    credFile << username << ":" << password << endl;
    cout << "Admin added successfully." << endl;
}

void adminMainMenu() {
    while (true) {
        cout << "\n=== Admin Main Menu ===" << endl;
        cout << "1. Case Management" << endl;
        cout << "2. Manager Management" << endl;
        cout << "3. Reports" << endl;
        cout << "4. Add New Admin" << endl;
        cout << "5. Logout" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        clearInputBuffer();
        
        switch (choice) {
            case 1: caseManagementMenu(); break;
            case 2: managerManagementMenu(); break;
            case 3: reportMenu(); break;
            case 4: addNewAdmin(); break;
            case 5: return;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

void login() {
    while (true) {
        cout << "=== IE System Login ===" << endl;
        cout << "1. Login as Admin" << endl;
        cout << "2. Login as Manager" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        clearInputBuffer();
        
        if (choice == 3) {
            cout << "Exiting system..." << endl;
            exit(0);
        }

        if (choice == 1) {
            string username, password;
            cout << "Enter admin username: ";
            getline(cin, username);
            cout << "Enter admin password: ";
            getline(cin, password);

            if (!validateAdminLogin(username, password)) {
                cout << "Invalid admin credentials. Access denied." << endl;
                continue;
            }

            currentUser = username;
            currentUserIsManager = false;
            cout << "Welcome, Admin " << currentUser << "!" << endl;
            adminMainMenu();
            return;
        }
        else if (choice == 2) {
            string managerName, password;
            cout << "Enter your name: ";
            getline(cin, managerName);
            
            int managerIndex = findManagerIndex(managerName);
            if (managerIndex == -1) {
                cout << "Manager not found. Access denied." << endl;
                continue;
            }
            
            if (!managers[managerIndex].active) {
                cout << "Your account is inactive. Please contact admin." << endl;
                continue;
            }

            cout << "Enter your password: ";
            getline(cin, password);
            
            if (password != managers[managerIndex].password) {
                cout << "Invalid password. Access denied." << endl;
                continue;
            }

            currentUser = managerName;
            currentUserIsManager = true;
            cout << "Welcome, Manager " << currentUser << "!" << endl;
            mainMenu();
            return;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void mainMenu() {
    while (true) {
        cout << "\n=== Main Menu ===" << endl;
        cout << "1. Case Management" << endl;
        if (!currentUserIsManager) {
            cout << "2. Manager Management" << endl;
            cout << "3. Reports" << endl;
            cout << "4. Logout" << endl;
        } else {
            cout << "2. Reports" << endl;
            cout << "3. Logout" << endl;
        }
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        clearInputBuffer();
        
        if (currentUserIsManager && choice >= 2) {
            choice += 1;
        }
        
        switch (choice) {
            case 1: caseManagementMenu(); break;
            case 2: 
                if (!currentUserIsManager) {
                    managerManagementMenu();
                } else {
                    reportMenu();
                }
                break;
            case 3: 
                if (!currentUserIsManager) {
                    reportMenu();
                } else {
                    return; 
                }
                break;
            case 4: return;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

void caseManagementMenu() {
    while (true) {
        cout << "\n=== Case Management ===" << endl;
        cout << "1. Add New Case" << endl;
        cout << "2. View All Cases" << endl;
        cout << "3. View Case Details" << endl;
        if (!currentUserIsManager) {
            cout << "4. Assign Manager to Case" << endl;
            cout << "5. Edit Case" << endl;
            cout << "6. Delete Case" << endl;
            cout << "7. Add Action to Case" << endl;
            cout << "8. Export Case" << endl;
            cout << "9. Close Case" << endl;
            cout << "10. Back to Main Menu" << endl;
        } else {
            cout << "4. Add Action to Case" << endl;
            cout << "5. Export Case" << endl;
            cout << "6. Close Case" << endl;
            cout << "7. Back to Main Menu" << endl;
        }
        cout << "Enter your choice: ";
    
        int choice;
        cin >> choice;
        clearInputBuffer();

        if ((!currentUserIsManager && choice == 10) || (currentUserIsManager && choice == 7)) {
            break;
        }
        
        if (currentUserIsManager) {
            if (choice >= 4) choice += 3;
        }
    
        switch (choice) {
            case 1: addCase(); break;
            case 2: viewCases(); break;
            case 3: {
                cout << "Enter Case ID: ";
                int caseId;
                cin >> caseId;
                clearInputBuffer();
                viewCaseDetails(caseId);
                break;
            }
            case 4: assignManagerToCase(); break;
            case 5: editCase(); break;
            case 6: deleteCase(); break;
            case 7: addActionToCase(); break;
            case 8: exportCase(); break;
            case 9: closeCase(); break;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

void managerManagementMenu() {
    while (true) {
        cout << "\n=== Manager Management ===" << endl;
        cout << "1. Add New Manager" << endl;
        cout << "2. View All Managers" << endl;
        cout << "3. Edit Manager" << endl;
        cout << "4. Toggle Manager Status" << endl;
        cout << "5. Delete Manager" << endl;
        cout << "6. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        clearInputBuffer();
        
        if (choice == 6) break;
        
        switch (choice) {
            case 1: addManager(); break;
            case 2: viewManagers(); break;
            case 3: editManager(); break;
            case 4: toggleManagerStatus(); break;
            case 5: deleteManager(); break;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

void reportMenu() {
    while (true) {
        cout << "\n=== Reports ===" << endl;
        cout << "1. Generate Case Report" << endl;
        cout << "2. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        clearInputBuffer();
        
        if (choice == 2) break;
        
        switch (choice) {
            case 1: generateReport(); break;
            default: cout << "Invalid choice!" << endl;
        }
    }
}

void addCase() {
    if (caseCount >= MAX_CASES) {
        cout << "Maximum number of cases reached!" << endl;
        return;
    }

    Case& newCase = cases[caseCount];
    newCase.id = nextCaseId++;
    cout << "Enter case title: ";
    getline(cin, newCase.title);
    cout << "Enter case description: ";
    getline(cin, newCase.description);
    newCase.creationDate = getCurrentDate();
    newCase.creationTime = getCurrentTime();
    cout << "Enter case source: ";
    getline(cin, newCase.source);
    newCase.status = "Open";
    newCase.assignedManagerCount = 0;
    newCase.actionCount = 0;

    caseCount++;
    cout << "Case added successfully with ID: " << newCase.id << endl;
}

void viewCases(bool brief) {
    if (caseCount == 0) {
        cout << "No cases found." << endl;
        return;
    }

    if (brief) {
        printCaseHeader();
        for (int i = 0; i < caseCount; i++) {
            if (!currentUserIsManager || isManagerAssignedToCase(cases[i], currentUser)) {
                printCaseSummary(cases[i]);
            }
        }
    } else {
        bool hasCases = false;
        for (int i = 0; i < caseCount; i++) {
            if (!currentUserIsManager || isManagerAssignedToCase(cases[i], currentUser)) {
                viewCaseDetails(cases[i].id);
                cout << "------------------------" << endl;
                hasCases = true;
            }
        }
        if (currentUserIsManager && !hasCases) {
            cout << "You are not assigned to any cases." << endl;
        }
    }
}

bool viewCaseDetails(int caseId) {
    int index = findCaseIndex(caseId);
    if (index == -1) {
        cout << "Case not found." << endl;
        return false;
    }

    Case& c = cases[index];

    if (currentUserIsManager && !isManagerAssignedToCase(c, currentUser)) {
        cout << "Access denied. You are not assigned to this case." << endl;
        return false;
    }

    cout << "\n=== Case Details ===" << endl;
    cout << "ID: " << c.id << endl;
    cout << "Title: " << c.title << endl;
    cout << "Description: " << c.description << endl;
    cout << "Created: " << c.creationDate << " at " << c.creationTime << endl;
    cout << "Source: " << c.source << endl;
    cout << "Status: " << c.status << endl;

    cout << "\nAssigned Managers (" << c.assignedManagerCount << "):" << endl;
    for (int i = 0; i < c.assignedManagerCount; i++) {
        cout << " - " << c.assignedManagers[i] << endl;
    }

    cout << "\nActions (" << c.actionCount << "):" << endl;
    for (int i = 0; i < c.actionCount; i++) {
        cout << " - " << c.actions[i].date << " " << c.actions[i].time 
             << " by " << c.actions[i].manager << ": " 
             << c.actions[i].description << endl;
    }

    return true;
}

void assignManagerToCase() {
    if (currentUserIsManager) {
        cout << "Only admin can assign managers to cases." << endl;
        return;
    }

    cout << "Enter Case ID: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    Case& c = cases[caseIndex];
    if (c.assignedManagerCount >= MAX_ASSIGNED_MANAGERS) {
        cout << "Maximum managers already assigned to this case." << endl;
        return;
    }

    cout << "Available Managers:" << endl;
    for (int i = 0; i < managerCount; i++) {
        if (managers[i].active) {
            cout << " - " << managers[i].name << " (" << managers[i].department << ")" << endl;
        }
    }

    cout << "Enter manager name to assign: ";
    string managerName;
    getline(cin, managerName);

    int managerIndex = findManagerIndex(managerName);
    if (managerIndex == -1 || !managers[managerIndex].active) {
        cout << "Manager not found or inactive." << endl;
        return;
    }

    for (int i = 0; i < c.assignedManagerCount; i++) {
        if (c.assignedManagers[i] == managerName) {
            cout << "Manager already assigned to this case." << endl;
            return;
        }
    }

    c.assignedManagers[c.assignedManagerCount++] = managerName;
    if (c.status == "Open") {
        c.status = "Assigned";
    }
    cout << "Manager assigned successfully." << endl;
}

void addActionToCase() {
    cout << "Enter Case ID: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    Case& c = cases[caseIndex];
    if (c.actionCount >= MAX_ACTIONS) {
        cout << "Maximum actions reached for this case." << endl;
        return;
    }

    if (currentUserIsManager) {
        bool isAssigned = false;
        for (int i = 0; i < c.assignedManagerCount; i++) {
            if (c.assignedManagers[i] == currentUser) {
                isAssigned = true;
                break;
            }
        }
        if (!isAssigned) {
            cout << "You are not assigned to this case." << endl;
            return;
        }
    }

    Action& action = c.actions[c.actionCount++];
    cout << "Enter action description: ";
    getline(cin, action.description);
    action.date = getCurrentDate();
    action.time = getCurrentTime();
    action.manager = currentUser;

    if (c.status == "Assigned") {
        c.status = "In Progress";
    }

    cout << "Action added successfully." << endl;
}

void exportCase() {
    cout << "Enter Case ID: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    Case& c = cases[caseIndex];
    if (c.status == "Closed") {
        cout << "Case is already closed and cannot be exported." << endl;
        return;
    }

    cout << "\nAvailable Managers:" << endl;
    int availableCount = 0;
    for (int i = 0; i < managerCount; i++) {
        if (managers[i].active) {
            bool alreadyAssigned = false;
            for (int j = 0; j < c.assignedManagerCount; j++) {
                if (c.assignedManagers[j] == managers[i].name) {
                    alreadyAssigned = true;
                    break;
                }
            }
            if (!alreadyAssigned) {
                cout << " - " << managers[i].name << " (" << managers[i].department << ")" << endl;
                availableCount++;
            }
        }
    }

    if (availableCount == 0) {
        cout << "No available managers to export to." << endl;
        return;
    }

    cout << "Enter manager name to export to (or 'cancel' to abort): ";
    string managerName;
    getline(cin, managerName);

    if (managerName == "cancel") {
        cout << "Export cancelled." << endl;
        return;
    }

    int managerIndex = findManagerIndex(managerName);
    if (managerIndex == -1 || !managers[managerIndex].active) {
        cout << "Invalid manager selection. Please choose from available managers." << endl;
        return;
    }

    for (int i = 0; i < c.assignedManagerCount; i++) {
        if (c.assignedManagers[i] == managerName) {
            cout << "Case is already assigned to this manager." << endl;
            return;
        }
    }

    cout << "Enter reason for export: ";
    string reason;
    getline(cin, reason);
    if (reason.empty()) {
        reason = "No reason provided";
    }

    cout << "\nYou are about to export this case to " << managerName << endl;
    cout << "Reason: " << reason << endl;
    cout << "Are you sure you want to proceed? (y/n): ";
    char confirm;
    cin >> confirm;
    clearInputBuffer();

    if (tolower(confirm) != 'y') {
        cout << "Export cancelled." << endl;
        return;
    }

    if (c.assignedManagerCount < MAX_ASSIGNED_MANAGERS) {
        c.assignedManagers[c.assignedManagerCount++] = managerName;
        
        if (c.actionCount < MAX_ACTIONS) {
            Action& action = c.actions[c.actionCount++];
            action.description = "Case exported to " + managerName + ". Reason: " + reason;
            action.date = getCurrentDate();
            action.time = getCurrentTime();
            action.manager = currentUser;
        }
        
        c.status = "Exported";
        
        cout << "\nCase successfully exported to " << managerName << endl;
        cout << "Export details have been recorded." << endl;
    } else {
        cout << "Cannot export: Maximum managers already assigned to this case." << endl;
    }
}

void closeCase() {
    cout << "Enter Case ID: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    Case& c = cases[caseIndex];
    if (c.status == "Closed") {
        cout << "Case is already closed." << endl;
        return;
    }

    c.status = "Closed";
    cout << "Case closed successfully." << endl;
}

void editCase() {
    if (currentUserIsManager) {
        cout << "Only admin can edit cases." << endl;
        return;
    }

    cout << "Enter Case ID: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    Case& c = cases[caseIndex];
    if (c.status == "Closed") {
        cout << "Cannot edit closed case." << endl;
        return;
    }

    cout << "Current title: " << c.title << endl;
    cout << "Enter new title (or press Enter to keep current): ";
    string newTitle;
    getline(cin, newTitle);
    if (!newTitle.empty()) {
        c.title = newTitle;
    }

    cout << "Current description: " << c.description << endl;
    cout << "Enter new description (or press Enter to keep current): ";
    string newDesc;
    getline(cin, newDesc);
    if (!newDesc.empty()) {
        c.description = newDesc;
    }

    cout << "Current source: " << c.source << endl;
    cout << "Enter new source (or press Enter to keep current): ";
    string newSource;
    getline(cin, newSource);
    if (!newSource.empty()) {
        c.source = newSource;
    }

    cout << "Case updated successfully." << endl;
}

void deleteCase() {
    if (currentUserIsManager) {
        cout << "Only admin can delete cases." << endl;
        return;
    }

    cout << "Enter Case ID to delete: ";
    int caseId;
    cin >> caseId;
    clearInputBuffer();

    int caseIndex = findCaseIndex(caseId);
    if (caseIndex == -1) {
        cout << "Case not found." << endl;
        return;
    }

    for (int i = caseIndex; i < caseCount - 1; i++) {
        cases[i] = cases[i + 1];
    }
    caseCount--;

    cout << "Case deleted successfully." << endl;
}

void addManager() {
    if (managerCount >= MAX_MANAGERS) {
        cout << "Maximum number of managers reached." << endl;
        return;
    }

    Manager& m = managers[managerCount++];
    cout << "Enter manager name: ";
    getline(cin, m.name);
    cout << "Enter department: ";
    getline(cin, m.department);
    cout << "Set password: ";
    getline(cin, m.password);
    m.active = true;

    cout << "Manager added successfully." << endl;
}

void viewManagers() {
    if (managerCount == 0) {
        cout << "No managers found." << endl;
        return;
    }

    cout << "\n=== Manager List ===" << endl;
    for (int i = 0; i < managerCount; i++) {
        cout << "Name: " << managers[i].name << endl;
        cout << "Department: " << managers[i].department << endl;
        cout << "Status: " << (managers[i].active ? "Active" : "Inactive") << endl;
        cout << "---------------------" << endl;
    }
}

void editManager() {
    cout << "Enter manager name to edit: ";
    string name;
    getline(cin, name);

    int index = findManagerIndex(name);
    if (index == -1) {
        cout << "Manager not found." << endl;
        return;
    }

    Manager& m = managers[index];
    cout << "Current department: " << m.department << endl;
    cout << "Enter new department (or press Enter to keep current): ";
    string newDept;
    getline(cin, newDept);
    if (!newDept.empty()) {
        m.department = newDept;
    }

    cout << "Manager updated successfully." << endl;
}

void toggleManagerStatus() {
    cout << "Enter manager name to toggle status: ";
    string name;
    getline(cin, name);

    int index = findManagerIndex(name);
    if (index == -1) {
        cout << "Manager not found." << endl;
        return;
    }

    managers[index].active = !managers[index].active;
    cout << "Manager status updated to: " << (managers[index].active ? "Active" : "Inactive") << endl;
}

void deleteManager() {
    if (currentUserIsManager) {
        cout << "Only admin can delete managers." << endl;
        return;
    }

    if (managerCount <= 0) {
        cout << "No managers to delete." << endl;
        return;
    }

    cout << "Enter manager name to delete: ";
    string name;
    getline(cin, name);

    int index = findManagerIndex(name);
    if (index == -1) {
        cout << "Manager not found." << endl;
        return;
    }

    for (int i = 0; i < caseCount; i++) {
        for (int j = 0; j < cases[i].assignedManagerCount; j++) {
            if (cases[i].assignedManagers[j] == name) {
                cout << "Cannot delete manager. They are assigned to case ID: " 
                     << cases[i].id << endl;
                return;
            }
        }
    }

    for (int i = index; i < managerCount - 1; i++) {
        managers[i] = managers[i + 1];
    }
    managerCount--;

    cout << "Manager deleted successfully." << endl;
}

void generateReport() {
    if (caseCount == 0) {
        cout << "No cases to report." << endl;
        return;
    }

    string filename = "case_report_" + getCurrentDate() + ".txt";
    ofstream report(filename);

    if (!report) {
        cerr << "Error creating report file!" << endl;
        return;
    }

    report << "=== Case Management System Report ===" << endl;
    report << "Generated on: " << getCurrentDate() << " at " << getCurrentTime() << endl;
    report << "Total cases: " << caseCount << endl << endl;

    for (int i = 0; i < caseCount; i++) {
        Case& c = cases[i];
        report << "Case ID: " << c.id << endl;
        report << "Title: " << c.title << endl;
        report << "Status: " << c.status << endl;
        report << "Created: " << c.creationDate << endl;
        report << "Assigned Managers: ";
        for (int j = 0; j < c.assignedManagerCount; j++) {
            report << c.assignedManagers[j];
            if (j < c.assignedManagerCount - 1) report << ", ";
        }
        report << endl;
        report << "Action Count: " << c.actionCount << endl;
        report << "------------------------" << endl;
    }

    report.close();
    cout << "Report generated: " << filename << endl;
}

void saveData() {
    ofstream outFile(DATA_FILE);
    if (!outFile) {
        cerr << "Error opening file for writing!" << endl;
        return;
    }

    // Save managers with proper formatting
    outFile << "=== MANAGERS ===" << endl;
    for (int i = 0; i < managerCount; i++) {
        outFile << "Manager " << (i+1) << ":" << endl;
        outFile << "  Name: " << managers[i].name << endl;
        outFile << "  Department: " << managers[i].department << endl;
        outFile << "  Password: " << managers[i].password << endl;
        outFile << "  Status: " << (managers[i].active ? "Active" : "Inactive") << endl;
        outFile << endl; // Blank line between managers
    }

    // Save cases with proper formatting
    outFile << "=== CASES ===" << endl;
    for (int i = 0; i < caseCount; i++) {
        Case& c = cases[i];
        outFile << "Case ID: " << c.id << endl;
        outFile << "  Title: " << c.title << endl;
        outFile << "  Description: " << c.description << endl;
        outFile << "  Created: " << c.creationDate << " at " << c.creationTime << endl;
        outFile << "  Source: " << c.source << endl;
        outFile << "  Status: " << c.status << endl;
        
        // Save assigned managers
        outFile << "  Assigned Managers (" << c.assignedManagerCount << "):" << endl;
        for (int j = 0; j < c.assignedManagerCount; j++) {
            outFile << "    - " << c.assignedManagers[j] << endl;
        }
        
        // Save actions
        outFile << "  Actions (" << c.actionCount << "):" << endl;
        for (int j = 0; j < c.actionCount; j++) {
            outFile << "    - " << c.actions[j].date << " " << c.actions[j].time 
                   << " by " << c.actions[j].manager << ": " 
                   << c.actions[j].description << endl;
        }
        outFile << endl; // Blank line between cases
    }

    // Save system information
    outFile << "=== SYSTEM ===" << endl;
    outFile << "Next Case ID: " << nextCaseId << endl;

    outFile.close();
    cout << "Data saved successfully with improved formatting." << endl;
}

void loadData() {
    ifstream inFile(DATA_FILE);
    if (!inFile) {
        cout << "No existing data file found. Starting with empty database." << endl;
        return;
    }

    string line;
    string currentSection;
    managerCount = 0;
    caseCount = 0;
    nextCaseId = 1000;

    while (getline(inFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Check for section headers
        if (line.find("=== MANAGERS ===") != string::npos) {
            currentSection = "managers";
            continue;
        } else if (line.find("=== CASES ===") != string::npos) {
            currentSection = "cases";
            continue;
        } else if (line.find("=== SYSTEM ===") != string::npos) {
            currentSection = "system";
            continue;
        }

        if (currentSection == "managers" && managerCount < MAX_MANAGERS) {
            if (line.find("Manager ") == 0) {
                // New manager - the details will follow in subsequent lines
                continue;
            } else if (line.find("  Name: ") == 0) {
                managers[managerCount].name = line.substr(8);
            } else if (line.find("  Department: ") == 0) {
                managers[managerCount].department = line.substr(14);
            } else if (line.find("  Password: ") == 0) {
                managers[managerCount].password = line.substr(12);
            } else if (line.find("  Status: ") == 0) {
                managers[managerCount].active = (line.substr(10) == "Active");
                managerCount++;
            }
        }
        else if (currentSection == "cases" && caseCount < MAX_CASES) {
            if (line.find("Case ID: ") == 0) {
                // New case
                try {
                    cases[caseCount].id = stoi(line.substr(9));
                    if (cases[caseCount].id >= nextCaseId) {
                        nextCaseId = cases[caseCount].id + 1;
                    }
                } catch (...) {
                    cerr << "Error parsing case ID" << endl;
                    continue;
                }
            } 
            else if (line.find("  Title: ") == 0) {
                cases[caseCount].title = line.substr(9);
            }
            else if (line.find("  Description: ") == 0) {
                cases[caseCount].description = line.substr(15);
            }
            else if (line.find("  Created: ") == 0) {
                size_t atPos = line.find(" at ");
                if (atPos != string::npos) {
                    cases[caseCount].creationDate = line.substr(11, atPos - 11);
                    cases[caseCount].creationTime = line.substr(atPos + 4);
                }
            }
            else if (line.find("  Source: ") == 0) {
                cases[caseCount].source = line.substr(10);
            }
            else if (line.find("  Status: ") == 0) {
                cases[caseCount].status = line.substr(10);
            }
            else if (line.find("  Assigned Managers (") == 0) {
                // Managers will follow in subsequent lines
                cases[caseCount].assignedManagerCount = 0;
            }
            else if (line.find("    - ") == 0 && cases[caseCount].assignedManagerCount < MAX_ASSIGNED_MANAGERS) {
                cases[caseCount].assignedManagers[cases[caseCount].assignedManagerCount++] = line.substr(6);
            }
            else if (line.find("  Actions (") == 0) {
                // Actions will follow in subsequent lines
                cases[caseCount].actionCount = 0;
            }
            else if (line.find("    - ") == 0 && cases[caseCount].actionCount < MAX_ACTIONS) {
                Action& a = cases[caseCount].actions[cases[caseCount].actionCount];
                size_t byPos = line.find(" by ");
                size_t colonPos = line.find(": ", byPos);
                
                if (byPos != string::npos && colonPos != string::npos) {
                    string dateTime = line.substr(6, byPos - 6);
                    size_t spacePos = dateTime.find(' ');
                    
                    if (spacePos != string::npos) {
                        a.date = dateTime.substr(0, spacePos);
                        a.time = dateTime.substr(spacePos + 1);
                        a.manager = line.substr(byPos + 4, colonPos - (byPos + 4));
                        a.description = line.substr(colonPos + 2);
                        cases[caseCount].actionCount++;
                    }
                }
            }
            else if (line.empty()) {
                // Blank line indicates end of case
                caseCount++;
            }
        }
        else if (currentSection == "system") {
            if (line.find("Next Case ID: ") == 0) {
                try {
                    nextCaseId = stoi(line.substr(14));
                } catch (...) {
                    cerr << "Error parsing next case ID" << endl;
                }
            }
        }
    }

    inFile.close();
    cout << "Data loaded successfully from readable format. " 
         << caseCount << " cases and " << managerCount << " managers found." << endl;
}

int findCaseIndex(int caseId) {
    for (int i = 0; i < caseCount; i++) {
        if (cases[i].id == caseId) {
            return i;
        }
    }
    return -1;
}

int findManagerIndex(const string& managerName) {
    for (int i = 0; i < managerCount; i++) {
        if (managers[i].name == managerName) {
            return i;
        }
    }
    return -1;
}

void printCaseHeader() {
    cout << "ID\tTitle\t\tStatus\t\tCreated" << endl;
    cout << "------------------------------------------------" << endl;
}

void printCaseSummary(const Case& c) {
    cout << c.id << "\t" 
         << (c.title.length() > 10 ? c.title.substr(0, 7) + "..." : c.title) << "\t"
         << c.status << "\t"
         << c.creationDate << endl;
}