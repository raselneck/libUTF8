/**
 * Copyright (c) 2016 Richard Selneck (@TheCodeInside)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <stdint.h>

/* Get our malloc and free macros */
#if !defined(UTF8_MALLOC) || !defined(UTF8_FREE)
    #include <stdlib.h> // For malloc, free
    #if !defined(UTF8_MALLOC)
        #define UTF8_MALLOC malloc
    #endif
    #if !defined(UTF8_FREE)
        #define UTF8_FREE free
    #endif
#endif

/* Get us a universal boolean type up in here */
#if defined(__cplusplus)
typedef bool bool_t;
#else
typedef enum bool_t
{
    false,
    true
} bool_t;
#endif

/* The universal UTF-8 BOM (byte order mark) U+FEFF */
#define UTF8_BOM "\xEF\xBB\xBF"
#define UTF8_BOM_STRLEN 3

/* The typical UTF-8 replacement codepoint U+FFFD */
#define UTF8_REPLACEMENT "\xEF\xBF\xBD"

/**
 * An enumeration of UTF-8 error codes.
 */
typedef enum utf8_error_t
{
    UTF8_INVALID_STRING     = -(1 << 0),
    UTF8_INVALID_CODEPOINT  = -(1 << 1),
    UTF8_INVALID_LENGTH     = -(1 << 2)
} utf8_error_t;

/**
 * The type used for codepoints.
 */
typedef int32_t utf8_codepoint_t;

/**
 * The type used for iterating strings.
 */
typedef struct utf8_iterator_t utf8_iterator_t;

/**
 * \brief Encodes the given string data into a UTF-8 codepoint.
 *
 * \param str The string.
 * \return The encoded codepoint. Can return UTF8_INVALID_CODEPOINT if the string
 *         does not contain a valid UTF-8 codepoint.
 */
utf8_codepoint_t utf8_encode(const char* str);

/**
 * \brief Checks to see if the given string contains the UTF-8 BOM.
 *
 * \param str The UTF-8 string.
 * \return True if the string begins with the UTF-8 BOM, otherwise false.
 */
bool_t utf8_has_bom(const char* str);

/**
 * \brief Gets the length of a UTF-8 string.
 *
 * \param str The string.
 * \return The length of \p str. Can return UTF8_INVALID_STRING if the string is not
 *         valid UTF-8.
 */
size_t utf8_strlen(const char* str);

/**
 * \brief Checks to see if the given string is valid UTF-8.
 *
 * \param str The string.
 * \return True if the string is valid UTF-8, otherwise false.
 */
bool_t utf8_valid(const char* str);

/**
 * \brief Checks to see if the given Unicode codepoint is valid.
 *
 * \param codepoint The Unicode codepoint.
 * \return True if the codepoint is valid, otherwise false.
 */
bool_t utf8_valid_codepoint(utf8_codepoint_t codepoint);