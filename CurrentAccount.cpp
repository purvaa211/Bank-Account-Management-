#include "CurrentAccount.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>

CurrentAccount::CurrentAccount(const std::string& n, double b, const std::string& date, double overdraft)
    : Account(n, b, date), overdraftLimit(overdraft)
{
}

CurrentAccount::CurrentAccount(int accNum, const std::string& n, double b, const std::string& stat, const std::string& date, double overdraft)
    : Account(accNum, n, b, stat, date), overdraftLimit(overdraft)
{
}

bool CurrentAccount::canWithdraw(double amount, std::string& reason) const
{
    if (amount <= 0)
    {
        reason = "Withdrawal amount must be greater than zero.";
        return false;
    }

    // Maximum withdrawal allowable is current balance + overdraft limit
    double maxWithdrawal = getBalance() + overdraftLimit;
    if (amount > maxWithdrawal)
    {
        reason = "Amount exceeds allowable overdraft limit of " + 
                 UI::formatCurrency(overdraftLimit) + ". Maximum possible withdrawal: " + 
                 UI::formatCurrency(maxWithdrawal);
        return false;
    }

    return true;
}

void CurrentAccount::withdraw(double amount)
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
        double maxWithdrawal = getBalance() + overdraftLimit;
        throw OverdraftExceededException(maxWithdrawal, amount);
    }

    setBalance(getBalance() - amount);

    if (getBalance() < 0)
    {
        UI::printWarning("Account #" + std::to_string(getAccountNumber()) + 
                          " has entered overdraft. Utilized: " + 
                          UI::formatCurrency(-getBalance()) + " of limit " + 
                          UI::formatCurrency(overdraftLimit));
    }
}

void CurrentAccount::calculateInterest()
{
    const double interestRate = 0.0;
    double interest = 0.0;
    double projectedBalance = getBalance();

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
    UI::printInfo("Current accounts have an approved overdraft facility limit of " + 
                  UI::formatCurrency(overdraftLimit));
}

std::string CurrentAccount::getAccountType() const
{
    return "Current Account";
}

double CurrentAccount::getInterestRate() const
{
    return 0.0;
}

double CurrentAccount::getOverdraftLimit() const
{
    return overdraftLimit;
}

void CurrentAccount::setOverdraftLimit(double limit)
{
    overdraftLimit = limit;
}
