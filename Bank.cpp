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

// ============================================================================
// CUSTOMER MANAGEMENT
// ============================================================================

void Bank::addCustomer(const Customer& customer)
{
    customers.push_back(customer);
    saveData();
}

Customer* Bank::findCustomer(int customerId)
{
    for (auto& c : customers)
    {
        if (c.getCustomerId() == customerId)
        {
            return &c;
        }
    }
    throw BankingException("Customer not found! No customer record with ID: CUST-" + std::to_string(customerId));
}

Customer* Bank::findCustomer(const std::string& customerName)
{
    for (auto& c : customers)
    {
        if (c.getFullName() == customerName)
        {
            return &c;
        }
    }
    throw BankingException("Customer not found! No customer record matching name: " + customerName);
}

void Bank::updateCustomerContact(int customerId, const std::string& email, const std::string& phone)
{
    Customer* c = findCustomer(customerId);
    if (!email.empty()) c->setEmail(email);
    if (!phone.empty()) c->setPhone(phone);
    saveData();
}

std::vector<Account*> Bank::getAccountsByCustomerId(int customerId) const
{
    std::vector<Account*> result;
    for (Account* acc : accounts)
    {
        if (acc->getCustomerId() == customerId)
        {
            result.push_back(acc);
        }
    }
    return result;
}

void Bank::displayAllCustomers() const
{
    if (customers.empty())
    {
        UI::printWarning("No customer profiles registered. Please add a customer first.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-----------+--------------------------------+----------------------------+-----------------+----------+\n";
    std::cout << "| CUST ID   | CUSTOMER FULL NAME             | EMAIL ADDRESS              | PHONE NUMBER    | ACCOUNTS |\n";
    std::cout << "+-----------+--------------------------------+----------------------------+-----------------+----------+\n" << UI::Color::RESET;

    for (const auto& c : customers)
    {
        size_t accCount = getAccountsByCustomerId(c.getCustomerId()).size();

        std::cout << UI::Color::CYAN << "| " << UI::Color::BOLD_YELLOW 
                  << std::left << std::setw(9) << ("CUST-" + std::to_string(c.getCustomerId()))
                  << UI::Color::CYAN << " | " << UI::Color::BOLD_WHITE 
                  << std::left << std::setw(30) << c.getFullName()
                  << UI::Color::CYAN << " | " << UI::Color::WHITE 
                  << std::left << std::setw(26) << (c.getEmail().empty() ? "-" : c.getEmail())
                  << UI::Color::CYAN << " | " << UI::Color::WHITE 
                  << std::left << std::setw(15) << (c.getPhone().empty() ? "-" : c.getPhone())
                  << UI::Color::CYAN << " | " << UI::Color::BOLD_GREEN 
                  << std::right << std::setw(8) << accCount
                  << UI::Color::CYAN << " |\n" << UI::Color::RESET;
    }

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+-----------+--------------------------------+----------------------------+-----------------+----------+\n" << UI::Color::RESET;
    std::cout << UI::Color::BOLD_WHITE << " Total Registered Customers: " << UI::Color::BOLD_YELLOW << customers.size() << UI::Color::RESET << "\n";
}

void Bank::displayCustomerDetails(int customerId) const
{
    const Customer* cust = nullptr;
    for (const auto& c : customers)
    {
        if (c.getCustomerId() == customerId)
        {
            cust = &c;
            break;
        }
    }

    if (!cust)
    {
        throw BankingException("Customer not found with ID: CUST-" + std::to_string(customerId));
    }

    cust->display();

    auto userAccounts = getAccountsByCustomerId(customerId);
    std::cout << "\n" << UI::Color::BOLD_CYAN << "Associated Accounts for CUST-" << customerId << ":\n" << UI::Color::RESET;

    if (userAccounts.empty())
    {
        UI::printInfo("No active bank accounts linked to this customer profile.");
        return;
    }

    displayAccountsByCustomer(customerId);
}

void Bank::displayAccountsByCustomer(int customerId) const
{
    auto userAccounts = getAccountsByCustomerId(customerId);
    if (userAccounts.empty())
    {
        UI::printWarning("No bank accounts found for Customer CUST-" + std::to_string(customerId));
        return;
    }

    std::cout << UI::Color::CYAN;
    std::cout << "+-------+-----------------+-----------------+-----------+\n";
    std::cout << "| ACC # | ACCOUNT TYPE    | CURRENT BALANCE | STATUS    |\n";
    std::cout << "+-------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;

    double custTotal = 0.0;
    for (const Account* a : userAccounts)
    {
        std::string statusCol = a->isActive() ? UI::Color::GREEN : UI::Color::RED;

        std::cout << UI::Color::CYAN << "| " << UI::Color::BOLD_WHITE 
                  << std::left << std::setw(5) << a->getAccountNumber()
                  << UI::Color::CYAN << " | " << UI::Color::YELLOW 
                  << std::left << std::setw(15) << a->getAccountType()
                  << UI::Color::CYAN << " | " << (a->getBalance() >= 0 ? UI::Color::BOLD_GREEN : UI::Color::BOLD_RED)
                  << std::right << std::setw(15) << UI::formatCurrency(a->getBalance())
                  << UI::Color::CYAN << " | " << statusCol 
                  << std::left << std::setw(9) << a->getStatus()
                  << UI::Color::CYAN << " |\n" << UI::Color::RESET;

        if (a->isActive()) custTotal += a->getBalance();
    }

    std::cout << UI::Color::CYAN;
    std::cout << "+-------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;
    std::cout << UI::Color::BOLD_WHITE << " Total Customer Deposits: " 
              << UI::Color::BOLD_GREEN << UI::formatCurrency(custTotal) 
              << UI::Color::RESET << "\n";
}

// ============================================================================
// ACCOUNT MANAGEMENT
// ============================================================================

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

// ============================================================================
// FINANCIAL TRANSACTIONS & ATOMIC TRANSFERS
// ============================================================================

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
        throw BankingException("Transfer failed: Source and destination account numbers cannot be identical.");
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
    // 1. Validate withdrawal from sender first (simulated pre-check)
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

// ============================================================================
// SECURITY MONITORING
// ============================================================================

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

// ============================================================================
// REPORTING, STATEMENTS & AUDIT
// ============================================================================

void Bank::displayAllAccounts() const
{
    if (accounts.empty())
    {
        UI::printWarning("No bank accounts registered in system. Please create an account first.");
        return;
    }

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------+-----------+--------------------------------+-----------------+-----------------+-----------+\n";
    std::cout << "| ACC # | CUST ID   | ACCOUNT HOLDER                 | ACCOUNT TYPE    | CURRENT BALANCE | STATUS    |\n";
    std::cout << "+-------+-----------+--------------------------------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;

    double totalDeposits = 0.0;

    for (const Account* account : accounts)
    {
        std::string statusColor = account->isActive() ? UI::Color::GREEN : UI::Color::RED;

        std::cout << UI::Color::CYAN << "| " << UI::Color::BOLD_WHITE 
                  << std::left << std::setw(5) << account->getAccountNumber()
                  << UI::Color::CYAN << " | " << UI::Color::BOLD_YELLOW
                  << std::left << std::setw(9) << ("CUST-" + std::to_string(account->getCustomerId()))
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
    std::cout << "+-------+-----------+--------------------------------+-----------------+-----------------+-----------+\n" << UI::Color::RESET;

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

// ============================================================================
// DATA PERSISTENCE
// ============================================================================

void Bank::saveData() const
{
    database.saveCustomers(customers);
    database.saveAccounts(accounts);
    database.saveTransactions(transactions);
    database.saveAlerts(fraudDetector.getAlerts());
}

void Bank::loadData()
{
    // Clean existing accounts
    for (Account* a : accounts) delete a;
    accounts.clear();
    customers.clear();

    customers = database.loadCustomers();
    accounts = database.loadAccounts();
    transactions = database.loadTransactions();
    auto alerts = database.loadAlerts();
    for (const auto& a : alerts)
    {
        fraudDetector.logAlert(a);
    }
}

// ============================================================================
// METRICS & COLLECTIONS
// ============================================================================

size_t Bank::getAccountCount() const
{
    return accounts.size();
}

size_t Bank::getCustomerCount() const
{
    return customers.size();
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

const std::vector<Customer>& Bank::getCustomers() const
{
    return customers;
}

const std::vector<Account*>& Bank::getAccounts() const
{
    return accounts;
}

const std::vector<Transaction>& Bank::getTransactions() const
{
    return transactions;
}
