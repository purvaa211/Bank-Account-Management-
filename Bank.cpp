#include "Bank.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>
#include <fstream>
#include <iomanip>

Bank::~Bank()
{
    // Clean up heap-allocated Account objects to prevent memory leaks
    for (Account* account : accounts)
    {
        delete account;
    }
    accounts.clear();
}

void Bank::addAccount(Account* account)
{
    accounts.push_back(account);
}

Account* Bank::findAccount(int accountNumber)
{
    for (Account* account : accounts)
    {
        if (account->getAccountNumber() == accountNumber)
        {
            return account;
        }
    }

    // Account not found - throw custom exception with account number
    throw AccountNotFoundException(accountNumber);
}

void Bank::displayAllAccounts() const
{
    if (accounts.empty())
    {
        UI::printWarning("No bank accounts have been created yet. Please create an account first.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------+--------------------------------+-----------------+-----------------+\n";
    std::cout << "| ACC # | ACCOUNT HOLDER                 | ACCOUNT TYPE    | CURRENT BALANCE |\n";
    std::cout << "+-------+--------------------------------+-----------------+-----------------+\n" << UI::Color::RESET;

    double totalDeposits = 0.0;

    for (const Account* account : accounts)
    {
        std::cout << UI::Color::CYAN << "| " << UI::Color::BOLD_WHITE 
                  << std::left << std::setw(5) << account->getAccountNumber()
                  << UI::Color::CYAN << " | " << UI::Color::WHITE 
                  << std::left << std::setw(30) << account->getName()
                  << UI::Color::CYAN << " | " << UI::Color::YELLOW 
                  << std::left << std::setw(15) << account->getAccountType()
                  << UI::Color::CYAN << " | " << UI::Color::BOLD_GREEN 
                  << std::right << std::setw(15) << UI::formatCurrency(account->getBalance())
                  << UI::Color::CYAN << " |\n" << UI::Color::RESET;

        totalDeposits += account->getBalance();
    }

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+-------+--------------------------------+-----------------+-----------------+\n" << UI::Color::RESET;

    std::cout << UI::Color::BOLD_WHITE << " Total Accounts: " << UI::Color::BOLD_YELLOW << accounts.size() 
              << UI::Color::BOLD_WHITE << "   |   Total Deposits Under Management: " 
              << UI::Color::BOLD_GREEN << UI::formatCurrency(totalDeposits) 
              << UI::Color::RESET << "\n";
}

void Bank::saveAccounts() const
{
    if (accounts.empty())
    {
        UI::printWarning("No accounts to save. The bank database is currently empty.");
        return;
    }

    std::ofstream file("accounts.txt");

    if (!file)
    {
        UI::printError("Unable to open or create file 'accounts.txt' for writing.");
        return;
    }

    file << "======================================================================\n";
    file << "                 BANK ACCOUNT MANAGEMENT SYSTEM RECORD                \n";
    file << "======================================================================\n\n";

    double totalDeposits = 0.0;

    for (const Account* account : accounts)
    {
        file << "Account Number : " << account->getAccountNumber() << "\n";
        file << "Account Holder : " << account->getName() << "\n";
        file << "Account Type   : " << account->getAccountType() << "\n";
        file << "Balance        : " << UI::formatCurrency(account->getBalance()) << "\n";
        file << "Interest Rate  : " << std::fixed << std::setprecision(1) 
             << (account->getInterestRate() * 100.0) << "%\n";
        file << "----------------------------------------------------------------------\n";

        totalDeposits += account->getBalance();
    }

    file << "\nSUMMARY:\n";
    file << "Total Accounts : " << accounts.size() << "\n";
    file << "Total Reserves : " << UI::formatCurrency(totalDeposits) << "\n";
    file << "======================================================================\n";

    file.close();

    UI::printSuccess("Successfully saved " + std::to_string(accounts.size()) + 
                     " account record(s) to 'accounts.txt'!");
    UI::printInfo("You can open 'accounts.txt' in any text editor to view the persistent records.");
}

size_t Bank::getAccountCount() const
{
    return accounts.size();
}

double Bank::getTotalBankBalance() const
{
    double total = 0.0;
    for (const Account* account : accounts)
    {
        total += account->getBalance();
    }
    return total;
}