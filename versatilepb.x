OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
	RAM(rwx) : org = 0x00000000, len = 64M
}

__ram = ORIGIN(RAM);
__eram = ORIGIN(RAM) + LENGTH(RAM);
__sys_tos = __eram;

SECTIONS {
	.text : {
		*(.vectors)
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
	}
	
	.data BLOCK(4) : {
	    __data = .;
		*(.data*)
		. = ALIGN(4);
		__edata = .;
	}
	
	.bss BLOCK(4) : {
	    __bss = .;  
		*(COMMON)
		*(.bss*)
		. = ALIGN(4);
		__ebss = .;
	}	 
}