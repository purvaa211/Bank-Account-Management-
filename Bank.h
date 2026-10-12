#ifndef BANK_H
#define BANK_H

#include <vector>
#include "Account.h"

/**
 * ============================================================================
 * OOP CONCEPT: STL CONTAINER (VECTOR OF BASE CLASS POINTERS) & AGGREGATION
 * Bank manages a collection of dynamically allocated Account objects.
 * Demonstrates:
 *   - STL std::vector<Account*>
 *   - Destructor memory management (deallocating heap-allocated objects)
 *   - Runtime Polymorphism when iterating over accounts
 *   - File Handling using std::ofstream
 * ============================================================================
 */
class Bank
{
private:
    std::vector<Account*> accounts;

public:
    // Destructor: properly frees dynamically allocated Account objects
    ~Bank();

    // Account management functions
    void addAccount(Account* account);
    Account* findAccount(int accountNumber);
    void displayAllAccounts() const;

    // File handling: saves all account records to accounts.txt
    void saveAccounts() const;

    // Additional helper statistics
    size_t getAccountCount() const;
    double getTotalBankBalance() const;
};

#endif // BANK_H