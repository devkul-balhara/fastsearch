#include "ForwardIndex.hpp"

#include <algorithm>
#include <cctype>

namespace fastsearch {

void ForwardIndex::clear() { docs_.clear(); }

DocId ForwardIndex::addDocument(std::string name, std::string text, std::vector<std::string> terms) {
    const DocId id = docs_.size();
    docs_.push_back(Document{id, std::move(name), std::move(text), std::move(terms)});
    return id;
}

const Document* ForwardIndex::get(DocId id) const {
    if (id >= docs_.size()) return nullptr;
    return &docs_[id];
}

const std::vector<Document>& ForwardIndex::documents() const { return docs_; }

std::size_t ForwardIndex::size() const { return docs_.size(); }

std::size_t ForwardIndex::docLength(DocId id) const {
    const Document* doc = get(id);
    return doc == nullptr ? 0 : doc->terms.size();
}

std::string ForwardIndex::snippet(DocId id, const std::vector<std::string>& terms, std::size_t radius) const {
    const Document* doc = get(id);
    if (doc == nullptr) return {};
    std::string lower = doc->text;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::size_t pos = std::string::npos;
    for (const auto& term : terms) {
        pos = lower.find(term);
        if (pos != std::string::npos) break;
    }
    if (pos == std::string::npos) pos = 0;
    const std::size_t begin = pos > radius ? pos - radius : 0;
    const std::size_t end = std::min(doc->text.size(), pos + radius);
    std::string s = doc->text.substr(begin, end - begin);
    if (begin > 0) s = "..." + s;
    if (end < doc->text.size()) s += "...";
    return s;
}

}  // namespace fastsearch
