# the/args
Command line arguments parser and tools

## Installation

```bash
the install the/args
```

## Example usage

```the
import Args from "the/args"
```

## API

### `Args{lineLength: int}`
Constructs `Args` object.

**Fields**

- `lineLength` - maximum line length of the output produced by `help` command

**Examples**

```the
Args{lineLength: 70}
```

### `Args.command (name: str, description: str, alias := "", default := "", hasValue := true) ref Self`
Adds command to the `Args` object. Command is something that is usually passed as first argument to your program.

**Parameters**

- `name` - Name of the command
- `description` - Description of the command
- `alias` - Command alias
- `default` - Default value of the command
- `hasValue` - Whether or not command has a value

**Return value**

Return a reference to self.

**Examples**

```the
Args{}.command("run", "Description of the command")
Args{}.command("run", "Description of the command", alias: "r", default: "value")
Args{}.command("run", "Description of the command", hasValue: false)
```

### `Args.example (text: str) ref Self`
Adds example to the `Args` object.

**Parameters**

- `text` - Text content of the example

**Return value**

Return a reference to self.

**Examples**

```the
Args{}.example("program run")
```

### `Args.help () str`
Returns help information about `Args` object.

**Return value**

Help information about `Args` object.

**Examples**

```the
args := Args{}
  .command("run", "Description of the command")
  .example("program run")

args.help()
```

### `Args.option (name: str, description: str, alias := "", default := "", hasValue := true) ref Self`
Adds option to the `Args` object. Options is something that is passed with a prefix of double dashes (`--`).

**Parameters**

- `name` - Name of the option
- `description` - Description of the option
- `alias` - Option alias
- `default` - Default value of the option
- `hasValue` - Whether or not option has a value

**Return value**

Return a reference to self.

**Examples**

```the
Args{}.option("output", "Output location")
Args{}.option("output", "Output location", alias: "o", default: "a.txt")
Args{}.option("verbose", "Whether output should be verbose", alias: "v", hasValue: false)
```

### `Args.parse (args: str[]) ref Self`
Parses arguments into `data` field.

**Parameters**

- `args` - Arguments to be parsed, usually `process.args`

**Return value**

Return a reference to self.

**Exceptions**

- `ArgsError` - thrown on unexpected argument

**Examples**

```the
mut args := Args{}
args.parse(process.args)

loop i := 0; i < args.data.len; i++ {
  item := args.data[i]
}
```

### `Args.usage (text: str) ref Self`
Adds usage instruction to the `Args` object.

**Parameters**

- `text` - Text content of the usage

**Return value**

Return a reference to self.

**Examples**

```the
Args{}.usage("program run [file]")
```
