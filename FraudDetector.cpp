#include "FraudDetector.h"
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <sstream>

int FraudDetector::nextAlertId = 5001;

FraudDetector::FraudDetector()
    : config()
{
}

std::time_t FraudDetector::parseTimestampToEpoch(const std::string& ts)
{
    if (ts.length() < 19) return 0;

    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
    if (std::sscanf(ts.c_str(), "%d-%d-%d %d:%d:%d", 
                    &year, &month, &day, &hour, &min, &sec) == 6)
    {
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = min;
        tm.tm_sec = sec;
        tm.tm_isdst = -1;
        return std::mktime(&tm);
    }
    return 0;
}

double FraudDetector::getDailyDebitTotal(int accountNumber, const std::string& datePrefix, 
                                        const std::vector<Transaction>& history) const
{
    double total = 0.0;
    for (const auto& tx : history)
    {
        // Only count outgoing debits (withdrawals or transfers initiated by this sender)
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

int FraudDetector::getRecentTransactionCountWithinWindow(int accountNumber, const std::string& currentTimestamp, 
                                                         const std::vector<Transaction>& history) const
{
    std::time_t currentEpoch = parseTimestampToEpoch(currentTimestamp);
    if (currentEpoch == 0) return 0;

    double windowSeconds = static_cast<double>(config.rapidWindowMinutes * 60);
    int count = 0;

    // Iterate backwards through recent transactions
    for (auto it = history.rbegin(); it != history.rend(); ++it)
    {
        if (it->getSenderAccount() == accountNumber || it->getReceiverAccount() == accountNumber)
        {
            std::time_t pastEpoch = parseTimestampToEpoch(it->getTimestamp());
            if (pastEpoch > 0)
            {
                double delta = std::difftime(currentEpoch, pastEpoch);
                // Within the past window (e.g. within 2 minutes)
                if (delta >= 0.0 && delta <= windowSeconds)
                {
                    count++;
                }
                else if (delta > windowSeconds)
                {
                    // Transactions are chronological; once outside window, we can stop
                    break;
                }
            }
        }
    }
    return count;
}

SecurityAlert FraudDetector::evaluateTransaction(const Transaction& tx, const std::vector<Transaction>& history)
{
    std::string risk = "LOW";
    std::string reason = "Normal transaction verified under standard security guidelines.";
    std::string status = "APPROVED";

    double amt = tx.getAmount();
    int acc = tx.getSenderAccount();
    std::string datePrefix = tx.getTimestamp().length() >= 10 ? tx.getTimestamp().substr(0, 10) : "";

    // RULE 1: Large Transaction Threshold Rule
    if (amt >= config.criticalTransactionThreshold)
    {
        risk = "HIGH";
        reason = "Critical large transaction amount (Rs. " + std::to_string(amt) + 
                 ") meets or exceeds critical threshold (Rs. " + std::to_string(config.criticalTransactionThreshold) + ").";
        status = "PENDING_REVIEW";
    }
    else if (amt >= config.advisoryTransactionThreshold)
    {
        risk = "MEDIUM";
        reason = "Large transaction amount (Rs. " + std::to_string(amt) + 
                 ") meets advisory review threshold (Rs. " + std::to_string(config.advisoryTransactionThreshold) + ").";
        status = "PENDING_REVIEW";
    }

    // RULE 2: Daily Cumulative Debit Limit Rule (applies only to outgoing withdrawals and transfers)
    if (tx.getType() == "WITHDRAWAL" || tx.getType() == "TRANSFER")
    {
        double pastDaily = getDailyDebitTotal(acc, datePrefix, history);
        if ((pastDaily + amt) > config.dailyDebitLimit)
        {
            risk = "HIGH";
            reason = "Daily cumulative debit limit of Rs. " + std::to_string(config.dailyDebitLimit) + 
                     " exceeded. Cumulative daily debit reached Rs. " + std::to_string(pastDaily + amt) + ".";
            status = "PENDING_REVIEW";
        }
    }

    // RULE 3: Rapid Transaction Velocity Alert (using timestamp delta comparison)
    int recentCount = getRecentTransactionCountWithinWindow(acc, tx.getTimestamp(), history);
    if (recentCount >= config.rapidMaxCount)
    {
        if (risk == "LOW")
        {
            risk = "MEDIUM";
            reason = "Rapid transaction velocity: " + std::to_string(recentCount) + 
                     " transactions recorded within " + std::to_string(config.rapidWindowMinutes) + 
                     " minutes for account #" + std::to_string(acc) + ".";
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

void FraudDetector::setAdvisoryThreshold(double threshold)
{
    config.advisoryTransactionThreshold = threshold;
}

void FraudDetector::setCriticalThreshold(double threshold)
{
    config.criticalTransactionThreshold = threshold;
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
