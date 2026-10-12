#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: INHERITANCE & METHOD OVERRIDING (POLYMORPHISM)
 * SavingsAccount inherits publicly from Account.
 * Implements calculateInterest() providing 4% annual interest rate.
 * ============================================================================
 */
class SavingsAccount : public Account
{
public:
    SavingsAccount(const std::string& n, double b);

    // Overriding pure virtual functions from Account base class
    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
};

#endif // SAVINGSACCOUNT_H