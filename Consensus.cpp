#include "Consensus.h"
#include <algorithm>

bool Consensus::hashLess(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return a.size() < b.size();
    return a < b;  // hex digits '0'-'9' < 'a'-'f' in ASCII, so this is numeric order
}

std::vector<Candidate> Consensus::rank(std::vector<Candidate> candidates) {
    std::sort(candidates.begin(), candidates.end(),
              [](const Candidate& x, const Candidate& y) {
                  const std::string hx = x.block.getHash();
                  const std::string hy = y.block.getHash();
                  if (hx != hy) return hashLess(hx, hy);
                  return x.nodeId < y.nodeId;  // deterministic tie-break
              });
    return candidates;
}

const Candidate* Consensus::selectWinner(const std::vector<Candidate>& candidates) {
    if (candidates.empty()) return nullptr;
    const Candidate* best = &candidates[0];
    for (const Candidate& c : candidates) {
        const std::string hc = c.block.getHash();
        const std::string hb = best->block.getHash();
        if (hashLess(hc, hb) || (hc == hb && c.nodeId < best->nodeId)) best = &c;
    }
    return best;
}
