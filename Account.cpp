#include "Account.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>

// Definition and initialization of static member variable
int Account::nextAccountNumber = 1001;

Account::Account(const std::string& n, double b)
{
    // 'this' pointer explicitly differentiates member variables from constructor parameters
    this->name = n;
    this->balance = b;
    this->accountNumber = nextAccountNumber;

    // Increment shared static counter for the next account to be created
    nextAccountNumber++;
}

Account::~Account()
{
    // Virtual destructor body (base class cleanup)
}

void Account::deposit(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException("Deposit amount must be greater than zero.");
    }

    balance = balance + amount;
}

void Account::withdraw(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException("Withdrawal amount must be greater than zero.");
    }

    if (amount > balance)
    {
        throw InsufficientBalanceException(balance, amount);
    }

    balance = balance - amount;
}

void Account::display() const
{
    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                     ACCOUNT PASSBOOK CARD                   |\n";
    std::cout << "+------------------------------+------------------------------+\n" << UI::Color::RESET;

    auto printField = [](const std::string& label, const std::string& val, const std::string& color = UI::Color::BOLD_WHITE) {
        std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(28) << label 
                  << UI::Color::CYAN << "| " << color << std::left << std::setw(28) << val 
                  << UI::Color::CYAN << "|\n" << UI::Color::RESET;
    };

    printField("Account Number", std::to_string(accountNumber));
    printField("Account Holder", name);
    printField("Account Type", getAccountType());
    printField("Current Balance", UI::formatCurrency(balance), UI::Color::BOLD_GREEN);

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+------------------------------+------------------------------+\n";
    std::cout << UI::Color::RESET;
}

int Account::getAccountNumber() const
{
    return accountNumber;
}

std::string Account::getName() const
{
    return name;
}

double Account::getBalance() const
{
    return balance;
}

int Account::getNextAccountNumber()
{
    return nextAccountNumber;
}