#ifndef STRANGE_SINKS_TO_VECTOR_HEADER
#define STRANGE_SINKS_TO_VECTOR_HEADER

#include <vector>

#include "strange/core.h"

namespace strange{
    template<typename T>
    struct to_vector{
        std::vector<T>& result;

        void operator()(strange::sink, strange::begin) noexcept{}
        void operator()(strange::sink, strange::end) noexcept{}
        void operator()(strange::sink, T const& value) noexcept{
            result.push_back(value);
        }
    };
}

#endif
