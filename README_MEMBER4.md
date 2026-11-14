\# Member 4 — Node, Network \& Consensus



Integrates the full system: multiple nodes, transaction/block propagation,

Minimum-Hash Consensus, block validation, block rewards, and the menu.



\## Files

\- `Node.h`, `Node.cpp`

\- `Network.h`, `Network.cpp`

\- `Consensus.h`, `Consensus.cpp`

\- `main.cpp`



\## Node

Each node has its own wallet, its own copy of the blockchain, and its own

pending-transaction pool (FIFO).

\- Creates transactions (rejects bad receivers, non-positive amounts, overspending).

\- Accepts incoming transactions with duplicate and double-spend checks.

\- Builds a candidate block: a reward transaction first, then up to 5 pending

&#x20; transactions, using the fixed nonce.

\- Independently validates any block: index, previous hash, recomputed hash,

&#x20; fixed nonce, reward transaction, and balances inside the block.

\- Balances are computed from the node's own chain.



\## Network

The peer-to-peer network is a graph (adjacency list) of nodes.

\- Transactions and blocks spread by breadth-first search; a node only

&#x20; forwards what it accepted.

\- `runConsensusRound()` runs one full round: candidates, minimum hash,

&#x20; validation, distribution, reward.

\- `synchronize()` brings lagging nodes up to the longest valid chain.

\- A joining node copies an existing peer's chain so all nodes share one genesis block.



\## Consensus (Minimum-Hash)

\- Every node with pending transactions builds a candidate block using the same

&#x20; fixed nonce (42).

\- Candidates are ranked by hash, smallest first; ties are broken by node id.

\- The smallest-hash candidate wins. If the winner fails its own validation,

&#x20; the next smallest gets a chance.

\- The winner receives a 50-coin reward through a `SYSTEM` transaction placed

&#x20; first in its block.



\## Rules (in `Consensus.h`)

| Setting | Value |

|---|---|

| Block reward | 50 |

| Fixed nonce | 42 |

| Max user transactions per block | 5 |



\## Bootstrap

Nodes start with no coins. A faucet issues `SYSTEM` transactions, which are

only valid in block #1. The demo network (4 nodes in a ring, 100 coins each)

is set up this way at startup.



\## Build and run

Requires OpenSSL (used by `Block.cpp` for SHA-256).



&#x20;   g++ -std=c++17 -Wall -Wextra \*.cpp -o blockchain -lssl -lcrypto

&#x20;   ./blockchain



On Windows, use MSYS2 UCRT64 and install

`mingw-w64-ucrt-x86\_64-gcc` and `mingw-w64-ucrt-x86\_64-openssl`.



\## Menu

1\. Add node

2\. Connect two nodes

3\. Create transaction

4\. Run consensus round

5\. Show balances

6\. Show pending pools

7\. Show a node's blockchain

8\. Validate all chains

9\. Synchronize nodes

10\. Show network topology

11\. Faucet

0\. Exit



\## Limitations

\- Simulation inside one process (no real TCP sockets).

\- No digital signatures, so any node could spend from any address.

\- Winner selection is effectively random, since the nonce is fixed and

&#x20; candidates differ only in timestamp and miner address.

