#ifndef STRANGE_SOURCES_RANGE_HEADER
#define STRANGE_SOURCES_RANGE_HEADER

#include "strange/core.h"

namespace strange{
    template<typename T>
    struct range{
        T start;
        T end;
        T step = 1;
        constexpr void operator()(auto&& yield) const noexcept{
            yield(strange::begin{});
            for(T i = start; i < end; i += step){
                yield(i);
            }
            yield(strange::end{});
        }
    };

    template<auto start, decltype(start) end, decltype(start) step = 1>
    struct unrolled_range{
        constexpr static decltype(start) n_iterations = (end - start) / step;
        template<std::size_t... xs>
        constexpr void flush(auto&& yield, std::index_sequence<xs...>) const noexcept{
            (yield(start + xs * step), ...); 
        }
        constexpr void operator()(auto&& yield) const noexcept{
            yield(strange::begin{});
            flush(FWD(yield), std::make_index_sequence<n_iterations>{});
            yield(strange::end{});
        }
    };
}

#endif
