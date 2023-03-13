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

static bool is_digital(char *str, int base)
{
    if (base == 16)
    {
        return strlen(str) > 2 && strspn(str + 2, "0123456789abcdefABCDEF") == strlen(str + 2) && str[0] == '0' && (str[1] == 'x' || str[1] == 'X');
    }
    else
    {
        return strspn(str, "0123456789") == strlen(str);
    }
}

// len是期望最大参数个数，返回实际参数个数，会更改arg数组
static int get_args(char *args, char *arg[], int len)
{
    int cnt = 0;
    char *single_arg = strtok(args, " ");
    arg[cnt] = single_arg;
    while (single_arg)
    {
        cnt++;
        single_arg = strtok(NULL, " ");
        if (cnt < len)
            arg[cnt] = single_arg;
    }
    return cnt;
}

static int cmd_si(char *args)
{
    // 解析参数
    char *arg[1];
    int nr_arg = get_args(args, arg, 1);

    if (nr_arg > 1 || (nr_arg == 1 && !is_digital(arg[0], 10)))
    {
        Log("%s\n", "参数不合法, 指令格式为 si [N], N为正整数");
    }
    else
    {
        cpu_exec(nr_arg == 0 ? 1 : atoll(arg[0]));
    }

    return 0;
}

static int cmd_info(char *args)
{
    void isa_reg_display(void);
    void print_wp_info(void);

    char *arg[1];
    int nr_arg = get_args(args, arg, 1);

    if (nr_arg == 1 && !strcmp(arg[0], "r"))
    {
        isa_reg_display();
    }
    else if (nr_arg == 1 && !strcmp(arg[0], "w"))
    {
        // 打印断点
        print_wp_info(); 
    }
    else
    {
        Log("%s\n", "参数不合法, 指令格式为 info r/w");
    }

    return 0;
}

static int cmd_p(char *args)
{
    bool success = false;
    uint32_t result = 0;
    if (args)
        result = expr(args, &success);
    else
        success = false;

    if (!success)
    {
        Log("%s\n", "输入表达式不合法");
    }
    else
    {
        printf("%d\n", *(int *)(&result));
    }

    return 0;
}

static int cmd_x(char *args)
{
    char *arg[2];
    int nr_arg = get_args(args, arg, 2);

    if (nr_arg == 2 && is_digital(arg[0], 10) && is_digital(arg[1], 16))
    {
        int words = atoi(arg[0]); // 打印words个四字节
        uint32_t addr = strtoll(arg[1], NULL, 16);
        int i = 0;
        for (; i < words; i++)
        {
            if (i % 4 == 0)
            {
                if (i != 0)
                    printf("\n");
                printf("0x%x:\t", addr);
            }
            uint32_t data = paddr_read(addr, 4);
            printf("0x");
            int j = 0;
            for (; j < 4; j++)
            {
                printf("%02x ", (data & 0xFF));
                data >>= 8;
            }
            printf("\t");
            addr += 4;
        }
        printf("\n");
    }
    else
    {
        Log("%s\n", "参数不合法, 指令格式为 x N addr, N 为正整数, addr为16进制地址");
    }

    return 0;
}

static int cmd_w(char *args)
{
    WP* new_wp(char*);
    new_wp(args);
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
                // printf("outer:%s\n", args);
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
