@echo off
REM ==============================================================================
REM SmartBank - Smart Banking & Transaction Security Management System
REM Build & Launch Script
REM ==============================================================================

echo [BUILD] Compiling SmartBank C++ Core with g++...
g++ -std=c++11 -Wall -Wextra Account.cpp Bank.cpp BankDatabase.cpp BankStatementGenerator.cpp CurrentAccount.cpp Customer.cpp FixedDepositAccount.cpp FraudDetector.cpp SavingsAccount.cpp Transaction.cpp UIHelper.cpp main.cpp -o smartbank.exe

if %ERRORLEVEL% EQU 0 (
    echo [BUILD SUCCESS] Compilation finished with 0 errors!
    echo [LAUNCH] Opening SmartBank Fintech Platform in default browser...
    start "" "%~dp0docs\index.html"
    echo [RUN] Launching SmartBank Console Application...
    echo.
    smartbank.exe
) else (
    echo.
    echo [BUILD FAILED] An error occurred during compilation. Please verify MinGW/g++ installation.
    pause
)
