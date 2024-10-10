# How To Install
Updated: Oct 10, 2024

> [!important]
> I tested it only on macOS. Windows commands is what I remember of how it should work.
> If you have suggestions feel free to tell me on Discord or create a PR.

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

If that doesn't work, try this one:
```powershell
(New-Object System.Net.WebClient).DownloadString('https://ps1.thelang.io/') | iex
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
To compile you will need a compiler now :)

Before you will get a compiler you will need to have CMake installed: https://cmake.org/download/

Next thing you need to download prebuilt dev version of OpenSSL
from: https://cdn.thelang.io/deps.tar.gz and unpack the file.
It will contain folder "native" with your platform inside of it.
This is needed by compiler itself when you are compiling programs that require SSL support.

After that you need to create system environment variable or add it to your `~/.profile`:
```
DEPS_DIR="path/to/native/$platform"
```

## Building Compiler
This is how to build a compiler from source on Unix:
```sh
git clone --depth=1 -b feat-new-codegen --single-branch https://github.com/thelang-io/the.git
cmake the -B the/build -D CMAKE_BUILD_TYPE=Release
cmake --build the/build
cp the/build/the ~/.the/bin/thex
rm -rf the
```

And this is how to build a compiler from source on Windows:
```powershell
git clone --depth=1 -b feat-new-codegen --single-branch https://github.com/thelang-io/the.git
cmake the -B the/build -D CMAKE_BUILD_TYPE=Release
cmake --build the/build
Copy-Item "the/build/Debug/the.exe" -Destination "$env:UserProfile/The/bin/thex.exe"
Remove-Item -Recurse -Force the
```

That should be it!

## Test Installation
Create a file, eg "main.the":

```the
main {
  print("Hello, World")
}
```

And run it like this:

```sh
the run main.the --compiler=thex
```

> [!important]
> If you don't add `--compiler=thex` it will try to run on cloud server, and you will get an error.

## Further Reading
Remember, this is old version that is built using C++.
This version is slow, and I'm aware of that.
The new version, where it's much faster and self-compiles, is still in development.
Expected to be released by the end of 2024.

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
