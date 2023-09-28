#ifndef STRANGE_ADAPTERS_FILTER_HEADER
#define STRANGE_ADAPTERS_FILTER_HEADER

#include "strange/core.h"

namespace strange{
    template<auto f>
    struct filter{
        void operator()(auto&& yield, strange::begin const& _) noexcept{
            yield(_);
        }
        void operator()(auto&& yield, strange::end const& _) noexcept{
            yield(_);
        }
        void operator()(auto&& yield, auto const&... xs) noexcept{
            if(f(xs...)){
                yield(xs...);
            }
        }
    };
}

#endif

