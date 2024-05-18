# Build

```shell
cmake --build Build && (cd Build && ctest .)
```

# Convention

## Naming

## Structure

- namespace `xxx_details`

```c++
class C {
  // private members
public:
  // public members
};
```
