# @thelang-io/the

## Running

```the
$ cmake . -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
$ ./the-core program
```

## Testing

```the
$ cmake . -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .
$ ctest --output-on-failure
```

## Memory Checking

```the
$ cmake . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCODEGEN_MEMCHECK=ON
$ cmake --build .
$ ctest --output-on-failure
```

## Coverage

```bash
$ cmake . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping"
$ cmake --build .
$ gcov src/*
```
