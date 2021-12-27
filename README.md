# SCL_String
Public domain, header-only file to simplify the C programmer's life in their interaction with strings

NOTE: This library is still under construction, and should only be used with caution.
If you do use it and uncover any bugs or modifications that would be helpful, please contact s0lly via any of the
mediums listed above.

The purpose of this header-only file is to simplify the C programmer's life in their interaction with strings.

This includes the following:

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
