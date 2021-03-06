#include "console.h"
#include "tty.h"
#include "global.h"
#include "klib.h"
static void set_cursor(unsigned int position);
static void set_video_start_addr(u32 addr);
/**
 * 刷新
 */
static void flush(struct console *p_console);
/**
 * 判断是否是当前console
 */
int is_current_console(struct console *p_console)
{
	return (p_console == &console_table[current_console]);
}
/**
 * 输出字符到屏幕
 */
void out_char(struct console *p_console, char ch)
{
	//u8 *p_vmem = (u8*)(V_MEM_BASE + p_console->cursor * 2);
	u8 * p_vmem;
	switch(ch){
		case '\n':
			if(p_console->cursor<p_console->original_addr + p_console->v_mem_limit-SCR_WIDTH){
				//*p_vmem = ' ';
				//*(p_vmem+1) = DEFAULT_CHAR_COLOR;
				p_console->cursor = p_console->original_addr + SCR_WIDTH * ((p_console->cursor - p_console->original_addr)/SCR_WIDTH + 1);
				p_vmem = (u8*)(V_MEM_BASE + p_console->cursor*2);
				*p_vmem = ' ';
				*(p_vmem+1) = DEFAULT_CHAR_COLOR;
			}
			break;
		case '\b':
			if(p_console->cursor > p_console->original_addr){
				p_console->cursor--;
				p_vmem = (u8*)(V_MEM_BASE + p_console->cursor*2);
				*p_vmem=' ';
				*(p_vmem+1)=DEFAULT_CHAR_COLOR;
			}
			break;
		case '\t':
			if(p_console->cursor<p_console->original_addr + p_console->v_mem_limit -1){
				unsigned int tmp = ((p_console->cursor - p_console->original_addr)/SCR_WIDTH + 1) * SCR_WIDTH ;
				if(tmp - p_console->cursor >= 4){
					p_console->cursor += 4;
				} else {
					p_console->cursor = tmp;
				}
				p_vmem = (u8*)(V_MEM_BASE + p_console->cursor*2);
				*p_vmem = ' ';
				*(p_vmem + 1) = DEFAULT_CHAR_COLOR;
			}
			break;
		case '\a':
			break;	
		default:
			if(p_console->cursor<p_console->original_addr + p_console->v_mem_limit -1){
				p_vmem = (u8*)(V_MEM_BASE + p_console->cursor*2);	
				*p_vmem++ = ch;
				*p_vmem++ = DEFAULT_CHAR_COLOR;
				p_console->cursor++;
				*p_vmem = ' ';
				*(p_vmem+1) = DEFAULT_CHAR_COLOR;
			}
			break;
	}
	while(p_console->cursor >= p_console->current_start_addr + SCR_SIZE){
		scroll_screen(p_console, SCR_DN);
	}
	//set_cursor(p_console->cursor);
	flush(p_console);
}

/**
 * 设置光标位置
 */
void set_cursor(unsigned int position)
{
	_disable_int();
	_out_byte(CRTC_ADDR_REG, CURSOR_H);
	_out_byte(CRTC_DATA_REG, (position>>8)&0xFF);
	_out_byte(CRTC_ADDR_REG, CURSOR_L);
	_out_byte(CRTC_DATA_REG, position & 0xFF);
	_enable_int();
}

/**
 * @function init_screen
 * @brief 初始化屏幕
 * @param p_tty tty指针
 *
 */
void init_screen(struct tty *p_tty)
{
	int nr_tty  =  p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;
	
	int v_mem_size = V_MEM_SIZE >>1;	//显存总大小
	int con_v_mem_size = v_mem_size/CONSOLE_COUNT;
	p_tty->p_console->original_addr = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit=con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;
	//默认光标位置在开始处
	p_tty->p_console->cursor = p_tty->p_console->original_addr;
//	p_tty->p_console->cursor = 10;
	
	//out_char(p_tty->p_console, nr_tty+'0');
	//out_char(p_tty->p_console,'#');
	//out_char(p_tty->p_console,'>');
	////set_cursor(p_tty->p_console->cursor);
	flush(p_tty->p_console);
}
/**
 * 滚屏
 */
void scroll_screen(struct console *p_console, int direction)
{
	if(direction == SCR_UP){
		if(p_console->current_start_addr > p_console->original_addr){
			p_console->current_start_addr -= SCR_WIDTH;
		} 
	} else if(direction == SCR_DN){
		if(p_console->current_start_addr + SCR_SIZE < p_console->original_addr + p_console->v_mem_limit){
			p_console->current_start_addr += SCR_WIDTH;
		}
	} else {
	}
	flush(p_console);
//	set_video_start_addr(p_console->current_start_addr);
//	set_cursor(p_console->cursor);
}

/**
 * 切换控制台
 */
void select_console(int console_idx)
{
	if((console_idx<0)||(console_idx >= CONSOLE_COUNT)){
		return;
	}
	current_console = console_idx;
	flush(&console_table[console_idx]);
	//set_cursor(console_table[console_idx].cursor);
	//set_video_start_addr(console_table[console_idx].current_start_addr);
}

void set_video_start_addr(u32 addr)
{
	_disable_int();
	_out_byte(CRTC_ADDR_REG, START_ADDR_H);
	_out_byte(CRTC_DATA_REG, (addr>>8)&0xFF);
	_out_byte(CRTC_ADDR_REG, START_ADDR_L);
	_out_byte(CRTC_DATA_REG, addr & 0xFF);
	_enable_int();
}


/**
 * @function flush
 * @brief 刷新console 设置指针位置，当前显存指针地址
 * @param p_console  console指针
 *
 */
void flush(struct console *p_console){
	set_cursor(p_console->cursor);
	set_video_start_addr(p_console->current_start_addr);
}
