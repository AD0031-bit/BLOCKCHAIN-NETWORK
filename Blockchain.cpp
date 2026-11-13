
#include "Blockchain.h"

#include <iostream>
using namespace std;

// Constructor: Create Genesis Block
Blockchain::Blockchain() {

    vector<Transaction> genesisTransactions;

    Block genesisBlock(
        0,
        genesisTransactions,
        "0",
        0,
        "GENESIS"
    );

    chain.push_back(genesisBlock);
}

// Get latest block
const Block& Blockchain::getLatestBlock() const {
    return chain.back();
}

// Get complete blockchain
const vector<Block>& Blockchain::getChain() const {
    return chain;
}

// Add new block
bool Blockchain::addBlock(const Block& newBlock) {

    // Validate existing chain
    if (!isChainValid()) {
        return false;
    }

    // Check block index
    if (newBlock.getIndex() != static_cast<int>(chain.size())) {
        return false;
    }

    // Check previous hash
    if (newBlock.getPreviousHash() != getLatestBlock().getHash()) {
        return false;
    }

    // Check current block hash
    if (newBlock.getHash() != newBlock.calculateHash()) {
        return false;
    }

    // Add block
    chain.push_back(newBlock);

    return true;
}

// Validate complete blockchain
bool Blockchain::isChainValid() const {

    if (chain.empty()) {
        return false;
    }

    for (size_t i = 0; i < chain.size(); i++) {

        const Block& currentBlock = chain[i];

        // Check block index
        if (currentBlock.getIndex() != static_cast<int>(i)) {
            return false;
        }

        // Check block hash
        if (currentBlock.getHash() != currentBlock.calculateHash()) {
            return false;
        }

        // Check previous hash link
        if (i > 0) {

            const Block& previousBlock = chain[i - 1];

            if (currentBlock.getPreviousHash() != previousBlock.getHash()) {
                return false;
            }
        }

        // Check Genesis Block
        else {

            if (currentBlock.getPreviousHash() != "0") {
                return false;
            }

            if (currentBlock.getMinerAddress() != "GENESIS") {
                return false;
            }
        }
    }

    return true;
}

// Display complete blockchain
void Blockchain::displayChain() const {

    cout << "\n========== BLOCKCHAIN ==========\n";

    for (const Block& block : chain) {
        block.displayBlock();
    }

    cout << "Total Blocks: " << chain.size() << endl;

    cout << "Blockchain Valid: "
         << (isChainValid() ? "YES" : "NO")
         << endl;

    cout << "================================\n";
}
