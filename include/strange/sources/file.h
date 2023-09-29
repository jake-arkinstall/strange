#ifndef STRANGE_SOURCES_TEXTFILE_HEADER
#define STRANGE_SOURCES_TEXTFILE_HEADER

#include <cstdio>
#include <optional>
#include <filesystem>
#include "strange/core.h"

namespace strange{
    template<std::size_t line_length>
    struct text_file_reader{
        FILE* handle;
        private:
        constexpr text_file_reader(FILE* handle) noexcept
            : handle{handle}
        {
        }

        public:
        constexpr text_file_reader(text_file_reader&& other){
            handle = other.handle;
            other.handle = nullptr;
        }
        ~text_file_reader() noexcept{
            if(handle){
                fclose(handle);
            }
        }
        constexpr void operator()(auto&& yield) const noexcept{
            char buffer[line_length];
            yield(strange::begin{});
            while(fgets(buffer, line_length, handle)){
                yield(buffer);
            }
            yield(strange::end{});
        }

        static std::optional<text_file_reader> try_open(std::filesystem::path const& path, bool skip_header = false) noexcept{
            auto handle = fopen(path.c_str(), "r");
            if(!handle){
                return std::nullopt;
            }
            if(skip_header){
                int character;
                do{
                    character = fgetc(handle);
                }while(character != EOF && character != '\n');
            }
            return text_file_reader{handle};
        }
    };
}

#endif
