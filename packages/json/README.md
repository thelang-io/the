# the/json
JSON parser

## Installation

```bash
the install the/json
```

## Example usage

```the
import * as json from "the/exec"
```

## API

### `parse (code: str) Value[]`
Parses JSON string into list of values. Supports `jsonl` format.

**Parameters**

- `code` - JSON string to parse

**Return value**

Array of constructed `Value` objects.

**Examples**

```the
parse("[1, true, null]")
parse("[1] [true] [null]")
```

### `Value.asArray () ArrayValue`
### `Value.asLiteral () LiteralValue`
### `Value.asNumber () NumberValue`
### `Value.asObject () ObjectValue`
### `Value.asString () StringValue`
These methods are shortcuts for `as` expression.

**Return value**

Returns value casted to corresponding body type.

**Examples**

```the
values := parse("[1, true, null]")
value := values[0].asArray()
```

### `Value.isArray () bool`
### `Value.isLiteral () bool`
### `Value.isNumber () bool`
### `Value.isObject () bool`
### `Value.isString () bool`
These methods are shortcuts for `is` expression.

**Return value**

Checks whether value's body contains corresponding type.

**Examples**

```the
values := parse("[1, true, null]")
values[0].isArray()
```
