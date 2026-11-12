/**
 * ==============================================================================
 * SmartBank - Smart Banking & Transaction Security Management System
 * Core Frontend JavaScript Engine & OOP Simulation
 * Author: Purva Deshmukh
 * Course: Object-Oriented Programming (OOP) in C++
 * ==============================================================================
 */

// Format currency to Indian Rupees (₹) with Indian numbering system
function formatRupees(amount) {
  const num = Number(amount) || 0;
  return '₹' + num.toLocaleString('en-IN', { minimumFractionDigits: 2, maximumFractionDigits: 2 });
}

function formatDate(date) {
  const d = date ? new Date(date) : new Date();
  const year = d.getFullYear();
  const month = String(d.getMonth() + 1).padStart(2, '0');
  const day = String(d.getDate()).padStart(2, '0');
  return `${year}-${month}-${day}`;
}

function formatTimestamp(date) {
  const d = date ? new Date(date) : new Date();
  const dateStr = formatDate(d);
  const hours = String(d.getHours()).padStart(2, '0');
  const mins = String(d.getMinutes()).padStart(2, '0');
  const secs = String(d.getSeconds()).padStart(2, '0');
  return `${dateStr} ${hours}:${mins}:${secs}`;
}

function calculateFutureDate(months) {
  const d = new Date();
  d.setMonth(d.getMonth() + Number(months));
  return formatDate(d);
}

// Custom Domain Exceptions
class BankingException extends Error {
  constructor(message) {
    super(message);
    this.name = 'BankingException';
  }
}

class InvalidAmountException extends BankingException {
  constructor(msg = 'Invalid amount! Transaction amount must be strictly greater than zero.') {
    super(msg);
    this.name = 'InvalidAmountException';
  }
}

class InsufficientBalanceException extends BankingException {
  constructor(balance, requested) {
    super(`Insufficient balance! Available balance is ${formatRupees(balance)}, but requested withdrawal is ${formatRupees(requested)}.`);
    this.name = 'InsufficientBalanceException';
  }
}

class MinimumBalanceException extends BankingException {
  constructor(minBal, projectedBal) {
    super(`Transaction rejected! Savings account requires a minimum balance of ${formatRupees(minBal)}. Projected balance would be ${formatRupees(projectedBal)}.`);
    this.name = 'MinimumBalanceException';
  }
}

class OverdraftExceededException extends BankingException {
  constructor(maxAllowed, requested) {
    super(`Transaction rejected! Overdraft limit exceeded. Maximum allowable withdrawal is ${formatRupees(maxAllowed)}, but requested ${formatRupees(requested)}.`);
    this.name = 'OverdraftExceededException';
  }
}

class AccountNotFoundException extends BankingException {
  constructor(query) {
    super(`Account not found! No active bank record matches: ${query}`);
    this.name = 'AccountNotFoundException';
  }
}

class AccountClosedException extends BankingException {
  constructor(accNum) {
    super(`Operation rejected! Account #${accNum} is closed.`);
    this.name = 'AccountClosedException';
  }
}

// ==============================================================================
// OOP CLASS HIERARCHY (Mirroring C++ Backend Architecture)
// ==============================================================================

class Account {
  static nextAccountNumber = 1001;

  constructor(name, balance, date = '', status = 'ACTIVE', accNum = null) {
    if (new.target === Account) {
      throw new TypeError('Cannot construct abstract instance of Account directly.');
    }
    this.accountNumber = accNum ? Number(accNum) : Account.nextAccountNumber++;
    this.name = name;
    this.balance = Number(balance);
    this.status = status;
    this.openingDate = date || formatDate();
  }

  getAccountNumber() { return this.accountNumber; }
  getName() { return this.name; }
  getBalance() { return this.balance; }
  getStatus() { return this.status; }
  getOpeningDate() { return this.openingDate; }
  isActive() { return this.status === 'ACTIVE'; }

  deposit(amount) {
    if (!this.isActive()) throw new AccountClosedException(this.accountNumber);
    const amt = Number(amount);
    if (isNaN(amt) || amt <= 0) throw new InvalidAmountException();
    this.balance += amt;
    return this.balance;
  }

  withdraw(amount) {
    if (!this.isActive()) throw new AccountClosedException(this.accountNumber);
    const amt = Number(amount);
    if (isNaN(amt) || amt <= 0) throw new InvalidAmountException();
    let reason = '';
    if (!this.canWithdraw(amt, (r) => { reason = r; })) {
      throw new InsufficientBalanceException(this.balance, amt);
    }
    this.balance -= amt;
    return this.balance;
  }

  closeAccount() {
    if (this.status === 'CLOSED') {
      throw new BankingException(`Account #${this.accountNumber} is already closed.`);
    }
    if (this.balance < 0) {
      throw new BankingException(`Cannot close account #${this.accountNumber}! Outstanding overdraft of ${formatRupees(this.balance)} must be cleared first.`);
    }
    this.status = 'CLOSED';
  }

  // Pure virtual method signatures
  calculateInterest() { throw new Error('Abstract method calculateInterest() must be implemented.'); }
  getAccountType() { throw new Error('Abstract method getAccountType() must be implemented.'); }
  getInterestRate() { throw new Error('Abstract method getInterestRate() must be implemented.'); }
  canWithdraw(amount, setReason) { throw new Error('Abstract method canWithdraw() must be implemented.'); }
}

// Derived Class 1: SavingsAccount
class SavingsAccount extends Account {
  constructor(name, balance, date = '', minBal = 1000.0, status = 'ACTIVE', accNum = null) {
    super(name, balance, date, status, accNum);
    this.minimumBalance = Number(minBal);
    this.interestRate = 0.04;
  }

  getMinimumBalance() { return this.minimumBalance; }

  canWithdraw(amount, setReason) {
    if (amount <= 0) {
      if (setReason) setReason('Withdrawal amount must be greater than zero.');
      return false;
    }
    if ((this.balance - amount) < this.minimumBalance) {
      if (setReason) setReason(`Withdrawal breaches minimum balance threshold of ${formatRupees(this.minimumBalance)}.`);
      return false;
    }
    return true;
  }

  withdraw(amount) {
    const amt = Number(amount);
    if (!this.canWithdraw(amt)) {
      throw new MinimumBalanceException(this.minimumBalance, this.balance - amt);
    }
    return super.withdraw(amt);
  }

  calculateInterest() {
    const interest = this.balance * this.interestRate;
    return {
      ratePercent: 4.0,
      principal: this.balance,
      interest: interest,
      projectedBalance: this.balance + interest,
      note: 'Savings accounts accrue 4.0% annual interest with Rs. 1,000 minimum balance enforcement.'
    };
  }

  getAccountType() { return 'Savings Account'; }
  getInterestRate() { return this.interestRate; }
}

// Derived Class 2: CurrentAccount
class CurrentAccount extends Account {
  constructor(name, balance, date = '', overdraft = 25000.0, status = 'ACTIVE', accNum = null) {
    super(name, balance, date, status, accNum);
    this.overdraftLimit = Number(overdraft);
    this.interestRate = 0.0;
  }

  getOverdraftLimit() { return this.overdraftLimit; }

  canWithdraw(amount, setReason) {
    if (amount <= 0) return false;
    const maxWithdrawal = this.balance + this.overdraftLimit;
    if (amount > maxWithdrawal) {
      if (setReason) setReason(`Withdrawal exceeds maximum allowable overdraft limit of ${formatRupees(this.overdraftLimit)}.`);
      return false;
    }
    return true;
  }

  withdraw(amount) {
    const amt = Number(amount);
    if (!this.canWithdraw(amt)) {
      throw new OverdraftExceededException(this.balance + this.overdraftLimit, amt);
    }
    return super.withdraw(amt);
  }

  calculateInterest() {
    return {
      ratePercent: 0.0,
      principal: this.balance,
      interest: 0.0,
      projectedBalance: this.balance,
      note: `Commercial checking account with 0.0% interest and approved ${formatRupees(this.overdraftLimit)} overdraft facility.`
    };
  }

  getAccountType() { return 'Current Account'; }
  getInterestRate() { return 0.0; }
}

// Derived Class 3: FixedDepositAccount
class FixedDepositAccount extends Account {
  constructor(name, principal, term = 12, rate = 0.07, date = '', matAmt = null, matDate = null, status = 'ACTIVE', accNum = null) {
    super(name, principal, date, status, accNum);
    this.termMonths = Number(term);
    this.interestRate = Number(rate);
    this.maturityDate = matDate || calculateFutureDate(this.termMonths);
    this.maturityAmount = matAmt !== null ? Number(matAmt) : this.computeMaturity();
  }

  computeMaturity() {
    const t = this.termMonths / 12.0;
    // Quarterly compounding
    return this.balance * Math.pow(1.0 + (this.interestRate / 4.0), 4.0 * t);
  }

  getTermMonths() { return this.termMonths; }
  getMaturityAmount() { return this.maturityAmount; }
  getMaturityDate() { return this.maturityDate; }

  canWithdraw(amount) {
    return amount > 0 && amount <= this.balance;
  }

  withdraw(amount) {
    const res = super.withdraw(amount);
    this.maturityAmount = this.computeMaturity();
    return res;
  }

  calculateInterest() {
    const interest = this.maturityAmount - this.balance;
    return {
      ratePercent: this.interestRate * 100.0,
      principal: this.balance,
      interest: interest,
      projectedBalance: this.maturityAmount,
      maturityDate: this.maturityDate,
      termMonths: this.termMonths,
      note: `Fixed deposit locked for ${this.termMonths} months maturing on ${this.maturityDate}.`
    };
  }

  getAccountType() { return 'Fixed Deposit'; }
  getInterestRate() { return this.interestRate; }
}

// Transaction Model
class Transaction {
  static nextTransactionId = 10001;

  constructor(sender, receiver, type, amount, timestamp = null, status = 'SUCCESS', risk = 'LOW', reason = 'Normal transaction', id = null) {
    this.transactionId = id || `TXN-${Transaction.nextTransactionId++}`;
    this.senderAccount = Number(sender);
    this.receiverAccount = receiver !== null && receiver !== -1 ? Number(receiver) : -1;
    this.type = type; // DEPOSIT, WITHDRAWAL, TRANSFER, CLOSE_ACCOUNT
    this.amount = Number(amount);
    this.timestamp = timestamp || formatTimestamp();
    this.status = status; // SUCCESS, PENDING_REVIEW, REJECTED
    this.riskLevel = risk; // LOW, MEDIUM, HIGH
    this.flagReason = reason;
  }
}

// ==============================================================================
// RULE-BASED TRANSACTION SECURITY & FRAUD DETECTOR ENGINE
// ==============================================================================
class FraudDetector {
  static nextAlertId = 5001;

  constructor() {
    this.config = {
      largeTransactionThreshold: 50000.0,
      dailyDebitLimit: 100000.0,
      rapidWindowMinutes: 2,
      rapidMaxCount: 3
    };
    this.alerts = [];
  }

  getDailyDebitTotal(accountNumber, datePrefix, history) {
    return history
      .filter(tx => tx.senderAccount === accountNumber && 
                    (tx.type === 'WITHDRAWAL' || tx.type === 'TRANSFER') && 
                    tx.status !== 'REJECTED' && 
                    tx.timestamp.startsWith(datePrefix))
      .reduce((sum, tx) => sum + tx.amount, 0);
  }

  getRecentTransactionCount(accountNumber, history) {
    // Inspect up to last 10 entries for rapid bursts
    let count = 0;
    const now = new Date();
    for (let i = history.length - 1; i >= 0 && i >= history.length - 10; i--) {
      const tx = history[i];
      if (tx.senderAccount === accountNumber || tx.receiverAccount === accountNumber) {
        count++;
      }
    }
    return count;
  }

  evaluateTransaction(tx, history) {
    let risk = 'LOW';
    let reason = 'Normal transaction verified under standard security guidelines.';
    let reviewStatus = 'APPROVED';

    const amt = tx.amount;
    const acc = tx.senderAccount;
    const datePrefix = tx.timestamp.substring(0, 10);

    // Rule 1: Large Transaction Threshold Rule
    if (amt >= (this.config.largeTransactionThreshold * 2.0)) {
      risk = 'HIGH';
      reason = `Critical large transaction amount (${formatRupees(amt)}) exceeds high-risk threshold (${formatRupees(this.config.largeTransactionThreshold * 2.0)}).`;
      reviewStatus = 'PENDING_REVIEW';
    } else if (amt >= this.config.largeTransactionThreshold) {
      risk = 'MEDIUM';
      reason = `Large transaction amount (${formatRupees(amt)}) meets advisory review threshold (${formatRupees(this.config.largeTransactionThreshold)}).`;
      reviewStatus = 'PENDING_REVIEW';
    }

    // Rule 2: Daily Cumulative Debit Limit Rule
    if (tx.type === 'WITHDRAWAL' || tx.type === 'TRANSFER') {
      const pastDaily = this.getDailyDebitTotal(acc, datePrefix, history);
      if ((pastDaily + amt) > this.config.dailyDebitLimit) {
        risk = 'HIGH';
        reason = `Daily cumulative debit limit of ${formatRupees(this.config.dailyDebitLimit)} exceeded. Today's total debit reached ${formatRupees(pastDaily + amt)}.`;
        reviewStatus = 'PENDING_REVIEW';
      }
    }

    // Rule 3: Rapid Transaction Velocity Alert
    const recentCount = this.getRecentTransactionCount(acc, history);
    if (recentCount >= this.config.rapidMaxCount && risk === 'LOW') {
      risk = 'MEDIUM';
      reason = `Rapid transaction velocity: ${recentCount} consecutive transactions detected in short window.`;
      reviewStatus = 'PENDING_REVIEW';
    }

    const alertId = (risk !== 'LOW') ? FraudDetector.nextAlertId++ : 0;
    return {
      alertId,
      transactionId: tx.transactionId,
      accountNumber: acc,
      amount: amt,
      riskLevel: risk,
      reason,
      timestamp: tx.timestamp,
      reviewStatus
    };
  }

  logAlert(alert) {
    if (alert && alert.riskLevel !== 'LOW') {
      this.alerts.unshift(alert);
    }
  }

  reviewAlert(alertId, newStatus) {
    const a = this.alerts.find(al => al.alertId === Number(alertId));
    if (a) {
      a.reviewStatus = newStatus;
    }
  }

  getHighRiskCount() { return this.alerts.filter(a => a.riskLevel === 'HIGH').length; }
  getMediumRiskCount() { return this.alerts.filter(a => a.riskLevel === 'MEDIUM').length; }
  getPendingCount() { return this.alerts.filter(a => a.reviewStatus === 'PENDING_REVIEW').length; }
}

// ==============================================================================
// BANK CONTAINER & STORAGE SYNCHRONIZATION
// ==============================================================================
class Bank {
  constructor() {
    this.accounts = [];
    this.transactions = [];
    this.fraudDetector = new FraudDetector();
    this.loadFromStorage();
  }

  addAccount(account) {
    this.accounts.push(account);
    this.saveToStorage();
  }

  findAccount(query) {
    if (query === null || query === undefined || query === '') {
      throw new BankingException('Account search query cannot be empty.');
    }
    const clean = String(query).trim().toLowerCase();
    const acc = this.accounts.find(a => 
      String(a.getAccountNumber()) === clean || 
      a.getName().toLowerCase() === clean
    );
    if (!acc) throw new AccountNotFoundException(query);
    return acc;
  }

  closeAccount(accNum) {
    const acc = this.findAccount(accNum);
    acc.closeAccount();
    const tx = new Transaction(acc.getAccountNumber(), -1, 'CLOSE_ACCOUNT', 0.0, formatTimestamp(), 'SUCCESS', 'LOW', 'Account closed by user');
    this.transactions.unshift(tx);
    this.saveToStorage();
  }

  deposit(accNum, amount, note = '') {
    const acc = this.findAccount(accNum);
    const prevBal = acc.getBalance();
    acc.deposit(amount);

    const tx = new Transaction(acc.getAccountNumber(), -1, 'DEPOSIT', amount, formatTimestamp(), 'SUCCESS', 'LOW', note || 'Standard deposit');
    const alert = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (alert.riskLevel !== 'LOW') {
      tx.riskLevel = alert.riskLevel;
      tx.flagReason = alert.reason;
      this.fraudDetector.logAlert(alert);
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, prevBal, newBal: acc.getBalance(), acc, alert };
  }

  withdraw(accNum, amount, note = '') {
    const acc = this.findAccount(accNum);
    const prevBal = acc.getBalance();
    acc.withdraw(amount);

    const tx = new Transaction(acc.getAccountNumber(), -1, 'WITHDRAWAL', amount, formatTimestamp(), 'SUCCESS', 'LOW', note || 'Cash withdrawal');
    const alert = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (alert.riskLevel !== 'LOW') {
      tx.riskLevel = alert.riskLevel;
      tx.flagReason = alert.reason;
      this.fraudDetector.logAlert(alert);
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, prevBal, newBal: acc.getBalance(), acc, alert };
  }

  transfer(senderNum, receiverNum, amount, note = '') {
    const sender = this.findAccount(senderNum);
    const receiver = this.findAccount(receiverNum);

    if (sender.getAccountNumber() === receiver.getAccountNumber()) {
      throw new BankingException('Transfer failed: Source and destination accounts cannot be identical.');
    }

    const amt = Number(amount);
    if (isNaN(amt) || amt <= 0) throw new InvalidAmountException();

    // Verify sender can withdraw
    let reason = '';
    if (!sender.canWithdraw(amt, (r) => { reason = r; })) {
      throw new InsufficientBalanceException(sender.getBalance(), amt);
    }

    // Atomic execution
    const senderPrev = sender.getBalance();
    const receiverPrev = receiver.getBalance();

    sender.withdraw(amt);
    receiver.deposit(amt);

    const tx = new Transaction(sender.getAccountNumber(), receiver.getAccountNumber(), 'TRANSFER', amt, formatTimestamp(), 'SUCCESS', 'LOW', note || `Transfer to Acc #${receiver.getAccountNumber()}`);
    const alert = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (alert.riskLevel !== 'LOW') {
      tx.riskLevel = alert.riskLevel;
      tx.flagReason = alert.reason;
      this.fraudDetector.logAlert(alert);
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, sender, receiver, senderPrev, senderNew: sender.getBalance(), alert };
  }

  getTotalDeposits() {
    return this.accounts
      .filter(a => a.isActive())
      .reduce((sum, a) => sum + Math.max(0, a.getBalance()), 0);
  }

  getTotalWithdrawals() {
    return this.transactions
      .filter(t => (t.type === 'WITHDRAWAL' || t.type === 'TRANSFER') && t.status === 'SUCCESS')
      .reduce((sum, t) => sum + t.amount, 0);
  }

  saveToStorage() {
    try {
      const data = {
        accounts: this.accounts.map(a => ({
          type: a.getAccountType(),
          accountNumber: a.getAccountNumber(),
          name: a.getName(),
          balance: a.getBalance(),
          status: a.getStatus(),
          openingDate: a.getOpeningDate(),
          minBalance: (a instanceof SavingsAccount) ? a.getMinimumBalance() : null,
          overdraftLimit: (a instanceof CurrentAccount) ? a.getOverdraftLimit() : null,
          termMonths: (a instanceof FixedDepositAccount) ? a.getTermMonths() : null,
          interestRate: (a instanceof FixedDepositAccount) ? a.getInterestRate() : null,
          maturityAmount: (a instanceof FixedDepositAccount) ? a.getMaturityAmount() : null,
          maturityDate: (a instanceof FixedDepositAccount) ? a.getMaturityDate() : null
        })),
        transactions: this.transactions,
        alerts: this.fraudDetector.alerts,
        config: this.fraudDetector.config,
        nextAccNum: Account.nextAccountNumber,
        nextTxId: Transaction.nextTransactionId,
        nextAlertId: FraudDetector.nextAlertId
      };
      localStorage.setItem('smartbank_db_v2', JSON.stringify(data));
    } catch (err) {
      console.warn('Storage sync notice:', err);
    }
  }

  loadFromStorage() {
    const raw = localStorage.getItem('smartbank_db_v2');
    if (!raw) {
      this.seedDefaultData();
      return;
    }

    try {
      const parsed = JSON.parse(raw);
      this.accounts = (parsed.accounts || []).map(a => {
        if (a.type === 'Savings Account') {
          return new SavingsAccount(a.name, a.balance, a.openingDate, a.minBalance || 1000, a.status, a.accountNumber);
        } else if (a.type === 'Current Account') {
          return new CurrentAccount(a.name, a.balance, a.openingDate, a.overdraftLimit || 25000, a.status, a.accountNumber);
        } else if (a.type === 'Fixed Deposit') {
          return new FixedDepositAccount(a.name, a.balance, a.termMonths || 12, a.interestRate || 0.07, a.openingDate, a.maturityAmount, a.maturityDate, a.status, a.accountNumber);
        }
        return new SavingsAccount(a.name, a.balance, a.openingDate, 1000, a.status, a.accountNumber);
      });

      this.transactions = (parsed.transactions || []).map(t => 
        new Transaction(t.senderAccount, t.receiverAccount, t.type, t.amount, t.timestamp, t.status, t.riskLevel, t.flagReason, t.transactionId)
      );

      this.fraudDetector.alerts = parsed.alerts || [];
      if (parsed.config) this.fraudDetector.config = parsed.config;

      Account.nextAccountNumber = parsed.nextAccNum || 1004;
      Transaction.nextTransactionId = parsed.nextTxId || 10005;
      FraudDetector.nextAlertId = parsed.nextAlertId || 5003;

    } catch (e) {
      console.error('Failed to parse stored bank data, resetting to defaults:', e);
      this.seedDefaultData();
    }
  }

  seedDefaultData() {
    this.accounts = [
      new SavingsAccount('Purva Deshmukh', 75000.0, '2026-10-01', 1000.0, 'ACTIVE', 1001),
      new CurrentAccount('Deshmukh Enterprises', 150000.0, '2026-10-05', 50000.0, 'ACTIVE', 1002),
      new FixedDepositAccount('Aarav Sharma', 100000.0, 12, 0.0725, '2026-10-10', 107449.54, calculateFutureDate(12), 'ACTIVE', 1003)
    ];

    this.transactions = [
      new Transaction(1001, -1, 'DEPOSIT', 25000.0, '2026-10-15 10:30:00', 'SUCCESS', 'LOW', 'Direct corporate deposit', 'TXN-10001'),
      new Transaction(1002, 1001, 'TRANSFER', 20000.0, '2026-10-20 14:15:00', 'SUCCESS', 'LOW', 'Monthly vendor settlement', 'TXN-10002'),
      new Transaction(1001, -1, 'DEPOSIT', 60000.0, '2026-11-05 11:20:00', 'SUCCESS', 'MEDIUM', 'Commercial real estate deposit', 'TXN-10003'),
      new Transaction(1002, -1, 'WITHDRAWAL', 70000.0, '2026-11-10 16:45:00', 'SUCCESS', 'MEDIUM', 'Equipment procurement cash withdrawal', 'TXN-10004')
    ];

    this.fraudDetector.alerts = [
      {
        alertId: 5001,
        transactionId: 'TXN-10003',
        accountNumber: 1001,
        amount: 60000.0,
        riskLevel: 'MEDIUM',
        reason: 'Large transaction amount (₹60,000.00) meets advisory review threshold (₹50,000.00).',
        timestamp: '2026-11-05 11:20:00',
        reviewStatus: 'APPROVED'
      },
      {
        alertId: 5002,
        transactionId: 'TXN-10004',
        accountNumber: 1002,
        amount: 70000.0,
        riskLevel: 'MEDIUM',
        reason: 'Large transaction amount (₹70,000.00) meets advisory review threshold (₹50,000.00).',
        timestamp: '2026-11-10 16:45:00',
        reviewStatus: 'PENDING_REVIEW'
      }
    ];

    Account.nextAccountNumber = 1004;
    Transaction.nextTransactionId = 10005;
    FraudDetector.nextAlertId = 5003;
    this.saveToStorage();
  }
}

// Global Bank Instance
const bank = new Bank();

// ==============================================================================
// TOAST NOTIFICATIONS & DIALOGS
// ==============================================================================
function showToast(message, type = 'info') {
  const container = document.getElementById('toastContainer');
  if (!container) return;

  const toast = document.createElement('div');
  toast.className = `toast toast-${type}`;
  toast.innerHTML = `<span>${escapeHtml(message)}</span>`;

  container.appendChild(toast);
  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(100%)';
    toast.style.transition = 'all 0.25s';
    setTimeout(() => toast.remove(), 250);
  }, 3500);
}

function escapeHtml(str) {
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#039;');
}

// ==============================================================================
// NAVIGATION CONTROLLER
// ==============================================================================
function navigateTo(pageId, subAction = null) {
  // Update sidebar active state
  document.querySelectorAll('.sidebar-nav .nav-item').forEach(item => {
    item.classList.toggle('active', item.dataset.page === pageId);
  });

  // Switch visible section
  document.querySelectorAll('.page-section').forEach(sec => {
    sec.classList.remove('active');
  });

  const targetSection = document.getElementById(`page-${pageId}`);
  if (targetSection) {
    targetSection.classList.add('active');
  }

  // Update topbar header
  const titleHeading = document.getElementById('pageTitleHeading');
  const subHeading = document.getElementById('pageSubtitleHeading');

  const headers = {
    dashboard: { title: 'Dashboard', sub: 'Financial metrics, transaction flow & live security monitoring' },
    accounts: { title: 'Account Management', sub: 'Encapsulated account records, limits & polymorphic rules' },
    transactions: { title: 'Financial Transactions', sub: 'Deposits, withdrawals, atomic transfers & rule-based security' },
    security: { title: 'Security Center & Anomaly Engine', sub: 'Live fraud detection rules, anomaly alerts & administrative audits' },
    statements: { title: 'Financial Statements', sub: 'Friend-class privileged account statements & audit export' },
    settings: { title: 'System Settings', sub: 'Risk threshold tuning & persistent database management' }
  };

  if (headers[pageId]) {
    titleHeading.textContent = headers[pageId].title;
    subHeading.textContent = headers[pageId].sub;
  }

  // Handle specific subactions
  if (pageId === 'accounts' && subAction === 'create') {
    openCreateAccountModal();
  } else if (pageId === 'transactions' && subAction) {
    switchTxTab(subAction);
  }

  // Refresh page specific content
  refreshActivePage(pageId);
}

function refreshActivePage(pageId) {
  updateMetrics();
  if (pageId === 'dashboard') {
    renderDashboardRecent();
    renderCharts();
  } else if (pageId === 'accounts') {
    renderAccountsTable();
  } else if (pageId === 'transactions') {
    renderTransactionHistory();
  } else if (pageId === 'security') {
    renderSecurityCenter();
  }
}

// ==============================================================================
// METRICS & DASHBOARD
// ==============================================================================
function updateMetrics() {
  const accountsCount = bank.accounts.length;
  const totalDeposits = bank.getTotalDeposits();
  const txnCount = bank.transactions.length;
  const totalWithdrawals = bank.getTotalWithdrawals();
  const totalAlerts = bank.fraudDetector.alerts.length;
  const highRisk = bank.fraudDetector.getHighRiskCount();
  const medRisk = bank.fraudDetector.getMediumRiskCount();
  const pendingAlerts = bank.fraudDetector.getPendingCount();

  // Top stats
  const statAcc = document.getElementById('statAccountsCount');
  const statDep = document.getElementById('statTotalDeposits');
  const statTxn = document.getElementById('statTransactionsCount');
  const statWithSub = document.getElementById('statWithdrawalsSub');
  const statAlert = document.getElementById('statAlertsCount');
  const statHighSub = document.getElementById('statHighRiskSub');

  if (statAcc) statAcc.textContent = accountsCount;
  if (statDep) statDep.textContent = formatRupees(totalDeposits);
  if (statTxn) statTxn.textContent = txnCount;
  if (statWithSub) statWithSub.textContent = `Debits: ${formatRupees(totalWithdrawals)}`;
  if (statAlert) statAlert.textContent = totalAlerts;
  if (statHighSub) statHighSub.textContent = `${highRisk} High Risk`;

  // Sidebar badge
  const sideBadge = document.getElementById('sidebarAlertBadge');
  const headerBadge = document.getElementById('headerAlertBadge');
  if (sideBadge) {
    sideBadge.style.display = pendingAlerts > 0 ? 'inline-block' : 'none';
    sideBadge.textContent = pendingAlerts;
  }
  if (headerBadge) {
    headerBadge.style.display = pendingAlerts > 0 ? 'block' : 'none';
  }

  // Security dashboard breakdown
  const lowCount = bank.transactions.filter(t => t.riskLevel === 'LOW').length;
  const dashLow = document.getElementById('dashLowRiskCount');
  const dashMed = document.getElementById('dashMedRiskCount');
  const dashHigh = document.getElementById('dashHighRiskCount');
  if (dashLow) dashLow.textContent = lowCount;
  if (dashMed) dashMed.textContent = medRisk;
  if (dashHigh) dashHigh.textContent = highRisk;
}

function renderDashboardRecent() {
  const tbody = document.getElementById('dashRecentTxnBody');
  if (!tbody) return;

  const recents = bank.transactions.slice(0, 5);
  if (recents.length === 0) {
    tbody.innerHTML = '<tr><td colspan="8" class="text-center text-muted">No transactions recorded yet.</td></tr>';
    return;
  }

  tbody.innerHTML = recents.map(tx => {
    const toStr = tx.receiverAccount === -1 ? '-' : `#${tx.receiverAccount}`;
    const riskBadge = `badge-risk-${tx.riskLevel.toLowerCase()}`;
    return `
      <tr>
        <td><strong class="font-mono text-sm">${tx.transactionId}</strong></td>
        <td class="text-secondary text-sm">${tx.timestamp}</td>
        <td><span class="badge badge-neutral">${tx.type}</span></td>
        <td class="font-mono">#${tx.senderAccount}</td>
        <td class="font-mono">${toStr}</td>
        <td><strong class="font-mono">${formatRupees(tx.amount)}</strong></td>
        <td><span class="badge ${tx.status === 'SUCCESS' ? 'badge-success' : 'badge-warning'}">${tx.status}</span></td>
        <td><span class="badge ${riskBadge}">${tx.riskLevel}</span></td>
      </tr>
    `;
  }).join('');
}

// ==============================================================================
// CANVAS CHARTS (Dynamic HTML5 Canvas Rendering without External CDN dependencies)
// ==============================================================================
function renderCharts() {
  drawCashFlowChart();
  drawAccountTypeChart();
}

function drawCashFlowChart() {
  const canvas = document.getElementById('cashFlowChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.parentElement.clientWidth || 400;
  const h = 220;
  canvas.width = w;
  canvas.height = h;

  // Inflows vs Outflows
  const inflows = bank.transactions
    .filter(t => t.type === 'DEPOSIT' && t.status === 'SUCCESS')
    .reduce((sum, t) => sum + t.amount, 0);

  const outflows = bank.transactions
    .filter(t => (t.type === 'WITHDRAWAL' || t.type === 'TRANSFER') && t.status === 'SUCCESS')
    .reduce((sum, t) => sum + t.amount, 0);

  ctx.clearRect(0, 0, w, h);

  const maxVal = Math.max(inflows, outflows, 10000);
  const chartHeight = 140;
  const baseY = 170;

  // Bar dimensions
  const barWidth = Math.min(80, w / 4);
  const bar1X = (w / 3) - (barWidth / 2);
  const bar2X = ((w * 2) / 3) - (barWidth / 2);

  const h1 = (inflows / maxVal) * chartHeight;
  const h2 = (outflows / maxVal) * chartHeight;

  // Background grid line
  ctx.strokeStyle = '#E2E8F0';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(30, baseY);
  ctx.lineTo(w - 30, baseY);
  ctx.stroke();

  // Draw Bar 1 (Inflows - Green)
  ctx.fillStyle = '#059669';
  ctx.beginPath();
  ctx.roundRect ? ctx.roundRect(bar1X, baseY - h1, barWidth, h1, [6, 6, 0, 0]) : ctx.rect(bar1X, baseY - h1, barWidth, h1);
  ctx.fill();

  // Draw Bar 2 (Outflows - Navy/Red)
  ctx.fillStyle = '#0F172A';
  ctx.beginPath();
  ctx.roundRect ? ctx.roundRect(bar2X, baseY - h2, barWidth, h2, [6, 6, 0, 0]) : ctx.rect(bar2X, baseY - h2, barWidth, h2);
  ctx.fill();

  // Labels & Values
  ctx.font = '600 11px Plus Jakarta Sans, sans-serif';
  ctx.fillStyle = '#64748B';
  ctx.textAlign = 'center';
  ctx.fillText('INFLOWS (Deposits)', bar1X + (barWidth / 2), baseY + 20);
  ctx.fillText('OUTFLOWS (Debits)', bar2X + (barWidth / 2), baseY + 20);

  ctx.font = '700 12px JetBrains Mono, monospace';
  ctx.fillStyle = '#059669';
  ctx.fillText(formatRupees(inflows), bar1X + (barWidth / 2), baseY - h1 - 8);

  ctx.fillStyle = '#0F172A';
  ctx.fillText(formatRupees(outflows), bar2X + (barWidth / 2), baseY - h2 - 8);
}

function drawAccountTypeChart() {
  const canvas = document.getElementById('accountTypeChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.parentElement.clientWidth || 400;
  const h = 220;
  canvas.width = w;
  canvas.height = h;

  ctx.clearRect(0, 0, w, h);

  const savingsCount = bank.accounts.filter(a => a instanceof SavingsAccount).length;
  const currentCount = bank.accounts.filter(a => a instanceof CurrentAccount).length;
  const fdCount = bank.accounts.filter(a => a instanceof FixedDepositAccount).length;
  const total = savingsCount + currentCount + fdCount;

  const centerX = w / 3;
  const centerY = h / 2;
  const radius = 65;
  const innerRadius = 40;

  if (total === 0) {
    ctx.font = '500 12px Plus Jakarta Sans, sans-serif';
    ctx.fillStyle = '#94A3B8';
    ctx.textAlign = 'center';
    ctx.fillText('No active accounts', centerX, centerY);
    return;
  }

  const data = [
    { label: 'Savings (4%)', count: savingsCount, color: '#0EA5E9' },
    { label: 'Current (Overdraft)', count: currentCount, color: '#0F172A' },
    { label: 'Fixed Deposit', count: fdCount, color: '#059669' }
  ];

  let currentAngle = -0.5 * Math.PI;

  data.forEach(item => {
    const sliceAngle = (item.count / total) * 2 * Math.PI;
    if (item.count > 0) {
      ctx.beginPath();
      ctx.arc(centerX, centerY, radius, currentAngle, currentAngle + sliceAngle);
      ctx.arc(centerX, centerY, innerRadius, currentAngle + sliceAngle, currentAngle, true);
      ctx.closePath();
      ctx.fillStyle = item.color;
      ctx.fill();
    }
    currentAngle += sliceAngle;
  });

  // Center Count
  ctx.font = '800 18px JetBrains Mono, monospace';
  ctx.fillStyle = '#0F172A';
  ctx.textAlign = 'center';
  ctx.fillText(total, centerX, centerY + 5);

  ctx.font = '500 9px Plus Jakarta Sans, sans-serif';
  ctx.fillStyle = '#64748B';
  ctx.fillText('ACCOUNTS', centerX, centerY + 18);

  // Legend
  const legendX = (w / 2) + 20;
  let legendY = 60;

  data.forEach(item => {
    const percent = total > 0 ? ((item.count / total) * 100).toFixed(0) : 0;

    // Color box
    ctx.fillStyle = item.color;
    ctx.beginPath();
    ctx.arc(legendX, legendY, 5, 0, 2 * Math.PI);
    ctx.fill();

    // Text
    ctx.font = '600 11px Plus Jakarta Sans, sans-serif';
    ctx.fillStyle = '#1E293B';
    ctx.textAlign = 'left';
    ctx.fillText(`${item.label}`, legendX + 12, legendY + 4);

    ctx.font = '700 11px JetBrains Mono, monospace';
    ctx.fillStyle = '#64748B';
    ctx.fillText(`${item.count} (${percent}%)`, legendX + 12, legendY + 18);

    legendY += 38;
  });
}

// ==============================================================================
// ACCOUNTS MODULE
// ==============================================================================
function renderAccountsTable() {
  const tbody = document.getElementById('accountsTableBody');
  const badge = document.getElementById('accountCountBadge');
  const searchInput = document.getElementById('accountSearchInput');
  const typeFilter = document.getElementById('accountTypeFilter');

  if (!tbody) return;

  const query = searchInput ? searchInput.value.trim().toLowerCase() : '';
  const selectedType = typeFilter ? typeFilter.value : 'ALL';

  let list = bank.accounts;

  if (selectedType !== 'ALL') {
    list = list.filter(a => a.getAccountType() === selectedType);
  }

  if (query) {
    list = list.filter(a => 
      String(a.getAccountNumber()).includes(query) || 
      a.getName().toLowerCase().includes(query)
    );
  }

  if (badge) badge.textContent = `${list.length} Accounts`;

  if (list.length === 0) {
    tbody.innerHTML = '<tr><td colspan="7" class="text-center text-muted">No accounts match the current filter.</td></tr>';
    return;
  }

  tbody.innerHTML = list.map(acc => {
    let featureInfo = '';
    if (acc instanceof SavingsAccount) {
      featureInfo = `<span class="text-xs text-muted">Min Bal: ${formatRupees(acc.getMinimumBalance())} (4% p.a.)</span>`;
    } else if (acc instanceof CurrentAccount) {
      featureInfo = `<span class="text-xs text-muted">Overdraft: ${formatRupees(acc.getOverdraftLimit())}</span>`;
    } else if (acc instanceof FixedDepositAccount) {
      featureInfo = `<span class="text-xs text-muted">${acc.getTermMonths()} Mo | Mat: ${formatRupees(acc.getMaturityAmount())}</span>`;
    }

    const balColor = acc.getBalance() >= 0 ? 'text-green' : 'text-red';
    const statusClass = acc.isActive() ? 'badge-success' : 'badge-danger';

    return `
      <tr>
        <td><strong class="font-mono">#${acc.getAccountNumber()}</strong></td>
        <td><strong>${escapeHtml(acc.getName())}</strong></td>
        <td><span class="badge badge-neutral">${acc.getAccountType()}</span></td>
        <td>${featureInfo}</td>
        <td><strong class="font-mono ${balColor}">${formatRupees(acc.getBalance())}</strong></td>
        <td><span class="badge ${statusClass}">${acc.getStatus()}</span></td>
        <td>
          <div class="d-flex gap-2">
            <button class="btn btn-secondary btn-sm" onclick="openAccountDetailModal(${acc.getAccountNumber()})">Passbook</button>
            <button class="btn btn-secondary btn-sm" onclick="quickFillTx('deposit', ${acc.getAccountNumber()})">Deposit</button>
            ${acc.isActive() ? `<button class="btn btn-secondary btn-sm" onclick="confirmCloseAccount(${acc.getAccountNumber()})">Close</button>` : ''}
          </div>
        </td>
      </tr>
    `;
  }).join('');
}

function openAccountDetailModal(accNum) {
  try {
    const acc = bank.findAccount(accNum);
    const modal = document.getElementById('accountDetailModal');
    const modalBody = document.getElementById('accountDetailModalBody');
    const sub = document.getElementById('modalAccSubtitle');

    if (!modal || !modalBody) return;

    sub.textContent = `Account #${acc.getAccountNumber()} - ${acc.getName()}`;

    const interestData = acc.calculateInterest();

    let extraDetails = '';
    if (acc instanceof SavingsAccount) {
      extraDetails = `
        <div class="info-cell">
          <span class="lbl">Mandatory Minimum Balance</span>
          <span class="val font-mono">${formatRupees(acc.getMinimumBalance())}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Annual Interest Rate</span>
          <span class="val font-mono text-green">4.0% p.a.</span>
        </div>
      `;
    } else if (acc instanceof CurrentAccount) {
      extraDetails = `
        <div class="info-cell">
          <span class="lbl">Approved Overdraft Limit</span>
          <span class="val font-mono">${formatRupees(acc.getOverdraftLimit())}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Available Credit Line</span>
          <span class="val font-mono text-green">${formatRupees(acc.getBalance() + acc.getOverdraftLimit())}</span>
        </div>
      `;
    } else if (acc instanceof FixedDepositAccount) {
      extraDetails = `
        <div class="info-cell">
          <span class="lbl">Investment Term</span>
          <span class="val">${acc.getTermMonths()} Months</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Guaranteed Maturity Date</span>
          <span class="val font-mono">${acc.getMaturityDate()}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Maturity Value</span>
          <span class="val font-mono text-green">${formatRupees(acc.getMaturityAmount())}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Interest Accrued at Maturity</span>
          <span class="val font-mono text-green">${formatRupees(acc.getMaturityAmount() - acc.getBalance())}</span>
        </div>
      `;
    }

    modalBody.innerHTML = `
      <div class="statement-account-info mb-3">
        <div class="info-cell">
          <span class="lbl">Account Holder</span>
          <span class="val">${escapeHtml(acc.getName())}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Account Type</span>
          <span class="val">${acc.getAccountType()}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Current Ledger Balance</span>
          <span class="val font-mono ${acc.getBalance() >= 0 ? 'text-green' : 'text-red'}">${formatRupees(acc.getBalance())}</span>
        </div>
        <div class="info-cell">
          <span class="lbl">Account Status</span>
          <span class="val ${acc.isActive() ? 'text-green' : 'text-red'}">${acc.getStatus()}</span>
        </div>
        ${extraDetails}
      </div>

      <div class="card mb-3" style="background:#F8FAFC;">
        <div class="card-body">
          <h4 class="mb-1 text-sm font-600">Polymorphic Business Rules &amp; Interest Simulation</h4>
          <p class="text-sm text-secondary">${interestData.note}</p>
        </div>
      </div>

      <div class="d-flex justify-between gap-2 mt-3">
        <button class="btn btn-secondary btn-sm" onclick="quickFillTx('deposit', ${acc.getAccountNumber()})">Deposit into Account</button>
        <button class="btn btn-secondary btn-sm" onclick="quickFillTx('withdraw', ${acc.getAccountNumber()})">Withdraw from Account</button>
        <button class="btn btn-secondary btn-sm" onclick="viewStatementForAccount(${acc.getAccountNumber()})">View Statement</button>
      </div>
    `;

    modal.classList.add('active');
  } catch (err) {
    showToast(err.message, 'error');
  }
}

function openCreateAccountModal() {
  const modal = document.getElementById('createAccountModal');
  if (modal) modal.classList.add('active');
}

function closeCreateAccountModal() {
  const modal = document.getElementById('createAccountModal');
  if (modal) modal.classList.remove('active');
}

function confirmCloseAccount(accNum) {
  try {
    const acc = bank.findAccount(accNum);
    if (confirm(`Are you sure you want to permanently close Account #${accNum} (${acc.getName()})?`)) {
      bank.closeAccount(accNum);
      showToast(`Account #${accNum} has been permanently closed.`, 'info');
      refreshActivePage('accounts');
    }
  } catch (err) {
    showToast(err.message, 'error');
  }
}

function quickFillTx(type, accNum) {
  // Close any open modals
  document.querySelectorAll('.modal-overlay').forEach(m => m.classList.remove('active'));
  navigateTo('transactions', type);
  setTimeout(() => {
    if (type === 'deposit') {
      const inp = document.getElementById('depAccountNum');
      if (inp) { inp.value = accNum; inp.dispatchEvent(new Event('input')); }
    } else if (type === 'withdraw') {
      const inp = document.getElementById('withAccountNum');
      if (inp) { inp.value = accNum; inp.dispatchEvent(new Event('input')); }
    }
  }, 50);
}

// ==============================================================================
// TRANSACTIONS MODULE & FORM CONTROLLERS
// ==============================================================================
function switchTxTab(tabName) {
  document.querySelectorAll('.tabs-nav .tab-btn').forEach(btn => {
    btn.classList.toggle('active', btn.dataset.txTab === tabName);
  });
  document.querySelectorAll('.tx-form').forEach(f => {
    f.classList.remove('active');
  });

  const targetForm = document.getElementById(`${tabName}Form`);
  if (targetForm) targetForm.classList.add('active');
}

function renderTransactionHistory() {
  const tbody = document.getElementById('transactionHistoryTableBody');
  const searchInput = document.getElementById('txHistorySearch');
  const typeFilter = document.getElementById('txTypeFilter');
  const riskFilter = document.getElementById('txRiskFilter');

  if (!tbody) return;

  const query = searchInput ? searchInput.value.trim().toLowerCase() : '';
  const selectedType = typeFilter ? typeFilter.value : 'ALL';
  const selectedRisk = riskFilter ? riskFilter.value : 'ALL';

  let list = bank.transactions;

  if (selectedType !== 'ALL') {
    list = list.filter(t => t.type === selectedType);
  }

  if (selectedRisk !== 'ALL') {
    list = list.filter(t => t.riskLevel === selectedRisk);
  }

  if (query) {
    list = list.filter(t => 
      t.transactionId.toLowerCase().includes(query) || 
      String(t.senderAccount).includes(query) || 
      String(t.receiverAccount).includes(query)
    );
  }

  if (list.length === 0) {
    tbody.innerHTML = '<tr><td colspan="9" class="text-center text-muted">No transactions matching criteria.</td></tr>';
    return;
  }

  tbody.innerHTML = list.map(tx => {
    const toStr = tx.receiverAccount === -1 ? '-' : `#${tx.receiverAccount}`;
    const riskBadge = `badge-risk-${tx.riskLevel.toLowerCase()}`;
    return `
      <tr>
        <td><strong class="font-mono text-sm">${tx.transactionId}</strong></td>
        <td class="text-secondary text-sm">${tx.timestamp}</td>
        <td><span class="badge badge-neutral">${tx.type}</span></td>
        <td class="font-mono">#${tx.senderAccount}</td>
        <td class="font-mono">${toStr}</td>
        <td><strong class="font-mono">${formatRupees(tx.amount)}</strong></td>
        <td><span class="badge ${tx.status === 'SUCCESS' ? 'badge-success' : 'badge-danger'}">${tx.status}</span></td>
        <td><span class="badge ${riskBadge}">${tx.riskLevel}</span></td>
        <td class="text-sm text-secondary">${escapeHtml(tx.flagReason || 'None')}</td>
      </tr>
    `;
  }).join('');
}

function renderReceipt(data) {
  const container = document.getElementById('receiptOutputContainer');
  if (!container) return;

  const tx = data.tx;
  const isTransfer = tx.type === 'TRANSFER';
  const riskClass = `badge-risk-${tx.riskLevel.toLowerCase()}`;

  let partyRows = '';
  if (isTransfer) {
    partyRows = `
      <div class="receipt-item">
        <span class="receipt-lbl">Sender Account</span>
        <span class="receipt-val">#${data.sender.getAccountNumber()} (${escapeHtml(data.sender.getName())})</span>
      </div>
      <div class="receipt-item">
        <span class="receipt-lbl">Receiver Account</span>
        <span class="receipt-val">#${data.receiver.getAccountNumber()} (${escapeHtml(data.receiver.getName())})</span>
      </div>
      <div class="receipt-item">
        <span class="receipt-lbl">Sender Remaining Balance</span>
        <span class="receipt-val font-mono">${formatRupees(data.senderNew)}</span>
      </div>
    `;
  } else {
    partyRows = `
      <div class="receipt-item">
        <span class="receipt-lbl">Account Holder</span>
        <span class="receipt-val">#${data.acc.getAccountNumber()} - ${escapeHtml(data.acc.getName())}</span>
      </div>
      <div class="receipt-item">
        <span class="receipt-lbl">Account Type</span>
        <span class="receipt-val">${data.acc.getAccountType()}</span>
      </div>
      <div class="receipt-item">
        <span class="receipt-lbl">Previous Balance</span>
        <span class="receipt-val font-mono">${formatRupees(data.prevBal)}</span>
      </div>
      <div class="receipt-item">
        <span class="receipt-lbl">Updated Balance</span>
        <span class="receipt-val font-mono highlight">${formatRupees(data.newBal)}</span>
      </div>
    `;
  }

  container.innerHTML = `
    <div class="receipt-voucher">
      <div class="receipt-head">
        <div>
          <span class="receipt-title-text">${tx.type} RECEIPT</span>
          <span class="card-subtitle">${tx.timestamp}</span>
        </div>
        <span class="badge ${riskClass}">${tx.riskLevel} RISK</span>
      </div>

      <div class="receipt-grid">
        <div class="receipt-item">
          <span class="receipt-lbl">Transaction ID</span>
          <span class="receipt-val font-mono">${tx.transactionId}</span>
        </div>
        <div class="receipt-item">
          <span class="receipt-lbl">Amount Transacted</span>
          <span class="receipt-val font-mono highlight">${formatRupees(tx.amount)}</span>
        </div>
        ${partyRows}
        <div class="receipt-item">
          <span class="receipt-lbl">Transaction Status</span>
          <span class="receipt-val text-green">${tx.status}</span>
        </div>
      </div>

      <div class="receipt-security-stamp">
        <strong>Security Audit Note:</strong> ${escapeHtml(tx.flagReason || 'Standard verification passed')}
      </div>
    </div>
  `;
}

// Security Confirmation Handling
let pendingSecTxExecution = null;

function promptSecurityReview(alert, onConfirm) {
  const modal = document.getElementById('securityConfirmModal');
  const banner = document.getElementById('secConfirmBanner');
  const riskLbl = document.getElementById('secConfirmRiskLevel');
  const reasonTxt = document.getElementById('secConfirmReason');

  if (!modal) {
    onConfirm();
    return;
  }

  riskLbl.textContent = `${alert.riskLevel} RISK TRANSACTION ADVISORY`;
  reasonTxt.textContent = alert.reason;
  modal.classList.add('active');

  pendingSecTxExecution = onConfirm;
}

// ==============================================================================
// SECURITY CENTER MODULE
// ==============================================================================
function renderSecurityCenter() {
  const tbody = document.getElementById('securityAlertsTableBody');
  const secTotal = document.getElementById('secTotalAlerts');
  const secHigh = document.getElementById('secHighRiskCount');
  const secMed = document.getElementById('secMedRiskCount');
  const secApproved = document.getElementById('secApprovedCount');

  const alerts = bank.fraudDetector.alerts;

  if (secTotal) secTotal.textContent = alerts.length;
  if (secHigh) secHigh.textContent = bank.fraudDetector.getHighRiskCount();
  if (secMed) secMed.textContent = bank.fraudDetector.getMediumRiskCount();
  if (secApproved) secApproved.textContent = alerts.filter(a => a.reviewStatus === 'APPROVED').length;

  if (!tbody) return;

  if (alerts.length === 0) {
    tbody.innerHTML = '<tr><td colspan="9" class="text-center text-muted">No security alerts logged. All operations normal.</td></tr>';
    return;
  }

  tbody.innerHTML = alerts.map(a => {
    const riskBadge = `badge-risk-${a.riskLevel.toLowerCase()}`;
    const statusClass = a.reviewStatus === 'APPROVED' ? 'badge-success' : 'badge-warning';

    return `
      <tr>
        <td><strong class="font-mono">ALT-${a.alertId}</strong></td>
        <td><strong class="font-mono">${a.transactionId}</strong></td>
        <td><strong class="font-mono">#${a.accountNumber}</strong></td>
        <td><strong class="font-mono">${formatRupees(a.amount)}</strong></td>
        <td><span class="badge ${riskBadge}">${a.riskLevel}</span></td>
        <td class="text-sm">${escapeHtml(a.reason)}</td>
        <td class="text-secondary text-sm">${a.timestamp}</td>
        <td><span class="badge ${statusClass}">${a.reviewStatus}</span></td>
        <td>
          ${a.reviewStatus === 'PENDING_REVIEW' ? `
            <button class="btn btn-primary btn-sm" onclick="approveAlert(${a.alertId})">Approve</button>
          ` : `<span class="text-sm text-muted">Reviewed</span>`}
        </td>
      </tr>
    `;
  }).join('');
}

function approveAlert(alertId) {
  bank.fraudDetector.reviewAlert(alertId, 'APPROVED');
  bank.saveToStorage();
  showToast(`Security Alert ALT-${alertId} marked as APPROVED.`, 'success');
  renderSecurityCenter();
  updateMetrics();
}

// ==============================================================================
// STATEMENTS MODULE
// ==============================================================================
function viewStatementForAccount(accNum) {
  document.querySelectorAll('.modal-overlay').forEach(m => m.classList.remove('active'));
  navigateTo('statements');
  const inp = document.getElementById('stmtAccountInput');
  if (inp) {
    inp.value = accNum;
    generateStatement(accNum);
  }
}

function generateStatement(accNum) {
  try {
    const acc = bank.findAccount(accNum);

    document.getElementById('stmtHolderName').textContent = acc.getName();
    document.getElementById('stmtAccountNum').textContent = acc.getAccountNumber();
    document.getElementById('stmtAccountType').textContent = acc.getAccountType();
    document.getElementById('stmtClosingBalance').textContent = formatRupees(acc.getBalance());
    document.getElementById('stmtCurrentDate').textContent = `Generated: ${formatDate()} ${new Date().toLocaleTimeString()}`;

    const txns = bank.transactions.filter(t => 
      t.senderAccount === acc.getAccountNumber() || t.receiverAccount === acc.getAccountNumber()
    );

    const ledger = document.getElementById('stmtLedgerBody');
    if (!ledger) return;

    if (txns.length === 0) {
      ledger.innerHTML = '<tr><td colspan="6" class="text-center text-muted">No transactions found for this account.</td></tr>';
      return;
    }

    ledger.innerHTML = txns.map(t => {
      const isCredit = (t.receiverAccount === acc.getAccountNumber()) || (t.type === 'DEPOSIT');
      const amtColor = isCredit ? 'text-green' : 'text-red';
      const sign = isCredit ? '+' : '-';

      return `
        <tr>
          <td class="text-secondary text-sm">${t.timestamp}</td>
          <td class="font-mono text-sm">${t.transactionId}</td>
          <td>${escapeHtml(t.flagReason || t.type)}</td>
          <td><span class="badge badge-neutral">${t.type}</span></td>
          <td class="font-mono ${amtColor}"><strong>${sign}${formatRupees(t.amount)}</strong></td>
          <td><span class="badge ${t.status === 'SUCCESS' ? 'badge-success' : 'badge-danger'}">${t.status}</span></td>
        </tr>
      `;
    }).join('');

    showToast(`Statement generated for Account #${acc.getAccountNumber()}`, 'success');
  } catch (err) {
    showToast(err.message, 'error');
  }
}

// ==============================================================================
// GLOBAL DOM INITIALIZATION & EVENT LISTENERS
// ==============================================================================
document.addEventListener('DOMContentLoaded', () => {

  // Splash Screen Fade-out
  const splash = document.getElementById('splashScreen');
  if (splash) {
    setTimeout(() => {
      splash.classList.add('fade-out');
    }, 1800);
  }

  // Set Live Date
  const dateEl = document.getElementById('liveDateDisplay');
  if (dateEl) {
    const today = new Date();
    dateEl.textContent = `${today.toLocaleDateString('en-US', { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' })} | Operational`;
  }

  // Navigation Links
  document.querySelectorAll('.sidebar-nav .nav-item').forEach(link => {
    link.addEventListener('click', (e) => {
      e.preventDefault();
      const pageId = link.dataset.page;
      navigateTo(pageId);
    });
  });

  // Mobile menu toggle
  const toggleBtn = document.getElementById('menuToggle');
  const sidebar = document.getElementById('sidebar');
  if (toggleBtn && sidebar) {
    toggleBtn.addEventListener('click', () => {
      sidebar.classList.toggle('open');
    });
  }

  // Operation Tabs in Transactions
  document.querySelectorAll('.tabs-nav .tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      switchTxTab(btn.dataset.txTab);
    });
  });

  // Live account peek when user types account number
  const bindPeek = (inputId, peekId) => {
    const inp = document.getElementById(inputId);
    const peek = document.getElementById(peekId);
    if (!inp || !peek) return;

    inp.addEventListener('input', () => {
      const val = inp.value.trim();
      if (!val) { peek.textContent = ''; return; }
      try {
        const acc = bank.findAccount(val);
        peek.textContent = `Found: ${acc.getName()} (${acc.getAccountType()}) | Balance: ${formatRupees(acc.getBalance())}`;
        peek.style.color = 'var(--brand-blue)';
      } catch {
        peek.textContent = 'Account not found';
        peek.style.color = 'var(--color-danger)';
      }
    });
  };

  bindPeek('depAccountNum', 'depAccountPeek');
  bindPeek('withAccountNum', 'withAccountPeek');
  bindPeek('trSenderNum', 'trSenderPeek');
  bindPeek('trReceiverNum', 'trReceiverPeek');

  // Transaction Forms Submissions
  const depForm = document.getElementById('depositForm');
  if (depForm) {
    depForm.addEventListener('submit', (e) => {
      e.preventDefault();
      const accNum = document.getElementById('depAccountNum').value;
      const amt = parseFloat(document.getElementById('depAmount').value);
      const note = document.getElementById('depNote').value;

      try {
        // Evaluate pre-risk
        const dummyTx = new Transaction(accNum, -1, 'DEPOSIT', amt, formatTimestamp());
        const preAlert = bank.fraudDetector.evaluateTransaction(dummyTx, bank.transactions);

        const execute = () => {
          const res = bank.deposit(accNum, amt, note);
          renderReceipt(res);
          showToast(`Deposited ${formatRupees(amt)} into Account #${accNum}!`, 'success');
          depForm.reset();
          document.getElementById('depAccountPeek').textContent = '';
          updateMetrics();
        };

        if (preAlert.riskLevel !== 'LOW') {
          promptSecurityReview(preAlert, execute);
        } else {
          execute();
        }
      } catch (err) {
        showToast(err.message, 'error');
      }
    });
  }

  const withForm = document.getElementById('withdrawForm');
  if (withForm) {
    withForm.addEventListener('submit', (e) => {
      e.preventDefault();
      const accNum = document.getElementById('withAccountNum').value;
      const amt = parseFloat(document.getElementById('withAmount').value);
      const note = document.getElementById('withNote').value;

      try {
        const dummyTx = new Transaction(accNum, -1, 'WITHDRAWAL', amt, formatTimestamp());
        const preAlert = bank.fraudDetector.evaluateTransaction(dummyTx, bank.transactions);

        const execute = () => {
          const res = bank.withdraw(accNum, amt, note);
          renderReceipt(res);
          showToast(`Withdrew ${formatRupees(amt)} from Account #${accNum}!`, 'success');
          withForm.reset();
          document.getElementById('withAccountPeek').textContent = '';
          updateMetrics();
        };

        if (preAlert.riskLevel !== 'LOW') {
          promptSecurityReview(preAlert, execute);
        } else {
          execute();
        }
      } catch (err) {
        showToast(err.message, 'error');
      }
    });
  }

  const trForm = document.getElementById('transferForm');
  if (trForm) {
    trForm.addEventListener('submit', (e) => {
      e.preventDefault();
      const senderNum = document.getElementById('trSenderNum').value;
      const receiverNum = document.getElementById('trReceiverNum').value;
      const amt = parseFloat(document.getElementById('trAmount').value);
      const note = document.getElementById('trNote').value;

      try {
        const dummyTx = new Transaction(senderNum, receiverNum, 'TRANSFER', amt, formatTimestamp());
        const preAlert = bank.fraudDetector.evaluateTransaction(dummyTx, bank.transactions);

        const execute = () => {
          const res = bank.transfer(senderNum, receiverNum, amt, note);
          renderReceipt({ tx: res.tx, sender: res.sender, receiver: res.receiver, senderNew: res.senderNew });
          showToast(`Successfully transferred ${formatRupees(amt)} from #${senderNum} to #${receiverNum}!`, 'success');
          trForm.reset();
          document.getElementById('trSenderPeek').textContent = '';
          document.getElementById('trReceiverPeek').textContent = '';
          updateMetrics();
        };

        if (preAlert.riskLevel !== 'LOW') {
          promptSecurityReview(preAlert, execute);
        } else {
          execute();
        }
      } catch (err) {
        showToast(err.message, 'error');
      }
    });
  }

  // Security Review Modal Buttons
  document.getElementById('btnApproveSecTx')?.addEventListener('click', () => {
    document.getElementById('securityConfirmModal')?.classList.remove('active');
    if (pendingSecTxExecution) {
      pendingSecTxExecution();
      pendingSecTxExecution = null;
    }
  });

  document.getElementById('btnRejectSecTx')?.addEventListener('click', () => {
    document.getElementById('securityConfirmModal')?.classList.remove('active');
    pendingSecTxExecution = null;
    showToast('Transaction was rejected by administrator.', 'warning');
  });

  document.getElementById('btnCloseSecConfirmModal')?.addEventListener('click', () => {
    document.getElementById('securityConfirmModal')?.classList.remove('active');
    pendingSecTxExecution = null;
  });

  // Create Account Modal Toggle & Type Switching
  document.getElementById('btnOpenCreateAccountModal')?.addEventListener('click', openCreateAccountModal);
  document.getElementById('btnCloseCreateModal')?.addEventListener('click', closeCreateAccountModal);
  document.getElementById('btnCancelCreateModal')?.addEventListener('click', closeCreateAccountModal);
  document.getElementById('btnCloseDetailModal')?.addEventListener('click', () => {
    document.getElementById('accountDetailModal')?.classList.remove('active');
  });

  let selectedAccType = 'savings';
  document.querySelectorAll('.modal-type-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.modal-type-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      selectedAccType = btn.dataset.type;

      const overGroup = document.getElementById('caOverdraftGroup');
      const fdGroup = document.getElementById('caFdGroup');
      const balHint = document.getElementById('caBalanceHint');
      const balInput = document.getElementById('caBalance');

      if (selectedAccType === 'savings') {
        overGroup.style.display = 'none';
        fdGroup.style.display = 'none';
        balHint.textContent = 'Savings accounts require a minimum balance of ₹1,000.';
        balInput.min = '1000';
      } else if (selectedAccType === 'current') {
        overGroup.style.display = 'block';
        fdGroup.style.display = 'none';
        balHint.textContent = 'Current account opening balance (0% interest transactional).';
        balInput.min = '0';
      } else if (selectedAccType === 'fd') {
        overGroup.style.display = 'none';
        fdGroup.style.display = 'block';
        balHint.textContent = 'Fixed deposit principal investment amount.';
        balInput.min = '1000';
        updateFdPreview();
      }
    });
  });

  const updateFdPreview = () => {
    const bal = parseFloat(document.getElementById('caBalance').value) || 0;
    const term = parseInt(document.getElementById('caFdTerm').value) || 12;
    const rate = (term >= 36) ? 0.075 : ((term >= 12) ? 0.070 : 0.065);
    const t = term / 12.0;
    const matVal = bal * Math.pow(1.0 + (rate / 4.0), 4.0 * t);

    document.getElementById('fdMaturityValPreview').textContent = formatRupees(matVal);
    document.getElementById('fdMaturityDatePreview').textContent = calculateFutureDate(term);
  };

  document.getElementById('caBalance')?.addEventListener('input', updateFdPreview);
  document.getElementById('caFdTerm')?.addEventListener('change', updateFdPreview);

  // Create Account Form Submission
  const caForm = document.getElementById('createAccountForm');
  if (caForm) {
    caForm.addEventListener('submit', (e) => {
      e.preventDefault();
      const name = document.getElementById('caName').value.trim();
      const balance = parseFloat(document.getElementById('caBalance').value);

      try {
        let newAcc = null;
        if (selectedAccType === 'savings') {
          if (balance < 1000) throw new BankingException('Savings account requires minimum opening deposit of ₹1,000.');
          newAcc = new SavingsAccount(name, balance);
        } else if (selectedAccType === 'current') {
          const overdraft = parseFloat(document.getElementById('caOverdraft').value) || 0;
          newAcc = new CurrentAccount(name, balance, formatDate(), overdraft);
        } else if (selectedAccType === 'fd') {
          const term = parseInt(document.getElementById('caFdTerm').value) || 12;
          const rate = (term >= 36) ? 0.075 : ((term >= 12) ? 0.070 : 0.065);
          newAcc = new FixedDepositAccount(name, balance, term, rate);
        }

        bank.addAccount(newAcc);
        showToast(`Opened ${newAcc.getAccountType()} #${newAcc.getAccountNumber()} for ${name}!`, 'success');
        closeCreateAccountModal();
        caForm.reset();
        refreshActivePage('accounts');
      } catch (err) {
        showToast(err.message, 'error');
      }
    });
  }

  // Statements Trigger
  document.getElementById('btnGenerateStatement')?.addEventListener('click', () => {
    const num = document.getElementById('stmtAccountInput').value;
    if (num) generateStatement(num);
  });

  // Settings: Rules update
  document.getElementById('settingsRulesForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const largeTh = parseFloat(document.getElementById('setLargeThreshold').value);
    const dailyLim = parseFloat(document.getElementById('setDailyLimit').value);
    const velCnt = parseInt(document.getElementById('setVelocityCount').value);

    bank.fraudDetector.config.largeTransactionThreshold = largeTh;
    bank.fraudDetector.config.dailyDebitLimit = dailyLim;
    bank.fraudDetector.config.rapidMaxCount = velCnt;
    bank.saveToStorage();

    document.getElementById('cfgRuleLargeTx').textContent = formatRupees(largeTh);
    document.getElementById('cfgRuleDaily').textContent = formatRupees(dailyLim);
    document.getElementById('cfgRuleVelocity').textContent = `${velCnt} transactions in 2 mins`;

    showToast('Security rules updated successfully!', 'success');
  });

  // Settings: Database actions
  document.getElementById('btnExportJSON')?.addEventListener('click', () => {
    const raw = localStorage.getItem('smartbank_db_v2');
    const blob = new Blob([raw], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `smartbank_backup_${formatDate()}.json`;
    a.click();
    showToast('Exported SmartBank database JSON.', 'success');
  });

  document.getElementById('btnResetDemoData')?.addEventListener('click', () => {
    if (confirm('Reload default demo accounts and transactions?')) {
      localStorage.removeItem('smartbank_db_v2');
      bank.seedDefaultData();
      showToast('Default demo accounts reloaded.', 'info');
      refreshActivePage('dashboard');
    }
  });

  document.getElementById('btnClearAllData')?.addEventListener('click', () => {
    if (confirm('Are you sure you want to wipe all records? This cannot be undone.')) {
      bank.accounts = [];
      bank.transactions = [];
      bank.fraudDetector.alerts = [];
      Account.nextAccountNumber = 1001;
      Transaction.nextTransactionId = 10001;
      FraudDetector.nextAlertId = 5001;
      bank.saveToStorage();
      showToast('All banking records have been cleared.', 'warning');
      refreshActivePage('dashboard');
    }
  });

  // Global search input in topbar
  document.getElementById('globalSearchInput')?.addEventListener('keyup', (e) => {
    if (e.key === 'Enter') {
      const q = e.target.value.trim();
      if (!q) return;

      if (q.toUpperCase().startsWith('TXN-')) {
        navigateTo('transactions');
        const inp = document.getElementById('txHistorySearch');
        if (inp) { inp.value = q; renderTransactionHistory(); }
      } else {
        try {
          const acc = bank.findAccount(q);
          openAccountDetailModal(acc.getAccountNumber());
        } catch {
          navigateTo('accounts');
          const inp = document.getElementById('accountSearchInput');
          if (inp) { inp.value = q; renderAccountsTable(); }
        }
      }
    }
  });

  // Account Filters
  document.getElementById('accountSearchInput')?.addEventListener('input', renderAccountsTable);
  document.getElementById('accountTypeFilter')?.addEventListener('change', renderAccountsTable);

  // Transaction Filters
  document.getElementById('txHistorySearch')?.addEventListener('input', renderTransactionHistory);
  document.getElementById('txTypeFilter')?.addEventListener('change', renderTransactionHistory);
  document.getElementById('txRiskFilter')?.addEventListener('change', renderTransactionHistory);

  // Initial Page Load
  navigateTo('dashboard');
});
