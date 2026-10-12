#ifndef BANK_H
#define BANK_H

#include <vector>
#include "Account.h"

using namespace std;

class Bank
{
private:
    vector<Account*> accounts;

public:
    ~Bank();

    void addAccount(Account* account);
    Account* findAccount(int accountNumber);
    void displayAllAccounts();

    void saveAccounts();
};

#endif