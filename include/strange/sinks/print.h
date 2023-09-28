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
    };
}

#endif
