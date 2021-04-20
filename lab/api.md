# Builtin
```the
const Program program
const str __dirname
const int __filecol
const int __fileline
const str __filename

class Array {
  pub bool empty
  pub int len

  pub mut str () clear
  pub mut str () reverse
  pub str () str
}

class Char {
  pub str () str
}

class Map<T, U> {
  pub str () str
}

class NaN {
  pub str () str
}

class Infinity

class Number {
  pub str () str
}

class Object {
  pub str () str
}

class Program {
  int argc
  str[] argv
  str argv0
}

class Range<T> {
  pub str () str
}

class String {
  pub bool empty

  pub at (int) char
  pub empty () bool
  pub toStr () str
  len prop
  to lower
  to upper
  ucfirst
  to title
  to kebab
  to snake
}

fn print (any... items, str separator = ' ', str terminator = '\n') void
```

# Clock
```the
export obj Timer {}

export fn clearTimer (Timer timer) void
export fn setInterval (fn () void cb, int delay) Timer
export fn setNextTick (fn () void cb) void
export fn setTimeout (fn () void cb, int delay) Timer
export fn time () u64
export fn hrtime () u64
```

# Date
https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/
Global_Objects/Date

# Error
```the
error class
  code

logic_error - exception class to indicate violations of logical preconditions
  or class invariants
invalid_argument - exception class to report invalid arguments
domain_error - exception class to report domain errors
length_error - exception class to report attempts to exceed maximum allowed
  size
out_of_range - exception class to report arguments outside of expected range
runtime_error - exception class to indicate conditions only detectable at run
  time
range_error - exception class to report range errors in internal computations
overflow_error - exception class to report arithmetic overflows
underflow_error - exception class to report arithmetic underflows

enum ErrorNo {
  E0001
  E0002
}

error number to string
```
https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/
Global_Objects/Error

# FileSystem
```the
access (path[, mode])
appendFile (path, data[, options])
chdir (str dir) void
chmod (path, mode)
chown (path, uid, gid)
copyFile (src, dest[, mode])
cwd () str
lchmod (path, mode)
lchown (path, uid, gid)
lutimes (path, atime, mtime)
link (existingPath, newPath)
lstat (path[, options])
mkdir (path[, options])
mkdtemp (prefix[, options])
open (path, flags[, mode])
opendir (path[, options])
readdir (path[, options])
readFile (path[, options])
readlink (path[, options])
realpath (path[, options])
rename (oldPath, newPath)
remove (path[, options])
removeDir (path[, options])
stat (path[, options])
unlink (path)
writeFile (filepath, data[, options])

os.homedir()
os.tmpdir()

path.basename(path[, ext])
path.delimiter
path.dirname(path)
path.extname(path)
path.format(pathObject)
path.isAbsolute(path)
path.join([...paths])
path.normalize(path)
path.parse(path)
path.posix
path.relative(from, to)
path.resolve([...paths])
path.sep
path.toNamespacedPath(path)
path.win32

class Path
filesystem_error
```

# IO
```the
stderr
stdin
stdout
```

# JSON
https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/
Global_Objects/JSON

# Limits
```the
export const f32 MAX_F32
export const f32 MIN_F32
export const f64 MAX_F64
export const f64 MIN_F64

export const int MAX_INT
export const int MIN_INT

export const i8 MAX_I8
export const i8 MIN_I8
export const i16 MAX_I16
export const i16 MIN_I16
export const i32 MAX_I32
export const i32 MIN_I32
export const i64 MAX_I64
export const i64 MIN_I64

export const u8 MAX_U8
export const u8 MIN_U8
export const u16 MAX_U16
export const u16 MIN_U16
export const u32 MAX_U32
export const u32 MIN_U32
export const u64 MAX_U64
export const u64 MIN_U64
```

# Math
```the
export const E := 2.71828182845904523536028747135266249775724709369995957496696763
export const PI := 3.14159265358979323846264338327950288419716939937510582097494459
export const PHI := 1.61803398874989484820458683436563811772030917980576286213544862

export const SQRT2 := 1.41421356237309504880168872420969807856967187537694807317667974
export const SQRTE := 1.64872127070012814684865078781416357165377610071014801157507931
export const SQRTPI := 1.77245385090551602729816748334114518279754945612238712821380779
export const SQRTPHI := 1.27201964951406896425242246173749149171560804184009624861664038

export const LN2 := 0.693147180559945309417232121458176568075500134360255254120680009
export const LOG2E := 1 / LN2
export const LN10 := 2.30258509299404568401799145468436420760110148862877297603332790
export const LOG10E := 1 / LN10

const (
	MaxFloat32             = 3.40282346638528859811704183484516925440e+38  // 2**127 * (2**24 - 1) / 2**23
	SmallestNonzeroFloat32 = 1.401298464324817070923729583289916131280e-45 // 1 / 2**(127 - 1 + 23)

	MaxFloat64             = 1.797693134862315708145274237317043567981e+308 // 2**1023 * (2**53 - 1) / 2**52
	SmallestNonzeroFloat64 = 4.940656458412465441765687928682213723651e-324 // 1 / 2**(1023 - 1 + 52)
)

const (
	MaxInt8   = 1<<7 - 1
	MinInt8   = -1 << 7
	MaxInt16  = 1<<15 - 1
	MinInt16  = -1 << 15
	MaxInt32  = 1<<31 - 1
	MinInt32  = -1 << 31
	MaxInt64  = 1<<63 - 1
	MinInt64  = -1 << 63
	MaxUint8  = 1<<8 - 1
	MaxUint16 = 1<<16 - 1
	MaxUint32 = 1<<32 - 1
	MaxUint64 = 1<<64 - 1
)

export fn abs (int x) int
export fn cbrt (int x) float
export fn ceil (float x) int
export fn exp (int x) float
export fn floor (float x) int
export fn log (int x) float
export fn log2 (int x) float
export fn log10 (int x) float
export fn max (int x, int y) int
export fn min (int x, int y) int
export fn pow (int x, int y) int
export fn random () int
export fn round (float x) int
export fn sqrt (int x) float
```

# RegEx
```the
```

# Socket
```the
tcp\udp
```
https://nodejs.org/api/http.html#http_http_get_url_options_callback
https://nodejs.org/api/dgram.html

# System
```the
enum Arch {
  arm32,
  arm64,
  x32,
  x64
}

obj CPU {
  str model
  int speed
}

obj ChildProcess {
  str stderr
  str stdout
}

obj ChildProcessExecOptions {
  str? cwd
  Map<str, str>? env
  int timeout := 0
}

obj ChildProcessSpawnOptions {
  str? cwd
  bool detached := false
  Map<str, str>? env
  int timeout := 0
}

enum OperatingSystem {
  linux,
  macos,
  windows
}

enum Signal {
  abrt,
  fpe,
  ill,
  int,
  segv,
  term
}

export const Arch ARCH
export const CPU[] CPUS
export const str ENDIANNESS
export const Map<str, str> ENV
export const str EOL
export const OperatingSystem OS
export const str OS_VERSION
export const int PID
export const str VERSION
export const str VERSION_NAME

export fn exec (str cmd, ChildProcessExecOptions options) async ChildProcess
export fn exit (int code) void
export fn kill (int pid, int code) void
export fn signal (Signal sig, fn (int) void cb) void
export fn spawn (str cmd, ChildProcessSpawnOptions options) async ChildProcess

os.hostname()
```

# Util
```the
swap
```

# Window
```the
> create
> destroy
> get/set size
> get frame size
> get/set position
> get/set title
> minimize/restore
> maximize/restore
> hide/show
> focus
```
