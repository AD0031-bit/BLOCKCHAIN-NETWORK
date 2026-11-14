#include "Node.h"
#include "Consensus.h"
#include <algorithm>

Node::Node(const std::string& id)
    : id_(id), wallet_(0), address_(wallet_.getAddress()) {}

std::size_t Node::chainLength() const { return chain_.getChain().size(); }

long long Node::balanceOf(const std::string& address) const {
    long long bal = 0;
    for (const Block& b : chain_.getChain())
        for (const Transaction& tx : b.getTransactions()) {
            if (tx.getReceiver() == address) bal += tx.getAmount();
            if (tx.getSender() == address) bal -= tx.getAmount();
        }
    return bal;
}

long long Node::getAvailableBalance() const {
    long long pending = 0;
    for (const Transaction& tx : pool_)
        if (tx.getSender() == address_) pending += tx.getAmount();
    return getBalance() - pending;
}

// ---------------- transactions ----------------

std::optional<Transaction> Node::createTransaction(const std::string& receiver, long long amount,
                                                   std::string& error) const {
    if (receiver.empty() || receiver == address_) { error = "invalid receiver"; return std::nullopt; }
    if (amount <= 0) { error = "amount must be positive"; return std::nullopt; }
    if (getAvailableBalance() < amount) { error = "insufficient balance"; return std::nullopt; }
    return Transaction(address_, receiver, amount);
}

bool Node::receiveTransaction(const Transaction& tx, bool trusted) {
    if (tx.getAmount() <= 0) return false;
    if (tx.getSender() == Config::SYSTEM_ADDRESS) {
        if (!trusted) return false;              // only the faucet may mint coins
    } else {
        long long pending = 0;
        for (const Transaction& p : pool_)
            if (p.getSender() == tx.getSender()) pending += p.getAmount();
        if (balanceOf(tx.getSender()) - pending < tx.getAmount()) return false;  // double-spend guard
    }
    for (const Transaction& p : pool_)
        if (txKey(p) == txKey(tx)) return false;                         // duplicate
    pool_.push_back(tx);
    return true;
}

void Node::removeFromPool(const std::vector<Transaction>& included) {
    pool_.erase(std::remove_if(pool_.begin(), pool_.end(),
        [&](const Transaction& p) {
            for (const Transaction& t : included)
                if (txKey(t) == txKey(p)) return true;
            return false;
        }), pool_.end());
}

// ---------------- blocks ----------------

long long& Node::balanceIn(BalanceMap& m, const std::string& address) const {
    auto it = m.find(address);
    if (it == m.end()) it = m.emplace(address, balanceOf(address)).first;
    return it->second;
}

bool Node::applyToBalances(const Transaction& tx, BalanceMap& m, bool allowSystem) const {
    if (tx.getAmount() <= 0) return false;
    if (tx.getSender() == Config::SYSTEM_ADDRESS) {
        if (!allowSystem) return false;
    } else {
        long long& s = balanceIn(m, tx.getSender());
        if (s < tx.getAmount()) return false;
        s -= tx.getAmount();
    }
    balanceIn(m, tx.getReceiver()) += tx.getAmount();
    return true;
}

Block Node::createCandidateBlock() const {
    const Block& last = chain_.getLatestBlock();
    int index = last.getIndex() + 1;
    bool bootstrap = (index == 1);               // block 1 may contain faucet (SYSTEM) txs

    std::vector<Transaction> txs;
    txs.push_back(Transaction(Config::SYSTEM_ADDRESS, address_, Config::BLOCK_REWARD));

    BalanceMap bal;
    balanceIn(bal, address_) += Config::BLOCK_REWARD;
    std::size_t taken = 0;
    for (const Transaction& tx : pool_) {
        if (taken >= Config::MAX_TX_PER_BLOCK) break;
        BalanceMap trial = bal;
        if (!applyToBalances(tx, trial, bootstrap)) continue;   // skip anything that would overspend
        bal = trial;
        txs.push_back(tx);
        ++taken;
    }
    return Block(index, txs, last.getHash(), Config::FIXED_NONCE, address_);
}

bool Node::validateBlock(const Block& block, std::string& reason) const {
    const Block& last = chain_.getLatestBlock();
    if (block.getIndex() != last.getIndex() + 1) { reason = "wrong index"; return false; }
    if (block.getPreviousHash() != last.getHash()) { reason = "previous hash mismatch"; return false; }
    if (block.calculateHash() != block.getHash()) { reason = "hash does not match contents"; return false; }
    if (block.getNonce() != Config::FIXED_NONCE) { reason = "nonce is not the fixed nonce"; return false; }

    const std::vector<Transaction>& txs = block.getTransactions();
    if (txs.empty()) { reason = "block has no transactions"; return false; }
    if (txs.size() > Config::MAX_TX_PER_BLOCK + 1) { reason = "too many transactions"; return false; }

    const Transaction& reward = txs[0];
    if (reward.getSender() != Config::SYSTEM_ADDRESS ||
        reward.getReceiver() != block.getMinerAddress() ||
        reward.getAmount() != Config::BLOCK_REWARD) {
        reason = "invalid reward transaction";
        return false;
    }

    bool bootstrap = (block.getIndex() == 1);
    BalanceMap bal;
    balanceIn(bal, reward.getReceiver()) += reward.getAmount();
    for (std::size_t i = 1; i < txs.size(); ++i)
        if (!applyToBalances(txs[i], bal, bootstrap)) { reason = "invalid transaction in block"; return false; }
    return true;
}

bool Node::acceptBlock(const Block& block) {
    std::string reason;
    if (!validateBlock(block, reason)) return false;
    if (!chain_.addBlock(block)) return false;
    removeFromPool(block.getTransactions());
    return true;
}

bool Node::replaceChain(const Blockchain& other) {
    if (other.getChain().size() <= chain_.getChain().size() || !other.isChainValid()) return false;
    chain_ = other;
    std::vector<Transaction> all;
    for (const Block& b : chain_.getChain())
        for (const Transaction& t : b.getTransactions()) all.push_back(t);
    removeFromPool(all);
    return true;
}

// ---------------- display ----------------

void Node::printPool(std::ostream& out) const {
    out << id_ << " pool (" << pool_.size() << "):\n";
    for (const Transaction& t : pool_) out << "   " << txText(t) << "\n";
}

void Node::printChain(std::ostream& out) const {
    out << "Blockchain of " << id_ << " (" << chainLength() << " blocks)\n";
    for (const Block& b : chain_.getChain()) {
        out << "  Block #" << b.getIndex()
            << "  hash=" << b.getHash().substr(0, 16) << "..."
            << "  prev=" << b.getPreviousHash().substr(0, 16) << "..."
            << "  miner=" << b.getMinerAddress() << "\n";
        for (const Transaction& t : b.getTransactions()) out << "      " << txText(t) << "\n";
    }
}
