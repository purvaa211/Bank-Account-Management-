# SmartBank - Bank Account & Transaction Security Management System

> **Course:** Object-Oriented Programming (OOP) in C++  
> **Author:** [Purva Deshmukh](https://github.com/purvaa211)  
> **Repository:** [https://github.com/purvaa211/Bank-Account-Management-](https://github.com/purvaa211/Bank-Account-Management-)  
> **Language / Standard:** C++ (C++11 / C++14 / C++17 compatible)  
> **Interface:** C++ Interactive Console Engine and Local Demonstration Web Interface  

---

## 1. Project Overview

**SmartBank** is an Object-Oriented Banking and Transaction Security Management System engineered in C++. Designed as an academic software engineering course project, it demonstrates foundational and advanced C++ Object-Oriented Programming concepts within a realistic banking domain.

The platform provides complete customer entity management with multi-account ownership, three specialized account types (Savings, Current, and Fixed Deposit), atomic fund transfers between accounts, rule-based fraud detection using real timestamp velocity mathematics, persistent CSV database storage, certified statement generation via friend classes, and custom domain exceptions.

In addition to the authoritative C++ console application, the repository provides a standalone, client-side web interface located in `docs/` styled with a private banking aesthetic (Forest Green, Warm Ivory, Brushed Brass, and Charcoal), utilizing browser `localStorage` for offline demonstration.

---

## 2. Object-Oriented Programming (OOP) Concept Mapping

The codebase is organized around C++ Object-Oriented design principles. The following table details how each syllabus concept is realized in code:

| OOP Concept | Class / Struct | Key Methods / Members | Source File | Educational Purpose & Viva Defense |
| :--- | :--- | :--- | :--- | :--- |
| **Abstraction** | `Account` | `calculateInterest()`, `getAccountType()`, `getInterestRate()`, `canWithdraw()` | `Account.h` | Defines an abstract base specification with pure virtual functions (`= 0`) that cannot be instantiated directly, enforcing polymorphic contracts on derived accounts. |
| **Encapsulation** | `Customer`, `Account`, `Transaction` | `balance`, `customerId`, `accountNumber`, `deposit()`, `withdraw()` | `Customer.h`, `Account.h` | Protects internal financial balances and identifiers by marking members `protected` or `private`. Balance mutations occur strictly through validated deposit, withdrawal, and transfer routines. |
| **Inheritance** | `SavingsAccount`, `CurrentAccount`, `FixedDepositAccount` | Base class constructor delegation, specialized member variables | `SavingsAccount.h`, `CurrentAccount.h`, `FixedDepositAccount.h` | Subclasses derive publicly (`public Account`) to inherit common attributes while adding specialized features (minimum balance, overdraft limits, maturity dates). |
| **Runtime Polymorphism** | `Account*` via `Bank` container | Virtual method table (vtable) dispatch: `a->canWithdraw()`, `a->withdraw()`, `a->calculateInterest()` | `Bank.cpp` | Allows the bank container (`std::vector<Account*>`) to manage diverse account types uniformly while invoking type-specific business logic dynamically. |
| **Constructors & Destructors** | `Account`, `Customer`, Derived classes | Member initializer lists, `virtual ~Account()` | All `.h` / `.cpp` files | Ensures deterministic resource initialization. Virtual base destructors guarantee safe, leak-free deletion of heap-allocated derived objects via base pointers (RAII). |
| **Function Overloading** | `Bank`, `BankDatabase` | `findCustomer(int)` vs `findCustomer(string)`, `findAccount(int)` vs `findAccount(string)` | `Bank.h`, `Bank.cpp` | Compile-time polymorphism enabling account and customer searches by unique numerical ID or full holder name. |
| **Operator Overloading** | `Customer`, `Account`, `Transaction` | `operator<<`, `operator>`, `operator<`, `operator==` | `Customer.cpp`, `Account.cpp`, `Transaction.cpp` | Overloads stream insertion for standard console rendering and relational operators for balance and identifier comparisons. |
| **Friend Class** | `BankStatementGenerator` | `friend class BankStatementGenerator;` in `Account` | `Account.h`, `BankStatementGenerator.h` | Implements the friend mechanism allowing an external auditing class privileged access to private account attributes to generate certified official statements. |
| **Static Members** | `Customer`, `Account`, `Transaction`, `FraudDetector` | `nextCustomerId`, `nextAccountNumber`, `nextTransactionSequence`, `nextAlertId` | `Customer.h`, `Account.h`, `Transaction.h`, `FraudDetector.h` | Class-level state management to generate sequential, collision-free identifiers without external global counters. |
| **Custom Exception Handling** | `BankingException` hierarchy | `InsufficientBalanceException`, `MinimumBalanceException`, `OverdraftExceededException`, `AccountNotFoundException` | `Exceptions.h` | Implements domain-specific error handling derived from `std::exception`. Operations fail safely by throwing descriptive exceptions caught by the UI layer. |
| **Composition & Aggregation** | `Bank` | `std::vector<Customer> customers;`, `std::vector<Account*> accounts;`, `FraudDetector detector;`, `BankDatabase database;` | `Bank.h`, `Bank.cpp` | Demonstrates architectural separation: `Bank` *aggregates* customers and accounts, and *composes* its database persistence and fraud detection modules. |

---

## 3. Core Functional Architecture

### A. Customer Entity & Multi-Account Association
- In realistic banking systems, customers exist independently of accounts.
- `Customer` maintains a unique auto-incremented identifier (`CUST-201`, `CUST-202`, etc.), full name, email address, and phone number.
- Each `Account` holds a `customerId` reference. A single customer entity can own multiple bank accounts simultaneously (for example, a personal Savings Account alongside a commercial Current Account or term Fixed Deposit).
- Bank operations include Customer Registration, Customer Directory lookups, contact detail updates, and customer-filtered account inquiries.

### B. Specialized Account Management
1. **Savings Account**:
   - Accrues an annual interest rate of 4.0%.
   - Enforces a mandatory minimum balance of Rs. 1,000.00.
   - Any withdrawal or transfer attempting to reduce the balance below Rs. 1,000.00 is rejected by `canWithdraw()`.
2. **Current Account**:
   - Commercial checking account designed for business operations with 0.0% interest.
   - Supports an approved, configurable Overdraft Facility (default: Rs. 25,000.00).
   - Allows balances to become negative up to the overdraft limit before rejecting transactions.
3. **Fixed Deposit Account**:
   - Term investment vehicle (6, 12, 24, or 36 months) with guaranteed yield (up to 7.5% per annum).
   - Distinct separation of balance: ledger principal balance ($P$) remains distinct from the projected maturity value ($A$).
   - Maturity amount is calculated using the quarterly compounding formula:
     $$A = P \times \left(1 + \frac{r}{4}\right)^{4 \times \left(\frac{t}{12}\right)}$$
   - Routine withdrawals are blocked during the lock-in period; liquidation requires formal account termination.

### C. Financial Transactions & Atomic Execution
- **Deposits**: Credits funds to the target account with immediate balance update and transaction logging.
- **Withdrawals**: Verifies account active status, validates amount parameters, and tests withdrawal rules before mutation.
- **Atomic Fund Transfers**:
  - Validates source and destination accounts exist, are distinct, and are currently `ACTIVE`.
  - Tests sender balance against `canWithdraw()` prior to debiting.
  - If any condition fails, neither account balance is mutated.
  - Once validated, the debit and credit execute atomically, generating an audit record.

### D. Smart Rule-Based Transaction Security (FraudDetector)
The transaction monitor is a deterministic rule-based C++ engine (`FraudDetector`) that inspects transactions across three dimensions:
1. **Large Transaction Alert**: Flags single transactions exceeding configurable thresholds (default: Rs. 50,000.00 triggers MEDIUM risk; transactions exceeding 2x the threshold trigger HIGH risk).
2. **Daily Cumulative Outgoing Debit Limit**: Sums only outgoing debits (withdrawals and transfers where the account is the sender) on the current calendar day. If projected daily debits exceed Rs. 1,00,000.00, a HIGH risk alert is raised to prevent rapid account depletion. Incoming deposits and incoming transfer credits are excluded.
3. **Rapid Transaction Velocity**: Parses timestamp strings (`YYYY-MM-DD HH:MM:SS`) into epoch seconds using `std::mktime` and compares elapsed time with `std::difftime`. Flags transactions if 3 or more transactions originate from the same account within a 2-minute rolling window.
- **Risk Categorization**:
  - `LOW`: Routine, authorized operation within standard risk parameters.
  - `MEDIUM`: Advisory threshold reached; flagged for supervisory review.
  - `HIGH`: Critical anomaly detected; flagged for immediate administrative review.

### E. Persistent Storage System
- All banking data persists in standard CSV files inside the `data/` directory:
  - `data/customers.csv`: Customer ID, Full Name, Email, Phone Number.
  - `data/accounts.csv`: Account Number, Customer ID, Holder Name, Type, Balance, Status, Opening Date, Extra fields (min balance / overdraft / FD term, rate, maturity date).
  - `data/transactions.csv`: Transaction ID, Sender Account, Receiver Account, Type, Amount, Timestamp, Status, Risk Level, Flag Reason.
  - `data/security_alerts.csv`: Alert ID, Transaction ID, Account Number, Amount, Risk Level, Reason, Timestamp, Review Status.
- Database loading automatically initializes static ID counters to prevent identifier collisions across sessions.

---

## 4. Project Directory Structure

```
Bank-Account-Management-/
|
|-- Account.h / Account.cpp                     # Abstract base class for bank accounts
|-- SavingsAccount.h / SavingsAccount.cpp       # Savings account derived class
|-- CurrentAccount.h / CurrentAccount.cpp       # Current account with overdraft facility
|-- FixedDepositAccount.h / .cpp                # Fixed deposit with quarterly compounding
|-- Customer.h / Customer.cpp                   # Customer entity with multi-account linking
|-- Transaction.h / Transaction.cpp             # Financial transaction record model
|-- FraudDetector.h / FraudDetector.cpp         # Rule-based transaction security engine
|-- BankStatementGenerator.h / .cpp             # Friend class for certified statement exports
|-- BankDatabase.h / BankDatabase.cpp           # Persistent CSV database storage engine
|-- Bank.h / Bank.cpp                           # Core banking business logic controller
|-- Exceptions.h                                # Custom domain exception hierarchy
|-- UIHelper.h / UIHelper.cpp                   # Console styling and robust stream input guards
|-- main.cpp                                    # Menu driver and console interface
|
|-- data/                                       # Persistent CSV storage directory
|   |-- customers.csv                           # Customer entity records
|   |-- accounts.csv                            # Account records with customer IDs
|   |-- transactions.csv                        # Transaction audit ledger
|   `-- security_alerts.csv                     # Security anomaly log
|
|-- statements/                                 # Generated bank statement text files
|
|-- docs/                                       # Standalone Web Demonstration Interface
|   |-- index.html                              # Private banking interface layout
|   |-- style.css                               # Private banking theme stylesheet
|   |-- app.js                                  # JavaScript OOP engine and localStorage sync
|   `-- logo.png                                # Bank emblem asset
|
|-- build.bat                                   # Windows compilation and launch script
|-- run_website.bat                             # Web interface browser launcher
|-- Makefile                                    # Cross-platform GNU Make configuration
|-- index.html                                  # Root redirect to docs/
`-- README.md                                   # Project documentation
```

---

## 5. Build and Execution Instructions

### Prerequisites
- **Compiler**: GCC / MinGW `g++` (version 5.0 or newer supporting C++11 or higher).
- **Operating System**: Windows, Linux, or macOS.

### Compilation Commands

#### Windows (MinGW g++)
```powershell
g++ -std=c++11 -Wall -Wextra Account.cpp Bank.cpp BankDatabase.cpp BankStatementGenerator.cpp CurrentAccount.cpp Customer.cpp FixedDepositAccount.cpp FraudDetector.cpp SavingsAccount.cpp Transaction.cpp UIHelper.cpp main.cpp -o smartbank.exe
```

Run the application:
```powershell
.\smartbank.exe
```

#### Linux / macOS
```bash
g++ -std=c++11 -Wall -Wextra Account.cpp Bank.cpp BankDatabase.cpp BankStatementGenerator.cpp CurrentAccount.cpp Customer.cpp FixedDepositAccount.cpp FraudDetector.cpp SavingsAccount.cpp Transaction.cpp UIHelper.cpp main.cpp -o smartbank
./smartbank
```

#### Using GNU Make
```bash
make
./smartbank
```

#### Automated Script (Windows)
Double-click or run `build.bat` from PowerShell / Command Prompt to compile and launch both the console engine and the browser interface.

---

## 6. Console Interface Menu Structure

The C++ console interface is divided into functional operational domains:

```
+====================================================================+
|   SMARTBANK - BANK ACCOUNT & TRANSACTION SECURITY SYSTEM           |
|              C++ Object-Oriented Programming Project               |
+====================================================================+
 Customers: 4  | Accounts: 5  | Reserves: Rs. 390000.00  | Alerts: 2
+--------------------------------------------------------------------+
| CUSTOMER MANAGEMENT                                                |
|  [1] Register New Customer     (Name, Email, Phone contact profile)   |
|  [2] View Customer Directory   (List all customers & owned accounts)  |
|  [3] Update Customer Contact   (Edit email / phone by Customer ID)    |
+--------------------------------------------------------------------+
| ACCOUNT MANAGEMENT                                                 |
|  [4] Create Savings Account    (Min Balance Rs. 1,000 | 4.0% interest) |
|  [5] Create Current Account    (Overdraft Protection | 0.0% interest)|
|  [6] Create Fixed Deposit      (Term Investment | Guaranteed Maturity)|
|  [7] Close Bank Account        (Validated Account Termination)        |
+--------------------------------------------------------------------+
| FINANCIAL TRANSACTIONS                                             |
|  [8] Deposit Funds             (Credit account balance)               |
|  [9] Withdraw Funds            (Debit balance with limit check)       |
|  [10] Transfer Money            (Atomic transfer between 2 accounts)   |
+--------------------------------------------------------------------+
| INQUIRY, STATEMENTS & AUDIT                                        |
|  [11] Fast Account Inquiry      (Passbook card lookup)                 |
|  [12] View All Accounts Registry(Tabular accounts directory)           |
|  [13] Transaction History      (Audit ledger with filters)            |
|  [14] Official Statement Export(Friend class certified statement)     |
+--------------------------------------------------------------------+
| SECURITY MONITORING & PLATFORM                                     |
|  [15] Security Center          (Rule-based Anomaly Monitor Alerts)    |
|  [16] Save & Persist Database  (Sync to data/ CSV files)              |
|  [17] Open Web Platform        (Launch Fintech GUI in Browser)        |
|  [18] Exit Application         (Safe shutdown and storage sync)       |
+====================================================================+
```

---

## 7. Web Platform Demonstration (`docs/`)

The web platform provides a visual interface for demonstrating system capabilities:
- **Private Banking Aesthetic**: Styled with Forest Green (`#183D32`), Warm Ivory (`#F5F1E8`), Brushed Brass (`#C5AD7D`), and Charcoal typography (`#292D29`) using Google Fonts Lora and Inter.
- **Customer Entity Section**: Dedicated customer directory displaying registered profiles, owned accounts count, and contact update tools.
- **Account Creation**: Opens accounts associated with selected customer entities, providing dynamic calculation previews for Fixed Deposit maturity.
- **Transaction Processing**: Live operations form for deposits, withdrawals, and transfers with real-time audit receipts.
- **Security Center**: Live monitoring queue displaying rule flags, risk ratings, and supervisory clear/flag actions.
- **Statement Generator**: Printable official statement layout with certified copy tagging.
- **Client-Side Persistence**: State persists across page refreshes via `localStorage` with JSON export support.

---

## 8. Author

- **Author:** Purva Deshmukh
- **Course:** Object-Oriented Programming (OOP) in C++
- **GitHub:** [@purvaa211](https://github.com/purvaa211)
- **Repository:** [https://github.com/purvaa211/Bank-Account-Management-](https://github.com/purvaa211/Bank-Account-Management-)
