# Contributing
We want to say "Welcome 👋!" if you are here at first and "We are glad to meet you again!" if you are already our contributor. \
We appreciate your help!

## Code of Conduct
All interactions for this project are covered by the [Contributor Code of Conduct](CODE_OF_CONDUCT.md). \
By participating in this project you agree to abide by its terms.

## If you just want to ask a question
- Ask it on our [forum][the-forum]
- Or talk directly to founder on [Discord][the-discord] or [Telegram][the-telegram]

## When opening an issue
- [Search][the-issues] for duplicates

## When sending a pull request
- [Search][the-prs] for duplicates
- Use the same coding style as the rest of the codebase

## Testing

```shell
cmake . -D BUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

## Memory Checking

```shell
cmake . -D BUILD_TESTS=ON -D TEST_CODEGEN_MEMCHECK=ON
cmake --build .
ctest --output-on-failure
```

## Coverage

```shell
cmake . -D BUILD_COVERAGE=ON -D BUILD_TESTS=ON
cmake --build .
gcov src/*
```

[the-discord]: https://discord.gg/CTMM583G7V
[the-forum]: https://forum.thelang.io/
[the-issues]: https://github.com/thelang-io/docs.thelang.io/issues?q=is%3Aissue
[the-prs]: https://github.com/thelang-io/docs.thelang.io/pulls?q=is%3Apr
[the-telegram]: https://t.me/thelang_io
