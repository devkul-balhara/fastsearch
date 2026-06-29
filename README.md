# FastSearch — In-Memory Search Engine (C++17)

<p align="center">

**An in-memory search engine built in modern C++17 implementing core Information Retrieval techniques including BM25 ranking, positional indexing, Boolean search, phrase search, proximity search and query optimization.**

</p>

---

# Overview

- FastSearch is an educational search engine built completely from scratch in **modern C++17** to demonstrate the core algorithms used in Information Retrieval (IR) systems.
- Instead of relying on existing search libraries, the project implements indexing, ranking, query processing, caching and benchmarking from first principles while remaining lightweight and dependency-free.
- The project supports positional indexing, BM25 ranking, Boolean retrieval, phrase search, proximity search, autocomplete, spell correction, REST APIs, performance benchmarking and an interactive web dashboard.

---

## Highlights

- Implements core Information Retrieval algorithms from scratch.
- Supports BM25 ranking, Boolean, phrase and proximity search.
- Includes REST APIs, automated tests and an interactive web dashboard.
- Built in modern C++17 with minimal external dependencies.

---

# Why FastSearch?

Modern search systems are significantly more sophisticated than simple string matching.

FastSearch explores the complete retrieval pipeline including:

- Efficient document indexing
- Query parsing and optimization
- BM25 ranking
- Positional retrieval
- Low-latency search and context-aware result presentation
- Query caching
- Performance benchmarking

The project focuses on implementing the underlying algorithms rather than relying on third-party search libraries.

---

# Features

## Information Retrieval

- Forward Index
- Positional Inverted Index
- BM25 Ranking
- Top-K Retrieval using Min Heap

## Search

- Keyword Search
- Boolean Search
- Phrase Search
- Proximity Search
- Context-Aware Snippet Generation with Intelligent Highlighting

Example queries

```text
machine learning

"search engine"

machine AND ranking

database OR indexing

search NEAR/4 ranking

---

## Query Processing

- Query Parser
- Text Normalization
- Stop-word Handling
- Posting List Ordering
- Phrase Verification
- Proximity Verification

---

## Autocomplete

Trie-based autocomplete supporting

- Prefix search
- Frequency ranking
- Fast suggestions

---

## Spell Correction

Levenshtein-distance based spell correction with Trie candidate generation.

Example

```text
machien
↓
machine
```

---

## Performance

- LRU Query Cache
- Metrics Collector
- Benchmark Suite
- Search Throughput Measurement
- Latency Statistics
- P95 / P99 Reporting

---

## Interfaces

- Command Line Interface
- REST API
- Interactive Web Dashboard

---

# Architecture

```mermaid
flowchart LR

D[Documents]

D --> T[Tokenization & Normalization]

T --> F[Forward Index]

T --> P[Positional Inverted Index]

T --> TR[Trie]

Q[User Query]

Q --> Parser[Query Parser]

Parser --> Eval[Boolean / Phrase / Proximity Evaluation]

Eval --> BM25[BM25 Ranker]

BM25 --> Heap[Top-K Min Heap]

Heap --> Cache[LRU Query Cache]

Cache --> API[CLI / REST API / Dashboard]
```

---

# Components

| Component | Responsibility |
|------------|----------------|
| ForwardIndex | Stores original documents for context-aware snippet generation and result presentation |
| PositionalIndex | Stores positional postings for phrase and proximity search |
| QueryParser | Parses and validates user queries |
| BM25Ranker | Scores candidate documents |
| Trie | Prefix autocomplete |
| SpellCorrector | Levenshtein-based correction |
| LRUCache | Query result caching |
| MetricsCollector | Latency and cache statistics |
| BenchmarkRunner | Performance benchmarking |

---

# Indexing Pipeline

```
Documents
      │
      ▼
Tokenization
      │
      ▼
Normalization
      │
      ▼
Forward Index
      │
      ▼
Positional Inverted Index
      │
      ▼
Trie
      │
      ▼
Ready for Search
```

---

# Query Execution Pipeline

```
User Query
      │
      ▼
Query Parser
      │
      ▼
Posting Retrieval
      │
      ▼
Boolean / Phrase / Proximity Evaluation
      │
      ▼
BM25 Ranking
      │
      ▼
Top-K Heap
      │
      ▼
LRU Cache
      │
      ▼
Search Results
```

---

# REST API

| Method | Endpoint | Description |
|---------|----------|-------------|
| GET | `/search?q=` | Returns ranked search results |
| GET | `/autocomplete?prefix=` | Returns autocomplete suggestions |
| POST | `/upload` | Uploads a new in-memory document |
| POST | `/rebuild` | Rebuilds the index from the data directory |
| GET | `/metrics` | Returns engine statistics |

---

# Project Structure

```text
FastSearch
│
├── api/                    REST API implementation
├── benchmarks/
│   ├── corpus/
│   ├── plots/
│   └── benchmark.cpp
├── data/                   Sample documents
├── frontend/               HTML/CSS/JS dashboard
├── include/                Public headers
├── scripts/                Benchmark plotting utilities
├── src/                    Core implementation
├── tests/                  Unit tests
│
├── CMakeLists.txt
└── README.md
```

---

# Performance Benchmarks

The benchmark suite evaluates the engine using:

- 544 synthetic technical paragraphs
- 80 representative search queries
- Dataset sizes of **500**, **1000**, **2500** and **5000** documents

Metrics collected include:

- Indexing latency
- Cold search latency
- Warm search latency
- Autocomplete latency
- Spell correction latency
- Search throughput (queries/second)

Cold search invalidates the LRU query cache before every query.

Warm search executes repeated queries with caching enabled.

## Sample Benchmark Results

| Metric | 500 Docs | 1000 Docs | 2500 Docs | 5000 Docs |
|--------|---------:|----------:|----------:|----------:|
| Indexing | 43.3 ms | 86.4 ms | 233.5 ms | 494.3 ms |
| Cold Search | 0.246 ms | 0.356 ms | 0.657 ms | 1.171 ms |
| Warm Search | 0.006 ms | 0.007 ms | 0.007 ms | 0.007 ms |
| Throughput | 65.4K qps | 49.3K qps | 31.1K qps | 19.0K qps |

---

## Indexing Performance

<p align="center">
<img src="benchmarks/plots/indexing_time.png" width="850">
</p>

---

## Cold Search Latency

<p align="center">
<img src="benchmarks/plots/search_latency.png" width="850">
</p>

---

## Search Throughput

<p align="center">
<img src="benchmarks/plots/throughput.png" width="850">
</p>

---

# Complexity

| Operation | Complexity |
|------------|------------|
| Indexing | **O(T)** |
| Term Lookup | **O(1)** average |
| Boolean Search | **O(sum of posting lists)** |
| Phrase Search | **O(candidate positions)** |
| BM25 Top-K Ranking | **O(n log k)** |
| Autocomplete | **O(prefix + matches)** |
| LRU Cache | **O(1)** average |

---

# Design Trade-offs

- A forward index stores original documents to enable snippet generation and result presentation.
- A positional inverted index consumes more memory than a frequency-only index but enables efficient phrase and proximity search.
- The engine is intentionally in-memory to prioritize low-latency retrieval and architectural simplicity.

---

# Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

---

# Run

CLI

```bash
./build/fastsearch_cli data
```

REST API

```bash
./build/fastsearch_api data 8080
```

Benchmark Suite

```bash
./build/fastsearch_benchmark
```

Dashboard

```
http://localhost:8080
```

---

# Tech Stack

| Category | Technology |
|-----------|------------|
| Language | C++17 |
| Build System | CMake |
| Networking | POSIX Sockets |
| Frontend | HTML, CSS, JavaScript |
| Testing | CTest |
| Benchmarking | Custom Benchmark Framework |
| Visualization | Python (Matplotlib, Pandas) |

---

# Design Decisions

- BM25 replaces TF-IDF for improved document ranking.
- Positional indexing enables efficient phrase and proximity retrieval.
- Posting lists are reordered before intersection to reduce Boolean evaluation cost.
- Query results are cached using an LRU cache to accelerate repeated searches.
- A forward index stores original document text for snippet generation.
- The engine is fully in-memory to simplify the architecture and minimize query latency.

---

# Future Improvements

- Incremental indexing
- Persistent storage
- Compressed posting lists
- Parallel indexing
- Parallel query execution
- Distributed search

---

# License

This project is intended for educational and learning purposes.