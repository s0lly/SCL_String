
/*********************************************************************************************************
 
File Name           : SCL_String.h
Version             : v0.01
Date Last Modified  : 27.12.2021
Author              : s0lly
License             : The Unlicense / Public Domain (see end of file for license description)

Discord Channel     : https://discord.gg/kvappxm
YouTube             : https://www.youtube.com/c/s0lly
Twitter             : https://twitter.com/s0lly

NOTE: This library is still under construction, and should only be used with caution.
If you do use it and uncover any bugs or modifications that would be helpful, please contact s0lly via any of the
mediums listed above.

The purpose of this header-only file is to simplify the C programmerâ€™s life in their interaction with strings.

This includes the following:

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

*********************************************************************************************************/

// NOTE(s0lly): Careful on ftell with large files due to return type
// NOTE(s0lly): Using uint8_t as char representation - this may break code on certain platforms
// NOTE(s0lly): Library assumes ASCII style strings only
// TODO(s0lly): Ensure that whenever count reduces, "freed" space is cleared to 0 to ensure cstring code works
// TODO(s0lly): Rethink error code naming scheme
// TODO(s0lly): Add delimiter parsing code in StringList_From_String_SplitByDelimiters function


#pragma once


// NOTE(s0lly): External dependencies

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>


// NOTE(s0lly): Defines

#define Mem_ClearBytes(ptr, bytes) (memset((ptr), 0, (bytes)))


// NOTE(s0lly): Enums

typedef enum SSL_STRING_CODE
{
    SSL_STRING_CODE__NONE,
    SSL_STRING_CODE__CANT_CONVERT_STRING_TO_int64_t,
    SSL_STRING_CODE__CANT_CONVERT_STRING_TO_double,
    SSL_STRING_CODE__NULL_STRING_OR_DATA_PASSED_TO_FUNCTION,
    SSL_STRING_CODE__COMPARE_ERROR,
    SSL_STRING_CODE__COMPARE_LESS_THAN,
    SSL_STRING_CODE__COMPARE_EQUAL,
    SSL_STRING_CODE__COMPARE_GREATER_THAN,
    
} SSL_STRING_CODE;


// NOTE(s0lly): Structs

typedef struct String
{
    uint8_t *e;
    int64_t count;
    int64_t countMax;
    
} String;

typedef struct File
{
    FILE *handle;
    int64_t cursor;
    
} File;

typedef struct int64_tWithStringErrorCode
{
    int64_t val;
    SSL_STRING_CODE errorCode;
    
} int64_tWithStringErrorCode;

typedef struct doubleWithStringErrorCode
{
    double val;
    SSL_STRING_CODE errorCode;
    
} doubleWithStringErrorCode;


typedef struct StringList
{
    String *e;
    int64_t count;
    int64_t countMax;
    
} StringList;


// NOTE(s0lly): String functions

static int64_t String_Get_Count(String *string)
{
    int64_t result = -1;
    if (string && string->e)
    {
        result = string->count;
    }
    return result;
}

static int64_t String_Get_CountMax(String *string)
{
    int64_t result = -1;
    if (string && string->e)
    {
        result = string->countMax;
    }
    return result;
}

static uint8_t *String_Get_First(String *string)
{
    uint8_t *result = 0;
    if (string && string->count > 0)
    {
        result = &string->e[0];
    }
    return result;
}

static uint8_t *String_Get_Last(String *string)
{
    uint8_t *result = 0;
    if (string && string->count > 0)
    {
        result = &string->e[string->count - 1];
    }
    return result;
}

static uint8_t *String_Get(String *string, int64_t index)
{
    uint8_t *result = 0;
    if (string && string->e &&
        index >= 0 && index < string->count)
    {
        result = &string->e[index];
    }
    return result;
}

static String String_From_CountMax(int64_t countMax)
{
    String result = { 0 };
    if (countMax >= 0)
    {
        result.e = calloc(countMax + 1, sizeof(result.e[0]));
        result.countMax = countMax;
    }
    return result;
}

static String String_From_CStr(uint8_t *cStr)
{
    String result = { 0 };
    if (cStr)
    {
        int64_t newCountMax = strlen(cStr);
        result = String_From_CountMax(newCountMax);;
        memmove(result.e, cStr, newCountMax);
        result.count = newCountMax;
    }
    return result;
}

static void String_Destroy(String *string)
{
    if (string)
    {
        if (string->e)
        {
            free(string->e);
        }
        *string = (String) { 0 };
    }
}

static String String_From_String(String *otherString)
{
    String result = { 0 };
    if (otherString && otherString->e)
    {
        int64_t newCountMax = otherString->count;
        result = String_From_CountMax(newCountMax);
        memmove(result.e, otherString->e, newCountMax);
        result.count = newCountMax;
    }
    return result;
}

static String String_From_SubString(String *string, int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    String result = { 0 };
    if (string && string->e &&
        indexStartInclusive >= 0 && indexStartInclusive < string->count &&
        indexEndInclusive >= 0 && indexEndInclusive < string->count &&
        indexStartInclusive <= indexEndInclusive)
    {
        int64_t newCountMax = indexEndInclusive - indexStartInclusive + 1;
        result = String_From_CountMax(newCountMax);
        memmove(result.e, string->e + indexStartInclusive, newCountMax);
        result.count = newCountMax;
    }
    return result;
}

static String String_From_int64_t(int64_t val)
{
    String result = { 0 };
    uint8_t tempIntValString[32];
    int32_t tempCheck = sprintf_s(tempIntValString, 32, "%I64d", val);
    if (tempCheck != -1)
    {
        result = String_From_CStr(tempIntValString);
    }
    return result;
}

static String String_From_double(double val)
{
    String result = { 0 };
    uint8_t tempIntValString[32];
    int32_t tempCheck = sprintf_s(tempIntValString, 32, "%.16f", val);
    if (tempCheck != -1)
    {
        result = String_From_CStr(tempIntValString);
    }
    return result;
}

static String String_From_FileNextLine(File *file)
{
    String result = { 0 };
    
    if (file && file->handle)
    {
        int64_t newSize = 256;
        result = String_From_CountMax(newSize);
        
        int64_t startCursor = file->cursor;
        int32_t fseekCheck = fseek(file->handle, startCursor, SEEK_SET);
        
        uint8_t *fileScanResult = fgets(result.e, result.countMax, file->handle);
        result.count = strlen(result.e);
        if (fileScanResult == 0)
        {
            String_Destroy(&result);
            return result;
        }
        assert(fileScanResult);
        
        int64_t endCursor = file->cursor + result.count;
        int64_t cursorDiff = endCursor - startCursor;
        fseekCheck = fseek(file->handle, endCursor - 1, SEEK_SET);
        int32_t isLineEnd = (int32_t)fgetc(file->handle);
        int32_t isEOF = (int32_t)fgetc(file->handle);
        while(!(isLineEnd == '\n') && !(isEOF == '\0' || isEOF == EOF))
        {
            int32_t fseekCheck = fseek(file->handle, startCursor, SEEK_SET);
            newSize = result.countMax * 2;
            String_Destroy(&result);
            result = String_From_CountMax(newSize);
            fileScanResult = fgets(result.e, result.countMax, file->handle);
            result.count = strlen(result.e);
            assert(fileScanResult);
            endCursor = file->cursor + result.count;
            cursorDiff = endCursor - startCursor;
            fseekCheck = fseek(file->handle, endCursor - 1, SEEK_SET);
            isLineEnd = (int32_t)fgetc(file->handle);
            isEOF = (int32_t)fgetc(file->handle);
        }
        
        file->cursor += result.count;
        
        if(*(String_Get_Last(&result)) == '\n')
        {
            *(String_Get_Last(&result)) = '\0';
            result.count--;
        }
    }
    
    return result;
}

static void String_Reinit_CountMax(String *string, int64_t countMax)
{
    String_Destroy(string);
    if (string)
    {
        *string = String_From_CountMax(countMax);
    }
}

static void String_Reinit_CStr(String *string, uint8_t *cStr)
{
    String_Destroy(string);
    if (string)
    {
        *string = String_From_CStr(cStr);
    }
}

static void String_Reinit_String(String *string, String *otherString)
{
    String_Destroy(string);
    if (string)
    {
        *string = String_From_String(otherString);
    }
}

static void String_Reinit_FileNextLine(String *string, File *file)
{
    String_Destroy(string);
    if (string)
    {
        *string = String_From_FileNextLine(file);
    }
}

static void String_Clear(String *string)
{
    if (string && string->e)
    {
        Mem_ClearBytes(string->e, string->countMax * sizeof(string->e[0]));
        string->count = 0;
    }
}

static void String_Resize(String *string, int64_t newCountMax)
{
    if (newCountMax > 0)
    {
        if (string && string->e)
        {
            if (newCountMax >= 0 && newCountMax < string->countMax)
            {
                string->countMax = newCountMax;
                string->count = min(string->count, string->countMax);
            }
            else
            {
                String tempString = { 0 };
                tempString = String_From_CountMax(newCountMax);
                memmove(tempString.e, string->e, string->count);
                tempString.count = string->count;
                String_Destroy(string);
                *string = tempString;
            }
        }
        else
        {
            *string = String_From_CountMax(newCountMax);
        }
    }
    else
    {
        String_Destroy(string);
    }
}

static int32_t String_IsEmpty(String *string)
{
    int32_t result = -1;
    if (string && string->e)
    {
        result = (int32_t)(string->count == 0);
    }
    return result;
}

static void String_Insert_Generic(String *string, uint8_t *otherData, int64_t index, int64_t otherCount)
{
    if (string && string->e && index >= 0 && index <= string->count && otherCount > 0)
    {
        String tempResult = { 0 };
        int64_t newCountMax = string->count + otherCount;
        if (newCountMax > string->countMax)
        {
            tempResult = String_From_CountMax(newCountMax);
            memmove(tempResult.e, string->e, index);
            memmove(tempResult.e + index, otherData, otherCount);
            memmove(tempResult.e + index + otherCount, string->e + index, string->count - index);
            tempResult.count = tempResult.countMax;
            String_Destroy(string);
            *string = tempResult;
        }
        else
        {
            memmove(string->e + index + otherCount, string->e + index, string->count - index);
            memmove(string->e + index, otherData, otherCount);
            string->count += otherCount;
        }
    }
}

static void String_Insert_CStr(String *string, uint8_t *cStr, int64_t index)
{
    if (cStr)
    {
        String_Insert_Generic(string, cStr, index, strlen(cStr));
    }
}

static void String_Insert_uint8_t(String *string, uint8_t ch, int64_t index)
{
    if (ch != 0)
    {
        String_Insert_Generic(string, &ch, index, 1);
    }
}

static void String_Insert_String(String *string, String *otherString, int64_t index)
{
    if (otherString && otherString->e)
    {
        String_Insert_Generic(string, otherString->e, index, otherString->count);
    }
}

static void String_Append_Generic(String *string, uint8_t *otherData, int64_t otherCount)
{
    if (string && string->e)
    {
        String_Insert_Generic(string, otherData, string->count, otherCount);
    }
}

static void String_Append_CStr(String *string, uint8_t *cStr)
{
    if (cStr)
    {
        String_Append_Generic(string, cStr, strlen(cStr));
    }
}

static void String_Append_uint8_t(String *string, uint8_t ch)
{
    if (ch != 0)
    {
        String_Append_Generic(string, &ch, 1);
    }
}

static void String_Append_String(String *string, String *otherString)
{
    if (otherString && otherString->e)
    {
        String_Append_Generic(string, otherString->e, otherString->count);
    }
}

static int64_tWithStringErrorCode int64_t_From_String(String *string)
{
    int64_tWithStringErrorCode result = { 0 };
    if (string && string->e)
    {
        result.val = atoi(string->e);
        String tempIntValString = String_From_CountMax(string->count);
        sprintf_s(tempIntValString.e, tempIntValString.countMax + 1, "%I64d", result.val);
        if (String_Compare(&tempIntValString, string) == SSL_STRING_CODE__COMPARE_EQUAL)
        {
            result.errorCode = SSL_STRING_CODE__NONE;
        }
        else
        {
            result.errorCode = SSL_STRING_CODE__CANT_CONVERT_STRING_TO_int64_t;
        }
        String_Destroy(&tempIntValString);
    }
    else
    {
        result.errorCode = SSL_STRING_CODE__NULL_STRING_OR_DATA_PASSED_TO_FUNCTION;
    }
    return result;
}

static doubleWithStringErrorCode double_From_String(String *string)
{
    doubleWithStringErrorCode result = { 0 };
    if (string && string->e)
    {
        result.val = atof(string->e);
        
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
        
        if (countDots == 1 && countOther == 0 &&
            countNumsBeforeDot >= 0 && countNumsAfterDot >= 0 &&
            countNumsBeforeDot + countNumsAfterDot > 0 &&
            ((countMinus == 0 && !encounteredMinusFirst) || (countMinus == 1 && encounteredMinusFirst)))
            // Must be in format [dddd].[dddd] i.e. numbers either or both before and after '.',
            //     but at least one numeric value somewhere
        {
            result.errorCode = SSL_STRING_CODE__NONE;
        }
        else
        {
            result.errorCode = SSL_STRING_CODE__CANT_CONVERT_STRING_TO_double;
        }
    }
    else
    {
        result.errorCode = SSL_STRING_CODE__NULL_STRING_OR_DATA_PASSED_TO_FUNCTION;
    }
    return result;
}

static SSL_STRING_CODE String_Compare(String *stringA, String *stringB)
{
    SSL_STRING_CODE result = SSL_STRING_CODE__COMPARE_ERROR;
    if (stringA && stringA->e && stringB && stringB->e)
    {
        int32_t compVal = strcmp(stringA->e, stringB->e);
        if (compVal == 0)
        {
            result = SSL_STRING_CODE__COMPARE_EQUAL;
        }
        else if(compVal < 0)
        {
            result = SSL_STRING_CODE__COMPARE_LESS_THAN;
        }
        else
        {
            result = SSL_STRING_CODE__COMPARE_GREATER_THAN;
        }
    }
    return result;
}

static void String_Remove(String *string, int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    if (string && string->e &&
        indexStartInclusive >= 0 && indexStartInclusive < string->count &&
        indexEndInclusive >= 0 && indexEndInclusive < string->count &&
        indexStartInclusive <= indexEndInclusive)
    {
        int64_t originalCount = string->count;
        memmove(string->e + indexStartInclusive, string->e + indexEndInclusive + 1, string->count - (indexEndInclusive + 1));
        string->count = string->count - (indexEndInclusive - indexStartInclusive + 1);
        Mem_ClearBytes(string->e + string->count, originalCount - string->count);
    }
}

static void String_Remove_WhitespacePrecending(String *string)
{
    if (string && string->e && string->count > 0)
    {
        int64_t indexFirstNonWhitespace = -1;
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            if (*String_Get(string, strIndex) != ' ')
            {
                indexFirstNonWhitespace = strIndex;
                break;
            }
        }
        
        if (indexFirstNonWhitespace != 0)
        {
            if (indexFirstNonWhitespace != -1)
            {
                memmove(string->e, string->e + indexFirstNonWhitespace, string->count - indexFirstNonWhitespace);
                int64_t originalCount = string->count;
                string->count = string->count - indexFirstNonWhitespace;
                Mem_ClearBytes(string->e + string->count, originalCount - string->count);
            }
            else
            {
                int64_t originalCount = string->count;
                string->count = 0;
                Mem_ClearBytes(string->e, originalCount);
            }
        }
    }
}

static void String_Remove_WhitespaceFollowing(String *string)
{
    if (string && string->e && string->count > 0)
    {
        int64_t indexLastNonWhitespace = -1;
        for (int64_t strIndex = string->count - 1; strIndex >= 0; strIndex--)
        {
            if (*String_Get(string, strIndex) != ' ')
            {
                indexLastNonWhitespace = strIndex;
                break;
            }
        }
        
        if (indexLastNonWhitespace != string->count - 1)
        {
            if (indexLastNonWhitespace != -1)
            {
                int64_t originalCount = string->count;
                string->count = indexLastNonWhitespace + 1;
                Mem_ClearBytes(string->e + string->count, originalCount - string->count);
            }
            else
            {
                int64_t originalCount = string->count;
                string->count = 0;
                Mem_ClearBytes(string->e, originalCount);
            }
        }
    }
}

static void String_Remove_WhitespaceSurrounding(String *string)
{
    if (string && string->e)
    {
        String_Remove_WhitespacePrecending(string);
        String_Remove_WhitespaceFollowing(string);
    }
}

static void String_ToUpper(String *string)
{
    if (string && string->e)
    {
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            uint8_t *currentCh = String_Get(string, strIndex);
            *currentCh = toupper(*currentCh);
        }
    }
}

static void String_ToLower(String *string)
{
    if (string && string->e)
    {
        for (int64_t strIndex = 0; strIndex < string->count; strIndex++)
        {
            uint8_t *currentCh = String_Get(string, strIndex);
            *currentCh = tolower(*currentCh);
        }
    }
}

static int64_t String_Find_FirstFrom(String *within, String *toFind, int64_t indexStart)
{
    int64_t result = -1;
    if (within && within->e && 
        toFind && toFind->e &&
        indexStart >= 0 && indexStart < within->count)
    {
        uint8_t *foundPtr = strstr(within->e + indexStart, toFind->e);
        if (foundPtr)
        {
            result = (int64_t)(foundPtr - within->e);
        }
    }
    return result;
}

static int64_t String_Find_LastFrom(String *within, String *toFind, int64_t indexStart)
{
    int64_t result = -1;
    if (within && within->e && 
        toFind && toFind->e &&
        indexStart >= 0 && indexStart < within->count)
    {
        int64_t indexCurrentFound = -1;
        int64_t indexStartNew = (int64_t)indexStart;
        
        while(indexStartNew < within->count &&
              (indexCurrentFound = String_Find_FirstFrom(within, toFind, indexStartNew)) != -1)
        {
            result = indexCurrentFound;
            indexStartNew = indexCurrentFound + 1;
        }
    }
    return result;
}

static void String_Replace(String *string, String *newContents, int64_t indexStartInclusive, int64_t indexEndInclusive)
{
    if (string && string->e && newContents && newContents->e &&
        indexStartInclusive >= 0 && indexStartInclusive < string->count &&
        indexEndInclusive >= 0 && indexEndInclusive < string->count &&
        indexStartInclusive <= indexEndInclusive)
    {
        String_Remove(string, indexStartInclusive, indexEndInclusive);
        String_Insert_String(string, newContents, indexStartInclusive);
    }
}

static void String_FindReplaceFrom(String *string, String *oldContents, String *newContents, int64_t indexStart)
{
    if (string && string->e && 
        oldContents && oldContents->e &&
        newContents && newContents->e &&
        indexStart >= 0 && indexStart < string->count)
    {
        int64_t firstFind = String_Find_FirstFrom(string, oldContents, indexStart);
        if (firstFind != -1)
        {
            String_Replace(string, newContents, firstFind, firstFind + oldContents->count - 1);
        }
    }
}

static void String_FindReplaceFrom_All(String *string, String *oldContents, String *newContents, int64_t indexStart)
{
    if (string && string->e && 
        oldContents && oldContents->e &&
        newContents && newContents->e &&
        indexStart >= 0 && indexStart < string->count)
    {
        int64_t firstFind = -1;
        while((firstFind = String_Find_FirstFrom(string, oldContents, indexStart)) != -1)
        {
            String_Replace(string, newContents, firstFind, firstFind + oldContents->count - 1);
        }
    }
}


// NOTE(s0lly): StringList functions

static void StringList_Destroy(StringList *stringList)
{
    if (stringList)
    {
        if (stringList->e)
        {
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
            memmove(result.e, stringList->e, stringList->count * sizeof(String));
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

static String *StringList_Get(StringList *stringList, int64_t index)
{
    String *result = 0;
    if (stringList && stringList->e && index >= 0 && index < stringList->count)
    {
        result = &stringList->e[index];
    }
    return result;
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
        
        stringList->e[stringList->count] = String_From_String(string);
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
            
            int64_t strLenCheck =  string->count;
            
            // TODO(JS): Need to think through how to handle all ignorables
            while(cursorIndex <= string->count + 1 && endCellIndex == -1 && !isEmptyCell)
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
            
            if (cursorIndex > strlen(string->e) && endCellIndex == -1)
            {
                String tempString = String_From_CStr("");
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
                
                int destIndex = 0;
                int srcIndex = startCellIndex;
                
                // TODO(JS): hits out of bounds on end of string - fix
                uint8_t originalDelimited = string->e[endCellIndex + 1];
                string->e[endCellIndex + 1] = 0;
                
                ignoreCharCounter = 0;
                
                // TODO(JS): allow for larger sizes?
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
                
                size_t segmentLen = strlen(segment);
                
                String tempString = String_From_CStr(segment);
                StringList_PushCopy(&result, &tempString);
                String_Destroy(&tempString);
                
                string->e[endCellIndex + 1] = originalDelimited;
                startCellIndex = cursorIndex;
            }
            else
            {
                String tempString = String_From_CStr("");
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
        String fileStr = String_From_FileNextLine(file);
        
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
        fopen_s(&file.handle, filename->e, "rb");
        if (file.handle)
        {
            result = StringList_From_File(&file);
        }
        
        fclose(file.handle);
    }
    
    return result;
}

static StringList StringList_From_Filename_CStr(uint8_t *cStr)
{
    StringList result = { 0 };
    
    if (cStr)
    {
        String filename = String_From_CStr(cStr);
        result = StringList_From_Filename_String(&filename);
        String_Destroy(&filename);
    }
    
    return result;
}


// NOTE(s0lly): Undefines

#undef Mem_ClearBytes



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
