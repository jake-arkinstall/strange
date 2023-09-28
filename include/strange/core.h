#ifndef STRANGE_API_HEADER
#define STRANGE_API_HEADER

#define FWD(a) std::forward<decltype(a)>(a)

namespace strange{
    struct begin{};
    struct end{};

    struct sink{
        constexpr static bool appendable_pipeline = false;
        constexpr auto operator()(auto... xs) const noexcept{
            // static_assert(false) is, at the time of writing, ill formed.
            // I only want this to trigger iff this is invoked, hence the sizeof hack.
            static_assert(sizeof...(xs) < 0, "You're trying to invoke a sink!");
        }
    };


}

#endif
