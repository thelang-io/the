# the/exec
System commands wrapper

## Installation

```bash
the install the/exec
```

## Example usage

```the
import exec from "the/exec"
```

## API

### `exec (cmd: str) Exec`
Constructs `Exec` object.

**Parameters**

- `cmd` - command to create `Exec` object for

**Return value**

Constructed `Exec` object.

**Exceptions**

- `ExecError` - thrown if `cmd` is empty

**Examples**

```the
exec("echo test")
```

### `Exec.and (other: str) ref Self`
Adds `&&` clause to the command.

**Parameters**

- `other` - another command to add to `Exec` object

**Return value**

Return a reference to self.

**Examples**

```the
exec("echo HELLO").and("echo WORLD")
```

### `Exec.or (other: str) ref Self`
Adds `||` clause to the command.

**Parameters**

- `other` - another command to add to `Exec` object

**Return value**

Return a reference to self.

**Examples**

```the
exec("cd directory").or("echo 'Failed to change directory'")
```

### `Exec.pipe (other: str) ref Self`
Adds `|` clause to the command.

**Parameters**

- `other` - another command to add to `Exec` object

**Return value**

Return a reference to self.

**Examples**

```the
exec("echo HELLO").pipe("grep L")
```

### `Exec.run () ExecOutput`
Executes a command via a system call.

**Return value**

Returns `ExecOutput` object with `code`, `stderr` and `stdout` properties set.

**Examples**

```the
output := exec("echo HELLO").and("echo WORLD").run()
```
