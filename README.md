# narrow()
Safely convert between various arithmetic types:

```c++
int   x = narrow<int>(1234567890llu); // ok
int   x = narrow<int>(12345.0);       // ok
float x = narrow<float>(-16'000'000); // ok

char  x = narrow<char>(128);          // throws
int   x = narrow<int>(3.14f);         // throws
float x = narrow<float>(0.1);         // throws
```

`narrow<To>(From x)` can be used to safely convert the value `x` from its source type `From` to the destination type `To`. If `x` can be preciesly represented as a `To`, `static_cast<To>(x)` is returned, otherwise the function throws.
