#include <iostream>
#include <iomanip>
#include <string>

#include "Customer.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "FixedDepositAccount.h"
#include "Bank.h"
#include "Exceptions.h"
#include "UIHelper.h"

/**
 * ============================================================================
 * SMARTBANK – SMART BANKING & TRANSACTION SECURITY MANAGEMENT SYSTEM
 * Course: Object-Oriented Programming (OOP) in C++
 * Author: Purva Deshmukh
 *
 * Core OOP Principles Demonstrated:
 *   1. Encapsulation (Private account & customer fields, validated methods)
 *   2. Abstraction (Pure virtual functions in abstract base Account)
 *   3. Inheritance (SavingsAccount, CurrentAccount, FixedDepositAccount)
 *   4. Runtime Polymorphism (Virtual dispatch via Account* base pointers in Bank)
 *   5. Function Overloading (findAccount by ID vs Name; findCustomer by ID vs Name)
 *   6. Operator Overloading (Account comparison >, <, == and stream << insertion)
 *   7. Constructors & Destructors (Member initialization lists, RAII cleanup)
 *   8. Static Members (Auto-incrementing ID sequences: Account, Customer, Txn, Alert)
 *   9. Exception Handling (Hierarchy of custom banking exceptions)
 *   10. Composition & Aggregation (Bank manages Customers, Accounts, Transactions)
 *   11. Friend Class (BankStatementGenerator for privileged statement generation)
 *   12. File Handling & Persistence (CSV persistence across application restarts)
 * ============================================================================
 */

void printMainMenu(size_t activeCustomers, size_t activeAccounts, double totalReserves, size_t totalAlerts)
{
    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+====================================================================+\n";
    std::cout << "|   SMARTBANK - BANK ACCOUNT & TRANSACTION SECURITY SYSTEM           |\n";
    std::cout << "|              C++ Object-Oriented Programming Project               |\n";
    std::cout << "+====================================================================+\n" << UI::Color::RESET;

    std::cout << UI::Color::BOLD_WHITE << " Customers: " << UI::Color::BOLD_YELLOW << activeCustomers
              << UI::Color::BOLD_WHITE << "  | Accounts: " << UI::Color::BOLD_YELLOW << activeAccounts 
              << UI::Color::BOLD_WHITE << "  | Reserves: " << UI::Color::BOLD_GREEN << UI::formatCurrency(totalReserves)
              << UI::Color::BOLD_WHITE << "  | Alerts: " 
              << (totalAlerts > 0 ? UI::Color::BOLD_RED : UI::Color::BOLD_GREEN) << totalAlerts
              << UI::Color::RESET << "\n";

    std::cout << UI::Color::CYAN;
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "CUSTOMER MANAGEMENT" << UI::Color::CYAN << "                                                |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [1] " << UI::Color::WHITE << "Register New Customer     " << UI::Color::GRAY << "(Name, Email, Phone contact profile)   " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [2] " << UI::Color::WHITE << "View Customer Directory   " << UI::Color::GRAY << "(List all customers & owned accounts)  " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [3] " << UI::Color::WHITE << "Update Customer Contact   " << UI::Color::GRAY << "(Edit email / phone by Customer ID)    " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "ACCOUNT MANAGEMENT" << UI::Color::CYAN << "                                                 |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [4] " << UI::Color::WHITE << "Create Savings Account    " << UI::Color::GRAY << "(Min Balance Rs. 1,000 | 4.0% interest) " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [5] " << UI::Color::WHITE << "Create Current Account    " << UI::Color::GRAY << "(Overdraft Protection | 0.0% interest)" << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [6] " << UI::Color::WHITE << "Create Fixed Deposit      " << UI::Color::GRAY << "(Term Investment | Guaranteed Maturity)" << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [7] " << UI::Color::WHITE << "Close Bank Account        " << UI::Color::GRAY << "(Validated Account Termination)        " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "FINANCIAL TRANSACTIONS" << UI::Color::CYAN << "                                             |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [8] " << UI::Color::WHITE << "Deposit Funds             " << UI::Color::GRAY << "(Credit account balance)               " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [9] " << UI::Color::WHITE << "Withdraw Funds            " << UI::Color::GRAY << "(Debit balance with limit check)       " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [10]" << UI::Color::WHITE << " Transfer Money            " << UI::Color::GRAY << "(Atomic transfer between 2 accounts)   " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "INQUIRY, STATEMENTS & AUDIT" << UI::Color::CYAN << "                                        |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [11]" << UI::Color::WHITE << " Fast Account Inquiry      " << UI::Color::GRAY << "(Passbook card lookup)                 " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [12]" << UI::Color::WHITE << " View All Accounts Registry" << UI::Color::GRAY << "(Tabular accounts directory)           " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [13]" << UI::Color::WHITE << " Transaction History      " << UI::Color::GRAY << "(Audit ledger with filters)            " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [14]" << UI::Color::WHITE << " Official Statement Export" << UI::Color::GRAY << "(Friend class certified statement)     " << UI::Color::CYAN << "|\n";
    std::cout << "+--------------------------------------------------------------------+\n";
    std::cout << "| " << UI::Color::BOLD_YELLOW << "SECURITY MONITORING & PLATFORM" << UI::Color::CYAN << "                                     |\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [15]" << UI::Color::BOLD_MAGENTA << " Security Center          " << UI::Color::GRAY << "(Rule-based Anomaly Monitor Alerts)    " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [16]" << UI::Color::CYAN << " Save & Persist Database  " << UI::Color::GRAY << "(Sync to data/ CSV files)              " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [17]" << UI::Color::CYAN << " Open Web Platform        " << UI::Color::GRAY << "(Launch Fintech GUI in Browser)        " << UI::Color::CYAN << "|\n";
    std::cout << "| " << UI::Color::BOLD_WHITE << " [18]" << UI::Color::RED << " Exit Application         " << UI::Color::GRAY << "(Safe shutdown and storage sync)       " << UI::Color::CYAN << "|\n";
    std::cout << "+====================================================================+\n" << UI::Color::RESET;
}

int main()
{
    UI::initTerminal();

    Bank bank;

    // Seed sample customers and accounts if starting completely fresh
    if (bank.getCustomerCount() == 0 && bank.getAccountCount() == 0)
    {
        Customer c1(201, "Vikram Malhotra", "vikram.malhotra@email.com", "+91 98765 43210");
        Customer c2(202, "Apex Global Enterprises", "finance@apexglobal.com", "+91 98111 22334");
        Customer c3(203, "Aarav Sharma", "aarav.sharma@email.com", "+91 97234 56789");

        bank.addCustomer(c1);
        bank.addCustomer(c2);
        bank.addCustomer(c3);

        bank.addAccount(new SavingsAccount(1001, 201, "Vikram Malhotra", 75000.0, "ACTIVE", "2026-10-01", 1000.0));
        bank.addAccount(new CurrentAccount(1002, 202, "Apex Global Enterprises", 150000.0, "ACTIVE", "2026-10-05", 50000.0));
        bank.addAccount(new FixedDepositAccount(1003, 203, "Aarav Sharma", 100000.0, "ACTIVE", "2026-10-10", 12, 0.0725, 107449.54, "2027-10-10"));

        bank.deposit(1001, 25000.0, "Direct corporate deposit");
        bank.transfer(1002, 1001, 20000.0, "Monthly vendor settlement");
        bank.saveData();
    }

    int choice = 0;

    do
    {
        if (std::cin.eof()) break;
        UI::clearScreen();
        printMainMenu(bank.getCustomerCount(), bank.getAccountCount(), 
                      bank.getTotalBankBalance(), bank.getFraudDetector().getTotalAlertsCount());

        choice = UI::getInt("\nEnter your operation choice (1-18): ", 1, 18);
        if (std::cin.eof() && choice == 1) break;

        switch (choice)
        {
            case 1: // Register New Customer
            {
                UI::printSectionHeader("REGISTER NEW CUSTOMER");
                std::string name = UI::getString("Enter Customer Full Name    : ");
                std::string email = UI::getString("Enter Customer Email Address : ");
                std::string phone = UI::getString("Enter Customer Phone Number  : ");

                try
                {
                    Customer newCust(name, email, phone);
                    bank.addCustomer(newCust);
                    UI::printSuccess("Customer profile successfully created!");
                    newCust.display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 2: // View Customer Directory
            {
                UI::printSectionHeader("REGISTERED CUSTOMERS DIRECTORY");
                bank.displayAllCustomers();

                std::string viewDetail = UI::getString("\nWould you like to inspect a customer's linked accounts? (yes/no): ");
                if (viewDetail == "yes" || viewDetail == "y")
                {
                    int cid = UI::getInt("Enter Customer ID number (e.g. 201): ");
                    try
                    {
                        bank.displayCustomerDetails(cid);
                    }
                    catch (const std::exception& e)
                    {
                        UI::printError(e.what());
                    }
                }
                UI::pauseScreen();
                break;
            }

            case 3: // Update Customer Contact Info
            {
                UI::printSectionHeader("UPDATE CUSTOMER CONTACT DETAILS");
                int cid = UI::getInt("Enter Customer ID to update (e.g. 201): ");

                try
                {
                    Customer* c = bank.findCustomer(cid);
                    std::cout << "Updating Customer: " << UI::Color::BOLD_WHITE << c->getFullName() << UI::Color::RESET << "\n";
                    std::cout << "Current Email : " << c->getEmail() << "\n";
                    std::cout << "Current Phone : " << c->getPhone() << "\n";

                    std::string newEmail = UI::getString("Enter New Email Address (leave '-' to keep unchanged): ");
                    std::string newPhone = UI::getString("Enter New Phone Number  (leave '-' to keep unchanged): ");

                    if (newEmail == "-") newEmail = "";
                    if (newPhone == "-") newPhone = "";

                    bank.updateCustomerContact(cid, newEmail, newPhone);
                    UI::printSuccess("Customer contact information updated successfully!");
                    c->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 4: // Create Savings Account
            {
                UI::printSectionHeader("CREATE SAVINGS ACCOUNT (4% INTEREST | RS. 1,000 MIN BAL)");
                int cid = UI::getInt("Enter Customer ID to link this account (e.g. 201): ");

                try
                {
                    Customer* c = bank.findCustomer(cid);
                    std::cout << "Linked Customer: " << UI::Color::BOLD_WHITE << c->getFullName() << UI::Color::RESET << "\n";

                    double balance = UI::getDouble("Enter Initial Opening Balance (Rs.) [Min 1000] : ", 1000.0);

                    Account* account = new SavingsAccount(cid, c->getFullName(), balance);
                    bank.addAccount(account);
                    UI::printSuccess("Savings Account successfully opened and linked to CUST-" + std::to_string(cid) + "!");
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 5: // Create Current Account
            {
                UI::printSectionHeader("CREATE CURRENT ACCOUNT (COMMERCIAL | OVERDRAFT PROTECTION)");
                int cid = UI::getInt("Enter Customer ID to link this account (e.g. 202): ");

                try
                {
                    Customer* c = bank.findCustomer(cid);
                    std::cout << "Linked Customer: " << UI::Color::BOLD_WHITE << c->getFullName() << UI::Color::RESET << "\n";

                    double balance = UI::getDouble("Enter Initial Opening Balance (Rs.)      : ", 0.0);
                    double overdraft = UI::getDouble("Enter Approved Overdraft Limit (Rs.)    : ", 0.0);

                    Account* account = new CurrentAccount(cid, c->getFullName(), balance, "", overdraft);
                    bank.addAccount(account);
                    UI::printSuccess("Current Account successfully opened with Rs. " + 
                                     std::to_string(overdraft) + " overdraft limit!");
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 6: // Create Fixed Deposit
            {
                UI::printSectionHeader("CREATE FIXED DEPOSIT ACCOUNT (QUARTERLY COMPOUNDING)");
                int cid = UI::getInt("Enter Customer ID to link this account (e.g. 203): ");

                try
                {
                    Customer* c = bank.findCustomer(cid);
                    std::cout << "Linked Customer: " << UI::Color::BOLD_WHITE << c->getFullName() << UI::Color::RESET << "\n";

                    double principal = UI::getDouble("Enter Fixed Deposit Principal (Rs.)     : ", 5000.0);
                    int term = UI::getInt("Enter Investment Term (6, 12, 24, 36 Months) : ", 1, 120);

                    double rate = (term >= 36) ? 0.075 : ((term >= 12) ? 0.070 : 0.065);

                    Account* account = new FixedDepositAccount(cid, c->getFullName(), principal, term, "", rate);
                    bank.addAccount(account);
                    UI::printSuccess("Fixed Deposit successfully booked for CUST-" + std::to_string(cid) + "!");
                    account->display();
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 7: // Close Bank Account
            {
                UI::printSectionHeader("CLOSE BANK ACCOUNT");
                int accNum = UI::getInt("Enter Account Number to Close : ");

                try
                {
                    Account* acc = bank.findAccount(accNum);
                    acc->display();

                    std::string confirm = UI::getString("Are you sure you want to permanently close this account? (yes/no): ");
                    if (confirm == "yes" || confirm == "y")
                    {
                        bank.closeAccount(accNum);
                        UI::printSuccess("Account #" + std::to_string(accNum) + " has been successfully closed.");
                    }
                    else
                    {
                        UI::printInfo("Account closure aborted.");
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 8: // Deposit
            {
                UI::printSectionHeader("DEPOSIT FUNDS");
                int accNum = UI::getInt("Enter Destination Account Number : ");

                try
                {
                    Account* account = bank.findAccount(accNum);
                    double prevBal = account->getBalance();
                    std::cout << "Holder: " << UI::Color::BOLD_WHITE << account->getName() 
                              << " (CUST-" << account->getCustomerId() << ")" << UI::Color::RESET 
                              << " | Current Balance: " << UI::Color::BOLD_GREEN 
                              << UI::formatCurrency(prevBal) << UI::Color::RESET << "\n";

                    double amount = UI::getDouble("Enter Deposit Amount (Rs.)       : ", 0.01);
                    std::string note = UI::getString("Enter Transaction Narration / Note: ");

                    Transaction tx = bank.deposit(accNum, amount, note);
                    UI::printSuccess("Deposit completed successfully!");
                    UI::printReceipt(accNum, account->getName(), account->getAccountType(), 
                                     "DEPOSIT", prevBal, amount, account->getBalance(), 
                                     tx.getTransactionId(), tx.getRiskLevel());

                    if (tx.getRiskLevel() != "LOW")
                    {
                        UI::printWarning("SECURITY MONITOR NOTICE: Flagged as " + 
                                          tx.getRiskLevel() + " risk: " + tx.getFlagReason());
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 9: // Withdraw
            {
                UI::printSectionHeader("WITHDRAW FUNDS");
                int accNum = UI::getInt("Enter Source Account Number : ");

                try
                {
                    Account* account = bank.findAccount(accNum);
                    double prevBal = account->getBalance();
                    std::cout << "Holder: " << UI::Color::BOLD_WHITE << account->getName() 
                              << " (CUST-" << account->getCustomerId() << ")" << UI::Color::RESET 
                              << " | Available Balance: " << UI::Color::BOLD_GREEN 
                              << UI::formatCurrency(prevBal) << UI::Color::RESET << "\n";

                    double amount = UI::getDouble("Enter Withdrawal Amount (Rs.)    : ", 0.01);
                    std::string note = UI::getString("Enter Withdrawal Reason / Note  : ");

                    Transaction tx = bank.withdraw(accNum, amount, note);
                    UI::printSuccess("Withdrawal completed successfully!");
                    UI::printReceipt(accNum, account->getName(), account->getAccountType(), 
                                     "WITHDRAWAL", prevBal, amount, account->getBalance(), 
                                     tx.getTransactionId(), tx.getRiskLevel());

                    if (tx.getRiskLevel() != "LOW")
                    {
                        UI::printWarning("SECURITY MONITOR NOTICE: Flagged as " + 
                                          tx.getRiskLevel() + " risk: " + tx.getFlagReason());
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 10: // Transfer Money (Atomic execution)
            {
                UI::printSectionHeader("TRANSFER MONEY BETWEEN ACCOUNTS");
                int senderAcc = UI::getInt("Enter Source (Sender) Account Number      : ");
                int receiverAcc = UI::getInt("Enter Destination (Receiver) Account Number: ");

                try
                {
                    Account* sender = bank.findAccount(senderAcc);
                    Account* receiver = bank.findAccount(receiverAcc);

                    std::cout << "Sender   : " << UI::Color::BOLD_WHITE << sender->getName() 
                              << " (Acc #" << senderAcc << " | CUST-" << sender->getCustomerId() << ")" << UI::Color::RESET 
                              << " | Balance: " << UI::Color::BOLD_GREEN << UI::formatCurrency(sender->getBalance()) << UI::Color::RESET << "\n";
                    std::cout << "Receiver : " << UI::Color::BOLD_WHITE << receiver->getName() 
                              << " (Acc #" << receiverAcc << " | CUST-" << receiver->getCustomerId() << ")" << UI::Color::RESET << "\n";

                    double amount = UI::getDouble("Enter Amount to Transfer (Rs.)            : ", 0.01);
                    std::string note = UI::getString("Enter Transfer Description / Purpose     : ");

                    Transaction tx = bank.transfer(senderAcc, receiverAcc, amount, note);
                    UI::printSuccess("Transfer successfully completed!");
                    UI::printTransferReceipt(tx.getTransactionId(), senderAcc, sender->getName(), 
                                             receiverAcc, receiver->getName(), amount, 
                                             sender->getBalance(), tx.getRiskLevel());

                    if (tx.getRiskLevel() != "LOW")
                    {
                        UI::printWarning("SECURITY MONITOR NOTICE: Flagged as " + 
                                          tx.getRiskLevel() + " risk: " + tx.getFlagReason());
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 11: // Fast Account Inquiry
            {
                UI::printSectionHeader("FAST ACCOUNT INQUIRY & PASSBOOK LOOKUP");
                std::cout << "[1] Search by Account Number\n[2] Search by Holder Name\n[3] Search Accounts by Customer ID\n";
                int sub = UI::getInt("Enter choice (1-3): ", 1, 3);

                try
                {
                    if (sub == 1)
                    {
                        int accNum = UI::getInt("Enter Account Number: ");
                        Account* acc = bank.findAccount(accNum);
                        acc->display();
                        acc->calculateInterest();
                    }
                    else if (sub == 2)
                    {
                        std::string name = UI::getString("Enter Account Holder Name: ");
                        Account* acc = bank.findAccount(name);
                        acc->display();
                        acc->calculateInterest();
                    }
                    else
                    {
                        int cid = UI::getInt("Enter Customer ID (e.g. 201): ");
                        bank.displayAccountsByCustomer(cid);
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 12: // View All Accounts Registry
            {
                UI::printSectionHeader("REGISTERED BANK ACCOUNTS DIRECTORY");
                bank.displayAllAccounts();
                UI::pauseScreen();
                break;
            }

            case 13: // Transaction History
            {
                UI::printSectionHeader("TRANSACTION HISTORY & AUDIT LEDGER");
                std::cout << "[1] View Global Bank Transaction History\n[2] View Specific Account History\n";
                int sub = UI::getInt("Enter choice (1-2): ", 1, 2);

                if (sub == 1)
                {
                    bank.displayTransactionHistory(0);
                }
                else
                {
                    int accNum = UI::getInt("Enter Account Number: ");
                    bank.displayTransactionHistory(accNum);
                }
                UI::pauseScreen();
                break;
            }

            case 14: // Statement Export (Friend class audit)
            {
                UI::printSectionHeader("ACCOUNT FINANCIAL STATEMENT (FRIEND CLASS AUDIT)");
                int accNum = UI::getInt("Enter Account Number for Official Statement: ");

                try
                {
                    bank.printAccountStatement(accNum);
                    std::string exportedPath = bank.exportAccountStatement(accNum);
                    if (!exportedPath.empty())
                    {
                        UI::printSuccess("Official Statement exported to file: " + exportedPath);
                    }
                }
                catch (const std::exception& e)
                {
                    UI::printError(e.what());
                }
                UI::pauseScreen();
                break;
            }

            case 15: // Security Center & Fraud Monitor
            {
                UI::printSectionHeader("TRANSACTION SECURITY & ANOMALY MONITOR");
                std::cout << "Protection Engine: " << UI::Color::BOLD_GREEN << "ACTIVE (Rule-Based Anomaly Monitor)" << UI::Color::RESET << "\n";
                std::cout << "Active Rules:\n";
                std::cout << "  1. Advisory Transaction Threshold : " << UI::formatCurrency(bank.getFraudDetector().getConfig().advisoryTransactionThreshold) << " (MEDIUM)\n";
                std::cout << "  2. Critical Transaction Threshold : " << UI::formatCurrency(bank.getFraudDetector().getConfig().criticalTransactionThreshold) << " (HIGH)\n";
                std::cout << "  3. Daily Cumulative Debit Limit   : " << UI::formatCurrency(bank.getFraudDetector().getConfig().dailyDebitLimit) << " (HIGH)\n";
                std::cout << "  4. Rapid Velocity Anomaly Window  : " << bank.getFraudDetector().getConfig().rapidMaxCount 
                          << " transactions within " << bank.getFraudDetector().getConfig().rapidWindowMinutes << " minutes\n\n";

                bank.displaySecurityAlerts();

                if (bank.getFraudDetector().getPendingReviewCount() > 0)
                {
                    std::string opt = UI::getString("\nWould you like to review/approve an alert? (yes/no): ");
                    if (opt == "yes" || opt == "y")
                    {
                        int alertId = UI::getInt("Enter Alert ID to update: ");
                        std::cout << "[1] APPROVED\n[2] REVIEWED\n[3] REJECTED\n";
                        int st = UI::getInt("Select new status (1-3): ", 1, 3);
                        std::string statStr = (st == 1) ? "APPROVED" : ((st == 2) ? "REVIEWED" : "REJECTED");
                        bank.reviewSecurityAlert(alertId, statStr);
                        UI::printSuccess("Alert #" + std::to_string(alertId) + " status updated to " + statStr);
                    }
                }
                UI::pauseScreen();
                break;
            }

            case 16: // Save & Persist
            {
                UI::printSectionHeader("PERSIST DATABASE RECORDS");
                bank.saveData();
                UI::printSuccess("All customer records, account registries, transaction ledgers, and security alerts saved to 'data/' folder!");
                UI::pauseScreen();
                break;
            }

            case 17: // Launch Web Platform
            {
                UI::printSectionHeader("LAUNCH SMARTBANK FINTECH WEB PLATFORM");
                UI::printInfo("Opening SmartBank Web Platform in default browser...");
#ifdef _WIN32
                system("start \"\" \"docs\\index.html\"");
#elif __APPLE__
                system("open docs/index.html");
#else
                system("xdg-open docs/index.html > /dev/null 2>&1 &");
#endif
                UI::pauseScreen();
                break;
            }

            case 18: // Exit
            {
                UI::printSectionHeader("SAFE APPLICATION SHUTDOWN");
                bank.saveData();
                UI::printSuccess("SmartBank state successfully synchronized with disk storage.");
                UI::printInfo("Thank you for using SmartBank Management System.");
                break;
            }
        }

    } while (choice != 18);

    return 0;
}
