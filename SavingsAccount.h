#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: INHERITANCE & METHOD OVERRIDING (POLYMORPHISM)
 * SavingsAccount inherits publicly from Account.
 * Specific business rules:
 *   - Accrues 4.0% annual interest.
 *   - Enforces a mandatory minimum balance requirement (Rs. 1,000.00).
 * ============================================================================
 */
class SavingsAccount : public Account
{
private:
    double minimumBalance;
    double interestRate;

public:
    SavingsAccount(const std::string& n, double b, const std::string& date = "", double minBal = 1000.0);
    SavingsAccount(int custId, const std::string& n, double b, const std::string& date = "", double minBal = 1000.0);
    SavingsAccount(int accNum, int custId, const std::string& n, double b, const std::string& stat, const std::string& date, double minBal = 1000.0);

    // Overriding pure virtual functions from Account base class
    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
    bool canWithdraw(double amount, std::string& reason) const override;

    // Overriding withdraw to enforce minimum balance rule
    void withdraw(double amount) override;

    double getMinimumBalance() const;
};

#endif // SAVINGSACCOUNT_H
