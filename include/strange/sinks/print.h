#ifndef STRANGE_SINKS_PRINT_HEADER
#define STRANGE_SINKS_PRINT_HEADER

#include <cstdio>
#include "strange/core.h"

namespace strange{
    struct print{
        void operator()(strange::sink, strange::begin) const noexcept{}
        void operator()(strange::sink, int const& value) const noexcept{
            printf("integer - %d\n", value);
        }
        void operator()(strange::sink, double const& value) const noexcept{
            printf("double  - %f\n", value);
        }
        void operator()(strange::sink, uint64_t const& value) const noexcept{
            printf("uint64  - %lu\n", value);
        }
        void operator()(strange::sink, strange::end) const noexcept{}
    };
}

#endif
