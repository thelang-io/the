# the/html
HTML parser

## Installation

```bash
the install the/html
```

## Example usage

```the
import parse from "the/html"
```

### `parse (code: str) HTMLNode[]`
Parses code into `HTMLNode` array.

**Parameters**

- `code` - code to parse

**Return value**

Parsed code as `HTMLNode` array.

**Examples**

```the
nodes := parse("<html></html>")
```

### `toJSON (nodes: ref HTMLNode[]) str`
Convert `HTMLNode` array to JSON string.

**Parameters**

- `nodes` - nodes to convert

**Return value**

`HTMLNode` array converted to JSON string.

**Examples**

```the
nodes := parse("<html></html>")
json := toJSON(ref nodes)
```

### `HTMLAttribute`
`HTMLElement` attribute.

**Properties**

- `name` - attribute name
- `value` - attribute value (if present)

### `HTMLNode`
Union of all possible nodes.

### `HTMLComment`
Textual notations within HTML markup.

### `HTMLDocumentType`
Node containing document type (preamble of every HTML page).

**Properties**

- `name` - type of document. Always "html" for HTML documents.

### `HTMLElement`
Represents any HTML element.

**Properties**

- `attributes` - array of element's attributes
- `childNodes` - array of element's child nodes
- `tagName` - tag name for the given element
- `textContent` - textual content in case child nodes are not present. Available only for script, style, textarea, title tags.

### `HTMLText`
Text node.

**Properties**

- `nodeValue` - value of the current node
