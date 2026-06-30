#include "BM25Ranker.hpp"

#include <cmath>
#include <unordered_set>

namespace fastsearch {

BM25Ranker::BM25Ranker(const ForwardIndex& forward, const PositionalIndex& index, double k1, double b)
    : forward_(forward), index_(index), k1_(k1), b_(b) {
    refresh();
}

void BM25Ranker::refresh() {
    double total = 0.0;
    for (const auto& doc : forward_.documents()) total += static_cast<double>(doc.terms.size());
    avg_doc_len_ = forward_.size() == 0 ? 0.0 : total / static_cast<double>(forward_.size());
}

double BM25Ranker::score(DocId doc_id, const std::vector<std::string>& terms) const {
    if (forward_.size() == 0 || avg_doc_len_ <= 0.0) return 0.0;
    std::unordered_set<std::string> unique(terms.begin(), terms.end());
    double total = 0.0;
    const double n = static_cast<double>(forward_.size());
    const double dl = static_cast<double>(forward_.docLength(doc_id));
    for (const auto& term : unique) {
        const int tf = index_.termFrequency(doc_id, term);
        if (tf == 0) continue;
        const double df = static_cast<double>(index_.documentFrequency(term));
        const double idf = std::log(1.0 + (n - df + 0.5) / (df + 0.5));
        const double numerator = static_cast<double>(tf) * (k1_ + 1.0);
        const double denominator = static_cast<double>(tf) + k1_ * (1.0 - b_ + b_ * (dl / avg_doc_len_));
        total += idf * (numerator / denominator);
    }
    return total;
}

double BM25Ranker::averageDocumentLength() const { return avg_doc_len_; }

}  // namespace fastsearch
