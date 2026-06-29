#include "SearchEngine.hpp"

#include <filesystem>
#include <iomanip>
#include <iostream>

int main(int argc, char** argv) {
    fastsearch::SearchEngine engine;
    std::filesystem::path data_dir = argc > 1 ? argv[1] : "data";
    engine.indexDirectory(data_dir);
    std::cout << "FastSearch indexed " << engine.metrics().corpus_size << " documents from " << data_dir << "\n";
    std::cout << "Commands: search text | phrase \"text here\" | auto prefix | spell token | metrics | exit\n";
    std::string line;
    while (std::cout << "> " && std::getline(std::cin, line)) {
        if (line == "exit") break;
        if (line.rfind("auto ", 0) == 0) {
            for (const auto& s : engine.autocomplete(line.substr(5))) std::cout << s << "\n";
        } else if (line.rfind("spell ", 0) == 0) {
            std::cout << engine.spellCorrect(line.substr(6)) << "\n";
        } else if (line == "metrics") {
            auto m = engine.metrics();
            std::cout << "corpus=" << m.corpus_size << " vocab=" << m.vocabulary_size << " index=" << m.index_size
                      << " avg_ms=" << std::fixed << std::setprecision(4) << m.average_latency_ms
                      << " p95_ms=" << m.p95_latency_ms << " p99_ms=" << m.p99_latency_ms
                      << " cache_hit_rate=" << m.cache_hit_rate << "\n";
        } else {
            std::string q = line.rfind("search ", 0) == 0 ? line.substr(7) : line;
            for (const auto& r : engine.search(q, 10)) {
                std::cout << r.document << " score=" << std::fixed << std::setprecision(4) << r.score << "\n"
                          << r.snippet << "\n";
            }
        }
    }
    return 0;
}
