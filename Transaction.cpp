#include "Transaction.h"
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

int Transaction::nextTransactionSequence = 10001;

Transaction::Transaction()
    : transactionId("TXN-0"), senderAccount(0), receiverAccount(-1), 
      type("UNKNOWN"), amount(0.0), timestamp(""), status("REJECTED"), 
      riskLevel("LOW"), flagReason("None")
{
}

Transaction::Transaction(int sender, int receiver, const std::string& txType, 
                         double amt, const std::string& timeStr, 
                         const std::string& stat, const std::string& risk, 
                         const std::string& reason)
    : senderAccount(sender), receiverAccount(receiver), type(txType), 
      amount(amt), timestamp(timeStr), status(stat), riskLevel(risk), 
      flagReason(reason)
{
    transactionId = "TXN-" + std::to_string(nextTransactionSequence++);
}

Transaction::Transaction(const std::string& id, int sender, int receiver, 
                         const std::string& txType, double amt, 
                         const std::string& timeStr, const std::string& stat, 
                         const std::string& risk, const std::string& reason)
    : transactionId(id), senderAccount(sender), receiverAccount(receiver), 
      type(txType), amount(amt), timestamp(timeStr), status(stat), 
      riskLevel(risk), flagReason(reason)
{
}

std::string Transaction::getTransactionId() const
{
    return transactionId;
}

int Transaction::getSenderAccount() const
{
    return senderAccount;
}

int Transaction::getReceiverAccount() const
{
    return receiverAccount;
}

std::string Transaction::getType() const
{
    return type;
}

double Transaction::getAmount() const
{
    return amount;
}

std::string Transaction::getTimestamp() const
{
    return timestamp;
}

std::string Transaction::getStatus() const
{
    return status;
}

std::string Transaction::getRiskLevel() const
{
    return riskLevel;
}

std::string Transaction::getFlagReason() const
{
    return flagReason;
}

void Transaction::setStatus(const std::string& stat)
{
    status = stat;
}

void Transaction::setRiskLevel(const std::string& risk, const std::string& reason)
{
    riskLevel = risk;
    flagReason = reason;
}

std::ostream& operator<<(std::ostream& os, const Transaction& tx)
{
    os << "[" << tx.transactionId << "] "
       << tx.timestamp << " | "
       << std::setw(10) << tx.type << " | "
       << "Acc #" << tx.senderAccount;
    if (tx.receiverAccount != -1)
    {
        os << " -> Acc #" << tx.receiverAccount;
    }
    os << " | Rs. " << std::fixed << std::setprecision(2) << tx.amount
       << " | Status: " << tx.status
       << " | Risk: " << tx.riskLevel;
    return os;
}

int Transaction::getNextTransactionSequence()
{
    return nextTransactionSequence;
}

void Transaction::setNextTransactionSequence(int seq)
{
    nextTransactionSequence = seq;
}

std::string Transaction::generateCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* parts = std::localtime(&now_c);
    char buf[64];
    if (parts)
    {
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", parts);
    }
    else
    {
        return "2026-10-12 12:00:00";
    }
    return std::string(buf);
}
