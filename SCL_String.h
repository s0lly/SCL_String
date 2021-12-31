
/*********************************************************************************************************
 
File Name           : SCL_String.h
Version             : v0.04
Date Last Modified  : 30.12.2021
Author              : s0lly
License             : The Unlicense / Public Domain (see end of file for license description)

Discord Channel     : https://discord.gg/kvappxm
YouTube             : https://www.youtube.com/c/s0lly
Twitter             : https://twitter.com/s0lly

NOTE: This library is still under construction, and should only be used with caution.
If you do use it and uncover any bugs or modifications that would be helpful, please contact s0lly via any of the
mediums listed above.

The purpose of this header-only file is to simplify the C programmerâ€™s life in their interaction with strings.

This includes the following features:

- A wrapper around the null-terminated C string into a relatively simple struct with associated functions that manage
the size and allocation of memory.

- The C-style null-terminator is maintained for easy use in functions such as printf and co. 
But the user doesnâ€™t need to concern themselves with the requirement for the null-terminator:
itâ€™s all taken care of behind the scenes via the family of functions provided.
No need to remember how the various stdlib functions treat the null terminator.

 - The wrapper functions provide the functionality of the C standard library with more error detection and versatility.
For example, they account for the additional memory allocation required to store the null terminator,
and expand and contract dynamically according to whatever the user throws at them. 

 - More clearly worded function calls: however, these are more verbose, so may not be to everyoneâ€™s tastes.

 - Additional functionality and clarity around file reading and conversion between strings and other base types,
and additional structs to make working with strings and their conversions that much simpler.

- There are only two allocation (calloc) and deallocation (free) points in the entire library.
These can therefore be modified more easily to the user's own allocation methods, if desired.

- This code runs without error messages when compiling via msvc with /Wall expect for those within <stdio.h>,
and error 4201 (nameless struct) & error 4820 (struct padding) which I accept as a necessary fact of life.

*********************************************************************************************************/

// NOTE(s0lly): Careful on ftell with large files due to return type
// NOTE(s0lly): Using uint8_t as char representation - this may break code on certain platforms
// NOTE(s0lly): Library assumes ASCII style strings only
// TODO(s0lly): Ensure that whenever count reduces, "freed" space is cleared to 0 to ensure cstring code works
// TODO(s0lly): Sift through all integer conversions to ensure correct, limit to int32?


#pragma once

// NOTE(s0lly): External dependencies

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>


// NOTE(s0lly): Defines



// NOTE(s0lly): Enums

typedef enum SCL_STRING_CODE
{
    SCL_STRING_CODE__NO_MESSAGE,
    SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_NULL_CHARACTER_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_INVALID_STRING_COUNT_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_NULL_FILE_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_NULL_FILE_HANDLE_PASSED_TO_FUNCTION,
    SCL_STRING_CODE__ERROR_SPRINTF_CONVERTING_FROM_int64_t_TO_STRING,
    SCL_STRING_CODE__ERROR_SPRINTF_CONVERTING_FROM_double_TO_STRING,
    SCL_STRING_CODE__ERROR_CANT_CONVERT_STRING_TO_int64_t,
    SCL_STRING_CODE__ERROR_CANT_CONVERT_STRING_TO_double,
    SCL_STRING_CODE__ERROR_COMPARE_FAILURE,
    SCL_STRING_CODE__ERROR_COUNTMAX,
    SCL_STRING_CODE__FIND_NO_MATCH,
    SCL_STRING_CODE__FILE_ENCOUNTERED_EOF,
    SCL_STRING_CODE__COMPARE_LESS_THAN,
    SCL_STRING_CODE__COMPARE_EQUAL,
    SCL_STRING_CODE__COMPARE_GREATER_THAN,
    
} SCL_STRING_CODE;


// NOTE(s0lly): Structs

typedef struct String
{
    uint8_t *e;
    int64_t count;
    int64_t countMax;
    
} String;

typedef struct StringMessage
{
    SCL_STRING_CODE code;
    union
    {
        String string;
        int64_t int64Val;
        double doubleVal;
        String *stringPtr;
        uint8_t *chPtr;
    };
    
} StringMessage;

typedef struct File
{
    FILE *handle;
    int32_t cursor;
    
} File;

typedef struct StringList
{
    String *e;
    int64_t count;
    int64_t countMax;
    
} StringList;


// NOTE(s0lly): Helper functions

static void Mem_ClearBytes(void *ptr, int64_t bytes)
{
    memset(ptr, 0, bytes);
}


// NOTE(s0lly): String functions

static StringMessage String_Get_Count(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.int64Val = string->count;
    }
    return msg;
}

static StringMessage String_Get_CountMax(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.int64Val = string->countMax;
    }
    return msg;
}

static StringMessage String_Get_First(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if(string->count == 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.chPtr = &string->e[0];
    }
    return msg;
}

static StringMessage String_Get_Last(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if(string->count == 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.chPtr = &string->e[string->count - 1];
    }
    return msg;
}

static StringMessage String_Get(String *string, int64_t index)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (index < 0 || index >= string->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.chPtr = &string->e[index];
    }
    return msg;
}

static StringMessage String_From_CountMax(int64_t countMax)
{
    StringMessage msg = { 0 };
    if (countMax < 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_INVALID_STRING_COUNT_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.string.e = calloc(countMax + 1, sizeof(msg.string.e[0]));
        msg.string.countMax = countMax;
    }
    return msg;
}

static StringMessage String_Internal_CopyStringIntoMessage(const void *src, int64_t newCount, int64_t newCountMax)
{
    StringMessage msg = String_From_CountMax(newCountMax);
    if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
    {
        memmove(msg.string.e, src, newCount);
        msg.string.count = newCount;
    }
    return msg;
}

static StringMessage String_From_CStr(const char *cStr)
{
    StringMessage msg = { 0 };
    if (!cStr)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Internal_CopyStringIntoMessage((void *)cStr, strlen((const char *)cStr), strlen((const char *)cStr));
    }
    return msg;
}

static StringMessage String_Destroy(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else
    {
        free(string->e);
        *string = (String) { 0 };
    }
    return msg;
}

static StringMessage String_From_String(String *src)
{
    StringMessage msg = { 0 };
    if (!src)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!src->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Internal_CopyStringIntoMessage(src->e, src->count, src->count);
    }
    return msg;
}

static StringMessage String_From_SubString(String *string, int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStartInclusive < 0 || indexStartInclusive >= string->count ||
             indexEndInclusive < 0 || indexEndInclusive >= string->count ||
             indexStartInclusive > indexEndInclusive)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else 
    {
        msg = String_Internal_CopyStringIntoMessage(string->e + indexStartInclusive,
                                                    indexEndInclusive - indexStartInclusive + 1,
                                                    indexEndInclusive - indexStartInclusive + 1);
        
    }
    return msg;
}

static StringMessage String_From_int64_t(int64_t val)
{
    StringMessage msg = { 0 };
    uint8_t tempValString[32];
    int32_t tempCheck = sprintf((char *)tempValString, "%lld", val);
    if (tempCheck < 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_SPRINTF_CONVERTING_FROM_int64_t_TO_STRING;
    }
    else
    {
        msg = String_From_CStr((const char *)tempValString);
    }
    return msg;
}

static StringMessage String_From_double(double val)
{
    StringMessage msg = { 0 };
    uint8_t tempValString[32];
    int32_t tempCheck = sprintf((char *)tempValString, "%.16f", val);
    if (tempCheck < 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_SPRINTF_CONVERTING_FROM_int64_t_TO_STRING;
    }
    else
    {
        msg = String_From_CStr((const char *)tempValString);
    }
    return msg;
}

// TODO(s0lly): Flatten code
static StringMessage String_From_FileNextLine(File *file)
{
    StringMessage msg = { 0 };
    if (!file)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_FILE_PASSED_TO_FUNCTION;
    }
    else if(!file->handle)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_FILE_HANDLE_PASSED_TO_FUNCTION;
    }
    else
    {
        int64_t newSize = 256;
        msg = String_From_CountMax(newSize);
        
        int32_t startCursor = file->cursor;
        int32_t fseekCheck = fseek(file->handle, startCursor, SEEK_SET);
        
        uint8_t *fileScanResult = (uint8_t *)fgets((char *)msg.string.e, (uint32_t)msg.string.countMax, file->handle);
        msg.string.count = strlen((const char *)msg.string.e);
        if (fileScanResult == 0)
        {
            String_Destroy(&msg.string);
            msg.code = SCL_STRING_CODE__FILE_ENCOUNTERED_EOF;
            return msg;
        }
        
        int32_t endCursor = file->cursor + (int32_t)msg.string.count;
        int32_t cursorDiff = endCursor - startCursor;
        fseekCheck = fseek(file->handle, endCursor - 1, SEEK_SET);
        int32_t isLineEnd = (int32_t)fgetc(file->handle);
        int32_t isEOF = (int32_t)fgetc(file->handle);
        while(!(isLineEnd == '\n') && !(isEOF == '\0' || isEOF == EOF))
        {
            fseekCheck = fseek(file->handle, startCursor, SEEK_SET);
            newSize = msg.string.countMax * 2;
            String_Destroy(&msg.string);
            msg = String_From_CountMax(newSize);
            fileScanResult = (uint8_t *)fgets((char *)msg.string.e, (int32_t)msg.string.countMax, file->handle);
            msg.string.count = strlen((const char *)msg.string.e);
            endCursor = file->cursor + (int32_t)msg.string.count;
            cursorDiff = endCursor - startCursor;
            fseekCheck = fseek(file->handle, endCursor - 1, SEEK_SET);
            isLineEnd = (int32_t)fgetc(file->handle);
            isEOF = (int32_t)fgetc(file->handle);
        }
        
        file->cursor += (int32_t)msg.string.count;
        
        if(*(String_Get_Last(&msg.string)).chPtr == '\n')
        {
            *(String_Get_Last(&msg.string)).chPtr = '\0';
            msg.string.count--;
        }
    }
    
    return msg;
}

static void String_Internal_ExtractStringFromMessage(String *dst, StringMessage* msg)
{
    *dst = msg->string;
    msg->string = (String) { 0 };
}

static StringMessage String_Reinit_CountMax(String *string, int64_t countMax)
{
    StringMessage msg = String_Destroy(string);
    if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
    {
        msg = String_From_CountMax(countMax);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            String_Internal_ExtractStringFromMessage(string, &msg);
        }
    }
    return msg;
}

static StringMessage String_Reinit_CStr(String *string, uint8_t *cStr)
{
    StringMessage msg = String_Destroy(string);
    if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
    {
        msg = String_From_CStr((const char *)cStr);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            String_Internal_ExtractStringFromMessage(string, &msg);
        }
    }
    return msg;
}

static StringMessage String_Reinit_String(String *string, String *otherString)
{
    StringMessage msg = String_Destroy(string);
    if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
    {
        msg = String_From_String(otherString);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            String_Internal_ExtractStringFromMessage(string, &msg);
        }
    }
    return msg;
}

static StringMessage String_Reinit_FileNextLine(String *string, File *file)
{
    StringMessage msg = String_Destroy(string);
    if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
    {
        msg = String_From_FileNextLine(file);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            String_Internal_ExtractStringFromMessage(string, &msg);
        }
    }
    return msg;
}

static StringMessage String_Clear(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        Mem_ClearBytes(string->e, string->countMax * sizeof(string->e[0]));
        string->count = 0;
    }
    return msg;
}

static StringMessage String_Resize(String *string, int64_t newCountMax)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (newCountMax < 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_INVALID_STRING_COUNT_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg = String_Reinit_CountMax(string, newCountMax);
    }
    else if (newCountMax <= string->countMax)
    {
        string->countMax = newCountMax;
        string->count = min(string->count, string->countMax);
        string->e[string->count] = '\0';
    }
    else
    {
        msg = String_Internal_CopyStringIntoMessage(string->e, string->count, newCountMax);
        String_Destroy(string);
        String_Internal_ExtractStringFromMessage(string, &msg);
    }
    return msg;
}

static StringMessage String_IsEmpty(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.int64Val = (int64_t)(string->count == 0);
    }
    return msg;
}

static StringMessage String_Insert_Generic(String *string, uint8_t *otherData, int64_t index, int64_t otherCount)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (index < 0 || index > string->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else if (otherCount <= 0)
    {
        msg.code = SCL_STRING_CODE__ERROR_INVALID_STRING_COUNT_PASSED_TO_FUNCTION;
    }
    else if (string->count + otherCount > string->countMax)
    {
        msg = String_From_CountMax(string->count + otherCount);
        memmove(msg.string.e, string->e, index);
        memmove(msg.string.e + index, otherData, otherCount);
        memmove(msg.string.e + index + otherCount, string->e + index, string->count - index);
        msg.string.count = msg.string.countMax;
        String_Destroy(string);
        String_Internal_ExtractStringFromMessage(string, &msg);
    }
    else
    {
        memmove(string->e + index + otherCount, string->e + index, string->count - index);
        memmove(string->e + index, otherData, otherCount);
        string->count += otherCount;
    }
    return msg;
}

static StringMessage String_Insert_CStr(String *string, uint8_t *cStr, int64_t index)
{
    StringMessage msg = { 0 };
    if (!cStr)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Insert_Generic(string, cStr, index, strlen((const char *)cStr));
    }
    return msg;
}

static StringMessage String_Insert_uint8_t(String *string, uint8_t ch, int64_t index)
{
    StringMessage msg = { 0 };
    if (ch == '\0')
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_CHARACTER_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Insert_Generic(string, &ch, index, 1);
    }
    return msg;
}

static StringMessage String_Insert_String(String *string, String *otherString, int64_t index)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Insert_Generic(string, otherString->e, index, otherString->count);
    }
    return msg;
}

static StringMessage String_Append_Generic(String *string, uint8_t *otherData, int64_t otherCount)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Insert_Generic(string, otherData, string->count, otherCount);
    }
    return msg;
}

static StringMessage String_Append_CStr(String *string, uint8_t *cStr)
{
    StringMessage msg = { 0 };
    if (!cStr)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Append_Generic(string, cStr, strlen((const char *)cStr));
    }
    return msg;
}

static StringMessage String_Append_uint8_t(String *string, uint8_t ch)
{
    StringMessage msg = { 0 };
    if (ch == '\0')
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_CHARACTER_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Append_Generic(string, &ch, 1);
    }
    return msg;
}

static StringMessage String_Append_String(String *string, String *otherString)
{
    StringMessage msg = { 0 };
    if (!otherString)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!otherString->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Append_Generic(string, otherString->e, otherString->count);
    }
    return msg;
}

static StringMessage String_Compare(String *stringA, String *stringB)
{
    StringMessage msg = { 0 };
    if (!stringA || !stringB)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if(!stringA->e || !stringB->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        int32_t compVal = strcmp((const char *)stringA->e, (const char *)stringB->e);
        if (compVal == 0)
        {
            msg.code = SCL_STRING_CODE__COMPARE_EQUAL;
        }
        else if(compVal < 0)
        {
            msg.code = SCL_STRING_CODE__COMPARE_LESS_THAN;
        }
        else
        {
            msg.code = SCL_STRING_CODE__COMPARE_GREATER_THAN;
        }
    }
    return msg;
}

static StringMessage int64_t_From_String(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.int64Val = atoi((const char *)string->e);
        StringMessage tempIntValString = String_From_CountMax(string->count);
        sprintf((char *)tempIntValString.string.e, "%lld", msg.int64Val);
        if (String_Compare(&tempIntValString.string, string).code == SCL_STRING_CODE__COMPARE_EQUAL)
        {
            msg.code = SCL_STRING_CODE__NO_MESSAGE;
        }
        else
        {
            msg.code = SCL_STRING_CODE__ERROR_CANT_CONVERT_STRING_TO_int64_t;
        }
        String_Destroy(&tempIntValString.string);
    }
    return msg;
}

// TODO(s0lly): Reorder elses
static StringMessage double_From_String(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        msg.doubleVal = atof((const char *)string->e);
        
        int32_t encounteredDot = 0;
        int32_t countDots = 0;
        int32_t countNumsBeforeDot = 0;
        int32_t countNumsAfterDot = 0;
        int32_t encounteredMinusFirst = 0;
        int32_t countMinus = 0;
        int32_t countOther = 0;
        
        int64_t segmentIndex = 0;
        while (segmentIndex < string->count)
        {
            uint8_t currentChar = string->e[segmentIndex];
            if (currentChar == '.')
            {
                countDots++;
                encounteredDot = 1;
            }
            else if (currentChar >= '0' && currentChar <= '9')
            {
                if (encounteredDot == 0)
                {
                    countNumsBeforeDot++;
                }
                else
                {
                    countNumsAfterDot++;
                }
            }
            else if (currentChar == '-')
            {
                countMinus++;
                if (segmentIndex == 0)
                {
                    encounteredMinusFirst = 1;
                }
            }
            else
            {
                countOther++;
            }
            
            segmentIndex++;
        }
        
        if (!(countDots == 1 && countOther == 0 &&
              countNumsBeforeDot >= 0 && countNumsAfterDot >= 0 &&
              countNumsBeforeDot + countNumsAfterDot > 0 &&
              ((countMinus == 0 && !encounteredMinusFirst) || (countMinus == 1 && encounteredMinusFirst))))
            // Must be in format [dddd].[dddd] i.e. numbers either or both before and after '.',
            //     but at least one numeric value somewhere
        {
            msg.code = SCL_STRING_CODE__ERROR_CANT_CONVERT_STRING_TO_double;
        }
    }
    return msg;
}

static StringMessage String_Remove(String *string, int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStartInclusive < 0 || indexStartInclusive >= string->count ||
             indexEndInclusive < 0 || indexEndInclusive >= string->count ||
             indexStartInclusive > indexEndInclusive)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        int64_t originalCount = string->count;
        memmove(string->e + indexStartInclusive, string->e + indexEndInclusive + 1, string->count - (indexEndInclusive + 1));
        string->count = string->count - (indexEndInclusive - indexStartInclusive + 1);
        Mem_ClearBytes(string->e + string->count, originalCount - string->count);
    }
    return msg;
}

static StringMessage String_Remove_WhitespacePrecending(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        int64_t indexFirstNonWhitespace = -1;
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            if (*String_Get(string, strIndex).chPtr != ' ')
            {
                indexFirstNonWhitespace = strIndex;
                break;
            }
        }
        
        if (indexFirstNonWhitespace > 0)
        {
            memmove(string->e, string->e + indexFirstNonWhitespace, string->count - indexFirstNonWhitespace);
            int64_t originalCount = string->count;
            string->count = string->count - indexFirstNonWhitespace;
            Mem_ClearBytes(string->e + string->count, originalCount - string->count);
        }
        else if (indexFirstNonWhitespace == -1)
        {
            int64_t originalCount = string->count;
            string->count = 0;
            Mem_ClearBytes(string->e, originalCount);
        }
    }
    return msg;
}

static StringMessage String_Remove_WhitespaceFollowing(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        int64_t indexLastNonWhitespace = -1;
        for (int64_t strIndex = string->count - 1; strIndex >= 0; strIndex--)
        {
            if (*String_Get(string, strIndex).chPtr != ' ')
            {
                indexLastNonWhitespace = strIndex;
                break;
            }
        }
        
        if (indexLastNonWhitespace >= 0 && indexLastNonWhitespace < string->count - 1)
        {
            int64_t originalCount = string->count;
            string->count = indexLastNonWhitespace + 1;
            Mem_ClearBytes(string->e + string->count, originalCount - string->count);
        }
        else if (indexLastNonWhitespace == -1)
        {
            int64_t originalCount = string->count;
            string->count = 0;
            Mem_ClearBytes(string->e, originalCount);
        }
    }
    return msg;
}

static StringMessage String_Remove_WhitespaceSurrounding(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        String_Remove_WhitespacePrecending(string);
        String_Remove_WhitespaceFollowing(string);
    }
    return msg;
}

static StringMessage String_ToUpper(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            uint8_t *currentCh = String_Get(string, strIndex).chPtr;
            *currentCh = (uint8_t)toupper(*currentCh);
        }
    }
    return msg;
}

static StringMessage String_ToLower(String *string)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else
    {
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            uint8_t *currentCh = String_Get(string, strIndex).chPtr;
            *currentCh = (uint8_t)tolower(*currentCh);
        }
    }
    return msg;
}

static StringMessage String_Find_FirstFrom(String *within, String *toFind, int64_t indexStart)
{
    StringMessage msg = { 0 };
    if (!within || !toFind)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!within->e || !toFind->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStart < 0 || indexStart >= within->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        uint8_t *foundPtr = (uint8_t *)strstr((char *)within->e + indexStart, (char *)toFind->e);
        if (!foundPtr)
        {
            msg.code = SCL_STRING_CODE__FIND_NO_MATCH;
        }
        else
        {
            msg.int64Val = (int64_t)(foundPtr - within->e);
        }
    }
    return msg;
}

static StringMessage String_Find_LastFrom(String *within, String *toFind, int64_t indexStart)
{
    StringMessage msg = { 0 };
    if (!within || !toFind)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!within->e || !toFind->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStart < 0 || indexStart >= within->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        int64_t indexStartNew = (int64_t)indexStart;
        StringMessage findMsg = String_Find_FirstFrom(within, toFind, indexStartNew);
        
        if (findMsg.code ==  SCL_STRING_CODE__FIND_NO_MATCH)
        {
            msg.code = SCL_STRING_CODE__FIND_NO_MATCH;
        }
        else
        {
            while(indexStartNew < within->count &&
                  (findMsg.code != SCL_STRING_CODE__FIND_NO_MATCH))
            {
                msg.int64Val = findMsg.int64Val;
                indexStartNew = findMsg.int64Val + 1;
                findMsg = String_Find_FirstFrom(within, toFind, indexStartNew);
            }
        }
    }
    return msg;
}

static StringMessage String_Replace(String *string, String *newContents,
                                    int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    StringMessage msg = { 0 };
    if (!string)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStartInclusive < 0 || indexStartInclusive >= string->count ||
             indexEndInclusive < 0 || indexEndInclusive >= string->count ||
             indexStartInclusive > indexEndInclusive)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Remove(string, indexStartInclusive, indexEndInclusive);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            msg = String_Insert_String(string, newContents, indexStartInclusive);
        }
    }
    return msg;
}

static StringMessage String_FindReplaceFrom(String *string, String *oldContents, String *newContents, int64_t indexStart)
{
    StringMessage msg = { 0 };
    if (!string || !oldContents || !newContents)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e || !oldContents->e || !newContents->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStart < 0 || indexStart >= string->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Find_FirstFrom(string, oldContents, indexStart);
        if (msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            int64_t firstFind = msg.int64Val;
            msg = String_Replace(string, newContents, firstFind, firstFind + oldContents->count - 1);
        }
    }
    return msg;
}

static StringMessage String_FindReplaceFrom_All(String *string, String *oldContents, String *newContents,
                                                int64_t indexStart)
{
    StringMessage msg = { 0 };
    if (!string || !oldContents || !newContents)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_STRING_PASSED_TO_FUNCTION;
    }
    else if (!string->e || !oldContents->e || !newContents->e)
    {
        msg.code = SCL_STRING_CODE__ERROR_NULL_DATA_PASSED_TO_FUNCTION;
    }
    else if (indexStart < 0 || indexStart >= string->count)
    {
        msg.code = SCL_STRING_CODE__ERROR_OUT_OF_RANGE_INDEX_PASSED_TO_FUNCTION;
    }
    else
    {
        msg = String_Find_FirstFrom(string, oldContents, indexStart);
        while(msg.code == SCL_STRING_CODE__NO_MESSAGE)
        {
            int64_t firstFind = msg.int64Val;
            msg = String_Replace(string, newContents, firstFind, firstFind + oldContents->count - 1);
        }
    }
    return msg;
}


// NOTE(s0lly): StringList functions

static String *StringList_Get(StringList *stringList, int64_t index)
{
    String *result = 0;
    if (stringList && stringList->e && index >= 0 && index < stringList->count)
    {
        result = &stringList->e[index];
    }
    return result;
}

static void StringList_Destroy(StringList *stringList)
{
    if (stringList)
    {
        if (stringList->e)
        {
            for (int64_t stringIndex = 0; stringIndex < stringList->count; stringIndex++)
            {
                String *currentString = StringList_Get(stringList, stringIndex);
                if (currentString && currentString->e)
                {
                    String_Destroy(currentString);
                }
            }
            free(stringList->e);
        }
        *stringList = (StringList) { 0 }; 
    }
}

static StringList StringList_From_CountMax(int64_t countMax)
{
    StringList result = { 0 };
    if (countMax > 0)
    {
        result.countMax = countMax;
        result.e = calloc(result.countMax, sizeof(String));
        
    }
    return result;
}

static void StringList_Resize(StringList *stringList, int64_t countMaxNew)
{
    if (countMaxNew > 0)
    {
        if (stringList && stringList->e)
        {
            StringList result = { 0 };
            result = StringList_From_CountMax(countMaxNew);
            for (int64_t stringIndex = 0; stringIndex < min(countMaxNew, stringList->count); stringIndex++)
            {
                result.e[stringIndex] = String_From_String(StringList_Get(stringList, stringIndex)).string;
            }
            result.count = stringList->count;
            StringList_Destroy(stringList);
            *stringList = result;
        }
        else
        {
            *stringList = StringList_From_CountMax(countMaxNew);
        }
    }
    else
    {
        StringList_Destroy(stringList);
    }
}

static void StringList_PushCopy(StringList *stringList, String *string)
{
    if (stringList && string)
    {
        if (stringList->e)
        {
            if (stringList->count >= stringList->countMax)
            {
                StringList_Resize(stringList, stringList->countMax * 2);
            }
        }
        else
        {
            *stringList = StringList_From_CountMax(1);
        }
        
        stringList->e[stringList->count] = String_From_String(string).string;
        stringList->count++;
    }
}

static StringList StringList_From_String_SplitByDelimiters(String *string, String *delimiters, String *ignoreChs)
{
    StringList result = { 0 };
    if (string && string->e && delimiters && delimiters->e && ignoreChs && ignoreChs->e)
    {
        result = StringList_From_CountMax(1);
        
        int64_t cursorIndex = 0;
        int64_t dataCounter = 0;
        
        while(cursorIndex <= string->count)
        {
            int64_t startCellIndex = cursorIndex;
            int64_t endCellIndex = -1;
            int32_t isEmptyCell = 0;
            int32_t isInIgnorableRegion = 0;
            int64_t ignoreCharCounter = 0;
            
            
            for (int64_t i = 0; i < ignoreChs->count; i++)
            {
                if (string->e[cursorIndex] == ignoreChs->e[i])
                {
                    isInIgnorableRegion = 1;
                    ignoreCharCounter++;
                    cursorIndex++;
                }
            }
            
            // TODO(s0lly): Need to think through how to handle all ignorables
            while(cursorIndex <= string->count && endCellIndex == -1 && !isEmptyCell)
            {
                uint8_t *currentChar = &string->e[cursorIndex]; 
                
                for (int64_t i = 0; i < delimiters->count; i++)
                {
                    if ((*currentChar == delimiters->e[i]) && ignoreCharCounter == 0)
                    {
                        if (cursorIndex == startCellIndex)
                        {
                            isEmptyCell = 1;
                        }
                        endCellIndex = cursorIndex - 1;
                    }
                }
                
                for (int64_t i = 0; i < ignoreChs->count; i++)
                {
                    if (*currentChar == ignoreChs->e[i])
                    {
                        ignoreCharCounter = 1 - ignoreCharCounter;
                    }
                }
                
                cursorIndex++;
            }
            
            if (endCellIndex == -1 && !isEmptyCell)
            {
                endCellIndex = string->countMax - 1;
            }
            
            if (cursorIndex > (int64_t)strlen((const char *)string->e) && endCellIndex == -1)
            {
                String tempString = String_From_CStr("").string;
                StringList_PushCopy(&result, &tempString);
                String_Destroy(&tempString);
                break;
            }
            
            if (endCellIndex != -1 && !isEmptyCell)
            {
                if (isInIgnorableRegion)
                {
                    startCellIndex ++;
                    endCellIndex--;
                }
                
                int64_t destIndex = 0;
                int64_t srcIndex = startCellIndex;
                
                // TODO(s0lly): hits out of bounds on end of string - fix
                uint8_t originalDelimited = string->e[endCellIndex + 1];
                string->e[endCellIndex + 1] = 0;
                
                ignoreCharCounter = 0;
                
                // TODO(s0lly): allow for larger sizes?
                uint8_t *segment = calloc(4096, sizeof(uint8_t)); 
                
                while(srcIndex <= endCellIndex)
                {
                    if (string->e[srcIndex] == '\"')
                    {
                        if (ignoreCharCounter == 0)
                        {
                            ignoreCharCounter++;
                        }
                        else
                        {
                            segment[destIndex] = '\"';
                            destIndex++;
                            ignoreCharCounter = 0;
                        }
                    }
                    else
                    {
                        segment[destIndex] = string->e[srcIndex];
                        destIndex++;
                    }
                    
                    srcIndex++;
                    
                }
                
                String tempString = String_From_CStr((const char *)segment).string;
                StringList_PushCopy(&result, &tempString);
                String_Destroy(&tempString);
                
                string->e[endCellIndex + 1] = originalDelimited;
                startCellIndex = cursorIndex;
            }
            else
            {
                String tempString = String_From_CStr("").string;
                StringList_PushCopy(&result, &tempString);
                String_Destroy(&tempString);
                
                startCellIndex = cursorIndex;
            }
            
            dataCounter++;
        }
    }
    return result;
}

static StringList StringList_From_File(File *file)
{
    StringList result = { 0 };
    
    if (file && file->handle)
    {
        String fileStr = String_From_FileNextLine(file).string;
        
        while(fileStr.e)
        {
            StringList_PushCopy(&result, &fileStr);
            String_Reinit_FileNextLine(&fileStr, file);
        }
        String_Destroy(&fileStr);
    }
    
    return result;
}

static StringList StringList_From_Filename_String(String *filename)
{
    StringList result = { 0 };
    
    if (filename && filename->e)
    {
        File file = { 0 };
        file.handle = fopen((const char *)filename->e, "rb");
        if (file.handle)
        {
            result = StringList_From_File(&file);
        }
        
        fclose(file.handle);
    }
    
    return result;
}

static StringList StringList_From_Filename_CStr(const char *cStr)
{
    StringList result = { 0 };
    
    if (cStr)
    {
        String filename = String_From_CStr(cStr).string;
        result = StringList_From_Filename_String(&filename);
        String_Destroy(&filename);
    }
    
    return result;
}


// NOTE(s0lly): Undefines

#undef Mem_ClearBytes
#undef _CRT_SECURE_NO_WARNINGS


// NOTE(s0lly): License information

/*********************************************************************************************************
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*********************************************************************************************************/
