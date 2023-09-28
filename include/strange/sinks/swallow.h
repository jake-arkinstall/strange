#ifndef STRANGE_SINKS_SWALLOW_HEADER
#define STRANGE_SINKS_SWALLOW_HEADER

namespace strange{
    struct swallow{
        void operator()(strange::sink, auto&&...){}
    };
}

#endif
