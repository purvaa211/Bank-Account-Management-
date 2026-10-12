#include "CurrentAccount.h"
#include <iostream>

using namespace std;

CurrentAccount::CurrentAccount(string n, double b)
    : Account(n, b)
{
}

void CurrentAccount::calculateInterest()
{
    cout << "Current Account Interest: 0" << endl;
}