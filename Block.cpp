#include "Block.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>

namespace {

std::string currentTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() % 1000;

    std::tm utcTime{};
#ifdef _WIN32
    gmtime_s(&utcTime, &nowTime);
#else
    gmtime_r(&nowTime, &utcTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%S")
           << '.' << std::setfill('0') << std::setw(3) << milliseconds << 'Z';
    return stream.str();
}

void appendField(std::ostringstream& stream, const std::string& value) {
    stream << value.size() << ':' << value;
}

void appendInteger(std::ostringstream& stream, long long value) {
    appendField(stream, std::to_string(value));
}

std::string sha256(const std::string& data) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        throw std::runtime_error("Unable to create OpenSSL digest context");
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;
    const bool success = EVP_DigestInit_ex(context, EVP_sha256(), nullptr) == 1 &&
                         EVP_DigestUpdate(context, data.data(), data.size()) == 1 &&
                         EVP_DigestFinal_ex(context, digest, &digestLength) == 1;
    EVP_MD_CTX_free(context);

    if (!success) {
        throw std::runtime_error("OpenSSL SHA-256 calculation failed");
    }

    std::ostringstream hex;
    hex << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < digestLength; ++i) {
        hex << std::setw(2) << static_cast<unsigned int>(digest[i]);
    }
    return hex.str();
}

}  // namespace

Block::Block(int index,
             const std::vector<Transaction>& transactions,
             const std::string& previousHash,
             int nonce,
             const std::string& minerAddress)
    : index(index),
      timestamp(currentTimestamp()),
      transactions(transactions),
      previousHash(previousHash),
      nonce(nonce),
      minerAddress(minerAddress) {
    recalculateHash();
}

std::string Block::calculateHash() const {
    std::ostringstream serialized;
    appendInteger(serialized, index);
    appendField(serialized, timestamp);
    appendField(serialized, previousHash);
    appendInteger(serialized, nonce);
    appendField(serialized, minerAddress);
    appendInteger(serialized, static_cast<long long>(transactions.size()));

    for (const Transaction& transaction : transactions) {
        appendField(serialized, transaction.getSender());
        appendField(serialized, transaction.getReceiver());
        appendInteger(serialized, transaction.getAmount());
        appendInteger(serialized, transaction.getTimestamp());
    }

    return sha256(serialized.str());
}

void Block::displayBlock() const {
    std::cout << "========== Block " << index << " ==========\n"
              << "Timestamp: " << timestamp << '\n'
              << "Previous Hash: " << previousHash << '\n'
              << "Nonce: " << nonce << '\n'
              << "Miner Address: " << minerAddress << '\n'
              << "Transactions (" << transactions.size() << "):\n";

    if (transactions.empty()) {
        std::cout << "  (none)\n";
    } else {
        for (std::size_t i = 0; i < transactions.size(); ++i) {
            const Transaction& transaction = transactions[i];
            std::cout << "  " << i + 1 << ". " << transaction.getSender()
                      << " -> " << transaction.getReceiver()
                      << " | Amount: " << transaction.getAmount()
                      << " | Transaction timestamp: " << transaction.getTimestamp()
                      << '\n';
        }
    }
    std::cout << "Block Hash: " << hash << "\n==============================\n";
}

int Block::getIndex() const { return index; }
const std::string& Block::getTimestamp() const { return timestamp; }
const std::vector<Transaction>& Block::getTransactions() const { return transactions; }
const std::string& Block::getPreviousHash() const { return previousHash; }
int Block::getNonce() const { return nonce; }
const std::string& Block::getMinerAddress() const { return minerAddress; }
const std::string& Block::getHash() const { return hash; }

void Block::setTimestamp(const std::string& value) { timestamp = value; }
void Block::setTransactions(const std::vector<Transaction>& value) { transactions = value; }
void Block::setPreviousHash(const std::string& value) { previousHash = value; }
void Block::setNonce(int value) { nonce = value; }
void Block::setMinerAddress(const std::string& value) { minerAddress = value; }
void Block::recalculateHash() { hash = calculateHash(); }
