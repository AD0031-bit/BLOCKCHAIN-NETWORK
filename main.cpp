#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include "Network.h"

static std::string readWord(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    std::cin >> s;
    return s;
}

static long long readNumber(const std::string& prompt) {
    long long v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter a number: ";
    }
    return v;
}

static void menu() {
    std::cout << "\n===== BLOCKCHAIN NETWORK =====\n"
                 " 1. Add node\n"
                 " 2. Connect two nodes\n"
                 " 3. Create transaction\n"
                 " 4. Run consensus round (create + select + distribute block)\n"
                 " 5. Show balances\n"
                 " 6. Show pending pools\n"
                 " 7. Show a node's blockchain\n"
                 " 8. Validate all chains\n"
                 " 9. Synchronize nodes\n"
                 "10. Show network topology\n"
                 "11. Faucet (give coins to a node)\n"
                 " 0. Exit\n> ";
}

int main() {
    Network net;

    // Default demo network: 4 nodes in a ring, each funded with 100 coins in block #1.
    const char* ids[] = {"Node1", "Node2", "Node3", "Node4"};
    for (const char* id : ids) net.addNode(id);
    for (int i = 0; i < 4; ++i) net.connect(ids[i], ids[(i + 1) % 4]);
    for (const char* id : ids) net.faucet(net.findNode(id)->getAddress(), 100);
    net.runConsensusRound(std::cout);

    int choice = -1;
    while (choice != 0) {
        menu();
        if (!(std::cin >> choice)) break;

        if (choice == 1) {
            std::string id = readWord("New node id: ");
            if (net.findNode(id)) std::cout << "Node already exists.\n";
            else {
                Node& n = net.addNode(id);
                std::cout << "Added " << id << " with address " << n.getAddress()
                          << ". Connect it, then Synchronize to download the chain.\n";
            }
        } else if (choice == 2) {
            std::string a = readWord("First node: "), b = readWord("Second node: ");
            std::cout << (net.connect(a, b) ? "Connected.\n" : "Could not connect (unknown, same, or already linked).\n");
        } else if (choice == 3) {
            std::string from = readWord("Sender node: "), to = readWord("Receiver node: ");
            long long amt = readNumber("Amount: ");
            Node* s = net.findNode(from);
            Node* r = net.findNode(to);
            std::string err;
            if (!s || !r) std::cout << "Unknown node.\n";
            else {
                std::optional<Transaction> tx = s->createTransaction(r->getAddress(), amt, err);
                if (!tx) std::cout << "Rejected: " << err << "\n";
                else {
                    net.broadcastTransaction(from, *tx);
                    std::cout << "Transaction broadcast.\n";
                }
            }
        } else if (choice == 4) {
            net.runConsensusRound(std::cout);
        } else if (choice == 5) {
            net.printBalances(std::cout);
        } else if (choice == 6) {
            for (const auto& n : net.nodes()) n->printPool(std::cout);
        } else if (choice == 7) {
            Node* n = net.findNode(readWord("Node id: "));
            if (n) n->printChain(std::cout); else std::cout << "Unknown node.\n";
        } else if (choice == 8) {
            std::cout << (net.validateAllChains(std::cout) ? "All chains valid.\n" : "Some chains are invalid!\n");
        } else if (choice == 9) {
            net.synchronize(std::cout);
        } else if (choice == 10) {
            net.printTopology(std::cout);
        } else if (choice == 11) {
            std::string id = readWord("Node id: ");
            long long amt = readNumber("Amount: ");
            Node* n = net.findNode(id);
            if (n && amt > 0) { net.faucet(n->getAddress(), amt); std::cout << "Queued; run a consensus round to confirm.\n"; }
            else std::cout << "Invalid input.\n";
        } else if (choice != 0) {
            std::cout << "Invalid choice.\n";
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}
