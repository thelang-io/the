# How To Install
Updated: Jan 28, 2025

## CLI
First you need CLI to orchestrate the compiler.

For Unix:
```sh
curl -fsSL sh.thelang.io | bash
```

In case you are curious what it does,
you can check here: https://github.com/thelang-io/cli/blob/main/install.sh

For Windows:
```powershell
irm ps1.thelang.io | iex
```

In case you are curious what it does,
you can check here: https://github.com/thelang-io/cli/blob/main/install.ps1

## Test CLI Installation

```sh
the -v
```

If that doesn't work you probably need to re-login into your terminal.

## Before Downloading Compiler
Before, everything was compiled on cloud servers.
After Oct 2024, I disabled cloud servers (I paid $600 for the last year and nobody used it).
If you think I should enabled it back, let me know. But for now you will need an offline compiler.

Before you will get a compiler you will need to have CMake installed: https://cmake.org/download/

Next thing you need to run this command:
```sh
the offline
```

This should be it!

## Test Installation
Create a file, eg "main":

```the
main {
  print("Hello, World")
}
```

And run it like this:

```sh
the run main
```

> [!important]
> Under the hood CLI will recognize that you have offline compiler installed and will use it.

## Further Reading
Remember, this is old version that is built using C++.
This version is slow, and I'm aware of that.
The new version, where it's much faster and self-compiles, is still in development.
Expected to be released somewhere in 2025.

1. [Comments](https://docs.thelang.io/guides/comments.html)
2. [Operations](https://docs.thelang.io/guides/operations.html)
3. [Types](https://docs.thelang.io/guides/types.html)
4. [Mutability](https://docs.thelang.io/guides/mutability.html)
5. [Control Flow](https://docs.thelang.io/guides/control-flow.html)
6. [Iteration](https://docs.thelang.io/guides/iteration.html)
7. Types: [Arrays](https://docs.thelang.io/guides/arrays.html) /
   [Enumerations](https://docs.thelang.io/guides/enumerations.html) /
   [Objects](https://docs.thelang.io/guides/objects.html) /
   [Maps](https://docs.thelang.io/guides/maps.html) /
   [Unions](https://docs.thelang.io/guides/unions.html) /
   [Functions](https://docs.thelang.io/guides/functions.html) /
   [Optionals](https://docs.thelang.io/guides/optionals.html) /
   [References](https://docs.thelang.io/guides/references.html) /
   [Any](https://docs.thelang.io/guides/any.html)
8. [Anonymous Functions (Closures)](https://docs.thelang.io/guides/anonymous-functions.html)
9. [Variadic Parameters](https://docs.thelang.io/guides/variadics.html)
10. [Type Aliasing](https://docs.thelang.io/guides/type-aliasing.html)
11. [Type Checking](https://docs.thelang.io/guides/type-checking.html)
12. [Type Casting](https://docs.thelang.io/guides/type-casting.html)
13. [Error Handling](https://docs.thelang.io/guides/error-handling.html)
14. [Asynchronous Programming](https://docs.thelang.io/guides/asynchronous-programming.html)
15. [Modules](https://docs.thelang.io/guides/modules.html)
