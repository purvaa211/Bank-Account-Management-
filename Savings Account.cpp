#include "SavingsAccount.h"
#include <iostream>

using namespace std;

SavingsAccount::SavingsAccount(string n, double b)
    : Account(n, b)
{
}

void SavingsAccount::calculateInterest()
{
    double interest = getBalance() * 0.04;

    cout << "Savings Account Interest: " << interest << endl;
}