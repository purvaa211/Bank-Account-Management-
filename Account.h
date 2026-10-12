#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

using namespace std;

class Account
{
private:
    int accountNumber;
    string name;
    double balance;

    static int nextAccountNumber;

public:
    Account(string n, double b);
    virtual ~Account();

    void deposit(double amount);
    void withdraw(double amount);

    virtual void calculateInterest() = 0;

    void display();

    double getBalance();
    int getAccountNumber();
};

#endif