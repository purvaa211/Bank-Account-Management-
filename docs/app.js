/**
 * ==============================================================================
 * SmartBank - Smart Banking & Transaction Security Management System
 * Core Frontend JavaScript Engine & OOP Simulation
 * Course: Object-Oriented Programming (OOP) in C++
 * ==============================================================================
 */

// Format currency to Indian Rupees (Rs.)
function formatRupees(amount) {
  const num = Number(amount) || 0;
  return 'Rs. ' + num.toLocaleString('en-IN', { minimumFractionDigits: 2, maximumFractionDigits: 2 });
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

function calculateFutureDate(months, fromDateStr = '') {
  const d = fromDateStr ? new Date(fromDateStr) : new Date();
  d.setMonth(d.getMonth() + Number(months));
  return formatDate(d);
}

// Convert "YYYY-MM-DD HH:MM:SS" or ISO string to epoch milliseconds
function parseTimestampToEpoch(tsStr) {
  if (!tsStr) return Date.now();
  const clean = tsStr.replace(' ', 'T');
  const parsed = Date.parse(clean);
  return isNaN(parsed) ? Date.now() : parsed;
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

class CustomerNotFoundException extends BankingException {
  constructor(query) {
    super(`Customer not found! No registered customer matches: ${query}`);
    this.name = 'CustomerNotFoundException';
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

// Entity Class: Customer
class Customer {
  static nextCustomerId = 201;

  constructor(fullName, email, phone, custId = null) {
    this.customerId = custId ? Number(custId) : Customer.nextCustomerId++;
    this.fullName = fullName;
    this.email = email;
    this.phone = phone;
  }

  getCustomerId() { return this.customerId; }
  getFullName() { return this.fullName; }
  getEmail() { return this.email; }
  getPhone() { return this.phone; }

  setFullName(name) { this.fullName = name; }
  setEmail(email) { this.email = email; }
  setPhone(phone) { this.phone = phone; }
}

// Abstract Base Class: Account
class Account {
  static nextAccountNumber = 1001;

  constructor(customerId, name, balance, date = '', status = 'ACTIVE', accNum = null) {
    if (new.target === Account) {
      throw new TypeError('Cannot construct abstract instance of Account directly.');
    }
    this.accountNumber = accNum ? Number(accNum) : Account.nextAccountNumber++;
    this.customerId = Number(customerId) || 201;
    this.name = name;
    this.balance = Number(balance);
    this.status = status;
    this.openingDate = date || formatDate();
  }

  getAccountNumber() { return this.accountNumber; }
  getCustomerId() { return this.customerId; }
  setCustomerId(id) { this.customerId = Number(id); }
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
      throw new BankingException(`Cannot close account #${this.accountNumber}! Outstanding overdraft of ${formatRupees(this.balance)} must be settled first.`);
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
  constructor(customerId, name, balance, date = '', minBal = 1000.0, status = 'ACTIVE', accNum = null) {
    super(customerId, name, balance, date, status, accNum);
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
  constructor(customerId, name, balance, date = '', overdraft = 25000.0, status = 'ACTIVE', accNum = null) {
    super(customerId, name, balance, date, status, accNum);
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
  constructor(customerId, name, principal, term = 12, rate = 0.07, date = '', matAmt = null, matDate = null, status = 'ACTIVE', accNum = null) {
    super(customerId, name, principal, date, status, accNum);
    this.termMonths = Number(term);
    this.interestRate = Number(rate);
    this.maturityDate = matDate || calculateFutureDate(this.termMonths, this.openingDate);
    this.maturityAmount = matAmt !== null ? Number(matAmt) : this.computeMaturity();
  }

  computeMaturity() {
    const t = this.termMonths / 12.0;
    // Quarterly compounding formula: A = P * (1 + r/4)^(4*t)
    return this.balance * Math.pow(1.0 + (this.interestRate / 4.0), 4.0 * t);
  }

  getTermMonths() { return this.termMonths; }
  getMaturityDate() { return this.maturityDate; }
  getMaturityAmount() { return this.maturityAmount; }

  canWithdraw(amount, setReason) {
    if (amount <= 0) return false;
    if (setReason) {
      setReason(`Fixed Deposit funds are locked until maturity date (${this.maturityDate}). Premature liquidation requires formal account closure.`);
    }
    return false;
  }

  withdraw() {
    throw new BankingException(`Standard withdrawals are not permitted on Fixed Deposit accounts. Term lock-in active until ${this.maturityDate}. Close account to liquidate principal.`);
  }

  calculateInterest() {
    const yieldAmount = this.maturityAmount - this.balance;
    return {
      ratePercent: (this.interestRate * 100).toFixed(2),
      principal: this.balance,
      interest: yieldAmount,
      projectedBalance: this.maturityAmount,
      note: `Quarterly compounded fixed-term investment maturing on ${this.maturityDate}.`
    };
  }

  getAccountType() { return 'Fixed Deposit'; }
  getInterestRate() { return this.interestRate; }
}

// Transaction Model
class Transaction {
  static nextTransactionId = 10001;

  constructor(sender, receiver, type, amount, timestamp = '', status = 'SUCCESS', risk = 'LOW', reason = '', txId = null) {
    this.transactionId = txId || `TXN-${Transaction.nextTransactionId++}`;
    this.senderAccount = Number(sender);
    this.receiverAccount = Number(receiver);
    this.type = type.toUpperCase();
    this.amount = Number(amount);
    this.timestamp = timestamp || formatTimestamp();
    this.status = status;
    this.riskLevel = risk;
    this.flagReason = reason;
  }
}

// Rule-Based Fraud Detection Engine
class FraudDetector {
  static nextAlertId = 5001;

  constructor() {
    this.config = {
      largeTxThreshold: 50000.0,
      dailyDebitLimit: 100000.0,
      rapidVelocityCount: 3,
      rapidWindowMinutes: 2
    };
    this.alerts = [];
  }

  evaluateTransaction(tx, transactionHistory) {
    let riskLevel = 'LOW';
    let reasons = [];

    // Rule 1: Large Transaction Alert
    if (tx.amount >= (this.config.largeTxThreshold * 2.0)) {
      riskLevel = 'HIGH';
      reasons.push(`High-value transaction (${formatRupees(tx.amount)}) is >= 2x standard threshold (${formatRupees(this.config.largeTxThreshold)}).`);
    } else if (tx.amount >= this.config.largeTxThreshold) {
      if (riskLevel !== 'HIGH') riskLevel = 'MEDIUM';
      reasons.push(`Large transaction (${formatRupees(tx.amount)}) meets advisory threshold (${formatRupees(this.config.largeTxThreshold)}).`);
    }

    // Rule 2: Daily Cumulative Outgoing Debit Limit
    const today = (tx.timestamp || formatDate()).split(' ')[0];
    const sourceAcc = tx.senderAccount;

    if (sourceAcc > 0 && (tx.type === 'WITHDRAWAL' || tx.type === 'TRANSFER')) {
      const priorDailyDebit = transactionHistory
        .filter(t => t.senderAccount === sourceAcc && 
                     (t.type === 'WITHDRAWAL' || t.type === 'TRANSFER') &&
                     t.status === 'SUCCESS' && 
                     t.timestamp.startsWith(today))
        .reduce((sum, t) => sum + t.amount, 0);

      const projectedDailyDebit = priorDailyDebit + tx.amount;
      if (projectedDailyDebit > this.config.dailyDebitLimit) {
        riskLevel = 'HIGH';
        reasons.push(`Daily debit limit exceeded! Prior: ${formatRupees(priorDailyDebit)}, with this tx: ${formatRupees(projectedDailyDebit)} (Limit: ${formatRupees(this.config.dailyDebitLimit)}).`);
      }
    }

    // Rule 3: Rapid Velocity Burst (Epoch timestamp math)
    if (sourceAcc > 0) {
      const windowMillis = this.config.rapidWindowMinutes * 60 * 1000;
      const currentEpoch = parseTimestampToEpoch(tx.timestamp);

      const recentBurstCount = transactionHistory.filter(t => {
        if (t.senderAccount !== sourceAcc) return false;
        const diff = currentEpoch - parseTimestampToEpoch(t.timestamp);
        return diff >= 0 && diff <= windowMillis;
      }).length;

      if (recentBurstCount >= (this.config.rapidVelocityCount - 1)) {
        if (riskLevel !== 'HIGH') riskLevel = 'MEDIUM';
        reasons.push(`Rapid transaction velocity detected: ${recentBurstCount + 1} transactions within ${this.config.rapidWindowMinutes} minutes.`);
      }
    }

    const finalReason = reasons.length > 0 ? reasons.join(' | ') : 'Transaction conforms to standard risk parameters.';
    return {
      riskLevel,
      reason: finalReason,
      requiresReview: (riskLevel !== 'LOW')
    };
  }

  logAlert(alertData) {
    const alert = {
      alertId: FraudDetector.nextAlertId++,
      transactionId: alertData.transactionId,
      accountNumber: alertData.accountNumber,
      amount: alertData.amount,
      riskLevel: alertData.riskLevel,
      reason: alertData.reason,
      timestamp: alertData.timestamp || formatTimestamp(),
      reviewStatus: alertData.reviewStatus || 'PENDING_REVIEW'
    };
    this.alerts.unshift(alert);
    return alert;
  }

  updateReviewStatus(alertId, newStatus) {
    const a = this.alerts.find(item => item.alertId === Number(alertId));
    if (a) {
      a.reviewStatus = newStatus;
    }
  }

  getHighRiskCount() { return this.alerts.filter(a => a.riskLevel === 'HIGH').length; }
  getMediumRiskCount() { return this.alerts.filter(a => a.riskLevel === 'MEDIUM').length; }
  getApprovedCount() { return this.alerts.filter(a => a.reviewStatus === 'APPROVED').length; }
}

// ==============================================================================
// BANK CONTAINER & STORAGE SYNCHRONIZATION
// ==============================================================================
class Bank {
  constructor() {
    this.customers = [];
    this.accounts = [];
    this.transactions = [];
    this.fraudDetector = new FraudDetector();
    this.loadFromStorage();
  }

  // Customer Management
  addCustomer(customer) {
    this.customers.push(customer);
    this.saveToStorage();
    return customer;
  }

  findCustomer(query) {
    if (!query) throw new BankingException('Customer search query cannot be empty.');
    const clean = String(query).trim().toLowerCase();
    const cust = this.customers.find(c => 
      String(c.getCustomerId()) === clean ||
      `cust-${c.getCustomerId()}` === clean ||
      c.getFullName().toLowerCase().includes(clean)
    );
    if (!cust) throw new CustomerNotFoundException(query);
    return cust;
  }

  updateCustomerContact(custId, newEmail, newPhone) {
    const cust = this.findCustomer(custId);
    if (newEmail) cust.setEmail(newEmail);
    if (newPhone) cust.setPhone(newPhone);
    this.saveToStorage();
    return cust;
  }

  getAccountsByCustomerId(custId) {
    const id = Number(custId);
    return this.accounts.filter(a => a.getCustomerId() === id);
  }

  // Account Management
  addAccount(account) {
    this.accounts.push(account);
    this.saveToStorage();
    return account;
  }

  findAccount(query) {
    if (query === null || query === undefined || query === '') {
      throw new BankingException('Account search query cannot be empty.');
    }
    const clean = String(query).trim().toLowerCase();
    const acc = this.accounts.find(a => 
      String(a.getAccountNumber()) === clean || 
      a.getName().toLowerCase().includes(clean)
    );
    if (!acc) throw new AccountNotFoundException(query);
    return acc;
  }

  closeAccount(accNum) {
    const acc = this.findAccount(accNum);
    acc.closeAccount();
    const tx = new Transaction(acc.getAccountNumber(), -1, 'CLOSE_ACCOUNT', 0.0, formatTimestamp(), 'SUCCESS', 'LOW', 'Account closed by administrator');
    this.transactions.unshift(tx);
    this.saveToStorage();
  }

  deposit(accNum, amount, note = '') {
    const acc = this.findAccount(accNum);
    const prevBal = acc.getBalance();
    acc.deposit(amount);

    const tx = new Transaction(acc.getAccountNumber(), -1, 'DEPOSIT', amount, formatTimestamp(), 'SUCCESS', 'LOW', note || 'Standard deposit');
    const evalResult = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (evalResult.riskLevel !== 'LOW') {
      tx.riskLevel = evalResult.riskLevel;
      tx.flagReason = evalResult.reason;
      this.fraudDetector.logAlert({
        transactionId: tx.transactionId,
        accountNumber: acc.getAccountNumber(),
        amount: tx.amount,
        riskLevel: evalResult.riskLevel,
        reason: evalResult.reason,
        timestamp: tx.timestamp
      });
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, prevBal, newBal: acc.getBalance(), acc, evalResult };
  }

  withdraw(accNum, amount, note = '') {
    const acc = this.findAccount(accNum);
    const prevBal = acc.getBalance();
    acc.withdraw(amount);

    const tx = new Transaction(acc.getAccountNumber(), -1, 'WITHDRAWAL', amount, formatTimestamp(), 'SUCCESS', 'LOW', note || 'Cash withdrawal');
    const evalResult = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (evalResult.riskLevel !== 'LOW') {
      tx.riskLevel = evalResult.riskLevel;
      tx.flagReason = evalResult.reason;
      this.fraudDetector.logAlert({
        transactionId: tx.transactionId,
        accountNumber: acc.getAccountNumber(),
        amount: tx.amount,
        riskLevel: evalResult.riskLevel,
        reason: evalResult.reason,
        timestamp: tx.timestamp
      });
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, prevBal, newBal: acc.getBalance(), acc, evalResult };
  }

  transfer(senderNum, receiverNum, amount, note = '') {
    const sender = this.findAccount(senderNum);
    const receiver = this.findAccount(receiverNum);

    if (sender.getAccountNumber() === receiver.getAccountNumber()) {
      throw new BankingException('Transfer failed: Source and destination accounts cannot be identical.');
    }
    if (!sender.isActive()) throw new AccountClosedException(sender.getAccountNumber());
    if (!receiver.isActive()) throw new AccountClosedException(receiver.getAccountNumber());

    const amt = Number(amount);
    if (isNaN(amt) || amt <= 0) throw new InvalidAmountException();

    let reason = '';
    if (!sender.canWithdraw(amt, (r) => { reason = r; })) {
      throw new InsufficientBalanceException(sender.getBalance(), amt);
    }

    // Atomic execution
    const senderPrev = sender.getBalance();
    sender.withdraw(amt);
    receiver.deposit(amt);

    const tx = new Transaction(sender.getAccountNumber(), receiver.getAccountNumber(), 'TRANSFER', amt, formatTimestamp(), 'SUCCESS', 'LOW', note || `Transfer to Acc #${receiver.getAccountNumber()}`);
    const evalResult = this.fraudDetector.evaluateTransaction(tx, this.transactions);
    if (evalResult.riskLevel !== 'LOW') {
      tx.riskLevel = evalResult.riskLevel;
      tx.flagReason = evalResult.reason;
      this.fraudDetector.logAlert({
        transactionId: tx.transactionId,
        accountNumber: sender.getAccountNumber(),
        amount: tx.amount,
        riskLevel: evalResult.riskLevel,
        reason: evalResult.reason,
        timestamp: tx.timestamp
      });
    }
    this.transactions.unshift(tx);
    this.saveToStorage();
    return { tx, sender, receiver, senderPrev, senderNew: sender.getBalance(), evalResult };
  }

  getTotalDeposits() {
    return this.accounts
      .filter(a => a.isActive())
      .reduce((sum, a) => sum + Math.max(0, a.getBalance()), 0);
  }

  saveToStorage() {
    try {
      const data = {
        customers: this.customers.map(c => ({
          customerId: c.getCustomerId(),
          fullName: c.getFullName(),
          email: c.getEmail(),
          phone: c.getPhone()
        })),
        accounts: this.accounts.map(a => ({
          type: a.getAccountType(),
          accountNumber: a.getAccountNumber(),
          customerId: a.getCustomerId(),
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
        nextCustId: Customer.nextCustomerId,
        nextAccNum: Account.nextAccountNumber,
        nextTxId: Transaction.nextTransactionId,
        nextAlertId: FraudDetector.nextAlertId
      };
      localStorage.setItem('smartbank_db_v4', JSON.stringify(data));
    } catch (err) {
      console.warn('Storage sync notice:', err);
    }
  }

  loadFromStorage() {
    const raw = localStorage.getItem('smartbank_db_v4');
    if (!raw) {
      this.seedDefaultData();
      return;
    }

    try {
      const parsed = JSON.parse(raw);
      this.customers = (parsed.customers || []).map(c => 
        new Customer(c.fullName, c.email, c.phone, c.customerId)
      );

      this.accounts = (parsed.accounts || []).map(a => {
        const custId = a.customerId || 201;
        if (a.type === 'Savings Account') {
          return new SavingsAccount(custId, a.name, a.balance, a.openingDate, a.minBalance || 1000, a.status, a.accountNumber);
        } else if (a.type === 'Current Account') {
          return new CurrentAccount(custId, a.name, a.balance, a.openingDate, a.overdraftLimit || 25000, a.status, a.accountNumber);
        } else if (a.type === 'Fixed Deposit') {
          return new FixedDepositAccount(custId, a.name, a.balance, a.termMonths || 12, a.interestRate || 0.07, a.openingDate, a.maturityAmount, a.maturityDate, a.status, a.accountNumber);
        }
        return new SavingsAccount(custId, a.name, a.balance, a.openingDate, 1000, a.status, a.accountNumber);
      });

      this.transactions = (parsed.transactions || []).map(t => 
        new Transaction(t.senderAccount, t.receiverAccount, t.type, t.amount, t.timestamp, t.status, t.riskLevel, t.flagReason, t.transactionId)
      );

      this.fraudDetector.alerts = parsed.alerts || [];
      if (parsed.config) this.fraudDetector.config = parsed.config;

      Customer.nextCustomerId = parsed.nextCustId || 205;
      Account.nextAccountNumber = parsed.nextAccNum || 1006;
      Transaction.nextTransactionId = parsed.nextTxId || 10006;
      FraudDetector.nextAlertId = parsed.nextAlertId || 5003;

    } catch (e) {
      console.error('Failed to parse stored bank data, resetting to defaults:', e);
      this.seedDefaultData();
    }
  }

  seedDefaultData() {
    this.customers = [
      new Customer('Vikram Malhotra', 'vikram.malhotra@fintechcorp.in', '+91 98201 12345', 201),
      new Customer('Apex Global Enterprises', 'finance@apexglobal.com', '+91 98202 23456', 202),
      new Customer('Aarav Sharma', 'aarav.sharma@innovate.org', '+91 98203 34567', 203),
      new Customer('Priya Nair', 'priya.nair@techhub.in', '+91 98204 45678', 204)
    ];

    this.accounts = [
      new SavingsAccount(201, 'Vikram Malhotra', 75000.0, '2026-10-01', 1000.0, 'ACTIVE', 1001),
      new CurrentAccount(202, 'Apex Global Enterprises', 150000.0, '2026-10-05', 50000.0, 'ACTIVE', 1002),
      new SavingsAccount(203, 'Aarav Sharma', 45000.0, '2026-10-08', 1000.0, 'ACTIVE', 1003),
      new FixedDepositAccount(201, 'Vikram Malhotra', 100000.0, 12, 0.0725, '2026-10-10', 107449.54, calculateFutureDate(12, '2026-10-10'), 'ACTIVE', 1004),
      new SavingsAccount(204, 'Priya Nair', 20000.0, '2026-11-17', 1000.0, 'ACTIVE', 1005)
    ];

    this.transactions = [
      new Transaction(1001, -1, 'DEPOSIT', 25000.0, '2026-10-15 10:30:00', 'SUCCESS', 'LOW', 'Direct corporate deposit', 'TXN-10001'),
      new Transaction(1002, 1001, 'TRANSFER', 20000.0, '2026-10-20 14:15:00', 'SUCCESS', 'LOW', 'Monthly vendor settlement', 'TXN-10002'),
      new Transaction(1001, -1, 'DEPOSIT', 60000.0, '2026-11-05 11:20:00', 'SUCCESS', 'MEDIUM', 'Commercial real estate deposit', 'TXN-10003'),
      new Transaction(1002, -1, 'WITHDRAWAL', 70000.0, '2026-11-10 16:45:00', 'SUCCESS', 'MEDIUM', 'Equipment procurement cash withdrawal', 'TXN-10004'),
      new Transaction(1005, 1001, 'TRANSFER', 5000.0, '2026-11-17 23:01:22', 'SUCCESS', 'LOW', 'Initial activation transfer', 'TXN-10005')
    ];

    this.fraudDetector.alerts = [
      {
        alertId: 5001,
        transactionId: 'TXN-10003',
        accountNumber: 1001,
        amount: 60000.0,
        riskLevel: 'MEDIUM',
        reason: 'Large transaction (Rs. 60,000.00) meets advisory threshold (Rs. 50,000.00).',
        timestamp: '2026-11-05 11:20:00',
        reviewStatus: 'APPROVED'
      },
      {
        alertId: 5002,
        transactionId: 'TXN-10004',
        accountNumber: 1002,
        amount: 70000.0,
        riskLevel: 'MEDIUM',
        reason: 'Large transaction (Rs. 70,000.00) meets advisory threshold (Rs. 50,000.00).',
        timestamp: '2026-11-10 16:45:00',
        reviewStatus: 'PENDING_REVIEW'
      }
    ];

    Customer.nextCustomerId = 205;
    Account.nextAccountNumber = 1006;
    Transaction.nextTransactionId = 10006;
    FraudDetector.nextAlertId = 5003;
    this.saveToStorage();
  }
}

// Global Bank Instance
const bank = new Bank();

// ==============================================================================
// TOAST NOTIFICATIONS
// ==============================================================================
function showToast(message, type = 'info') {
  const container = document.getElementById('toastContainer');
  if (!container) return;

  const toast = document.createElement('div');
  toast.className = `toast toast-${type}`;
  toast.textContent = message;
  container.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateY(8px)';
    toast.style.transition = 'all 0.3s ease';
    setTimeout(() => toast.remove(), 300);
  }, 4000);
}

// ==============================================================================
// NAVIGATION & PAGE ROUTING
// ==============================================================================
function navigateTo(pageId, subAction = '') {
  document.querySelectorAll('.page-section').forEach(s => s.classList.remove('active'));
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));

  const target = document.getElementById(`page-${pageId}`);
  if (target) {
    target.classList.add('active');
  }

  const navLink = document.querySelector(`.nav-item[data-page="${pageId}"]`);
  if (navLink) {
    navLink.classList.add('active');
  }

  // Update Page Title
  const titles = {
    dashboard: ['Dashboard', 'Financial metrics, transaction flow and live security monitoring'],
    customers: ['Customer Directory', 'Registered customer entities holding specialized bank accounts'],
    accounts: ['Accounts Registry', 'Encapsulated account records with specialized interest and withdrawal rules'],
    transactions: ['Operations & Ledger', 'Execute deposits, withdrawals, atomic transfers and audit logs'],
    security: ['Security Center', 'Rule-based anomaly monitoring, threshold parameters and review queue'],
    statements: ['Bank Statements', 'Official account certified statement export using Friend Class pattern'],
    settings: ['Platform Settings', 'Security threshold tuning, JSON export and persistent storage controls']
  };

  const info = titles[pageId] || ['SmartBank Management', 'Core Banking & Security'];
  const titleEl = document.getElementById('pageTitleHeading');
  const subEl = document.getElementById('pageSubtitleHeading');
  if (titleEl) titleEl.textContent = info[0];
  if (subEl) subEl.textContent = info[1];

  // Sub action handling
  if (pageId === 'accounts' && subAction === 'create') {
    openCreateAccountModal();
  } else if (pageId === 'transactions' && subAction === 'transfer') {
    switchTxTab('transfer');
  }

  window.scrollTo({ top: 0, behavior: 'smooth' });
}

// Operation Tab Switching
function switchTxTab(tabKey) {
  document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  document.querySelectorAll('.tx-form').forEach(f => f.classList.remove('active'));

  const btn = document.querySelector(`.tab-btn[data-tx-tab="${tabKey}"]`);
  const form = document.getElementById(`${tabKey}Form`);
  if (btn) btn.classList.add('active');
  if (form) form.classList.add('active');
}

// ==============================================================================
// UI RENDERERS
// ==============================================================================

// Dashboard Refresh
function renderDashboard() {
  const statCust = document.getElementById('statCustomersCount');
  const statAcc = document.getElementById('statAccountsCount');
  const statDep = document.getElementById('statTotalDeposits');
  const statAlerts = document.getElementById('statAlertsCount');
  const statHigh = document.getElementById('statHighRiskSub');

  if (statCust) statCust.textContent = bank.customers.length;
  if (statAcc) statAcc.textContent = bank.accounts.filter(a => a.isActive()).length;
  if (statDep) statDep.textContent = formatRupees(bank.getTotalDeposits());
  if (statAlerts) statAlerts.textContent = bank.fraudDetector.alerts.length;
  if (statHigh) statHigh.textContent = `${bank.fraudDetector.getHighRiskCount()} High Risk`;

  // Risk breakdown badges
  const dLow = document.getElementById('dashLowRiskCount');
  const dMed = document.getElementById('dashMedRiskCount');
  const dHigh = document.getElementById('dashHighRiskCount');
  if (dLow) dLow.textContent = bank.transactions.filter(t => t.riskLevel === 'LOW').length;
  if (dMed) dMed.textContent = bank.fraudDetector.getMediumRiskCount();
  if (dHigh) dHigh.textContent = bank.fraudDetector.getHighRiskCount();

  // Sidebar alert badge
  const pendingCount = bank.fraudDetector.alerts.filter(a => a.reviewStatus === 'PENDING_REVIEW').length;
  const sideBadge = document.getElementById('sidebarAlertBadge');
  const headBadge = document.getElementById('headerAlertBadge');
  if (sideBadge) {
    sideBadge.textContent = pendingCount;
    sideBadge.style.display = pendingCount > 0 ? 'inline-block' : 'none';
  }
  if (headBadge) {
    headBadge.style.display = pendingCount > 0 ? 'inline-block' : 'none';
  }

  // Recent transactions table
  const tbody = document.getElementById('dashRecentTxnBody');
  if (tbody) {
    const recents = bank.transactions.slice(0, 5);
    if (recents.length === 0) {
      tbody.innerHTML = '<tr><td colspan="8" class="text-center text-muted">No transactions recorded yet.</td></tr>';
    } else {
      tbody.innerHTML = recents.map(t => `
        <tr>
          <td><strong>${t.transactionId}</strong></td>
          <td class="text-muted text-sm">${t.timestamp}</td>
          <td><span class="badge badge-neutral">${t.type}</span></td>
          <td>${t.senderAccount > 0 ? '#' + t.senderAccount : '-'}</td>
          <td>${t.receiverAccount > 0 ? '#' + t.receiverAccount : '-'}</td>
          <td><strong>${formatRupees(t.amount)}</strong></td>
          <td><span class="badge badge-success">${t.status}</span></td>
          <td><span class="risk-tag risk-${t.riskLevel.toLowerCase()}">${t.riskLevel}</span></td>
        </tr>
      `).join('');
    }
  }

  // Draw Charts
  drawCashFlowChart();
  drawAccountTypeChart();
}

// Customers Table Render
function renderCustomersTable(filterText = '') {
  const tbody = document.getElementById('customersTableBody');
  const badge = document.getElementById('customerCountBadge');
  if (!tbody) return;

  const clean = filterText.trim().toLowerCase();
  const list = bank.customers.filter(c => {
    if (!clean) return true;
    return (
      String(c.getCustomerId()).includes(clean) ||
      `cust-${c.getCustomerId()}`.includes(clean) ||
      c.getFullName().toLowerCase().includes(clean) ||
      c.getEmail().toLowerCase().includes(clean) ||
      c.getPhone().toLowerCase().includes(clean)
    );
  });

  if (badge) badge.textContent = `${list.length} Customers`;

  if (list.length === 0) {
    tbody.innerHTML = '<tr><td colspan="6" class="text-center text-muted">No customers match search criteria.</td></tr>';
    return;
  }

  tbody.innerHTML = list.map(c => {
    const ownedAccs = bank.getAccountsByCustomerId(c.getCustomerId());
    return `
      <tr>
        <td><strong>CUST-${c.getCustomerId()}</strong></td>
        <td class="font-600">${c.getFullName()}</td>
        <td class="text-secondary text-sm">${c.getEmail()}</td>
        <td class="text-secondary text-sm">${c.getPhone()}</td>
        <td>
          <span class="badge ${ownedAccs.length > 0 ? 'badge-forest' : 'badge-neutral'}">
            ${ownedAccs.length} Account${ownedAccs.length === 1 ? '' : 's'}
          </span>
        </td>
        <td>
          <button class="btn btn-secondary btn-sm" onclick="openCustomerAccountsModal(${c.getCustomerId()})">View Accounts</button>
          <button class="btn btn-secondary btn-sm" onclick="openEditCustomerModal(${c.getCustomerId()})">Edit Contact</button>
        </td>
      </tr>
    `;
  }).join('');
}

// Accounts Table Render
function renderAccountsTable() {
  const tbody = document.getElementById('accountsTableBody');
  const badge = document.getElementById('accountCountBadge');
  if (!tbody) return;

  const searchVal = (document.getElementById('accountSearchInput')?.value || '').trim().toLowerCase();
  const typeFilter = document.getElementById('accountTypeFilter')?.value || 'ALL';

  const filtered = bank.accounts.filter(a => {
    if (typeFilter !== 'ALL' && a.getAccountType() !== typeFilter) return false;
    if (searchVal) {
      const matchNum = String(a.getAccountNumber()).includes(searchVal);
      const matchCust = String(a.getCustomerId()).includes(searchVal) || `cust-${a.getCustomerId()}`.includes(searchVal);
      const matchName = a.getName().toLowerCase().includes(searchVal);
      return matchNum || matchCust || matchName;
    }
    return true;
  });

  if (badge) badge.textContent = `${filtered.length} Accounts`;

  if (filtered.length === 0) {
    tbody.innerHTML = '<tr><td colspan="8" class="text-center text-muted">No accounts match search criteria.</td></tr>';
    return;
  }

  tbody.innerHTML = filtered.map(a => {
    let features = '';
    if (a instanceof SavingsAccount) {
      features = `Min Bal: ${formatRupees(a.getMinimumBalance())} | 4.0% p.a.`;
    } else if (a instanceof CurrentAccount) {
      features = `Overdraft Limit: ${formatRupees(a.getOverdraftLimit())}`;
    } else if (a instanceof FixedDepositAccount) {
      features = `Term: ${a.getTermMonths()}m @ ${(a.getInterestRate() * 100).toFixed(2)}% | Matures: ${a.getMaturityDate()}`;
    }

    const isClosed = a.getStatus() === 'CLOSED';

    return `
      <tr class="${isClosed ? 'text-muted' : ''}">
        <td><strong>#${a.getAccountNumber()}</strong></td>
        <td><span class="badge badge-brass">CUST-${a.getCustomerId()}</span></td>
        <td class="font-600">${a.getName()}</td>
        <td><span class="badge badge-neutral">${a.getAccountType()}</span></td>
        <td class="text-sm text-secondary">${features}</td>
        <td><strong class="${a.getBalance() >= 0 ? 'text-green' : 'text-red'}">${formatRupees(a.getBalance())}</strong></td>
        <td>
          <span class="badge ${a.isActive() ? 'badge-success' : 'badge-danger'}">${a.getStatus()}</span>
        </td>
        <td>
          <button class="btn btn-secondary btn-sm" onclick="openAccountDetailModal(${a.getAccountNumber()})">Passbook</button>
          ${a.isActive() ? `<button class="btn btn-danger btn-sm" onclick="handleCloseAccount(${a.getAccountNumber()})">Close</button>` : ''}
        </td>
      </tr>
    `;
  }).join('');
}

// Transactions Table Render
function renderTransactionsTable() {
  const tbody = document.getElementById('transactionHistoryTableBody');
  if (!tbody) return;

  const searchVal = (document.getElementById('txHistorySearch')?.value || '').trim().toLowerCase();
  const typeFilter = document.getElementById('txTypeFilter')?.value || 'ALL';
  const riskFilter = document.getElementById('txRiskFilter')?.value || 'ALL';

  const filtered = bank.transactions.filter(t => {
    if (typeFilter !== 'ALL' && t.type !== typeFilter) return false;
    if (riskFilter !== 'ALL' && t.riskLevel !== riskFilter) return false;
    if (searchVal) {
      const matchId = t.transactionId.toLowerCase().includes(searchVal);
      const matchSender = String(t.senderAccount).includes(searchVal);
      const matchReceiver = String(t.receiverAccount).includes(searchVal);
      return matchId || matchSender || matchReceiver;
    }
    return true;
  });

  if (filtered.length === 0) {
    tbody.innerHTML = '<tr><td colspan="9" class="text-center text-muted">No transactions matching selected filters.</td></tr>';
    return;
  }

  tbody.innerHTML = filtered.map(t => `
    <tr>
      <td><strong>${t.transactionId}</strong></td>
      <td class="text-sm text-muted">${t.timestamp}</td>
      <td><span class="badge badge-neutral">${t.type}</span></td>
      <td>${t.senderAccount > 0 ? '#' + t.senderAccount : '-'}</td>
      <td>${t.receiverAccount > 0 ? '#' + t.receiverAccount : '-'}</td>
      <td><strong>${formatRupees(t.amount)}</strong></td>
      <td><span class="badge badge-success">${t.status}</span></td>
      <td><span class="risk-tag risk-${t.riskLevel.toLowerCase()}">${t.riskLevel}</span></td>
      <td class="text-sm text-secondary">${t.flagReason || '-'}</td>
    </tr>
  `).join('');
}

// Security Center Table Render
function renderSecurityTable() {
  const tbody = document.getElementById('securityAlertsTableBody');
  if (!tbody) return;

  const totalEl = document.getElementById('secTotalAlerts');
  const highEl = document.getElementById('secHighRiskCount');
  const medEl = document.getElementById('secMedRiskCount');
  const appEl = document.getElementById('secApprovedCount');

  if (totalEl) totalEl.textContent = bank.fraudDetector.alerts.length;
  if (highEl) highEl.textContent = bank.fraudDetector.getHighRiskCount();
  if (medEl) medEl.textContent = bank.fraudDetector.getMediumRiskCount();
  if (appEl) appEl.textContent = bank.fraudDetector.getApprovedCount();

  if (bank.fraudDetector.alerts.length === 0) {
    tbody.innerHTML = '<tr><td colspan="9" class="text-center text-muted">No security anomaly events recorded.</td></tr>';
    return;
  }

  tbody.innerHTML = bank.fraudDetector.alerts.map(a => `
    <tr>
      <td><strong>ALT-${a.alertId}</strong></td>
      <td><strong>${a.transactionId}</strong></td>
      <td>#${a.accountNumber}</td>
      <td><strong>${formatRupees(a.amount)}</strong></td>
      <td><span class="risk-tag risk-${a.riskLevel.toLowerCase()}">${a.riskLevel}</span></td>
      <td class="text-sm text-secondary">${a.reason}</td>
      <td class="text-sm text-muted">${a.timestamp}</td>
      <td>
        <span class="badge ${a.reviewStatus === 'APPROVED' ? 'badge-success' : (a.reviewStatus === 'REJECTED' ? 'badge-danger' : 'badge-warning')}">
          ${a.reviewStatus}
        </span>
      </td>
      <td>
        ${a.reviewStatus === 'PENDING_REVIEW' ? `
          <button class="btn btn-secondary btn-sm" onclick="reviewAlert(${a.alertId}, 'APPROVED')">Clear</button>
          <button class="btn btn-danger btn-sm" onclick="reviewAlert(${a.alertId}, 'REJECTED')">Flag</button>
        ` : `<span class="text-sm text-muted">Reviewed</span>`}
      </td>
    </tr>
  `).join('');
}

// Statement Generation
function generateStatement(accNum) {
  const acc = bank.findAccount(accNum);
  const cust = bank.findCustomer(acc.getCustomerId());

  const curDateEl = document.getElementById('stmtCurrentDate');
  const custIdEl = document.getElementById('stmtCustomerId');
  const holderEl = document.getElementById('stmtHolderName');
  const numEl = document.getElementById('stmtAccountNum');
  const typeEl = document.getElementById('stmtAccountType');
  const balEl = document.getElementById('stmtClosingBalance');
  const tbody = document.getElementById('stmtLedgerBody');

  if (curDateEl) curDateEl.textContent = `Date: ${formatDate()}`;
  if (custIdEl) custIdEl.textContent = `CUST-${cust.getCustomerId()}`;
  if (holderEl) holderEl.textContent = `${cust.getFullName()} (${cust.getEmail()})`;
  if (numEl) numEl.textContent = acc.getAccountNumber();
  if (typeEl) typeEl.textContent = acc.getAccountType();
  if (balEl) balEl.textContent = formatRupees(acc.getBalance());

  // Filter transactions for this account
  const accountTxns = bank.transactions.filter(t => 
    t.senderAccount === acc.getAccountNumber() || t.receiverAccount === acc.getAccountNumber()
  );

  if (accountTxns.length === 0) {
    tbody.innerHTML = '<tr><td colspan="6" class="text-center text-muted">No transaction history found for this account.</td></tr>';
    return;
  }

  tbody.innerHTML = accountTxns.map(t => {
    const isOutflow = (t.senderAccount === acc.getAccountNumber() && t.type !== 'DEPOSIT');
    const flowClass = isOutflow ? 'text-red' : 'text-green';
    const sign = isOutflow ? '-' : '+';
    return `
      <tr>
        <td class="text-sm text-muted">${t.timestamp}</td>
        <td><strong>${t.transactionId}</strong></td>
        <td class="text-sm">${t.flagReason || t.type}</td>
        <td><span class="badge badge-neutral">${t.type}</span></td>
        <td><strong class="${flowClass}">${sign}${formatRupees(t.amount)}</strong></td>
        <td><span class="badge badge-success">${t.status}</span></td>
      </tr>
    `;
  }).join('');
}

// ==============================================================================
// MODAL CONTROLLERS
// ==============================================================================

// Register Customer Modal
function openRegisterCustomerModal() {
  const modal = document.getElementById('registerCustomerModal');
  const form = document.getElementById('registerCustomerForm');
  if (form) form.reset();
  if (modal) modal.classList.add('active');
}

function closeRegisterCustomerModal() {
  const modal = document.getElementById('registerCustomerModal');
  if (modal) modal.classList.remove('active');
}

// Edit Customer Contact Modal
function openEditCustomerModal(custId) {
  const cust = bank.findCustomer(custId);
  const modal = document.getElementById('editCustomerModal');
  document.getElementById('editCustId').value = cust.getCustomerId();
  document.getElementById('editCustSubtitle').textContent = `Customer CUST-${cust.getCustomerId()}`;
  document.getElementById('editCustName').value = cust.getFullName();
  document.getElementById('editCustEmail').value = cust.getEmail();
  document.getElementById('editCustPhone').value = cust.getPhone();
  if (modal) modal.classList.add('active');
}

function closeEditCustomerModal() {
  const modal = document.getElementById('editCustomerModal');
  if (modal) modal.classList.remove('active');
}

// Customer Accounts Modal (Passbook view for customer)
function openCustomerAccountsModal(custId) {
  const cust = bank.findCustomer(custId);
  const ownedAccs = bank.getAccountsByCustomerId(cust.getCustomerId());
  const modal = document.getElementById('customerAccountsModal');
  const sub = document.getElementById('custAccModalSubtitle');
  const body = document.getElementById('customerAccountsModalBody');

  if (sub) sub.textContent = `Portfolio of ${cust.getFullName()} (CUST-${cust.getCustomerId()}) • ${cust.getEmail()}`;

  if (ownedAccs.length === 0) {
    body.innerHTML = `
      <div class="empty-state">
        <h4>No Accounts Linked</h4>
        <p>This customer does not currently hold any bank accounts.</p>
        <button class="btn btn-primary mt-2" onclick="closeCustomerAccountsModal(); openCreateAccountModal(${cust.getCustomerId()})">Open Account for Customer</button>
      </div>
    `;
  } else {
    body.innerHTML = `
      <div class="table-wrapper">
        <table class="data-table">
          <thead>
            <tr>
              <th>Account #</th>
              <th>Account Type</th>
              <th>Current Balance</th>
              <th>Status</th>
              <th>Opened On</th>
              <th>Actions</th>
            </tr>
          </thead>
          <tbody>
            ${ownedAccs.map(a => `
              <tr>
                <td><strong>#${a.getAccountNumber()}</strong></td>
                <td><span class="badge badge-neutral">${a.getAccountType()}</span></td>
                <td><strong class="text-green">${formatRupees(a.getBalance())}</strong></td>
                <td><span class="badge ${a.isActive() ? 'badge-success' : 'badge-danger'}">${a.getStatus()}</span></td>
                <td class="text-sm text-muted">${a.getOpeningDate()}</td>
                <td>
                  <button class="btn btn-secondary btn-sm" onclick="closeCustomerAccountsModal(); openAccountDetailModal(${a.getAccountNumber()})">Passbook</button>
                </td>
              </tr>
            `).join('')}
          </tbody>
        </table>
      </div>
      <div class="modal-footer">
        <button class="btn btn-primary" onclick="closeCustomerAccountsModal(); openCreateAccountModal(${cust.getCustomerId()})">Open Another Account</button>
      </div>
    `;
  }

  if (modal) modal.classList.add('active');
}

function closeCustomerAccountsModal() {
  const modal = document.getElementById('customerAccountsModal');
  if (modal) modal.classList.remove('active');
}

// Create Account Modal
let currentCreateType = 'savings';

function openCreateAccountModal(preselectCustId = null) {
  const modal = document.getElementById('createAccountModal');
  const custSelect = document.getElementById('caCustomerId');

  // Populate customers dropdown
  if (custSelect) {
    custSelect.innerHTML = bank.customers.map(c => 
      `<option value="${c.getCustomerId()}" ${preselectCustId && c.getCustomerId() === Number(preselectCustId) ? 'selected' : ''}>
        CUST-${c.getCustomerId()} - ${c.getFullName()} (${c.getEmail()})
      </option>`
    ).join('');
  }

  setAccountModalType('savings');
  updateFdCalculations();
  if (modal) modal.classList.add('active');
}

function closeCreateAccountModal() {
  const modal = document.getElementById('createAccountModal');
  if (modal) modal.classList.remove('active');
}

function setAccountModalType(type) {
  currentCreateType = type;
  document.querySelectorAll('.modal-type-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-type') === type);
  });

  const odGroup = document.getElementById('caOverdraftGroup');
  const fdGroup = document.getElementById('caFdGroup');
  const hint = document.getElementById('caBalanceHint');
  const balInput = document.getElementById('caBalance');

  if (type === 'savings') {
    if (odGroup) odGroup.style.display = 'none';
    if (fdGroup) fdGroup.style.display = 'none';
    if (hint) hint.textContent = 'Savings accounts require a minimum balance of Rs. 1,000.';
    if (balInput) { balInput.min = '1000'; balInput.value = '1000'; }
  } else if (type === 'current') {
    if (odGroup) odGroup.style.display = 'block';
    if (fdGroup) fdGroup.style.display = 'none';
    if (hint) hint.textContent = 'Commercial checking account with overdraft facility. Opening balance may be Rs. 0 or higher.';
    if (balInput) { balInput.min = '0'; balInput.value = '5000'; }
  } else if (type === 'fd') {
    if (odGroup) odGroup.style.display = 'none';
    if (fdGroup) fdGroup.style.display = 'block';
    if (hint) hint.textContent = 'Fixed term locked investment. Principal is securely held until guaranteed maturity date.';
    if (balInput) { balInput.min = '1000'; balInput.value = '100000'; }
    updateFdCalculations();
  }
}

function updateFdCalculations() {
  const principal = Number(document.getElementById('caBalance')?.value) || 0;
  const termMonths = Number(document.getElementById('caFdTerm')?.value) || 12;

  let rate = 0.07;
  if (termMonths === 6) rate = 0.065;
  else if (termMonths === 12) rate = 0.070;
  else if (termMonths === 24) rate = 0.0725;
  else if (termMonths === 36) rate = 0.075;

  const t = termMonths / 12.0;
  const maturityVal = principal * Math.pow(1.0 + (rate / 4.0), 4.0 * t);
  const maturityDate = calculateFutureDate(termMonths);

  const pEl = document.getElementById('fdPrincipalValPreview');
  const mEl = document.getElementById('fdMaturityValPreview');
  const dEl = document.getElementById('fdMaturityDatePreview');

  if (pEl) pEl.textContent = formatRupees(principal);
  if (mEl) mEl.textContent = formatRupees(maturityVal);
  if (dEl) dEl.textContent = maturityDate;
}

// Account Passbook Detail Modal
function openAccountDetailModal(accNum) {
  const acc = bank.findAccount(accNum);
  const cust = bank.findCustomer(acc.getCustomerId());
  const modal = document.getElementById('accountDetailModal');
  const subtitle = document.getElementById('modalAccSubtitle');
  const body = document.getElementById('accountDetailModalBody');

  if (subtitle) subtitle.textContent = `Account #${acc.getAccountNumber()} • ${acc.getAccountType()}`;

  let specificDetails = '';
  if (acc instanceof SavingsAccount) {
    const calc = acc.calculateInterest();
    specificDetails = `
      <div class="passbook-cell">
        <span class="lbl">Interest Rate</span>
        <span class="val text-green">${calc.ratePercent}% p.a.</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Minimum Balance Requirement</span>
        <span class="val">${formatRupees(acc.getMinimumBalance())}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Projected Annual Interest</span>
        <span class="val text-green">${formatRupees(calc.interest)}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Withdrawal Eligibility</span>
        <span class="val text-green">Available balance minus Rs. 1,000</span>
      </div>
    `;
  } else if (acc instanceof CurrentAccount) {
    specificDetails = `
      <div class="passbook-cell">
        <span class="lbl">Approved Overdraft Facility</span>
        <span class="val text-brass">${formatRupees(acc.getOverdraftLimit())}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Maximum Withdrawal Power</span>
        <span class="val">${formatRupees(acc.getBalance() + acc.getOverdraftLimit())}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Interest Accrual</span>
        <span class="val text-muted">0.0% (Checking Account)</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Credit Status</span>
        <span class="val ${acc.getBalance() < 0 ? 'text-red' : 'text-green'}">${acc.getBalance() < 0 ? 'OVERDRAFT IN USE' : 'STANDARD POSITIVE'}</span>
      </div>
    `;
  } else if (acc instanceof FixedDepositAccount) {
    specificDetails = `
      <div class="passbook-cell">
        <span class="lbl">Investment Term</span>
        <span class="val">${acc.getTermMonths()} Months</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Agreed Interest Rate</span>
        <span class="val text-green">${(acc.getInterestRate() * 100).toFixed(2)}% p.a.</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Principal Ledger Balance</span>
        <span class="val">${formatRupees(acc.getBalance())}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Projected Guaranteed Maturity</span>
        <span class="val text-green font-600">${formatRupees(acc.getMaturityAmount())}</span>
      </div>
      <div class="passbook-cell" style="grid-column: span 2;">
        <span class="lbl">Contract Maturity Date</span>
        <span class="val text-brass">${acc.getMaturityDate()} (Locked until maturity)</span>
      </div>
    `;
  }

  body.innerHTML = `
    <div class="passbook-grid">
      <div class="passbook-cell">
        <span class="lbl">Account Number</span>
        <span class="val">#${acc.getAccountNumber()}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Customer ID</span>
        <span class="val">CUST-${cust.getCustomerId()}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Account Holder</span>
        <span class="val">${cust.getFullName()}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Contact Information</span>
        <span class="val text-sm">${cust.getEmail()} | ${cust.getPhone()}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Account Status</span>
        <span class="val ${acc.isActive() ? 'text-green' : 'text-red'}">${acc.getStatus()}</span>
      </div>
      <div class="passbook-cell">
        <span class="lbl">Opening Date</span>
        <span class="val">${acc.getOpeningDate()}</span>
      </div>
      <div class="passbook-cell" style="grid-column: span 2; background: var(--color-forest-subtle);">
        <span class="lbl" style="color: var(--color-forest);">Current Book Balance</span>
        <span class="val" style="font-size: 1.35rem; color: var(--color-forest);">${formatRupees(acc.getBalance())}</span>
      </div>
      ${specificDetails}
    </div>
    <div class="modal-footer">
      <button class="btn btn-secondary" onclick="closeAccountDetailModal()">Close Passbook</button>
      <button class="btn btn-primary" onclick="closeAccountDetailModal(); navigateTo('statements'); document.getElementById('stmtAccountInput').value=${acc.getAccountNumber()}; generateStatement(${acc.getAccountNumber()});">View Statement</button>
    </div>
  `;

  if (modal) modal.classList.add('active');
}

function closeAccountDetailModal() {
  const modal = document.getElementById('accountDetailModal');
  if (modal) modal.classList.remove('active');
}

// Close Account Handler
function handleCloseAccount(accNum) {
  const acc = bank.findAccount(accNum);
  if (!confirm(`Are you certain you wish to permanently terminate Account #${acc.getAccountNumber()} (${acc.getName()})?\n\nCurrent Balance: ${formatRupees(acc.getBalance())}`)) {
    return;
  }

  try {
    bank.closeAccount(accNum);
    showToast(`Account #${accNum} has been closed.`, 'warning');
    refreshAllViews();
  } catch (err) {
    showToast(err.message, 'error');
  }
}

// Alert Review Handler
function reviewAlert(alertId, newStatus) {
  bank.fraudDetector.updateReviewStatus(alertId, newStatus);
  bank.saveToStorage();
  showToast(`Security alert ALT-${alertId} marked as ${newStatus}.`, 'success');
  renderSecurityTable();
  renderDashboard();
}

// Render Real-time Audit Receipt
function renderReceipt(tx, extra = {}) {
  const container = document.getElementById('receiptOutputContainer');
  if (!container) return;

  container.innerHTML = `
    <div class="receipt-box">
      <div class="receipt-header">
        <h4>SMARTBANK TRANSACTION RECEIPT</h4>
        <span class="text-sm text-muted">Core Banking Certification Voucher</span>
      </div>
      <div class="receipt-line">
        <span>Transaction ID:</span>
        <strong>${tx.transactionId}</strong>
      </div>
      <div class="receipt-line">
        <span>Timestamp:</span>
        <span>${tx.timestamp}</span>
      </div>
      <div class="receipt-line">
        <span>Operation Type:</span>
        <span class="badge badge-neutral">${tx.type}</span>
      </div>
      <div class="receipt-line">
        <span>Source Account:</span>
        <span>${tx.senderAccount > 0 ? '#' + tx.senderAccount : 'EXTERNAL CASH / DEPOSIT'}</span>
      </div>
      ${tx.receiverAccount > 0 ? `
        <div class="receipt-line">
          <span>Destination Account:</span>
          <span>#${tx.receiverAccount}</span>
        </div>
      ` : ''}
      <div class="receipt-line">
        <span>Transaction Amount:</span>
        <strong style="font-size: 1.05rem; color: var(--color-forest);">${formatRupees(tx.amount)}</strong>
      </div>
      ${extra.senderNew !== undefined ? `
        <div class="receipt-line">
          <span>Sender New Balance:</span>
          <strong>${formatRupees(extra.senderNew)}</strong>
        </div>
      ` : ''}
      ${extra.newBal !== undefined ? `
        <div class="receipt-line">
          <span>Updated Balance:</span>
          <strong>${formatRupees(extra.newBal)}</strong>
        </div>
      ` : ''}
      <div class="receipt-line">
        <span>Execution Status:</span>
        <span class="badge badge-success">${tx.status}</span>
      </div>
      <div class="receipt-line">
        <span>Security Risk Level:</span>
        <span class="risk-tag risk-${tx.riskLevel.toLowerCase()}">${tx.riskLevel}</span>
      </div>
      ${tx.flagReason ? `
        <div class="receipt-line" style="flex-direction: column; align-items: flex-start; gap: 0.2rem; margin-top: 0.4rem; padding-top: 0.4rem; border-top: 1px dashed var(--border-medium);">
          <span class="text-sm text-muted">Security Tag / Reason:</span>
          <span class="text-sm font-600">${tx.flagReason}</span>
        </div>
      ` : ''}
    </div>
  `;
}

// Refresh all active tables and components
function refreshAllViews() {
  renderDashboard();
  renderCustomersTable();
  renderAccountsTable();
  renderTransactionsTable();
  renderSecurityTable();
}

// ==============================================================================
// CHARTS (Canvas Implementation)
// ==============================================================================
function drawCashFlowChart() {
  const canvas = document.getElementById('cashFlowChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width = canvas.parentElement.clientWidth || 360;
  const h = canvas.height = 220;

  ctx.clearRect(0, 0, w, h);

  const totalIn = bank.transactions
    .filter(t => t.type === 'DEPOSIT' && t.status === 'SUCCESS')
    .reduce((sum, t) => sum + t.amount, 0);

  const totalOut = bank.transactions
    .filter(t => (t.type === 'WITHDRAWAL' || t.type === 'TRANSFER') && t.status === 'SUCCESS')
    .reduce((sum, t) => sum + t.amount, 0);

  const maxVal = Math.max(totalIn, totalOut, 50000);
  const barWidth = 70;
  const gap = 60;
  const startX = (w - (barWidth * 2 + gap)) / 2;
  const bottomY = h - 45;
  const chartHeight = h - 70;

  // Draw background grid lines
  ctx.strokeStyle = '#E5E0D5';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(30, bottomY);
  ctx.lineTo(w - 30, bottomY);
  ctx.stroke();

  // Draw Inflows Bar (Forest Green)
  const inBarH = (totalIn / maxVal) * chartHeight;
  ctx.fillStyle = '#183D32';
  ctx.fillRect(startX, bottomY - inBarH, barWidth, inBarH);

  ctx.fillStyle = '#292D29';
  ctx.font = '600 11px Inter, sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText('Inflows', startX + barWidth / 2, bottomY + 18);
  ctx.font = '700 11px JetBrains Mono, monospace';
  ctx.fillText(formatRupees(totalIn), startX + barWidth / 2, bottomY - inBarH - 8);

  // Draw Outflows Bar (Brass / Warm Ochre)
  const outBarH = (totalOut / maxVal) * chartHeight;
  const outX = startX + barWidth + gap;
  ctx.fillStyle = '#C5AD7D';
  ctx.fillRect(outX, bottomY - outBarH, barWidth, outBarH);

  ctx.fillStyle = '#292D29';
  ctx.font = '600 11px Inter, sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText('Outflows', outX + barWidth / 2, bottomY + 18);
  ctx.font = '700 11px JetBrains Mono, monospace';
  ctx.fillText(formatRupees(totalOut), outX + barWidth / 2, bottomY - outBarH - 8);
}

function drawAccountTypeChart() {
  const canvas = document.getElementById('accountTypeChart');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width = canvas.parentElement.clientWidth || 360;
  const h = canvas.height = 220;

  ctx.clearRect(0, 0, w, h);

  const counts = {
    savings: bank.accounts.filter(a => a instanceof SavingsAccount && a.isActive()).length,
    current: bank.accounts.filter(a => a instanceof CurrentAccount && a.isActive()).length,
    fd: bank.accounts.filter(a => a instanceof FixedDepositAccount && a.isActive()).length
  };

  const total = counts.savings + counts.current + counts.fd || 1;
  const centerX = w * 0.38;
  const centerY = h * 0.5;
  const radius = Math.min(centerX, centerY) - 20;
  const innerRadius = radius * 0.6;

  const data = [
    { label: 'Savings', count: counts.savings, color: '#183D32' },
    { label: 'Current', count: counts.current, color: '#C5AD7D' },
    { label: 'Fixed Deposit', count: counts.fd, color: '#1D6F42' }
  ];

  let startAngle = -Math.PI / 2;

  data.forEach(item => {
    const sliceAngle = (item.count / total) * 2 * Math.PI;
    ctx.beginPath();
    ctx.arc(centerX, centerY, radius, startAngle, startAngle + sliceAngle);
    ctx.arc(centerX, centerY, innerRadius, startAngle + sliceAngle, startAngle, true);
    ctx.closePath();
    ctx.fillStyle = item.color;
    ctx.fill();
    startAngle += sliceAngle;
  });

  // Center text
  ctx.fillStyle = '#292D29';
  ctx.font = '700 18px JetBrains Mono, monospace';
  ctx.textAlign = 'center';
  ctx.textBaseline = 'middle';
  ctx.fillText(counts.savings + counts.current + counts.fd, centerX, centerY - 2);
  ctx.font = '500 10px Inter, sans-serif';
  ctx.fillStyle = '#7E857D';
  ctx.fillText('ACCOUNTS', centerX, centerY + 14);

  // Legend
  const legendX = w * 0.72;
  let legendY = 55;
  ctx.textAlign = 'left';
  ctx.textBaseline = 'alphabetic';

  data.forEach(item => {
    ctx.fillStyle = item.color;
    ctx.fillRect(legendX, legendY - 9, 12, 12);
    ctx.fillStyle = '#292D29';
    ctx.font = '500 11px Inter, sans-serif';
    ctx.fillText(`${item.label}: ${item.count}`, legendX + 18, legendY);
    legendY += 28;
  });
}

// ==============================================================================
// EVENT LISTENERS & APPLICATION BOOTSTRAP
// ==============================================================================
document.addEventListener('DOMContentLoaded', () => {

  // 1. Splash Screen Timeout
  setTimeout(() => {
    const splash = document.getElementById('splashScreen');
    if (splash) splash.classList.add('fade-out');
  }, 1200);

  // 2. Navigation Item Clicks
  document.querySelectorAll('.nav-item').forEach(link => {
    link.addEventListener('click', (e) => {
      e.preventDefault();
      const page = link.getAttribute('data-page');
      if (page) navigateTo(page);
    });
  });

  // 3. Operation Tab Switching
  document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      const tab = btn.getAttribute('data-tx-tab');
      if (tab) switchTxTab(tab);
    });
  });

  // 4. Modal Type Selector Tabs
  document.querySelectorAll('.modal-type-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      const type = btn.getAttribute('data-type');
      if (type) setAccountModalType(type);
    });
  });

  // 5. Dynamic Form Account Peeks
  const setupPeek = (inputId, peekId) => {
    const input = document.getElementById(inputId);
    const peek = document.getElementById(peekId);
    if (!input || !peek) return;
    input.addEventListener('input', () => {
      const val = input.value.trim();
      if (!val) { peek.textContent = ''; return; }
      try {
        const acc = bank.findAccount(val);
        const cust = bank.findCustomer(acc.getCustomerId());
        peek.textContent = `[Verified] ${cust.getFullName()} (#${acc.getAccountNumber()} • ${acc.getAccountType()} • Balance: ${formatRupees(acc.getBalance())})`;
        peek.style.color = 'var(--color-forest)';
      } catch (e) {
        peek.textContent = `[Notice] No active account matching #${val}`;
        peek.style.color = 'var(--color-danger)';
      }
    });
  };

  setupPeek('depAccountNum', 'depAccountPeek');
  setupPeek('withAccountNum', 'withAccountPeek');
  setupPeek('trSenderNum', 'trSenderPeek');
  setupPeek('trReceiverNum', 'trReceiverPeek');

  // FD calculations preview on input
  document.getElementById('caBalance')?.addEventListener('input', updateFdCalculations);
  document.getElementById('caFdTerm')?.addEventListener('change', updateFdCalculations);

  // 6. Form Handlers

  // Register Customer Form
  document.getElementById('registerCustomerForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const name = document.getElementById('custNameInput').value.trim();
    const email = document.getElementById('custEmailInput').value.trim();
    const phone = document.getElementById('custPhoneInput').value.trim();

    if (!name || !email || !phone) {
      showToast('All customer fields are mandatory.', 'error');
      return;
    }

    const newCust = new Customer(name, email, phone);
    bank.addCustomer(newCust);
    showToast(`Customer CUST-${newCust.getCustomerId()} (${name}) registered successfully!`, 'success');
    closeRegisterCustomerModal();
    refreshAllViews();
  });

  // Edit Customer Form
  document.getElementById('editCustomerForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const custId = document.getElementById('editCustId').value;
    const email = document.getElementById('editCustEmail').value.trim();
    const phone = document.getElementById('editCustPhone').value.trim();

    try {
      bank.updateCustomerContact(custId, email, phone);
      showToast(`Contact information updated for CUST-${custId}.`, 'success');
      closeEditCustomerModal();
      refreshAllViews();
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Create Account Form
  document.getElementById('createAccountForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const custId = Number(document.getElementById('caCustomerId').value);
    const balance = Number(document.getElementById('caBalance').value);
    const cust = bank.findCustomer(custId);

    let newAcc = null;
    try {
      if (currentCreateType === 'savings') {
        newAcc = new SavingsAccount(custId, cust.getFullName(), balance);
      } else if (currentCreateType === 'current') {
        const od = Number(document.getElementById('caOverdraft').value) || 25000;
        newAcc = new CurrentAccount(custId, cust.getFullName(), balance, formatDate(), od);
      } else if (currentCreateType === 'fd') {
        const term = Number(document.getElementById('caFdTerm').value) || 12;
        let rate = 0.07;
        if (term === 6) rate = 0.065;
        else if (term === 12) rate = 0.070;
        else if (term === 24) rate = 0.0725;
        else if (term === 36) rate = 0.075;
        newAcc = new FixedDepositAccount(custId, cust.getFullName(), balance, term, rate);
      }

      bank.addAccount(newAcc);
      showToast(`Account #${newAcc.getAccountNumber()} successfully created for ${cust.getFullName()}!`, 'success');
      closeCreateAccountModal();
      refreshAllViews();
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Deposit Form
  document.getElementById('depositForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const accNum = document.getElementById('depAccountNum').value.trim();
    const amount = Number(document.getElementById('depAmount').value);
    const note = document.getElementById('depNote').value.trim();

    try {
      const result = bank.deposit(accNum, amount, note);
      renderReceipt(result.tx, { newBal: result.newBal });
      showToast(`Deposit of ${formatRupees(amount)} credited to Account #${accNum}.`, 'success');
      e.target.reset();
      document.getElementById('depAccountPeek').textContent = '';
      refreshAllViews();
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Withdrawal Form
  document.getElementById('withdrawForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const accNum = document.getElementById('withAccountNum').value.trim();
    const amount = Number(document.getElementById('withAmount').value);
    const note = document.getElementById('withNote').value.trim();

    try {
      const result = bank.withdraw(accNum, amount, note);
      renderReceipt(result.tx, { newBal: result.newBal });
      showToast(`Withdrawal of ${formatRupees(amount)} debited from Account #${accNum}.`, 'success');
      e.target.reset();
      document.getElementById('withAccountPeek').textContent = '';
      refreshAllViews();
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Transfer Form
  document.getElementById('transferForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const senderNum = document.getElementById('trSenderNum').value.trim();
    const receiverNum = document.getElementById('trReceiverNum').value.trim();
    const amount = Number(document.getElementById('trAmount').value);
    const note = document.getElementById('trNote').value.trim();

    try {
      const result = bank.transfer(senderNum, receiverNum, amount, note);
      renderReceipt(result.tx, { senderNew: result.senderNew });
      showToast(`Transferred ${formatRupees(amount)} from #${senderNum} to #${receiverNum}.`, 'success');
      e.target.reset();
      document.getElementById('trSenderPeek').textContent = '';
      document.getElementById('trReceiverPeek').textContent = '';
      refreshAllViews();
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Statement Generation Button
  document.getElementById('btnGenerateStatement')?.addEventListener('click', () => {
    const input = document.getElementById('stmtAccountInput');
    const val = input ? input.value.trim() : '';
    if (!val) {
      showToast('Please enter an account number.', 'error');
      return;
    }
    try {
      generateStatement(val);
      showToast(`Statement generated for Account #${val}.`, 'success');
    } catch (err) {
      showToast(err.message, 'error');
    }
  });

  // Settings Rules Form
  document.getElementById('settingsRulesForm')?.addEventListener('submit', (e) => {
    e.preventDefault();
    const large = Number(document.getElementById('setLargeThreshold').value);
    const daily = Number(document.getElementById('setDailyLimit').value);
    const count = Number(document.getElementById('setVelocityCount').value);

    bank.fraudDetector.config.largeTxThreshold = large;
    bank.fraudDetector.config.dailyDebitLimit = daily;
    bank.fraudDetector.config.rapidVelocityCount = count;
    bank.saveToStorage();

    const cfgL = document.getElementById('cfgRuleLargeTx');
    const cfgD = document.getElementById('cfgRuleDaily');
    const cfgV = document.getElementById('cfgRuleVelocity');
    if (cfgL) cfgL.textContent = formatRupees(large);
    if (cfgD) cfgD.textContent = formatRupees(daily);
    if (cfgV) cfgV.textContent = `${count} transactions in 2 mins`;

    showToast('Security rules configuration successfully saved!', 'success');
  });

  // Export JSON
  document.getElementById('btnExportJSON')?.addEventListener('click', () => {
    const raw = localStorage.getItem('smartbank_db_v4') || '{}';
    const blob = new Blob([raw], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `smartbank_backup_${formatDate()}.json`;
    a.click();
    URL.revokeObjectURL(url);
    showToast('Database exported to JSON file.', 'success');
  });

  // Reset Demo Data
  document.getElementById('btnResetDemoData')?.addEventListener('click', () => {
    if (confirm('Reset all customers, accounts and transactions to default demo state?')) {
      bank.seedDefaultData();
      showToast('Default demo accounts and customers reloaded.', 'success');
      refreshAllViews();
    }
  });

  // Wipe All Data
  document.getElementById('btnClearAllData')?.addEventListener('click', () => {
    if (confirm('Are you sure you want to completely erase the entire banking registry?')) {
      localStorage.removeItem('smartbank_db_v4');
      bank.customers = [];
      bank.accounts = [];
      bank.transactions = [];
      bank.fraudDetector.alerts = [];
      showToast('Database wiped. Application reset.', 'warning');
      refreshAllViews();
    }
  });

  // Search & Filter event bindings
  document.getElementById('customerSearchInput')?.addEventListener('input', (e) => {
    renderCustomersTable(e.target.value);
  });

  document.getElementById('accountSearchInput')?.addEventListener('input', renderAccountsTable);
  document.getElementById('accountTypeFilter')?.addEventListener('change', renderAccountsTable);

  document.getElementById('txHistorySearch')?.addEventListener('input', renderTransactionsTable);
  document.getElementById('txTypeFilter')?.addEventListener('change', renderTransactionsTable);
  document.getElementById('txRiskFilter')?.addEventListener('change', renderTransactionsTable);

  // Global search in topbar
  document.getElementById('globalSearchInput')?.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
      const q = e.target.value.trim();
      if (!q) return;
      try {
        const acc = bank.findAccount(q);
        navigateTo('accounts');
        openAccountDetailModal(acc.getAccountNumber());
      } catch (err) {
        try {
          const cust = bank.findCustomer(q);
          navigateTo('customers');
          openCustomerAccountsModal(cust.getCustomerId());
        } catch (err2) {
          showToast(`No account or customer found matching "${q}".`, 'error');
        }
      }
    }
  });

  // Modal open / close bindings
  document.getElementById('btnOpenRegisterCustomerModal')?.addEventListener('click', openRegisterCustomerModal);
  document.getElementById('btnCloseCustomerModal')?.addEventListener('click', closeRegisterCustomerModal);
  document.getElementById('btnCancelCustomerModal')?.addEventListener('click', closeRegisterCustomerModal);

  document.getElementById('btnCloseEditCustModal')?.addEventListener('click', closeEditCustomerModal);
  document.getElementById('btnCancelEditCustModal')?.addEventListener('click', closeEditCustomerModal);

  document.getElementById('btnCloseCustAccModal')?.addEventListener('click', closeCustomerAccountsModal);

  document.getElementById('btnOpenCreateAccountModal')?.addEventListener('click', () => openCreateAccountModal());
  document.getElementById('btnCloseCreateModal')?.addEventListener('click', closeCreateAccountModal);
  document.getElementById('btnCancelCreateModal')?.addEventListener('click', closeCreateAccountModal);

  document.getElementById('btnCloseDetailModal')?.addEventListener('click', closeAccountDetailModal);
  document.getElementById('btnCloseSecConfirmModal')?.addEventListener('click', () => {
    document.getElementById('securityConfirmModal')?.classList.remove('active');
  });

  // Mobile menu toggle
  document.getElementById('menuToggle')?.addEventListener('click', () => {
    const sidebar = document.getElementById('sidebar');
    if (sidebar) sidebar.classList.toggle('open');
  });

  // Notification bell click
  document.getElementById('notifBellBtn')?.addEventListener('click', () => {
    navigateTo('security');
  });

  // Window resize to redraw charts
  window.addEventListener('resize', () => {
    drawCashFlowChart();
    drawAccountTypeChart();
  });

  // Initial Boot
  refreshAllViews();
});
