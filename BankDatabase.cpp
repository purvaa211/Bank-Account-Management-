#include "BankDatabase.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "FixedDepositAccount.h"
#include <iostream>
#include <fstream>
#include <sstream>
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
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, ','))
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool BankDatabase::saveAccounts(const std::vector<Account*>& accounts) const
{
    ensureDirectoryExists();
    std::ofstream out(dataDirectory + "/accounts.csv");
    if (!out) return false;

    // Header
    out << "AccountNumber,HolderName,AccountType,Balance,Status,OpeningDate,Extra1,Extra2,Extra3\n";

    for (const Account* acc : accounts)
    {
        out << acc->getAccountNumber() << ","
            << acc->getName() << ","
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
    // Skip header line
    if (!std::getline(in, line)) return list;

    int maxAccNum = 1000;

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 6) continue;

        try
        {
            int accNum = std::stoi(tokens[0]);
            std::string name = tokens[1];
            std::string type = tokens[2];
            double bal = std::stod(tokens[3]);
            std::string stat = tokens[4];
            std::string date = tokens[5];

            if (accNum > maxAccNum) maxAccNum = accNum;

            if (type == "Savings Account")
            {
                double minBal = tokens.size() > 6 ? std::stod(tokens[6]) : 1000.0;
                list.push_back(new SavingsAccount(accNum, name, bal, stat, date, minBal));
            }
            else if (type == "Current Account")
            {
                double overdraft = tokens.size() > 6 ? std::stod(tokens[6]) : 25000.0;
                list.push_back(new CurrentAccount(accNum, name, bal, stat, date, overdraft));
            }
            else if (type == "Fixed Deposit")
            {
                int term = tokens.size() > 6 ? std::stoi(tokens[6]) : 12;
                double rate = tokens.size() > 7 ? std::stod(tokens[7]) : 0.07;
                std::string matDate = tokens.size() > 8 ? tokens[8] : "";
                double matAmt = bal * 1.07; // approximate or compute
                list.push_back(new FixedDepositAccount(accNum, name, bal, stat, date, term, rate, matAmt, matDate));
            }
        }
        catch (...)
        {
            // Gracefully ignore malformed record
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
            std::string id = tokens[0];
            int sender = std::stoi(tokens[1]);
            int receiver = std::stoi(tokens[2]);
            std::string type = tokens[3];
            double amt = std::stod(tokens[4]);
            std::string timeStr = tokens[5];
            std::string stat = tokens[6];
            std::string risk = tokens[7];
            std::string reason = tokens.size() > 8 ? tokens[8] : "Normal";

            // Remove surrounding quotes if present
            if (!reason.empty() && reason.front() == '"') reason.erase(0, 1);
            if (!reason.empty() && reason.back() == '"') reason.pop_back();

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
            int alertId = std::stoi(tokens[0]);
            std::string txId = tokens[1];
            int acc = std::stoi(tokens[2]);
            double amt = std::stod(tokens[3]);
            std::string risk = tokens[4];
            std::string reason = tokens[5];
            std::string timeStr = tokens[6];
            std::string revStatus = tokens.size() > 7 ? tokens[7] : "PENDING_REVIEW";

            if (!reason.empty() && reason.front() == '"') reason.erase(0, 1);
            if (!reason.empty() && reason.back() == '"') reason.pop_back();

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
