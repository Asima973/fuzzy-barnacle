#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
using namespace std;

// Bank account class
class Bank {
public:
    int accno;
    string acchname;
    double balance;
    string password;
    Bank* next;

    Bank(int acc, string name, double bal, string pass) 
        : accno(acc), acchname(name), balance(bal), password(pass), next(nullptr) {}
};

// Bank Management System class
class BankManagementSystem {
private:
    Bank* head;

    // Save all accounts to a file
    void saveToFile() {
        ofstream outFile("accounts.txt");
        if (!outFile) {
            cerr << "Error opening file for saving.\n";
            return;
        }
        Bank* temp = head;
        while (temp) {
            outFile << temp->accno << "," << temp->acchname << "," 
                    << temp->balance << "," << temp->password << "\n";
            temp = temp->next;
        }
        outFile.close();
    }

    // Load accounts from a file
    void loadFromFile() {
        ifstream inFile("accounts.txt");
        if (!inFile) {
            cout << "No existing accounts file found. Starting fresh.\n";
            return;
        }
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            int accno;
            string name, pass;
            double balance;
            char delim;

            ss >> accno >> delim;
            getline(ss, name, ',');
            ss >> balance >> delim;
            getline(ss, pass);

            addAccount(accno, name, balance, pass, false);
        }
        inFile.close();
    }

    // Helper function to find an account
    Bank* findAccount(int accNumber) {
        Bank* temp = head;
        while (temp) {
            if (temp->accno == accNumber)
                return temp;
            temp = temp->next;
        }
        return nullptr;
    }

public:
    // Constructor
    BankManagementSystem() : head(nullptr) {
        loadFromFile();
    }

    // Destructor to clean up memory
    ~BankManagementSystem() {
        Bank* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Add account (Admin only)
    void addAccount(int accNumber, string name, double initialBalance, string password, bool save = true) {
        if (findAccount(accNumber)) {
            cout << "Account with number " << accNumber << " already exists.\n";
            return;
        }
        Bank* newAccount = new Bank(accNumber, name, initialBalance, password);
        if (!head) {
            head = newAccount;
        } else {
            Bank* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newAccount;
        }
        cout << "Account added successfully.\n";
        if (save) saveToFile();
    }

    // Display all accounts (Admin only)
    void displayAccounts() {
        if (!head) {
            cout << "No accounts to display.\n";
            return;
        }
        cout << left << setw(15) << "Acc No" << setw(25) << "Name" << setw(10) << "Balance\n";
        cout << string(50, '-') << "\n";
        Bank* temp = head;
        while (temp) {
            cout << left << setw(15) << temp->accno << setw(25) << temp->acchname << setw(10) << temp->balance << "\n";
            temp = temp->next;
        }
    }

    // Search account (Admin only)
    void searchAccount(int accNumber) {
        Bank* account = findAccount(accNumber);
        if (!account) {
            cout << "Account not found.\n";
            return;
        }
        cout << "Account Found:\n";
        cout << "Account Number: " << account->accno << "\n";
        cout << "Account Holder: " << account->acchname << "\n";
        cout << "Balance: " << account->balance << "\n";
    }

    // Deposit money (Customer only)
    void depositAmount(Bank* account, double amount) {
        account->balance += amount;
        cout << "Deposited " << amount << " successfully. New balance: " << account->balance << "\n";
        saveToFile();
    }

    // Withdraw money (Customer only)
    void withdrawAmount(Bank* account, double amount) {
        if (account->balance < amount) {
            cout << "Insufficient balance.\n";
            return;
        }
        account->balance -= amount;
        cout << "Withdrew " << amount << " successfully. New balance: " << account->balance << "\n";
        saveToFile();
    }

    // Delete account (Admin only)
    void deleteAccount(int accNumber) {
        if (!head) {
            cout << "No accounts available.\n";
            return;
        }
        if (head->accno == accNumber) {
            Bank* temp = head;
            head = head->next;
            delete temp;
            cout << "Account deleted successfully.\n";
            saveToFile();
            return;
        }
        Bank* temp = head;
        while (temp->next && temp->next->accno != accNumber) {
            temp = temp->next;
        }
        if (temp->next) {
            Bank* toDelete = temp->next;
            temp->next = temp->next->next;
            delete toDelete;
            cout << "Account deleted successfully.\n";
            saveToFile();
        } else {
            cout << "Account not found.\n";
        }
    }

    // Customer login and operations
    void customerMenu(int accNumber, string password) {
        Bank* account = findAccount(accNumber);
        if (!account || account->password != password) {
            cout << "Invalid account number or password.\n";
            return;
        }

        int choice;
        do {
            cout << "\nCustomer Menu\n";
            cout << "1. View Account Details\n";
            cout << "2. Deposit Money\n";
            cout << "3. Withdraw Money\n";
            cout << "4. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            double amount;
            switch (choice) {
            case 1:
                cout << "Account Number: " << account->accno << "\n";
                cout << "Account Holder: " << account->acchname << "\n";
                cout << "Balance: " << account->balance << "\n";
                break;
            case 2:
                cout << "Enter amount to deposit: ";
                cin >> amount;
                depositAmount(account, amount);
                break;
            case 3:
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                withdrawAmount(account, amount);
                break;
            case 4:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 4);
    }
};

// Admin menu
void adminMenu(BankManagementSystem& bms) {
    int choice;
    do {
        cout << "\nAdmin Menu\n";
        cout << "1. Add Account\n";
        cout << "2. Display All Accounts\n";
        cout << "3. Search Account\n";
        cout << "4. Delete Account\n";
        cout << "5. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        int accNumber;
        string name, password;
        double amount;

        switch (choice) {
        case 1:
            cout << "Enter account number: ";
            cin >> accNumber;
            cout << "Enter account holder name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter initial deposit amount: ";
            cin >> amount;
            cout << "Set a password for the account: ";
            cin >> password;
            bms.addAccount(accNumber, name, amount, password);
            break;
        case 2:
            bms.displayAccounts();
            break;
        case 3:
            cout << "Enter account number to search: ";
            cin >> accNumber;
            bms.searchAccount(accNumber);
            break;
        case 4:
            cout << "Enter account number to delete: ";
            cin >> accNumber;
            bms.deleteAccount(accNumber);
            break;
        case 5:
            cout << "Logging out...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 5);
}

// Main function
int main() {
    BankManagementSystem bms;

    int role;
    do {
        cout << "\nWelcome to the Bank Management System\n";
        cout << "1. Admin Login\n";
        cout << "2. Customer Login\n";
        cout << "3. Exit\n";
        cout << "Enter your role: ";
        cin >> role;

        int accNumber;
        string password;

        switch (role) {
        case 1:
            adminMenu(bms);
            break;
        case 2:
            cout << "Enter your account number: ";
            cin >> accNumber;
            cout << "Enter your password: ";
            cin >> password;
            bms.customerMenu(accNumber, password);
            break;
        case 3:
            cout << "Exiting the system. Goodbye!\n";
            break;
        default:
            cout << "Invalid role. Please try again.\n";
        }
    } while (role != 3);

    return 0;
}
