
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

#### Create New Logger

```c
WLL_Logger wll_logger_new()
```

#### Add New Stream to Logger

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

#### Log a message (convenience macro)

```c
wll_log(LOGGER, LEVEL, MSG)
```

| Parameter | Type                      | Description                |
| --------  | -------                   | ------------------------- |
| `LOGGER`  | `WLL_Logger*`             | Logger to send message to. |
| `LEVEL`   | `WLL_Level (uint64_t)`    | Level Flags. |
| `MSG`     | `const char*`             | Message to log. |

#### Log a message (advanced version, use with care)

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