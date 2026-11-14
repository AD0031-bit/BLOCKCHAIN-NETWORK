#include "Network.h"
#include "Consensus.h"
#include <iomanip>
#include <queue>
#include <set>

Node& Network::addNode(const std::string& id) {
    nodes_.push_back(std::unique_ptr<Node>(new Node(id)));
    // A joining node downloads an existing peer's chain so everyone shares the same genesis block
    // (Blockchain() may stamp genesis with the current time, which can differ between nodes).
    if (nodes_.size() > 1) nodes_.back()->adoptChain(nodes_.front()->getChain());
    adj_[id];   // make sure the node has an adjacency entry
    return *nodes_.back();
}

Node* Network::findNode(const std::string& id) {
    for (auto& n : nodes_) if (n->getId() == id) return n.get();
    return nullptr;
}
const Node* Network::findNode(const std::string& id) const {
    for (const auto& n : nodes_) if (n->getId() == id) return n.get();
    return nullptr;
}

bool Network::connect(const std::string& a, const std::string& b) {
    if (a == b || !findNode(a) || !findNode(b)) return false;
    auto& la = adj_[a];
    for (const std::string& x : la) if (x == b) return false;   // already linked
    la.push_back(b);
    adj_[b].push_back(a);
    return true;
}

// ---------------- propagation ----------------

bool Network::broadcastTransaction(const std::string& origin, const Transaction& tx) {
    Node* start = findNode(origin);
    if (!start || !start->receiveTransaction(tx)) return false;
    std::set<std::string> seen{origin};
    std::queue<std::string> q;
    q.push(origin);
    while (!q.empty()) {
        std::string cur = q.front(); q.pop();
        for (const std::string& nb : adj_[cur])
            if (seen.insert(nb).second) {
                Node* n = findNode(nb);
                if (n && n->receiveTransaction(tx)) q.push(nb);   // forward only if accepted
            }
    }
    return true;
}

void Network::faucet(const std::string& address, long long amount) {
    Transaction tx(Config::SYSTEM_ADDRESS, address, amount);
    for (auto& n : nodes_) n->receiveTransaction(tx, true);
}

std::size_t Network::propagateBlock(const std::string& origin, const Block& block) {
    std::size_t accepted = 1;                                    // origin already accepted it
    std::set<std::string> seen{origin};
    std::queue<std::string> q;
    q.push(origin);
    while (!q.empty()) {
        std::string cur = q.front(); q.pop();
        for (const std::string& nb : adj_[cur])
            if (seen.insert(nb).second) {
                Node* n = findNode(nb);
                if (n && n->acceptBlock(block)) { ++accepted; q.push(nb); }  // independent validation
            }
    }
    return accepted;
}

// ---------------- consensus round ----------------

bool Network::runConsensusRound(std::ostream& out) {
    std::vector<Candidate> candidates;
    for (auto& n : nodes_)
        if (n->hasPending()) candidates.push_back(Candidate(n->getId(), n->createCandidateBlock()));

    if (candidates.empty()) { out << "No node has pending transactions.\n"; return false; }

    std::vector<Candidate> ranked = Consensus::rank(candidates);
    out << "\n--- Candidate blocks (smallest hash wins) ---\n";
    for (const Candidate& c : ranked)
        out << "  " << std::left << std::setw(8) << c.nodeId
            << c.block.getHash().substr(0, 24) << "...\n";

    for (const Candidate& c : ranked) {
        Node* winner = findNode(c.nodeId);
        std::string reason;
        if (!winner->validateBlock(c.block, reason)) {
            out << "  " << c.nodeId << " disqualified: " << reason << "\n";
            continue;                                           // next-smallest hash gets a chance
        }
        winner->acceptBlock(c.block);
        std::size_t ok = propagateBlock(c.nodeId, c.block);
        out << "\nWinner: " << c.nodeId << "  (block #" << c.block.getIndex() << ")\n"
            << "Reward of " << Config::BLOCK_REWARD << " credited to " << winner->getAddress() << "\n"
            << "Block accepted by " << ok << "/" << nodes_.size() << " nodes\n";
        if (ok < nodes_.size()) out << "Some nodes were not reached; use Synchronize.\n";
        return true;
    }
    out << "No valid candidate block this round.\n";
    return false;
}

void Network::synchronize(std::ostream& out) {
    const Node* best = nullptr;
    for (const auto& n : nodes_)
        if (n->isChainValid() && (!best || n->chainLength() > best->chainLength())) best = n.get();
    if (!best) { out << "No valid chain found.\n"; return; }
    int updated = 0;
    for (auto& n : nodes_)
        if (n.get() != best && n->replaceChain(best->getChain())) {
            out << "  " << n->getId() << " synced to length " << best->chainLength() << "\n";
            ++updated;
        }
    out << (updated ? "Synchronization done.\n" : "All nodes already up to date.\n");
}

// ---------------- reports ----------------

bool Network::validateAllChains(std::ostream& out) const {
    bool all = true;
    for (const auto& n : nodes_) {
        bool ok = n->isChainValid();
        out << "  " << std::left << std::setw(8) << n->getId()
            << "length=" << n->chainLength() << "  " << (ok ? "VALID" : "INVALID") << "\n";
        all = all && ok;
    }
    return all;
}

void Network::printTopology(std::ostream& out) const {
    for (const auto& kv : adj_) {
        out << "  " << kv.first << " -> ";
        for (const std::string& p : kv.second) out << p << " ";
        out << "\n";
    }
}

void Network::printBalances(std::ostream& out) const {
    for (const auto& n : nodes_)
        out << "  " << std::left << std::setw(8) << n->getId() << std::setw(14) << n->getAddress()
            << "balance=" << n->getBalance() << "  pool=" << n->poolSize()
            << "  chain=" << n->chainLength() << "\n";
}
