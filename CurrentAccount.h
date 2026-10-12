#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H

#include "Account.h"

class CurrentAccount : public Account
{
public:
    CurrentAccount(string n, double b);

    void calculateInterest() override;
};

#endif