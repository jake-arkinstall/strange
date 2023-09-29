# Strange - streaming ranges

Strange is a continuation passing library that aims to provide ranges-like functionality.


Rather than using containers or iterators such as in traditional approaches, Strange uses callback pipelines.
This has several pros and cons, as discussed [here](docs/continuation_passing.md), but here's a rapid rundown:
- Instead of returning, strange components invoke a callback.
- They can do this zero times, one time, or many times.
- They don't have to do it with the same type each time, and they can yield multiple values at the same time.
- Control flow is more limited, as with any callback approach
- But the resulting code is very, very lean if you use it well.

Let's go through some examples, with some explanations along the way.

## Basic Examples

### Building a pipeline

1. Create a builder

```cpp
auto pipeline = strange::builder{};
```

2. Add pipeline components using the pipe operator

```cpp
auto pipeline = strange::builder{}
              | strange::range(1, 100)
              | strange::printer{};
```

3. Invoke the pipeline.

```cpp
auto pipeline = strange::builder{}
              | strange::range(1, 100)
              | strange::printer{};
pipeline();
```


### Iterate through a range

Let's look at what the above example does, then we can get into the how.
```cpp
#include <strange/strange.h>

int main(){
    auto pipeline = strange::builder{}
                  | strange::range(1, 100)
                  | strange::printer{};
    pipeline();
    /* Result:
     * 1
     * 2
     * ...
     * 98
     * 99
     */
}
```

`strange::range` is a simple object that invokes the remainder of the pipeline with each value in the provided range.
Its implementation is trivial:
```cpp
template<typename T>
struct range{
    T start;
    T end;
    T step = 1;
    constexpr void operator()(auto&& yield) const noexcept{
        yield(strange::begin{});
        for(T i = start; i < end; i += step){
            yield(i);
        }
        yield(strange::end{});
    }
};
```

Upon `operator(rest_of_the_pipeline)`, it first sends a `strange::begin` to the rest of the pipeline
to prepare it for incoming data. It then invokes it with the value 1. Then 2. Then 3. And so on, until it hits 99.
Then it yields a `strange::end` and returns.

`strange::printer` is a trivial wrapper over `fmt::print`.

### Iterate through an unrolled range

If you're feeling particularly metaprogrammy, you can use `unrolled_range`:
```cpp
auto pipeline = strange::builder{}
              | strange::unrolled_range<1, 100>
              | strange::printer{};
pipeline();
```

This has the same result, except without a runtime for loop. Instead, the sequence is
expanded at compile time, so that the effective output code is effectively identical to
```cpp
yield(strange::begin{});
yield(1);
yield(2);
...
yield(98);
yield(99);
yield(strange::end{});
```

### Invoke a callback on any sequence

A range of numbers is fun, but what if you have a sequence of known values
that you want to iterate through? That can be done with `strange::each`,
and the types needn't be the same:

```cpp
#include <strange/strange.h>

int main() {
    auto pipeline = strange::builder{}
                  | strange::each{1, 3.14159265, "Hello, world"}
                  | strange::printer{}
    pipeline();
    /*
    1
    3.14159265
    "Hello, world!"
    */
}
```

### File I/O

`strange::text_file_reader` and `strange::text_file_writer` can be used as strange
sources and sinks.

You first attempt to open them with their respective static `try_open` method, which
returns a std::optional containing the reader or writer. On success, plug their value
into the pipeline and away you go!

The `text_file_reader` requires a template parameter `line_length`, corresponding to the maximum line size
you want to read in. It simply creates a raw char array of this size, then flushes each
line in the file. The implementation of the invocation is trivial:
```cpp
void operator()(auto&& yield) const noexcept{
    char buffer[line_length];
    yield(strange::begin{});
    while(fgets(buffer, line_length, handle)){
        yield(buffer);
    }
    yield(strange::end{});
}
```

The `text_file_writer` doesn't need a buffer. It simply accepts a string from
a prior component, then writes it to the file with an appended newline.

Let's have a look at this in action. Here we open a reader and a writer. We stream
lines from the reader, pipe to some adapters, then pipe to the writer.

The adapters used for an example here are:

1. A `strange::enumerate`. When this is invoked with value `x`, it invokes the next element with values `(i, x)`,
   with `i` starting at 0 and counting upwards.

2. A `strange::transform_invoke`. You provide this with a callback as a template argument, telling it how to
   invoke the remainder of the pipeline. In this case, we choose to invoke the remainder of the pipeline
   `i+1` times for line index `i`, resulting in an ever-growing sequence of repeating lines.

If you're not feeling so adventurous and want a 1:1 mapping of inputs to outputs, there's also
`strange::transform` which simply returns a transformed value.

```cpp

#include <cassert>
#include <strange/strange.h>

int main() {
    constexpr std::size_t line_length = 512;
    auto input_file = strange::text_file_reader<line_length>::try_open("input_file.txt");
    assert(input_file.has_value());

    auto output_file = strange::text_file_writer::try_open("output_file.txt");
    assert(output_file.has_value());

    auto pipeline = strange::builder{}
                  | input_file.value()
                  | strange::enumerate{}
                  | strange::transform_invoke<
                        [](auto&& yield, std::size_t line_number, char const* line){
                            // repeat line n, n times!
                            for(std::size_t i = 0; i <= line_number; ++i){
                                yield(line);
                            }
                        }
                    >{}
                  | output_file.value()
                  | strange::swallow{};
    pipeline();
}

```

Input file:
```cpp
There once was a ship that put to sea
The name of the ship was the Billy O' Tea
The winds blew up, her bow dipped down,
Oh blow, my bully boys, blow!
```

Output file:
```cpp
There once was a ship that put to sea

The name of the ship was the Billy O' Tea

The name of the ship was the Billy O' Tea

The winds blew up, her bow dipped down,

The winds blew up, her bow dipped down,

The winds blew up, her bow dipped down,

Oh blow, my bully boys, blow!

Oh blow, my bully boys, blow!

Oh blow, my bully boys, blow!

Oh blow, my bully boys, blow!
```
