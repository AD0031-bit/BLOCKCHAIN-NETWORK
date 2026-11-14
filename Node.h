#ifndef NODE_H
#define NODE_H

#include <deque>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include "Block.h"
#include "Blockchain.h"
#include "Transaction.h"
#include "Wallet.h"

// One participant: own wallet, own blockchain copy, own pending-transaction pool.
// Transaction has no toString(), so build identity / display text here.
inline std::string txKey(const Transaction& t) {
    return t.getSender() + "|" + t.getReceiver() + "|" + std::to_string(t.getAmount()) + "|" + std::to_string(t.getTimestamp());
}
inline std::string txText(const Transaction& t) {
    return t.getSender() + " -> " + t.getReceiver() + " : " + std::to_string(t.getAmount());
}

class Node {
public:
    explicit Node(const std::string& id);

    const std::string& getId() const { return id_; }
    const std::string& getAddress() const { return address_; }
    std::size_t chainLength() const;
    std::size_t poolSize() const { return pool_.size(); }
    bool hasPending() const { return !pool_.empty(); }

    // Balance confirmed on this node's chain / minus own pending outgoing txs.
    long long getBalance() const { return balanceOf(address_); }
    long long getAvailableBalance() const;
    long long balanceOf(const std::string& address) const;

    // Transactions
    std::optional<Transaction> createTransaction(const std::string& receiver, long long amount,
                                                 std::string& error) const;
    bool receiveTransaction(const Transaction& tx, bool trusted = false);

    // Blocks
    Block createCandidateBlock() const;
    bool validateBlock(const Block& block, std::string& reason) const;
    bool acceptBlock(const Block& block);

    // Chain sync / checks
    bool isChainValid() const { return chain_.isChainValid(); }
    bool replaceChain(const Blockchain& other);
    void adoptChain(const Blockchain& other) { if (other.isChainValid()) chain_ = other; }  // join: copy a peer's chain (same genesis)
    const Blockchain& getChain() const { return chain_; }

    // Display
    void printPool(std::ostream& out) const;
    void printChain(std::ostream& out) const;

private:
    typedef std::map<std::string, long long> BalanceMap;
    long long& balanceIn(BalanceMap& m, const std::string& address) const;
    bool applyToBalances(const Transaction& tx, BalanceMap& m, bool allowSystem) const;
    void removeFromPool(const std::vector<Transaction>& included);

    std::string id_;
    Wallet wallet_;          // generates this node's address
    std::string address_;
    Blockchain chain_;
    std::deque<Transaction> pool_;   // FIFO transaction pool
};

#endif
