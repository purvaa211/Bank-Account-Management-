#include "Account.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

// Definition and initialization of static member variable
int Account::nextAccountNumber = 1001;

static std::string getCurrentDateString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* parts = std::localtime(&now_c);
    char buf[32];
    if (parts)
    {
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", parts);
    }
    else
    {
        return "2026-11-12";
    }
    return std::string(buf);
}

Account::Account(const std::string& n, double b, const std::string& date)
    : accountNumber(nextAccountNumber++), customerId(201), name(n), balance(b), status("ACTIVE")
{
    this->openingDate = date.empty() ? getCurrentDateString() : date;
}

Account::Account(int custId, const std::string& n, double b, const std::string& date)
    : accountNumber(nextAccountNumber++), customerId(custId), name(n), balance(b), status("ACTIVE")
{
    this->openingDate = date.empty() ? getCurrentDateString() : date;
}

Account::Account(int accNum, int custId, const std::string& n, double b, const std::string& stat, const std::string& date)
    : accountNumber(accNum), customerId(custId), name(n), balance(b), status(stat), openingDate(date)
{
}

Account::~Account()
{
}

void Account::setBalance(double b)
{
    balance = b;
}

void Account::deposit(double amount)
{
    if (status == "CLOSED")
    {
        throw AccountClosedException(accountNumber);
    }

    if (amount <= 0)
    {
        throw InvalidAmountException("Deposit amount must be strictly greater than zero.");
    }

    balance += amount;
}

void Account::withdraw(double amount)
{
    if (status == "CLOSED")
    {
        throw AccountClosedException(accountNumber);
    }

    if (amount <= 0)
    {
        throw InvalidAmountException("Withdrawal amount must be strictly greater than zero.");
    }

    std::string failureReason;
    if (!canWithdraw(amount, failureReason))
    {
        throw InsufficientBalanceException(balance, amount);
    }

    balance -= amount;
}

void Account::closeAccount()
{
    if (status == "CLOSED")
    {
        throw BankingException("Account #" + std::to_string(accountNumber) + " is already closed.");
    }

    if (balance < 0)
    {
        throw BankingException("Cannot close account #" + std::to_string(accountNumber) + 
                               "! Outstanding negative overdraft balance of " + UI::formatCurrency(balance) + 
                               " must be cleared first.");
    }

    status = "CLOSED";
}

void Account::display() const
{
    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                 SMARTBANK ACCOUNT PASSBOOK                  |\n";
    std::cout << "+------------------------------+------------------------------+\n" << UI::Color::RESET;

    auto printField = [](const std::string& label, const std::string& val, const std::string& color = UI::Color::BOLD_WHITE) {
        std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(28) << label 
                  << UI::Color::CYAN << "| " << color << std::left << std::setw(28) << val 
                  << UI::Color::CYAN << "|\n" << UI::Color::RESET;
    };

    printField("Account Number", std::to_string(accountNumber));
    printField("Customer ID", "CUST-" + std::to_string(customerId), UI::Color::BOLD_YELLOW);
    printField("Account Holder", name);
    printField("Account Type", getAccountType());
    printField("Current Balance", UI::formatCurrency(balance), balance >= 0 ? UI::Color::BOLD_GREEN : UI::Color::BOLD_RED);
    printField("Account Status", status, status == "ACTIVE" ? UI::Color::BOLD_GREEN : UI::Color::BOLD_RED);
    printField("Opening Date", openingDate);

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+------------------------------+------------------------------+\n";
    std::cout << UI::Color::RESET;
}

int Account::getAccountNumber() const
{
    return accountNumber;
}

int Account::getCustomerId() const
{
    return customerId;
}

std::string Account::getName() const
{
    return name;
}

double Account::getBalance() const
{
    return balance;
}

std::string Account::getStatus() const
{
    return status;
}

std::string Account::getOpeningDate() const
{
    return openingDate;
}

bool Account::isActive() const
{
    return status == "ACTIVE";
}

void Account::setCustomerId(int id)
{
    customerId = id;
}

void Account::setName(const std::string& n)
{
    name = n;
}

bool Account::operator>(const Account& other) const
{
    return this->balance > other.balance;
}

bool Account::operator<(const Account& other) const
{
    return this->balance < other.balance;
}

bool Account::operator==(const Account& other) const
{
    return this->accountNumber == other.accountNumber;
}

std::ostream& operator<<(std::ostream& os, const Account& acc)
{
    os << "Acc #" << acc.accountNumber << " (CUST-" << acc.customerId << ") [" << acc.getAccountType() << "] "
       << acc.name << " | Bal: " << UI::formatCurrency(acc.balance)
       << " | " << acc.status;
    return os;
}

int Account::getNextAccountNumber()
{
    return nextAccountNumber;
}

void Account::setNextAccountNumber(int nextId)
{
    nextAccountNumber = nextId;
}
