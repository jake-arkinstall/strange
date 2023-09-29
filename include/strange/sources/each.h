#ifndef STRANGE_SOURCES_EACH_HEADER
#define STRANGE_SOURCES_EACH_HEADER

#include <tuple>

namespace strange{
    template<typename... Ts>
    struct each{
        std::tuple<Ts...> ts;
        each(Ts... ts) noexcept
            : ts{ts...}
        {
        }
        constexpr void operator()(auto&& yield) const noexcept{
            yield(strange::begin{});
            std::apply([&](auto&&... ts){
                (yield(ts), ...);
            }, ts);
            yield(strange::end{});
        }
    };
    template<typename... Ts>
    each(Ts...) -> each<Ts...>;
}
#endif
