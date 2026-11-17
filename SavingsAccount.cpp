#include "SavingsAccount.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>

SavingsAccount::SavingsAccount(const std::string& n, double b, const std::string& date, double minBal)
    : Account(201, n, b, date), minimumBalance(minBal), interestRate(0.04)
{
}

SavingsAccount::SavingsAccount(int custId, const std::string& n, double b, const std::string& date, double minBal)
    : Account(custId, n, b, date), minimumBalance(minBal), interestRate(0.04)
{
}

SavingsAccount::SavingsAccount(int accNum, int custId, const std::string& n, double b, const std::string& stat, const std::string& date, double minBal)
    : Account(accNum, custId, n, b, stat, date), minimumBalance(minBal), interestRate(0.04)
{
}

bool SavingsAccount::canWithdraw(double amount, std::string& reason) const
{
    if (amount <= 0)
    {
        reason = "Withdrawal amount must be greater than zero.";
        return false;
    }

    if ((getBalance() - amount) < minimumBalance)
    {
        reason = "Withdrawal violates minimum balance requirement of " + 
                 UI::formatCurrency(minimumBalance) + ". Current balance: " + 
                 UI::formatCurrency(getBalance());
        return false;
    }

    return true;
}

void SavingsAccount::withdraw(double amount)
{
    if (!isActive())
    {
        throw AccountClosedException(getAccountNumber());
    }

    if (amount <= 0)
    {
        throw InvalidAmountException("Withdrawal amount must be greater than zero.");
    }

    std::string reason;
    if (!canWithdraw(amount, reason))
    {
        throw MinimumBalanceException(minimumBalance, getBalance() - amount);
    }

    setBalance(getBalance() - amount);
}

void SavingsAccount::calculateInterest()
{
    double interest = getBalance() * interestRate;
    double projectedBalance = getBalance() + interest;

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
    UI::printInfo("Savings account maintains minimum balance threshold of " + 
                  UI::formatCurrency(minimumBalance));
}

std::string SavingsAccount::getAccountType() const
{
    return "Savings Account";
}

double SavingsAccount::getInterestRate() const
{
    return interestRate;
}

double SavingsAccount::getMinimumBalance() const
{
    return minimumBalance;
}
