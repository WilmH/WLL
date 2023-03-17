
# WLL - Wilm's Logging Library

A small, lightweight single-header logging library for C/C++.  Makes no allocations, has no external dependencies, and is easy to configure or extend.

## Authors

- [@WilmH](https://www.github.com/WilmH) - Willem Hunt

## Installation

WLL is a single-header library.  To use it in your project, simply install with cmake and include `<wll.h>` in your source files...

```bash
cmake .
make install
```

OR copy `wll.h` into the include directory of your project.

``` bash
cp wll/include/wll.h myproject/include/
```

## API Reference

### Create New Logger

```c
WLL_Logger wll_logger_new()
```

### Add New Stream to Logger

Returns a bool indicating success.

```c
bool wll_logger_add_stream(WLL_Logger* logger, WLL_Stream* stream, WLL_Stream_Option options, WLL_Level ignored_levels)
```

| Parameter         | Type                              | Description                |
| --------          | -------                           | ------------------------- |
| `logger`          | `WLL_Logger*`                     | Pointer to logger object. |
| `stream`          | `WLL_Stream (FILE)`               | FILE pointer, often stdout. |
| `options`         | `WLL_Stream_Option (uint64_t)`    | Flags for stream options.  Examples include WLL_COLOR, which enables color printing on outputs that support it. |
| `ignored_levels`  | `WLL_Level (uint64_t)`            | Flags for ignore options, indicating which levels to skip printing for this stream.|

### Log a message (convenience macro)

```c
wll_log(LOGGER, LEVEL, MSG)
```

| Parameter | Type                      | Description                |
| --------  | -------                   | ------------------------- |
| `LOGGER`  | `WLL_Logger*`             | Logger to send message to. |
| `LEVEL`   | `WLL_Level (uint64_t)`    | Level Flags. |
| `MSG`     | `const char*`             | Message to log. |

### Log a message (advanced version, use with care)

```c
void wll_advanced_log(WLL_Logger* logger, WLL_Level level, char* file, uint64_t line, char* message, char* datetime)
```

| Parameter     | Type          | Description                |
| --------      | -------       | ------------------------- |
| `logger`      | `WLL_Logger*` | Logger to send message to. |
| `level`       | `WLL_Level`   | Level Flags|
| `file`        | `char*`       | Source File Name |
| `line`        | `uint64_t`    | Source File Line Number |
| `message`     | `char*`       | Message to log. |
| `datetime`    | `char*`       | String representing current datetime. |

## Configuring WLL

### Max Stream Count

Redefine `WLL_MAX_STREAM_COUNT` to any valid uint64_t value to change the max number of streams in a logger. Default: 255

### Modifying Levels

The default table of levels looks like this:

```c
#define wll_levels \
    (struct WLL_Level_Data[]){   \
        {WLL_WARN,      WLL_WARN_STRING,    WLL_YELLOW},\
        {WLL_ERROR,     WLL_ERROR_STRING,   WLL_RED},\
        {WLL_INFO,      WLL_INFO_STRING,    WLL_BLUE},\
        {WLL_SUCCESS,   WLL_SUCCESS_STRING, WLL_GREEN},\
        {WLL_DEBUG,     WLL_DEBUG_STRING,   WLL_CYAN},\
    }\
```

To modify a default level, redefine one of its constituent macros.

To *add* a level, you'll need to:
- define its name, flag value, and color code.
- add it to the wll_levels list.
- update WLL_LEVEL_COUNT to reflect your changes.

#### Flag Values

Levels are represented by a 64-bit flag (a 1 bit-shifted shifted 0-63 places).  Thus, the maximum number of levels that can be represented in WLL is 64.  Hopefully this will be enough for any conventional program.

The default level flags are defined like this:

```c
// WLL_SHIFT(N) is equivalent to (1 << N)

#define WLL_WARN            WLL_SHIFT(0)  
#define WLL_ERROR           WLL_SHIFT(1)
#define WLL_INFO            WLL_SHIFT(2)
#define WLL_SUCCESS         WLL_SHIFT(3)
#define WLL_DEBUG           WLL_SHIFT(4)
#define WLL_ALL_LEVELS      \
(WLL_WARN | WLL_ERROR | WLL_INFO | WLL_DEBUG | WLL_SUCCESS)

```

If you add your own, make sure they don't conflict with an existing one.

#### Names

Level names are stored as simple string literals.  The default list looks like this:

```c
#define WLL_WARN_STRING     "WARN"
#define WLL_ERROR_STRING    "ERROR"
#define WLL_INFO_STRING     "INFO"
#define WLL_SUCCESS_STRING  "SUCCESS"
#define WLL_DEBUG_STRING    "DEBUG"
```

#### Colors

Colors are defined as a string representing the ANSI control codes for that color.  The default colors included with WLL are defined like this:

```c
#define WLL_NOCOLOR         "\e[0;m"
#define WLL_RED             "\e[0;31m" 
#define WLL_GREEN           "\e[0;32m" 
#define WLL_YELLOW          "\e[0;33m"
#define WLL_BLUE            "\e[0;34m" 
#define WLL_CYAN            "\e[0;36m"
```

Any ANSI color escape code can be used, depending on terminal support.  For more info on ANSI color codes, see this [page](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors).




