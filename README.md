#自己动手自作一个操作系统
###说明
* 参考《Orange's一个操作系统的实现》、《30天自制操作系统》。

* 这个操作系统目前只能通过软盘引导，同时为了方便编程，使用fat12格式化的软盘（fat文件系统头部信息已经写入boot.asm中），使用mount -o loop,rw a.img /xxx可以挂在，并且将其他程序写入软盘；相对的，linux 加载0扇区bootsec后，接着用bootsec加载1-4扇区的setup程序，setup继续引导剩余的全部扇区到内存，但是这样写入组织好的多个文件到软盘操作起来不是很方便，所以这里借用fat12格式的文件系统。

* 为了方便使用c语言，系统内核和未来的应用程序均使用ELF格式的可执行文件，这样就可以在Linux中编写程序。

* 运行环境：bochs release 2.6.7
* bochs调试断点：1.0x7c00 boot.bin入口地址；2.0x90100 loader.bin入口地址；3.0x400 kernel.bin入口地址

###目前进度:

1. boot loader 完成
2. 微内核架构kernel，主要处理进程调度
3. 自定义系统中断函数sendrecv，用于进程间消息通信
4. 屏幕输出，完成用户级别输出（printf）
5. 键盘输入
6. 简单的文件系统，文件操作接口open close read write等
7. 内存管理，fork进程
8. tty设备归入文件系统，由文件系统接口统一管理
9. 支持创建目录，树形文件系统
10. 一些测试用例
11. 共用数据结构hashmap

###近期计划：
0. 粗粒度内核锁(只供系统进程TASK使用)
1. 软盘驱动，首先需要了解x86架构下的软盘驱动指令
2. 挂载/卸载文件系统，实现软盘数据向硬盘的拷贝
2. 通用的数据结构，hashmap 链表 红黑树
3. 动态的文件结构（现在创建文件/文件夹后会固定2048个扇区solt，不足时会浪费空间，多余时无法处理）
4. 动态内存管理 malloc free
5. 实现exec，为操作系统编写应用程序
5. 脚本（类似bash）：设计脚本语言，解析器
6. 启动时修改显示模式，进入VGA模式
7. 添加鼠标
7. GUI图形操作界面，窗口管理器

###问题：

* 操作系统调试很困难，打log的方式有时候都不可取，因为打log会操作文件系统，产生系统中断，会影响现场
* 微内核架构，所有操作都是异步的方式，编码调试非常困难,printf log追踪都不行，IO操作会改变进程执行顺序
* 目前的问题是打开stdin stdout时会死锁阻塞进程，但是当调用printk系统中断时，死锁又会解除导致问题无从查找
