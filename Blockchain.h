
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include "Block.h"

class Blockchain {
private:
    std::vector<Block> chain;

public:
    // Constructor
    Blockchain();

    // Getters
    const Block& getLatestBlock() const;
    const std::vector<Block>& getChain() const;

    // Add block
    bool addBlock(const Block& newBlock);

    // Validate blockchain
    bool isChainValid() const;

    // Display blockchain
    void displayChain() const;
};

#endif
