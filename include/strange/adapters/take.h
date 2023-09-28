#ifndef STRANGE_ADAPTERS_TAKE_HEADER
#define STRANGE_ADAPTERS_TAKE_HEADER

#include "strange/core.h"

namespace strange{
    template<std::size_t total>
    struct take{
        std::size_t remaining = total;
        void operator()(auto&& yield, strange::begin const& _) noexcept{
            yield(_);
        }
        void operator()(auto&& yield, strange::end const& _) noexcept{
            if(remaining > 0){
                yield(_);
            }
        }
        void operator()(auto&& yield, auto const& x) noexcept{
            if(remaining == 0) return;
            yield(x);
            --remaining;
            if(remaining == 0){
                yield(strange::end{});
            }
        }
    };
}

#endif

