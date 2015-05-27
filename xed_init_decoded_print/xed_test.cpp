extern "C" {
#include "xed-interface.h"
}
#include <iostream>
#include <cstdlib>

using namespace std;

void abort_function(const char *msg, const char *file, int line, void *other)
{
	cerr << "================abort_function" << endl
		 << "msg: " << msg << endl
		 << "file: " << file << endl;
}

const xed_uint8_t* read_elf()
{
	xed_uint8_t* itext = (xed_uint8_t*)calloc(15, sizeof(xed_uint8_t));
	itext[0] = 0xe8;
	itext[1] = 0xb6;
	itext[2] = 0x03;
	return itext;
}

int disassembly_callback(xed_uint64_t address, char *symbol_buffer, xed_uint32_t buffer_length, xed_uint64_t *offset, void *context)
{
	cout << "==============my own printer" << endl
		 << "addr: 0x" << hex << address << endl
		 << "sym_buf: " << symbol_buffer << endl
		 << "buf_len: " << dec << buffer_length <<endl
		 << offset << endl;
}

int main()
{
	//注册断言终止函数
	xed_register_abort_function(abort_function, 0);

	//xed的初始化
	xed_state_t xed_state;
	xed_state_zero(&xed_state);	//清空状态
	xed_state_init2(&xed_state, XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b);		//初始化状态
	char state_buf[50];
	xed_state_print(&xed_state, state_buf, 50);
	cout << "============state" << endl << state_buf << endl;
	xed_tables_init();		//初始化编码解码表
	const xed_inst_t* xed_table_base = xed_inst_table_base();	//返回指令表基址

	//解码
	//初始化
	xed_decoded_inst_t xed_decoded_inst;
	xed_decoded_inst_zero_set_mode(&xed_decoded_inst, &xed_state);
	//反汇编
	const xed_uint8_t* itext = read_elf();
	xed_decode(&xed_decoded_inst, itext, 15);
	//打印
	char decoded_buf[700] = "";
	xed_decoded_inst_dump(&xed_decoded_inst, decoded_buf, 699);
	cout << "==============xed_decoded_inst_dump" << endl << decoded_buf << endl;
	xed_decoded_inst_dump_xed_format(&xed_decoded_inst, decoded_buf, 699, 0x80495a5);
	cout << "==============xed_decoded_inst_dump_xed_format" << endl << decoded_buf << endl;
	xed_format_context(XED_SYNTAX_ATT, &xed_decoded_inst, decoded_buf, 300, 0x80495a5, 0, disassembly_callback);
	cout << "==============xed_format_context" << endl << decoded_buf << endl;
	//第4种打印函数
	//首先设置xed_format_options_t
	xed_format_options_t xed_format_options = {
		1,			//.hex_address_before_symbolic_name, 打印分支目标带符号
		0,			//.xml_a, Intel语法xml格式输出
		0,			//.xml_f, xml格式的标志,大概与上一同设置
		1,			//.omit_unit_scale, 内存单元单位?
		1,			//.no_sign_extend_signed_immediates, 不进行符号扩展
		0			//.write_mask_curly_k0
	};
	xed_format_set_options(xed_format_options);		//被xed_print_info_t使用
	//设置xed_print_info_t
	xed_print_info_t xed_print_info;
	xed_init_print_info(&xed_print_info);	//初始化,就是清零
	xed_print_info.blen = 699;				//输出缓冲区,必须>25
	xed_print_info.buf = decoded_buf;
	xed_print_info.context = 0;			//传递给回调函数的参数
	xed_print_info.disassembly_callback = disassembly_callback;
	xed_print_info.format_options = xed_format_options;
	xed_print_info.format_options_valid = 1;		//为1使用上行的format_options,为0使用xed_format_set_options()注册的format_options
	xed_print_info.p = &xed_decoded_inst;
	xed_print_info.runtime_address = 0x80495a5;
	xed_print_info.syntax = XED_SYNTAX_ATT;
	//开始打印
	xed_format_generic(&xed_print_info);
	cout << "==============xed_format_generic" << endl << xed_print_info.buf << endl;


	return 0;
}
