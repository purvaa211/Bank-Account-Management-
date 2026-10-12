#ifndef UIHELPER_H
#define UIHELPER_H

#include <string>
#include <vector>

namespace UI
{
    // ANSI Color Escape Sequences
    namespace Color
    {
        extern const std::string RESET;
        extern const std::string BOLD;
        extern const std::string DIM;
        extern const std::string UNDERLINE;

        extern const std::string RED;
        extern const std::string GREEN;
        extern const std::string YELLOW;
        extern const std::string BLUE;
        extern const std::string MAGENTA;
        extern const std::string CYAN;
        extern const std::string WHITE;
        extern const std::string GRAY;

        extern const std::string BOLD_RED;
        extern const std::string BOLD_GREEN;
        extern const std::string BOLD_YELLOW;
        extern const std::string BOLD_MAGENTA;
        extern const std::string BOLD_CYAN;
        extern const std::string BOLD_WHITE;
    }

    // Terminal initialization
    void initTerminal();

    // Screen utilities
    void clearScreen();
    void pauseScreen();
    void printDivider(int width = 70, char ch = '-');
    void printDoubleDivider(int width = 70);

    // Banners & Headers
    void printHeroBanner();
    void printSectionHeader(const std::string& title);

    // Status notifications
    void printSuccess(const std::string& message);
    void printError(const std::string& message);
    void printInfo(const std::string& message);
    void printWarning(const std::string& message);

    // Formatted Data Displays
    void printReceipt(int accNum, const std::string& holder, const std::string& type, 
                      const std::string& txType, double prevBal, double amount, double newBal);
    
    void printInterestCard(int accNum, const std::string& holder, const std::string& type,
                           double ratePercent, double principal, double interest, double projectedBal);

    // Formatting helpers
    std::string formatCurrency(double amount);

    // Safe Input Handlers
    std::string getString(const std::string& prompt);
    int getInt(const std::string& prompt, int minVal = 0, int maxVal = 2147483647);
    double getDouble(const std::string& prompt, double minVal = 0.0);
}

#endif // UIHELPER_H
