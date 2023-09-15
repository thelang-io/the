# the/testing
The programming language test framework

## Installation

```bash
the install the/testing
```

## Example usage

```the
import Tests from "the/testing"
```

> For using `the test` is enough to just `import "the/testing"` in every file where you use `the/testing` features.
> To test your own project you don't need to use anything from API explained below.

## API

### `Tests.register (name: str, body: () -> void) ref Self`
Registers test to be run.

**Parameters**

- `name` - test name
- `body` - test body (a function that runs when all tests run)

**Return value**

Reference to self.

**Examples**

```the
mut tests := Tests{}
tests.register("stringify", TEST_stringify)
```

### `Tests.run (args: str[]) ref Self`
Runs registered tests.

**Parameters**

- `args` - arguments that should be parsed by `Tests` object, usually `process.args`

**Return value**

Reference to self.

**Examples**

```the
mut tests := Tests{}
tests.register("stringify", TEST_stringify)
tests.run(process.args)
```
