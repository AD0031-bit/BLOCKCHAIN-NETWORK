
#include "Wallet.h"
#include "Transaction.h"

int main() {

    Wallet alice(1000);
    Wallet bob(500);
    Wallet charlie(300);

    TransactionPool pool;

    cout << "===== BLOCKCHAIN TRANSACTION TEST =====\n";

    Transaction t1(alice.getAddress(), bob.getAddress(), 700);

    cout << "\nTest 1: Valid transaction\n";
    cout << (pool.addTransaction(t1, alice, bob) ? "PASS" : "FAIL") << endl;

    Transaction t2(alice.getAddress(), bob.getAddress(), 400);

    cout << "\nTest 2: Overspending\n";
    cout << (!pool.addTransaction(t2, alice, bob) ? "PASS" : "FAIL") << endl;

    Transaction t3(alice.getAddress(), bob.getAddress(), 0);

    cout << "\nTest 3: Zero amount\n";
    cout << (!pool.addTransaction(t3, alice, bob) ? "PASS" : "FAIL") << endl;

    Transaction t4(alice.getAddress(), bob.getAddress(), -50);

    cout << "\nTest 4: Negative amount\n";
    cout << (!pool.addTransaction(t4, alice, bob) ? "PASS" : "FAIL") << endl;

    Transaction t5(alice.getAddress(), alice.getAddress(), 100);

    cout << "\nTest 5: Self transaction\n";
    cout << (!pool.addTransaction(t5, alice, alice) ? "PASS" : "FAIL") << endl;

    Transaction t6(alice.getAddress(), bob.getAddress(), 100);

    cout << "\nTest 6: Incorrect sender wallet\n";
    cout << (!pool.addTransaction(t6, charlie, bob) ? "PASS" : "FAIL") << endl;

    cout << "\nPending transactions: "
         << pool.getPendingTransactions().size() << endl;

    cout << "Alice balance: " << alice.getBalance() << endl;
    cout << "Bob balance: " << bob.getBalance() << endl;

    vector<Transaction> confirmed;
    confirmed.push_back(t1);

    pool.removeConfirmedTransactions(confirmed);

    cout << "\nAfter removing confirmed transaction:";
    cout << "\nPending transactions: "
         << pool.getPendingTransactions().size() << endl;

    return 0;
}
