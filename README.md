# The Programming Language

## Running
```sh
$ cmake . -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
$ ./the program
```

## Testing
```sh
$ cmake . -DCMAKE_BUILD_TYPE=Debug
$ cmake --build .
$ ctest --output-on-failure
```

## Memory Checking
```sh
$ cmake . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCODEGEN_MEMCHECK=ON
$ cmake --build .
$ ctest --output-on-failure
```

## Coverage
```sh
$ cmake . -DCMAKE_BUILD_TYPE=Coverage -DBUILD_TESTS -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping"
$ cmake --build .
$ gcov src/*
```
