# BlockChain Network 
## Member 1 — Wallet & Transaction Module

### Files
- `Wallet.h`
- `Wallet.cpp`
- `Transaction.h`
- `Transaction.cpp`
- `tests/test_member1.cpp`

### Wallet
Represents a user's wallet in the blockchain network.

Features:
- Automatically generates unique wallet addresses.
- Maintains wallet balance.
- Checks sufficient funds.
- Supports balance deduction and receiving coins.
- Prevents negative initial balances and integer overflow.

### Transaction
Represents a transfer between two wallets.

Features:
- Stores sender and receiver addresses.
- Stores transaction amount and timestamp.
- Validates transaction authenticity.
- Rejects invalid amounts and self-transactions.

### TransactionPool
Maintains transactions waiting to be included in a block.

Features:
- Adds valid transactions.
- Prevents pending transactions from exceeding available balance.
- Displays pending transactions.
- Removes confirmed transactions.

### Testing
All 7 Member 1 test cases pass, including:
- Valid transaction
- Overspending prevention
- Zero amount rejection
- Negative amount rejection
- Self-transaction rejection
- Incorrect sender wallet rejection
- Wallet overflow protection

**Status:** Initial implementation complete; ready for integration testing.
