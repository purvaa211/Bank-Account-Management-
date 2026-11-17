#ifndef BANKDATABASE_H
#define BANKDATABASE_H

#include <string>
#include <vector>
#include "Account.h"
#include "Customer.h"
#include "Transaction.h"
#include "FraudDetector.h"

/**
 * ============================================================================
 * OOP CONCEPT: SEPARATION OF CONCERNS & DATA PERSISTENCE
 * BankDatabase handles persistent disk I/O operations for the banking system:
 *   - data/customers.csv
 *   - data/accounts.csv
 *   - data/transactions.csv
 *   - data/security_alerts.csv
 * Ensures data survives application restarts with ID preservation.
 * ============================================================================
 */
class BankDatabase
{
private:
    std::string dataDirectory;

    void ensureDirectoryExists() const;

public:
    explicit BankDatabase(const std::string& dir = "data");

    // Customer Persistence
    bool saveCustomers(const std::vector<Customer>& customers) const;
    std::vector<Customer> loadCustomers() const;

    // Account Persistence
    bool saveAccounts(const std::vector<Account*>& accounts) const;
    std::vector<Account*> loadAccounts() const;

    // Transaction Persistence
    bool saveTransactions(const std::vector<Transaction>& transactions) const;
    std::vector<Transaction> loadTransactions() const;

    // Security Alerts Persistence
    bool saveAlerts(const std::vector<SecurityAlert>& alerts) const;
    std::vector<SecurityAlert> loadAlerts() const;
};

#endif // BANKDATABASE_H
