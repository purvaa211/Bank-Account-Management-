#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

/**
 * ============================================================================
 * OOP CONCEPT: EXCEPTION HANDLING & INHERITANCE IN EXCEPTIONS
 * Base custom exception for banking domain inheriting from std::exception.
 * Demonstrates:
 *   - Inheritance (BankingException inherits from std::exception)
 *   - Method Overriding (overrides what() const noexcept)
 *   - Polymorphic exception handling
 * ============================================================================
 */
class BankingException : public std::exception
{
protected:
    std::string message;

public:
    explicit BankingException(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept override
    {
        return message.c_str();
    }
};

/**
 * Thrown when an invalid transaction amount is entered (e.g., <= 0).
 */
class InvalidAmountException : public BankingException
{
public:
    InvalidAmountException() 
        : BankingException("Invalid amount! Amount must be strictly greater than zero.") {}
    
    explicit InvalidAmountException(const std::string& customMsg) 
        : BankingException(customMsg) {}
};

/**
 * Thrown when a withdrawal amount exceeds the available account balance.
 */
class InsufficientBalanceException : public BankingException
{
public:
    InsufficientBalanceException() 
        : BankingException("Insufficient balance! Withdrawal amount exceeds available balance.") {}

    explicit InsufficientBalanceException(double currentBalance, double requestedAmount)
        : BankingException("Insufficient balance! Available balance is $" + 
                           std::to_string(currentBalance) + ", but requested withdrawal is $" + 
                           std::to_string(requestedAmount) + ".") {}
};

/**
 * Thrown when searching for an account number that does not exist in the bank.
 */
class AccountNotFoundException : public BankingException
{
public:
    AccountNotFoundException() 
        : BankingException("Account not found! The specified account number does not exist.") {}

    explicit AccountNotFoundException(int accNum) 
        : BankingException("Account not found! Account #" + std::to_string(accNum) + " does not exist in records.") {}
};

#endif // EXCEPTIONS_H