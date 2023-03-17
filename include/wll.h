#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Default allocation for number of streams in a logger, set to whatever you want.
#define WLL_MAX_STREAM_COUNT 255

// INTERNAL bit-shifting mnemonic for making flags.
#define WLL_SHIFT(X)        (1 << (X))

// Levels //

#define WLL_WARN            WLL_SHIFT(0)
#define WLL_ERROR           WLL_SHIFT(1)
#define WLL_INFO            WLL_SHIFT(2)
#define WLL_SUCCESS         WLL_SHIFT(3)
#define WLL_DEBUG           WLL_SHIFT(4)
#define WLL_ALL_LEVELS      (WLL_WARN | WLL_ERROR | WLL_INFO | WLL_DEBUG | WLL_SUCCESS)

// Options // 

#define WLL_DEFAULT         0
#define WLL_COLOR           WLL_SHIFT(5)

// Types, aliased for opacity //

/**
 * @brief Logger handle, create with wll_logger_new() and free when done!
 */
typedef void WLL_Logger;

/**
 * @brief Stream handle, aliased from file for opacity, may change to allow other types later.
 */
typedef FILE WLL_Stream;

/**
 * @brief Stream options, binary flags on a 64-bit uint.
 */
typedef uint64_t WLL_Stream_Option;

/**
 * @brief Level flags, binary flags on a 64-bit uint.
 */
typedef uint64_t WLL_Level;

/**
 * @brief Allocates a new logger using the given malloc-like function.
 * 
 * @param allocator *alloc-like function for allocating the required memory.  May require result to be freed later.
 * @return logger handle
 */
WLL_Logger* 
wll_logger_new(void* (*allocator)(size_t));

/**
 * @brief Add a FILE stream to the given logger.
 * 
 * @param logger logger handle.
 * @param stream FILE handle, often stdout.
 * @param options WLL_Stream_Option flags. Specify settings like whether to print in color.
 * @param ignore_levels WLL_Level ignore flags. Ignores messages of all levels specified here.
 * @return true on success or false on failure
 */
bool 
wll_logger_add_stream(WLL_Logger* logger, WLL_Stream* stream, WLL_Stream_Option options, WLL_Level ignore_levels);

/**
 * @brief Internal logging function.  Do not call directly unless you know what you're doing! Instead use wll_log for simple usage.
 * 
 * @param logger logger handle
 * @param level logging level
 * @param file file name
 * @param line line number
 * @param message log message
 */
void 
wll_internal_log(WLL_Logger* logger, WLL_Level level, char* file, uint64_t line, char* message);

/**
 * @brief Macro used to send log message to logger, specifying level
 * 
 * @param LOGGER logger handle
 * @param LEVEL log level
 * @param MSG log message
 */
#define wll_log(LOGGER, LEVEL, MSG) wll_internal_log(LOGGER, LEVEL, __FILE__, __LINE__, MSG)