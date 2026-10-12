#include "CurrentAccount.h"
#include "UIHelper.h"
#include <iostream>

CurrentAccount::CurrentAccount(const std::string& n, double b)
    : Account(n, b)
{
    // Constructor delegates initialization to base class Account constructor
}

void CurrentAccount::calculateInterest()
{
    // Current accounts earn 0% interest
    const double interestRate = 0.0;
    double interest = 0.0;
    double projectedBalance = getBalance();

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
    UI::printInfo("Note: Current Accounts are transactional accounts and accrue 0% annual interest.");
}

std::string CurrentAccount::getAccountType() const
{
    return "Current Account";
}

double CurrentAccount::getInterestRate() const
{
    return 0.0; // 0%
}