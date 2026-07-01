const api = "http://localhost:8080";

const q = document.querySelector("#query");
const results = document.querySelector("#results");
const suggestions = document.querySelector("#suggestions");
const latency = document.querySelector("#latency");
const count = document.querySelector("#count");
const metrics = document.querySelector("#metrics");

function esc(s) {
    return s.replace(/[&<>"']/g, c => ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;"
    }[c]));
}

function highlight(text) {
    return text;
}

async function search() {

    if (!q.value.trim())
        return;

    results.innerHTML = `
        <div class="empty-state">
            <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"><circle cx="11" cy="11" r="8"></circle><line x1="21" y1="21" x2="16.65" y2="16.65"></line></svg>
            <p>Searching...</p>
        </div>
    `;

    const start = performance.now();

    const res = await fetch(
        `${api}/search?q=${encodeURIComponent(q.value)}`
    );

    const data = await res.json();

    // Adjusted to drop the manual labels since HTML has icons now
    latency.textContent = `${(performance.now() - start).toFixed(1)} ms`;
    count.textContent = `${data.results.length} found`;

    if (data.results.length === 0) {
        results.innerHTML = `
            <div class="empty-state">
                <svg width="40" height="40" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="8" x2="12" y2="12"></line><line x1="12" y1="16" x2="12.01" y2="16"></line></svg>
                <h3>No Results Found</h3>
                <p>Try refining your search query.</p>
            </div>
        `;
        refreshMetrics();
        return;
    }

    results.innerHTML = data.results.map(r => `
        <article class="result">
            <div class="result-header">
                <h3>${esc(r.document)}</h3>
                <span class="score">Score: ${Number(r.score).toFixed(3)}</span>
            </div>
            <p class="snippet">${highlight(r.snippet)}</p>
        </article>
    `).join("");

    refreshMetrics();
}

async function complete() {

    const prefix = q.value.split(/\s+/).pop();

    if (!prefix) {
        suggestions.innerHTML = "";
        return;
    }

    const res = await fetch(
        `${api}/autocomplete?prefix=${encodeURIComponent(prefix)}`
    );

    const data = await res.json();

    suggestions.innerHTML = data.suggestions
        .map(s => `<button>${esc(s)}</button>`)
        .join("");

    suggestions
        .querySelectorAll("button")
        .forEach(button => {
            button.addEventListener("click", () => {
                q.value = q.value.replace(/\S*$/, button.textContent);
                suggestions.innerHTML = "";
                search();
            });
        });
}

async function refreshMetrics() {

    const res = await fetch(`${api}/metrics`);
    const m = await res.json();

    function formatLabel(label) {
      return label
          .replace(/_ms$/i, "")
          .replace(/_/g, " ")
          .replace(/\b\w/g, c => c.toUpperCase());
    }

    function formatValue(key, value) {
        if (key.includes("size")) return Number(value).toFixed(0);
        if (key.includes("latency")) return `${Number(value).toFixed(3)} ms`;
        if (key.includes("cache")) return `${(Number(value) * 100).toFixed(1)}%`;
        return value;
    }

    metrics.innerHTML = Object.entries(m)
        .map(([k, v]) => `
            <div class="metric-row">
                <dt>${formatLabel(k)}</dt>
                <dd>${formatValue(k, v)}</dd>
            </div>
        `)
        .join("");
}

document.querySelector("#search").addEventListener("click", search);

q.addEventListener("keydown", e => {
    if (e.key === "Enter") search();
});

q.addEventListener("input", () => {
  complete();
  if (q.value.trim() === "") {
    results.innerHTML = "";
    count.textContent = "0 found";
    latency.textContent = "0 ms";
  }
});

refreshMetrics();