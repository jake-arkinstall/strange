# Work in progress

This readme is a work in progress. Please take it with a pinch of salt for the time being.

# What is continuation passing?

Continuation passing is an alternative way of using functions.

Here's a normal function and its usage:

```cpp
int divide(int a, int b){
    return a / b;
}

int main(){
    int x = divide(10, 3);
    fmt::print("{}\n", x); // 3
}
```

And here's a continuation passing approach:
```cpp
void divide(auto&& yield, int a, int b){
    yield(a / b);
}

int main(){
    divide(
        [](int x){ fmt::print("{}\n", x); },
        10, 3
    ); // 3
}

```

There are some interesting consequences of this approach:

1. You can yield different types depending on runtime information.
```cpp
void divide(auto&& yield, int a, int b){
    if(a % b == 0){
        // if it divides exactly, return an int
        yield(a / b); 
    }else{
        // otherwise, return a double
        yield(static_cast<double>(a) / b);
    }
}

int main(){
    constexpr auto printer = []<typename T>(auto x){
        if constexpr(std::floating<T>){
            fmt::print("I'm a floating point number! {}\n", x);
        }else if(constexpr(std::integral<T>){
            fmt::print("I'm an integer! {}\n", x);
        }
    };

    divide(printer, 10, 3); // I'm a floating point number! 3.3333...
    divide(printer, 15, 3); // I'm an integer! 5
}
```

The equivalent approach with a normal function call would require a sum type such as std::variant.
Don't get me wrong - variants are awesome and should absolutely be used when you need to store state
somewhere long-term. They are inefficient vehicles for returning runtime-determined types, though,
because you introduce a second branch (I call this "double discovery", for lack of a better name).

Consider the variant approach:

```cpp
std::variant<int, double> divide(int a, int b){
    if(a % b == 0){
        return a / b;
    }else{
        return static_cast<double>(a) / b;
    }
}
```

This does some runtime check to determine what type to bundle into the variant. When you decide
to *use* that value, you have to do *another* runtime check:

```cpp
int main(){
    auto x = divide(10, 3); // branches on if(a % b == 0)
    std::visit(             // branches on the index held in the variant
        [](auto y){ fmt::print("{}\n", y },
        x); 
}
```

This makes sense when you're storing the variant somewhere for later, but doing it
immediately after the creation of the variant seems wasteful. If you want a variant,
you can pass the variant assignment as the callback:

```cpp
void divide(auto&& yield, int a, int b){
    if(a % b == 0){
        yield(a / b);
    }else{
        yield(static_cast<double>(a) / b);
    }
}
void divide_as_variant(int a, int b){
    std::variant<int, double> result;
    divide([&](auto x){ result = x; }, a, b); 
    return result;
}
```

This has no additional overhead compared with the `std::variant<int, double> divide(int, int)` approach,
and you should expect it to generate identical assembly.

2. You can return more than once.

For example:
```cpp
void range(auto&& yield, int start, int stop, int step){
    for(int i = start; i < stop; i += step){
        yield(i);
    }
}

int main(){
    range(
        [](int x){ fmt::print(" - {}\n", x); },
        1, 10, 2
    );
    /*
      - 1
      - 3
      - 5
      - 7
      - 9
    */
}
```

No vectors needed. No iterators, no ranges, nothing. Of course, you can wrap this functionality to get a vector if you want:

```cpp
void range(auto&& yield, int start, int stop, int step){
    for(int i = start; i < stop; i += step){
        yield(i);
    }
}

std::vector<int> range_vector(int start, int stop, int step){
    std::vector<int> result;
    result.reserve((stop - start) / step);
    range(
        [&](int x){ result.push_back(x); },
        start, stop, step
    );
    return result;
}
``` 

3. You can return different types, many times

Because the function doesn't exit on invocation of the callback, you can call it multiple times, and you don't
have to stick to the same type. This combines points 1 and 2, but is a powerful example to make.

You can construct an [extremely simple message parser](https://godbolt.org/z/hcnoToE6j) using this approach.
In this scenario, you receive a message that could correspond to zero, one, or more values, each of a different
type. You are able to parse the message item by item, "yielding" them out to a handler one by one.

The alternative might be along the lines of returning a `std::vector<std::variant<...>>`, or exposing message
parsing steps to the call site. Another alternative might be to use a coroutine that `co_yield`s a std::variant.


4. Lifetimes are easier to work with

Any temporaries that are created in the function will still be alive during invocation of the callback:

```
char const* get_line(FILE* handle){
    char line[256];
    fgets(line, 256, handle);
    return line; // oops
}

int main(){
    auto handle = fopen("my_file.txt", "r");
    char const* line = get_line(handle);
    fmt::print("{}\n", line);
}
```

However, with continuation passing, temporaries created in the function call are still alive during callback invocation:

```cpp
void get_line(auto&& callback, FILE* handle){
    char line[256];
    fgets(line, 256, handle);
    yield(line); // this is fine
}

int main(){
    auto handle = fopen("my_file.txt", "r");
    get_line(
        [](char const* line){ fmt::print("{}\n", line); },
        handle
    );
}
```
