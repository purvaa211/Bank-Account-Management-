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
        const std::string RESET        = "\033[0m";
        const std::string BOLD         = "\033[1m";
        const std::string DIM          = "\033[2m";
        const std::string UNDERLINE    = "\033[4m";

        const std::string RED          = "\033[31m";
        const std::string GREEN        = "\033[32m";
        const std::string YELLOW       = "\033[33m";
        const std::string BLUE         = "\033[34m";
        const std::string MAGENTA      = "\033[35m";
        const std::string CYAN         = "\033[36m";
        const std::string WHITE        = "\033[37m";
        const std::string GRAY         = "\033[90m";

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
        // Enable UTF-8 console output and ANSI virtual terminal escape sequences
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
#ifdef _WIN32
        // Using ANSI clear screen with fallback
        std::cout << "\033[2J\033[H";
#else
        std::cout << "\033[2J\033[H";
#endif
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
        std::cout << "                 BANK ACCOUNT MANAGEMENT SYSTEM                       \n";
        std::cout << "             Object-Oriented Programming in C++ Project               \n";
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
        std::cout << "\n" << Color::BOLD_GREEN << "[SUCCESS] " << Color::GREEN 
                  << message << Color::RESET << "\n";
    }

    void printError(const std::string& message)
    {
        std::cout << "\n" << Color::BOLD_RED << "[ERROR] " << Color::RED 
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
                      const std::string& txType, double prevBal, double amount, double newBal)
    {
        std::cout << "\n" << Color::BOLD_CYAN;
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                    TRANSACTION RECEIPT                      |\n";
        std::cout << "+------------------------------+------------------------------+\n" << Color::RESET;

        auto printRow = [](const std::string& label, const std::string& val, const std::string& valColor = Color::BOLD_WHITE) {
            std::cout << Color::CYAN << "| " << Color::WHITE << std::left << std::setw(28) << label 
                      << Color::CYAN << "| " << valColor << std::left << std::setw(28) << val 
                      << Color::CYAN << "|\n" << Color::RESET;
        };

        printRow("Account Number", std::to_string(accNum));
        printRow("Account Holder", holder);
        printRow("Account Type", type);
        printRow("Transaction Type", txType, (txType == "DEPOSIT" ? Color::BOLD_GREEN : Color::BOLD_YELLOW));
        printRow("Previous Balance", formatCurrency(prevBal));
        printRow("Transaction Amount", formatCurrency(amount), (txType == "DEPOSIT" ? Color::BOLD_GREEN : Color::BOLD_RED));
        printRow("Updated Balance", formatCurrency(newBal), Color::BOLD_GREEN);

        std::cout << Color::BOLD_CYAN;
        std::cout << "+------------------------------+------------------------------+\n";
        std::cout << Color::RESET;
    }

    void printInterestCard(int accNum, const std::string& holder, const std::string& type,
                           double ratePercent, double principal, double interest, double projectedBal)
    {
        std::cout << "\n" << Color::BOLD_MAGENTA;
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                INTEREST CALCULATION SUMMARY                 |\n";
        std::cout << "+------------------------------+------------------------------+\n" << Color::RESET;

        auto printRow = [](const std::string& label, const std::string& val, const std::string& valColor = Color::BOLD_WHITE) {
            std::cout << Color::MAGENTA << "| " << Color::WHITE << std::left << std::setw(28) << label 
                      << Color::MAGENTA << "| " << valColor << std::left << std::setw(28) << val 
                      << Color::MAGENTA << "|\n" << Color::RESET;
        };

        std::stringstream rateStr;
        rateStr << std::fixed << std::setprecision(1) << (ratePercent * 100.0) << "% per annum";

        printRow("Account Number", std::to_string(accNum));
        printRow("Account Holder", holder);
        printRow("Account Type", type);
        printRow("Annual Interest Rate", rateStr.str());
        printRow("Current Principal", formatCurrency(principal));
        printRow("Annual Interest Earned", formatCurrency(interest), Color::BOLD_GREEN);
        printRow("Projected Balance (1 Yr)", formatCurrency(projectedBal), Color::BOLD_GREEN);

        std::cout << Color::BOLD_MAGENTA;
        std::cout << "+------------------------------+------------------------------+\n";
        std::cout << Color::RESET;
    }

    std::string getString(const std::string& prompt)
    {
        std::string line;
        while (true)
        {
            if (std::cin.eof()) return "";

            std::cout << Color::BOLD_WHITE << prompt << Color::CYAN;
            if (!std::getline(std::cin, line))
            {
                std::cout << Color::RESET;
                return "";
            }
            std::cout << Color::RESET;

            // Trim leading/trailing whitespace
            size_t first = line.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                if (std::cin.eof()) return "";
                printError("Input cannot be empty. Please enter a valid text.");
                continue;
            }
            size_t last = line.find_last_not_of(" \t\r\n");
            return line.substr(first, (last - first + 1));
        }
    }

    int getInt(const std::string& prompt, int minVal, int maxVal)
    {
        while (true)
        {
            if (std::cin.eof()) return maxVal; // default safe exit if input stream ends

            std::cout << Color::BOLD_WHITE << prompt << Color::CYAN;
            std::string line;
            if (!std::getline(std::cin, line))
            {
                std::cout << Color::RESET;
                return maxVal;
            }
            std::cout << Color::RESET;

            // Trim whitespace
            size_t first = line.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                if (std::cin.eof()) return maxVal;
                printError("Please enter a number.");
                continue;
            }
            size_t last = line.find_last_not_of(" \t\r\n");
            std::string trimmed = line.substr(first, (last - first + 1));

            try
            {
                size_t idx = 0;
                int val = std::stoi(trimmed, &idx);
                if (idx == trimmed.length())
                {
                    if (val >= minVal && val <= maxVal)
                    {
                        return val;
                    }
                    printError("Value must be between " + std::to_string(minVal) + " and " + std::to_string(maxVal) + ".");
                    continue;
                }
            }
            catch (...)
            {
                // Fallthrough to error below
            }
            printError("Invalid numerical input. Please enter a valid integer.");
        }
    }

    double getDouble(const std::string& prompt, double minVal)
    {
        while (true)
        {
            if (std::cin.eof()) return minVal;

            std::cout << Color::BOLD_WHITE << prompt << Color::CYAN;
            std::string line;
            if (!std::getline(std::cin, line))
            {
                std::cout << Color::RESET;
                return minVal;
            }
            std::cout << Color::RESET;

            // Trim whitespace
            size_t first = line.find_first_not_of(" \t\r\n");
            if (first == std::string::npos)
            {
                if (std::cin.eof()) return minVal;
                printError("Please enter an amount.");
                continue;
            }
            size_t last = line.find_last_not_of(" \t\r\n");
            std::string trimmed = line.substr(first, (last - first + 1));

            try
            {
                size_t idx = 0;
                double val = std::stod(trimmed, &idx);
                if (idx == trimmed.length())
                {
                    if (val >= minVal)
                    {
                        return val;
                    }
                    printError("Amount cannot be less than " + formatCurrency(minVal) + ".");
                    continue;
                }
            }
            catch (...)
            {
                // Fallthrough to error below
            }
            printError("Invalid numerical input. Please enter a valid decimal number (e.g. 500 or 1250.75).");
        }
    }
}
