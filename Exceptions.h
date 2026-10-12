#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
using namespace std;

class InvalidAmountException : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Invalid amount.";
    }
};

class InsufficientBalanceException : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Insufficient balance.";
    }
};

class AccountNotFoundException : public exception
{
public:
    const char* what() const noexcept override
    {
        return "Account not found.";
    }
};

#endif