extern "C" {
#include "xed-interface.h"
}
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

const xed_uint8_t* read_elf()
{
	xed_uint8_t* itext = (xed_uint8_t*)malloc(0x1078c*sizeof(xed_uint8_t));
	int fd = open("/bin/ls", O_RDONLY);
	lseek(fd, 0x1cc0, SEEK_SET);
	read(fd, itext, 0x1078c);
	return itext;
}

int main()
{
	//xed的初始化
	xed_state_t xed_state;
	xed_state_zero(&xed_state);	//清空状态
	xed_state_init2(&xed_state, XED_MACHINE_MODE_LEGACY_32, XED_ADDRESS_WIDTH_32b);		//初始化状态
	xed_tables_init();		//初始化编码解码表
	const xed_inst_t* xed_table_base = xed_inst_table_base();	//返回指令表基址

	//解码
	//初始化
	xed_decoded_inst_t xed_decoded_inst;
	xed_decoded_inst_zero_set_mode(&xed_decoded_inst, &xed_state);
	//反汇编
	const xed_uint8_t* itext = read_elf();
	char decoded_buf[700] = "";
	unsigned int addr = 0x8049cc0;
	const xed_uint8_t* start = itext;
	while(1)
	{
		xed_decode(&xed_decoded_inst, itext, 15);
		//打印
		xed_format_context(XED_SYNTAX_ATT, &xed_decoded_inst, decoded_buf, 300, addr, 0, 0);
		cout << "0x" << hex << addr << "\t "<< decoded_buf << endl;
//		cout << "分支偏移: 0x" << xed_decoded_inst_get_branch_displacement(&xed_decoded_inst) << endl;	//通过判断分支偏移是否为0可知是否为分支指令
		itext += xed_decoded_inst_get_length(&xed_decoded_inst);
		if(itext>=start+0x1078c)
			return 0;
		addr += xed_decoded_inst_get_length(&xed_decoded_inst);
		xed_decoded_inst_zero_keep_mode(&xed_decoded_inst);
	}

	return 0;
}
