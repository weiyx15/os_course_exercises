# 在Linux中打印函数调用栈

## 要求

在Linux系统的应用程序中写一个函数print_stackframe()，用于获取当前位置的函数调用栈信息

## 方法

execinfo.h库下的函数backtrace可以得到当前线程的函数调用栈指针和调用栈深度，backtrace_symbols可以将调用栈指针转化为字符串信息。详见[Backtraces](https://www.gnu.org/software/libc/manual/html_node/Backtraces.html)或在Linux命令行运行命令man backtrace.

## 代码

```c
 1 // Created Time:    2019-03-08 09:48:30                                               
 2 // Modified Time:   2019-03-08 10:45:25
 3  
 4 #include <stdio.h>
 5 #include <execinfo.h>
 6  
 7 #define MAX_DEPTH (20)
 8  
 9 void func0(void);
10 void func1(void);
11 void func2(void);
12 void print_stackframe(void);
13  
14 void print_stackframe(void)
15 {
16     void * buffer[MAX_DEPTH];
17     int depth = backtrace(buffer, MAX_DEPTH);
18     char ** func_names = backtrace_symbols(buffer, depth);
19     for (int i=0; i<depth; i++)
20     {
21         printf("Depth: %d, func name: %s\n", i, func_names[i]);
22     }
23 }
24  
25 void func0(void)
26 {
27     func1();
28 }
29  
30 void func1(void)
31 {
32     func2();
33 }
34  
35 void func2(void)
36 {
37     print_stackframe();
38 }
39  
40 int main(int argc, char *argv[]) {
41     // print_stackframe();
42     func0();
43     return 0;
44 }
```

## 编译运行

gcc编译时加上-rdynamic参数，通知链接器支持函数名功能（不加-rdynamic参数则无函数名打印）：

```shell
gcc print_stackframe.c -o print_stackframe -rdynamic
```

运行可执行文件：

```shell
./print_stackframe
```

结果

```shell
Depth: 0, func name: ./print_stackframe(print_stackframe+0x2e) [0x400964]
Depth: 1, func name: ./print_stackframe(func2+0x9) [0x400a15]
Depth: 2, func name: ./print_stackframe(func1+0x9) [0x400a09]
Depth: 3, func name: ./print_stackframe(func0+0x9) [0x4009fd]
Depth: 4, func name: ./print_stackframe(main+0x14) [0x400a2c]
Depth: 5, func name: /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0xf0) [0x7f7abe418830]
Depth: 6, func name: ./print_stackframe(_start+0x29) [0x400869]
```

