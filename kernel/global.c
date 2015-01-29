#include "type.h"	//平台无关的基本数据类型
#include "protect.h"	//保护模式下gdt ldt等数据结构
#include "interrupt.h"	
#include "proc.h"
#define _DIYOS_GLOABL_C_HERE
#include "global.h"
/**
 * 定义全局描述符
 */
struct descriptor gdt[MAX_GDT_ITEMS];
u8	gdt_ptr[6];	//全局gdt_ptr
//这里不在c语言中重新给gdt赋值了，因为按位操作太复杂了，在汇编中使用sgdt，然后memcpy
/*
={
	//base
	{0,0,0,0,0,0,0,0,0,0,0,0,0},
//	SET_DESC(0,0,0,0,0,0),					//Base
	SET_DESC(0x0000,0xFFFF,DA_CR,DA_32,DA_LIMIT_4K,DA_DPL0), //0-4G  code segment
	SET_DESC(0x0000,0xFFFF,DA_DRW,DA_32,DA_LIMIT_4K,DA_DPL0), //0-4G data segment
	SET_DESC(0xB8000,0xFFFF,DA_DRW,DA_16,DA_LIMIT_1B,DA_DPL3), //0xB8000-0xC7FFF  for gs
};	//全局描述符表
*/
/*
 *  kernel的全局变量
 */
/*
struct descriptor_table gdt_ptr={
//	0, // 对齐
	(MAX_GDT_ITEMS-1)*8 ,	//gdt长度
	gdt,		//gdt指针
} a
;
*/	//全局描述符表
//定义中断描述符表
struct gate idt[MAX_IDT_ITEMS];
u8 idt_ptr[6];

//全局中断处理函数表
irq_handler irq_handler_table[MAX_IRQ_HANDLER_COUNT];
//全局线程表
struct process*  p_proc_ready;	//获得cpu时间的进程

struct process proc_table[MAX_PROCESS_COUNT];	//全局线程表
//TSS
struct tss g_tss;
//全局变量，判断是否在中断中(由于中断是可重入的，所以需要判断中断之前是否执行的是中断函数）
//int k_reenter = -1;	//初始值是1 ，当进入中断时+1 ，中断返回时-1,用于判断是否有重入的中断，（只允许不同种类中断重入，同种中断会在中断发生时被屏蔽掉)
int k_reenter = -1;	//由于最开始执行中断时，会先减1 ，所以这里改成0



//这里定义Task
extern void testA();
extern void testB();
extern void testC();
struct task task_table[TASKS_COUNT] = {
	/* entry        stack size        task name */
	/* -----        ----------        --------- */
	{testA,		0x80,		"TeatA"},
	{testB,		0x80,		"TestB"},
	{testC,		0x80,		"TestC"}
};



//定义任务栈空间
char task_stack[STACK_SIZE_TOTAL];



//全局时钟
//int ticks;
//这里不是类似unix时间戳，而是系统启动以来发生时钟中断的次数
//时钟中断通过init_clock设置为每隔10ms中断一次，也就是说一个进程的cpu时间片约为10ms
long long ticks;



//in main
extern sys_get_ticks();
//系统调用
system_call sys_call_table[MAX_SYSCALL_COUNT] =
{sys_get_ticks};