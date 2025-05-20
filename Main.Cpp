#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <fstream>
#include <filesystem>
using namespace std;

const string basePath = "D:/Abdullah/MAJU/University Study/Computer Programming  LAB/Project Expense-Manager/Data Save/";

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

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

void saveSession() {
    ofstream file(basePath + sessionName + "_session.txt");
    file << userType << "\n";
    file << setBudget << "\n";
    for (string& cat : categories) file << cat << "\n";
    file.close();
}

void loadSession() {
    ifstream file(basePath + sessionName + "_session.txt");
    getline(file, userType);
    file >> setBudget;
    file.ignore();
    string cat;
    categories.clear();
    while (getline(file, cat)) categories.push_back(cat);
    file.close();
}

void saveExpenses() {
    ofstream file(basePath + sessionName + "_expenses.txt");
    for (Expense& e : expenses) {
        file << e.date << "," << e.category << "," << e.amount << "\n";
    }
    file.close();
}

void loadExpenses() {
    ifstream file(basePath + sessionName + "_expenses.txt");
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
    cout << "Enter amount (₹): ";
    cin >> e.amount;
    expenses.push_back(e);
    saveExpenses();
    cout << "Expense added successfully!\n";
}

void showExpenses() {
    if (expenses.empty()) {
        cout << "\nNo expenses to show!\n";
        return;
    }
    cout << "\n--- All Expenses ---\n";
    cout << left << setw(15) << "Date" << setw(20) << "Category" << "Amount (₹)\n";
    cout << "-----------------------------------------------\n";
    for (auto& e : expenses) {
        cout << setw(15) << e.date << setw(20) << e.category << e.amount << endl;
    }
}

void budgetSummary() {
    float totalSpent = 0;
    for (auto& e : expenses) totalSpent += e.amount;
    float remaining = setBudget - totalSpent;

    cout << "\n--- Budget Summary ---\n";
    cout << "Set Budget: ₹" << setBudget << endl;
    cout << "Total Spent: ₹" << totalSpent << endl;
    cout << "Remaining: ₹" << remaining << endl;

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
        cout << pair.first << ": ₹" << pair.second << endl;
    }
}

void createNewSession() {
    cout << "\nEnter a name for this session: ";
    cin >> sessionName;
    cout << "Enter your role (e.g., Student, Employee, Freelancer): ";
    cin >> userType;
    cout << "Enter your monthly budget (₹): ";
    cin >> setBudget;
    cout << "Add your preferred expense categories (type 'done' to finish):\n";
    string cat;
    categories.clear();
    while (true) {
        cout << "Category: ";
        cin >> cat;
        if (cat == "done") break;
        categories.push_back(cat);
    }
    saveSession();
    expenses.clear();
    saveExpenses();
    cout << "Session created and saved!\n";
}

void loadExistingSession() {
    cout << "\nEnter existing session name to load: ";
    cin >> sessionName;
    if (!fileExists(basePath + sessionName + "_session.txt")) {
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

    do {
        cout << "\n===== Session Menu =====\n";
        cout << "1. Create New Session\n";
        cout << "2. Load Existing Session\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> mainChoice;

        switch (mainChoice) {
            case 1: createNewSession(); break;
            case 2: loadExistingSession(); break;
            case 3: cout << "\nExiting program.\n"; return 0;
            default: cout << "Invalid choice. Try again.\n"; continue;
        }

        if (sessionName.empty()) continue;

        do {
            cout << "\n===== Main Menu =====\n";
            cout << "1. Add New Expense\n";
            cout << "2. Show All Expenses\n";
            cout << "3. Show Budget Summary\n";
            cout << "4. Show Category-wise Summary\n";
            cout << "5. Exit to Session Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addExpense(); break;
                case 2: showExpenses(); break;
                case 3: budgetSummary(); break;
                case 4: categorySummary(); break;
                case 5: cout << "Returning to session menu...\n"; break;
                default: cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 5);

    } while (mainChoice != 3);

    return 0;
}
