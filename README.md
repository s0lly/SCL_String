# SCL_String
Public domain, header-only file to simplify the C programmer's life in their interaction with strings

NOTE: This library is still under construction, and should only be used with caution.
If you do use it and uncover any bugs or modifications that would be helpful, please contact s0lly via any of the following mediums:

Discord Channel     : https://discord.gg/kvappxm

YouTube             : https://www.youtube.com/c/s0lly

Twitter             : https://twitter.com/s0lly


This library includes the following features:

 - A wrapper around the null-terminated C string into a relatively simple struct with associated functions that manage
the size and allocation of memory.

 - The C-style null-terminator is maintained for easy use in functions such as printf and co. 
But the user doesn't need to concern themselves with the requirement for the null-terminator:
it's all taken care of behind the scenes via the family of functions provided.
No need to remember how the various stdlib functions treat the null terminator.

 - The wrapper functions provide the functionality of the C standard library with more error detection and versatility.
For example, they account for the additional memory allocation required to store the null terminator,
and expand and contract dynamically according to whatever the user throws at them. 

 - More clearly worded function calls: however, these are more verbose, so may not be to everyone's tastes.

 - Additional functionality and clarity around file reading and conversion between strings and other base types,
and additional structs to make working with strings and their conversions that much simpler.

 - There are only two allocation (calloc) and deallocation (free) points in the entire library.
These can therefore be modified more easily to the user's own allocation methods, if desired.



Example usage code, reading in the entire contents of the SCL_String.h file, converting it to an easy-to-use list of strings (for each line of the file), and separating out the 10th line of the file into component parts.

Only the 10th line is displayed on the console, including its component parts:

```
#include "SCL_String.h"

int main(int argc, char *argv[])
{
    StringList fileStrings = { 0 };
    fileStrings = StringList_From_Filename_CStr("SCL_String.h");
    
    for (int32_t i = 0; i < fileStrings.count; i++)
    {
        String *currentString = StringList_Get(&fileStrings, i);
        // view the contents of each string of the file here
    }
    
    String *currentString = StringList_Get(&fileStrings, 9);
    printf("%s\n", currentString->e);
    
    String delimiters = String_From_CStr(":/. ");
    String ignoreChs = String_From_CStr("");
    
    StringList discordSplit = StringList_From_String_SplitByDelimiters(currentString, &delimiters, &ignoreChs);
    
    for (int32_t i = 0; i < discordSplit.count; i++)
    {
        String *currentString2 = StringList_Get(&discordSplit, i);
        printf("%s\n", currentString2->e);
    }
       
    String_Destroy(&delimiters);
    String_Destroy(&ignoreChs);
    StringList_Destroy(&fileStrings);
    StringList_Destroy(&discordSplit);
    
}
```

This prints the following to the console:

```
Discord Channel     : https://discord.gg/kvappxm
Discord
Channel






https


discord
gg
kvappxm
```
