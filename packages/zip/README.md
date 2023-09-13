# the/zip
Zip Utilities

## Installation

```bash
the install the/zip
```

## Example usage

```the
import zip, unzip from "the/zip"
```

## API

### `zip.zip (name: str, path: str, cwd: str? = nil) void`
Thin wrapper around `zip` system call.

**Parameters**

- `name` - name of archive to create
- `path` - path to files to archive
- `cwd` - optional. Working directory for `zip` system call

**Return value**

none

**Exceptions**

- `ZipError` - throw if path doesn't exists

**Examples**

```the
zip.zip("archive.zip", "file.txt")
zip.zip("archive.zip", "*", cwd: ".")
```

### `zip.unzip (source: str, destination: str) void`
Thin wrapper around `unzip` system call.

**Parameters**

- `source` - source location of ZIP file
- `destination` - destination location to where to unzip archive

**Return value**

none

**Exceptions**

- `ZipError` - throw if path doesn't exists

**Examples**

```the
zip.unzip("archive.zip", ".")
```
