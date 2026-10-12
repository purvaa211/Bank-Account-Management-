/**
 * ==============================================================================
 * Bank Account Management System - Clean Interactive Banking Engine
 * Author: Purva Deshmukh
 * Course: Object-Oriented Programming (OOP) in C++
 * ==============================================================================
 */

// Custom Exception Classes (mirroring C++ domain exceptions)
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

// Abstract Base Class: Account
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

// Derived Class: SavingsAccount (4% annual interest)
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

// Derived Class: CurrentAccount (0% checking account)
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
      note: 'Current accounts are commercial transactional accounts with 0% interest.'
    };
  }

  getAccountType() {
    return 'Current Account';
  }

  getInterestRate() {
    return 0.0;
  }
}

// Container Class: Bank (manages collection of accounts)
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

  getSavingsCount() {
    return this.accounts.filter(a => a instanceof SavingsAccount).length;
  }

  getCurrentCount() {
    return this.accounts.filter(a => a instanceof CurrentAccount).length;
  }
}

// Global Bank Instance
const bank = new Bank();
let currentAction = 'create-savings';

// Seed Sample Accounts
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

// Update Top Metrics Banner
function updateMetrics() {
  document.getElementById('statAccountCount').textContent = bank.getAccountCount();
  document.getElementById('statTotalBalance').textContent = `$${bank.getTotalBankBalance().toLocaleString('en-US', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`;
  document.getElementById('statSavingsCount').textContent = bank.getSavingsCount();
  document.getElementById('statCurrentCount').textContent = bank.getCurrentCount();
}

// Render Active Operation Form
function renderActionForm(action) {
  const container = document.getElementById('actionFormContainer');
  let html = '';

  const getAccountOptions = () => {
    if (bank.accounts.length === 0) {
      return '<option value="">No accounts exist. Please create one first.</option>';
    }
    return bank.accounts
      .map(a => `<option value="${a.getAccountNumber()}">#${a.getAccountNumber()} - ${escapeHtml(a.getName())} (${a.getAccountType()} | $${a.getBalance().toFixed(2)})</option>`)
      .join('');
  };

  if (action === 'create-savings') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Account Holder Full Name</label>
          <input type="text" class="form-control" id="inpName" placeholder="Enter account holder name (e.g. Purva Deshmukh)" required>
        </div>
        <div class="form-group">
          <label>Initial Opening Balance ($)</label>
          <input type="number" step="0.01" min="0" class="form-control" id="inpBalance" placeholder="Enter opening balance (e.g. 5000)" required>
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
          <input type="text" class="form-control" id="inpName" placeholder="Enter commercial / entity name (e.g. Global Tech Ltd)" required>
        </div>
        <div class="form-group">
          <label>Initial Opening Balance ($)</label>
          <input type="number" step="0.01" min="0" class="form-control" id="inpBalance" placeholder="Enter opening balance (e.g. 15000)" required>
          <span class="form-hint">Current Account is designed for commercial transactions with 0% interest</span>
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
          <input type="number" step="0.01" min="0.01" class="form-control" id="inpAmount" placeholder="Enter deposit amount (e.g. 2500)" required>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Deposit Funds</button>
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
          <input type="number" step="0.01" min="0.01" class="form-control" id="inpAmount" placeholder="Enter withdrawal amount (e.g. 1000)" required>
          <span class="form-hint">Withdrawal amount must not exceed the available balance</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Withdraw Funds</button>
        </div>
      </form>
    `;
  } else if (action === 'balance-inquiry') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Select Account to Check Balance</label>
          <select class="form-control" id="selAccount" required>
            ${getAccountOptions()}
          </select>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Check Balance</button>
        </div>
      </form>
    `;
  } else if (action === 'calculate-interest') {
    html = `
      <form id="formAction">
        <div class="form-group">
          <label>Select Account for Annual Interest Calculation</label>
          <select class="form-control" id="selAccount" required>
            ${getAccountOptions()}
          </select>
          <span class="form-hint">Calculates 4% interest for Savings Accounts and 0% for Current Accounts</span>
        </div>
        <div class="form-actions">
          <button type="submit" class="btn btn-primary">Calculate Projected Interest</button>
        </div>
      </form>
    `;
  }

  container.innerHTML = html;

  const form = document.getElementById('formAction');
  if (form) {
    form.addEventListener('submit', handleFormSubmit);
  }
}

// Render Accounts Table
function renderTable() {
  const tbody = document.getElementById('accountsTableBody');
  if (bank.accounts.length === 0) {
    tbody.innerHTML = `
      <tr class="empty-row">
        <td colspan="6">No accounts registered yet. Use the operations form above or click "Load Sample Accounts".</td>
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
        <td class="account-num-text">#${acc.getAccountNumber()}</td>
        <td><strong>${escapeHtml(acc.getName())}</strong></td>
        <td><span class="type-pill ${typeClass}">${acc.getAccountType()}</span></td>
        <td>${rateText}</td>
        <td class="balance-text">$${acc.getBalance().toFixed(2)}</td>
        <td>
          <div class="action-btn-group">
            <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('deposit', ${acc.getAccountNumber()})">Deposit</button>
            <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('withdraw', ${acc.getAccountNumber()})">Withdraw</button>
            <button class="btn btn-secondary btn-sm" onclick="quickSelectAction('calculate-interest', ${acc.getAccountNumber()})">Interest</button>
          </div>
        </td>
      </tr>
    `;
  }).join('');
}

// Quick action from table rows
function quickSelectAction(action, accNum) {
  switchAction(action);
  setTimeout(() => {
    const sel = document.getElementById('selAccount');
    if (sel) sel.value = String(accNum);
  }, 10);
}

// Switch between action tabs
function switchAction(action) {
  currentAction = action;
  document.querySelectorAll('.action-btn').forEach(btn => {
    btn.classList.toggle('active', btn.getAttribute('data-action') === action);
  });
  renderActionForm(action);
}

// Transaction Submissions
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

      renderActionForm(currentAction);

    } else if (currentAction === 'deposit') {
      const accNum = document.getElementById('selAccount').value;
      const amount = parseFloat(document.getElementById('inpAmount').value);
      const acc = bank.findAccount(accNum);
      const prevBal = acc.getBalance();

      acc.deposit(amount);
      const newBal = acc.getBalance();

      showReceipt({
        title: 'DEPOSIT TRANSACTION RECEIPT',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'DEPOSIT',
        prevBal: prevBal,
        amount: amount,
        newBal: newBal
      });

    } else if (currentAction === 'withdraw') {
      const accNum = document.getElementById('selAccount').value;
      const amount = parseFloat(document.getElementById('inpAmount').value);
      const acc = bank.findAccount(accNum);
      const prevBal = acc.getBalance();

      acc.withdraw(amount);
      const newBal = acc.getBalance();

      showReceipt({
        title: 'WITHDRAWAL TRANSACTION RECEIPT',
        type: 'success',
        accNum: acc.getAccountNumber(),
        holder: acc.getName(),
        accType: acc.getAccountType(),
        txType: 'WITHDRAWAL',
        prevBal: prevBal,
        amount: amount,
        newBal: newBal
      });

    } else if (currentAction === 'balance-inquiry') {
      const accNum = document.getElementById('selAccount').value;
      const acc = bank.findAccount(accNum);

      showBalanceReceipt(acc);

    } else if (currentAction === 'calculate-interest') {
      const accNum = document.getElementById('selAccount').value;
      const acc = bank.findAccount(accNum);
      const result = acc.calculateInterest();

      showInterestCard(acc, result);
    }

    updateMetrics();
    renderTable();

  } catch (err) {
    showErrorReceipt(err.name || 'Transaction Error', err.message);
  }
}

// Receipt Rendering Functions
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
        <span class="receipt-item-val" style="color:${data.txType === 'DEPOSIT' ? 'var(--color-success)' : 'var(--color-danger)'}">$${data.amount.toFixed(2)}</span>

        <span class="receipt-item-label">Updated Balance</span>
        <span class="receipt-item-val" style="color:var(--color-success);">$${data.newBal.toFixed(2)}</span>
      </div>
    </div>
  `;
}

function showBalanceReceipt(acc) {
  const container = document.getElementById('receiptContainer');
  container.innerHTML = `
    <div class="receipt-box inquiry">
      <div class="receipt-header-row">
        <span class="receipt-title inquiry">BALANCE INQUIRY PASSBOOK</span>
        <span class="account-num-text">#${acc.getAccountNumber()}</span>
      </div>
      <div class="receipt-grid">
        <span class="receipt-item-label">Account Holder</span>
        <span class="receipt-item-val">${escapeHtml(acc.getName())}</span>

        <span class="receipt-item-label">Account Type</span>
        <span class="receipt-item-val">${acc.getAccountType()}</span>

        <span class="receipt-item-label">Interest Rate</span>
        <span class="receipt-item-val">${(acc.getInterestRate() * 100).toFixed(1)}% p.a.</span>

        <span class="receipt-item-label">Available Balance</span>
        <span class="receipt-item-val" style="color:var(--color-success); font-size:1.05rem;">$${acc.getBalance().toFixed(2)}</span>
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

        <span class="receipt-item-label">Annual Interest Rate</span>
        <span class="receipt-item-val">${res.ratePercent.toFixed(1)}% per annum</span>

        <span class="receipt-item-label">Current Principal</span>
        <span class="receipt-item-val">$${res.principal.toFixed(2)}</span>

        <span class="receipt-item-label">Annual Interest Earned</span>
        <span class="receipt-item-val" style="color:var(--color-purple);">$${res.interest.toFixed(2)}</span>

        <span class="receipt-item-label">Projected Balance (1 Yr)</span>
        <span class="receipt-item-val" style="color:var(--color-success); font-size:1.05rem;">$${res.projectedBalance.toFixed(2)}</span>
      </div>
      <div style="margin-top:0.85rem; font-size:0.78rem; color:var(--text-secondary); border-top:1px dashed var(--border-light); padding-top:0.5rem;">
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
        <span style="font-size:0.75rem; color:var(--color-danger); font-weight:600;">Transaction Failed</span>
      </div>
      <p style="font-size:0.85rem; color:var(--color-danger); margin-top:0.35rem;">
        ${escapeHtml(message)}
      </p>
    </div>
  `;
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

  // Action button clicks
  document.querySelectorAll('.action-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      switchAction(btn.getAttribute('data-action'));
    });
  });

  // Table sample data & reset buttons
  document.getElementById('btnSeedDemo').addEventListener('click', () => {
    seedInitialData();
    updateMetrics();
    renderTable();
  });

  document.getElementById('btnClearBank').addEventListener('click', () => {
    bank.accounts = [];
    Account.nextAccountNumber = 1001;
    updateMetrics();
    renderTable();
    document.getElementById('receiptContainer').innerHTML = `
      <div class="receipt-placeholder">
        All accounts have been cleared. Select an operation to create a new account.
      </div>
    `;
  });

  // Initial render
  switchAction('create-savings');
  updateMetrics();
  renderTable();
});
