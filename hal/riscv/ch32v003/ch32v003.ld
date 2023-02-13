/* Entrace Address */
ENTRY( _start )

__stack_size = 512;

/* Define _stack_size symbol, similar to global variables */
PROVIDE( _stack_size = __stack_size );

/* Memory distribution declaration
 * Define the size and starting position of Flash and RAM.
 *
 * Flash   starting address is 0x00000000 and length is 16KB
 * RAM     starting address is 0x20000000, length 2KB
 *
 * Among them, the starting position of Flash is set to virtual 0x0000000, \
 * and the MCU is mapped from 0x00000000 to 0x08000000.
 *
 * The kernel startup always starts from the 0 address, that is, PC = 0;
 * so it is possible to set the virtual address of Flash at 0x00000000 in ld.
 *
 * If you need to set the starting address of flash to the actual address 0x08000000,
 * considering that the first instruction in the startup file is j handle_reset,
 * handle_reset is a jump instruction, setting the PC as the current PC value + offset address,
 * and the range of offset address is 2^21 = 2MB = ±1MB.
 *
 * The PC = 0 at startup, and the address of handle_reset is at 0x0800xxxx,
 * far beyond the range of ±1MB.
 *
 * At this time, it is necessary to manually offset the PC and move the PC value
 * to the jump range of handle_reset ±1MB. The method is as follows:
 *
 * Add two instructions before `j handle_reset`:
 * lui t0, 0x08000  # t0 assignment 0x08000000
 * jr 8(t0)         # Jump to t0+8 = 0x08000008 position, that is, PC = 0x08000008
 *
 * The above two instructions account for 8 bytes, so the position of 0x08000008
 * happens to be the position of the j handle_reset instruction. At this time,
 * the PC value is 0x08000008, and the jump can be completed.
 *
 * In particular, when the starting address of Flash is set to the actual address,
 * that is, 0x08000000, wch-link simulation debugging is needed.
 *
 * You need to modify the wch-riscv.cfg file in the MRS installation directory.
 * Modify the file wlink_set_address 0x00000000 to wlink_set_address 0x08000000

 * The Flash and RAM space of MCU are allocated, where
 * ORIGIN defines the starting address of the address space
 * and LENGTH defines the length of the address space. */
MEMORY
{
    /* The attr here can only be composed of the following characteristics.
     *
     * 'R' - Read-only section
     * 'W' - Read/write section
     * 'X' - Executable section
     * 'A' - Allocatable section
     * 'I' - Initialized section
     * 'L' - Same as I
     * '!' - Invert the sense of any of the attributes that follow
     */
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 16K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 2K
}

/* Link distributions used to define the text, data, bss
 * and other segments of the target file (.o).
 */
SECTIONS
{
    /* Initialize the segment, the entrance of the program _start
     * is stored in the segment. */
    .init :
    {
      _sinit = .; /* Current address assignment to _sinit symbol */
      . = ALIGN(4);
      /* When the linker uses ('--gc-sections') for garbage collection,
       * KEEP() can make the content of the tagged segment unclear */
      KEEP(*(SORT_NONE(.init)))
      . = ALIGN(4);
      _einit = .;
    } >FLASH AT>FLASH

    /* Save the interrupt vector table */

    /* Code Segment */
    .text :
    {
      . = ALIGN(4);
      *(.text)
      *(.text.*)
      *(.rodata)
      *(.rodata*)
      *(.gnu.linkonce.t.*)
      . = ALIGN(4);
    } >FLASH AT>FLASH

    .fini :
    {
      KEEP(*(SORT_NONE(.fini)))
      . = ALIGN(4);
    } >FLASH AT>FLASH

    PROVIDE( _etext = . );
    PROVIDE( _eitcm = . );

    .preinit_array :
    {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array))
      PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH AT>FLASH

    .init_array :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
      KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
      PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH AT>FLASH

    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
      KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH AT>FLASH

    .ctors :
    {
      /* gcc uses crtbegin.o to find the start of
         the constructors, so we make sure it is
         first.  Because this is a wildcard, it
         doesn't matter if the user does not
         actually link against crtbegin.o; the
         linker won't look for a file to match a
         wildcard.  The wildcard also means that it
         doesn't matter which directory crtbegin.o
         is in.  */
      KEEP (*crtbegin.o(.ctors))
      KEEP (*crtbegin?.o(.ctors))
      /* We don't want to include the .ctor section from
         the crtend.o file until after the sorted ctors.
         The .ctor section from the crtend file contains the
         end of ctors marker and it must be last */
      KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
      KEEP (*(SORT(.ctors.*)))
      KEEP (*(.ctors))
    } >FLASH AT>FLASH

    .dtors :
    {
      KEEP (*crtbegin.o(.dtors))
      KEEP (*crtbegin?.o(.dtors))
      KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
      KEEP (*(SORT(.dtors.*)))
      KEEP (*(.dtors))
    } >FLASH AT>FLASH

    /* This segment defines the global variable _data_vma.
     * Because the segment saved in RAM is declared for the first time from this paragraph,
     * so the address of the _data_vma variable is the starting address of RAM 0x20000000.
     */
    .dalign :
    {
      . = ALIGN(4);
      PROVIDE(_data_vma = .);
    } >RAM AT>FLASH


    /* This paragraph defines the global variable _data_lma.
     *
     * This segment is located at the end of the previous paragraphs
     * and is used to store data stored in flash data in the data segment.
     *
     * The program runtime loads data segment data from this address into RAM.
     */
    .dlalign :
    {
      . = ALIGN(4);
      PROVIDE(_data_lma = .);
    } >FLASH AT>FLASH

    .data :
    {
      . = ALIGN(4);
      *(.gnu.linkonce.r.*)
      *(.data .data.*)
      *(.gnu.linkonce.d.*)
      . = ALIGN(8);
      PROVIDE( __global_pointer$ = . + 0x800 );
      *(.sdata .sdata.*)
      *(.sdata2*)
      *(.gnu.linkonce.s.*)
      . = ALIGN(8);
      *(.srodata.cst16)
      *(.srodata.cst8)
      *(.srodata.cst4)
      *(.srodata.cst2)
      *(.srodata .srodata.*)
      . = ALIGN(4);
      PROVIDE( _edata = .);
    } >RAM AT>FLASH

    .bss :
    {
      . = ALIGN(4);
      PROVIDE( _sbss = .);
      *(.sbss*)
      *(.gnu.linkonce.sb.*)
      *(.bss*)
      *(.gnu.linkonce.b.*)
      *(COMMON*)
      . = ALIGN(4);
      PROVIDE( _ebss = .);
    } >RAM AT>FLASH

    PROVIDE( _end = _ebss);
	PROVIDE( end = . );

    /*
     * The starting address is ORIGIN(RAM) + LENGTH(RAM) - __stack_size
     */
	.stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
	{
	    PROVIDE( _heap_end = . ); /* The end of the heap */
	    . = ALIGN(4);
	    PROVIDE(_susrstack = . ); /* Bottom of the stack */
        /* The current address is added to the __stack_size length
         * to generate __stack_size length space. */
	    . = . + __stack_size;
	    PROVIDE( _eusrstack = .); /* The top of the stack */
	} >RAM

}
