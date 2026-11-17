#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H

#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: INHERITANCE & METHOD OVERRIDING (POLYMORPHISM)
 * CurrentAccount inherits publicly from Account.
 * Specific business rules:
 *   - Commercial transactional account with 0.0% interest.
 *   - Configurable Overdraft Limit allowing negative balance protection.
 * ============================================================================
 */
class CurrentAccount : public Account
{
private:
    double overdraftLimit;

public:
    CurrentAccount(const std::string& n, double b, const std::string& date = "", double overdraft = 25000.0);
    CurrentAccount(int custId, const std::string& n, double b, const std::string& date = "", double overdraft = 25000.0);
    CurrentAccount(int accNum, int custId, const std::string& n, double b, const std::string& stat, const std::string& date, double overdraft = 25000.0);

    // Overriding pure virtual functions from Account base class
    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
    bool canWithdraw(double amount, std::string& reason) const override;

    // Overriding withdraw to handle overdraft facility
    void withdraw(double amount) override;

    double getOverdraftLimit() const;
    void setOverdraftLimit(double limit);
};

#endif // CURRENTACCOUNT_H
