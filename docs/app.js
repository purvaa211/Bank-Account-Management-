/**
 * ==============================================================================
 * Bank Account Management System - Web Interactive Simulation & OOP Engine
 * Author: Purva Deshmukh (https://github.com/purvaa211)
 * Course: Object-Oriented Programming (OOP) in C++
 * ==============================================================================
 */

// ==============================================================================
// 1. C++ OBJECT-ORIENTED PROGRAMMING CORE DOMAIN MIRROR
// ==============================================================================

class BankingException extends Error {
  constructor(message) {
    super(message);
    this.name = 'BankingException';
  }
}

class InvalidAmountException extends BankingException {
  constructor(msg = 'Invalid amount! Amount must be strictly greater than zero.') {
    super(msg);
    this.name = 'InvalidAmountException';
  }
}

class InsufficientBalanceException extends BankingException {
  constructor(currentBalance, requestedAmount) {
    super(`Insufficient balance! Available balance is $${currentBalance.toFixed(2)}, but requested withdrawal is $${requestedAmount.toFixed(2)}.`);
    this.name = 'InsufficientBalanceException';
  }
}

class AccountNotFoundException extends BankingException {
  constructor(accNum) {
    super(`Account not found! Account #${accNum} does not exist in banking records.`);
    this.name = 'AccountNotFoundException';
  }
}

// Abstract Base Class
class Account {
  static nextAccountNumber = 1001;

  constructor(name, balance) {
    if (new.target === Account) {
      throw new Error('Cannot instantiate abstract class Account directly.');
    }
    this._accountNumber = Account.nextAccountNumber++;
    this._name = name;
    this._balance = balance;
  }

  getAccountNumber() {
    return this._accountNumber;
  }

  getName() {
    return this._name;
  }

  getBalance() {
    return this._balance;
  }

  deposit(amount) {
    if (amount <= 0) {
      throw new InvalidAmountException('Deposit amount must be greater than zero.');
    }
    this._balance += amount;
  }

  withdraw(amount) {
    if (amount <= 0) {
      throw new InvalidAmountException('Withdrawal amount must be greater than zero.');
    }
    if (amount > this._balance) {
      throw new InsufficientBalanceException(this._balance, amount);
    }
    this._balance -= amount;
  }

  calculateInterest() {
    throw new Error('Pure virtual function calculateInterest() must be implemented.');
  }

  getAccountType() {
    throw new Error('Pure virtual function getAccountType() must be implemented.');
  }

  getInterestRate() {
    throw new Error('Pure virtual function getInterestRate() must be implemented.');
  }
}

// Derived Class: SavingsAccount
class SavingsAccount extends Account {
  constructor(name, balance) {
    super(name, balance);
  }

  calculateInterest() {
    const rate = 0.04;
    const interest = this.getBalance() * rate;
    return {
      ratePercent: 4.0,
      principal: this.getBalance(),
      interest: interest,
      projectedBalance: this.getBalance() + interest,
      note: 'Savings accounts earn 4.0% interest per annum.'
    };
  }

  getAccountType() {
    return 'Savings Account';
  }

  getInterestRate() {
    return 0.04;
  }
}

// Derived Class: CurrentAccount
class CurrentAccount extends Account {
  constructor(name, balance) {
    super(name, balance);
  }

  calculateInterest() {
    return {
      ratePercent: 0.0,
      principal: this.getBalance(),
      interest: 0.0,
      projectedBalance: this.getBalance(),
      note: 'Current accounts are transactional accounts with 0% interest.'
    };
  }

  getAccountType() {
    return 'Current Account';
  }

  getInterestRate() {
    return 0.0;
  }
}

// Container Class: Bank
class Bank {
  constructor() {
    this.accounts = [];
  }

  addAccount(account) {
    this.accounts.push(account);
  }

  findAccount(accountNumber) {
    const acc = this.accounts.find(a => a.getAccountNumber() === Number(accountNumber));
    if (!acc) {
      throw new AccountNotFoundException(accountNumber);
    }
    return acc;
  }

  getTotalBankBalance() {
    return this.accounts.reduce((acc, curr) => acc + curr.getBalance(), 0);
  }

  getAccountCount() {
    return this.accounts.length;
  }

  exportTextReport() {
    let out = '======================================================================\n';
    out += '                 BANK ACCOUNT MANAGEMENT SYSTEM RECORD                \n';
    out += '======================================================================\n\n';

    let total = 0;
    for (const acc of this.accounts) {
      out += `Account Number : ${acc.getAccountNumber()}\n`;
      out += `Account Holder : ${acc.getName()}\n`;
      out += `Account Type   : ${acc.getAccountType()}\n`;
      out += `Balance        : $${acc.getBalance().toFixed(2)}\n`;
      out += `Interest Rate  : ${(acc.getInterestRate() * 100).toFixed(1)}%\n`;
      out += '----------------------------------------------------------------------\n';
      total += acc.getBalance();
    }

    out += '\nSUMMARY:\n';
    out += `Total Accounts : ${this.accounts.length}\n`;
    out += `Total Reserves : $${total.toFixed(2)}\n`;
    out += '======================================================================\n';
    return out;
  }
}

// ==============================================================================
// 2. C++ CODE SNIPPET MAPPINGS & OOP CONCEPTS
// ==============================================================================

const OOP_CONCEPTS = {
  'create-savings': {
    badge: 'Inheritance & Constructors',
    file: 'SavingsAccount.cpp',
    title: 'Derived Class Constructor & Base Delegation',
    desc: 'SavingsAccount publicly inherits from Account. Its constructor invokes the base Account constructor using member initialization list to assign values and generate sequential account IDs.',
    snippet: `// SavingsAccount constructor delegates initialization to base Account
SavingsAccount::SavingsAccount(const std::string& n, double b)
    : Account(n, b)
{
}

// Dynamically created on heap and stored in vector<Account*>
Account* account = new SavingsAccount(name, balance);
bank.addAccount(account);`
  },
  'create-current': {
    badge: 'Polymorphic Base Class',
    file: 'CurrentAccount.cpp',
    title: 'Multiple Derived Classes Sharing Base Interface',
    desc: 'CurrentAccount implements the same Account interface but provides commercial checking semantics with zero interest. Both derived objects share polymorphic Account* pointers.',
    snippet: `// CurrentAccount constructor delegating to base Account
CurrentAccount::CurrentAccount(const std::string& n, double b)
    : Account(n, b)
{
}

Account* account = new CurrentAccount(name, balance);
bank.addAccount(account);`
  },
  'deposit': {
    badge: 'Encapsulation & Validation',
    file: 'Account.cpp',
    title: 'Data Encapsulation & Mutator Guarding',
    desc: 'Balance is private in Account. External code cannot modify balance directly; it must invoke deposit() which verifies that the amount is strictly positive, throwing InvalidAmountException if invalid.',
    snippet: `void Account::deposit(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException("Deposit amount must be greater than zero.");
    }

    balance = balance + amount;
}`
  },
  'withdraw': {
    badge: 'Exception Handling',
    file: 'Account.cpp & Exceptions.h',
    title: 'Domain Exception Handling (try / throw / catch)',
    desc: 'withdraw() guards against negative amounts and insufficient balance. If requested withdrawal exceeds available funds, InsufficientBalanceException is thrown and caught by main().',
    snippet: `void Account::withdraw(double amount)
{
    if (amount <= 0)
    {
        throw InvalidAmountException("Withdrawal amount must be greater than zero.");
    }

    if (amount > balance)
    {
        throw InsufficientBalanceException(balance, amount);
    }

    balance = balance - amount;
}`
  },
  'calculate-interest': {
    badge: 'Runtime Polymorphism',
    file: 'Account.h & Derived Classes',
    title: 'Pure Virtual Functions & Dynamic Method Dispatch',
    desc: 'Account declares virtual void calculateInterest() = 0. When calling account->calculateInterest() through an Account* pointer, the vtable dynamically invokes SavingsAccount (4%) or CurrentAccount (0%).',
    snippet: `// Pure virtual function in Account.h
virtual void calculateInterest() = 0;

// Dynamic dispatch in main.cpp:
Account* account = bank.findAccount(accountNumber);
account->calculateInterest(); // Calls SavingsAccount or CurrentAccount at runtime!`
  },
  'save-file': {
    badge: 'File I/O Streams',
    file: 'Bank.cpp',
    title: 'File Persistence with std::ofstream',
    desc: 'Bank::saveAccounts() iterates over accounts in the STL vector and writes formatted passbook statements to accounts.txt using C++ output file streams.',
    snippet: `void Bank::saveAccounts() const
{
    std::ofstream file("accounts.txt");
    if (!file) {
        throw BankingException("Unable to create accounts.txt");
    }

    for (const Account* account : accounts) {
        file << "Account Number : " << account->getAccountNumber() << "\\n";
        file << "Account Holder : " << account->getName() << "\\n";
        file << "Balance        : $" << account->getBalance() << "\\n";
    }
    file.close();
}`
  }
};

// ==============================================================================
// 3. APPLICATION STATE & DOMAIN INSTANCE
// ==============================================================================

const bank = new Bank();
let currentAction = 'create-savings';

// Seed initial educational sample data
function seedInitialData() {
  Account.nextAccountNumber = 1001;
  bank.accounts = [];
  
  const a1 = new SavingsAccount('Purva Deshmukh', 15000);
  const a2 = new CurrentAccount('Tech Innovations Ltd', 35000);
  const a3 = new SavingsAccount('Aarav Mehta', 8200);

  bank.addAccount(a1);
  bank.addAccount(a2);
  bank.addAccount(a3);
}

// ==============================================================================
// 4. UI RENDERERS & EVENT HANDLERS
// ==============================================================================

function updateMetrics() {
  document.getElementById('statAccountCount').textContent = bank.getAccountCount();
  document.getElementById('statTotalBalance').textContent = `$${bank.getTotalBankBalance().toLocaleString('en-US', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;
  document.getElementById('statNextAccount').textContent = Account.nextAccountNumber;
}

function updateConceptPanel(actionKey) {
  const concept = OOP_CONCEPTS[actionKey] || OOP_CONCEPTS['create-savings'];
  document.getElementById('conceptBadge').textContent = concept.badge;
  document.getElementById('conceptFile').textContent = concept.file;
  document.getElementById('conceptTitle').textContent = concept.title;
  document.getElementById('conceptDesc').textContent = concept.desc;
  document.getElementById('codeSnippet').textContent = concept.snippet;
}

function renderActionForm(action) {
  const container = document.getElementById('actionFormContainer');
  let html = '';

  const getAccountOptions = () => {
    if (bank.accounts.length === 0) {
      return '<option value="">No accounts exist. Create one first!</option>';
    }
    return bank.accounts
      .map(a => `<option value="${a.getAccountNumber()}">#${a.getAccountNumber()} - ${a.getName()} (${a.getAccountType()} | $${a.getBalance().toFixed(2)})</option>`)
      .join('');
  };

  if (action === 'create-savings') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Account Holder's Full Name</label>
          <input type="text" class="form-control" id="inpName" placeholder="e.g. Purva Deshmukh" required>
        </div>
        <div class="form-group">
          <label>Initial Opening Balance ($)</label>
          <input type="number" step="0.01" min="0" class="form-control" id="inpBalance" placeholder="e.g. 5000" required>
          <span class="form-hint">Savings Account earns 4.0% annual interest</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Open Savings Account</button>
        </div>
      </form>
    `;
  } else if (action === 'create-current') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Account Holder / Entity Name</label>
          <input type="text" class="form-control" id="inpName" placeholder="e.g. Global Tech Ltd" required>
        </div>
        <div class="form-group">
          <label>Initial Opening Balance ($)</label>
          <input type="number" step="0.01" min="0" class="form-control" id="inpBalance" placeholder="e.g. 15000" required>
          <span class="form-hint">Current Account: 0% interest checking account for commercial operations</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Open Current Account</button>
        </div>
      </form>
    `;
  } else if (action === 'deposit') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Select Target Account</label>
          <select class="form-control" id="selAccount" required>
            ${getAccountOptions()}
          </select>
        </div>
        <div class="form-group">
          <label>Deposit Amount ($)</label>
          <input type="number" step="0.01" min="0.01" class="form-control" id="inpAmount" placeholder="e.g. 2500" required>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Process Deposit</button>
        </div>
      </form>
    `;
  } else if (action === 'withdraw') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Select Source Account</label>
          <select class="form-control" id="selAccount" required>
            ${getAccountOptions()}
          </select>
        </div>
        <div class="form-group">
          <label>Withdrawal Amount ($)</label>
          <input type="number" step="0.01" min="0.01" class="form-control" id="inpAmount" placeholder="e.g. 1000" required>
          <span class="form-hint">Throws InsufficientBalanceException if amount exceeds current balance</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Process Withdrawal</button>
        </div>
      </form>
    `;
  } else if (action === 'calculate-interest') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Select Account for Polymorphic Interest Calculation</label>
          <select class="form-control" id="selAccount" required>
            ${getAccountOptions()}
          </select>
          <span class="form-hint">Dynamically invokes calculateInterest() based on derived type</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Calculate Projected Interest</button>
        </div>
      </form>
    `;
  } else if (action === 'save-file') {
    html = `
      <div>
        <p style="font-size:0.85rem; color:var(--text-secondary); margin-bottom:1rem;">
          Exports in-memory records from <code>vector&lt;Account*&gt;</code> into formatted text file <code>accounts.txt</code> using <code>std::ofstream</code>.
        </p>
        <div class="form-actions">
          <button type="button" class="btn btn-primary" id="btnExportModal">View accounts.txt File</button>
          <button type="button" class="btn btn-secondary" id="btnDownloadDirect">Direct Download</button>
        </div>
      </div>
    `;
  }

  container.innerHTML = html;

  const form = document.getElementById('formAction');
  if (form) {
    form.addEventListener('submit', handleFormSubmit);
  }

  const btnModal = document.getElementById('btnExportModal');
  if (btnModal) {
    btnModal.addEventListener('click', openFileModal);
  }
  const btnDownload = document.getElementById('btnDownloadDirect');
  if (btnDownload) {
    btnDownload.addEventListener('click', downloadAccountsFile);
  }
}

function renderTable() {
  const tbody = document.getElementById('accountsTableBody');
  if (bank.accounts.length === 0) {
    tbody.innerHTML = `
      <tr class="empty-row">
        <td colspan="6">No accounts registered yet. Use the operations above or click "Load Sample Data".</td>
      </tr>
    `;
    return;
  }

  tbody.innerHTML = bank.accounts.map(acc => {
    const isSavings = acc instanceof SavingsAccount;
    const typeClass = isSavings ? 'type-savings' : 'type-current';
    const rateText = isSavings ? '4.0% p.a.' : '0.0% (N/A)';

    return `
      <tr>
        <td class="account-num-text">${acc.getAccountNumber()}</td>
        <td><strong>${escapeHtml(acc.getName())}</strong></td>
        <td><span class="type-pill ${typeClass}">${acc.getAccountType()}</span></td>
        <td>${rateText}</td>
        <td class="balance-text">$${acc.getBalance().toFixed(2)}</td>
        <td>
          <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('deposit', ${acc.getAccountNumber()})">Deposit</button>
          <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('withdraw', ${acc.getAccountNumber()})">Withdraw</button>
          <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('calculate-interest', ${acc.getAccountNumber()})">Interest</button>
        </td>
      </tr>
    `;
  }).join('');
}

function quickSelectAction(action, accNum) {
  switchAction(action);
  setTimeout(() => {
    const sel = document.getElementById('selAccount');
    if (sel) sel.value = String(accNum);
  }, 10);
}

function switchAction(action) {
  currentAction = action;
  document.querySelectorAll('.action-btn').forEach(btn => {
    btn.classList.toggle('active', btn.getAttribute('data-action') === action);
  });
  renderActionForm(action);
  updateConceptPanel(action);
}

// ==============================================================================
// 5. TRANSACTION HANDLER & TERMINAL EMULATION
// ==============================================================================

function handleFormSubmit(e) {
  e.preventDefault();

  try {
    if (currentAction === 'create-savings') {
      const name = document.getElementById('inpName').value.trim();
      const balance = parseFloat(document.getElementById('inpBalance').value);
      if (!name) throw new Error('Account holder name is required.');
      if (isNaN(balance) || balance < 0) throw new InvalidAmountException('Opening balance must be non-negative.');

      const acc = new SavingsAccount(name, balance);
      bank.addAccount(acc);

      showReceipt({
        title: 'ACCOUNT CREATED (SAVINGS)',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'NEW ACCOUNT OPENING',
        prevBal: 0,
        amount: balance,
        newBal: balance
      });

      logToTerminal(`[SUCCESS] Savings Account #${acc.getAccountNumber()} opened for "${name}" with initial deposit $${balance.toFixed(2)}`);
      renderActionForm(currentAction);

    } else if (currentAction === 'create-current') {
      const name = document.getElementById('inpName').value.trim();
      const balance = parseFloat(document.getElementById('inpBalance').value);
      if (!name) throw new Error('Account holder name is required.');
      if (isNaN(balance) || balance < 0) throw new InvalidAmountException('Opening balance must be non-negative.');

      const acc = new CurrentAccount(name, balance);
      bank.addAccount(acc);

      showReceipt({
        title: 'ACCOUNT CREATED (CURRENT)',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'NEW ACCOUNT OPENING',
        prevBal: 0,
        amount: balance,
        newBal: balance
      });

      logToTerminal(`[SUCCESS] Current Account #${acc.getAccountNumber()} opened for "${name}" with initial deposit $${balance.toFixed(2)}`);
      renderActionForm(currentAction);

    } else if (currentAction === 'deposit') {
      const accNum = document.getElementById('selAccount').value;
      const amount = parseFloat(document.getElementById('inpAmount').value);
      const acc = bank.findAccount(accNum);
      const prevBal = acc.getBalance();

      acc.deposit(amount);
      const newBal = acc.getBalance();

      showReceipt({
        title: 'TRANSACTION RECEIPT',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'DEPOSIT',
        prevBal: prevBal,
        amount: amount,
        newBal: newBal
      });

      logToTerminal(`[DEPOSIT] Credited $${amount.toFixed(2)} to Account #${acc.getAccountNumber()}. New Balance: $${newBal.toFixed(2)}`);

    } else if (currentAction === 'withdraw') {
      const accNum = document.getElementById('selAccount').value;
      const amount = parseFloat(document.getElementById('inpAmount').value);
      const acc = bank.findAccount(accNum);
      const prevBal = acc.getBalance();

      acc.withdraw(amount);
      const newBal = acc.getBalance();

      showReceipt({
        title: 'TRANSACTION RECEIPT',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'WITHDRAWAL',
        prevBal: prevBal,
        amount: amount,
        newBal: newBal
      });

      logToTerminal(`[WITHDRAWAL] Debited $${amount.toFixed(2)} from Account #${acc.getAccountNumber()}. New Balance: $${newBal.toFixed(2)}`);

    } else if (currentAction === 'calculate-interest') {
      const accNum = document.getElementById('selAccount').value;
      const acc = bank.findAccount(accNum);
      const result = acc.calculateInterest();

      showInterestCard(acc, result);
      logToTerminal(`[INTEREST] Account #${acc.getAccountNumber()} (${acc.getAccountType()}): Rate = ${result.ratePercent}%, Earned = $${result.interest.toFixed(2)}, Projected = $${result.projectedBalance.toFixed(2)}`);
    }

    updateMetrics();
    renderTable();

  } catch (err) {
    showErrorReceipt(err.name || 'Error', err.message);
    logToTerminal(`[EXCEPTION] ${err.name}: ${err.message}`, 'error');
  }
}

function showReceipt(data) {
  const container = document.getElementById('receiptContainer');
  container.innerHTML = `
    <div class="receipt-box ${data.type}">
      <div class="receipt-header-row">
        <span class="receipt-title ${data.type}">${data.title}</span>
        <span class="account-num-text">#${data.accNum}</span>
      </div>
      <div class="receipt-grid">
        <span class="receipt-item-label">Account Holder</span>
        <span class="receipt-item-val">${escapeHtml(data.holder)}</span>

        <span class="receipt-item-label">Account Type</span>
        <span class="receipt-item-val">${data.accType}</span>

        <span class="receipt-item-label">Transaction Type</span>
        <span class="receipt-item-val">${data.txType}</span>

        <span class="receipt-item-label">Previous Balance</span>
        <span class="receipt-item-val">$${data.prevBal.toFixed(2)}</span>

        <span class="receipt-item-label">Transaction Amount</span>
        <span class="receipt-item-val" style="color:${data.txType === 'DEPOSIT' ? 'var(--accent-green)' : 'var(--accent-red)'}">$${data.amount.toFixed(2)}</span>

        <span class="receipt-item-label">Updated Balance</span>
        <span class="receipt-item-val" style="color:var(--accent-green);">$${data.newBal.toFixed(2)}</span>
      </div>
    </div>
  `;
}

function showInterestCard(acc, res) {
  const container = document.getElementById('receiptContainer');
  container.innerHTML = `
    <div class="receipt-box interest">
      <div class="receipt-header-row">
        <span class="receipt-title interest">INTEREST CALCULATION SUMMARY</span>
        <span class="account-num-text">#${acc.getAccountNumber()}</span>
      </div>
      <div class="receipt-grid">
        <span class="receipt-item-label">Account Holder</span>
        <span class="receipt-item-val">${escapeHtml(acc.getName())}</span>

        <span class="receipt-item-label">Account Type</span>
        <span class="receipt-item-val">${acc.getAccountType()}</span>

        <span class="receipt-item-label">Annual Rate</span>
        <span class="receipt-item-val">${res.ratePercent.toFixed(1)}% p.a.</span>

        <span class="receipt-item-label">Principal Balance</span>
        <span class="receipt-item-val">$${res.principal.toFixed(2)}</span>

        <span class="receipt-item-label">Annual Interest Earned</span>
        <span class="receipt-item-val" style="color:var(--accent-magenta);">$${res.interest.toFixed(2)}</span>

        <span class="receipt-item-label">Projected Balance (1 Yr)</span>
        <span class="receipt-item-val" style="color:var(--accent-green);">$${res.projectedBalance.toFixed(2)}</span>
      </div>
      <div style="margin-top:0.75rem; font-size:0.75rem; color:var(--text-secondary); border-top:1px dashed var(--border-color); padding-top:0.5rem;">
        ${res.note}
      </div>
    </div>
  `;
}

function showErrorReceipt(title, message) {
  const container = document.getElementById('receiptContainer');
  container.innerHTML = `
    <div class="receipt-box error">
      <div class="receipt-header-row">
        <span class="receipt-title error">${title}</span>
        <span style="font-size:0.75rem; color:var(--accent-red);">C++ Exception</span>
      </div>
      <p style="font-size:0.85rem; color:#f87171; margin-top:0.35rem;">
        ${escapeHtml(message)}
      </p>
    </div>
  `;
}

function logToTerminal(message, type = 'info') {
  const term = document.getElementById('terminalOutput');
  const time = new Date().toLocaleTimeString();
  let colorClass = 't-cyan';
  if (type === 'error') colorClass = 't-red';
  else if (message.includes('SUCCESS') || message.includes('DEPOSIT')) colorClass = 't-green';
  else if (message.includes('INTEREST')) colorClass = 't-magenta';

  const line = document.createElement('div');
  line.className = 'terminal-line';
  line.innerHTML = `<span class="t-gray">[${time}]</span> <span class="${colorClass}">${escapeHtml(message)}</span>`;
  term.appendChild(line);
  term.scrollTop = term.scrollHeight;
}

// ==============================================================================
// 6. FILE EXPORT MODAL & DOWNLOAD
// ==============================================================================

function openFileModal() {
  const content = bank.exportTextReport();
  document.getElementById('filePreviewContent').textContent = content;
  document.getElementById('fileModal').classList.add('active');
}

function closeFileModal() {
  document.getElementById('fileModal').classList.remove('active');
}

function downloadAccountsFile() {
  const content = bank.exportTextReport();
  const blob = new Blob([content], { type: 'text/plain;charset=utf-8' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'accounts.txt';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}

// ==============================================================================
// 7. SOURCE CODE EXPLORER DATABASE
// ==============================================================================

const SOURCE_FILES = {
  'Account.h': `// Account.h - Abstract Base Class
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account
{
private:
    int accountNumber;
    std::string name;
    double balance;

    static int nextAccountNumber;

public:
    Account(const std::string& n, double b);
    virtual ~Account();

    void deposit(double amount);
    void withdraw(double amount);

    virtual void calculateInterest() = 0;
    virtual std::string getAccountType() const = 0;
    virtual double getInterestRate() const = 0;

    virtual void display() const;

    int getAccountNumber() const;
    std::string getName() const;
    double getBalance() const;

    static int getNextAccountNumber();
};

#endif`,

  'Account.cpp': `// Account.cpp - Account Base Implementations
#include "Account.h"
#include "Exceptions.h"
#include "UIHelper.h"

int Account::nextAccountNumber = 1001;

Account::Account(const std::string& n, double b)
{
    this->name = n;
    this->balance = b;
    this->accountNumber = nextAccountNumber;
    nextAccountNumber++;
}

Account::~Account() {}

void Account::deposit(double amount)
{
    if (amount <= 0) {
        throw InvalidAmountException("Deposit amount must be greater than zero.");
    }
    balance = balance + amount;
}

void Account::withdraw(double amount)
{
    if (amount <= 0) {
        throw InvalidAmountException("Withdrawal amount must be greater than zero.");
    }
    if (amount > balance) {
        throw InsufficientBalanceException(balance, amount);
    }
    balance = balance - amount;
}

int Account::getAccountNumber() const { return accountNumber; }
std::string Account::getName() const { return name; }
double Account::getBalance() const { return balance; }`,

  'SavingsAccount.h': `// SavingsAccount.h - Derived Savings Class
#ifndef SAVINGSACCOUNT_H
#define SAVINGSACCOUNT_H

#include "Account.h"

class SavingsAccount : public Account
{
public:
    SavingsAccount(const std::string& n, double b);

    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
};

#endif`,

  'SavingsAccount.cpp': `// SavingsAccount.cpp - Savings Interest (4%)
#include "SavingsAccount.h"
#include "UIHelper.h"

SavingsAccount::SavingsAccount(const std::string& n, double b)
    : Account(n, b)
{
}

void SavingsAccount::calculateInterest()
{
    const double interestRate = 0.04;
    double interest = getBalance() * interestRate;
    double projectedBalance = getBalance() + interest;

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
}

std::string SavingsAccount::getAccountType() const { return "Savings Account"; }
double SavingsAccount::getInterestRate() const { return 0.04; }`,

  'CurrentAccount.h': `// CurrentAccount.h - Derived Current Account
#ifndef CURRENTACCOUNT_H
#define CURRENTACCOUNT_H

#include "Account.h"

class CurrentAccount : public Account
{
public:
    CurrentAccount(const std::string& n, double b);

    void calculateInterest() override;
    std::string getAccountType() const override;
    double getInterestRate() const override;
};

#endif`,

  'CurrentAccount.cpp': `// CurrentAccount.cpp - Current Account (0% Interest)
#include "CurrentAccount.h"
#include "UIHelper.h"

CurrentAccount::CurrentAccount(const std::string& n, double b)
    : Account(n, b)
{
}

void CurrentAccount::calculateInterest()
{
    const double interestRate = 0.0;
    double interest = 0.0;
    double projectedBalance = getBalance();

    UI::printInterestCard(getAccountNumber(), getName(), getAccountType(), 
                          interestRate, getBalance(), interest, projectedBalance);
}

std::string CurrentAccount::getAccountType() const { return "Current Account"; }
double CurrentAccount::getInterestRate() const { return 0.0; }`,

  'Bank.h': `// Bank.h - Bank Container Class
#ifndef BANK_H
#define BANK_H

#include <vector>
#include "Account.h"

class Bank
{
private:
    std::vector<Account*> accounts;

public:
    ~Bank();

    void addAccount(Account* account);
    Account* findAccount(int accountNumber);
    void displayAllAccounts() const;
    void saveAccounts() const;

    size_t getAccountCount() const;
    double getTotalBankBalance() const;
};

#endif`,

  'Bank.cpp': `// Bank.cpp - Bank Memory & File Operations
#include "Bank.h"
#include "Exceptions.h"
#include <fstream>

Bank::~Bank()
{
    for (Account* account : accounts) {
        delete account;
    }
    accounts.clear();
}

void Bank::addAccount(Account* account) {
    accounts.push_back(account);
}

Account* Bank::findAccount(int accountNumber) {
    for (Account* account : accounts) {
        if (account->getAccountNumber() == accountNumber) {
            return account;
        }
    }
    throw AccountNotFoundException(accountNumber);
}

void Bank::saveAccounts() const {
    std::ofstream file("accounts.txt");
    for (const Account* account : accounts) {
        file << "Account Number: " << account->getAccountNumber() << "\\n";
        file << "Balance: " << account->getBalance() << "\\n";
    }
    file.close();
}`,

  'Exceptions.h': `// Exceptions.h - Custom Banking Exception Hierarchy
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class BankingException : public std::exception
{
protected:
    std::string message;
public:
    explicit BankingException(const std::string& msg) : message(msg) {}
    virtual const char* what() const noexcept override { return message.c_str(); }
};

class InvalidAmountException : public BankingException {
public:
    InvalidAmountException(const std::string& msg = "Invalid amount!")
        : BankingException(msg) {}
};

class InsufficientBalanceException : public BankingException {
public:
    InsufficientBalanceException(double currentBal, double reqAmt)
        : BankingException("Insufficient balance! Available: $" + std::to_string(currentBal)) {}
};

class AccountNotFoundException : public BankingException {
public:
    AccountNotFoundException(int accNum)
        : BankingException("Account #" + std::to_string(accNum) + " not found.") {}
};

#endif`,

  'UIHelper.h': `// UIHelper.h - Terminal Styling & Validation
#ifndef UIHELPER_H
#define UIHELPER_H

#include <string>

namespace UI
{
    void initTerminal();
    void clearScreen();
    void pauseScreen();
    void printSuccess(const std::string& msg);
    void printError(const std::string& msg);
    std::string formatCurrency(double amount);
    std::string getString(const std::string& prompt);
    int getInt(const std::string& prompt, int minVal, int maxVal);
    double getDouble(const std::string& prompt, double minVal);
}

#endif`,

  'UIHelper.cpp': `// UIHelper.cpp - ANSI Colors & Safe Streams
#include "UIHelper.h"
#include <iostream>

namespace UI {
    void printSuccess(const std::string& msg) {
        std::cout << "\\033[1;32m[SUCCESS] " << msg << "\\033[0m\\n";
    }
    void printError(const std::string& msg) {
        std::cout << "\\033[1;31m[ERROR] " << msg << "\\033[0m\\n";
    }
}`,

  'main.cpp': `// main.cpp - Bank System Controller
#include "Bank.h"
#include "SavingsAccount.h"
#include "CurrentAccount.h"
#include "Exceptions.h"
#include "UIHelper.h"

int main()
{
    UI::initTerminal();
    Bank bank;
    int choice = 0;

    do {
        // Render menu and prompt user
        choice = UI::getInt("Enter your choice (1-10): ", 1, 10);
        // Switch execution with try-catch blocks
    } while (choice != 10);

    return 0;
}`
};

function setupSourceExplorer() {
  const fileItems = document.querySelectorAll('.file-item');
  const codeEl = document.getElementById('fullSourceCode');
  const titleEl = document.getElementById('currentViewingFile');

  fileItems.forEach(item => {
    item.addEventListener('click', () => {
      fileItems.forEach(i => i.classList.remove('active'));
      item.classList.add('active');
      const filename = item.getAttribute('data-file');
      titleEl.textContent = filename;
      codeEl.textContent = SOURCE_FILES[filename] || '// Source not available';
    });
  });

  // Load first file
  codeEl.textContent = SOURCE_FILES['Account.h'];
}

// ==============================================================================
// 8. TABS, CLIPBOARD, AND INITIALIZATION
// ==============================================================================

function setupTabs() {
  const links = document.querySelectorAll('.nav-link[data-tab]');
  links.forEach(link => {
    link.addEventListener('click', e => {
      e.preventDefault();
      links.forEach(l => l.classList.remove('active'));
      link.classList.add('active');

      const targetTab = link.getAttribute('data-tab');
      document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
      const activePane = document.getElementById(`tab-${targetTab}`);
      if (activePane) activePane.classList.add('active');
    });
  });
}

function setupCopyButtons() {
  document.getElementById('btnCopySnippet').addEventListener('click', () => {
    const text = document.getElementById('codeSnippet').textContent;
    navigator.clipboard.writeText(text).then(() => {
      const btn = document.getElementById('btnCopySnippet');
      btn.textContent = 'Copied!';
      setTimeout(() => btn.textContent = 'Copy', 1500);
    });
  });

  document.getElementById('btnCopyFullSource').addEventListener('click', () => {
    const text = document.getElementById('fullSourceCode').textContent;
    navigator.clipboard.writeText(text).then(() => {
      const btn = document.getElementById('btnCopyFullSource');
      btn.textContent = 'Copied!';
      setTimeout(() => btn.textContent = 'Copy File', 1500);
    });
  });
}

function escapeHtml(str) {
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#039;');
}

// Global initialization
document.addEventListener('DOMContentLoaded', () => {
  seedInitialData();
  setupTabs();
  setupSourceExplorer();
  setupCopyButtons();

  // Action button switchers
  document.querySelectorAll('.action-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      switchAction(btn.getAttribute('data-action'));
    });
  });

  // Modal events
  document.getElementById('btnCloseModal').addEventListener('click', closeFileModal);
  document.getElementById('btnCloseModalBtn').addEventListener('click', closeFileModal);
  document.getElementById('btnDownloadFile').addEventListener('click', downloadAccountsFile);

  // Table header buttons
  document.getElementById('btnSeedDemo').addEventListener('click', () => {
    seedInitialData();
    updateMetrics();
    renderTable();
    logToTerminal('[SYSTEM] Reloaded sample account records into vector<Account*>');
  });

  document.getElementById('btnClearBank').addEventListener('click', () => {
    bank.accounts = [];
    Account.nextAccountNumber = 1001;
    updateMetrics();
    renderTable();
    logToTerminal('[SYSTEM] Cleared all account objects. Memory deallocated.');
  });

  document.getElementById('btnClearTerminal').addEventListener('click', () => {
    document.getElementById('terminalOutput').innerHTML = '';
  });

  // Initial renders
  switchAction('create-savings');
  updateMetrics();
  renderTable();
  logToTerminal('+====================================================================+');
  logToTerminal('|                 BANK ACCOUNT MANAGEMENT SYSTEM                     |');
  logToTerminal('|              C++ Object-Oriented Programming Project               |');
  logToTerminal('+====================================================================+');
  logToTerminal('[READY] System initialized with 3 sample accounts. Ready for operations.');
});
