#include "UIHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

namespace UI
{
    namespace Color
    {
        const std::string RESET       = "\033[0m";
        const std::string BOLD        = "\033[1m";
        const std::string DIM         = "\033[2m";
        const std::string UNDERLINE   = "\033[4m";

        const std::string RED         = "\033[31m";
        const std::string GREEN       = "\033[32m";
        const std::string YELLOW      = "\033[33m";
        const std::string BLUE        = "\033[34m";
        const std::string MAGENTA     = "\033[35m";
        const std::string CYAN        = "\033[36m";
        const std::string WHITE       = "\033[37m";
        const std::string GRAY        = "\033[90m";

        const std::string BOLD_RED     = "\033[1;31m";
        const std::string BOLD_GREEN   = "\033[1;32m";
        const std::string BOLD_YELLOW  = "\033[1;33m";
        const std::string BOLD_MAGENTA = "\033[1;35m";
        const std::string BOLD_CYAN    = "\033[1;36m";
        const std::string BOLD_WHITE   = "\033[1;37m";
    }

    void initTerminal()
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE)
        {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode))
            {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    void clearScreen()
    {
        std::cout << "\033[2J\033[H";
    }

    void pauseScreen()
    {
        std::cout << "\n" << Color::GRAY << "Press Enter to continue..." << Color::RESET;
        if (std::cin.eof()) return;
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void printDivider(int width, char ch)
    {
        std::cout << Color::GRAY << std::string(width, ch) << Color::RESET << "\n";
    }

    void printDoubleDivider(int width)
    {
        std::cout << Color::CYAN << std::string(width, '=') << Color::RESET << "\n";
    }

    void printHeroBanner()
    {
        std::cout << Color::BOLD_CYAN;
        std::cout << "======================================================================\n";
        std::cout << "   SMARTBANK - SMART BANKING & TRANSACTION SECURITY SYSTEM            \n";
        std::cout << "               Object-Oriented Programming in C++                     \n";
        std::cout << "======================================================================\n";
        std::cout << Color::RESET;
    }

    void printSectionHeader(const std::string& title)
    {
        std::cout << "\n" << Color::BOLD_YELLOW << ">>> " << title << " <<<\n" << Color::RESET;
        printDivider(70, '-');
    }

    void printSuccess(const std::string& message)
    {
        std::cout << Color::BOLD_GREEN << "[SUCCESS] " << Color::GREEN 
                  << message << Color::RESET << "\n";
    }

    void printError(const std::string& message)
    {
        std::cout << Color::BOLD_RED << "[ERROR] " << Color::RED 
                  << message << Color::RESET << "\n";
    }

    void printInfo(const std::string& message)
    {
        std::cout << Color::CYAN << "[INFO] " << Color::RESET << message << "\n";
    }

    void printWarning(const std::string& message)
    {
        std::cout << Color::BOLD_YELLOW << "[WARNING] " << Color::YELLOW 
                  << message << Color::RESET << "\n";
    }

    std::string formatCurrency(double amount)
    {
        std::stringstream ss;
        ss << "Rs. " << std::fixed << std::setprecision(2) << amount;
        return ss.str();
    }

    void printReceipt(int accNum, const std::string& holder, const std::string& type, 
                      const std::string& txType, double prevBal, double amount, double newBal,
                      const std::string& txId, const std::string& risk)
    {
        std::cout << "\n" << Color::BOLD_CYAN;
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                 SMARTBANK AUDIT RECEIPT                     |\n";
        std::cout << "+------------------------------+------------------------------+\n" << Color::RESET;

        auto printRow = [](const std::string& label, const std::string& val, const std::string& valColor = Color::BOLD_WHITE) {
            std::cout << Color::CYAN << "| " << Color::WHITE << std::left << std::setw(28) << label 
                      << Color::CYAN << "| " << valColor << std::left << std::setw(28) << val 
                      << Color::CYAN << "|\n" << Color::RESET;
        };

        if (!txId.empty()) printRow("Transaction ID", txId, Color::BOLD_YELLOW);
        printRow("Account Number", std::to_string(accNum));
        printRow("Account Holder", holder);
        printRow("Account Type", type);
        printRow("Operation Type", txType, (txType == "DEPOSIT" ? Color::BOLD_GREEN : Color::BOLD_RED));
        printRow("Previous Balance", formatCurrency(prevBal));
        printRow("Transaction Amount", formatCurrency(amount), (txType == "DEPOSIT" ? Color::BOLD_GREEN : Color::BOLD_RED));
        printRow("Updated Balance", formatCurrency(newBal), Color::BOLD_GREEN);
        printRow("Security Risk Level", risk, (risk == "HIGH" ? Color::BOLD_RED : (risk == "MEDIUM" ? Color::BOLD_YELLOW : Color::BOLD_GREEN)));

        std::cout << Color::BOLD_CYAN;
        std::cout << "+------------------------------+------------------------------+\n";
        std::cout << Color::RESET;
    }

    void printTransferReceipt(const std::string& txId, int senderAcc, const std::string& senderName,
                              int receiverAcc, const std::string& receiverName,
                              double amount, double senderNewBal, const std::string& risk)
    {
        std::cout << "\n" << Color::BOLD_CYAN;
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|             SMARTBANK FUND TRANSFER RECEIPT                 |\n";
        std::cout << "+------------------------------+------------------------------+\n" << Color::RESET;

        auto printRow = [](const std::string& label, const std::string& val, const std::string& valColor = Color::BOLD_WHITE) {
            std::cout << Color::CYAN << "| " << Color::WHITE << std::left << std::setw(28) << label 
                      << Color::CYAN << "| " << valColor << std::left << std::setw(28) << val 
                      << Color::CYAN << "|\n" << Color::RESET;
        };

        printRow("Transaction ID", txId, Color::BOLD_YELLOW);
        printRow("Source Account", "Acc #" + std::to_string(senderAcc) + " (" + senderName + ")");
        printRow("Destination Account", "Acc #" + std::to_string(receiverAcc) + " (" + receiverName + ")");
        printRow("Transfer Amount", formatCurrency(amount), Color::BOLD_GREEN);
        printRow("Sender Remaining Balance", formatCurrency(senderNewBal), Color::BOLD_WHITE);
        printRow("Transaction Status", "SUCCESS", Color::BOLD_GREEN);
        printRow("Risk Category", risk, (risk == "HIGH" ? Color::BOLD_RED : (risk == "MEDIUM" ? Color::BOLD_YELLOW : Color::BOLD_GREEN)));

        std::cout << Color::BOLD_CYAN;
        std::cout << "+------------------------------+------------------------------+\n";
        std::cout << Color::RESET;
    }

    void printInterestCard(int accNum, const std::string& holder, const std::string& type,
                           double ratePercent, double principal, double interest, double projectedBal)
    {
        std::cout << "\n" << Color::BOLD_CYAN;
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                 ANNUAL INTEREST PROJECTION                  |\n";
        std::cout << "+------------------------------+------------------------------+\n" << Color::RESET;

        auto printRow = [](const std::string& label, const std::string& val, const std::string& valColor = Color::BOLD_WHITE) {
            std::cout << Color::CYAN << "| " << Color::WHITE << std::left << std::setw(28) << label 
                      << Color::CYAN << "| " << valColor << std::left << std::setw(28) << val 
                      << Color::CYAN << "|\n" << Color::RESET;
        };

        std::stringstream ssRate;
        ssRate << std::fixed << std::setprecision(1) << (ratePercent * 100.0) << "% per annum";

        printRow("Account Number", std::to_string(accNum));
        printRow("Account Holder", holder);
        printRow("Account Type", type);
        printRow("Annual Interest Rate", ssRate.str(), Color::YELLOW);
        printRow("Principal Balance", formatCurrency(principal));
        printRow("Estimated Annual Interest", formatCurrency(interest), Color::BOLD_MAGENTA);
        printRow("Projected Balance (1 Year)", formatCurrency(projectedBal), Color::BOLD_GREEN);

        std::cout << Color::BOLD_CYAN;
        std::cout << "+------------------------------+------------------------------+\n";
        std::cout << Color::RESET;
    }

    std::string getString(const std::string& prompt)
    {
        std::string input;
        while (true)
        {
            if (std::cin.eof()) return "";
            std::cout << Color::WHITE << prompt << Color::BOLD_CYAN;
            if (!std::getline(std::cin, input)) return "";
            std::cout << Color::RESET;

            size_t first = input.find_first_not_of(" \t\r\n");
            size_t last = input.find_last_not_of(" \t\r\n");
            if (first != std::string::npos && last != std::string::npos)
            {
                return input.substr(first, (last - first + 1));
            }

            printError("Input cannot be empty. Please enter a valid string.");
        }
    }

    int getInt(const std::string& prompt, int minVal, int maxVal)
    {
        int value;
        std::string line;
        while (true)
        {
            if (std::cin.eof()) return minVal;
            std::cout << Color::WHITE << prompt << Color::BOLD_CYAN;
            if (!std::getline(std::cin, line)) return minVal;
            std::cout << Color::RESET;

            std::stringstream ss(line);
            if (ss >> value && !(ss >> line))
            {
                if (value >= minVal && value <= maxVal)
                {
                    return value;
                }
                printError("Value out of valid range [" + std::to_string(minVal) + " - " + 
                           std::to_string(maxVal) + "]. Please re-enter.");
            }
            else
            {
                printError("Invalid numerical format! Please enter an integer.");
            }
        }
    }

    double getDouble(const std::string& prompt, double minVal)
    {
        double value;
        std::string line;
        while (true)
        {
            if (std::cin.eof()) return minVal;
            std::cout << Color::WHITE << prompt << Color::BOLD_CYAN;
            if (!std::getline(std::cin, line)) return minVal;
            std::cout << Color::RESET;

            std::stringstream ss(line);
            if (ss >> value && !(ss >> line))
            {
                if (value >= minVal)
                {
                    return value;
                }
                printError("Amount cannot be less than " + formatCurrency(minVal) + ". Please re-enter.");
            }
            else
            {
                printError("Invalid monetary amount! Please enter a valid decimal number.");
            }
        }
    }
}
