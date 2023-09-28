# the/parser
The programming language parser

## Installation

```bash
the install the/parser
```

## Example usage

```the
import parse from "the/parser"
```

## API

### `parse (pathOrCode: str) File`
Parses path into `File` object.

**Parameters**

- `pathOrCode` - path or code to parse

**Return value**

Parsed path as `File` object.

**Exceptions**

- `ParserError` - thrown if path doesn't exists
- `ParserError` - thrown if path is not a file

**Examples**

```the
f := parse("path/to/project")
```

### `report (file: ref File) void`
Reports errors to `stderr` for the `File` object.

**Parameters**

- `file` - reference to `File` object

**Return value**

none

**Examples**

```the
f := parse("path/to/project")
report(ref f)
```

### `Program_traverse (program: ref Program, cb: (it: Statement) -> void) void`
Deep iterate through all of the program's statements.

**Parameters**

- `program` - reference to `Program` object to iterate through
- `cb` - callback that should be called for each statement

**Return value**

none

**Examples**

```the
f := parse("path/to/project")

Program_traverse(ref f.program, (it: Statement) -> void {
  Statement_toText(it)
})
```

### `Expression_toText (self: Expression, indent := 0) str`
Generates string representation of the expression.

**Parameters**

- `self` - expression to generate string representation for
- `indent` - indentation for string representation

**Return value**

String representation of the expression.

**Examples**

```the
Expression_toText(expression)
Expression_toText(expression, indent: 2)
```

### `FunctionParameter_toText (self: FunctionParameter, indent := 0) str`
Generates string representation of the function parameter.

**Parameters**

- `self` - function parameter to generate string representation for
- `indent` - indentation for string representation

**Return value**

String representation of the function parameter.

**Examples**

```the
FunctionParameter_toText(param)
FunctionParameter_toText(param, indent: 2)
```

### `Identifier_toText (self: Identifier, indent := 0) str`
Generates string representation of the identifier.

**Parameters**

- `self` - identifier to generate string representation for
- `indent` - indentation for string representation

**Return value**

String representation of the identifier.

**Examples**

```the
Identifier_toText(id)
Identifier_toText(id, indent: 2)
```

### `Statement_toText (self: Statement, indent := 0) str`
Generates string representation of the statement.

**Parameters**

- `self` - statement to generate string representation for
- `indent` - indentation for string representation

**Return value**

String representation of the statement.

**Examples**

```the
Statement_toText(statement)
Statement_toText(statement, indent: 2)
```

### `Type_toText (self: Type, indent := 0) str`
Generates string representation of the type.

**Parameters**

- `self` - type to generate string representation for
- `indent` - indentation for string representation

**Return value**

String representation of the type.

**Examples**

```the
Type_toText(type)
Type_toText(type, indent: 2)
```

### `Expression.asIdentifier () Identifier`
### `Expression.asBooleanLiteral () BooleanLiteral`
### `Expression.asCharacterLiteral () CharacterLiteral`
### `Expression.asFloatingPointLiteral () FloatingPointLiteral`
### `Expression.asIntegerLiteral () IntegerLiteral`
### `Expression.asNilLiteral () NilLiteral`
### `Expression.asStringLiteral () StringLiteral`
### `Expression.asArray () ArrayExpression`
### `Expression.asAs () AsExpression`
### `Expression.asAssignment () AssignmentExpression`
### `Expression.asAwait () AwaitExpression`
### `Expression.asBinary () BinaryExpression`
### `Expression.asCall () CallExpression`
### `Expression.asClosure () ClosureExpression`
### `Expression.asConditional () ConditionalExpression`
### `Expression.asElementAccess () ElementAccessExpression`
### `Expression.asIs () IsExpression`
### `Expression.asMap () MapExpression`
### `Expression.asMemberAccess () MemberAccessExpression`
### `Expression.asObject () ObjectExpression`
### `Expression.asParenthesized () ParenthesizedExpression`
### `Expression.asPropertyAccess () PropertyAccessExpression`
### `Expression.asReference () ReferenceExpression`
### `Expression.asUnary () UnaryExpression`
These methods are shortcuts for `as` expression.

**Return value**

Returns body casted to corresponding body type.

**Examples**

```the
expression.asIdentifier()
```

### `Expression.isIdentifier () bool`
### `Expression.isBooleanLiteral () bool`
### `Expression.isCharacterLiteral () bool`
### `Expression.isFloatingPointLiteral () bool`
### `Expression.isIntegerLiteral () bool`
### `Expression.isNilLiteral () bool`
### `Expression.isStringLiteral () bool`
### `Expression.isArray () bool`
### `Expression.isAs () bool`
### `Expression.isAssignment () bool`
### `Expression.isAwait () bool`
### `Expression.isBinary () bool`
### `Expression.isCall () bool`
### `Expression.isClosure () bool`
### `Expression.isConditional () bool`
### `Expression.isElementAccess () bool`
### `Expression.isIs () bool`
### `Expression.isMap () bool`
### `Expression.isMemberAccess () bool`
### `Expression.isObject () bool`
### `Expression.isParenthesized () bool`
### `Expression.isPropertyAccess () bool`
### `Expression.isReference () bool`
### `Expression.isUnary () bool`
These methods are shortcuts for `is` expression.

**Return value**

Checks whether expression's body contains corresponding type.

**Examples**

```the
expression.isIdentifier()
```

### `Statement.asBlock () BlockStatement`
### `Statement.asBreak () BreakStatement`
### `Statement.asContinue () ContinueStatement`
### `Statement.asEmpty () EmptyStatement`
### `Statement.asEnumDeclaration () EnumDeclaration`
### `Statement.asEof () EofStatement`
### `Statement.asExportDeclaration () ExportDeclaration`
### `Statement.asExportNamedDeclaration () ExportNamedDeclaration`
### `Statement.asExpression () ExpressionStatement`
### `Statement.asFunctionDeclaration () FunctionDeclaration`
### `Statement.asIf () IfStatement`
### `Statement.asImportDeclaration () ImportDeclaration`
### `Statement.asLoop () LoopStatement`
### `Statement.asMainDeclaration () MainDeclaration`
### `Statement.asObjectDeclaration () ObjectDeclaration`
### `Statement.asObjectDeclarationMethod () ObjectDeclarationMethod`
### `Statement.asObjectDeclarationProperty () ObjectDeclarationProperty`
### `Statement.asReturn () ReturnStatement`
### `Statement.asThrow () ThrowStatement`
### `Statement.asTry () TryStatement`
### `Statement.asTypeAliasDeclaration () TypeAliasDeclaration`
### `Statement.asVariableDeclaration () VariableDeclaration`
These methods are shortcuts for `as` expression.

**Return value**

Returns body casted to corresponding body type.

**Examples**

```the
statement.asBlock()
```

### `Statement.isBlock () bool`
### `Statement.isBreak () bool`
### `Statement.isContinue () bool`
### `Statement.isEmpty () bool`
### `Statement.isEnumDeclaration () bool`
### `Statement.isEof () bool`
### `Statement.isExportDeclaration () bool`
### `Statement.isExportNamedDeclaration () bool`
### `Statement.isExpression () bool`
### `Statement.isFunctionDeclaration () bool`
### `Statement.isIf () bool`
### `Statement.isImportDeclaration () bool`
### `Statement.isLoop () bool`
### `Statement.isMainDeclaration () bool`
### `Statement.isObjectDeclaration () bool`
### `Statement.isObjectDeclarationMethod () bool`
### `Statement.isObjectDeclarationProperty () bool`
### `Statement.isReturn () bool`
### `Statement.isThrow () bool`
### `Statement.isTry () bool`
### `Statement.isTypeAliasDeclaration () bool`
### `Statement.isVariableDeclaration () bool`
These methods are shortcuts for `is` expression.

**Return value**

Checks whether statement's body contains corresponding type.

**Examples**

```the
statement.isBlock()
```

### `Type.asArray () ArrayType`
### `Type.asFunction () FunctionType`
### `Type.asIdentifier () IdentifierType`
### `Type.asMap () MapType`
### `Type.asMember () MemberType`
### `Type.asOptional () OptionalType`
### `Type.asParenthesized () ParenthesizedType`
### `Type.asReference () ReferenceType`
### `Type.asUnion () UnionType`
These methods are shortcuts for `as` expression.

**Return value**

Returns body casted to corresponding body type.

**Examples**

```the
type.asArray()
```

### `Type.isArray () bool`
### `Type.isFunction () bool`
### `Type.isIdentifier () bool`
### `Type.isMap () bool`
### `Type.isMember () bool`
### `Type.isOptional () bool`
### `Type.isParenthesized () bool`
### `Type.isReference () bool`
### `Type.isUnion () bool`
These methods are shortcuts for `is` expression.

**Return value**

Checks whether type's body contains corresponding type.

**Examples**

```the
type.isArray()
```
