#include "BankStatementGenerator.h"
#include "UIHelper.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void BankStatementGenerator::generateConsoleStatement(const Account& acc, const std::vector<Transaction>& transactions)
{
    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "===============================================================================\n";
    std::cout << "                  SMARTBANK OFFICIAL FINANCIAL STATEMENT                       \n";
    std::cout << "===============================================================================\n" << UI::Color::RESET;

    std::cout << UI::Color::WHITE << " Account Number : " << UI::Color::BOLD_YELLOW << acc.accountNumber
              << UI::Color::WHITE << "    | Customer ID    : " << UI::Color::BOLD_YELLOW << "CUST-" << acc.customerId << "\n";
    std::cout << UI::Color::WHITE << " Account Holder : " << UI::Color::BOLD_WHITE << acc.name
              << UI::Color::WHITE << "    | Account Type   : " << UI::Color::CYAN << acc.getAccountType() << "\n";
    std::cout << UI::Color::WHITE << " Current Balance: " << UI::Color::BOLD_GREEN << UI::formatCurrency(acc.balance)
              << UI::Color::WHITE << "    | Status         : " << (acc.status == "ACTIVE" ? UI::Color::BOLD_GREEN : UI::Color::BOLD_RED) 
              << acc.status << UI::Color::RESET << "\n";
    std::cout << UI::Color::WHITE << " Opening Date   : " << UI::Color::GRAY << acc.openingDate << UI::Color::RESET << "\n";

    std::cout << UI::Color::CYAN;
    std::cout << "+---------------------+---------------+-------------+-------------+-----------+\n";
    std::cout << "| TIMESTAMP           | TXN ID        | TYPE        | AMOUNT      | STATUS    |\n";
    std::cout << "+---------------------+---------------+-------------+-------------+-----------+\n" << UI::Color::RESET;

    int count = 0;
    double totalDeposits = 0.0;
    double totalWithdrawals = 0.0;

    for (const auto& tx : transactions)
    {
        if (tx.getSenderAccount() == acc.accountNumber || tx.getReceiverAccount() == acc.accountNumber)
        {
            count++;
            bool isCredit = (tx.getReceiverAccount() == acc.accountNumber) || (tx.getType() == "DEPOSIT");
            if (isCredit) totalDeposits += tx.getAmount();
            else totalWithdrawals += tx.getAmount();

            std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(19) << tx.getTimestamp()
                      << UI::Color::CYAN << " | " << UI::Color::YELLOW << std::left << std::setw(13) << tx.getTransactionId()
                      << UI::Color::CYAN << " | " << (isCredit ? UI::Color::GREEN : UI::Color::RED) << std::left << std::setw(11) << tx.getType()
                      << UI::Color::CYAN << " | " << UI::Color::BOLD_WHITE << std::right << std::setw(11) << UI::formatCurrency(tx.getAmount())
                      << UI::Color::CYAN << " | " << (tx.getStatus() == "SUCCESS" ? UI::Color::GREEN : UI::Color::YELLOW) 
                      << std::left << std::setw(9) << tx.getStatus()
                      << UI::Color::CYAN << " |\n" << UI::Color::RESET;
        }
    }

    if (count == 0)
    {
        std::cout << UI::Color::CYAN << "| " << UI::Color::GRAY 
                  << "No transactions recorded yet for this account." 
                  << std::setw(29) << " " << UI::Color::CYAN << "|\n" << UI::Color::RESET;
    }

    std::cout << UI::Color::CYAN;
    std::cout << "+---------------------+---------------+-------------+-------------+-----------+\n" << UI::Color::RESET;
    std::cout << UI::Color::BOLD_WHITE << " Total Inflows : " << UI::Color::BOLD_GREEN << UI::formatCurrency(totalDeposits)
              << UI::Color::BOLD_WHITE << "   | Total Outflows: " << UI::Color::BOLD_RED << UI::formatCurrency(totalWithdrawals)
              << UI::Color::RESET << "\n\n";
}

std::string BankStatementGenerator::exportFileStatement(const Account& acc, const std::vector<Transaction>& transactions, const std::string& outputFolder)
{
    std::string filename = outputFolder + "/statement_" + std::to_string(acc.accountNumber) + ".txt";
    std::ofstream out(filename);

    if (!out)
    {
        return "";
    }

    out << "===============================================================================\n";
    out << "                    SMARTBANK FINANCIAL STATEMENT                             \n";
    out << "===============================================================================\n";
    out << "Account Number : " << acc.accountNumber << "\n";
    out << "Customer ID    : CUST-" << acc.customerId << "\n";
    out << "Account Holder : " << acc.name << "\n";
    out << "Account Type   : " << acc.getAccountType() << "\n";
    out << "Account Status : " << acc.status << "\n";
    out << "Opening Date   : " << acc.openingDate << "\n";
    out << "Closing Balance: " << UI::formatCurrency(acc.balance) << "\n";
    out << "-------------------------------------------------------------------------------\n";
    out << "TRANSACTION LEDGER:\n";
    out << "Timestamp            | Txn ID       | Type        | Amount       | Status\n";
    out << "---------------------+--------------+-------------+--------------+-------------\n";

    double totalDeposits = 0.0;
    double totalWithdrawals = 0.0;

    for (const auto& tx : transactions)
    {
        if (tx.getSenderAccount() == acc.accountNumber || tx.getReceiverAccount() == acc.accountNumber)
        {
            bool isCredit = (tx.getReceiverAccount() == acc.accountNumber) || (tx.getType() == "DEPOSIT");
            if (isCredit) totalDeposits += tx.getAmount();
            else totalWithdrawals += tx.getAmount();

            out << std::left << std::setw(20) << tx.getTimestamp() << " | "
                << std::left << std::setw(12) << tx.getTransactionId() << " | "
                << std::left << std::setw(11) << tx.getType() << " | "
                << std::right << std::setw(12) << UI::formatCurrency(tx.getAmount()) << " | "
                << tx.getStatus() << "\n";
        }
    }

    out << "-------------------------------------------------------------------------------\n";
    out << "Total Inflows : " << UI::formatCurrency(totalDeposits) << "\n";
    out << "Total Outflows: " << UI::formatCurrency(totalWithdrawals) << "\n";
    out << "Net Balance   : " << UI::formatCurrency(acc.balance) << "\n";
    out << "===============================================================================\n";
    out.close();

    return filename;
}
