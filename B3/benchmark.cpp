#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

using Clock = std::chrono::steady_clock;
volatile double sink = 0;
struct Base {
    virtual ~Base() = default;
    virtual double total() const = 0;
};
template<int K> struct VBill : Base {
    double a, b, c;
    VBill(double x, double y, double z) : a(x), b(y), c(z) {}
    double total() const override
    {
        if constexpr (K == 0) return a + b;
        if constexpr (K == 1) return a * b + c;
        if constexpr (K == 2) return a;
        return a + b * c;
    }
};
struct Tagged {
    int kind;
    double a, b, c;
    double total() const
    {
        switch (kind) {
        case 0: return a + b;
        case 1: return a * b + c;
        case 2: return a;
        default: return a + b * c;
        }
    }
};
template<int K> struct Plain {
    double a, b, c;
    double total() const
    {
        if constexpr (K == 0) return a + b;
        if constexpr (K == 1) return a * b + c;
        if constexpr (K == 2) return a;
        return a + b * c;
    }
};
using Variant = std::variant<Plain<0>, Plain<1>, Plain<2>, Plain<3>>;
std::uint32_t randomValue(std::uint32_t& seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}
template<typename F> double medianMs(F action)
{
    std::array<double, 3> samples;
    action();
    for (double& sample : samples) {
        auto start = Clock::now();
        action();
        auto finish = Clock::now();
        sample = std::chrono::duration<double, std::milli>(finish - start).count();
    }
    std::sort(samples.begin(), samples.end());
    return samples[1];
}
std::unique_ptr<Base> makeVirtual(int k, double a, double b, double c)
{
    switch (k) {
    case 0: return std::make_unique<VBill<0>>(a, b, c);
    case 1: return std::make_unique<VBill<1>>(a, b, c);
    case 2: return std::make_unique<VBill<2>>(a, b, c);
    default: return std::make_unique<VBill<3>>(a, b, c);
    }
}
Variant makeVariant(int k, double a, double b, double c)
{
    switch (k) {
    case 0: return Plain<0>{a, b, c};
    case 1: return Plain<1>{a, b, c};
    case 2: return Plain<2>{a, b, c};
    default: return Plain<3>{a, b, c};
    }
}
void billBenchmark(std::size_t n)
{
    double sumV = 0, sumS = 0, sumR = 0;
    double tv, ts, tr;
    {
        std::vector<std::unique_ptr<Base>> data;
        data.reserve(n);
        std::uint32_t seed = 20261123;
        for (std::size_t i = 0; i < n; ++i) {
            int k = randomValue(seed) % 4;
            if (i < 4) k = int(i);
            data.push_back(makeVirtual(k, 500 + i % 100, 3, 200));
        }
        tv = medianMs([&] {
            double s = 0;
            for (const auto& b : data) s += b->total();
            sink = sumV = s;
        });
    }
    {
        std::vector<Tagged> data;
        data.reserve(n);
        std::uint32_t seed = 20261123;
        for (std::size_t i = 0; i < n; ++i) {
            int k = randomValue(seed) % 4;
            if (i < 4) k = int(i);
            data.push_back({k, double(500 + i % 100), 3, 200});
        }
        ts = medianMs([&] {
            double s = 0;
            for (const auto& b : data) s += b.total();
            sink = sumS = s;
        });
    }
    {
        std::vector<Variant> data;
        data.reserve(n);
        std::uint32_t seed = 20261123;
        for (std::size_t i = 0; i < n; ++i) {
            int k = randomValue(seed) % 4;
            if (i < 4) k = int(i);
            data.push_back(makeVariant(k, 500 + i % 100, 3, 200));
        }
        tr = medianMs([&] {
            double s = 0;
            for (const auto& b : data) {
                s += std::visit([](const auto& x) { return x.total(); }, b);
            }
            sink = sumR = s;
        });
    }
    assert(sumV == sumS && sumS == sumR);
    if (n == 10) {
        std::uint32_t seed = 20261123;
        for (std::size_t i = 0; i < n; ++i) {
            int k = randomValue(seed) % 4;
            if (i < 4) k = int(i);
            Tagged record{k, double(500 + i % 100), 3, 200};
            auto virtualBill = makeVirtual(k, record.a, record.b, record.c);
            auto variantBill = makeVariant(k, record.a, record.b, record.c);
            double visited = std::visit([](const auto& x) { return x.total(); }, variantBill);
            assert(record.total() == virtualBill->total());
            assert(record.total() == visited);
            std::cout << "small," << i << ",type=" << k
                      << ",virtual=" << virtualBill->total()
                      << ",switch=" << record.total()
                      << ",variant=" << visited << '\n';
        }
    }
    std::cout << "bills," << n << ',' << tv << ',' << ts << ',' << tr
              << ',' << std::setprecision(0) << sumV << std::setprecision(4) << '\n';
}
__attribute__((noinline)) bool returnCode(double value, double& result)
{
    if (value < 0) return false;
    result = value;
    return true;
}
__attribute__((noinline)) std::optional<double> optionalResult(double value)
{
    if (value < 0) return std::nullopt;
    return value;
}
__attribute__((noinline)) double throwingResult(double value)
{
    if (value < 0) throw std::invalid_argument("negative");
    return value;
}
void errorBenchmark(int percent)
{
    std::vector<double> data(1000000);
    std::uint32_t seed = 20261123;
    std::vector<std::size_t> positions(data.size());
    for (std::size_t i = 0; i < data.size(); ++i) positions[i] = i;
    for (std::size_t i = positions.size() - 1; i > 0; --i) {
        std::swap(positions[i], positions[randomValue(seed) % (i + 1)]);
    }
    for (std::size_t i = 0; i < data.size(); ++i) data[i] = 100;
    for (std::size_t i = 0; i < data.size() * percent / 100; ++i) data[positions[i]] = -1;
    std::size_t badCode = 0, badOptional = 0, badThrow = 0;
    double tc = medianMs([&] {
        double sum = 0;
        std::size_t bad = 0;
        for (double x : data) {
            double result = 0;
            if (returnCode(x, result)) sum += result;
            else ++bad;
        }
        badCode = bad;
        sink = sum;
    });
    double to = medianMs([&] {
        double sum = 0;
        std::size_t bad = 0;
        for (double x : data) {
            auto result = optionalResult(x);
            if (result) sum += *result;
            else ++bad;
        }
        badOptional = bad;
        sink = sum;
    });
    double te = medianMs([&] {
        double sum = 0;
        std::size_t bad = 0;
        for (double x : data) {
            try { sum += throwingResult(x); }
            catch (const std::invalid_argument&) { ++bad; }
        }
        badThrow = bad;
        sink = sum;
    });
    assert(badCode == badOptional && badOptional == badThrow);
    assert(badCode == data.size() * percent / 100);
    std::cout << "errors," << percent << ',' << tc << ',' << to << ',' << te
              << ',' << badCode << '\n';
}
int main()
{
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "sizeof: virtual_object=" << sizeof(VBill<0>)
              << " unique_ptr=" << sizeof(std::unique_ptr<Base>)
              << " tagged=" << sizeof(Tagged) << " variant=" << sizeof(Variant)
              << " optional_double=" << sizeof(std::optional<double>) << '\n';
    std::cout << "bills,n,virtual_ms,switch_ms,variant_ms,checksum\n";
    for (std::size_t n : {10, 1000, 1000000, 10000000}) billBenchmark(n);
    std::cout << "errors,percent,return_ms,optional_ms,exception_ms,rejected\n";
    for (int p : {0, 1, 50}) errorBenchmark(p);
}
