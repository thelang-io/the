# The Programming Language

## Running
```sh
$ cmake . -D CMAKE_BUILD_TYPE=Release
$ cmake --build .
$ ./the program
```

## Testing
```sh
$ cmake . -D CMAKE_BUILD_TYPE=Debug
$ cmake --build .
$ ctest --output-on-failure
```

## Memory Checking
```sh
$ cmake . -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTS=ON -D TEST_CODEGEN_MEMCHECK=ON
$ cmake --build .
$ ctest --output-on-failure
```

## Coverage
```sh
$ cmake . -D CMAKE_BUILD_TYPE=Coverage -D BUILD_TESTS -D CMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping"
$ cmake --build .
$ gcov src/*
```
