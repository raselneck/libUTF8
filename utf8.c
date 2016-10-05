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
#include "utf8.h"

/* TODO - Consolidate code between utf8_strlen and utf8_valid by providing some kind of walking function */

static const char* UTF8_BOM_INST = UTF8_BOM;

/**
 * \brief Gets information about the given codepoint.
 *
 * \param str The codepoint string.
 * \param mask [out] The mask to use when building the codepoint.
 * \param length [out] The number of bytes expected for the codepoint.
 */
bool_t utf8_get_codepoint_info(const char* str, int32_t* mask, int32_t* length)
{
    uint8_t ch = *str;

    // TODO - Optimize this into a loop

    if (ch < 128)
    {
        *mask = 0b01111111;
        *length = 1;
    }
    else if ((ch & 0b11100000) == 0b11000000)
    {
        *mask = 0b00011111;
        *length = 2;
    }
    else if ((ch & 0b11110000) == 0b11100000)
    {
        *mask = 0b00001111;
        *length = 3;
    }
    else if ((ch & 0b11111000) == 0b11110000)
    {
        *mask = 0b00000111;
        *length = 4;
    }
    else if ((ch & 0b11111100) == 0b11111000)
    {
        *mask = 0b00000011;
        *length = 5;
    }
    else if ((ch & 0b11111110) == 0b11111100)
    {
        *mask = 0b0000001;
        *length = 6;
    }
    else
    {
        *mask = 0b0000000;
        *length = UTF8_INVALID_LENGTH;
        return false;
    }

    return true;
}

/**
 * \brief Encodes the given string data into a UTF-8 codepoint.
 *
 * \param str The string.
 * \param mask The codepoint mask retrieved from utf8_char_info.
 * \param length The codepoint length retrieved from utf8_char_info.
 * \return The encoded codepoint.
 */
utf8_codepoint_t utf8_encode_info(const char* str, int32_t mask, int32_t length)
{
    utf8_codepoint_t codepoint = 0;

    /* So, we at least know that the first character exists and is legit */
    codepoint = *str & mask;
    codepoint <<= (7 - length);

    /* All bytes after the first have the same 0b10xxxxxx format */
    mask = 0b00111111;
    while (--length && ++str)
    {
        codepoint <<= 6;
        codepoint |= *str & mask;
    }

    /* Let's just make sure that we finished reading the full character */
    if (length)
    {
        return UTF8_INVALID_CODEPOINT;
    }

    return codepoint;
}

/**
 * \brief Encodes the given string data into a UTF-8 codepoint.
 *
 * \param str The string.
 * \return The encoded codepoint. Can return UTF8_INVALID_CODEPOINT if the string
 *         does not contain a valid UTF-8 codepoint.
 */
utf8_codepoint_t utf8_encode(const char* str)
{
    int32_t mask = 0;
    int32_t length = 0;
    
    utf8_get_codepoint_info(str, &mask, &length);
    if (length == UTF8_INVALID_LENGTH)
    {
        return UTF8_INVALID_CODEPOINT;
    }

    return utf8_encode_info(str, mask, length);
}

/**
 * \brief Checks to see if the given string contains the UTF-8 BOM.
 *
 * \param str The UTF-8 string.
 * \return True if the string begins with the UTF-8 BOM, otherwise false.
 */
bool_t utf8_has_bom(const char* str)
{
    if (str[0] && str[1] && str[2])
    {
        return str[0] == UTF8_BOM_INST[0]
            && str[1] == UTF8_BOM_INST[1]
            && str[2] == UTF8_BOM_INST[2];
    }
    return false;
}

/**
 * \brief Gets the length of a UTF-8 string.
 *
 * \param str The string.
 * \return The length of \p str. Can return UTF8_INVALID_STRING if the string is not
 *         valid UTF-8.
 */
size_t utf8_strlen(const char* str)
{
    const char* iter = str;
    int32_t     mask = 0;
    int32_t     length_cp = 0;
    size_t      length_str = 0;
    utf8_codepoint_t codepoint = 0;

    while (*iter)
    {
        // Get current character info
        utf8_get_codepoint_info(iter, &mask, &length_cp);

        // Ensure there's a codepoint
        if (length_cp == UTF8_INVALID_LENGTH)
        {
            return (size_t)UTF8_INVALID_STRING;
        }

        // Get the codepoint and validate it
        codepoint = utf8_encode_info(iter, mask, length_cp);
        if (codepoint == UTF8_INVALID_CODEPOINT)
        {
            return (size_t)UTF8_INVALID_STRING;
        }

        length_str++;
        iter += length_cp;
    }

    return length_str;
}

/**
 * \brief Checks to see if the given string is valid UTF-8.
 *
 * \param str The string.
 * \return True if the string is valid UTF-8, otherwise false.
 */
bool_t utf8_valid(const char* str)
{
    const char* iter = str;
    int32_t     mask = 0;
    int32_t     length = 0;
    utf8_codepoint_t codepoint = 0;

    while (*iter)
    {
        // Get current character info
        utf8_get_codepoint_info(iter, &mask, &length);

        // Ensure there's a codepoint
        if (length == UTF8_INVALID_LENGTH)
        {
            return false;
        }
        
        // Get the codepoint and validate it
        codepoint = utf8_encode_info(iter, mask, length);
        if (codepoint == UTF8_INVALID_CODEPOINT)
        {
            return false;
        }

        iter += length;
    }

    return true;
}

/**
 * \brief Checks to see if the given Unicode codepoint is valid.
 *
 * \param codepoint The Unicode codepoint.
 * \return True if the codepoint is valid, otherwise false.
 */
bool_t utf8_valid_codepoint(utf8_codepoint_t codepoint)
{
    uint32_t cp = (uint32_t)codepoint;

    /**
     * These two checks cover:
     * 1) That the codepoint is not above 0x10FFFF, which are not encodable by UTF-16, and
     * 2) That the codepoint is not 0xD800 to 0xDFFF, which are UTF-16 surrogates
     */
    return  (cp < 0x110000)
        && ((cp < 0xD800) || (cp > 0xDFFF));
}
