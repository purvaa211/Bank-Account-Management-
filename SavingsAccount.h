#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "Account.h"

class SavingsAccount : public Account
{
public:
    SavingsAccount(string n, double b);

    void calculateInterest() override;
};

#endif