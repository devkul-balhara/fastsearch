#include "BenchmarkRunner.hpp"
#include "SearchEngine.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace fastsearch {
    double BenchmarkRunner::mean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return std::accumulate(
            values.begin(),
            values.end(),
            0.0) / values.size();
    }

    double BenchmarkRunner::median(std::vector<double> values) {
        if (values.empty()) return 0.0;
        std::sort(values.begin(), values.end());
        return values[values.size() / 2];
    }

    double BenchmarkRunner::percentile(
        std::vector<double> values,
        double p) {
        if (values.empty()) return 0.0;
        std::sort(values.begin(), values.end());
        std::size_t index = static_cast<std::size_t>(p * (values.size() - 1));
        return values[index];
    }

    std::vector<std::string> BenchmarkRunner::buildCorpus() {
        std::vector<std::string> corpus;
        std::ifstream file("benchmarks/corpus/technical_corpus.txt");
        if (!file.is_open()) {
            throw std::runtime_error(
                "Unable to open benchmarks/corpus/technical_corpus.txt");
        }
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) corpus.push_back(line);
        }
        return corpus;
    }

    std::vector<std::string> BenchmarkRunner::buildQueries() {
        return {
            // ---------- Single term ----------
            "machine",
            "learning",
            "search",
            "engine",
            "operating",
            "database",
            "redis",
            "postgresql",
            "docker",
            "kubernetes",
            "compiler",
            "cache",
            "index",
            "retrieval",
            "backend",

            // ---------- Multi-term ----------
            "machine learning",
            "distributed systems",
            "operating systems",
            "search engine",
            "information retrieval",
            "inverted index",
            "positional index",
            "virtual memory",
            "backend services",
            "cloud computing",
            "natural language",
            "process scheduling",
            "load balancing",
            "fault tolerance",
            "query processing",

            // ---------- Phrase ----------
            "\"machine learning\"",
            "\"distributed systems\"",
            "\"search engine\"",
            "\"operating systems\"",
            "\"information retrieval\"",
            "\"virtual memory\"",
            "\"load balancing\"",
            "\"fault tolerance\"",
            "\"query optimization\"",
            "\"network latency\"",
            "\"cache eviction\"",
            "\"backend services\"",
            "\"thread synchronization\"",
            "\"deadlock prevention\"",
            "\"cloud computing\"",

            // ---------- Boolean ----------
            "machine AND learning",
            "search AND engine",
            "redis OR postgresql",
            "backend AND api",
            "operating NOT windows",
            "compiler OR linker",
            "cache AND memory",
            "database AND indexing",
            "docker OR kubernetes",
            "thread AND synchronization",
            "virtual AND memory",
            "search OR retrieval",
            "distributed AND systems",
            "cloud AND computing",
            "linux NOT kernel",

            // ---------- Proximity ----------
            "machine NEAR/2 learning",
            "search NEAR/3 engine",
            "distributed NEAR/3 systems",
            "cache NEAR/2 eviction",
            "backend NEAR/2 services",
            "virtual NEAR/2 memory",
            "operating NEAR/2 systems",
            "information NEAR/2 retrieval",
            "query NEAR/2 optimization",
            "load NEAR/2 balancing",

            // ---------- Misspellings ----------
            "machien",
            "lerning",
            "opertaing",
            "databse",
            "retrival",
            "distributd",
            "compiller",
            "dockre",
            "kubernets",
            "algorithim"
        };
    }

    BenchmarkResult BenchmarkRunner::benchmarkIndexing(std::size_t docs) {
        SearchEngine engine;
        const auto corpus = buildCorpus();
        auto started = std::chrono::steady_clock::now();
        for (std::size_t i = 0; i < docs; ++i) {
            engine.addDocument("doc" + std::to_string(i),corpus[i % corpus.size()]);
        }
        BenchmarkResult result;
        result.suite = "indexing";
        result.docs = docs;
        result.average = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count();
        return result;
    }

    BenchmarkResult BenchmarkRunner::benchmarkSearch(
        std::size_t docs,
        bool warm) {
        SearchEngine engine;
        const auto corpus = buildCorpus();
        for (std::size_t i = 0; i < docs; ++i) {
            engine.addDocument("doc" + std::to_string(i),corpus[i % corpus.size()]);
        }
        const auto queries = buildQueries();
        std::vector<double> latencies;
        if (warm) {
            // Warm cache
            for (const auto& q : queries) engine.search(q);
            for (int i = 0; i < 100; ++i) {
                auto started = std::chrono::steady_clock::now();
                engine.search(queries[i % queries.size()]);
                latencies.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count());
            }
        }
        else {
            for (const auto& q : queries) {
                engine.invalidateCache();
                auto started = std::chrono::steady_clock::now();
                engine.search(q);
                latencies.push_back(
                    std::chrono::duration<double, std::milli>(
                        std::chrono::steady_clock::now() - started
                    ).count());
            }
        }
        BenchmarkResult result;
        result.suite = warm ? "search warm": "search cold";
        result.docs = docs;
        result.average = mean(latencies);
        result.median = median(latencies);
        result.p95 = percentile(latencies, 0.95);
        result.p99 = percentile(latencies, 0.99);
        return result;
    }

    BenchmarkResult BenchmarkRunner::benchmarkAutocomplete(std::size_t docs) {
        SearchEngine engine;
        const auto corpus = buildCorpus();
        for (std::size_t i = 0; i < docs; ++i) {
            engine.addDocument("doc" + std::to_string(i),corpus[i % corpus.size()]);
        }
        std::vector<double> latencies;
        for (int i = 0; i < 1000; ++i) {
            auto started = std::chrono::steady_clock::now();
            engine.autocomplete("ma");
            latencies.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count());
        }
        BenchmarkResult result;
        result.suite = "autocomplete";
        result.docs = docs;
        result.average = mean(latencies);
        result.median = median(latencies);
        result.p95 = percentile(latencies, 0.95);
        result.p99 = percentile(latencies, 0.99);
        return result;
    }

    BenchmarkResult BenchmarkRunner::benchmarkSpellCorrection(std::size_t docs) {
        SearchEngine engine;
        const auto corpus = buildCorpus();
        for (std::size_t i = 0; i < docs; ++i) {
            engine.addDocument(
                "doc" + std::to_string(i),
                corpus[i % corpus.size()]);
        }
        const std::vector<std::string> misspelled = {
            "machien",
            "opertaing",
            "retrievel",
            "serach",
            "backedn",
            "distrbuted"
        };
        std::vector<double> latencies;
        for (int i = 0; i < 500; ++i) {
            auto started = std::chrono::steady_clock::now();
            engine.spellCorrect(misspelled[i % misspelled.size()]);
            latencies.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - started).count());
        }
        BenchmarkResult result;
        result.suite = "spell correction";
        result.docs = docs;
        result.average = mean(latencies);
        result.median = median(latencies);
        result.p95 = percentile(latencies,0.95);
        result.p99 = percentile(latencies,0.99);
        return result;
    }

    BenchmarkResult BenchmarkRunner::benchmarkThroughput(std::size_t docs) {
        SearchEngine engine;
        const auto corpus = buildCorpus();
        for (std::size_t i = 0; i < docs; ++i) {
            engine.addDocument("doc" + std::to_string(i),corpus[i % corpus.size()]);
        }
        const auto queries = buildQueries();
        constexpr int total_queries = 2000;
        auto started = std::chrono::steady_clock::now();
        for (int i = 0; i < total_queries; ++i) {
            engine.search(queries[i % queries.size()]);
        }
        double elapsed =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
        BenchmarkResult result;
        result.suite = "throughput";
        result.docs = docs;
        result.throughput = total_queries / elapsed;
        return result;
    }

    void BenchmarkRunner::writeJSON(
        const std::vector<BenchmarkResult>& results) {
        std::ofstream json("benchmark_results.json");
        json << "[\n";
        for (std::size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            json << "  {\n";
            json << "    \"suite\": \"" << r.suite << "\",\n";
            json << "    \"docs\": " << r.docs << ",\n";
            json << "    \"average\": " << r.average << ",\n";
            json << "    \"median\": " << r.median << ",\n";
            json << "    \"p95\": " << r.p95 << ",\n";
            json << "    \"p99\": " << r.p99 << ",\n";
            json << "    \"throughput\": " << r.throughput << "\n";
            json << "  }";
            if (i + 1 != results.size()) json << ",";
            json << "\n";
        }
        json << "]\n";
    }

    std::string BenchmarkRunner::run() {
        std::vector<BenchmarkResult> all_results;
        std::ostringstream out;
        std::ofstream csv("benchmark_results.csv");
        out << "FastSearch Benchmark Suite\n";
        out << "==========================\n";
        out << "Compiler : C++17\n";
        out << "Corpus : technical_corpus.txt (~500 technical paragraphs)\n";
        out << "Query Workload : " << buildQueries().size() << " representative queries\n";
        out << "Benchmark Sizes : 500 → 5000 documents\n\n";

        csv << "suite,docs,avg_ms,median_ms,p95_ms,p99_ms,throughput_qps\n";
        out << "| suite | docs | avg_ms | median_ms | p95_ms | p99_ms |\n";
        out << "|---|---:|---:|---:|---:|---:|\n";
        for (std::size_t docs : {500u,1000u,2500u,5000u}){
            std::vector<BenchmarkResult> results = {
                benchmarkIndexing(docs),
                benchmarkSearch(docs, false),
                benchmarkSearch(docs, true),
                benchmarkAutocomplete(docs),
                benchmarkSpellCorrection(docs),
                benchmarkThroughput(docs)
            };
            for (const auto& r : results) {
                if (r.suite == "throughput") {
                    out << "| "
                        << r.suite
                        << " | "
                        << r.docs
                        << " | "
                        << std::fixed << std::setprecision(0)
                        << r.throughput
                        << " qps | - | - | - |\n";

                    csv << r.suite << ","
                        << r.docs << ",,,,,"
                        << r.throughput << "\n";
                }
                else if (r.suite == "indexing") {
                    out << "| "
                        << r.suite
                        << " | "
                        << r.docs
                        << " | "
                        << std::fixed << std::setprecision(3)
                        << r.average
                        << " | - | - | - |\n";
                    csv << r.suite << ","
                        << r.docs << ","
                        << r.average
                        << ",,,,\n";
                }
                else {
                    out << "| "
                        << r.suite
                        << " | "
                        << r.docs
                        << " | "
                        << std::fixed << std::setprecision(3)
                        << r.average
                        << " | "
                        << r.median
                        << " | "
                        << r.p95
                        << " | "
                        << r.p99
                        << " |\n";
                    csv << r.suite << ","
                        << r.docs << ","
                        << r.average << ","
                        << r.median << ","
                        << r.p95 << ","
                        << r.p99 << ",\n";
                }
            }
        }
        writeJSON(all_results);
        return out.str();
    }
}  // namespace fastsearch
