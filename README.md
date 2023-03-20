
# WLL - Wilm's Logging Library

A small, lightweight single-header logging library for C/C++.  Makes no allocations, has no external dependencies, and is easy to configure or extend.

Developed for Unix-like environments, but *should* work on Windows as well.

## Authors

- [@WilmH](https://www.github.com/WilmH) - Willem Hunt

## License

[MIT](https://choosealicense.com/licenses/mit/)

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

## Example Usage

```c
// Create new logger object.
WLL_Logger logger = wll_logger_new();

// Add stdout as a stream with color enabled and no levels ignore.
wll_logger_add_stream(&logger, stdout, WLL_COLOR, 0);

// Add a file stream to the logger with default options and ignoring error-level messages.
FILE *log_file = fopen("log.txt", "w");
wlL_logger_add_stream(&logger, log_file, 0, WLL_ERROR);

// Send message at warning level.  Will print in color to stdout and without color to file_open.
wll_log(logger, WLL_WARN, "Warning!");

// Send message at error level.  Will print in color to stdout and will not print to file_open.
wll_log(logger, WLL_ERROR, "Error!");

```

## API Reference

### Create New Logger

Creates a new logger with default levels included.

```c
WLL_Logger wll_logger_new()
```

Creates a new logger with no levels included by default.

```c
WLL_Logger wll_logger_new_empty()
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

### Add New Level to Logger

Adds a new level to a logger, returning the new level's bitflag for reference.

```c
WLL_Level wll_logger_add_level(WLL_Logger* logger, const char* level_name, const char* level_color_code)
```

| Parameter         | Type                              | Description                |
| --------          | -------                           | ------------------------- |
| `logger`          | `WLL_Logger*`                     | Pointer to logger object. |
| `level_name`      | `const char*`                     | String representing level name.  Max length controlled by `WLL_MAX_STRING_LENGTH |
| `level_color_code`         | `const char*`    | ANSI color escape sequence.  Use WLL_MK_COLOR for convenient creation. |


### Log a Message (convenience macro)

```c
bool wll_log(LOGGER, LEVEL, MSG)
```

| Parameter | Type                      | Description                |
| --------  | -------                   | ------------------------- |
| `LOGGER`  | `WLL_Logger`              | Logger to send message to. |
| `LEVEL`   | `WLL_Level (uint64_t)`    | Level Flags. |
| `MSG`     | `const char*`             | Message to log. |

### Log a message (advanced version, use with care)

```c
bool wll_advanced_log(WLL_Logger* logger, WLL_Level level, char* file, uint64_t line, char* message, char* datetime)
```

| Parameter     | Type          | Description                |
| --------      | -------       | ------------------------- |
| `logger`      | `WLL_Logger`  | Logger to send message to. |
| `level`       | `WLL_Level`   | Level Flags|
| `file`        | `char*`       | Source File Name |
| `line`        | `uint64_t`    | Source File Line Number |
| `message`     | `char*`       | Message to log. |
| `datetime`    | `char*`       | String representing current datetime. |

## Adding Custom Levels

You may wish to use levels other than the included set (WARN, ERROR, INFO, SUCCESS, DEBUG).  To do this, use the `wll_logger_add_level` function to add a new level to your existing WLL_Logger object.  For example:

```c
// Create new logger with default levels included
WLL_Logger logger = wll_logger_new(); 
// Add stdout as a stream with default opts.
wll_logger_add_stream(&logger, stdout, WLL_DEFAULT, WLL_DEFAULT);
// Add a custom level with no defined color and the name "LevelName".
WLL_Level new_level = wll_logger_add_level(&logger, "LevelName", NULL);
// Send log message with newly defined level.
wll_log(logger, new_level, "Message!");
```

### Names

Level names are stored as simple string literals.  The maximum length is controlled by `WLL_MAX_STRING_LENGTH`

### Colors

Colors are defined as a string representing the ANSI control codes for that color.  The default colors included with WLL are:

| Macro Shortcut    | Equivalent String
|----------------   |------------------
| WLL_NOCOLOR       | "\e[0;m"
| WLL_RED           | "\e[0;31m" 
| WLL_GREEN         | "\e[0;32m" 
| WLL_YELLOW        | "\e[0;33m"
| WLL_BLUE          | "\e[0;34m" 
| WLL_CYAN          | "\e[0;36m"

Any ANSI color escape code can be used, depending on terminal support.  For more info on ANSI color codes, see this [page](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors).  For convenience the `WLL_MK_COLOR(X)` macro is included. `WLL_MK_COLOR(X)` is equivalent to the string `"\e[0;{X}m"`, where X is any ANSI color code.

## Configuring and Extending WLL

### Max Stream Count

Redefine `WLL_MAX_STREAM_COUNT` to any valid uint64_t value to change the max number of streams in a logger. Default: 256.

### Max String Length

Redefine `WLL_MAX_STRING_LENGTH` to any valid uint64_t value to change the max length of level names and logging messages.  Default: 256.


