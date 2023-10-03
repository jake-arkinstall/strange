#ifndef STRANGE_SINKS_PRINT_HEADER
#define STRANGE_SINKS_PRINT_HEADER

#include <fmt/format.h>
#include "strange/core.h"

namespace strange{
    struct print{
        void operator()(strange::sink, strange::begin) const noexcept{}
        void operator()(strange::sink, strange::end) const noexcept{}
        void operator()(strange::sink, auto&& value) const noexcept{
            fmt::print("- {}\n", value);
        }
        void operator()(auto&& yield, strange::begin x) const noexcept{
            yield(x);
        }
        void operator()(auto&& yield, strange::end x) const noexcept{
            yield(x);
        }
        void operator()(auto&& yield, auto&& value) const noexcept{
            fmt::print("- {}\n", value);
            yield(value);
        }
    };
}

#endif
