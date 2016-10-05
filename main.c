#include "utf8.h"
#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
FILE* fopen_wrapper(const char* fname, const char* mode)
{
    FILE* file = NULL;
    fopen_s(&file, fname, mode);
    return file;
}

#define fopen fopen_wrapper
#endif

#define assert(test) if (test) puts("[PASSED] " #test); else puts("[FAILED] " #test)

bool_t validate_file(const char* fname)
{
    FILE* file = NULL;
    size_t fileSize = 0;
    char* fileContents = NULL;
    bool_t result = false;

    file = fopen(fname, "rb");
    if (!file)
    {
        printf("Failed to open '%s'\n", fname);
        return false;
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileContents = (char*)malloc(fileSize + 1);
    if (!fileContents)
    {
        printf("Failed to allocate %i bytes\n", fileSize);
        fclose(file);
        return false;
    }

    fileContents[fileSize] = 0;
    fread(fileContents, 1, fileSize, file);
    fclose(file);
    file = NULL;

    result = utf8_valid(fileContents);
    free(fileContents);

    return result;
}

int main(int argc, char** argv)
{
    const char* two_chars = "\xE6\x97\xA5\xD1\x88";
    const char* message = "Hello, world!";
    int32_t     index = 0;

    utf8_codepoint_t u0024  = utf8_encode("\x24"); // U+0024 == $
    utf8_codepoint_t u00A2  = utf8_encode("\xC2\xA2"); // U+00A2 == ¢
    utf8_codepoint_t u20AC  = utf8_encode("\xE2\x82\xAC"); // U+20AC == €
    utf8_codepoint_t u10348 = utf8_encode("\xF0\x90\x8D\x88"); // U+10348 == 𐍈

    assert(utf8_valid(two_chars));
    assert(utf8_valid(message));
    assert(utf8_strlen(two_chars) == 2);
    assert(utf8_strlen(message) == strlen(message));
    assert(!utf8_valid_codepoint(UTF8_INVALID_CODEPOINT));
    assert(utf8_valid_codepoint(u0024));
    assert(utf8_valid_codepoint(u00A2));
    assert(utf8_valid_codepoint(u20AC));
    assert(utf8_valid_codepoint(u10348));
    assert(u0024 == 0x24);
    assert(u00A2 == 0xA2);
    assert(u20AC == 0x20AC);
    assert(u10348 == 0x10348);
    assert(utf8_encode(UTF8_BOM) == 0xFEFF);
    assert(utf8_encode(UTF8_REPLACEMENT) == 0xFFFD);
    assert(!utf8_valid("\xFF\xEE"));
    assert(!utf8_valid("\xBE\xEF"));
    assert(!utf8_valid("\xFE"));
    assert(utf8_valid_codepoint(utf8_encode("?")));
    assert(utf8_valid_codepoint(utf8_encode("\xE2\x82\xAC")));

    if (argc > 1)
    {
        for (index = 1; index < argc; ++index)
        {
            if (validate_file(argv[index]))
            {
                printf("[PASSED] '%s' contains valid UTF-8 text\n", argv[index]);
            }
            else
            {
                printf("[FAILED] '%s' does NOT contain valid UTF-8 text\n", argv[index]);
            }
        }
    }

    return 0;
}
