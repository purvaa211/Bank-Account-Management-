#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <iostream>

/**
 * ============================================================================
 * OOP CONCEPT: ENCAPSULATION & STATIC MEMBERS
 * Represents an immutable financial transaction in the bank.
 * Demonstrates:
 *   - Encapsulation of transaction attributes.
 *   - Static member counter to guarantee unique IDs.
 *   - Operator overloading for stream insertion.
 * ============================================================================
 */
class Transaction
{
private:
    std::string transactionId;
    int senderAccount;
    int receiverAccount; // -1 if not a transfer
    std::string type;    // DEPOSIT, WITHDRAWAL, TRANSFER
    double amount;
    std::string timestamp;
    std::string status;     // SUCCESS, FLAGGED, REJECTED
    std::string riskLevel;  // LOW, MEDIUM, HIGH
    std::string flagReason;

    static int nextTransactionSequence;

public:
    Transaction();
    Transaction(int sender, int receiver, const std::string& txType, 
                double amt, const std::string& timeStr, 
                const std::string& stat = "SUCCESS", 
                const std::string& risk = "LOW", 
                const std::string& reason = "Normal transaction");

    // Overloaded constructor for loading saved transactions from storage
    Transaction(const std::string& id, int sender, int receiver, 
                const std::string& txType, double amt, 
                const std::string& timeStr, const std::string& stat, 
                const std::string& risk, const std::string& reason);

    // Getters
    std::string getTransactionId() const;
    int getSenderAccount() const;
    int getReceiverAccount() const;
    std::string getType() const;
    double getAmount() const;
    std::string getTimestamp() const;
    std::string getStatus() const;
    std::string getRiskLevel() const;
    std::string getFlagReason() const;

    void setStatus(const std::string& stat);
    void setRiskLevel(const std::string& risk, const std::string& reason);

    // Stream operator overloading
    friend std::ostream& operator<<(std::ostream& os, const Transaction& tx);

    // Static counter management
    static int getNextTransactionSequence();
    static void setNextTransactionSequence(int seq);
    static std::string generateCurrentTimestamp();
};

#endif // TRANSACTION_H
