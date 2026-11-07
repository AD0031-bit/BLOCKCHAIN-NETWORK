#include "../Block.h"

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

namespace {

int passed = 0;
int failed = 0;

void expect(bool condition, const std::string& name) {
    if (condition) {
        ++passed;
        std::cout << "PASS: " << name << '\n';
    } else {
        ++failed;
        std::cout << "FAIL: " << name << '\n';
    }
}

bool isSha256(const std::string& value) {
    if (value.size() != 64) return false;
    for (unsigned char character : value) {
        if (!std::isxdigit(character)) return false;
    }
    return true;
}

Block makeBlock(const std::vector<Transaction>& transactions) {
    Block block(7, transactions, "0000previoushash", 99, "miner-alpha");
    block.setTimestamp("2026-11-07T00:00:00.000Z");
    block.recalculateHash();
    return block;
}

}  // namespace

int main() {
    const Transaction paymentOne("alice", "bob", 50);
    const Transaction paymentTwo("bob", "carol", 25);
    const std::vector<Transaction> transactions{paymentOne, paymentTwo};
    Block block = makeBlock(transactions);

    expect(block.getTransactions().size() == 2, "1. Create a block with multiple transactions");
    expect(isSha256(block.getHash()), "2. Hash has 64 hexadecimal characters");

    std::string originalHash = block.getHash();
    block.setNonce(100);
    block.recalculateHash();
    expect(block.getHash() != originalHash, "3. Nonce changes the hash");

    originalHash = block.getHash();
    block.setPreviousHash("different-previous-hash");
    block.recalculateHash();
    expect(block.getHash() != originalHash, "4. Previous hash changes the hash");

    originalHash = block.getHash();
    block.setMinerAddress("miner-beta");
    block.recalculateHash();
    expect(block.getHash() != originalHash, "5. Miner address changes the hash");

    originalHash = block.getHash();
    block.setTransactions({Transaction("alice", "bob", 51), paymentTwo});
    block.recalculateHash();
    expect(block.getHash() != originalHash, "6. Transaction amount changes the hash");

    originalHash = block.getHash();
    block.setTransactions({Transaction("dave", "erin", 51), paymentTwo});
    block.recalculateHash();
    expect(block.getHash() != originalHash, "7. Transaction sender and receiver change the hash");

    originalHash = block.getHash();
    block.setTimestamp("2026-11-08T00:00:00.000Z");
    block.recalculateHash();
    expect(block.getHash() != originalHash, "8. Timestamp changes the hash");

    Block first = makeBlock(transactions);
    Block second = makeBlock(transactions);
    expect(first.getHash() == second.getHash(), "9. Identical data produces the same hash");

    std::cout << "\n10. Complete block display:\n";
    first.displayBlock();
    expect(true, "10. Display complete block information");

    std::cout << "\nResult: " << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}
