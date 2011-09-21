# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 74 "entry.S"
.globl syscall_table_size; .type syscall_table_size, @function; .align 0; syscall_table_size:
 .long 4



.globl sys_call_table; .type sys_call_table, @function; .align 0; sys_call_table:
 .long sys_ni_syscall
 .long sys_ni_syscall
 .long sys_ni_syscall
 .long sys_ni_syscall
 .long sys_write



.globl divide_error_handler; .type divide_error_handler, @function; .align 0; divide_error_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call divide_error_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl debug_handler; .type debug_handler, @function; .align 0; debug_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call debug_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl nm1_handler; .type nm1_handler, @function; .align 0; nm1_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call nm1_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl breakpoint_handler; .type breakpoint_handler, @function; .align 0; breakpoint_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call breakpoint_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl overflow_handler; .type overflow_handler, @function; .align 0; overflow_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call overflow_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl bounds_check_handler; .type bounds_check_handler, @function; .align 0; bounds_check_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call bounds_check_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret


.globl invalid_opcode_handler; .type invalid_opcode_handler, @function; .align 0; invalid_opcode_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call invalid_opcode_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl devive_not_available_handler; .type devive_not_available_handler, @function; .align 0; devive_not_available_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call device_not_available_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl double_fault_handler; .type double_fault_handler, @function; .align 0; double_fault_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call double_fault_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl coprocessor_segment_overrun_handler; .type coprocessor_segment_overrun_handler, @function; .align 0; coprocessor_segment_overrun_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call coprocessor_segment_overrun_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl invalid_tss_handler; .type invalid_tss_handler, @function; .align 0; invalid_tss_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call invalid_tss_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl segment_not_present_handler; .type segment_not_present_handler, @function; .align 0; segment_not_present_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call segment_not_present_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl stack_exception_handler; .type stack_exception_handler, @function; .align 0; stack_exception_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call stack_exception_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl general_protection_handler; .type general_protection_handler, @function; .align 0; general_protection_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call general_protection_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl page_fault_handler; .type page_fault_handler, @function; .align 0; page_fault_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call page_fault_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret

.globl intel_reserved_handler; .type intel_reserved_handler, @function; .align 0; intel_reserved_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call intel_reserved_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl floatin_point_error_handler; .type floatin_point_error_handler, @function; .align 0; floatin_point_error_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call floatin_point_error_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl alignment_check_handler; .type alignment_check_handler, @function; .align 0; alignment_check_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call alignment_check_routine
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 addl $4, %esp
 iret



.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call clock_routine
        movb $0x20, %al; outb %al, $0x20
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret

.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 call keyboard_routine
 movb $0x20, %al; outb %al, $0x20
 movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs
 iret


.globl system_call; .type system_call, @function; .align 0; system_call:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es


 movl $0, %ebx
 cmp %eax, %ebx
 jb err
 movl syscall_table_size, %ebx
 cmp %ebx, %eax
 jb err

 call *sys_call_table(, %eax, 0x04)
 pushl %eax
 jmp end

end: movl $0x2B, %edx; movl %edx, %ds; movl %edx, %es; popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs

 popl %eax
 iret

err: pushl $-1
 jmp end
