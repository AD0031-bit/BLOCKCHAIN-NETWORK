#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include "Node.h"

// Peer-to-peer network simulated as a graph (adjacency list) of nodes.
class Network {
public:
    Node& addNode(const std::string& id);
    bool connect(const std::string& a, const std::string& b);   // undirected link
    Node* findNode(const std::string& id);
    const Node* findNode(const std::string& id) const;
    std::size_t size() const { return nodes_.size(); }
    const std::vector<std::unique_ptr<Node>>& nodes() const { return nodes_; }

    // Gossip a transaction from `origin` to all reachable peers (BFS).
    bool broadcastTransaction(const std::string& origin, const Transaction& tx);

    // Mint coins to an address (bootstrap helper): delivered to every node's pool.
    void faucet(const std::string& address, long long amount);

    // One full round: candidates -> minimum hash -> validate -> distribute -> reward.
    bool runConsensusRound(std::ostream& out);

    // Bring lagging nodes up to the longest valid chain.
    void synchronize(std::ostream& out);

    bool validateAllChains(std::ostream& out) const;
    void printTopology(std::ostream& out) const;
    void printBalances(std::ostream& out) const;

private:
    std::size_t propagateBlock(const std::string& origin, const Block& block);

    std::vector<std::unique_ptr<Node>> nodes_;
    std::map<std::string, std::vector<std::string>> adj_;
};

#endif
