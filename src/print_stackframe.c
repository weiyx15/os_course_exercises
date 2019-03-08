// Created Time:    2019-03-08 09:48:30
// Modified Time:   2019-03-08 10:45:25

#include <stdio.h>
#include <execinfo.h>

#define MAX_DEPTH (20)

void func0(void);
void func1(void);
void func2(void);
void print_stackframe(void);

void print_stackframe(void)
{
    void * buffer[MAX_DEPTH];
    int depth = backtrace(buffer, MAX_DEPTH);
    char ** func_names = backtrace_symbols(buffer, depth);
    for (int i=0; i<depth; i++)
    {
        printf("Depth: %d, func name: %s\n", i, func_names[i]);
    }
}

void func0(void)
{
    func1();
}

void func1(void)
{
    func2();
}

void func2(void)
{
    print_stackframe();
}

int main(int argc, char *argv[]) {
    // print_stackframe();
    func0();
    return 0;
}

