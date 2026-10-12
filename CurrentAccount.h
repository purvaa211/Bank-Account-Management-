#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H

#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: INHERITANCE & METHOD OVERRIDING (POLYMORPHISM)
 * CurrentAccount inherits publicly from Account.
 * Current accounts offer zero interest (commercial/transactional accounts).
 * Demonstrates overriding of calculateInterest() with specific business logic.
 * ============================================================================
 */
class CurrentAccount : public Account
{
public:
    CurrentAccount(const std::string& n, double b);

    // Overriding pure virtual functions from Account base class
    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
};

#endif // CURRENTACCOUNT_H