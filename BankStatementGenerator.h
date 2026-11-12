#ifndef BANKSTATEMENTGENERATOR_H
#define BANKSTATEMENTGENERATOR_H

#include <string>
#include <vector>
#include "Account.h"
#include "Transaction.h"

/**
 * ============================================================================
 * OOP CONCEPT: FRIEND CLASS & AUDIT STATEMENT GENERATION
 * BankStatementGenerator is declared as a friend class of Account.
 * Has privileged access to internal Account members (e.g. balance, openingDate)
 * without exposing internal data members publicly.
 * ============================================================================
 */
class BankStatementGenerator
{
public:
    static void generateConsoleStatement(const Account& acc, const std::vector<Transaction>& transactions);
    static std::string exportFileStatement(const Account& acc, const std::vector<Transaction>& transactions, const std::string& outputFolder = "statements");
};

#endif // BANKSTATEMENTGENERATOR_H
