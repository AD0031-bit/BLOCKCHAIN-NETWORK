#ifndef WALLET_H
#define WALLET_H

#include <iostream>
#include <string>
using namespace std;

class Wallet {
private:
    string address;
    long long balance;

    static int walletCount;

public:
    Wallet(long long initialBalance);

    string getAddress() const;
    long long getBalance() const;

    bool hasSufficientBalance(long long amount) const;

    bool deduct(long long amount);
    void receive(long long amount);
};

#endif
