#ifndef STANDARDLOOP_SEASHELL_PROMPT_H
#define STANDARDLOOP_SEASHELL_PROMPT_H

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ANSI_STYLE_BOLD "\e[1m"
#define ANSI_STYLE_ITALICS "\e[3m"

extern void DisplayPrompt(int);

#endif
