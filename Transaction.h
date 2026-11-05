
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <string>
#include "Wallet.h"
#include <vector>

using namespace std;

class Transaction {
private:
    string sender;
    string receiver;
    long long amount;
    long long timestamp;

public:
    Transaction(string sender, string receiver, long long amount);

    bool isValid(const Wallet& senderWallet,
                 const Wallet& receiverWallet) const;

    string getSender() const;
    string getReceiver() const;
    long long getAmount() const;
    long long getTimestamp() const;
};


class TransactionPool {
private:
    vector<Transaction> pendingTransactions;

public:
    bool addTransaction(const Transaction& transaction,
                       const Wallet& senderWallet,
                       const Wallet& receiverWallet);

    const vector<Transaction>& getPendingTransactions() const;

    void removeConfirmedTransactions(
        const vector<Transaction>& confirmedTransactions);

    void displayPendingTransactions() const;
};

#endif
