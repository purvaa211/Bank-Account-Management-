#include "BankDatabase.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "FixedDepositAccount.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#define MKDIR(d) mkdir(d, 0755)
#endif

BankDatabase::BankDatabase(const std::string& dir)
    : dataDirectory(dir)
{
    ensureDirectoryExists();
}

void BankDatabase::ensureDirectoryExists() const
{
    MKDIR(dataDirectory.c_str());
}

static std::vector<std::string> splitCsvLine(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i)
    {
        char ch = line[i];
        if (ch == '\"')
        {
            inQuotes = !inQuotes;
        }
        else if (ch == ',' && !inQuotes)
        {
            tokens.push_back(token);
            token.clear();
        }
        else
        {
            token += ch;
        }
    }
    tokens.push_back(token);
    return tokens;
}

static std::string cleanToken(std::string t)
{
    while (!t.empty() && (t.front() == ' ' || t.front() == '\t' || t.front() == '\"'))
    {
        t.erase(0, 1);
    }
    while (!t.empty() && (t.back() == ' ' || t.back() == '\t' || t.back() == '\r' || t.back() == '\n' || t.back() == '\"'))
    {
        t.pop_back();
    }
    return t;
}

bool BankDatabase::saveCustomers(const std::vector<Customer>& customers) const
{
    ensureDirectoryExists();
    std::ofstream out(dataDirectory + "/customers.csv");
    if (!out) return false;

    out << "CustomerId,FullName,Email,Phone\n";
    for (const auto& c : customers)
    {
        out << c.getCustomerId() << ","
            << "\"" << c.getFullName() << "\","
            << "\"" << c.getEmail() << "\","
            << "\"" << c.getPhone() << "\"\n";
    }
    out.close();
    return true;
}

std::vector<Customer> BankDatabase::loadCustomers() const
{
    std::vector<Customer> list;
    std::ifstream in(dataDirectory + "/customers.csv");
    if (!in) return list;

    std::string line;
    if (!std::getline(in, line)) return list; // Skip header

    int maxCustId = 200;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 2) continue;

        try
        {
            int id = std::stoi(cleanToken(tokens[0]));
            std::string name = cleanToken(tokens[1]);
            std::string email = tokens.size() > 2 ? cleanToken(tokens[2]) : "";
            std::string phone = tokens.size() > 3 ? cleanToken(tokens[3]) : "";

            if (id > maxCustId) maxCustId = id;
            list.emplace_back(id, name, email, phone);
        }
        catch (...)
        {
        }
    }
    in.close();

    Customer::setNextCustomerId(maxCustId + 1);
    return list;
}

bool BankDatabase::saveAccounts(const std::vector<Account*>& accounts) const
{
    ensureDirectoryExists();
    std::ofstream out(dataDirectory + "/accounts.csv");
    if (!out) return false;

    // Header
    out << "AccountNumber,CustomerId,HolderName,AccountType,Balance,Status,OpeningDate,Extra1,Extra2,Extra3\n";

    for (const Account* acc : accounts)
    {
        out << acc->getAccountNumber() << ","
            << acc->getCustomerId() << ","
            << "\"" << acc->getName() << "\","
            << acc->getAccountType() << ","
            << acc->getBalance() << ","
            << acc->getStatus() << ","
            << acc->getOpeningDate() << ",";

        if (const auto* sa = dynamic_cast<const SavingsAccount*>(acc))
        {
            out << sa->getMinimumBalance() << ",0,0\n";
        }
        else if (const auto* ca = dynamic_cast<const CurrentAccount*>(acc))
        {
            out << ca->getOverdraftLimit() << ",0,0\n";
        }
        else if (const auto* fd = dynamic_cast<const FixedDepositAccount*>(acc))
        {
            out << fd->getTermMonths() << ","
                << fd->getInterestRate() << ","
                << fd->getMaturityDate() << "\n";
        }
        else
        {
            out << "0,0,0\n";
        }
    }
    out.close();
    return true;
}

std::vector<Account*> BankDatabase::loadAccounts() const
{
    std::vector<Account*> list;
    std::ifstream in(dataDirectory + "/accounts.csv");
    if (!in) return list;

    std::string line;
    if (!std::getline(in, line)) return list; // Skip header

    int maxAccNum = 1000;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 6) continue;

        try
        {
            // Support both 10-column (with CustomerId) and 9-column legacy schemas
            bool hasCustCol = (tokens.size() >= 10);
            int accNum = std::stoi(cleanToken(tokens[0]));
            int custId = hasCustCol ? std::stoi(cleanToken(tokens[1])) : 201;
            std::string name = cleanToken(tokens[hasCustCol ? 2 : 1]);
            std::string type = cleanToken(tokens[hasCustCol ? 3 : 2]);
            double bal = std::stod(cleanToken(tokens[hasCustCol ? 4 : 3]));
            std::string stat = cleanToken(tokens[hasCustCol ? 5 : 4]);
            std::string date = cleanToken(tokens[hasCustCol ? 6 : 5]);

            int extraIdx = hasCustCol ? 7 : 6;

            if (accNum > maxAccNum) maxAccNum = accNum;

            if (type == "Savings Account")
            {
                double minBal = tokens.size() > static_cast<size_t>(extraIdx) ? std::stod(cleanToken(tokens[extraIdx])) : 1000.0;
                list.push_back(new SavingsAccount(accNum, custId, name, bal, stat, date, minBal));
            }
            else if (type == "Current Account")
            {
                double overdraft = tokens.size() > static_cast<size_t>(extraIdx) ? std::stod(cleanToken(tokens[extraIdx])) : 25000.0;
                list.push_back(new CurrentAccount(accNum, custId, name, bal, stat, date, overdraft));
            }
            else if (type == "Fixed Deposit")
            {
                int term = tokens.size() > static_cast<size_t>(extraIdx) ? std::stoi(cleanToken(tokens[extraIdx])) : 12;
                double rate = tokens.size() > static_cast<size_t>(extraIdx + 1) ? std::stod(cleanToken(tokens[extraIdx + 1])) : 0.07;
                std::string matDate = tokens.size() > static_cast<size_t>(extraIdx + 2) ? cleanToken(tokens[extraIdx + 2]) : "";
                double t = term / 12.0;
                double matAmt = bal * std::pow(1.0 + (rate / 4.0), 4.0 * t);
                list.push_back(new FixedDepositAccount(accNum, custId, name, bal, stat, date, term, rate, matAmt, matDate));
            }
        }
        catch (...)
        {
            // Gracefully ignore corrupted record
        }
    }
    in.close();

    Account::setNextAccountNumber(maxAccNum + 1);
    return list;
}

bool BankDatabase::saveTransactions(const std::vector<Transaction>& transactions) const
{
    ensureDirectoryExists();
    std::ofstream out(dataDirectory + "/transactions.csv");
    if (!out) return false;

    out << "TransactionId,SenderAccount,ReceiverAccount,Type,Amount,Timestamp,Status,RiskLevel,FlagReason\n";

    for (const auto& tx : transactions)
    {
        out << tx.getTransactionId() << ","
            << tx.getSenderAccount() << ","
            << tx.getReceiverAccount() << ","
            << tx.getType() << ","
            << tx.getAmount() << ","
            << tx.getTimestamp() << ","
            << tx.getStatus() << ","
            << tx.getRiskLevel() << ","
            << "\"" << tx.getFlagReason() << "\"\n";
    }
    out.close();
    return true;
}

std::vector<Transaction> BankDatabase::loadTransactions() const
{
    std::vector<Transaction> list;
    std::ifstream in(dataDirectory + "/transactions.csv");
    if (!in) return list;

    std::string line;
    if (!std::getline(in, line)) return list;

    int maxSeq = 10000;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 8) continue;

        try
        {
            std::string id = cleanToken(tokens[0]);
            int sender = std::stoi(cleanToken(tokens[1]));
            int receiver = std::stoi(cleanToken(tokens[2]));
            std::string type = cleanToken(tokens[3]);
            double amt = std::stod(cleanToken(tokens[4]));
            std::string timeStr = cleanToken(tokens[5]);
            std::string stat = cleanToken(tokens[6]);
            std::string risk = cleanToken(tokens[7]);
            std::string reason = tokens.size() > 8 ? cleanToken(tokens[8]) : "Normal";

            if (id.find("TXN-") == 0)
            {
                int seq = std::stoi(id.substr(4));
                if (seq > maxSeq) maxSeq = seq;
            }

            list.emplace_back(id, sender, receiver, type, amt, timeStr, stat, risk, reason);
        }
        catch (...)
        {
        }
    }
    in.close();

    Transaction::setNextTransactionSequence(maxSeq + 1);
    return list;
}

bool BankDatabase::saveAlerts(const std::vector<SecurityAlert>& alerts) const
{
    ensureDirectoryExists();
    std::ofstream out(dataDirectory + "/security_alerts.csv");
    if (!out) return false;

    out << "AlertId,TransactionId,AccountNumber,Amount,RiskLevel,Reason,Timestamp,ReviewStatus\n";

    for (const auto& a : alerts)
    {
        out << a.alertId << ","
            << a.transactionId << ","
            << a.accountNumber << ","
            << a.amount << ","
            << a.riskLevel << ","
            << "\"" << a.reason << "\","
            << a.timestamp << ","
            << a.reviewStatus << "\n";
    }
    out.close();
    return true;
}

std::vector<SecurityAlert> BankDatabase::loadAlerts() const
{
    std::vector<SecurityAlert> list;
    std::ifstream in(dataDirectory + "/security_alerts.csv");
    if (!in) return list;

    std::string line;
    if (!std::getline(in, line)) return list;

    int maxAlertId = 5000;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 7) continue;

        try
        {
            int alertId = std::stoi(cleanToken(tokens[0]));
            std::string txId = cleanToken(tokens[1]);
            int acc = std::stoi(cleanToken(tokens[2]));
            double amt = std::stod(cleanToken(tokens[3]));
            std::string risk = cleanToken(tokens[4]);
            std::string reason = cleanToken(tokens[5]);
            std::string timeStr = cleanToken(tokens[6]);
            std::string revStatus = tokens.size() > 7 ? cleanToken(tokens[7]) : "PENDING_REVIEW";

            if (alertId > maxAlertId) maxAlertId = alertId;

            list.emplace_back(alertId, txId, acc, amt, risk, reason, timeStr, revStatus);
        }
        catch (...)
        {
        }
    }
    in.close();

    FraudDetector::setNextAlertId(maxAlertId + 1);
    return list;
}
