
#include "Wallet.h"
#include <stdexcept>

int Wallet::walletCount = 0;

Wallet::Wallet(long long initialBalance) {
    if (initialBalance < 0) {
        throw invalid_argument("Initial balance cannot be negative");
    }

    walletCount++;
    address = "WALLET" + to_string(walletCount);
    balance = initialBalance;
}

string Wallet::getAddress() const {
    return address;
}

long long Wallet::getBalance() const {
    return balance;
}

bool Wallet::hasSufficientBalance(long long amount) const {
    return amount > 0 && balance >= amount;
}

bool Wallet::deduct(long long amount) {
    if (!hasSufficientBalance(amount)) {
        return false;
    }

    balance -= amount;
    return true;
}

void Wallet::receive(long long amount) {
    if (amount > 0) {
        balance += amount;
    }
}
