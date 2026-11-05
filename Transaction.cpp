
#include "Transaction.h"
#include <ctime>

Transaction::Transaction(string sender, string receiver, long long amount) {
    this->sender = sender;
    this->receiver = receiver;
    this->amount = amount;
    timestamp = time(nullptr);
}

bool Transaction::isValid(const Wallet& senderWallet,const Wallet& receiverWallet) const {

    if (amount <= 0) {
        return false;
    }

    if (sender == receiver) {
        return false;
    }

    if (sender != senderWallet.getAddress()) {
        return false;
    }

    if (receiver != receiverWallet.getAddress()) {
        return false;
    }

    if (!senderWallet.hasSufficientBalance(amount)) {
        return false;
    }

    return true;
}

string Transaction::getSender() const {
    return sender;
}

string Transaction::getReceiver() const {
    return receiver;
}

long long Transaction::getAmount() const {
    return amount;
}

long long Transaction::getTimestamp() const {
    return timestamp;
}


bool TransactionPool::addTransaction(
    const Transaction& transaction,
    const Wallet& senderWallet,
    const Wallet& receiverWallet) {

    if (!transaction.isValid(senderWallet, receiverWallet)) {
        return false;
    }

    long long pendingAmount = 0;

    for (const Transaction& t : pendingTransactions) {
        if (t.getSender() == transaction.getSender()) {
            pendingAmount += t.getAmount();
        }
    }

    if (pendingAmount > senderWallet.getBalance()) {
        return false;
    }

    if (transaction.getAmount() >
        senderWallet.getBalance() - pendingAmount) {
        return false;
    }

    pendingTransactions.push_back(transaction);
    return true;
}

const vector<Transaction>&
TransactionPool::getPendingTransactions() const {
    return pendingTransactions;
}

void TransactionPool::removeConfirmedTransactions(
    const vector<Transaction>& confirmedTransactions) {

    for (const Transaction& confirmed : confirmedTransactions) {
        for (auto it = pendingTransactions.begin();
             it != pendingTransactions.end(); ++it) {

            if (it->getSender() == confirmed.getSender() &&
                it->getReceiver() == confirmed.getReceiver() &&
                it->getAmount() == confirmed.getAmount() &&
                it->getTimestamp() == confirmed.getTimestamp()) {

                pendingTransactions.erase(it);
                break;
            }
        }
    }
}

void TransactionPool::displayPendingTransactions() const {
    if (pendingTransactions.empty()) {
        cout << "No pending transactions.\n";
        return;
    }

    for (const Transaction& t : pendingTransactions) {
        cout << "Sender: " << t.getSender() << endl;
        cout << "Receiver: " << t.getReceiver() << endl;
        cout << "Amount: " << t.getAmount() << " coins" << endl;
        cout << "----------------------\n";
    }
}
