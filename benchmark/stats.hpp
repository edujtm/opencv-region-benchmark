#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <cmath>

long mean(const std::vector<int>& values) {
    long result = 0;
    for (auto& value : values) {
        result += value;
    }
    return result / values.size();
}

long std_dev(const std::vector<int>& values, int mean) {
    long sum_squares = 0;
    for (auto& value : values) {
        sum_squares += (value - mean) * (value - mean);
    }
    return std::sqrt(sum_squares / (values.size() - 1));
}

/*
 * This might add some overhead, but since this is a comparison between different implementations
 * the overhead will be present in all of them. The true execution time of the function might be
 * inaccurate, but the comparison between them is still valid.
 */
std::pair<int, int> measure_perf(int samples, const std::function<void ()>& f) {
    std::vector<int> benchmark_results;

    for (int i = 0; i < samples; i++) {
       auto begin = std::chrono::steady_clock::now(); 
       f();
       auto end = std::chrono::steady_clock::now();
       auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
       benchmark_results.push_back(elapsed);
    }
    auto bench_mean = mean(benchmark_results);
    auto bench_std = std_dev(benchmark_results, bench_mean);
    return std::make_pair(bench_mean, bench_std);
}
