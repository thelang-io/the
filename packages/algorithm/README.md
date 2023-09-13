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

### `qrcode.QRCode (input: Buffer) IQRCode`
Constructs `IQRCode` object.

**Parameters**

- `input` - Buffer to pass into IQRCode object

**Return value**

Constructed `IQRCode` object.

**Examples**

```the
mut qr := qrcode.QRCode("HELLO, WORLD!")
```

**Exceptions**

- `Base64Error` - throw if string to be decoded is not correctly encoded

### `qrcode.IQRCode.encode (ecc := ErrorCorrectionLevel.Q, mask: int? = nil) byte[][]`
Encodes `IQRCode` object and outputs array of bytes.

**Parameters**

- `ecc` - optional. Encoding error correction level
- `mask` - optional. Encoding mask, possible values are 0 - 7.

**Return value**

Array of bytes produced by encoding `IQRCode` object.

**Examples**

```the
mut qr := qrcode.QRCode("HELLO, WORLD!")
qr.encode(ecc: .M, mask: 3)
```

**Exceptions**

- `QRCodeError` - throw if QRCode input is too long for alignment pattern locations
- `QRCodeError` - throw if QRCode input is too long to calculate character count indicator
- `QRCodeError` - throw if QRCode input is too long to calculate version

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
