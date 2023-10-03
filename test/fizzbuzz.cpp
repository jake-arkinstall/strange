/* The classic FizzBuzz problem: Given a range of integers, print "Fizz" if the
 * integer is divisible by 3, "Buzz" if the integer is divisible by 5, and
 * "FizzBuzz" if the integer is divisible by both 3 and 5. Otherwise, print the
 * integer.
 *
 * This is silly, because the output of the fizzbuzz generator would traditionally
 * be a string. Instead, in this test, instead of printing directly, we yield Fizz,
 * Buzz, FizzBuzz or the integer. This is passed to a checker which uses the number
 * of invocations so far to determine whether the correct type was passed.
 *
 * This is a tongue-in-cheek example of using the Strange library to run through
 * a sequence of integers, use the value of each integer at runtime to yield
 * different types, and then test the type against expectations.
 *
 * */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>

#include <fmt/format.h>
#include <catch2/catch_test_macros.hpp>
#include <strange/strange.h>

// First we must define our types, and give them formatters for printing.

struct Fizz{};
struct Buzz{};
struct FizzBuzz{};

template<>
struct fmt::formatter<Fizz>{
    constexpr auto parse(format_parse_context& ctx) const {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(Fizz const&, FormatContext& ctx) const{
        return format_to(ctx.out(), "Fizz");
    }
};
template<>
struct fmt::formatter<Buzz>{
    constexpr auto parse(format_parse_context& ctx) const {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(Buzz const&, FormatContext& ctx) const{
        return format_to(ctx.out(), "Buzz");
    }
};
template<>
struct fmt::formatter<FizzBuzz>{
    constexpr auto parse(format_parse_context& ctx) const {
        return ctx.begin();
    }
    template<typename FormatContext>
    auto format(FizzBuzz const&, FormatContext& ctx) const{
        return format_to(ctx.out(), "FizzBuzz");
    }
};

// Now we need to construct a checker that will be used to verify the output
// of the fizzbuzz process. We use the number of invocations so far to determine
// whether the correct type was passed, using modulo checks.

struct FizzBuzzChecker{
    uint64_t expected_invocations;
    uint64_t invocations = 0;
    void operator()(auto&& yield, strange::begin x) noexcept{
        // The sequence has only just begun, so we haven't seen any invocations
        REQUIRE(invocations == 0);
        // Pass it along so that the next component knows that the sequence has begun.
        yield(x);
    }
    void operator()(auto&& yield, uint64_t const& x) noexcept{
        // We've received an integer, so it shouldn't be divisible by 3 or 5.
        ++invocations;
        REQUIRE(invocations <= expected_invocations);
        REQUIRE(x == invocations);
        REQUIRE(invocations % 5 != 0);
        REQUIRE(invocations % 3 != 0);
        // pass it along to the next component
        yield(x);
    }
    void operator()(auto&& yield, FizzBuzz x) noexcept{
        // We've received a FizzBuzz, so it should be divisible by 3 or 5.
        ++invocations;
        REQUIRE(invocations % 3 == 0);
        REQUIRE(invocations % 5 == 0);
        // pass it along to the next component
        yield(x);
    }
    void operator()(auto&& yield, Fizz x) noexcept{
        // We've received a Fizz, so it should be divisible by 3 but not 5.
        ++invocations;
        REQUIRE(invocations % 3 == 0);
        REQUIRE(invocations % 5 != 0);
        // pass it along to the next component
        yield(x);
    }
    void operator()(auto&& yield, Buzz x) noexcept{
        // We've received a Buzz, so it should be divisible by 5 but not 3.
        ++invocations;
        REQUIRE(invocations % 3 != 0);
        REQUIRE(invocations % 5 == 0);
        // pass it along to the next component
        yield(x);
    }
    void operator()(auto&& yield, strange::end x) noexcept{
        // The sequence has ended, so we expect the number invocations to match the expected invocations.
        REQUIRE(invocations == expected_invocations);
        // Pass it along so that the next component knows that the sequence has ended.
        yield(x);
    }
};

TEST_CASE("FizzBuzz to 100k"){
    FizzBuzzChecker checker{.expected_invocations = 99999};

    auto pipeline = strange::builder{}
                  // loop from 1 to 99,999 inclusive
                  | strange::range{1ull, 100000ull}
                  // transform each integer into a Fizz, Buzz, FizzBuzz or integer
                  | strange::transform_invoke<
                      [](auto&& yield, uint64_t const& i){
                        bool const divides_3 = i % 3 == 0;
                        bool const divides_5 = i % 5 == 0;
                        if(divides_3 && divides_5){
                            yield(FizzBuzz{});
                        }else if(divides_3){
                            yield(Fizz{});
                        }else if(divides_5){
                            yield(Buzz{});
                        }else{
                            yield(i);
                        }
                      }>{}
                  // check the output of the prior transformation
                  | checker
                  // print the output so we can see it in all of its beauty
                  | strange::print{};
    pipeline();
}
