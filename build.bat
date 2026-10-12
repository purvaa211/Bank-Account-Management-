@echo off
REM ==============================================================================
REM Bank Account Management System - Build & Run Script
REM ==============================================================================

echo [BUILD] Compiling Bank Account Management System with g++...
g++ -std=c++11 -Wall -Wextra Account.cpp Bank.cpp CurrentAccount.cpp SavingsAccount.cpp UIHelper.cpp main.cpp -o bank_management.exe

if %ERRORLEVEL% EQU 0 (
    echo [BUILD SUCCESS] Compilation finished with 0 errors!
    echo [RUN] Launching Bank Account Management System...
    echo.
    bank_management.exe
) else (
    echo.
    echo [BUILD FAILED] An error occurred during compilation. Please ensure MinGW/g++ is installed and added to PATH.
    pause
)
