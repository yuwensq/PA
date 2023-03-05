#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
    static char *line_read = NULL;

    if (line_read)
    {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read)
    {
        add_history(line_read);
    }

    return line_read;
}

static int cmd_c(char *args)
{
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char *args)
{
    return -1;
}

static int cmd_help(char *args);

// 在这里加几个调试函数

static int cmd_si(char *args)
{
    bool parser_success = true; // 是否解析成功
    uint64_t n = 1;             // 单步执行的条数
    char *single_arg = strtok(args, " ");
    if (single_arg)
    {
        // 判断是否为纯数字
        if (strspn(single_arg, "0123456789") == strlen(single_arg))
            n = atoll(single_arg);
        else
        {
            parser_success = false;
            Log("%s\n", "请输入正整数");
        }
    }
    // 如果参数多于1个，就噶了
    if (strtok(NULL, " "))
    {
        parser_success = false;
        Log("%s\n", "si指令至多有一个参数");
    }

    if (parser_success)
        cpu_exec(n);

    return 0;
}

static int cmd_info(char *args)
{
    void isa_reg_display(void);
    char *single_arg = strtok(args, " ");
    if (single_arg)
    {
        if (!strcmp(single_arg, "r"))
            isa_reg_display();
        else if (!strcmp(single_arg, "w"))
        {
            // TODO::打印断点信息
        }
        else
            Log("%s\n", "info指令参数应为r或w");
    }
    else
    {
        Log("%s\n", "info指令参数应为r或w");
    }

    if (strtok(NULL, " "))
    {
        Log("%s\n", "info指令至多有一个参数");
    }

    return 0;
}

static int cmd_p(char *args)
{
    return 0;
}

static int cmd_x(char *args)
{
    return 0;
}

static int cmd_w(char *args)
{
    return 0;
}

static int cmd_d(char *args)
{
    return 0;
}

static struct
{
    char *name;
    char *description;
    int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},

    /* TODO: Add more commands */
    {"si", "Let the program execute N instructions in a single step and then suspend execution, When N is not given, it defaults to 1", cmd_si},
    {"info", "Print register status or watchpoint information", cmd_info},
    {"p", "Expression evaluation", cmd_p},
    {"x", "Scan memory", cmd_x},
    {"w", "Set watchpoint", cmd_w},
    {"d", "Delete watchpoint", cmd_d},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if (arg == NULL)
    {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++)
        {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    }
    else
    {
        for (i = 0; i < NR_CMD; i++)
        {
            if (strcmp(arg, cmd_table[i].name) == 0)
            {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void ui_mainloop(int is_batch_mode)
{
    if (is_batch_mode)
    {
        cmd_c(NULL);
        return;
    }

    for (char *str; (str = rl_gets()) != NULL;)
    {
        char *str_end = str + strlen(str);

        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if (cmd == NULL)
        {
            continue;
        }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end)
        {
            args = NULL;
        }

#ifdef HAS_IOE
        extern void sdl_clear_event_queue(void);
        sdl_clear_event_queue();
#endif

        int i;
        for (i = 0; i < NR_CMD; i++)
        {
            if (strcmp(cmd, cmd_table[i].name) == 0)
            {
                if (cmd_table[i].handler(args) < 0)
                {
                    return;
                }
                break;
            }
        }

        if (i == NR_CMD)
        {
            printf("Unknown command '%s'\n", cmd);
        }
    }
}
