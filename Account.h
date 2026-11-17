#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <iostream>

/**
 * ============================================================================
 * OOP CONCEPT: ABSTRACT BASE CLASS, ENCAPSULATION & OPERATOR OVERLOADING
 * - Encapsulation: private data members accessed via public member functions.
 * - Abstraction: Pure virtual functions (= 0), cannot be directly instantiated.
 * - Runtime Polymorphism: Virtual functions and virtual destructor.
 * - Operator Overloading: Comparison operators (<, >, ==) and stream insertion (<<).
 * - Friend Class: BankStatementGenerator has privileged access for statements.
 * - Static Data Member: Shared auto-incrementing account counter.
 * ============================================================================
 */
class Account
{
private:
    int accountNumber;
    int customerId;
    std::string name;
    double balance;
    std::string status; // "ACTIVE" or "CLOSED"
    std::string openingDate;

    // Static member: shared across all objects
    static int nextAccountNumber;

protected:
    void setBalance(double b);

public:
    // Constructors (Constructor Overloading)
    Account(const std::string& n, double b, const std::string& date = "");
    Account(int custId, const std::string& n, double b, const std::string& date = "");
    Account(int accNum, int custId, const std::string& n, double b, const std::string& stat, const std::string& date);

    // Virtual destructor - critical for runtime polymorphism and safe memory cleanup
    virtual ~Account();

    // Transactions (balance is modified only through validated member functions)
    virtual void deposit(double amount);
    virtual void withdraw(double amount);

    // Pure virtual functions (must be implemented by derived classes)
    virtual void calculateInterest() = 0;
    virtual std::string getAccountType() const = 0;
    virtual double getInterestRate() const = 0;
    virtual bool canWithdraw(double amount, std::string& reason) const = 0;

    // Display account information (Passbook card view)
    virtual void display() const;

    // Public Getters (Encapsulation)
    int getAccountNumber() const;
    int getCustomerId() const;
    std::string getName() const;
    double getBalance() const;
    std::string getStatus() const;
    std::string getOpeningDate() const;
    bool isActive() const;

    // Setters
    void setCustomerId(int id);
    void setName(const std::string& n);

    // Account status management
    void closeAccount();

    // Operator Overloading
    bool operator>(const Account& other) const;
    bool operator<(const Account& other) const;
    bool operator==(const Account& other) const;

    // Friend stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const Account& acc);

    // Friend class declaration
    friend class BankStatementGenerator;

    // Static member functions
    static int getNextAccountNumber();
    static void setNextAccountNumber(int nextId);
};

#endif // ACCOUNT_H
