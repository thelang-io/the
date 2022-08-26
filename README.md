# The Programming Language

## Testing
```sh
$ cmake . -D BUILD_TESTS=ON
$ cmake --build .
$ ctest --output-on-failure
```

## Memory Checking
```sh
$ cmake . -D BUILD_TESTS=ON -D TEST_CODEGEN_MEMCHECK=ON
$ cmake --build .
$ ctest --output-on-failure
```

## Coverage
```sh
$ cmake . -D BUILD_COVERAGE=ON -D BUILD_TESTS=ON
$ cmake --build .
$ gcov src/*
```
