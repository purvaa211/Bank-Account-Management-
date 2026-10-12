#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

/**
 * ============================================================================
 * OOP CONCEPT: ABSTRACT BASE CLASS & ENCAPSULATION
 * - Encapsulation: private data members accessed via public member functions.
 * - Abstract Class: Contains pure virtual functions (= 0), cannot be instantiated.
 * - Virtual Destructor: Ensures proper cleanup of derived objects via base pointers.
 * - Static Data Member: Shared account counter across all Account instances.
 * ============================================================================
 */
class Account
{
private:
    int accountNumber;
    std::string name;
    double balance;

    // Static member: shared across all objects to generate auto-incrementing account numbers
    static int nextAccountNumber;

public:
    // Parameterized constructor
    Account(const std::string& n, double b);

    // Virtual destructor - critical for runtime polymorphism and proper memory cleanup
    virtual ~Account();

    // Member functions for account transactions
    void deposit(double amount);
    void withdraw(double amount);

    // Pure virtual functions (must be implemented by derived classes)
    virtual void calculateInterest() = 0;
    virtual std::string getAccountType() const = 0;
    virtual double getInterestRate() const = 0;

    // Display account information (Passbook card view)
    virtual void display() const;

    // Public Getters (Encapsulation)
    int getAccountNumber() const;
    std::string getName() const;
    double getBalance() const;

    // Static member function to inspect next account number counter
    static int getNextAccountNumber();
};

#endif // ACCOUNT_H