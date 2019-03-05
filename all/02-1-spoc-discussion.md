# lec 3 SPOC Discussion

## **提前准备**
（请在上课前完成）


 - 完成lec3的视频学习和提交对应的在线练习
 - git pull ucore_os_lab, v9_cpu, os_course_spoc_exercises  　in github repos。这样可以在本机上完成课堂练习。
 - 仔细观察自己使用的计算机的启动过程和linux/ucore操作系统运行后的情况。搜索“80386　开机　启动”
 - 了解控制流，异常控制流，函数调用,中断，异常(故障)，系统调用（陷阱）,切换，用户态（用户模式），内核态（内核模式）等基本概念。思考一下这些基本概念在linux, ucore, v9-cpu中的os*.c中是如何具体体现的。
 - 思考为什么操作系统需要处理中断，异常，系统调用。这些是必须要有的吗？有哪些好处？有哪些不好的地方？
 - 了解在PC机上有啥中断和异常。搜索“80386　中断　异常”
 - 安装好ucore实验环境，能够编译运行lab8的answer
 - 了解Linux和ucore有哪些系统调用。搜索“linux 系统调用", 搜索lab8中的syscall关键字相关内容。在linux下执行命令: ```man syscalls```
 - 会使用linux中的命令:objdump，nm，file, strace，man, 了解这些命令的用途。
 - 了解如何OS是如何实现中断，异常，或系统调用的。会使用v9-cpu的dis,xc, xem命令（包括启动参数），分析v9-cpu中的os0.c, os2.c，了解与异常，中断，系统调用相关的os设计实现。阅读v9-cpu中的cpu.md文档，了解汇编指令的类型和含义等，了解v9-cpu的细节。
 - 在piazza上就lec3学习中不理解问题进行提问。

## 第三讲 启动、中断、异常和系统调用-思考题

## 3.1 BIOS
- x86中BIOS从磁盘读入的第一个扇区是是什么内容？为什么没有直接读入操作系统内核映像？

  BIOS从磁盘读入的第一个扇区是bootloader, bootloader的作用是加载操作系统。没有读入操作系统内核映像的原因是操作系统内核映像比较大，一个扇区容纳不下，需要BIOS读取多个扇区。BIOS读取多个扇区会增加BIOS的复杂度，与BIOS的设计初衷相违背。

- 比较UEFI和BIOS的区别。

  - UEFI可以读取文件系统，从文件系统中启动操作系统；BIOS无法处理文件系统，只能从扇区读取bootloader，再用bootloader启动操作系统
  - UEFI支持图形用户界面，BIOS只有命令行
  - UEFI支持理论上无限多个主分区，BIOS只支持4个主分区

- 理解rcore中的Berkeley BootLoader (BBL)的功能。

  - 将实模式切换为保护模式
  - 读入操作系统

## 3.2 系统启动流程

- x86中分区引导扇区的结束标志是什么？

  0x55AA

- x86中在UEFI中的可信启动有什么作用？

  通过数字证书保证安全启动

- RV中BBL的启动过程大致包括哪些内容？

  - 系统上电，BIOS自检、初始化硬件
  - BIOS读取bootloader的代码
  - bootloader读取操作系统代码
  - 操作系统读取文件系统

## 3.3 中断、异常和系统调用比较
- 什么是中断、异常和系统调用？

  - 中断：来自硬件设备的处理请求

  - 异常：非法指令或其他原因导致当前指令执行失败

  - 系统调用：应用程序主动向操作系统发出的服务请求

- 中断、异常和系统调用的处理流程有什么异同？

  同：中断、异常和系统调用都是在内核进行处理，查找服务例程入口都经由中断向量表

  异：来源不同，中断来源于外部设备，异常和系统调用来源于应用程序。

  ​        响应方式不同：相对指令执行来说，中断是异步响应的，异常是同步响应的，系统调用是同步或异步响应的。

  ​        处理流程不同，中断在指令执行的间隙处理，处理完照常执行指令，中断处理不影响应用程序的指令执行；处理完异常通常操作系统会kill产生异常的程序；而应用程序会等待系统调用完成，系统调用完成后，程序继续执行。

- 以ucore/rcore lab8的answer为例，ucore的系统调用有哪些？大致的功能分类有哪些？

  - 进程管理: sys_exit, sys_fork, sys_wait, sys_exec, sys_yield, sys_kill, sys_getpid,  sys_gettime, sys_lab6_set_priority, sys_sleep
  - 文件管理: sys_open, sys_close, sys_read, sys_write, sys_seek, sys_fstat, sys_fsync, sys_getcwd, sys_getdirentry, sys_dup 
  - 外设管理: sys_putc
  - 内存管理: sys_pgdir      

## 3.4 linux系统调用分析
- 通过分析[lab1_ex0](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab1/lab1-ex0.md)了解Linux应用的系统调用编写和含义。(仅实践，不用回答)
- 通过调试[lab1_ex1](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab1/lab1-ex1.md)了解Linux应用的系统调用执行过程。(仅实践，不用回答)


## 3.5 ucore/rcore系统调用分析 （扩展练习，可选）
-  基于实验八的代码分析ucore的系统调用实现，说明指定系统调用的参数和返回值的传递方式和存放位置信息，以及内核中的系统调用功能实现函数。
- 以ucore/rcore lab8的answer为例，分析ucore 应用的系统调用编写和含义。
- 以ucore/rcore lab8的answer为例，尝试修改并运行ucore OS kernel代码，使其具有类似Linux应用工具`strace`的功能，即能够显示出应用程序发出的系统调用，从而可以分析ucore应用的系统调用执行过程。


## 3.6 请分析函数调用和系统调用的区别
- 系统调用与函数调用的区别是什么？

  - 系统调用和函数调用的汇编指令不同
  - 系统调用时发生堆栈切换和特权级切换

- 通过分析x86中函数调用规范以及`int`、`iret`、`call`和`ret`的指令准确功能和调用代码，比较x86中函数调用与系统调用的堆栈操作有什么不同？

  函数调用时，不切换堆栈，call指令向堆栈压入新的栈帧，栈帧记录未进入函数前的下一条指令地址CS和EIP，ret指令弹出新栈帧

  系统调用时，int指令从用户进程堆栈切换到内核堆栈，在内核堆栈的栈帧中不仅要记录未进入函数前的下一条指令地址CS和EIP，还要记录用户进程堆栈地址SS和ESP，iret指令根据SS和ESP从内核堆栈切换到相应的用户进程堆栈，并根据CS和EIP执行下一条指令。

- 通过分析RV中函数调用规范以及`ecall`、`eret`、`jal`和`jalr`的指令准确功能和调用代码，比较x86中函数调用与系统调用的堆栈操作有什么不同？


## 课堂实践 （在课堂上根据老师安排完成，课后不用做）
### 练习一
通过静态代码分析，举例描述ucore/rcore键盘输入中断的响应过程。

### 练习二
通过静态代码分析，举例描述ucore/rcore系统调用过程，及调用参数和返回值的传递方法。
