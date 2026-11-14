
#include "../Blockchain.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {

    cout << "===== BLOCKCHAIN TEST =====\n";

    Blockchain blockchain;

    // Test 1: Genesis Block
    cout << "\nTest 1: Genesis Block\n";
    cout << (blockchain.getChain().size() == 1 ? "PASS" : "FAIL") << endl;

    // Test 2: Initial Chain Validation
    cout << "\nTest 2: Initial Chain Validation\n";
    cout << (blockchain.isChainValid() ? "PASS" : "FAIL") << endl;

    // Test 3: Add Valid Block
    Block newBlock(
        1,
        vector<Transaction>(),
        blockchain.getLatestBlock().getHash(),
        0,
        "NODE_1"
    );

    cout << "\nTest 3: Add Valid Block\n";
    cout << (blockchain.addBlock(newBlock) ? "PASS" : "FAIL") << endl;

    // Test 4: Validate Updated Chain
    cout << "\nTest 4: Validate Updated Chain\n";
    cout << (blockchain.isChainValid() ? "PASS" : "FAIL") << endl;

    // Test 5: Reject Invalid Previous Hash
    Block invalidBlock(
        2,
        vector<Transaction>(),
        "WRONG_HASH",
        0,
        "NODE_2"
    );

    cout << "\nTest 5: Reject Invalid Previous Hash\n";
    cout << (!blockchain.addBlock(invalidBlock) ? "PASS" : "FAIL") << endl;

    // Test 6: Reject Incorrect Index
    Block wrongIndexBlock(
        5,
        vector<Transaction>(),
        blockchain.getLatestBlock().getHash(),
        0,
        "NODE_2"
    );

    cout << "\nTest 6: Reject Incorrect Index\n";
    cout << (!blockchain.addBlock(wrongIndexBlock) ? "PASS" : "FAIL") << endl;

    // Display Final Blockchain
    cout << "\nFinal Blockchain:\n";
    blockchain.displayChain();

    return 0;
}
