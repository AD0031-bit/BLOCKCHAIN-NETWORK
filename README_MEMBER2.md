# Member 2 — Block & Hashing

`Block` represents one candidate or accepted blockchain block. It owns an index,
UTC timestamp, transactions, previous block hash, nonce, miner address, and its
own calculated SHA-256 hash.

## Hashing

`calculateHash()` serializes every meaningful field using length-prefixed values,
then hashes those bytes with OpenSSL SHA-256. Each transaction contributes sender,
receiver, amount, and its timestamp. The serialization deliberately excludes the
block's `hash` field, so a block never hashes its own output. The result is a
64-character lowercase hexadecimal digest.

## Candidate blocks

Create one with the constructor. Supply the candidate index, pending transaction
vector, accepted previous hash, fixed nonce, and candidate miner address. The
constructor generates the current UTC timestamp and calculates the initial hash.

## Public interface for Members 3 and 4

```cpp
Block candidate(index, pendingTransactions, previousHash, nonce, minerAddress);
std::string computed = candidate.calculateHash();
bool unchanged = (computed == candidate.getHash());
candidate.setNonce(newNonce);
candidate.recalculateHash();
```

Available getters are `getIndex()`, `getTimestamp()`, `getTransactions()`,
`getPreviousHash()`, `getNonce()`, `getMinerAddress()`, and `getHash()`.
For deliberate edits, use `setTimestamp`, `setTransactions`, `setPreviousHash`,
`setNonce`, or `setMinerAddress`, followed by `recalculateHash()`.

## Transaction assumption

This implementation uses the existing `Transaction` getters: `getSender()`,
`getReceiver()`, `getAmount()`, and `getTimestamp()`. No Member 1 file was
modified.

## Build and test

OpenSSL development headers and libraries are required. From the repository root:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic tests/test_block.cpp Block.cpp Transaction.cpp Wallet.cpp -o test_block -lcrypto
./test_block
```

On Windows, install an OpenSSL development package compatible with MinGW and add
its `include` and `lib` paths, for example:

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic tests/test_block.cpp Block.cpp Transaction.cpp Wallet.cpp -IC:\OpenSSL-Win64\include -LC:\OpenSSL-Win64\lib -o test_block.exe -lcrypto
.\test_block.exe
```
