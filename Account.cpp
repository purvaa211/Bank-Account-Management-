#include "Account.h"
#include "Exceptions.h"
#include <iostream>

using namespace std;

int Account::nextAccountNumber = 1001;

Account::Account(string n, double b)
{
    this->name = n;
    this->balance = b;
    this->accountNumber = nextAccountNumber;

    nextAccountNumber++;
}

Account::~Account()
{
}

void Account::deposit(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException();
    }

    balance = balance + amount;

    cout << "Amount deposited successfully." << endl;
}

void Account::withdraw(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException();
    }

    if (amount > balance)
    {
        throw InsufficientBalanceException();
    }

    balance = balance - amount;

    cout << "Amount withdrawn successfully." << endl;
}

void Account::display()
{
    cout << "\nAccount Number: " << accountNumber << endl;
    cout << "Account Holder: " << name << endl;
    cout << "Balance: " << balance << endl;
}

double Account::getBalance()
{
    return balance;
}

int Account::getAccountNumber()
{
    return accountNumber;
}