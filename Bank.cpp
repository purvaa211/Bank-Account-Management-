#include "Bank.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include "BankStatementGenerator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Bank::Bank()
    : database("data")
{
    loadData();
}

Bank::~Bank()
{
    saveData();
    for (Account* account : accounts)
    {
        delete account;
    }
    accounts.clear();
}

void Bank::addAccount(Account* account)
{
    accounts.push_back(account);
    saveData();
}

void Bank::closeAccount(int accountNumber)
{
    Account* acc = findAccount(accountNumber);
    acc->closeAccount();

    // Record system transaction for account closure
    Transaction closeTx(accountNumber, -1, "CLOSE_ACCOUNT", 0.0, 
                        Transaction::generateCurrentTimestamp(), "SUCCESS", "LOW", 
                        "Account closed by user request.");
    transactions.push_back(closeTx);
    saveData();
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
    throw AccountNotFoundException(accountNumber);
}

Account* Bank::findAccount(const std::string& holderName)
{
    for (Account* account : accounts)
    {
        if (account->getName() == holderName)
        {
            return account;
        }
    }
    throw AccountNotFoundException(holderName);
}

Transaction Bank::deposit(int accountNumber, double amount, const std::string& note)
{
    Account* acc = findAccount(accountNumber);
    acc->deposit(amount);

    std::string timestamp = Transaction::generateCurrentTimestamp();
    Transaction tx(accountNumber, -1, "DEPOSIT", amount, timestamp, "SUCCESS", "LOW", 
                   note.empty() ? "Standard deposit" : note);

    // Evaluate with Fraud Detector
    SecurityAlert alert = fraudDetector.evaluateTransaction(tx, transactions);
    if (alert.riskLevel != "LOW")
    {
        tx.setRiskLevel(alert.riskLevel, alert.reason);
        fraudDetector.logAlert(alert);
    }

    transactions.push_back(tx);
    saveData();
    return tx;
}

Transaction Bank::withdraw(int accountNumber, double amount, const std::string& note)
{
    Account* acc = findAccount(accountNumber);
    acc->withdraw(amount);

    std::string timestamp = Transaction::generateCurrentTimestamp();
    Transaction tx(accountNumber, -1, "WITHDRAWAL", amount, timestamp, "SUCCESS", "LOW", 
                   note.empty() ? "Cash withdrawal" : note);

    // Evaluate with Fraud Detector
    SecurityAlert alert = fraudDetector.evaluateTransaction(tx, transactions);
    if (alert.riskLevel != "LOW")
    {
        tx.setRiskLevel(alert.riskLevel, alert.reason);
        fraudDetector.logAlert(alert);
    }

    transactions.push_back(tx);
    saveData();
    return tx;
}

Transaction Bank::transfer(int senderAccount, int receiverAccount, double amount, const std::string& note)
{
    if (senderAccount == receiverAccount)
    {
        throw BankingException("Transfer failed: Sender and receiver account numbers cannot be identical.");
    }

    if (amount <= 0)
    {
        throw InvalidAmountException("Transfer amount must be strictly greater than zero.");
    }

    Account* sender = findAccount(senderAccount);
    Account* receiver = findAccount(receiverAccount);

    if (!sender->isActive())
    {
        throw AccountClosedException(senderAccount);
    }

    if (!receiver->isActive())
    {
        throw AccountClosedException(receiverAccount);
    }

    // Atomic transaction execution:
    // 1. Validate withdrawal from sender first
    std::string failReason;
    if (!sender->canWithdraw(amount, failReason))
    {
        throw InsufficientBalanceException(sender->getBalance(), amount);
    }

    // 2. Perform withdrawal and deposit atomically
    sender->withdraw(amount);
    receiver->deposit(amount);

    std::string timestamp = Transaction::generateCurrentTimestamp();
    Transaction tx(senderAccount, receiverAccount, "TRANSFER", amount, timestamp, "SUCCESS", "LOW", 
                   note.empty() ? "Fund transfer to Acc #" + std::to_string(receiverAccount) : note);

    // Evaluate with Fraud Detector
    SecurityAlert alert = fraudDetector.evaluateTransaction(tx, transactions);
    if (alert.riskLevel != "LOW")
    {
        tx.setRiskLevel(alert.riskLevel, alert.reason);
        fraudDetector.logAlert(alert);
    }

    transactions.push_back(tx);
    saveData();
    return tx;
}

FraudDetector& Bank::getFraudDetector()
{
    return fraudDetector;
}

const FraudDetector& Bank::getFraudDetector() const
{
    return fraudDetector;
}

void Bank::reviewSecurityAlert(int alertId, const std::string& newStatus)
{
    fraudDetector.reviewAlert(alertId, newStatus);
    saveData();
}

void Bank::displaySecurityAlerts() const
{
    const auto& alerts = fraudDetector.getAlerts();
    if (alerts.empty())
    {
        UI::printSuccess("Zero security alerts recorded. All monitored transactions are operating within normal thresholds.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+---------+---------------+-------+---------------+---------+----------------------------------------------+-------------------+\n";
    std::cout << "| ALERT ID| TXN ID        | ACC # | AMOUNT        | RISK    | SECURITY FLAG REASON                         | REVIEW STATUS     |\n";
    std::cout << "+---------+---------------+-------+---------------+---------+----------------------------------------------+-------------------+\n" << UI::Color::RESET;

    for (const auto& a : alerts)
    {
        std::string riskColor = (a.riskLevel == "HIGH") ? UI::Color::BOLD_RED : UI::Color::BOLD_YELLOW;
        std::string statusColor = (a.reviewStatus == "APPROVED") ? UI::Color::GREEN : UI::Color::YELLOW;

        std::string shortReason = a.reason;
        if (shortReason.length() > 44) shortReason = shortReason.substr(0, 41) + "...";

        std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(7) << a.alertId
                  << UI::Color::CYAN << " | " << UI::Color::YELLOW << std::left << std::setw(13) << a.transactionId
                  << UI::Color::CYAN << " | " << UI::Color::WHITE << std::left << std::setw(5) << a.accountNumber
                  << UI::Color::CYAN << " | " << UI::Color::BOLD_WHITE << std::right << std::setw(13) << UI::formatCurrency(a.amount)
                  << UI::Color::CYAN << " | " << riskColor << std::left << std::setw(7) << a.riskLevel
                  << UI::Color::CYAN << " | " << UI::Color::WHITE << std::left << std::setw(44) << shortReason
                  << UI::Color::CYAN << " | " << statusColor << std::left << std::setw(17) << a.reviewStatus
                  << UI::Color::CYAN << " |\n" << UI::Color::RESET;
    }

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+---------+---------------+-------+---------------+---------+----------------------------------------------+-------------------+\n" << UI::Color::RESET;
    std::cout << UI::Color::BOLD_WHITE << " Total Alerts Flagged: " << UI::Color::BOLD_YELLOW << alerts.size()
              << UI::Color::BOLD_WHITE << "   | High Risk: " << UI::Color::BOLD_RED << fraudDetector.getHighRiskCount()
              << UI::Color::BOLD_WHITE << "   | Medium Risk: " << UI::Color::BOLD_YELLOW << fraudDetector.getMediumRiskCount()
              << UI::Color::BOLD_WHITE << "   | Pending Review: " << UI::Color::CYAN << fraudDetector.getPendingReviewCount()
              << UI::Color::RESET << "\n";
}

void Bank::displayAllAccounts() const
{
    if (accounts.empty())
    {
        UI::printWarning("No bank accounts registered in system. Please create an account first.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------+--------------------------------+-----------------+-----------------+-----------+\n";
    std::cout << "| ACC # | ACCOUNT HOLDER                 | ACCOUNT TYPE    | CURRENT BALANCE | STATUS    |\n";
    std::cout << "+-------+--------------------------------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;

    double totalDeposits = 0.0;

    for (const Account* account : accounts)
    {
        std::string statusColor = account->isActive() ? UI::Color::GREEN : UI::Color::RED;

        std::cout << UI::Color::CYAN << "| " << UI::Color::BOLD_WHITE 
                  << std::left << std::setw(5) << account->getAccountNumber()
                  << UI::Color::CYAN << " | " << UI::Color::WHITE 
                  << std::left << std::setw(30) << account->getName()
                  << UI::Color::CYAN << " | " << UI::Color::YELLOW 
                  << std::left << std::setw(15) << account->getAccountType()
                  << UI::Color::CYAN << " | " << (account->getBalance() >= 0 ? UI::Color::BOLD_GREEN : UI::Color::BOLD_RED)
                  << std::right << std::setw(15) << UI::formatCurrency(account->getBalance())
                  << UI::Color::CYAN << " | " << statusColor 
                  << std::left << std::setw(9) << account->getStatus()
                  << UI::Color::CYAN << " |\n" << UI::Color::RESET;

        if (account->isActive())
        {
            totalDeposits += account->getBalance();
        }
    }

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+-------+--------------------------------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;

    std::cout << UI::Color::BOLD_WHITE << " Total Accounts: " << UI::Color::BOLD_YELLOW << accounts.size() 
              << UI::Color::BOLD_WHITE << "   |   Total Deposits Under Management: " 
              << UI::Color::BOLD_GREEN << UI::formatCurrency(totalDeposits) 
              << UI::Color::RESET << "\n";
}

void Bank::displayTransactionHistory(int accountNumber) const
{
    if (transactions.empty())
    {
        UI::printWarning("No transactions recorded yet in banking ledger.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+---------------+---------------------+-------------+-------+-------+---------------+---------+---------+\n";
    std::cout << "| TXN ID        | TIMESTAMP           | TYPE        | FROM  | TO    | AMOUNT        | STATUS  | RISK    |\n";
    std::cout << "+---------------+---------------------+-------------+-------+-------+---------------+---------+---------+\n" << UI::Color::RESET;

    int displayCount = 0;
    for (const auto& tx : transactions)
    {
        if (accountNumber == 0 || tx.getSenderAccount() == accountNumber || tx.getReceiverAccount() == accountNumber)
        {
            displayCount++;
            std::string toStr = tx.getReceiverAccount() == -1 ? "-" : std::to_string(tx.getReceiverAccount());
            std::string riskCol = (tx.getRiskLevel() == "HIGH") ? UI::Color::RED : (tx.getRiskLevel() == "MEDIUM" ? UI::Color::YELLOW : UI::Color::GREEN);

            std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(13) << tx.getTransactionId()
                      << UI::Color::CYAN << " | " << UI::Color::GRAY << std::left << std::setw(19) << tx.getTimestamp()
                      << UI::Color::CYAN << " | " << UI::Color::YELLOW << std::left << std::setw(11) << tx.getType()
                      << UI::Color::CYAN << " | " << UI::Color::WHITE << std::left << std::setw(5) << tx.getSenderAccount()
                      << UI::Color::CYAN << " | " << UI::Color::WHITE << std::left << std::setw(5) << toStr
                      << UI::Color::CYAN << " | " << UI::Color::BOLD_WHITE << std::right << std::setw(13) << UI::formatCurrency(tx.getAmount())
                      << UI::Color::CYAN << " | " << (tx.getStatus() == "SUCCESS" ? UI::Color::GREEN : UI::Color::RED) << std::left << std::setw(7) << tx.getStatus()
                      << UI::Color::CYAN << " | " << riskCol << std::left << std::setw(7) << tx.getRiskLevel()
                      << UI::Color::CYAN << " |\n" << UI::Color::RESET;
        }
    }

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+---------------+---------------------+-------------+-------+-------+---------------+---------+---------+\n" << UI::Color::RESET;
    std::cout << UI::Color::BOLD_WHITE << " Displayed " << UI::Color::BOLD_YELLOW << displayCount 
              << UI::Color::BOLD_WHITE << " recorded transaction(s).\n" << UI::Color::RESET;
}

std::string Bank::exportAccountStatement(int accountNumber) const
{
    const Account* acc = nullptr;
    for (const auto* a : accounts)
    {
        if (a->getAccountNumber() == accountNumber)
        {
            acc = a;
            break;
        }
    }
    if (!acc) throw AccountNotFoundException(accountNumber);

    return BankStatementGenerator::exportFileStatement(*acc, transactions, "statements");
}

void Bank::printAccountStatement(int accountNumber) const
{
    const Account* acc = nullptr;
    for (const auto* a : accounts)
    {
        if (a->getAccountNumber() == accountNumber)
        {
            acc = a;
            break;
        }
    }
    if (!acc) throw AccountNotFoundException(accountNumber);

    BankStatementGenerator::generateConsoleStatement(*acc, transactions);
}

void Bank::saveData() const
{
    database.saveAccounts(accounts);
    database.saveTransactions(transactions);
    database.saveAlerts(fraudDetector.getAlerts());
}

void Bank::loadData()
{
    // Clean existing accounts
    for (Account* a : accounts) delete a;
    accounts.clear();

    accounts = database.loadAccounts();
    transactions = database.loadTransactions();
    auto alerts = database.loadAlerts();
    for (const auto& a : alerts)
    {
        fraudDetector.logAlert(a);
    }
}

size_t Bank::getAccountCount() const
{
    return accounts.size();
}

double Bank::getTotalBankBalance() const
{
    double total = 0.0;
    for (const Account* a : accounts)
    {
        if (a->isActive()) total += a->getBalance();
    }
    return total;
}

size_t Bank::getTotalTransactionsCount() const
{
    return transactions.size();
}

double Bank::getTotalWithdrawals() const
{
    double total = 0.0;
    for (const auto& tx : transactions)
    {
        if ((tx.getType() == "WITHDRAWAL" || tx.getType() == "TRANSFER") && tx.getStatus() == "SUCCESS")
        {
            total += tx.getAmount();
        }
    }
    return total;
}

const std::vector<Account*>& Bank::getAccounts() const
{
    return accounts;
}

const std::vector<Transaction>& Bank::getTransactions() const
{
    return transactions;
}
