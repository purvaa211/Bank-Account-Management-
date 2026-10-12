#include <iostream>
#include <iomanip>
#include <string>

#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "Bank.h"
#include "Exceptions.h"
#include "UIHelper.h"

/**
 * ============================================================================
 * BANK ACCOUNT MANAGEMENT SYSTEM - MAIN DRIVER & CONTROLLER
 * Demonstrates:
 *   - Object-Oriented Programming (Classes, Objects, Inheritance, Polymorphism)
 *   - Dynamic Memory Management (new / delete through Bank container)
 *   - Exception Handling (try / throw / catch hierarchy)
 *   - Separation of Concerns (UI separated from business logic)
 * ============================================================================
 */

void printMainMenu(size_t activeAccounts)
{
    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+====================================================================+\n";
    std::cout << "|                 BANK ACCOUNT MANAGEMENT SYSTEM                     |\n";
    std::cout << "|              C++ Object-Oriented Programming Project               |\n";
    std::cout << "+====================================================================+\n" << UI::Color::RESET;

    std::cout << UI::Color::BOLD_WHITE << " Status: " << UI::Color::GREEN << "Operational" 
              << UI::Color::BOLD_WHITE << "   |   Active Accounts: " << UI::Color::BOLD_YELLOW << activeAccounts 
              << UI::Color::RESET << "\n";

    std::cout << UI::Color::CYAN;
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "ACCOUNT CREATION & MANAGEMENT" << UI::Color::CYAN << "                                      |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [1] " << UI::Color::WHITE << "Create Savings Account    " << UI::Color::GRAY << "(Earns 4.0% annual interest)       " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [2] " << UI::Color::WHITE << "Create Current Account    " << UI::Color::GRAY << "(Transactional / Checking account) " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "TRANSACTIONS & OPERATIONS" << UI::Color::CYAN << "                                          |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [3] " << UI::Color::WHITE << "Deposit Funds             " << UI::Color::GRAY << "(Add funds to balance)             " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [4] " << UI::Color::WHITE << "Withdraw Funds            " << UI::Color::GRAY << "(Deduct funds from balance)        " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [5] " << UI::Color::WHITE << "Check Balance             " << UI::Color::GRAY << "(Fast balance inquiry)             " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "REPORTS & FILE PERSISTENCE" << UI::Color::CYAN << "                                         |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [6] " << UI::Color::WHITE << "Display Account Details   " << UI::Color::GRAY << "(Full passbook card view)          " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [7] " << UI::Color::WHITE << "Display All Accounts      " << UI::Color::GRAY << "(Tabular list of all accounts)     " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [8] " << UI::Color::WHITE << "Calculate Interest        " << UI::Color::GRAY << "(Polymorphic interest simulation)  " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [9] " << UI::Color::WHITE << "Save Accounts to File     " << UI::Color::GRAY << "(Export database to accounts.txt)  " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [10] " << UI::Color::CYAN << "Open Web Platform in Browser" << UI::Color::GRAY << " (Interactive GUI & OOP Inspector)" << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [11] " << UI::Color::RED << "Exit Program" << UI::Color::CYAN << "                                                 |\n";
    std::cout << "+====================================================================+\n" << UI::Color::RESET;
}

int main()
{
    // Initialize Windows terminal colors and UTF-8 support
    UI::initTerminal();

    // Automatically launch the interactive web platform in the default browser
    std::cout << UI::Color::BOLD_CYAN << "[LAUNCH] Opening Interactive Web Platform in default browser...\n" << UI::Color::RESET;
#ifdef _WIN32
    system("start docs\\index.html");
#elif __APPLE__
    system("open docs/index.html");
#else
    system("xdg-open docs/index.html > /dev/null 2>&1 &");
#endif

    Bank bank;
    int choice = 0;

    do
    {
        UI::clearScreen();
        printMainMenu(bank.getAccountCount());

        choice = UI::getInt("\nEnter your choice (1-11): ", 1, 11);

        switch (choice)
        {
            case 1: // Create Savings Account
            {
                UI::printSectionHeader("CREATE SAVINGS ACCOUNT (4% INTEREST)");
                std::string name = UI::getString("Enter Account Holder's Full Name : ");
                double balance = UI::getDouble("Enter Initial Opening Balance ($)   : ", 0.0);

                try
                {
                    // OOP Concept: Dynamic object creation & Inheritance
                    Account* account = new SavingsAccount(name, balance);
                    bank.addAccount(account);

                    UI::printSuccess("Savings Account successfully opened!");
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 2: // Create Current Account
            {
                UI::printSectionHeader("CREATE CURRENT ACCOUNT (0% INTEREST)");
                std::string name = UI::getString("Enter Account Holder's Full Name : ");
                double balance = UI::getDouble("Enter Initial Opening Balance ($)   : ", 0.0);

                try
                {
                    // OOP Concept: Dynamic object creation & Inheritance
                    Account* account = new CurrentAccount(name, balance);
                    bank.addAccount(account);

                    UI::printSuccess("Current Account successfully opened!");
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 3: // Deposit Funds
            {
                UI::printSectionHeader("DEPOSIT FUNDS");
                int accNum = UI::getInt("Enter Account Number : ", 1000);

                try
                {
                    Account* account = bank.findAccount(accNum);
                    double prevBal = account->getBalance();

                    std::cout << UI::Color::CYAN << "Verified Account Holder: " << UI::Color::BOLD_WHITE 
                              << account->getName() << " (" << account->getAccountType() << ")\n" 
                              << "Current Balance: " << UI::Color::BOLD_GREEN 
                              << UI::formatCurrency(prevBal) << UI::Color::RESET << "\n";

                    double amount = UI::getDouble("Enter Deposit Amount ($)  : ", 0.01);

                    account->deposit(amount);
                    double newBal = account->getBalance();

                    UI::printSuccess("Funds deposited successfully!");
                    UI::printReceipt(accNum, account->getName(), account->getAccountType(), 
                                     "DEPOSIT", prevBal, amount, newBal);
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 4: // Withdraw Funds
            {
                UI::printSectionHeader("WITHDRAW FUNDS");
                int accNum = UI::getInt("Enter Account Number : ", 1000);

                try
                {
                    Account* account = bank.findAccount(accNum);
                    double prevBal = account->getBalance();

                    std::cout << UI::Color::CYAN << "Verified Account Holder: " << UI::Color::BOLD_WHITE 
                              << account->getName() << " (" << account->getAccountType() << ")\n" 
                              << "Available Balance: " << UI::Color::BOLD_GREEN 
                              << UI::formatCurrency(prevBal) << UI::Color::RESET << "\n";

                    double amount = UI::getDouble("Enter Withdrawal Amount ($) : ", 0.01);

                    account->withdraw(amount);
                    double newBal = account->getBalance();

                    UI::printSuccess("Withdrawal processed successfully!");
                    UI::printReceipt(accNum, account->getName(), account->getAccountType(), 
                                     "WITHDRAWAL", prevBal, amount, newBal);
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 5: // Check Balance
            {
                UI::printSectionHeader("CHECK BALANCE INQUIRY");
                int accNum = UI::getInt("Enter Account Number : ", 1000);

                try
                {
                    Account* account = bank.findAccount(accNum);

                    std::cout << "\n" << UI::Color::BOLD_CYAN;
                    std::cout << "+-------------------------------------------------------------+\n";
                    std::cout << "|                      BALANCE INQUIRY                        |\n";
                    std::cout << "+------------------------------+------------------------------+\n";
                    std::cout << "| Account Number               | " << UI::Color::BOLD_WHITE << std::left << std::setw(28) << account->getAccountNumber() << UI::Color::CYAN << "|\n";
                    std::cout << "| Account Holder               | " << UI::Color::BOLD_WHITE << std::left << std::setw(28) << account->getName() << UI::Color::CYAN << "|\n";
                    std::cout << "| Account Type                 | " << UI::Color::YELLOW << std::left << std::setw(28) << account->getAccountType() << UI::Color::CYAN << "|\n";
                    std::cout << "| Available Balance            | " << UI::Color::BOLD_GREEN << std::left << std::setw(28) << UI::formatCurrency(account->getBalance()) << UI::Color::CYAN << "|\n";
                    std::cout << "+-------------------------------------------------------------+\n" << UI::Color::RESET;
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 6: // Display Account Details
            {
                UI::printSectionHeader("DISPLAY ACCOUNT DETAILS");
                int accNum = UI::getInt("Enter Account Number : ", 1000);

                try
                {
                    Account* account = bank.findAccount(accNum);
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 7: // Display All Accounts
            {
                UI::printSectionHeader("DIRECTORY OF ALL BANK ACCOUNTS");
                bank.displayAllAccounts();
                UI::pauseScreen();
                break;
            }

            case 8: // Calculate Interest (Polymorphism in action)
            {
                UI::printSectionHeader("CALCULATE & PROJECT INTEREST");
                int accNum = UI::getInt("Enter Account Number : ", 1000);

                try
                {
                    Account* account = bank.findAccount(accNum);
                    // OOP Concept: Runtime Polymorphism
                    // calculateInterest() dynamically executes SavingsAccount or CurrentAccount method
                    account->calculateInterest();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 9: // Save Accounts to File
            {
                UI::printSectionHeader("EXPORT DATABASE TO FILE");
                bank.saveAccounts();
                UI::pauseScreen();
                break;
            }

            case 10: // Launch Web Platform in Browser
            {
                UI::printSectionHeader("LAUNCH WEB PLATFORM");
                UI::printInfo("Opening interactive web platform in your default browser...");
#ifdef _WIN32
                system("start docs\\index.html");
#elif __APPLE__
                system("open docs/index.html");
#else
                system("xdg-open docs/index.html > /dev/null 2>&1 &");
#endif
                UI::printSuccess("Interactive web platform opened successfully!");
                UI::pauseScreen();
                break;
            }

            case 11: // Exit
            {
                UI::clearScreen();
                std::cout << UI::Color::BOLD_CYAN;
                std::cout << "======================================================================\n";
                std::cout << "          THANK YOU FOR USING BANK ACCOUNT MANAGEMENT SYSTEM          \n";
                std::cout << "======================================================================\n" << UI::Color::RESET;
                std::cout << UI::Color::GREEN << "All active records have been safely finalized in memory.\n";
                std::cout << "Goodbye and have a productive day!\n\n" << UI::Color::RESET;
                break;
            }

            default:
            {
                UI::printError("Invalid selection. Please choose an option between 1 and 11.");
                UI::pauseScreen();
                break;
            }
        }

    } while (choice != 11);

    return 0;
}