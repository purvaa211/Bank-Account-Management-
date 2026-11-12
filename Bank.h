#ifndef BANK_H
#define BANK_H

#include <vector>
#include <string>
#include "Account.h"
#include "Transaction.h"
#include "FraudDetector.h"
#include "BankDatabase.h"

/**
 * ============================================================================
 * OOP CONCEPT: STL CONTAINERS, COMPOSITION, FUNCTION OVERLOADING & AGGREGATION
 * Bank is the central management engine of SmartBank.
 * Demonstrates:
 *   - Aggregation: Manages vector of polymorphic Account* objects.
 *   - Composition: Composes FraudDetector, BankDatabase, and Transaction logs.
 *   - Function Overloading: findAccount(int) vs findAccount(std::string).
 *   - Destructor memory management: safely cleans heap allocations.
 * ============================================================================
 */
class Bank
{
private:
    std::vector<Account*> accounts;
    std::vector<Transaction> transactions;
    FraudDetector fraudDetector;
    BankDatabase database;

public:
    Bank();
    ~Bank();

    // Account Management
    void addAccount(Account* account);
    void closeAccount(int accountNumber);

    // Function Overloading: finding account by ID or by Holder Name
    Account* findAccount(int accountNumber);
    Account* findAccount(const std::string& holderName);

    // Financial Transactions
    Transaction deposit(int accountNumber, double amount, const std::string& note = "");
    Transaction withdraw(int accountNumber, double amount, const std::string& note = "");
    Transaction transfer(int senderAccount, int receiverAccount, double amount, const std::string& note = "");

    // Security Monitoring
    FraudDetector& getFraudDetector();
    const FraudDetector& getFraudDetector() const;
    void reviewSecurityAlert(int alertId, const std::string& newStatus);
    void displaySecurityAlerts() const;

    // Reporting & Auditing
    void displayAllAccounts() const;
    void displayTransactionHistory(int accountNumber = 0) const;
    std::string exportAccountStatement(int accountNumber) const;
    void printAccountStatement(int accountNumber) const;

    // Persistence
    void saveData() const;
    void loadData();

    // Metrics & Analytics
    size_t getAccountCount() const;
    double getTotalBankBalance() const;
    size_t getTotalTransactionsCount() const;
    double getTotalWithdrawals() const;
    const std::vector<Account*>& getAccounts() const;
    const std::vector<Transaction>& getTransactions() const;
};

#endif // BANK_H
