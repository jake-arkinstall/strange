#ifndef STRANGE_SINKS_TEXTFILE_HEADER
#define STRANGE_SINKS_TEXTFILE_HEADER

#include <cstdio>
#include <optional>
#include <filesystem>
#include "strange/core.h"

namespace strange{
    struct text_file_writer{
        FILE* handle;
        private:
        constexpr text_file_writer(FILE* handle) noexcept
            : handle{handle}
        {
        }

        public:
        constexpr text_file_writer(text_file_writer&& other){
            handle = other.handle;
            other.handle = nullptr;
        }
        ~text_file_writer() noexcept{
            if(handle){
                fclose(handle);
            }
        }

        constexpr void operator()(auto&& yield, strange::begin) const noexcept{}
        constexpr void operator()(auto&& yield, strange::end) const noexcept{}
        constexpr void operator()(auto&& yield, char const* line) const noexcept{
            fputs(line, handle);
            fputc('\n', handle);
            yield(line);
        }

        static std::optional<text_file_writer> try_open(char const* path, bool append = false) noexcept{
            auto handle = fopen(path, append ? "w+" : "w");
            if(!handle){
                return std::nullopt;
            }
            return text_file_writer{handle};
        }
        static std::optional<text_file_writer> try_open(std::filesystem::path const& path, bool append = false) noexcept{
            return try_open(path.c_str(), append);
        }
    };
}

#endif
