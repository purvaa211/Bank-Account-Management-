#include <iostream>
#include <string>

#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "Bank.h"
#include "Exceptions.h"

using namespace std;

int main()
{
    Bank bank;

    int choice;

    do
    {
        cout << "\n===== BANK ACCOUNT MANAGEMENT SYSTEM =====" << endl;
        cout << "1. Create Savings Account" << endl;
        cout << "2. Create Current Account" << endl;
        cout << "3. Deposit" << endl;
        cout << "4. Withdraw" << endl;
        cout << "5. Check Balance" << endl;
        cout << "6. Display Account" << endl;
        cout << "7. Display All Accounts" << endl;
        cout << "8. Calculate Interest" << endl;
        cout << "9. Save Accounts" << endl;
        cout << "10. Exit" << endl;

        cout << "\nEnter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            string name;
            double balance;

            cout << "Enter account holder name: ";
            cin >> name;

            cout << "Enter initial balance: ";
            cin >> balance;

            try
            {
                if (balance < 0)
                {
                    throw InvalidAmountException();
                }

                Account* account = new SavingsAccount(name, balance);

                bank.addAccount(account);

                cout << "Savings account created successfully." << endl;
                cout << "Account Number: "
                     << account->getAccountNumber() << endl;
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 2)
        {
            string name;
            double balance;

            cout << "Enter account holder name: ";
            cin >> name;

            cout << "Enter initial balance: ";
            cin >> balance;

            try
            {
                if (balance < 0)
                {
                    throw InvalidAmountException();
                }

                Account* account = new CurrentAccount(name, balance);

                bank.addAccount(account);

                cout << "Current account created successfully." << endl;
                cout << "Account Number: "
                     << account->getAccountNumber() << endl;
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 3)
        {
            int accountNumber;
            double amount;

            cout << "Enter account number: ";
            cin >> accountNumber;

            cout << "Enter amount to deposit: ";
            cin >> amount;

            try
            {
                Account* account = bank.findAccount(accountNumber);

                account->deposit(amount);
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 4)
        {
            int accountNumber;
            double amount;

            cout << "Enter account number: ";
            cin >> accountNumber;

            cout << "Enter amount to withdraw: ";
            cin >> amount;

            try
            {
                Account* account = bank.findAccount(accountNumber);

                account->withdraw(amount);
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 5)
        {
            int accountNumber;

            cout << "Enter account number: ";
            cin >> accountNumber;

            try
            {
                Account* account = bank.findAccount(accountNumber);

                cout << "Current Balance: "
                     << account->getBalance() << endl;
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 6)
        {
            int accountNumber;

            cout << "Enter account number: ";
            cin >> accountNumber;

            try
            {
                Account* account = bank.findAccount(accountNumber);

                account->display();
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 7)
        {
            cout << "\nAll Accounts:" << endl;

            bank.displayAllAccounts();
        }

        else if (choice == 8)
        {
            int accountNumber;

            cout << "Enter account number: ";
            cin >> accountNumber;

            try
            {
                Account* account = bank.findAccount(accountNumber);

                account->calculateInterest();
            }
            catch (exception& e)
            {
                cout << e.what() << endl;
            }
        }

        else if (choice == 9)
        {
            bank.saveAccounts();
        }

        else if (choice == 10)
        {
            cout << "Thank you for using Bank Account Management System." << endl;
        }

        else
        {
            cout << "Invalid choice." << endl;
        }

    } while (choice != 10);

    return 0;
}