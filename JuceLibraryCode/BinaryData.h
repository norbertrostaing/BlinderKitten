/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   colorpicker_png;
    const int            colorpicker_pngSize = 129085;

    extern const char*   crash_png;
    const int            crash_pngSize = 144198;

    extern const char*   default_blinderlayout;
    const int            default_blinderlayoutSize = 5694;

    extern const char*   icon_png;
    const int            icon_pngSize = 90807;

    extern const char*   newFileContent_olga;
    const int            newFileContent_olgaSize = 42162;

    extern const char*   newFileDefaultContent_mochi;
    const int            newFileDefaultContent_mochiSize = 99513;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
