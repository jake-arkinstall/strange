#ifndef STRANGE_ADAPTERS_ENUMERATE_HEADER
#define STRANGE_ADAPTERS_ENUMERATE_HEADER

#include "strange/core.h"

namespace strange{
    struct enumerate{
        std::size_t index = 0;
        void operator()(auto&& yield, strange::begin const& _) noexcept{
            yield(_);
        }
        void operator()(auto&& yield, strange::end const& _) noexcept{
            yield(_);
        }
        void operator()(auto&& yield, auto const&... xs) noexcept{
            yield(index++, xs...);
        }
    };
}

#endif

