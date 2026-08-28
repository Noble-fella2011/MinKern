//this is basically going to be the core stuff of MinOS, nothing fancy except for drivers, file utility, and the credits.
#include <stdint.h>
#include <stddef.h>
#include "fs.h"
#define VGA ((volatile uint16_t*)0xB8000)
#define W 80
#define H 25

static int row, col;
static uint8_t color = 0x07;

static inline uint8_t inb(uint16_t p)
{
    uint8_t v;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(v)
        : "Nd"(p)
    );

    return v;
}

static uint16_t vga(char c)
{
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void terminal_clear(void)
{
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            VGA[y * W + x] = vga(' ');

    row = 0;
    col = 0;
}

static void terminal_putchar(char c)
{
    if (c == '\n')
    {
        col = 0;

        if (row < H - 1)
            row++;

        return;
    }

    if (c == '\b')
    {
        if (col)
        {
            col--;
            VGA[row * W + col] = vga(' ');
        }

        return;
    }

    VGA[row * W + col] = vga(c);
    col++;

    if (col >= W)
    {
        col = 0;

        if (row < H - 1)
            row++;
    }
}

static void terminal_write(const char *s)
{
    while (*s)
        terminal_putchar(*s++);
}

//capitals don't work yet so I need to get them working sooner or later :sob:
//also this driver sucks, I REALLY could use a better one.
//probs not gonna tho, caps are bloat :nerd:
static const char k[] = {
    0, 27,
    '1','2','3','4','5','6','7','8','9','0','-','=',
    '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'#',
    'z','x','c','v','b','n','m',',','.','/',
    0,'*',
    0,' '
};
//thank you youtube for teaching me this bit :DDD
static int shift;

static char keyboard_getchar(void)
{
    while (!(inb(0x64) & 1))
        __asm__ volatile ("pause");

    uint8_t s = inb(0x60);

    if (s == 0x2A || s == 0x36)
    {
        shift = 1;
        return 0;
    }

    if (s == 0xAA || s == 0xB6)
    {
        shift = 0;
        return 0;
    }

    if (s & 0x80 || s >= sizeof(k))
        return 0;

    char c = k[s];

    if (shift && c >= 'a' && c <= 'z')
        c -= 'a' - 'A';
    return c;
}
static uint8_t keyboard_scancode(void)
{
    while (!(inb(0x64) & 1))
        __asm__ volatile ("pause");

    return inb(0x60);
}

static void boot_menu(void)
{
    int selected = 0;
    uint8_t s;

    terminal_clear();

    for (;;)
    {
        terminal_clear();

        terminal_write("MinKern Boot\n\n");

        if (selected == 0)
            terminal_write("> Start minKern\n");
        else
            terminal_write("  Start minKern\n");

        if (selected == 1)
            terminal_write("> Shut down\n");
        else
            terminal_write("  Shut down\n");

        while (!(inb(0x64) & 1))
            __asm__ volatile ("pause");

        s = inb(0x60);

        if (s == 0x48 && selected > 0)
            selected--;

        if (s == 0x50 && selected < 1)
            selected++;

        if (s == 0x1C)
        {
            if (selected == 0)
                return;

            if (selected == 1)
            {
                __asm__ volatile ("cli");
                for (;;)
                    terminal_clear();
terminal_write("\nIt is now safe to power off your device.\n");
                    __asm__ volatile ("hlt");
                terminal_clear();
            }
        }
    }
}

static char cmd[128];
static int n;

static int eq(const char *a, const char *b)
{
    while (*a && *b)
    {
        if (*a++ != *b++)
            return 0;
    }

    return !*a && !*b;
}

static void run(void)
{
    cmd[n] = 0;

    if (eq(cmd, "help"))
    {
        terminal_clear();
        terminal_write("\nCommands:\n");
        terminal_write("about - shows MinKern info\n");
        terminal_write("files - lists all files and folders in a directory\n");
        terminal_write("help - lists all commands\n");
        terminal_write("power off - shuts down the kernel\n");
    }

    else if (eq(cmd, "clear"))
    {
        terminal_clear();
    }

    else if (eq(cmd, "about"))
    {
        terminal_clear();
        terminal_write("\nMinKern i686 1.0.0\n");
        terminal_write("This kernel (MinKern), is barebones therefore it lacks features.\n");
        terminal_write("Check out MinOS!\n");
    }

else if (eq(cmd, "files"))
{
        terminal_clear();
    fs_list();
}

    else if (eq(cmd, "credits"))
{
    terminal_write("\nNoble/Joshua - Developer");
    terminal_write("\nMaika - Gave the name MinT and also supported me :D");
    terminal_write("\nZoe - Inspired me to do this, helped with code and supported me :D");  
    terminal_write("\nRui - My friend, supported me :D");
    terminal_write("\nBonbon/Blue - My friend, supported me :D");
    
}
    else if (eq(cmd, "power off"))
    {
        terminal_clear();
        terminal_write("\nSystem stopped.\n");
        terminal_write("\nIt is now safe to power off your device.\n");
        terminal_clear();
        __asm__ volatile ("cli");

        while (1)
            __asm__ volatile ("hlt");
    }

    else if (n)
    {
        terminal_clear();
        terminal_write("\nUnknown command\n");
    }

    n = 0;

    terminal_write("\n/min-t> ");
}

static void shell(void)
{
    terminal_write("/min-t> ");

    while (1)
    {
        char c = keyboard_getchar();

        if (!c)
            continue;

        if (c == '\n')
        {
            terminal_putchar('\n');
            run();
        }

        else if (c == '\b')
        {
            if (n)
            {
                n--;
                terminal_putchar('\b');
            }
        }

        else if (n < 127)
        {
            cmd[n++] = c;
            terminal_putchar(c);
        }
    }
}
//I lowk resent this kernel
//I hope GDT works now..
void gdt_init(void);

void kernel_main(void)
{
    terminal_clear();
    
    boot_menu();
    terminal_clear();
    terminal_write("MinKern v1.0.0 starting\n");
    terminal_write("Starting Intel GDT\n");
    gdt_init();
    terminal_write("GDT OK\n");
    terminal_write("Starting FS framework\n");
    fs_init();
    terminal_write("FS framework OK\n");
    terminal_write("Done!\n\n");
    terminal_write("Boot OK, everything has loaded!\n\n");
    terminal_write("Welcome to MinKern 1.0.0 i686!\n");
    terminal_write("Help lists all possible commands.\n\n");
    shell();

    while (1)
        __asm__ volatile ("hlt");
}
//this kernel pisses me off unimagineably.
//genuinely impressed at how it works...
//it shows ts when it boots as a dummy
