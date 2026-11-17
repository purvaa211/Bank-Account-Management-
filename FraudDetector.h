#ifndef FRAUDDETECTOR_H
#define FRAUDDETECTOR_H

#include <string>
#include <vector>
#include <ctime>
#include "Transaction.h"

/**
 * ============================================================================
 * OOP CONCEPT: MODULAR RULE-BASED SECURITY MONITORING & AGGREGATION
 * Dedicated Transaction Security Engine.
 * Evaluates live transactions against established banking risk rules:
 *   1. Large Transaction Threshold Rule (Advisory vs Critical)
 *   2. Daily Cumulative Debit Limit Rule (Withdrawals and outgoing transfers)
 *   3. Rapid Repeated Transaction Velocity Rule (Timestamp delta comparison)
 * ============================================================================
 */

struct SecurityRuleConfig
{
    double advisoryTransactionThreshold; // e.g. Rs. 50,000.00 (MEDIUM)
    double criticalTransactionThreshold; // e.g. Rs. 100,000.00 (HIGH)
    double dailyDebitLimit;               // e.g. Rs. 100,000.00 (HIGH)
    int rapidWindowMinutes;               // e.g. 2 minutes
    int rapidMaxCount;                    // e.g. 3 transactions within window

    SecurityRuleConfig()
        : advisoryTransactionThreshold(50000.0),
          criticalTransactionThreshold(100000.0),
          dailyDebitLimit(100000.0),
          rapidWindowMinutes(2),
          rapidMaxCount(3)
    {
    }
};

struct SecurityAlert
{
    int alertId;
    std::string transactionId;
    int accountNumber;
    double amount;
    std::string riskLevel; // LOW, MEDIUM, HIGH
    std::string reason;
    std::string timestamp;
    std::string reviewStatus; // PENDING_REVIEW, REVIEWED, APPROVED

    SecurityAlert()
        : alertId(0), transactionId(""), accountNumber(0), 
          amount(0.0), riskLevel("LOW"), reason(""), 
          timestamp(""), reviewStatus("APPROVED")
    {
    }

    SecurityAlert(int id, const std::string& txId, int acc, double amt, 
                  const std::string& risk, const std::string& reas, 
                  const std::string& timeStr, const std::string& status = "PENDING_REVIEW")
        : alertId(id), transactionId(txId), accountNumber(acc), 
          amount(amt), riskLevel(risk), reason(reas), 
          timestamp(timeStr), reviewStatus(status)
    {
    }
};

class FraudDetector
{
private:
    SecurityRuleConfig config;
    std::vector<SecurityAlert> alerts;
    static int nextAlertId;

    // Helper to calculate total outgoing debit amount for an account within the current day
    double getDailyDebitTotal(int accountNumber, const std::string& datePrefix, 
                              const std::vector<Transaction>& history) const;

    // Helper to calculate recent transactions within rapid velocity window using timestamp math
    int getRecentTransactionCountWithinWindow(int accountNumber, const std::string& currentTimestamp, 
                                              const std::vector<Transaction>& history) const;

public:
    FraudDetector();

    // Timestamp parsing utility
    static std::time_t parseTimestampToEpoch(const std::string& timestampStr);

    // Core rule-based transaction evaluation
    SecurityAlert evaluateTransaction(const Transaction& tx, const std::vector<Transaction>& history);

    // Alert record management
    void logAlert(const SecurityAlert& alert);
    const std::vector<SecurityAlert>& getAlerts() const;
    void reviewAlert(int alertId, const std::string& newStatus);

    // Statistics
    size_t getTotalAlertsCount() const;
    size_t getHighRiskCount() const;
    size_t getMediumRiskCount() const;
    size_t getPendingReviewCount() const;

    // Configuration getters & setters
    SecurityRuleConfig getConfig() const;
    void setAdvisoryThreshold(double threshold);
    void setCriticalThreshold(double threshold);
    void setDailyDebitLimit(double limit);
    void setRapidVelocityRules(int windowMinutes, int maxCount);

    // Static counter management
    static int getNextAlertId();
    static void setNextAlertId(int id);
};

#endif // FRAUDDETECTOR_H
