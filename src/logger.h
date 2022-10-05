#pragma once

#include "defines.h"


/*enum Text_Colour{
    
    TEXT_COLOUR_WHITE,
    TEXT_COLOUR_GREEN,
    TEXT_COLOUR_YELLOW,
    TEXT_COLOUR_RED,
    TEXT_COLOUR_RED_INTENSE,
};
void platform_console_write(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
    
    //puts("\033[1;31mThis is bold red text\033[0m\n");
    cout << "\033[;32mGreen Text\033[0m\n";
    cout << "\033[4;33mYellow underlined text\033[0m\n";
    cout << "\033[;34mBlue text\033[0m\n";

}
void platform_console_write_error(const char* message, u8 colour) {
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* colour_strings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colour_strings[colour], message);
}

PINLINE void platform_log(const char * msg, Text_Colour text_colour){};

*/