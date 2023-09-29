#ifndef STRANGE_PIPELINE_HEADER
#define STRANGE_PIPELINE_HEADER

#include <type_traits>

#include "strange/core.h"

namespace strange{
    template<typename T>
    concept appendable_pipeline = T::appendable_pipeline == true;

    struct builder{
        constexpr static bool appendable_pipeline = true;
        constexpr auto operator()(auto... xs) const noexcept{
            // static_assert(false) is, at the time of writing, ill formed.
            // I only want this to trigger iff this is invoked, hence the sizeof hack.
            static_assert(sizeof...(xs) < 0, "You're trying to invoke an empty pipeline!");
        }
    };
    template<typename... component_ts>
    void pipeline_params_rejected(auto... params);

    template<typename... Ts>
    struct pipeline;

    template<>
    struct pipeline<>{
        constexpr static bool appendable_pipeline = true;
        constexpr static bool callable_pipeline = false;
    };

    template<typename component_t>
    struct pipeline<component_t>{
        constexpr static bool appendable_pipeline = true;
        constexpr static bool callable_pipeline = true;
        component_t component;
        template<typename... param_ts>
        constexpr void operator()(param_ts&&... params) noexcept{
            if constexpr(std::is_invocable_v<component_t, sink, param_ts...>){
                component(sink{}, FWD(params)...);
            }else{
                return pipeline_params_rejected<component_t>(params...);
            }
        }
    };

    template<typename component_t, typename... continuation_ts>
    struct pipeline<component_t, continuation_ts...>{
        constexpr static bool appendable_pipeline = true;
        constexpr static bool callable_pipeline = true;
        using tail_t = pipeline<continuation_ts...>;
        component_t component;
        tail_t continuation;
        template<typename... param_ts>
        constexpr void operator()(param_ts&&... params) noexcept{
            if constexpr(std::is_invocable_v<component_t, tail_t, param_ts...>){
                component(continuation, FWD(params)...);
            }else{
                return pipeline_params_rejected<component_t, tail_t>(params...);
            }
        }
    };

    template<typename head_t, typename... tail_ts, typename new_component_t>
    constexpr auto append_to_pipeline(pipeline<head_t, tail_ts...> existing_pipeline,
                                      new_component_t&& new_component)
    -> pipeline<head_t, tail_ts..., new_component_t>
    {
        if constexpr(sizeof...(tail_ts) == 0){
            return {
                .component = FWD(existing_pipeline.component),
                .continuation = {
                    .component = FWD(new_component)
                }
            };
        }else{
            return {
                .component = existing_pipeline.component,
                .continuation = append_to_pipeline(
                    existing_pipeline.continuation,
                    FWD(new_component)
                )
            };
        }
    }
}

// pipes

template<typename component_t>
constexpr auto operator|(strange::builder, component_t&& new_component) -> strange::pipeline<component_t>{
    return {
        .component = FWD(new_component)
    };
}
template<typename... existing_ts, typename component_t>
constexpr auto operator|(strange::pipeline<existing_ts...>&& x, component_t&& new_component)
    -> strange::pipeline<existing_ts..., component_t>
{
    return append_to_pipeline(FWD(x), FWD(new_component));
}

#endif
