#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>

#include <catch2/catch_test_macros.hpp>
#include <strange/strange.h>

struct checker{
    std::size_t count = 0;
    std::size_t total = 0;
    std::size_t last = 0;
    void operator()(auto&& yield, std::size_t const& x) noexcept{
        REQUIRE(x > last);
        ++count;
        total += x;
        last = x;
        yield(x);
    }
    void operator()(auto&& yield, strange::begin _) noexcept{
        yield(_);
    }
    void operator()(auto&& yield, strange::end _) noexcept{
        yield(_);
    }
};


uint64_t sum_range(uint64_t N, uint64_t M) {
    REQUIRE(N < M);
    // Calculate the difference and sum of N and M
    uint64_t diff = M - N + 1;
    uint64_t sum = M + N;

    // Fail if we're going to overflow. It isn't a library failure
    // per se but having sum_range simplifies things for readers.
    REQUIRE(sum >= N);
    REQUIRE(sum >= M); 
    REQUIRE(diff <= std::numeric_limits<uint64_t>::max() / sum);

    return (diff * sum) / 2;
}

TEST_CASE("Simple range"){
    checker range_checker{};
    auto pipeline = strange::builder{}
                  | strange::range{1, 100}
                  | range_checker
                  | strange::swallow{};
    pipeline();
    REQUIRE(range_checker.count == 99);
    REQUIRE(range_checker.total == sum_range(1, 99));
    REQUIRE(range_checker.last == 99);
}

TEST_CASE("Range with drop"){
    checker drop_checker{};
    auto pipeline = strange::builder{}
                  | strange::range{1, 100}
                  | strange::drop<20>{}
                  | drop_checker
                  | strange::swallow{};
    pipeline();
    REQUIRE(drop_checker.count == 79);
    REQUIRE(drop_checker.total == sum_range(21, 99));
    REQUIRE(drop_checker.last == 99);
}

TEST_CASE("Range with take"){
    checker take_checker{};
    auto pipeline = strange::builder{}
                  | strange::range{1, 100}
                  | strange::take<20>{}
                  | take_checker
                  | strange::swallow{};
    pipeline();
    REQUIRE(take_checker.count == 20);
    REQUIRE(take_checker.total == sum_range(1, 20));
    REQUIRE(take_checker.last == 20);
}


TEST_CASE("Range with drop and take"){
    checker bare_range_checker{}, drop_range_checker{}, take_range_checker{};

    auto pipeline = strange::builder{}
                  | strange::range{1ull, 1'000'000ull}
                  | bare_range_checker
                  | strange::drop<500'000>{}
                  | drop_range_checker
                  | strange::take<10>{}
                  | take_range_checker
                  | strange::print{};
    pipeline();
    REQUIRE(bare_range_checker.count == 999'999);
    REQUIRE(bare_range_checker.total == sum_range(1, 999'999));
    REQUIRE(bare_range_checker.last == 999'999);

    REQUIRE(drop_range_checker.count == 499'999);
    REQUIRE(drop_range_checker.total == sum_range(500'001, 999'999));
    REQUIRE(drop_range_checker.last == 999'999);

    REQUIRE(take_range_checker.count == 10);
    REQUIRE(take_range_checker.total == sum_range(500'001, 500'010));
    REQUIRE(take_range_checker.last == 500'010);
}

TEST_CASE("Unrolled range"){
    checker bare_range_checker{}, drop_range_checker{}, take_range_checker{};

    auto pipeline = strange::builder{}
                  | strange::unrolled_range<1, 1'000>{}
                  | bare_range_checker
                  | strange::drop<500>{}
                  | drop_range_checker
                  | strange::take<10>{}
                  | take_range_checker
                  | strange::print{};
    pipeline();
    REQUIRE(bare_range_checker.count == 999);
    REQUIRE(bare_range_checker.total == sum_range(1, 999));
    REQUIRE(bare_range_checker.last == 999);

    REQUIRE(drop_range_checker.count == 499);
    REQUIRE(drop_range_checker.total == sum_range(501, 999));
    REQUIRE(drop_range_checker.last == 999);

    REQUIRE(take_range_checker.count == 10);
    REQUIRE(take_range_checker.total == sum_range(501, 510));
    REQUIRE(take_range_checker.last == 510);
}
