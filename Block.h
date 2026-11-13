#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>

#include "Transaction.h"

class Block {
private:
    int index;
    std::string timestamp;
    std::vector<Transaction> transactions;
    std::string previousHash;
    int nonce;
    std::string minerAddress;
    std::string hash;

public:
    Block(int index,
          const std::vector<Transaction>& transactions,
          const std::string& previousHash,
          int nonce,
          const std::string& minerAddress);

    std::string calculateHash() const;
    void displayBlock() const;

    int getIndex() const;
    const std::string& getTimestamp() const;
    const std::vector<Transaction>& getTransactions() const;
    const std::string& getPreviousHash() const;
    int getNonce() const;
    const std::string& getMinerAddress() const;
    const std::string& getHash() const;

    // Call recalculateHash() after changing data when the new hash is needed.
    void setTimestamp(const std::string& timestamp);
    void setTransactions(const std::vector<Transaction>& transactions);
    void setPreviousHash(const std::string& previousHash);
    void setNonce(int nonce);
    void setMinerAddress(const std::string& minerAddress);
    void recalculateHash();
};

#endif
