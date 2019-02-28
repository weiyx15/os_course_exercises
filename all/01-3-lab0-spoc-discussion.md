# lec2：lab0 SPOC思考题

## **提前准备**
（请在上课前完成，option）

- 完成lec2的视频学习
- git pull ucore_os_lab, os_tutorial_lab, os_course_exercises  in github repos。这样可以在本机上完成课堂练习。
- 了解代码段，数据段，执行文件，执行文件格式，堆，栈，控制流，函数调用,函数参数传递，用户态（用户模式），内核态（内核模式）等基本概念。思考一下这些基本概念在不同操作系统（如linux, ucore,etc.)与不同硬件（如 x86, riscv, v9-cpu,etc.)中是如何相互配合来体现的。
- 安装好ucore实验环境，能够编译运行ucore labs中的源码。
- 会使用linux中的shell命令:objdump，nm，file, strace，gdb等，了解这些命令的用途。
- 会编译，运行，使用v9-cpu的dis,xc, xem命令（包括启动参数），阅读v9-cpu中的v9\-computer.md文档，了解汇编指令的类型和含义等，了解v9-cpu的细节。
- 了解基于v9-cpu的执行文件的格式和内容，以及它是如何加载到v9-cpu的内存中的。
- 在piazza上就学习中不理解问题进行提问。

---

## 思考题

- 你理解的对于类似ucore这样需要进程/虚存/文件系统的操作系统，在硬件设计上至少需要有哪些直接的支持？至少应该提供哪些功能的特权指令？

  - 进程需要时钟中断支持，需要处理时钟中断进行进程切换的特权指令
  - 虚存需要MMU支持（虚拟地址到物理地址），需要计算地址映射的特权指令
  - 文件系统需要磁盘等非易失性存储器件的支持，需要磁盘读写的特权指令

- 你理解的x86的实模式和保护模式有什么区别？物理地址、线性地址、逻辑地址的含义分别是什么？

  实模式下进程可以访问整个地址空间，保护模式支持特权级，运行在不同特权级的进程，其可以访问的地址空间的范围是不同的。

  - 物理地址是提交到总线上用于访问内存和外设的地址，每个计算机系统只有一个物理地址空间
  - 线性地址是段管理机制下形成的地址空间，每个程序拥有一个或多个段，段机制用于隔离多个程序的地址空间
  - 逻辑地址是页管理机制下形成的地址空间，每个页的大小固定，可以克服内存碎片问题。

- 你理解的risc-v的特权模式有什么区别？不同模式在地址访问方面有何特征？

  | 特权模式        | 特权级别 | 地址访问               |
  | --------------- | -------- | ---------------------- |
  | User mode       | 最低     | 每个user独立的地址空间 |
  | Supervisor mode | 中       | 操作虚拟地址           |
  | Machine mode    | 最高     | 操作物理地址           |

  

- 理解ucore中list_entry双向链表数据结构及其4个基本操作函数和ucore中一些基于它的代码实现（此题不用填写内容）

- 对于如下的代码段，请说明":"后面的数字是什么含义
```
 /* Gate descriptors for interrupts and traps */
 struct gatedesc {
    unsigned gd_off_15_0 : 16;        // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;            // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;            // reserved(should be zero I guess)
    unsigned gd_type : 4;            // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;                // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;                // Present
    unsigned gd_off_31_16 : 16;        // high bits of offset in segment
 };
```

":"后面的数字表示变量所占用的空间的比特数

- 对于如下的代码段，

```
#define SETGATE(gate, istrap, sel, off, dpl) {            \
    (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;        \
    (gate).gd_ss = (sel);                                \
    (gate).gd_args = 0;                                    \
    (gate).gd_rsv1 = 0;                                    \
    (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;    \
    (gate).gd_s = 0;                                    \
    (gate).gd_dpl = (dpl);                                \
    (gate).gd_p = 1;                                    \
    (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
}
```
如果在其他代码段中有如下语句，
```
unsigned intr;
intr=8;
SETGATE(intr, 1,2,3,0);
```
请问执行上述指令后， intr的值是多少？

```c
intr.gd_off_15_0 = 3;
intr.gd_ss = 2;
intr.gd_args = 0;
intr.gd_rsv1 = 0;
intr.gd_type = STS_TG32;
intr.gd_s = 0;
intr.gd_dpl = 0;
intr.gd_p = 1;
intr.gd_off_31_16 = 0;
```



### 课堂实践练习

#### 练习一

1. 请在ucore中找一段你认为难度适当的AT&T格式X86汇编代码，尝试解释其含义。

   In file **~/ucore_lab/labcodes/lab2/kern/mm/pmm.c**

   ```c
   asm volatile ("lgdt (%0)" :: "r" (pd));                                
   asm volatile ("movw %%ax, %%gs" :: "a" (USER_DS));
   asm volatile ("movw %%ax, %%fs" :: "a" (USER_DS));
   asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
   asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
   asm volatile ("movw %%ax, %%ss" :: "a" (KERNEL_DS));
   // reload cs
   asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
   ```

   line1: 从pd加载全局描述符

   line2-5: 用代码段选择寄存器、数据段选择寄存器和堆栈段选择寄存器赋初值

   line6: 无条件跳转到KERNEL_CS段的"1“的位置并在下一条语句生成"1"位置

2. (option)请在rcore中找一段你认为难度适当的RV汇编代码，尝试解释其含义。

#### 练习二

宏定义和引用在内核代码中很常用。请枚举ucore或rcore中宏定义的用途，并举例描述其含义。

All examples in file **~/ucore_lab/labcodes/lab2/kern/mm/mmu.h**

- 定义常量

  ```c
  #define FL_CF           0x00000001  // Carry Flag
  ```

- 变量赋值

  ```c
   86 #define SETGATE(gate, istrap, sel, off, dpl) {               \
   87         (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;      \
   88         (gate).gd_ss = (sel);                                \
   89         (gate).gd_args = 0;                                 \
   90         (gate).gd_rsv1 = 0;                                 \
   91         (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;    \
   92         (gate).gd_s = 0;                                    \
   93         (gate).gd_dpl = (dpl);                              \
   94         (gate).gd_p = 1;                                    \
   95         (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
  ```

  或

  ```c
  139 #define SEGTSS(type, base, lim, dpl)                        \                                                                                                                                           
  140     (struct segdesc) {                                      \
  141         (lim) & 0xffff, (base) & 0xffff,                    \
  142         ((base) >> 16) & 0xff, type, 0, dpl, 1,             \
  143         (unsigned) (lim) >> 16, 0, 0, 1, 0,                 \
  144         (unsigned) (base) >> 24                             \
  145     }
  ```

- 表达式

  ```c
  #define PTX(la) ((((uintptr_t)(la)) >> PTXSHIFT) & 0x3FF)       
  ```

- 嵌入汇编

  ```c
   51 #define SEG_NULL                                                \
   52     .word 0, 0;                                                 \
   53     .byte 0, 0, 0, 0
  ```

  

#### reference
 - [Intel格式和AT&T格式汇编区别](http://www.cnblogs.com/hdk1993/p/4820353.html)
 - [x86汇编指令集  ](http://hiyyp1234.blog.163.com/blog/static/67786373200981811422948/)
 - [PC Assembly Language, Paul A. Carter, November 2003.](https://pdos.csail.mit.edu/6.828/2016/readings/pcasm-book.pdf)
 - [*Intel 80386 Programmer's Reference Manual*, 1987](https://pdos.csail.mit.edu/6.828/2016/readings/i386/toc.htm)
 - [IA-32 Intel Architecture Software Developer's Manuals](http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html)
