#include "Bank.h"
#include "Exceptions.h"
#include <iostream>
#include <fstream>

using namespace std;

Bank::~Bank()
{
    for (Account* account : accounts)
    {
        delete account;
    }
}

void Bank::addAccount(Account* account)
{
    accounts.push_back(account);
}

Account* Bank::findAccount(int accountNumber)
{
    for (Account* account : accounts)
    {
        if (account->getAccountNumber() == accountNumber)
        {
            return account;
        }
    }

    throw AccountNotFoundException();
}

void Bank::displayAllAccounts()
{
    if (accounts.empty())
    {
        cout << "No accounts available." << endl;
        return;
    }

    for (Account* account : accounts)
    {
        account->display();
    }
}

void Bank::saveAccounts()
{
    ofstream file("accounts.txt");

    if (!file)
    {
        cout << "Unable to open file." << endl;
        return;
    }

    for (Account* account : accounts)
    {
        file << "Account Number: " << account->getAccountNumber() << endl;
        file << "Balance: " << account->getBalance() << endl;
        file << "------------------------" << endl;
    }

    file.close();

    cout << "Account information saved successfully." << endl;
}