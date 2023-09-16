# the/yaml
YAML parser

## Installation

```bash
the install the/yaml
```

## Example usage

```the
import * as yaml from "the/yaml"
```

## API

### `parse (content: str) YAML`
Constructs `YAML` object with content.

**Parameters**

- `content` - content that needs to be passed to constructed `YAML` object

**Return value**

Constructed `YAML` object.

**Examples**

```the
parse("key: value")
```

### `YAML.hasNext () bool`
Checks whether `YAML` object will be able to read next node.

**Return value**

Result of checking whether `YAML` object will be able to read next node.

**Examples**

```the
mut yaml := parse("key: value")

loop yaml.hasNext() {
  //
}
```

### `YAML.next () Node`
Parses next node from `YAML` object.

**Return value**

Next parsed node from `YAML` object.

**Examples**

```the
mut yaml := parse("key: value")

loop yaml.hasNext() {
  node := yaml.next()
}
```

### `YAML.stringify () str`
Returns stringified representation of the parsed content.

**Return value**

Stringified representation of the parsed content.

**Examples**

```the
mut yaml := parse("key: value")
yaml.stringify()
```

### `Node_stringify () str`
Returns stringified representation of the `Node` object.

**Return value**

Stringified representation of the `Node` object.

**Examples**

```the
mut file := yaml.parse("key: value")
yaml.Node_stringify(file.next())
```
