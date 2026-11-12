# SmartBank - Smart Banking & Transaction Security Management System

> **Course:** Object-Oriented Programming (OOP) in C++  
> **Author:** [Purva Deshmukh](https://github.com/purvaa211)  
> **Repository:** [https://github.com/purvaa211/Bank-Account-Management-](https://github.com/purvaa211/Bank-Account-Management-)  
> **Language / Standard:** C++ (C++11 / C++14 / C++17 compatible)  
> **Interface:** C++ Interactive Terminal & Modern Fintech Web Dashboard  

---

## 1. Project Overview

**SmartBank** is a modern, modular banking and transaction security management system implemented in C++. Designed as an engineering course project for Object-Oriented Programming, it simulates a production-grade digital banking platform with multiple account specializations, atomic fund transfers, rule-based anomaly detection, persistent local storage, and official financial statements.

Beyond routine account operations, SmartBank integrates a rule-based Transaction Security Engine (`FraudDetector`) that analyzes transaction velocity, volume, and daily debit limits to classify financial events into distinct risk tiers (LOW, MEDIUM, HIGH) and prompt supervisory review.

---

## 2. Key Modules & Functional Capabilities

### A. Specialized Account Management
- **Savings Account**: Offers 4.0% annual interest with an enforced minimum balance policy (Rs. 1,000.00). Transactions that would deplete reserves below this threshold are rejected.
- **Current Account**: Commercial checking account with 0.0% interest and an approved, configurable Overdraft Protection Facility (default: Rs. 25,000.00).
- **Fixed Deposit Account**: Term investment vehicle (6, 12, 24, 36 months) with quarterly compounding interest (up to 7.5% per annum), maturity date calculation, and premature liquidation notices.
- **Account Lifecycle**: Creation, lookup by Account Number or Holder Name, passbook card view, and validated account termination (blocking closure if negative overdraft exists).

### B. Financial Transactions & Atomic Execution
- **Deposits**: Credits funds to destination account balance with instant receipt generation.
- **Withdrawals**: Validates available balance, minimum balance constraints, and overdraft limits before debiting.
- **Atomic Fund Transfers**: End-to-end atomic transfer between two distinct accounts, guaranteeing synchronized debit and credit operations.
- **Ledger Recording**: Every transaction is assigned a unique auto-incremented ID (`TXN-10001`), timestamp, status, and security classification.

### C. Smart Transaction Security & Fraud Monitor
A rule-based C++ risk assessment engine examining transactions in real-time:
1. **Large Transaction Alert**: Flags single transactions exceeding configurable thresholds (default: Rs. 50,000.00 for advisory review; Rs. 100,000.00 for critical review).
2. **Daily Cumulative Debit Limit**: Tracks outgoing withdrawals and transfers per account within the calendar day to prevent rapid account draining (default: Rs. 100,000.00).
3. **Rapid Velocity Alert**: Detects rapid consecutive transactions within a short time window (3 or more transactions within 2 minutes).
- **Risk Categorization**:
  - `LOW`: Standard authorized transaction.
  - `MEDIUM`: Advisory threshold reached; flagged for administrative awareness.
  - `HIGH`: Major security threshold breached; requires administrative authorization.

### D. Audit Ledger & Financial Statements
- Search transactions by Transaction ID or Account Number.
- Filter records by operation type (`DEPOSIT`, `WITHDRAWAL`, `TRANSFER`) and risk level.
- Generate certified bank statements displaying opening balances, inflows, outflows, and net balances.
- Export statements to text files (`statements/statement_<accNum>.txt`) or print-ready PDF via the web platform.

### E. Data Persistence
- Real-time disk persistence for accounts, transactions, and security alerts via structured CSV files stored in `data/`:
  - `data/accounts.csv`
  - `data/transactions.csv`
  - `data/security_alerts.csv`
- Records survive application restarts and are automatically loaded on boot without duplicating identifiers.
- Web platform synchronizes state seamlessly with browser `localStorage`.

---

## 3. Object-Oriented Programming (OOP) Implementation

SmartBank is structured to demonstrate the principles of C++ Object-Oriented Programming:

| OOP Concept | Implementation in SmartBank |
| :--- | :--- |
| **Encapsulation** | Sensitive members (`balance`, `accountNumber`, `status`) are strictly private and accessible only through validated methods (`deposit()`, `withdraw()`, `getBalance()`). |
| **Abstraction** | `Account` serves as an abstract base class containing pure virtual functions (`calculateInterest()`, `getAccountType()`, `canWithdraw()`). |
| **Inheritance** | Derived classes `SavingsAccount`, `CurrentAccount`, and `FixedDepositAccount` inherit from `Account`, specializing withdrawal and interest logic. |
| **Runtime Polymorphism** | Virtual method dispatch allows the `Bank` container (`std::vector<Account*>`) to process polymorphic operations seamlessly at runtime. |
| **Function Overloading** | Overloaded lookup functions (`findAccount(int)` and `findAccount(std::string)`) and overloaded constructors. |
| **Operator Overloading** | Relational operators (`>`, `<`, `==`) overloaded to compare accounts based on balance and account number; stream operator (`<<`) overloaded for formatted output. |
| **Constructors & Destructors** | Parameterized constructors with member initializer lists; virtual destructors ensure leak-free heap cleanup (`RAII`). |
| **Static Members** | Auto-incrementing static counters `Account::nextAccountNumber`, `Transaction::nextTransactionSequence`, and `FraudDetector::nextAlertId`. |
| **Custom Exception Handling** | Domain exception hierarchy rooted in `BankingException` (inheriting from `std::exception`), including `MinimumBalanceException`, `OverdraftExceededException`, `InsufficientBalanceException`, and `AccountNotFoundException`. |
| **Composition & Aggregation** | `Bank` composes `FraudDetector` and `BankDatabase`, and aggregates a collection of heap-allocated `Account` pointers. |
| **Friend Class** | `BankStatementGenerator` is declared as a `friend class` of `Account`, granting privileged access to internal fields for statement generation. |

---

## 4. Project Architecture & Directory Structure

```
Bank-Account-Management-/
|
|-- Account.h / Account.cpp                     # Abstract Base Class with pure virtual methods
|-- SavingsAccount.h / SavingsAccount.cpp       # Savings Account (4.0% interest, min balance)
|-- CurrentAccount.h / CurrentAccount.cpp       # Current Account (overdraft protection facility)
|-- FixedDepositAccount.h / .cpp                # Fixed Deposit Account (compounding & maturity)
|-- Customer.h / Customer.cpp                   # Customer identity & contact entity
|-- Transaction.h / Transaction.cpp             # Financial transaction record with static IDs
|-- FraudDetector.h / FraudDetector.cpp         # Rule-based transaction security & anomaly engine
|-- BankStatementGenerator.h / .cpp             # Friend class for privileged statement exports
|-- BankDatabase.h / BankDatabase.cpp           # Persistent CSV database storage engine
|-- Bank.h / Bank.cpp                           # Core banking controller & container
|-- Exceptions.h                                # Custom banking exception hierarchy
|-- UIHelper.h / UIHelper.cpp                   # Terminal styling, ANSI colors, and safe stream I/O
|-- main.cpp                                    # Menu driver & console interface
|
|-- data/                                       # Persistent storage directory
|   |-- accounts.csv                            # Serialized account records
|   |-- transactions.csv                        # Serialized transaction history
|   `-- security_alerts.csv                     # Serialized security anomaly log
|
|-- statements/                                 # Generated bank statement text files
|
|-- docs/                                       # Modern Fintech Web Dashboard
|   |-- index.html                              # Dashboard, Accounts, Transactions, Security, Statements
|   |-- style.css                               # Deep navy fintech stylesheet & responsive layout
|   |-- app.js                                  # JavaScript OOP engine & local persistence
|   `-- logo.png                                # Official SmartBank branding asset
|
|-- build.bat                                   # One-click Windows build and launch script
|-- run_website.bat                             # Direct browser launcher for web platform
|-- Makefile                                    # Cross-platform GNU Makefile
|-- index.html                                  # Root redirection entry point
`-- README.md                                   # Comprehensive documentation
```

---

## 5. Installation and Execution

### Prerequisites
- **C++ Compiler**: MinGW `g++` (version 5.0 or newer) supporting C++11 or higher.
- **Operating System**: Windows, macOS, or Linux.
- **Web Browser**: Chrome, Edge, Firefox, or Safari for the web platform.

### Option 1: One-Click Build on Windows
Run `build.bat` from the project directory:
```cmd
build.bat
```
This automatically compiles the C++ codebase with `g++`, launches the fintech dashboard in your default browser, and starts the C++ console application.

### Option 2: Manual Compilation with g++
Compile all modular source files into an executable:
```bash
g++ -std=c++11 -Wall -Wextra Account.cpp Bank.cpp BankDatabase.cpp BankStatementGenerator.cpp CurrentAccount.cpp Customer.cpp FixedDepositAccount.cpp FraudDetector.cpp SavingsAccount.cpp Transaction.cpp UIHelper.cpp main.cpp -o smartbank.exe
```
Execute the compiled binary:
```bash
./smartbank.exe
```

### Option 3: Using GNU Make
```bash
make
./smartbank
```

### Option 4: Launching the Web Platform Directly
Open `docs/index.html` in any web browser, or run:
```cmd
run_website.bat
```

---

## 6. Screenshots & Interface Overview

### A. Terminal Interface
```
+====================================================================+
|   SMARTBANK - SMART BANKING & TRANSACTION SECURITY SYSTEM          |
|              C++ Object-Oriented Programming Project               |
+====================================================================+
 Accounts: 3  | Reserves: Rs. 340000.00  | Security Alerts: 1
+--------------------------------------------------------------------+
| ACCOUNT MANAGEMENT                                                 |
|  [1] Create Savings Account    (Min Balance Rs. 1,000 | 4.0% interest) |
|  [2] Create Current Account    (Overdraft Protection | 0.0% interest)|
|  [3] Create Fixed Deposit      (Term Investment | Guaranteed Maturity)|
|  [4] Close Bank Account        (Validated Account Termination)        |
+--------------------------------------------------------------------+
| FINANCIAL TRANSACTIONS                                             |
|  [5] Deposit Funds             (Credit account balance)               |
|  [6] Withdraw Funds            (Debit balance with limit check)       |
|  [7] Transfer Money            (Atomic transfer between 2 accounts)   |
+--------------------------------------------------------------------+
| INQUIRY, STATEMENTS & AUDIT                                        |
|  [8] Fast Account Inquiry      (Passbook card lookup)                 |
|  [9] View All Accounts Directory(Tabular accounts registry)            |
|  [10] Transaction History      (Audit ledger with filters)            |
|  [11] Account Statement Export (Friend class statement export)        |
+--------------------------------------------------------------------+
| SECURITY MONITORING & PLATFORM                                     |
|  [12] Security Center          (Rule-based Fraud Monitoring Alerts)   |
|  [13] Save & Persist Database  (Sync to data/ CSV files)              |
|  [14] Open Web Platform        (Launch Fintech GUI in Browser)        |
|  [15] Exit Application         (Safe shutdown and storage sync)       |
+====================================================================+
```

### B. Web Fintech Dashboard Layout
- **Sidebar**: Quick navigation between Dashboard, Accounts, Transactions, Security Center, Statements, and Settings.
- **Dashboard**: Live financial statistics, canvas-rendered cash flow charts, and real-time security risk breakdown.
- **Accounts**: Filterable registry with passbook views, interest simulations, and creation forms.
- **Transactions**: Dedicated tabs for deposits, withdrawals, and transfers with instant ledger receipts.
- **Security Center**: Live table of flagged events showing transaction ID, amount, risk tier, flag reason, and approval workflows.
- **Statements**: Official printable bank letterhead with transaction ledger and PDF export support.

---

## 7. Author

- **Author:** Purva Deshmukh
- **GitHub:** [@purvaa211](https://github.com/purvaa211)
- **Repository:** [https://github.com/purvaa211/Bank-Account-Management-](https://github.com/purvaa211/Bank-Account-Management-)
