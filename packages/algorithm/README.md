# the/algorithm
Algorithms Library

## Installation

```bash
the install the/algorithm
```

## Example usage

```the
import base64 from "the/algorithm"
```

## API

### `base64.encode (data: Buffer) str`
Encodes buffer with base64 algorithm.

**Parameters**

- `data` - buffer to encode

**Return value**

A string in ASCII format containing base64 representation of the `data`.

**Examples**

```the
base64.encode("Aaron".toBuffer())
```

### `base64.decode (data: str) Buffer`
Decodes string with base64 algorithm.

**Parameters**

- `data` - string to decode

**Return value**

A string in ASCII format containing decoded data from `data`.

**Examples**

```the
base64.decode("QWFyb24=")
```

### `byteArray.fromStr (in: str) byte[]`
Converts a string into a byte array.

**Parameters**

- `in` - string to convert

**Return value**

A byte array representing input string.

**Examples**

```the
byteArray.fromStr("Aaron")
```

### `byteArray.toHexStr (in: byte[]) str`
Converts a byte array into a hex string.

**Parameters**

- `in` - byte array to convert

**Return value**

A hex string representing input byte array.

**Examples**

```the
byteArray.toHexStr([0x41, 0x61, 0x72, 0x6F, 0x6E])
```

### `byteArray.toStr (in: byte[]) str`
Converts a byte array into a string.

**Parameters**

- `in` - byte array to convert

**Return value**

A string representing input byte array.

**Examples**

```the
byteArray.toStr([0x41, 0x61, 0x72, 0x6F, 0x6E])
```

### `qrcode.QRCode (input: Buffer) IQRCode`
Constructs `IQRCode` object.

**Parameters**

- `input` - Buffer to pass into IQRCode object

**Return value**

Constructed `IQRCode` object.

**Exceptions**

- `QRCodeError` - thrown if string to be decoded is not correctly encoded

**Examples**

```the
mut qr := qrcode.QRCode("HELLO, WORLD!")
```

### `qrcode.IQRCode.encode (ecc := ErrorCorrectionLevel.Q, mask: int? = nil) byte[][]`
Encodes `IQRCode` object and outputs array of bytes.

**Parameters**

- `ecc` - optional. Encoding error correction level
- `mask` - optional. Encoding mask, possible values are 0 - 7.

**Return value**

Array of bytes produced by encoding `IQRCode` object.

**Exceptions**

- `QRCodeError` - thrown if QRCode input is too long for alignment pattern locations
- `QRCodeError` - thrown if QRCode input is too long to calculate character count indicator
- `QRCodeError` - thrown if QRCode input is too long to calculate version

**Examples**

```the
mut qr := qrcode.QRCode("HELLO, WORLD!")
qr.encode(ecc: .M, mask: 3)
```

### `qrcode.toTerminal (qrcode: IQRCode) void`
Prints encoded `IQRCode` to terminal.

**Parameters**

- `qrcode` - object of type `IQRCode`

**Return value**

none

**Examples**

```the
mut qr := qrcode.QRCode("HELLO, WORLD!")
qr.encode(ecc: .M, mask: 3)
qrcode.toTerminal(qr)
```

### `md5 (data: byte[]) byte[]`
Encodes byte array with md5 algorithm.

**Parameters**

- `data` - byte array to encode

**Return value**

A byte array containing md5 representation of the `data`.

**Examples**

```the
import byteArray, md5 from "the/algorithm"
result := md5(byteArray.fromStr("Aaron"))
```
