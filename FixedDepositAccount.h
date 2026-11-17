#ifndef FIXEDDEPOSITACCOUNT_H
#define FIXEDDEPOSITACCOUNT_H

#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: INHERITANCE & SPECIALIZATION
 * FixedDepositAccount inherits publicly from Account.
 * Specific business rules:
 *   - Lump-sum investment locked for a fixed term (e.g. 6, 12, 24, 36 months).
 *   - Quarterly compounding interest rate (6.5% - 7.5% per annum).
 *   - Clear distinction between current ledger principal and projected maturity value.
 *   - Calculated maturity date and premature liquidation rules.
 * ============================================================================
 */
class FixedDepositAccount : public Account
{
private:
    int termMonths;
    double interestRate;
    double maturityAmount;
    std::string maturityDate;
    bool isMatured;

    void computeMaturity();

public:
    FixedDepositAccount(const std::string& n, double principal, int term = 12, 
                        const std::string& date = "", double rate = 0.07);
    FixedDepositAccount(int custId, const std::string& n, double principal, int term = 12, 
                        const std::string& date = "", double rate = 0.07);
    FixedDepositAccount(int accNum, int custId, const std::string& n, double principal, 
                        const std::string& stat, const std::string& date, 
                        int term, double rate, double matAmt, const std::string& matDate);

    // Overriding pure virtual functions
    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
    bool canWithdraw(double amount, std::string& reason) const override;

    // Overriding withdraw to handle fixed-term liquidation
    void withdraw(double amount) override;

    // Display override with FD passbook details
    void display() const override;

    // Specific Getters
    int getTermMonths() const;
    double getMaturityAmount() const;
    std::string getMaturityDate() const;
    bool getIsMatured() const;
};

#endif // FIXEDDEPOSITACCOUNT_H
