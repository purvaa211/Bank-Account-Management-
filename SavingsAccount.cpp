#include "SavingsAccount.h"
#include "UIHelper.h"
#include <iostream>

SavingsAccount::SavingsAccount(const std::string& n, double b)
    : Account(n, b)
{
    // Constructor delegates initialization to base class Account constructor
}

void SavingsAccount::calculateInterest()
{
    // Savings accounts earn 4% annual interest
    const double interestRate = 0.04;
    double interest = getBalance() * interestRate;
    double projectedBalance = getBalance() + interest;

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
}

std::string SavingsAccount::getAccountType() const
{
    return "Savings Account";
}

double SavingsAccount::getInterestRate() const
{
    return 0.04; // 4%
}