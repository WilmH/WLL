#ifndef WLL_H
#define WLL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// Default allocation for number of streams in a logger, set to whatever you want.
// sizeof(logger) == (sizeof(uint64_t) * 2 + sizeof(FILE)) * WLL_MAX_STREAM_COUNT
#define WLL_MAX_STREAM_COUNT 255

// Bit-shifting mnemonic for making flags.  When adding a new flag, make sure it's shifted number is between 0 and 63 and unique.
#define WLL_SHIFT(X)        (1 << (X))

// Stream Options

#define WLL_DEFAULT         0
#define WLL_COLOR           WLL_SHIFT(5)

// ---------------------------------------- //

// Levels //

#define WLL_WARN            WLL_SHIFT(0)
#define WLL_ERROR           WLL_SHIFT(1)
#define WLL_INFO            WLL_SHIFT(2)
#define WLL_SUCCESS         WLL_SHIFT(3)
#define WLL_DEBUG           WLL_SHIFT(4)
#define WLL_ALL_LEVELS      (WLL_WARN | WLL_ERROR | WLL_INFO | WLL_DEBUG | WLL_SUCCESS)

// Colors codes

#define WLL_NOCOLOR         "\e[0;m"
#define WLL_RED             "\e[0;31m" 
#define WLL_GREEN           "\e[0;32m" 
#define WLL_YELLOW          "\e[0;33m"
#define WLL_BLUE            "\e[0;34m" 
#define WLL_CYAN            "\e[0;36m"

// Level Names

#define WLL_WARN_STRING     "WARN"
#define WLL_ERROR_STRING    "ERROR"
#define WLL_INFO_STRING     "INFO"
#define WLL_SUCCESS_STRING  "SUCCESS"
#define WLL_DEBUG_STRING    "DEBUG"

typedef uint64_t WLL_Level;

// Internal format for storing level information including name and color.
struct WLL_Level_Data 
{
    WLL_Level level;
    char* string;
    char* color_string;
};

// Level info, edit this to add custom levels
#define WLL_LEVELS \
    (struct WLL_Level_Data[]){   \
        {WLL_WARN,      WLL_WARN_STRING,    WLL_YELLOW},\
        {WLL_ERROR,     WLL_ERROR_STRING,   WLL_RED},\
        {WLL_INFO,      WLL_INFO_STRING,    WLL_BLUE},\
        {WLL_SUCCESS,   WLL_SUCCESS_STRING, WLL_GREEN},\
        {WLL_DEBUG,     WLL_DEBUG_STRING,   WLL_CYAN},\
    }\

// MUST be updated if you change the number of levels.
#define WLL_LEVEL_COUNT     5

// ---------------------------------------- //

// Types //

typedef FILE WLL_Stream;
typedef uint64_t WLL_Stream_Option;

// Logger struct, create with wll_logger_new() for convenience.
typedef struct WLL_Logger 
{
    WLL_Stream*         streams[WLL_MAX_STREAM_COUNT];
    WLL_Stream_Option   options[WLL_MAX_STREAM_COUNT];
    WLL_Level           ignored_levels[WLL_MAX_STREAM_COUNT];
    uint64_t            stream_count;
} WLL_Logger;

// ---------------------------------------- //

// Private Functions //

/**
 * @brief Writes a string representing a given level with optional color to a char*.
 * 
 * @param buffer FILE buffer to write to.
 * @param level Level to log with.
 * @param color Color?
 * @return Pointer to modified string.
 */
char*
wll_level_string(char* buffer, WLL_Level level, WLL_Stream_Option color)
{
    buffer[0] = '\0';
    for(int i = 0; i < WLL_LEVEL_COUNT; i++)
    {
        if(WLL_LEVELS[i].level == level)
        {
            struct WLL_Level_Data level_data = WLL_LEVELS[i];

            char* color_string;
            char* suffix_string;

            if(color & WLL_COLOR)
            {
                color_string = level_data.color_string;
                suffix_string = WLL_NOCOLOR;
            }
            else
            {
                color_string = "";
                suffix_string = "";
            }

            char* string        = level_data.string;
            sprintf(buffer, "%s%s%s", color_string, string, suffix_string);
        }
    }
    return buffer;
}

/**
 * @brief Writes a string representing the current date and time to a char*.
 *
 * @param datetime char* to write to.
 */
void 
wll_update_datetime(char* datetime)
{
    time_t t        = time(NULL);
    struct tm tm    = *localtime(&t);
    sprintf(
        datetime, 
        "%d-%02d-%02d %02d:%02d:%02d", 
        tm.tm_year + 1900, 
        tm.tm_mon + 1, 
        tm.tm_mday, 
        tm.tm_hour, 
        tm.tm_min, 
        tm.tm_sec
    );
}

/**
 * @brief fprints log message to a given stream. Do not call directly, instead use wll_log!
 * 
 * @param stream FILE handle
 * @param opts Stream option flags
 * @param ignored_levels Ignore flags 
 * @param level Logging level
 * @param file file name
 * @param line line number
 * @param message log message
 * @param datetime Datetime string
 */
void 
wll_internal_log_stream(WLL_Stream* stream, WLL_Stream_Option opts, WLL_Level ignored_levels, WLL_Level level, char* file, uint64_t line, char *message, char* datetime)
{
    if(ignored_levels & level)
    {
        return;
    }
    char level_string_buffer[64];
    fprintf(stream, "%s %s:%llu [%s] - %s\n", datetime, file, line, wll_level_string(level_string_buffer, level, opts & WLL_COLOR), message);
}

// ---------------------------------------- //

// Public API //

/**
 * @brief Creates and returns a new empty logger.
 * 
 * @return logger
 */
WLL_Logger  
wll_logger_new()
{
    return (WLL_Logger){
        .streams={NULL},
        .options={WLL_DEFAULT},
        .ignored_levels={WLL_DEFAULT},
        .stream_count=0
    };
}

/**
 * @brief Add a FILE stream to the given logger.
 * 
 * @param logger logger handle.
 * @param stream FILE handle, often stdout.
 * @param options WLL_Stream_Option flags. Specify settings like whether to print in color.
 * @param ignored_levels WLL_Level ignore flags. Ignores messages of all levels specified here.
 * @return true on success or false on failure
 */
bool 
wll_logger_add_stream(WLL_Logger* logger, WLL_Stream* stream, WLL_Stream_Option options, WLL_Level ignored_levels)
{
    if(logger->stream_count == WLL_MAX_STREAM_COUNT)
    {
        return false;
    }
    logger->streams[logger->stream_count]         = stream;
    logger->ignored_levels[logger->stream_count]   = ignored_levels;
    logger->options[logger->stream_count++]       = options;
    return true;
}

/**
 * @brief Internal logging function.  Do not call directly unless you know what you're doing! Instead use wll_log for simple usage.
 * 
 * @param logger logger handle
 * @param level logging level
 * @param file file name
 * @param line line number
 * @param message log message
 * @param datetime Custom datetime string.  Pass NULL to use WLL's datetime.
 */
void 
wll_advanced_log(WLL_Logger logger, WLL_Level level, char* file, uint64_t line, char* message, char* datetime)
{
    char my_datetime[64];
    
    if(!datetime)
    {
        wll_update_datetime(my_datetime);
    } 
    else
    {
        strcpy(my_datetime, datetime);
    }

    for(uint64_t i = 0; i < logger.stream_count; i++)
    {
        wll_internal_log_stream(logger.streams[i], logger.options[i], logger.ignored_levels[i], level, file, line, message, my_datetime);
    }
}

/**
 * @brief Macro used to send log message to logger, specifying level
 * 
 * @param LOGGER logger handle
 * @param LEVEL log level
 * @param MSG log message
 */
#define wll_log(LOGGER, LEVEL, MSG) wll_advanced_log(&LOGGER, LEVEL, __FILE__, __LINE__, MSG, NULL)

// ---------------------------------------- //

#endif