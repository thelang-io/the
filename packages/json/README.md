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
This method is a shortcut for `as` expression.

**Return value**

Returns value casted to `ArrayValue` object.

**Examples**

```the
values := parse("[1, true, null]")
value := values[0].asArray()
```

### `Value.asLiteral () LiteralValue`
This method is a shortcut for `as` expression.

**Return value**

Returns value casted to `LiteralValue` object.

**Examples**

```the
values := parse("true")
value := values[0].asLiteral()
```

### `Value.asNumber () NumberValue`
This method is a shortcut for `as` expression.

**Return value**

Returns value casted to `NumberValue` object.

**Examples**

```the
values := parse("10")
value := values[0].asNumber()
```

### `Value.asObject () ObjectValue`
This method is a shortcut for `as` expression.

**Return value**

Returns value casted to `ObjectValue` object.

**Examples**

```the
values := parse("{ \"key\": 10 }")
value := values[0].asObject()
```

### `Value.asString () StringValue`
This method is a shortcut for `as` expression.

**Return value**

Returns value casted to `StringValue` object.

**Examples**

```the
values := parse("\"HELLO WOLRD\"")
value := values[0].asString()
```

### `Value.isArray () bool`
This method is a shortcut for `is` expression.

**Return value**

Checke whether value body contains `ArrayValue`.

**Examples**

```the
values := parse("[1, true, null]")
values[0].isArray()
```

### `Value.isLiteral () bool`
This method is a shortcut for `is` expression.

**Return value**

Checke whether value body contains `LiteralValue`.

**Examples**

```the
values := parse("true")
values[0].isLiteral()
```

### `Value.isNumber () bool`
This method is a shortcut for `is` expression.

**Return value**

Checke whether value body contains `NumberValue`.

**Examples**

```the
values := parse("10")
values[0].isNumber()
```

### `Value.isObject () bool`
This method is a shortcut for `is` expression.

**Return value**

Checke whether value body contains `ObjectValue`.

**Examples**

```the
values := parse("{ \"key\": 10 }")
values[0].isObject()
```

### `Value.isString () bool`
This method is a shortcut for `is` expression.

**Return value**

Checke whether value body contains `StringValue`.

**Examples**

```the
values := parse("\"HELLO WORLD\"")
values[0].isString()
```
