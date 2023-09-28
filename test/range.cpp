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
                  | strange::range{1, 1'000'000}
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
    REQUIRE(drop_range_checker.last == 499'999);

    REQUIRE(take_range_checker.count == 10);
    REQUIRE(take_range_checker.total == sum_range(500'000, 500'009));
    REQUIRE(take_range_checker.last == 500'009);
}

TEST_CASE("Unrolled range"){
    checker bare_range_checker{}, drop_range_checker{}, take_range_checker{};

    auto pipeline = strange::builder{}
                  | strange::unrolled_range<1, 1'000'000>{}
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
    REQUIRE(drop_range_checker.last == 499'999);

    REQUIRE(take_range_checker.count == 10);
    REQUIRE(take_range_checker.total == sum_range(500'000, 500'009));
    REQUIRE(take_range_checker.last == 500'009);
}

/*
 * Flow: 
 *
 * Range  | Drop 10  | Take 10
 * ------ | -------- | -------
 *   1    |
 *   2    |
 *   3    |
 *   4    |
 *   5    |
 *   6    |
 *   7    |
 *   8    |
 *   9    |
 *   10   |
 *   11   |  11      |   11 
 *   12   |  12      |   12
 *   13   |  13      |   13
 *   14   |  14      |   14
 *   15   |  15      |   15
 *   16   |  16      |   16
 *   17   |  17      |   17
 *   18   |  18      |   18
 *   19   |  19      |   19
 *   20   |  20      |   20
 *   21   |  21
 *   22   |  22
 *   23   |  23
 *   24   |  24
 *   25   |  25
 *   26   |  26
 *   27   |  27
 *   28   |  28
 *   29   |  29
 *   30   |  30
 *   31   |  31
 *   32   |  32
 *   33   |  33
 *   34   |  34
 *   35   |  35
 *   36   |  36
 *   37   |  37
 *   38   |  38
 *   39   |  39
 *   40   |  40
 *   41   |  41
 *   42   |  42
 *   43   |  43
 *   44   |  44
 *   45   |  45
 *   46   |  46
 *   47   |  47
 *   48   |  48
 *   49   |  49
 *   50   |  50
 *   51   |  51
 *   52   |  52
 *   53   |  53
 *   54   |  54
 *   55   |  55
 *   56   |  56
 *   57   |  57
 *   58   |  58
 *   59   |  59
 *   60   |  60
 *   61   |  61
 *   62   |  62
 *   63   |  63
 *   64   |  64
 *   65   |  65
 *   66   |  66
 *   67   |  67
 *   68   |  68
 *   69   |  69
 *   70   |  70
 *   71   |  71
 *   72   |  72
 *   73   |  73
 *   74   |  74
 *   75   |  75
 *   76   |  76
 *   77   |  77
 *   78   |  78
 *   79   |  79
 *   80   |  80
 *   81   |  81
 *   82   |  82
 *   83   |  83
 *   84   |  84
 *   85   |  85
 *   86   |  86
 *   87   |  87
 *   88   |  88
 *   89   |  89
 *   90   |  90
 *   91   |  91
 *   92   |  92
 *   93   |  93
 *   94   |  94
 *   95   |  95
 *   96   |  96
 *   97   |  97
 *   98   |  98
 *   99   |  99
 */
