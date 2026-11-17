#ifndef BANK_H
#define BANK_H

#include <vector>
#include <string>
#include "Account.h"
#include "Customer.h"
#include "Transaction.h"
#include "FraudDetector.h"
#include "BankDatabase.h"

/**
 * ============================================================================
 * OOP CONCEPT: STL CONTAINERS, COMPOSITION, FUNCTION OVERLOADING & AGGREGATION
 * Bank is the central management engine of SmartBank.
 * Demonstrates:
 *   - Aggregation: Manages vector of polymorphic Account* objects and Customer entities.
 *   - Composition: Composes FraudDetector, BankDatabase, and Transaction audit logs.
 *   - Function Overloading: findAccount(int) vs findAccount(std::string),
 *                           findCustomer(int) vs findCustomer(std::string).
 *   - Destructor memory management: safely cleans heap-allocated Account pointers.
 * ============================================================================
 */
class Bank
{
private:
    std::vector<Customer> customers;
    std::vector<Account*> accounts;
    std::vector<Transaction> transactions;
    FraudDetector fraudDetector;
    BankDatabase database;

public:
    Bank();
    ~Bank();

    // Customer Management
    void addCustomer(const Customer& customer);
    Customer* findCustomer(int customerId);
    Customer* findCustomer(const std::string& customerName);
    void updateCustomerContact(int customerId, const std::string& email, const std::string& phone);
    std::vector<Account*> getAccountsByCustomerId(int customerId) const;
    void displayAllCustomers() const;
    void displayCustomerDetails(int customerId) const;

    // Account Management
    void addAccount(Account* account);
    void closeAccount(int accountNumber);

    // Function Overloading: finding account by ID or by Holder Name
    Account* findAccount(int accountNumber);
    Account* findAccount(const std::string& holderName);
    void displayAccountsByCustomer(int customerId) const;

    // Financial Transactions (Atomic execution)
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
    size_t getCustomerCount() const;
    double getTotalBankBalance() const;
    size_t getTotalTransactionsCount() const;
    double getTotalWithdrawals() const;

    // Getters for collections
    const std::vector<Customer>& getCustomers() const;
    const std::vector<Account*>& getAccounts() const;
    const std::vector<Transaction>& getTransactions() const;
};

#endif // BANK_H
