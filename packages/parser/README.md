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

### `interconnect () void`
Populates parent, next, prev properties with references.

**Return value**

none

**Examples**

```the
mut f := parse("./path")
interconnect(ref f)
```

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

### `Expression.asIdentifier () ref Identifier`
### `Expression.asBooleanLiteral () ref BooleanLiteral`
### `Expression.asCharacterLiteral () ref CharacterLiteral`
### `Expression.asFloatingPointLiteral () ref FloatingPointLiteral`
### `Expression.asIntegerLiteral () ref IntegerLiteral`
### `Expression.asNilLiteral () ref NilLiteral`
### `Expression.asStringLiteral () ref StringLiteral`
### `Expression.asArray () ref ArrayExpression`
### `Expression.asAs () ref AsExpression`
### `Expression.asAssignment () ref AssignmentExpression`
### `Expression.asAwait () ref AwaitExpression`
### `Expression.asBinary () ref BinaryExpression`
### `Expression.asCall () ref CallExpression`
### `Expression.asClosure () ref ClosureExpression`
### `Expression.asConditional () ref ConditionalExpression`
### `Expression.asElementAccess () ref ElementAccessExpression`
### `Expression.asIs () ref IsExpression`
### `Expression.asMap () ref MapExpression`
### `Expression.asMemberAccess () ref MemberAccessExpression`
### `Expression.asObject () ref ObjectExpression`
### `Expression.asParenthesized () ref ParenthesizedExpression`
### `Expression.asPropertyAccess () ref PropertyAccessExpression`
### `Expression.asReference () ref ReferenceExpression`
### `Expression.asUnary () ref UnaryExpression`
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

### `Expression.isAccess () bool`
Checks whether expression is `Identifier`, `ElementAccess` or `PropertyAccess`.

**Return value**

Whether expression is `Identifier`, `ElementAccess` or `PropertyAccess`.

**Examples**

```the
expression.isAccess()
```

### `Statement.asBlock () ref BlockStatement`
### `Statement.asBreak () ref BreakStatement`
### `Statement.asContinue () ref ContinueStatement`
### `Statement.asEmpty () ref EmptyStatement`
### `Statement.asEnumDeclaration () ref EnumDeclaration`
### `Statement.asEof () ref EofStatement`
### `Statement.asExportDeclaration () ref ExportDeclaration`
### `Statement.asExportNamedDeclaration () ref ExportNamedDeclaration`
### `Statement.asExpression () ref ExpressionStatement`
### `Statement.asFunctionDeclaration () ref FunctionDeclaration`
### `Statement.asIf () ref IfStatement`
### `Statement.asImportDeclaration () ref ImportDeclaration`
### `Statement.asLoop () ref LoopStatement`
### `Statement.asMainDeclaration () ref MainDeclaration`
### `Statement.asObjectDeclaration () ref ObjectDeclaration`
### `Statement.asObjectDeclarationMethod () ref ObjectDeclarationMethod`
### `Statement.asObjectDeclarationProperty () ref ObjectDeclarationProperty`
### `Statement.asReturn () ref ReturnStatement`
### `Statement.asThrow () ref ThrowStatement`
### `Statement.asTry () ref TryStatement`
### `Statement.asTypeAliasDeclaration () ref TypeAliasDeclaration`
### `Statement.asVariableDeclaration () ref VariableDeclaration`
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

### `Statement.hasNext () bool`
Check if statement has next sibling. Make sure to `interconnect` references.

**Return value**

Whether statement has next sibling.

**Examples**

```the
statement.hasNext()
```

### `Statement.hasParent () bool`
Check if statement has parent. Make sure to `interconnect` references.

**Return value**

Whether statement has parent.

**Examples**

```the
statement.hasParent()
```

### `Statement.hasPrev () bool`
Check if statement has previous sibling. Make sure to `interconnect` references.

**Return value**

Whether statement has previous sibling.

**Examples**

```the
statement.hasPrev()
```

### `Statement.next () ref Statement`
Return statement's next sibling. Make sure to `interconnect` references.

**Return value**

Statement's next sibling.

**Examples**

```the
statement.next()
```

**Exceptions**

- `ParserError` - thrown if tried accessing nil next

### `Statement.parent () ref Statement`
Return statement's parent. Make sure to `interconnect` references.

**Return value**

Statement's parent.

**Examples**

```the
statement.next()
```

**Exceptions**

- `ParserError` - thrown if tried accessing nil parent

### `Statement.prev () ref Statement`
Return statement's previous sibling. Make sure to `interconnect` references.

**Return value**

Statement's previous sibling.

**Examples**

```the
statement.prev()
```

**Exceptions**

- `ParserError` - thrown if tried accessing nil prev

### `Type.asArray () ref ArrayType`
### `Type.asFunction () ref FunctionType`
### `Type.asIdentifier () ref IdentifierType`
### `Type.asMap () ref MapType`
### `Type.asMember () ref MemberType`
### `Type.asOptional () ref OptionalType`
### `Type.asParenthesized () ref ParenthesizedType`
### `Type.asReference () ref ReferenceType`
### `Type.asUnion () ref UnionType`
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
