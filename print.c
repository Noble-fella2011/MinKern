#include "print.h"

extern void terminal_putchar(char c);

void print(const char *str)
{
    while (*str)
        terminal_putchar(*str++);

    terminal_putchar('\n');
}
