#include "FixedDepositAccount.h"
#include "Exceptions.h"
#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <ctime>

static std::string calculateFutureDate(int monthsToAdd)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* p = std::localtime(&now_c);
    if (!p) return "2027-10-12";

    std::tm parts = *p;
    parts.tm_mon += monthsToAdd;
    std::mktime(&parts);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &parts);
    return std::string(buf);
}

void FixedDepositAccount::computeMaturity()
{
    // Compound interest: A = P * (1 + r / 4)^(4 * t), compounding quarterly
    double t = static_cast<double>(termMonths) / 12.0;
    maturityAmount = getBalance() * std::pow(1.0 + (interestRate / 4.0), 4.0 * t);
    maturityDate = calculateFutureDate(termMonths);
}

FixedDepositAccount::FixedDepositAccount(const std::string& n, double principal, int term, 
                                         const std::string& date, double rate)
    : Account(n, principal, date), termMonths(term), interestRate(rate), 
      maturityAmount(0.0), maturityDate(""), isMatured(false)
{
    computeMaturity();
}

FixedDepositAccount::FixedDepositAccount(int accNum, const std::string& n, double principal, 
                                         const std::string& stat, const std::string& date, 
                                         int term, double rate, double matAmt, const std::string& matDate)
    : Account(accNum, n, principal, stat, date), termMonths(term), 
      interestRate(rate), maturityAmount(matAmt), maturityDate(matDate), isMatured(false)
{
}

bool FixedDepositAccount::canWithdraw(double amount, std::string& reason) const
{
    if (amount <= 0)
    {
        reason = "Withdrawal amount must be greater than zero.";
        return false;
    }

    if (amount > getBalance())
    {
        reason = "Withdrawal exceeds fixed deposit principal of " + UI::formatCurrency(getBalance());
        return false;
    }

    return true;
}

void FixedDepositAccount::withdraw(double amount)
{
    if (!isActive())
    {
        throw AccountClosedException(getAccountNumber());
    }

    if (amount <= 0)
    {
        throw InvalidAmountException("Withdrawal amount must be greater than zero.");
    }

    if (amount > getBalance())
    {
        throw InsufficientBalanceException(getBalance(), amount);
    }

    UI::printWarning("PREMATURE LIQUIDATION NOTICE: Withdrawing funds prior to maturity (" + 
                     maturityDate + ") will forgo accrued interest.");

    setBalance(getBalance() - amount);
    computeMaturity();
}

void FixedDepositAccount::calculateInterest()
{
    double interestEarned = maturityAmount - getBalance();

    std::cout << "\n" << UI::Color::BOLD_CYAN;
    std::cout << "+-------------------------------------------------------------+\n";
    std::cout << "|                 FIXED DEPOSIT MATURITY REPORT               |\n";
    std::cout << "+------------------------------+------------------------------+\n" << UI::Color::RESET;

    auto printField = [](const std::string& label, const std::string& val, const std::string& color = UI::Color::BOLD_WHITE) {
        std::cout << UI::Color::CYAN << "| " << UI::Color::WHITE << std::left << std::setw(28) << label 
                  << UI::Color::CYAN << "| " << color << std::left << std::setw(28) << val 
                  << UI::Color::CYAN << "|\n" << UI::Color::RESET;
    };

    printField("Account Number", std::to_string(getAccountNumber()));
    printField("Account Holder", getName());
    printField("Investment Principal", UI::formatCurrency(getBalance()));
    printField("Annual Interest Rate", std::to_string(interestRate * 100.0).substr(0, 4) + "% p.a.");
    printField("Investment Duration", std::to_string(termMonths) + " Months");
    printField("Guaranteed Maturity Date", maturityDate, UI::Color::BOLD_YELLOW);
    printField("Total Projected Interest", UI::formatCurrency(interestEarned), UI::Color::BOLD_GREEN);
    printField("Maturity Value", UI::formatCurrency(maturityAmount), UI::Color::BOLD_GREEN);

    std::cout << UI::Color::BOLD_CYAN;
    std::cout << "+------------------------------+------------------------------+\n";
    std::cout << UI::Color::RESET;
}

void FixedDepositAccount::display() const
{
    Account::display();
    std::cout << UI::Color::CYAN;
    std::cout << "| " << UI::Color::YELLOW << "Fixed Term: " << termMonths << " Months"
              << UI::Color::CYAN << " | " << UI::Color::BOLD_GREEN << "Maturity Date: " << maturityDate 
              << UI::Color::CYAN << " | Value: " << UI::formatCurrency(maturityAmount) << " |\n";
    std::cout << "+-------------------------------------------------------------+\n" << UI::Color::RESET;
}

std::string FixedDepositAccount::getAccountType() const
{
    return "Fixed Deposit";
}

double FixedDepositAccount::getInterestRate() const
{
    return interestRate;
}

int FixedDepositAccount::getTermMonths() const
{
    return termMonths;
}

double FixedDepositAccount::getMaturityAmount() const
{
    return maturityAmount;
}

std::string FixedDepositAccount::getMaturityDate() const
{
    return maturityDate;
}

bool FixedDepositAccount::getIsMatured() const
{
    return isMatured;
}
