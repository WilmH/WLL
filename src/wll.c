#include <time.h>
#include "wll.h"

char wll_current_datetime_s[64];

// Updates wll_current_datetime_s with the current datetime for logging strings.
void 
wll_update_datetime()
{
    time_t t        = time(NULL);
    struct tm tm    = *localtime(&t);
    sprintf(
        wll_current_datetime_s, 
        "%d-%02d-%02d %02d:%02d:%02d", 
        tm.tm_year + 1900, 
        tm.tm_mon + 1, 
        tm.tm_mday, 
        tm.tm_hour, 
        tm.tm_min, 
        tm.tm_sec
    );
}

// Colors

#define WLL_NOCOLOR         "\e[0;m"
#define WLL_RED             "\e[0;31m" 
#define WLL_GREEN           "\e[0;32m" 
#define WLL_YELLOW          "\e[0;33m"
#define WLL_BLUE            "\e[0;34m" 
#define WLL_CYAN            "\e[0;36m"

// Levels

#define WLL_LEVEL_COUNT     5
#define WLL_WARN_STRING     "WARN"
#define WLL_ERROR_STRING    "ERROR"
#define WLL_INFO_STRING     "INFO"
#define WLL_SUCCESS_STRING  "SUCCESS"
#define WLL_DEBUG_STRING    "DEBUG"

// Contains info on how to present each level when printing, including name and color.
struct WLL_Level_Data 
{
    WLL_Level level;
    char* string;
    char* color_string;
} 
levels[WLL_LEVEL_COUNT] = {
    {WLL_WARN,      WLL_WARN_STRING,    WLL_YELLOW},
    {WLL_ERROR,     WLL_ERROR_STRING,   WLL_RED},
    {WLL_INFO,      WLL_INFO_STRING,    WLL_BLUE},
    {WLL_SUCCESS,   WLL_SUCCESS_STRING, WLL_GREEN},
    {WLL_DEBUG,     WLL_DEBUG_STRING,   WLL_CYAN},
};

char level_string_buffer[64];

// Overwrites level_string_buffer with the approprate string.
void
wll_level_string(WLL_Level level, WLL_Stream_Option color)
{
    level_string_buffer[0] = '\0';
    for(int i = 0; i < WLL_LEVEL_COUNT; i++)
    {
        if(levels[i].level == level)
        {
            struct WLL_Level_Data level_data = levels[i];
            char* color_string  = (color & WLL_COLOR) ? level_data.color_string : "";
            char* suffix_string = (color & WLL_COLOR) ? WLL_NOCOLOR             : "";
            char* string        = level_data.string;
            sprintf(level_string_buffer, "%s%s%s", color_string, string, suffix_string);
        }
    }
}

/**
 * @brief Contains information on streams and stream options.  Generally passed as a opaque handle.
 */
struct WLL_Logger 
{
    WLL_Stream*         streams[WLL_MAX_STREAM_COUNT];
    WLL_Stream_Option   options[WLL_MAX_STREAM_COUNT];
    WLL_Level           ignore_levels[WLL_MAX_STREAM_COUNT];
    uint64_t            stream_count;
};

WLL_Logger*  
wll_logger_new(void* (*allocator)(size_t))
{
    struct WLL_Logger* logger = allocator(sizeof(struct WLL_Logger));
    if(!logger)
    {
        return NULL;
    }
    logger->stream_count = 0;
    return logger;
}

bool 
wll_logger_add_stream(WLL_Logger* logger, WLL_Stream* stream, WLL_Stream_Option options, WLL_Level ignore_levels)
{
    struct WLL_Logger* concrete_logger = (struct WLL_Logger*) logger;
    if(concrete_logger->stream_count == WLL_MAX_STREAM_COUNT)
    {
        return false;
    }
    concrete_logger->streams[concrete_logger->stream_count]         = stream;
    concrete_logger->ignore_levels[concrete_logger->stream_count]   = ignore_levels;
    concrete_logger->options[concrete_logger->stream_count++]       = options;
    return true;
}

/**
 * @brief Fprints log message to a given stream
 * 
 * @param stream FILE handle
 * @param opts Stream option flags
 * @param ignore_levels Ignore flags 
 * @param level Logging level
 * @param file file name
 * @param line line number
 * @param message log message
 */
void 
wll_internal_log_stream(WLL_Stream* stream, WLL_Stream_Option opts, WLL_Level ignore_levels, WLL_Level level, char* file, uint64_t line, char *message)
{
    if(ignore_levels & level)
    {
        return;
    }
    wll_level_string(level, opts & WLL_COLOR);
    fprintf(stream, "%s %s:%llu [%s] - %s\n", wll_current_datetime_s, file, line, level_string_buffer, message);
}

void 
wll_internal_log(WLL_Logger* logger, WLL_Level level, char* file, uint64_t line, char* message)
{
    wll_update_datetime();
    struct WLL_Logger* concrete_logger = (struct WLL_Logger*) logger;
    for(uint64_t i = 0; i < concrete_logger->stream_count; i++)
    {
        wll_internal_log_stream(concrete_logger->streams[i], concrete_logger->options[i], concrete_logger->ignore_levels[i], level, file, line, message);
    }
}