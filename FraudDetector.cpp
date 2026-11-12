#include "FraudDetector.h"
#include <iostream>
#include <algorithm>

int FraudDetector::nextAlertId = 5001;

FraudDetector::FraudDetector()
    : config()
{
}

double FraudDetector::getDailyDebitTotal(int accountNumber, const std::string& datePrefix, 
                                        const std::vector<Transaction>& history) const
{
    double total = 0.0;
    for (const auto& tx : history)
    {
        if (tx.getSenderAccount() == accountNumber && 
            (tx.getType() == "WITHDRAWAL" || tx.getType() == "TRANSFER") &&
            tx.getStatus() != "REJECTED")
        {
            if (tx.getTimestamp().find(datePrefix) == 0)
            {
                total += tx.getAmount();
            }
        }
    }
    return total;
}

int FraudDetector::getRecentTransactionCount(int accountNumber, const std::vector<Transaction>& history) const
{
    // Checks how many transactions occurred for this account in the last entries
    int count = 0;
    // Inspect up to the last 10 transactions in reverse
    for (auto it = history.rbegin(); it != history.rend() && count < 10; ++it)
    {
        if (it->getSenderAccount() == accountNumber || it->getReceiverAccount() == accountNumber)
        {
            count++;
        }
    }
    return count;
}

SecurityAlert FraudDetector::evaluateTransaction(const Transaction& tx, const std::vector<Transaction>& history)
{
    std::string risk = "LOW";
    std::string reason = "Normal transaction verified under standard security rules.";
    std::string status = "APPROVED";

    double amt = tx.getAmount();
    int acc = tx.getSenderAccount();
    std::string datePrefix = tx.getTimestamp().substr(0, 10); // "YYYY-MM-DD"

    // RULE 1: Large Transaction Alert
    if (amt >= (config.largeTransactionThreshold * 2.0))
    {
        risk = "HIGH";
        reason = "Critical large transaction amount (Rs. " + std::to_string(amt) + 
                 ") exceeds high-risk threshold (Rs. " + std::to_string(config.largeTransactionThreshold * 2.0) + ").";
        status = "PENDING_REVIEW";
    }
    else if (amt >= config.largeTransactionThreshold)
    {
        risk = "MEDIUM";
        reason = "Large transaction amount (Rs. " + std::to_string(amt) + 
                 ") meets advisory review threshold (Rs. " + std::to_string(config.largeTransactionThreshold) + ").";
        status = "PENDING_REVIEW";
    }

    // RULE 2: Daily Cumulative Debit Limit Rule (applies to withdrawals and outgoing transfers)
    if (tx.getType() == "WITHDRAWAL" || tx.getType() == "TRANSFER")
    {
        double pastDaily = getDailyDebitTotal(acc, datePrefix, history);
        if ((pastDaily + amt) > config.dailyDebitLimit)
        {
            risk = "HIGH";
            reason = "Daily debit limit of Rs. " + std::to_string(config.dailyDebitLimit) + 
                     " exceeded. Cumulative daily debit reached Rs. " + std::to_string(pastDaily + amt) + ".";
            status = "PENDING_REVIEW";
        }
    }

    // RULE 3: Rapid Transaction Velocity Alert
    int recentCount = getRecentTransactionCount(acc, history);
    if (recentCount >= config.rapidMaxCount)
    {
        if (risk == "LOW")
        {
            risk = "MEDIUM";
            reason = "Rapid transaction velocity: " + std::to_string(recentCount) + 
                     " consecutive transactions detected for account #" + std::to_string(acc) + ".";
            status = "PENDING_REVIEW";
        }
    }

    int alertId = (risk != "LOW") ? nextAlertId++ : 0;
    return SecurityAlert(alertId, tx.getTransactionId(), acc, amt, risk, reason, tx.getTimestamp(), status);
}

void FraudDetector::logAlert(const SecurityAlert& alert)
{
    if (alert.riskLevel != "LOW")
    {
        alerts.push_back(alert);
    }
}

const std::vector<SecurityAlert>& FraudDetector::getAlerts() const
{
    return alerts;
}

void FraudDetector::reviewAlert(int alertId, const std::string& newStatus)
{
    for (auto& a : alerts)
    {
        if (a.alertId == alertId)
        {
            a.reviewStatus = newStatus;
            break;
        }
    }
}

size_t FraudDetector::getTotalAlertsCount() const
{
    return alerts.size();
}

size_t FraudDetector::getHighRiskCount() const
{
    size_t count = 0;
    for (const auto& a : alerts)
    {
        if (a.riskLevel == "HIGH") count++;
    }
    return count;
}

size_t FraudDetector::getMediumRiskCount() const
{
    size_t count = 0;
    for (const auto& a : alerts)
    {
        if (a.riskLevel == "MEDIUM") count++;
    }
    return count;
}

size_t FraudDetector::getPendingReviewCount() const
{
    size_t count = 0;
    for (const auto& a : alerts)
    {
        if (a.reviewStatus == "PENDING_REVIEW") count++;
    }
    return count;
}

SecurityRuleConfig FraudDetector::getConfig() const
{
    return config;
}

void FraudDetector::setLargeTransactionThreshold(double threshold)
{
    config.largeTransactionThreshold = threshold;
}

void FraudDetector::setDailyDebitLimit(double limit)
{
    config.dailyDebitLimit = limit;
}

void FraudDetector::setRapidVelocityRules(int windowMinutes, int maxCount)
{
    config.rapidWindowMinutes = windowMinutes;
    config.rapidMaxCount = maxCount;
}

int FraudDetector::getNextAlertId()
{
    return nextAlertId;
}

void FraudDetector::setNextAlertId(int id)
{
    nextAlertId = id;
}
