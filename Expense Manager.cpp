#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <filesystem>
using namespace std;

struct Expense {
    string date;
    string category;
    float amount;
};

vector<Expense> expenses;
float setBudget = 0;
string userType;
string sessionName;
vector<string> categories;
string loggedInUser = "";

map<string, vector<string>> defaultCategories = {
    {"Student", {"Books", "Snacks", "Stationery", "Transport"}},
    {"Employee", {"Lunch", "Commute", "Bills", "Subscriptions"}},
    {"Freelancer", {"Software", "Workspace", "Learning", "Client Meetings"}},
    {"Parent", {"Groceries", "Kids School", "Utilities", "Healthcare"}},
    {"Traveler", {"Transport", "Accommodation", "Food", "Tickets"}},
    {"Gamer", {"Games", "Subscriptions", "Hardware", "Merchandise"}}
};

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

string getUserPath() {
    return "Accounts/" + loggedInUser + "/";
}

bool folderExists(const string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

bool createFolderIfNotExist(const string& path) {
    if (!folderExists(path)) {
        string command = "mkdir \"" + path + "\" 2>nul";
        return system(command.c_str()) == 0;
    }
    return true;
}

bool savePassword(const string& path, const string& password) {
    ofstream file(path + "password.txt");
    if (!file.is_open()) return false;
    file << password;
    file.close();
    return true;
}

bool verifyPassword(const string& path, const string& password) {
    ifstream file(path + "password.txt");
    if (!file.is_open()) return false;
    string stored;
    getline(file, stored);
    return stored == password;
}

void clearScreen() {
    system("cls");
}

vector<string> listSessions() {
    vector<string> sessions;
    string path = getUserPath();
    string command = "dir /b \"" + path + "\" > temp_sessions.txt";
    system(command.c_str());
    ifstream tempFile("temp_sessions.txt");
    string line;
    while (getline(tempFile, line)) {
        if (line.find("_session.txt") != string::npos) {
            sessions.push_back(line.substr(0, line.find("_session.txt")));
        }
    }
    tempFile.close();
    remove("temp_sessions.txt");
    return sessions;
}

void loginMenu() {
    int choice;
    cout << "\nWelcome to Expense Manager\n";
    cout << "===============================\n";
    cout << "\n1. Login to Existing Account\n";
    cout << "2. Create New Account\n";
    cout << "3. Refresh\n";
    cout << "\nEnter your choice: ";
    cin >> choice;

    if (choice == 3) {
        clearScreen();
        loginMenu();
        return;
    }

    string username, password;
    cout << "Enter your username: ";
    cin >> username;
    string userPath = "Accounts/" + username + "/";

    if (choice == 1) {
        if (!folderExists(userPath)) {
            cout << "Account not found! Please create a new one.\n";
            loginMenu();
            return;
        } else {
            cout << "Enter your password: ";
            cin >> password;
            if (!verifyPassword(userPath, password)) {
                cout << "Incorrect password!\n";
                loginMenu();
                return;
            }
            loggedInUser = username;
            cout << "Logged in successfully!\n";
        }
    } else if (choice == 2) {
        cout << "Create a password: ";
        cin >> password;
        if (createFolderIfNotExist(userPath) && savePassword(userPath, password)) {
            loggedInUser = username;
            cout << "Account created and logged in!\n";
        } else {
            cout << "Failed to create account. Try again.\n";
            loginMenu();
            return;
        }
    } else {
        cout << "Invalid choice. Try again.\n";
        loginMenu();
        return;
    }
}

void saveSession() {
    ofstream file(getUserPath() + sessionName + "_session.txt");
    file << userType << "\n";
    file << setBudget << "\n";
    for (string& cat : categories) file << cat << "\n";
    file.close();
}

void loadSession() {
    ifstream file(getUserPath() + sessionName + "_session.txt");
    getline(file, userType);
    file >> setBudget;
    file.ignore();
    string cat;
    categories.clear();
    while (getline(file, cat)) categories.push_back(cat);
    file.close();
}

void saveExpenses() {
    ofstream file(getUserPath() + sessionName + "_expenses.txt");
    for (Expense& e : expenses) {
        file << e.date << "," << e.category << "," << e.amount << "\n";
    }
    file.close();
}

void loadExpenses() {
    ifstream file(getUserPath() + sessionName + "_expenses.txt");
    string line;
    expenses.clear();
    while (getline(file, line)) {
        Expense e;
        size_t p1 = line.find(',');
        size_t p2 = line.rfind(',');
        e.date = line.substr(0, p1);
        e.category = line.substr(p1 + 1, p2 - p1 - 1);
        e.amount = stof(line.substr(p2 + 1));
        expenses.push_back(e);
    }
    file.close();
}

void addExpense() {
    if (categories.empty()) {
        cout << "\n[!] No categories found. Please create or load a session first.\n";
        return;
    }
    Expense e;
    cout << "\nEnter date (YYYY-MM-DD): ";
    cin >> e.date;
    cout << "Available categories: ";
    for (auto& c : categories) cout << c << " ";
    cout << "\nEnter category: ";
    cin >> e.category;
    cout << "Enter amount (Rs: ): ";
    cin >> e.amount;
    expenses.push_back(e);
    saveExpenses();
    cout << "Expense added successfully!\n";
}

void addNewCategory() {
    string newCat;
    cout << "\nEnter new category name: ";
    cin >> newCat;

    for (auto& cat : categories) {
        if (cat == newCat) {
            cout << "Category already exists!\n";
            return;
        }
    }

    categories.push_back(newCat);
    saveSession();
    cout << "Category added!\n";
}

void showExpenses() {
    if (expenses.empty()) {
        cout << "\nNo expenses to show!\n";
        return;
    }

    cout << "\n--- All Expenses ---\n";
    cout << left << setw(5) << "#" << setw(15) << "Date" << setw(20) << "Category" << "Amount (Rs:)\n";
    cout << "----------------------------------------------------------\n";

    for (size_t i = 0; i < expenses.size(); ++i) {
        cout << setw(5) << i + 1
             << setw(15) << expenses[i].date
             << setw(20) << expenses[i].category
             << expenses[i].amount << "\n";
    }

    int action;
    cout << "\n1. Edit Expense\n2. Delete Expense\n3. Go Back\nChoose an action: ";
    cin >> action;

    if (action == 1) {
        int idx;
        cout << "Enter expense number to edit: ";
        cin >> idx;
        if (idx < 1 || idx > expenses.size()) {
            cout << "Invalid index.\n";
            return;
        }
        --idx;
        cout << "Enter new date (YYYY-MM-DD): ";
        cin >> expenses[idx].date;
        cout << "Enter new category: ";
        cin >> expenses[idx].category;
        cout << "Enter new amount: ";
        cin >> expenses[idx].amount;
        saveExpenses();
        cout << "Expense updated!\n";

    } else if (action == 2) {
        int idx;
        cout << "Enter expense number to delete: ";
        cin >> idx;
        if (idx < 1 || idx > expenses.size()) {
            cout << "Invalid index.\n";
            return;
        }
        --idx;
        expenses.erase(expenses.begin() + idx);
        saveExpenses();
        cout << "Expense deleted!\n";
    } else {
        cout << "Returning to menu...\n";
    }
}

void budgetSummary() {
    float totalSpent = 0;
    for (auto& e : expenses) totalSpent += e.amount;
    float remaining = setBudget - totalSpent;

    cout << "\n--- Budget Summary ---\n";
    cout << "Set Budget: Rs: " << setBudget << endl;
    cout << "Total Spent: Rs: " << totalSpent << endl;
    cout << "Remaining: Rs: " << remaining << endl;

    if (remaining < 0) {
        cout << "Budget exceeded! Be careful with spending.\n";
    }
}

void categorySummary() {
    map<string, float> categoryTotals;
    for (auto& cat : categories) categoryTotals[cat] = 0;

    for (auto& e : expenses) {
        categoryTotals[e.category] += e.amount;
    }

    cout << "\n--- Category-wise Summary ---\n";
    for (auto& pair : categoryTotals) {
        cout << pair.first << ": Rs: " << pair.second << endl;
    }
}

void createNewSession() {
    cout << "\nEnter a name for this session: ";
    cin >> sessionName;
    cout << "Choose your role (Student, Employee, Freelancer, Parent, Traveler, Gamer): ";
    cin >> userType;

    cout << "Enter your monthly budget (Rs: ): ";
    cin >> setBudget;

    auto it = defaultCategories.find(userType);
    if (it != defaultCategories.end()) {
        categories = it->second;
    } else {
        cout << "Add your preferred expense categories (type 'done' to finish):\n";
        string cat;
        categories.clear();
        while (true) {
            cout << "Category: ";
            cin >> cat;
            if (cat == "done") break;
            categories.push_back(cat);
        }
    }

    saveSession();
    expenses.clear();
    saveExpenses();
    cout << "Session created and saved!\n";
}

void loadExistingSession() {
    vector<string> sessions = listSessions();
    if (sessions.empty()) {
        cout << "\nNo sessions found for this account.\n";
        return;
    }

    cout << "\nAvailable sessions: ";
    for (auto& s : sessions) cout << "[" << s << "] ";
    cout << "\nEnter session name to load: ";
    cin >> sessionName;
    if (!fileExists(getUserPath() + sessionName + "_session.txt")) {
        cout << "Session does not exist.\n";
        sessionName = "";
        return;
    }
    loadSession();
    loadExpenses();
    cout << "Session loaded successfully!\n";
}

int main() {
    int mainChoice, choice;
    cout << "Welcome to Expense Tracker & Budget Advisor\n";
    loginMenu();

    do {
        cout << "\n===== Session Menu =====\n";
        cout << "1. Create New Session\n";
        cout << "2. Load Existing Session\n";
        cout << "3. Refresh\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> mainChoice;

        if (mainChoice == 3) {
            clearScreen();
            continue;
        }

        switch (mainChoice) {
            case 1: createNewSession(); break;
            case 2: loadExistingSession(); break;
            case 4: cout << "\nExiting program.\n"; return 0;
            default: cout << "Invalid choice. Try again.\n"; continue;
        }

        if (sessionName.empty()) continue;

        do {
            cout << "\n===== Main Menu =====\n";
            cout << "1. Add New Category\n";
            cout << "2. Add New Expense\n";
            cout << "3. Show All Expenses\n";
            cout << "4. Show Budget Summary\n";
            cout << "5. Show Category-wise Summary\n";
            cout << "6. Refresh\n";
            cout << "7. Exit to Session Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 6) {
                clearScreen();
                continue;
            }

            switch (choice) {
                case 1: addNewCategory(); break;
                case 2: addExpense(); break;
                case 3: showExpenses(); break;
                case 4: budgetSummary(); break;
                case 5: categorySummary(); break;
                case 7: cout << "Returning to session menu...\n"; break;
                default: cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 7);

    } while (mainChoice != 4);

    return 0;
}
