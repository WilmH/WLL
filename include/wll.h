#ifndef WLL_H
#define WLL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define WLL_VERSION_MAJOR @WLL_VERSION_MAJOR@
#define WLL_VERSION_MINOR @WLL_VERSION_MINOR@

// Default allocation for number of streams in a logger, set to whatever you want.
// sizeof(logger) == (sizeof(uint64_t) * 2 + sizeof(FILE)) * WLL_MAX_STREAM_COUNT
#define WLL_MAX_STREAM_COUNT    256

// Default max string length for level names and log messages
#define WLL_MAX_STRING_LENGTH   256

// Default max number of defined levels.  This cannot raised currently, only lowered, as uint64_t is used for Level ids.
// TODO: find a way to make this configurable.
#define WLL_MAX_LEVEL_COUNT     64

// Bit-shifting mnemonic for making flags.  When adding a new flag, make sure it's shifted number is between 0 and 63 and unique.
// The last default flag is equal to WLL_SHIFT(4), 5+ should be fine to use.
#define WLL_SHIFT(X)        (1 << (X))

// Stream Options

#define WLL_DEFAULT         0
#define WLL_COLOR           WLL_SHIFT(5)

// Predefined Levels

#define WLL_WARN            WLL_SHIFT(0)
#define WLL_ERROR           WLL_SHIFT(1)
#define WLL_INFO            WLL_SHIFT(2)
#define WLL_SUCCESS         WLL_SHIFT(3)
#define WLL_DEBUG           WLL_SHIFT(4)

//Convenience macro for creating new colors.  X must be a valid ANSI color number.
#define WLL_MK_COLOR(X)         "\e[0;" #X "m"

#define WLL_NOCOLOR             "\e[0;m"
#define WLL_RED                 WLL_MK_COLOR(31)
#define WLL_GREEN               WLL_MK_COLOR(32)
#define WLL_YELLOW              WLL_MK_COLOR(33)
#define WLL_BLUE                WLL_MK_COLOR(34)
#define WLL_CYAN                WLL_MK_COLOR(36)

/// Types///

typedef FILE        WLL_Stream;
typedef uint64_t    WLL_Stream_Option;
typedef uint64_t    WLL_Level;

// Internal format for storing level information including name and color.
typedef struct WLL_Level_Data {
    char        name[WLL_MAX_STRING_LENGTH];
    char        color_code[WLL_MAX_STRING_LENGTH];
    uint64_t    bitflag;
} WLL_Level_Data;

// Logger struct, create with wll_logger_new() for convenience.
typedef struct WLL_Logger 
{
    WLL_Stream*         streams[WLL_MAX_STREAM_COUNT];
    WLL_Stream_Option   stream_options[WLL_MAX_STREAM_COUNT];
    WLL_Level           stream_ignored_levels[WLL_MAX_STREAM_COUNT];
    uint64_t            stream_count;

    WLL_Level_Data      levels[WLL_MAX_LEVEL_COUNT];
    uint64_t            level_count;

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
wll_level_string(char* buffer, WLL_Level_Data level_data, WLL_Stream_Option color)
{

    if(!buffer)
    {
        return NULL;
    }

    buffer[0] = '\0';
    char* color_string;
    char* suffix_string;

    if(color & WLL_COLOR)
    {
        color_string    = level_data.color_code;
        suffix_string   = WLL_NOCOLOR;
    }
    else
    {
        color_string    = "";
        suffix_string   = "";
    }

    char* string = level_data.name;
    sprintf(buffer, "%s%s%s", color_string, string, suffix_string);

    return buffer;
}

/**
 * @brief Gets level data from flag, if present.
 * @param logger Logger
 * @param level Level Flag
 * @return Level data or an error value.
 */
WLL_Level_Data 
wll_logger_get_level_data(WLL_Logger logger, WLL_Level level)
{
    for(int i = 0; i < logger.level_count; i++)
    {
        if(logger.levels[i].bitflag == level)
        {
            return logger.levels[i];
        }
    }
    return (WLL_Level_Data){.bitflag=0,.color_code="",.name=""};
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
wll_internal_log_stream(WLL_Stream* stream, WLL_Stream_Option opts, WLL_Level ignored_levels, WLL_Level_Data level_data, char* file, uint64_t line, char *message, char* datetime)
{
    if(ignored_levels & level_data.bitflag)
    {
        return;
    }
    char level_string_buffer[WLL_MAX_STRING_LENGTH];
    fprintf(stream, "%s %s:%llu [%s] - %s\n", datetime, file, line, wll_level_string(level_string_buffer, level_data, opts & WLL_COLOR), message);
}

// ---------------------------------------- //

// Public API //

/**
 * @brief Add a new level to a logger.
 * @param logger Logger
 * @param level_name Level name.  Ex: "WARN".
 * @param level_color_code Level color code.  Must be a string forming a valid ANSI color escape sequence.
 * @return New level's bitflag for reference.
 */
WLL_Level 
wll_logger_add_level(WLL_Logger* logger, const char* level_name, const char* level_color_code)
{
    if(!logger)
    {
        fprintf(stderr, "WLL: Logger is NULL, can't add level!\n");
        return 0;
    }

    if(logger->level_count == WLL_MAX_LEVEL_COUNT)
    {
        fprintf(stderr, "WLL: Max level count exceeded\n");
        return 0;
    }

    if(strlen(level_name) >= WLL_MAX_STRING_LENGTH)
    {
        fprintf(stderr, "WLL: Level name exceeds max string length (%d vs %d)", strlen(level_name), WLL_MAX_STRING_LENGTH);
        return 0;
    }

    strcpy(logger->levels[logger->level_count].name, level_name);
    strcpy(logger->levels[logger->level_count].color_code, level_color_code);
    logger->levels[logger->level_count].bitflag = 1 << logger->level_count++;
    return logger->levels[logger->level_count - 1].bitflag;
}

/**
 * @brief Creates and returns a new logger, adding default levels automatically.
 * 
 * @return logger
 */
WLL_Logger  
wll_logger_new()
{
    WLL_Logger logger = {
        .streams                = {NULL},
        .stream_options         = {WLL_DEFAULT},
        .stream_ignored_levels  = {WLL_DEFAULT},
        .stream_count           = 0,
        .levels                 = {},
        .level_count            = 0
    };
    wll_logger_add_level(&logger, "WARN", WLL_YELLOW);
    wll_logger_add_level(&logger, "ERROR", WLL_RED);
    wll_logger_add_level(&logger, "INFO", WLL_BLUE);
    wll_logger_add_level(&logger, "SUCCESS", WLL_GREEN);
    wll_logger_add_level(&logger, "DEBUG", WLL_CYAN);
    return logger;
}


/**
 * @brief Creates and returns a new empty logger.  No levels are added by default.
 * @return new logger.
 */
WLL_Logger
wll_logger_new_empty()
{
    return (WLL_Logger){
        .streams                = {NULL},
        .stream_options         = {WLL_DEFAULT},
        .stream_ignored_levels  = {WLL_DEFAULT},
        .stream_count           = 0,
        .levels                 = {},
        .level_count            = 0
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
    logger->streams[logger->stream_count]                   = stream;
    logger->stream_ignored_levels[logger->stream_count]     = ignored_levels;
    logger->stream_options[logger->stream_count++]          = options;
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
bool 
wll_advanced_log(WLL_Logger logger, WLL_Level level, char* file, uint64_t line, char* message, char* datetime)
{

    if(!file)
    {
        fprintf(stderr, "WLL: Unable to add NULL stream.\n");
        return false;
    }

    if(!message)
    {
        fprintf(stderr, "WLL: Unable to print NULL message.\n");
        return false;
    }

    if(!datetime)
    {
        fprintf(stderr, "WLL: Unable to print NULL datetime string.\n");
        return false;
    }

    if(strlen(message) >= WLL_MAX_STRING_LENGTH)
    {
        fprintf(stderr, "WLL: Message length exceeds max (%d vs %d)\n", strlen(message), WLL_MAX_STRING_LENGTH);
        return false;
    }

    char my_datetime[64];
    if(!datetime)
    {
        wll_update_datetime(my_datetime);
    } 
    else
    {
        strcpy(my_datetime, datetime);
    }

    WLL_Level_Data level_data = wll_logger_get_level_data(logger, level);
    if(level_data.bitflag == 0)
    {
        fprintf(stderr, "WLL: Matching level not found.\n");
        return false;
    }

    for(uint64_t i = 0; i < logger.stream_count; i++)
    {
        wll_internal_log_stream(logger.streams[i], logger.stream_options[i], logger.stream_ignored_levels[i], level_data, file, line, message, my_datetime);
    }

    return true;
}

/**
 * @brief Macro used to send log message to logger, specifying level
 * 
 * @param LOGGER logger handle
 * @param LEVEL log level
 * @param MSG log message
 * @return bool indicating success
 */
#define wll_log(LOGGER, LEVEL, MSG) wll_advanced_log(LOGGER, LEVEL, __FILE__, __LINE__, MSG, NULL)

// ---------------------------------------- //

#endif