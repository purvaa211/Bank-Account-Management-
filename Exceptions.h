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
 *   - Polymorphic exception handling across specific domain errors
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
 * Thrown when an invalid transaction amount is entered (e.g. <= 0).
 */
class InvalidAmountException : public BankingException
{
public:
    InvalidAmountException() 
        : BankingException("Invalid amount! Transaction amount must be strictly greater than zero.") {}
    
    explicit InvalidAmountException(const std::string& customMsg) 
        : BankingException(customMsg) {}
};

/**
 * Thrown when a withdrawal amount exceeds available funds.
 */
class InsufficientBalanceException : public BankingException
{
public:
    InsufficientBalanceException() 
        : BankingException("Insufficient balance! Withdrawal amount exceeds available balance.") {}

    explicit InsufficientBalanceException(double currentBalance, double requestedAmount)
        : BankingException("Insufficient balance! Available balance is Rs. " + 
                           std::to_string(currentBalance) + ", but requested withdrawal is Rs. " + 
                           std::to_string(requestedAmount) + ".") {}
};

/**
 * Thrown when searching for an account number that does not exist.
 */
class AccountNotFoundException : public BankingException
{
public:
    AccountNotFoundException() 
        : BankingException("Account not found! The specified account does not exist in banking records.") {}

    explicit AccountNotFoundException(int accNum) 
        : BankingException("Account not found! Account #" + std::to_string(accNum) + " does not exist.") {}

    explicit AccountNotFoundException(const std::string& name)
        : BankingException("Account not found! No account registered under holder name: " + name) {}
};

/**
 * Thrown when a withdrawal would breach the required minimum balance for Savings accounts.
 */
class MinimumBalanceException : public BankingException
{
public:
    explicit MinimumBalanceException(double minBal, double projectedBal)
        : BankingException("Transaction rejected! Savings account requires a minimum balance of Rs. " + 
                           std::to_string(minBal) + ". Projected balance would be Rs. " + 
                           std::to_string(projectedBal) + ".") {}
};

/**
 * Thrown when an overdraft withdrawal exceeds the approved limit for Current accounts.
 */
class OverdraftExceededException : public BankingException
{
public:
    explicit OverdraftExceededException(double maxAllowed, double requested)
        : BankingException("Transaction rejected! Overdraft limit exceeded. Maximum allowable withdrawal is Rs. " +
                           std::to_string(maxAllowed) + ", but requested Rs. " + std::to_string(requested) + ".") {}
};

/**
 * Thrown when attempting an operation on a closed account.
 */
class AccountClosedException : public BankingException
{
public:
    explicit AccountClosedException(int accNum)
        : BankingException("Operation rejected! Account #" + std::to_string(accNum) + " has been closed.") {}
};

/**
 * Thrown when a transaction breaches transaction security rules.
 */
class SecurityViolationException : public BankingException
{
public:
    explicit SecurityViolationException(const std::string& reason)
        : BankingException("Security Alert: " + reason) {}
};

#endif // EXCEPTIONS_H
