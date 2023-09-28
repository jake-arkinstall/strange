#ifndef STRANGE_ADAPTERS_DROP_HEADER
#define STRANGE_ADAPTERS_DROP_HEADER

#include "strange/core.h"

namespace strange{
    template<std::size_t total>
    struct drop{
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
            if(remaining > 0){
                --remaining;
                return;
            }
            yield(x);
        }
    };
}

#endif
