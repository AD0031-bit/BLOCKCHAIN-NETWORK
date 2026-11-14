#ifndef CONSENSUS_H
#define CONSENSUS_H

#include <cstddef>
#include <string>
#include <vector>
#include "Block.h"

// Global rules shared by Node, Network and Consensus.
namespace Config {
const std::string SYSTEM_ADDRESS = "SYSTEM";   // sender of reward / faucet transactions
const long long BLOCK_REWARD = 50;              // paid to the winning node
const int FIXED_NONCE = 42;                    // Minimum-Hash Consensus uses ONE fixed nonce
const std::size_t MAX_TX_PER_BLOCK = 5;        // user txs per block (reward tx not counted)
}

// A candidate block proposed by one node in a consensus round.
struct Candidate {
    std::string nodeId;
    Block block;
    Candidate(const std::string& id, const Block& b) : nodeId(id), block(b) {}
};

// Minimum-Hash Consensus: the candidate with the smallest hash wins.
class Consensus {
public:
    // true if hash a is numerically smaller than hash b (equal-length hex strings).
    static bool hashLess(const std::string& a, const std::string& b);

    // Candidates sorted best-first: smallest hash first, ties broken by node id.
    static std::vector<Candidate> rank(std::vector<Candidate> candidates);

    // Smallest-hash candidate, or nullptr if the list is empty.
    static const Candidate* selectWinner(const std::vector<Candidate>& candidates);
};

#endif
