#ifndef STRANGE_ADAPTERS_TRANSFORM_INVOKE_HEADER
#define STRANGE_ADAPTERS_TRANSFORM_INVOKE_HEADER

#include "strange/core.h"

namespace strange{
    template<auto f>
    struct transform_invoke{
        void operator()(auto&& yield, strange::begin const& _) const noexcept{
            yield(_);
        }
        void operator()(auto&& yield, strange::end const& _) const noexcept{
            yield(_);
        }
        void operator()(auto&& yield, auto const&... xs) const noexcept{
            f(yield, xs...);
        }
    };
}

#endif

