/**
 * Main assembly file setting up and managing TrustZone (for i.MX53 board).
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 *
 * XXX: file includes code from
 *      Dongli Zhang https://github.com/finallyjustice/imx53qsb-code "trustzone-smc"
 *      and
 *      James Walmsley https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
 **/
    
/* --- DEFINITIONS --- */
.equ AIPS1_BASE_ADDR,	0x53F00000
.equ AIPS2_BASE_ADDR,	0x63F00000
.equ CCM_BASE_ADDR,		(AIPS1_BASE_ADDR + 0x000D4000)

.equ PLL1_BASE_ADDR,    (AIPS2_BASE_ADDR + 0x00080000)
.equ PLL2_BASE_ADDR,    (AIPS2_BASE_ADDR + 0x00084000)
.equ PLL3_BASE_ADDR,    (AIPS2_BASE_ADDR + 0x00088000)
.equ PLL4_BASE_ADDR,    (AIPS2_BASE_ADDR + 0x0008c000)

.equ ARM_BASE_ADDR,		(AIPS2_BASE_ADDR + 0x000A0000)

.equ DP_OP_400,   ((8 << 4) + ((2 - 1)  << 0))
.equ DP_MFD_400,  (3 - 1)
.equ DP_MFN_400,  1

.equ DP_OP_455,   ((9 << 4) + ((2 - 1)  << 0))
.equ DP_MFD_455,  (48 - 1)
.equ DP_MFN_455,  23

.equ DP_OP_216,   ((6 << 4) + ((3 - 1)  << 0))
.equ DP_MFD_216,  (4 - 1)
.equ DP_MFN_216,  3

.equ DP_OP_800,   ((8 << 4) + ((1 - 1)  << 0))
.equ DP_MFD_800,  (3 - 1)
.equ DP_MFN_800,  1

.equ CLKCTL_CCMR, 0 /* offsetof(struct clkctl, ccr)   @ */
.equ CLKCTL_CCDR, 4 /* offsetof(struct clkctl, ccdr)  @ */
.equ CLKCTL_CSR, 8 /* offsetof(struct clkctl, csr)    @ */
.equ CLKCTL_CCSR, 12 /* offsetof(struct clkctl, ccsr) @ */
.equ CLKCTL_CACRR, 16 /* offsetof(struct clkctl, cacrr)   @ */
.equ CLKCTL_CBCDR, 20 /* offsetof(struct clkctl, cbcdr)   @ */
.equ CLKCTL_CBCMR, 24 /* offsetof(struct clkctl, cbcmr)   @ */
.equ CLKCTL_CSCMR1, 28 /* offsetof(struct clkctl, cscmr1) @ */
.equ CLKCTL_CSCMR2, 32 /* offsetof(struct clkctl, cscmr2) @ */
.equ CLKCTL_CSCDR1, 36 /* offsetof(struct clkctl, cscdr1) @ */
.equ CLKCTL_CS1CDR, 40 /* offsetof(struct clkctl, cs1cdr) @ */
.equ CLKCTL_CS2CDR, 44 /* offsetof(struct clkctl, cs2cdr) @ */
.equ CLKCTL_CDCDR, 48 /* offsetof(struct clkctl, cdcdr)   @ */
.equ CLKCTL_CHSCCDR, 52 /* offsetof(struct clkctl, chsccdr)   @ */
.equ CLKCTL_CSCDR2, 56 /* offsetof(struct clkctl, cscdr2) @ */
.equ CLKCTL_CSCDR3, 60 /* offsetof(struct clkctl, cscdr3) @ */
.equ CLKCTL_CSCDR4, 64 /* offsetof(struct clkctl, cscdr4) @ */
.equ CLKCTL_CWDR, 68 /* offsetof(struct clkctl, cwdr) @ */
.equ CLKCTL_CDHIPR, 72 /* offsetof(struct clkctl, cdhipr) @ */
.equ CLKCTL_CDCR, 76 /* offsetof(struct clkctl, cdcr) @ */
.equ CLKCTL_CTOR, 80 /* offsetof(struct clkctl, ctor) @ */
.equ CLKCTL_CLPCR, 84 /* offsetof(struct clkctl, clpcr)   @ */
.equ CLKCTL_CISR, 88 /* offsetof(struct clkctl, cisr) @ */
.equ CLKCTL_CIMR, 92 /* offsetof(struct clkctl, cimr) @ */
.equ CLKCTL_CCOSR, 96 /* offsetof(struct clkctl, ccosr)   @ */
.equ CLKCTL_CGPR, 100 /* offsetof(struct clkctl, cgpr)    @ */
.equ CLKCTL_CCGR0, 104 /* offsetof(struct clkctl, ccgr0)  @ */
.equ CLKCTL_CCGR1, 108 /* offsetof(struct clkctl, ccgr1)  @ */
.equ CLKCTL_CCGR2, 112 /* offsetof(struct clkctl, ccgr2)  @ */
.equ CLKCTL_CCGR3, 116 /* offsetof(struct clkctl, ccgr3)  @ */
.equ CLKCTL_CCGR4, 120 /* offsetof(struct clkctl, ccgr4)  @ */
.equ CLKCTL_CCGR5, 124 /* offsetof(struct clkctl, ccgr5)  @ */
.equ CLKCTL_CCGR6, 128 /* offsetof(struct clkctl, ccgr6)  @ */
.equ CLKCTL_CMEOR, 136 /* offsetof(struct clkctl, cmeor)  @ */
.equ CLKCTL_CCGR7, 132 /* offsetof(struct clkctl, ccgr7)  @ */
.equ PLL_DP_CTL, 0 /* offsetof(struct dpll, dp_ctl)   @ */
.equ PLL_DP_CONFIG, 4 /* offsetof(struct dpll, dp_config) @ */
.equ PLL_DP_OP, 8 /* offsetof(struct dpll, dp_op) @ */
.equ PLL_DP_MFD, 12 /* offsetof(struct dpll, dp_mfd)  @ */
.equ PLL_DP_MFN, 16 /* offsetof(struct dpll, dp_mfn)  @ */
.equ PLL_DP_HFS_OP, 28 /* offsetof(struct dpll, dp_hfs_op)    @ */
.equ PLL_DP_HFS_MFD, 32 /* offsetof(struct dpll, dp_hfs_mfd)  @ */
.equ PLL_DP_HFS_MFN, 36 /* offsetof(struct dpll, dp_hfs_mfn)  @ */

.equ W_DP_OP,     0
.equ W_DP_MFD,    4
.equ W_DP_MFN,    8

/* last 5 bit of Program Status Register (PSR) */
.equ Mode_USR, 0x10 @ User Mode
.equ Mode_FIQ, 0x11 @ Fast Interrupt Mode
.equ Mode_IRQ, 0x12 @ Interrupt Mode
.equ Mode_SVC, 0x13 @ Supervisor Mode
.equ Mode_ABT, 0x17 @ Abort Mode
.equ Mode_UND, 0x1B @ Undefined Mode
.equ Mode_SYS, 0x1F @ System Mode
.equ Mode_MON, 0x16 @ Monitor Mode

/* 6th and 7th bit of Program Status Register (PSR) */
.equ I_Bit,    0x80 @ IRQ interrupts disabled 
.equ F_Bit,    0x40 @ FIQ interrupts disabled

.equ NS_BIT,   0x1

/* memory reserved (in bytes) for stacks of different mode */
.equ Len_FIQ_Stack,  512
.equ Len_IRQ_Stack,  512
.equ Len_ABT_Stack,  512
.equ Len_UND_Stack,  512
.equ Len_SVC_Stack,  8192
.equ Len_USR_Stack,  8192

.equ Offset_FIQ_Stack, 0
.equ Offset_IRQ_Stack, Offset_FIQ_Stack + Len_FIQ_Stack
.equ Offset_ABT_Stack, Offset_IRQ_Stack + Len_IRQ_Stack
.equ Offset_UND_Stack, Offset_ABT_Stack + Len_ABT_Stack
.equ Offset_SVC_Stack, Offset_UND_Stack + Len_UND_Stack
.equ Offset_USR_Stack, Offset_SVC_Stack + Len_SVC_Stack

/* --- Extern Symbols --- */
.extern __bss_start
.extern __bss_end
/* FreeRTOS */
.extern vFreeRTOS_ISR
.extern vPortYieldProcessor
.extern DisableInterrupts
.extern bootmain
.extern pxCurrentTCB
.extern ulCriticalNesting
.extern vPortEnterCritical
.extern ulCriticalNesting   @ instead of vPortExitCritical
/* helpers */
.extern print_MODE
.extern print_SMC
.extern print_WORLD
/* self healing */
.extern is_currentTCB_restoration
.extern detection
/* timing */
#ifdef TIMING
    .extern stop_timer
#endif
    
.code 32 ;@ 32 = ARM; 16 = Thumb

	.text
	.global _start
	.global _ns_start
	.global _ns_end

/* --- START --- */
.align 5
_start:
    ;@ handler functions
	b reset                     ;@ processor reset handler (first instruction executed)
	ldr pc, UndefAddr           ;@ undefined instruction handler
	ldr pc, SWIAddr             ;@ software interrupt / TRAP (SVC) -- system SVC handler for switching to kernel mode
	ldr pc, PAbortAddr          ;@ Prefetch abort handler
	ldr pc, DAbortAddr          ;@ Data abort handler
	b .                         ;@ historical, now invalid address handler
	ldr pc, IRQAddr             ;@ IRQ handler
	ldr pc, FIQAddr             ;@ Fast interrupt handler

/* --- Exception Address Table --- */
;@ (reset handler already done above)
UndefAddr:		.word UndefHandler  ;@ loop
SWIAddr:        .word SWIHandler    ;@ software interrupt: calls vPortYieldProcessor on SWI 0, else own handler
PAbortAddr:     .word PAbortHandler ;@ loop
DAbortAddr:     .word DAbortHandler ;@ loop
IRQAddr:        .word vFreeRTOS_ISR ;@ IRQ: masked with TrustZone; vFreeRTOS_ISR without TrustZone
FIQAddr:        .word FIQHandler	;@ FIQ: goes to Monitor with TrustZone; goes to exceptions.c without TrustZone

@ ----------------------------------------------------------
@ Normal World Exception Vectors
@ ----------------------------------------------------------
.align 5
_ns_start:
	ldr pc, ns_Reset
	ldr pc, ns_UndefAddr
	ldr pc, ns_SWIAddr
	ldr pc, ns_PAbortAddr
	ldr pc, ns_DAbortAddr
    b .
	ldr pc, ns_IRQAddr
	b .                         ;@ FIQ goes to Monitor

/* --- Exception Address Table --- */
ns_Reset:          .word ns_ResetHandler
ns_UndefAddr:      .word ns_UndefHandler
ns_SWIAddr:        .word ns_SWIHandler
ns_PAbortAddr:     .word ns_PAbortHandler
ns_DAbortAddr:     .word ns_DAbortHandler
ns_IRQAddr:        .word ns_IRQHandler

/* --- Reset Handler --- */
.global reset
.align 4
reset:
	bl lowlevel_init
	ldr sp, =STACK_ADDR  @from kernel.ld
	ldr r0, =STACK_ADDR

    ;@ Set up the various STACK pointers for different CPU modes
    ;@ http://www.heyrick.co.uk/armwiki/The_Status_register
	cps #Mode_MON
	ldr sp, =STACK_ADDR

	msr cpsr_c, #Mode_FIQ | I_Bit | F_Bit /* Disable interrupts*/
	sub sp, r0, #Offset_FIQ_Stack
 
	msr cpsr_c, #Mode_IRQ | I_Bit | F_Bit /* Disable interrupts */
	sub     sp, r0, #Offset_IRQ_Stack
	 
	msr cpsr_c, #Mode_ABT | I_Bit | F_Bit /* Disable interrupts */
	sub sp, r0, #Offset_ABT_Stack
	
	msr cpsr_c, #Mode_UND | I_Bit | F_Bit /* Disable interrupts */
	sub sp, r0, #Offset_UND_Stack
    
	msr cpsr_c, #Mode_SVC | I_Bit | F_Bit /* Disable interrupts */
	sub sp, r0, #Offset_SVC_Stack

    /* enabling the USR stack not possible here as this would switch
    to normal world and we cannot switch back!
    But system and user mode share r13 (sp), r14 (lr), and r15 (pc)
    => setup system mode to init SP_usr
    */
    msr cpsr_c, #Mode_SYS | I_Bit | F_Bit /* Disable interrupts */
	sub sp, r0, #Offset_USR_Stack

    /* switch back to supervisor SVC mode */
	msr cpsr_c, #Mode_SVC | I_Bit | F_Bit /* Disable interrupts */
    
    ;@ zero filling of bss segment
	ldr r0, =__bss_start
	ldr r1, =__bss_end
	mov r2, #0
zero_loop:
	cmp 	r0,r1
	it		lt
	strlt	r2,[r0], #4
	blt		zero_loop

    ;@ make sure to mask all IRQs in the secure world!!!
    ;@ (implicitly done in interrupts.c by not allowing secure world to set IRQ and vice versa)
    ;@ disable interrupts before main
    mrs r0, CPSR        @ get CPSR register
    orr r0, r0, #0xC0   @ set IRQ and FIQ bit
    msr CPSR_c, r0      @ "_c" = set control bits
    mov r0, #0          @ clear r0
    
    ;@ start C execution
	bl bootmain
	b .

/* --- MONITOR --- */
.global monitor
.align 5
monitor:
	@ Monitor
	b . @ Reset         - not used by Monitor
	b . @ Undef         - not used by Monitor
	b mon_smc_handler   @ Monitor calls
	b . @ Prefetch      - aborts handled locally
	b . @ Data abort    - aborts handled locally
	b . @ RESERVED
	b . @ IRQ           - IRQ handled locally
	b mon_fiq_handler   @ save LR and call vFreeRTOS_ISR => interrupt.c => SMC => mon_smc_handler

/*
 * Monitor FIQ Handler
 */
mon_fiq_handler:
#ifdef INFO
    push {r0-r12, lr}
    ldr r0, =fiq_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
    push {lr}           @ push LR to SP_mon to make it available for mon_smc_handler later
    mrs lr, spsr        @ get SPSR (= of FIQ =task mode)
    push {lr}           @ push SPSR
    ldr lr, [sp, #4]    @ restore LR

#ifdef DEBUG
    @ debug print - SPSR
    push {r0-r12, lr}
    mrs r1, spsr
    ldr r0, =spsr_string
    bl cprintf
    pop {r0-r12, lr}
    
    @ debug print - MODE/WORLD
    push {r0-r12, lr}
    ldr r0, =col_monitor
    bl cprintf
    bl print_MODE
    ldr r0, =col_reset
    bl cprintf
    @ print world
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    and r0, r0, #NS_BIT                     @ Only return NS_BIT
    tst r0, #0
    ldreq r0, =sw_string
    ldrne r0, =nw_string
    bl cprintf
    pop {r0-r12, lr}

    @ debug print - LR
    push {r0-r12, lr}
    ldr r0, =lr_string
    mov r1, lr
    bl cprintf
    pop {r0-r12, lr}
#endif

    @ set world to secure world (otherwise b vFreeRTOS_ISR goes to normal world!)
    push {r0}
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    bic r0, r0, #NS_BIT                     @ unset NS bit
    mcr p15, 0, r0, c1, c1, 0				@ Write Secure Configuration Register data
    pop {r0}
    msr cpsr_c, #Mode_SVC | I_Bit | F_Bit   @ goto SVC mode for FreeRTOS
    clrex                                   @ clear local monitor
    b vFreeRTOS_ISR     @ does not return, lands in interrupt.c => SMC => mon_smc_handler
    ldr r0, =freertos_return_string
    bl cprintf
    SMC #13             @ safety

/* 
 * SMC Handler
 *
 * - check the SMC call
 * - execute SMC
 */
mon_smc_handler:
#ifdef DEBUG
    @ print mode
    @ ----------
    push {r0-r12, lr}
    ldr r0, =col_monitor
    bl cprintf
    bl print_MODE
    ldr r0, =col_reset
    bl cprintf

    @ print world
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    and r0, r0, #NS_BIT                     @ Only return NS_BIT
    tst r0, #0
    ldreq r0, =sw_string
    ldrne r0, =nw_string
    bl cprintf
    pop {r0-r12, lr}
#endif

    @ get SMC number
    @   (these might be all attack points of the normal world => would need strong security)
    @   SMC 0  = portRESTORE_CONTEXT    (should not be callable from normal world) IMPROVE: should change SP
    @   SMC 1  = portSAVE_CONTEXT       (should not be callable from normal world) IMPROVE: should change SP
    @   SMC 2  = SMC_is_secure_world    (callable from normal world)
    @   SMC 3  = ns_SWIHandler          (callable from normal world)
    @   ...
    @   SMC 13 = kernelpanic            (should not be callable from normal world)
    @   ...
    @   SMC 15 = (last possible call)
    @ --------------
    push {r0, lr}
    ldr r0, [lr,#-4]            @ SMC number = lower 15 bits of instruction
    and r0, r0, #15

#if defined DEBUG || defined PRINTSMC
    push {r0-r12, lr}
    push {r0}
    ldr r0, =col_monitor
    bl cprintf
    pop {r0}
    bl print_SMC                @ print r0 as unsigned int
    ldr r0, =col_reset
    bl cprintf
    pop {r0-r12, lr}
#endif

    cmp r0, #0                  @ SMC #0
    popeq {r0, lr}
    beq smc_switch_world_sw_nw

    cmp r0, #1                  @ SMC #1
    popeq {r0, lr}
    beq smc_switch_world_nw_sw

    cmp r0, #2                  @ SMC #2
    popeq {r0, lr}
    beq smc_return_world

    cmp r0, #3                  @ SMC #3
    bne next_smc
    pop  {r0, lr}               @ restore
    @ setup stack for portSAVE_CONTEXT/SMC 1 (called by freeRtos_PortYield)
    @ --------------------------------------
    add lr, lr, #4
    push {lr}                   @ push LR
    mrs  lr, spsr               @ get SPSR (= of ns_SWIHandler =task mode)
    push {lr}                   @ push SPSR
    ldr lr,[sp,#4]              @ restore LR (SP points to pushed SPSR => one before is LR)
    @ setup stack for freeRtos_PortYield
    @ ----------------------------------
    @ set world to secure world (otherwise b vPortYieldProcessor goes to normal world!)
    push {r0}
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    bic r0, r0, #NS_BIT                     @ unset NS bit
    mcr p15, 0, r0, c1, c1, 0				@ Write Secure Configuration Register data
    pop {r0}
    msr cpsr_c, #Mode_SVC | I_Bit | F_Bit   @ switch to SVC mode (for FreeRTOS)
    b   vPortYieldProcessor                 @ let FreeRTOS do the work
    ldr r0, =portyield_return_string
    bl cprintf
    SMC #13                                 @ kernel panic
    
next_smc:   
    cmp r0, #13                 @ SMC #13
    popeq {r0, lr}
    beq smc_kernel_panic

smc_default:
    pop {r0, lr}                @ SMC <default>
    clrex                       @ clear local monitor
    movs pc, lr                 @ exception return

/*
 * SMC to block system after panics
 */
smc_kernel_panic:
    msr cpsr_c, #Mode_MON | I_Bit | F_Bit   @ disable all interrupts
    ldr r0, =kernelpanic_string
    bl cprintf                              @ print kernelpanic message
    b .                                     @ block
    
/*
 * SMC to (reliably) return which world we are in
 */
smc_return_world:
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    and r0, r0, #NS_BIT                     @ Only return NS_BIT
	clrex                                   @ Clear local monitor
	movs pc, lr                             @ Perform exception return
    
/*
 * SMC debug function to print a stackframe
 */
print_my_stack:
    mov r2, r3
    sub r2, r2, #1
    lsl r2, r2, #2      @ r2 * 4
    add r1, r1, r2      @ r1 = r1 + (r3 * 4)
stack_loop:  
    cmp r3, #0
    moveq pc, lr
    sub r3, r3, #1
    ldr r0, =stack_string
    ldmfd r1, {r2}
    push {r0-r3, lr}
    bl cprintf
    pop {r0-r3, lr}
    sub r1, r1, #4
    b stack_loop
    
/*
 * SMC to switch from secure world to normal world
 */
smc_switch_world_sw_nw:
    @ SecWorld  -> NormWorld (=portRESTORE_CONTEXT) SMC 0
    
    @ save secure world
    @ -----------------
    ldr r3, =S_STACK_SP     @ load stack pointer
    ldr r2, [r3]
    stmfd r2!, {r4-r12}     @ save r4 to r12
    mrs   r0, spsr          @ get copy of SPSR
    stmfd r2!, {r0}         @ save original SPSR (do not save LR, this returns main!)
    str   r2, [r3]          @ save updated stack pointer back

    @ restore task (normal world)
    @ XXX: if altered, has to be altered below in TZ too!
    @ ---------------------------
    ldr r0, =pxCurrentTCB       @ load pxCurrentTCB (task control block) into r0
    ldr r0, [r0]            
    ldr lr, [r0]                @ load stack pointer of task

#ifdef TRUSTZONE
    @ detection routine for pxCurrentTCB
    @ ----------------------------------
    push {r0-r12, lr}
    mrs r0, SPSR
    push {r0}
    @ set: secure world SVC mode
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    bic r0, r0, #NS_BIT                     @ unset NS bit
    mcr p15, 0, r0, c1, c1, 0				@ Write Secure Configuration Register data
    mov r0, #0                              @ clear r0
    msr cpsr_c, #Mode_SVC | I_Bit | F_Bit   @ switch to SVC mode
    @bl vPortEnterCritical                   @ enter critical to avoid FreeRTOS enabling interrupts
    push {r0, r1}
    ldr r0, =ulCriticalNesting
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]
    pop {r0, r1}
    
    bl detection
    @ exit critical section without enabling interrupts (~vPortExitCritical)
    @ (these will be enabled by SPSR during movs pc, lr below
    @ If vPortExitCritical is called here, interrupts are enabled in the wrong mode
    @ (we are still in Monitor mode!). Thus, interrupts and mode has to be done at the
    @ same time => only possible via SPSR!
    @ ---------------------
    push {r0, r1}
    ldr r0, =ulCriticalNesting
    ldr r1, [r0]
    sub r1, r1, #1
    str r1, [r0]
    pop {r0, r1}
    @ restore SPSR
    @ ------------
    msr cpsr_c, #Mode_MON | I_Bit | F_Bit   @ switch back to MONITOR mode
    pop {r0}
    msr SPSR_cxsf, r0
    pop {r0-r12, lr}
    
    @ detection routine might have changed pxCurrentTCB
    @ => reload
    @ -------------------------------------------------
    ldr r0, =pxCurrentTCB       @ load pxCurrentTCB (task control block) into r0
    ldr r0, [r0]            
    ldr lr, [r0]                @ load stack pointer of task
#endif
    
#ifdef DEBUG
    @ debug print - SP
    push {r0-r12, lr}
    ldr r0, =sp_string
    mov r1, lr
    bl cprintf
    pop {r0-r12, lr}
    
    @ debug print - stack
    push {r0-r12, lr}
    mov r1, lr
    mov r3, #18
    bl print_my_stack
    pop {r0-r12, lr}
#endif
    
    ldr r1, =ulCriticalNesting  @ get ulCriticalNesting
    ldmfd lr!, {r2}             @ load first item on stack...
    str r2, [r1]                @ ...into ulCriticalNesting variable
    ldmfd lr!, {r0}             @ get SPSR from stack
    bic r0, r0, #0x40           @ make sure FIQs are enabled
    and r1, r0, #0x1F           @ get mode of the stored SPSR (lowest 5 bits)
    cmp r1, #Mode_SYS           @ is task SPSR SYS mode?
    beq set_SPSR                @ continue
    cmp r1, #Mode_USR           @ is task SPSR USR mode?
    beq set_SPSR                @ continue
    ldr r0, =wrongmode_string
    bl cprintf
    SMC #13                     @ else: kernel panic (task is attacking the modes?)
set_SPSR:   
    msr SPSR_cxsf, r0           @ set SPSR
    ldmfd lr, {r0-r14}^         @ restore registers
    nop
    ldr lr, [lr, #60]           @ restore task return address (15 registers * 4 bytes = 60)

#ifdef DEBUG
    @ debug print - LR
    push {r0-r12, lr}
    ldr r0, =lr_string
    mov r1, lr
    bl cprintf
    ldr r0, =newline_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
    subs lr, lr, #4             @ calculate return address (portINSTRUCTION_SIZE=4 was added)
    nop
    nop

    @ world switch
	@ ------------
    push {r0-r12, lr}
    @ only switch world for normal tasks, not for restoration!
    bl is_currentTCB_restoration
    cmp r0, #0
    @ all non-restoration tasks:
	mrcne p15, 0, r0, c1, c1, 0               @ Read Secure Configuration Register data
	orrne r0, r0, #NS_BIT                     @ switch to normal world
	mcrne p15, 0, r0, c1, c1, 0               @ Write Secure Configuration Register data
    @ restoration task:
    @ leave NS_BIT as it is
    mrseq r0, SPSR                            @ get SPSR
    orreq r0, r0, #0x80                       @ make sure IRQ is off
    msreq SPSR_cxsf, r0                       @ set SPSR
    pop {r0-r12, lr}
    
    @ Clear local monitor
	@ -------------------
	clrex

#ifdef TIMING
    push {r0-r12, lr}
    @ r0 = (get_timer() - timing - timing_overhead) / 1024
    @ -----------------------------------------------
    bl stop_timer
    @cprintf
    @-------
    mov r1, r0
    ldr r0, =timing_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
	@ goto loaded task
	@ ----------------
	movs pc, lr

/*
 * SMC to switch from normal world to secure world
 */
smc_switch_world_nw_sw: 
    @ NormWorld -> SecWorld: (=portSAVE_CONTEXT) SMC 1
    
#ifdef DEBUG
    @ debug print - SP_mon
    push {r0-r12, lr}
    mov r1, sp
    ldr r0, =sp_string
    bl cprintf
    ldr r0, =col_monitor
    bl cprintf
    bl print_MODE
    ldr r0, =col_reset
    bl cprintf
    pop {r0-r12, lr}

    @ print world
    push {r0-r12, lr}
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    and r0, r0, #NS_BIT                     @ Only return NS_BIT
    tst r0, #0
    ldreq r0, =sw_string
    ldrne r0, =nw_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
    @ save task (normal world)
    @ ------------------------
    @ get saved LR
    push {r0}           @ WATCH OUT WITH ALTERING (the "ldr ... #12" below has to be changed)
    push {r1}           @ WATCH OUT WITH ALTERING (the "ldr ... #12" below has to be changed)
    push {r2}           @ WATCH OUT WITH ALTERING (the "ldr ... #12" below has to be changed)
    ldr r1, [sp,#12]    @ get saved SPSR register from monitor stack (STACK GROWS DOWNWARDS!)
    ldr r2, [sp,#16]    @ get saved LR register from monitor stack (STACK GROWS DOWNWARDS!)
                        @ WATCH OUT: also adjust add sp, sp, #8 below
    
    @ get task stack pointer (normal/secure world share SP => get secure world SYS stack pointer)
    @ set world to secure world
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    bic r0, r0, #NS_BIT                     @ unset NS bit
    mcr p15, 0, r0, c1, c1, 0				@ Write Secure Configuration Register data
    msr cpsr_c, #Mode_SYS | I_Bit | F_Bit   @ goto System Mode (= mode of task)
    mov r0, sp                              @ save task stack pointer (Cortex-A8 manual p. 75)
    msr cpsr_c, #Mode_MON | I_Bit | F_Bit   @ switch back
    
#ifdef DEBUG
    @ debug print mode
    push {r0-r12, lr}
    ldr r0, =col_monitor
    bl cprintf
    bl print_MODE
    ldr r0, =col_reset
    bl cprintf
    pop {r0-r12, lr}

    @ print world
    push {r0-r12, lr}
    mrc p15, 0, r0, c1, c1, 0				@ Read Secure Configuration Register data
    and r0, r0, #NS_BIT                     @ Only return NS_BIT
    tst r0, #0
    ldreq r0, =sw_string
    ldrne r0, =nw_string
    bl cprintf
    pop {r0-r12, lr}
    
    @ debug print - SP_mon
    push {r0-r12, lr}
    mov r1, sp
    ldr r0, =sp_string
    bl cprintf
    pop {r0-r12, lr}
#endif

    
#ifdef DEBUG
    @ debug print - SPSR
    push {r0-r12, lr}
    ldr r0, =spsr_string
    bl cprintf
    pop {r0-r12, lr}

    @ debug print - LR
    push {r0-r12, lr}
    ldr r0, =lr_string
    mov r1, r2
    bl cprintf
    pop {r0-r12, lr}
    
    @ debug print - SP_sys
    push {r0-r12, lr}
    mov r1, r0
    ldr r0, =sp_string
    bl cprintf
    pop {r0-r12, lr}
#endif

    @ push return address onto task stack
    stmfd r0!, {r2}             @ push return address
    pop {r2}                    @ restore original r2
    @ push original r0-r14 to task stack (current r0 = task stack pointer)
    stmfd r0, {r2-r14}^         @ push currently unused of registers (they get pushed in reverse order!)
    sub r0, r0, #52             @ user space regs ("{...}^") do not adjust SP => 13 regs * 4 = 52
    nop
    pop {r2}                    @ original r1
    stmfd r0, {r2}^             @ push original r1
    sub r0, r0, #4              @ user space regs ("{...}^") do not adjust SP => 1 reg * 4 = 4
    nop
    pop {r2}                    @ original r0
    stmfd r0, {r2}^             @ push original r0
    sub r0, r0, #4              @ user space regs ("{...}^") do not adjust SP => 1 reg * 4 = 4
    nop                         @ 52+4+4 = 60 = 15 regs * 4 bytes
    @ push SPSR
    stmfd r0!, {r1}             @ push SPSR to task stack
    @ push ulCriticalNesting
    ldr r1, =ulCriticalNesting
    ldr r1, [r1]
    stmfd r0!, {r1}             @ push ulCriticalNesting to task stack
    @ update task stack pointer
    ldr r1, =pxCurrentTCB
    ldr r1, [r1]
    str r0, [r1]                @ store task stack pointer in pxCurrentTCB
    add sp, sp, #8              @ remove SPSR/LR from stack (since it is not needed any more)

#ifdef DEBUG
    @ debug print - SP
    push {r0-r12, lr}
    mov r1, r0
    ldr r0, =sp_string
    bl cprintf
    pop {r0-r12, lr}
    
    @ debug print - stack
    push {r0-r12, lr}
    mov r1, r0
    mov r3, #18
    bl print_my_stack
    pop {r0-r12, lr}
#endif
    
    
    @ restore secure world
    @ --------------------
    ldr   r1, =S_STACK_SP       @ load secure world stack pointer
    ldr   r3, [r1]
    ldmfd r3!, {r0}             @ get SPSR (do not restore LR! this returns from main!)
    msr   SPSR_cxsf, r0         @ restore SPSR
    ldmfd r3!, {r4-r12}         @ restore register r4 to r12
    str   r3, [r1]              @ update stack pointer
    mov   r0, #0                @ clear registers
    mov   r1, #0
    mov   r2, #0
    mov   r3, #0
    
    @ Clear local monitor
	@ -------------------
	clrex
    
	@ Perform SMC return
	@ ------------------
	movs pc, lr


.global InitSecureWorldWithoutTrustZone
InitSecureWorldWithoutTrustZone:
    push {r0}
  	LDR r0, =_start 
	MCR p15, 0, r0, c12, c0, 0          @sets up the secure world IRQ handler
    pop {r0}
	mov pc, lr

.global InitTrustzone
InitTrustzone: 
    @ backup return address     =>      DO NOT USE r3 IN THE ENTIRE FUNCTION!!!
    @ ---------------------
    mov r3, lr
    
	@ Install Secure Monitor
	@ -----------------------
	ldr r0, =monitor					@ Get address of Monitors vector table
	mcr p15, 0, r0, c12, c0, 1			@ Write Monitor Vector Base Address Register

    @ Configure IRQ/FIQ
    @ -----------------
    @ ARM Cortex-A8 manual p. 168 chapter 3.2.28
    @ 31-6: reserved
    @ 5:    AW  1 = enable Abort bit modification normal world (aborts handled locally)
    @ 4:    FW  0 = disable FIQ bit modification normal world
    @ 3:    EA  0 = branch to abort mode on abort (1 = branch to monitor mode on abort)
    @ 2:    FIQ 1 = branch to Monitor mode on FIQ
    @ 1:    IRQ 0 = branch to IRQ mode on IRQ (1 = branch to monitor mode)
    @ 0:    NS  0 = secure (1 = nonsecure)
    @ note: "there is no option to prevent the Normal world masking IRQ interrupts" (ARM website)
    @
    @ => route FIQ to monitor, IRQ to normal world, disallow normal world to set FIQ bit
    @    (= normal world cannot mask "secure world IRQ" - i.e. the FIQ)
    mov r0, #0x24               @ 0b100100
    mcr p15, 0, r0, c1, c1, 0   @ update settings
    mov r0, #0                  @ clear r0 - just in case
    
	@ Write Secure World Vector Base Address
	@ ---------------------------------------
	LDR r0, =_start
	MCR p15, 0, r0, c12, c0, 0          @sets up the secure world IRQ handler

	@ Save Secure world to S_STACK
	@ ----------------------------
    ldr    r0, =S_STACK_LIMIT			@ Get address of Secure state stack
	stmfd  r0!, {r4-r12}				@ Save general purpose registers
   	mrs    r1, cpsr						@ Also get a copy of the CPSR
	stmfd  r0!, {r1}	    			@ Save CPSR

    @ switch to Monitor mode (after saving state)
    @ -------------------------------------------
    cps   #Mode_MON                     @ change processor state changes one or more of mode,A,I,F
                                        @ without changing the others => IRQ/FIQ are still off
    
    @ Save secure world stack pointer
    @ -------------------------------
	ldr    r1, =S_STACK_SP				@ Get address of global
	str    r0, [r1]						@ Save pointer

    @ Write Normal World Vector Base Address
	@ --------------------------------------
    mrc p15, 0, r0, c1, c1, 0           @ get state
    orr r0, #NS_BIT                     @ set NS bit
    mcr p15, 0, r0, c1, c1, 0           @ switch to normal world
	ldr r1, =_ns_start                  @ get normal world start
	mcr p15, 0, r1, c12, c0, 0          @ setup normal world IRQs
    bic r0, #NS_BIT                     @ clear NS bit
    mcr p15, 0, r0, c1, c1, 0           @ switch back to secure world

    @ Restore secure world registers and return (= to secure world)
    @ -------------------------------------------------------------
    ldr     r0, =S_STACK_SP             @ get secure world stack pointer
    ldr     r1, [r0]                    @ into r1
    ldmfd   r1!, {r0}					@ Get CPSR
    msr     cpsr_cxsf, r0               @ Restore CPSR
    ldmfd   r1!, {r4-r12}				@ Restore registers r4 to r12
    mov     r0, #0                      @ clean up r0-r3
    mov     r1, #0
    mov     r2, #0

    @ restore return address
    @ ----------------------
    mov     lr, r3
    mov     r3, #0
    
    @RETURN! (no more statements!)
	mov pc, lr

.macro setup_pll pll, freq
		ldr r0, =\pll
		adr r2, W_DP_\freq
		bl setup_pll_func
.endm

.global lowlevel_init
.align 4
lowlevel_init:
	mov r10, lr
	ldr r0, =CCM_BASE_ADDR

	/* Gate of clocks to the peripherals first */
	ldr r1, =0x3FFFFFFF
	str r1, [r0, #CLKCTL_CCGR0]
	str r4, [r0, #CLKCTL_CCGR1]
	str r4, [r0, #CLKCTL_CCGR2]
	str r4, [r0, #CLKCTL_CCGR3]
	str r4, [r0, #CLKCTL_CCGR7]
	ldr r1, =0x00030000
	str r1, [r0, #CLKCTL_CCGR4]
	ldr r1, =0x00FFF030
	str r1, [r0, #CLKCTL_CCGR5]
	ldr r1, =0x0F00030F
	str r1, [r0, #CLKCTL_CCGR6]

	/* Switch ARM to step clock */
	mov r1, #0x4
	str r1, [r0, #CLKCTL_CCSR]

	setup_pll PLL1_BASE_ADDR, 800

	setup_pll PLL3_BASE_ADDR, 400	

	/* Switch peripheral to PLL3 */
	ldr r0, =CCM_BASE_ADDR
	ldr r1, =0x00015154
	str r1, [r0, #CLKCTL_CBCMR]
	ldr r1, =0x02898945
	str r1, [r0, #CLKCTL_CBCDR]
	/* make sure change is effective */
1:      ldr r1, [r0, #CLKCTL_CDHIPR]
		cmp r1, #0x0
		bne 1b

	setup_pll PLL2_BASE_ADDR, 400

	
	/* Switch peripheral to PLL2 */
	ldr r0, =CCM_BASE_ADDR
	ldr r1, =0x00888945
	str r1, [r0, #CLKCTL_CBCDR]

	ldr r1, =0x00016154
	str r1, [r0, #CLKCTL_CBCMR]

	/*change uart clk parent to pll2*/
	ldr r1, [r0, #CLKCTL_CSCMR1]
	and r1, r1, #0xfcffffff
	orr r1, r1, #0x01000000
	str r1, [r0, #CLKCTL_CSCMR1]

	/* make sure change is effective */
1:      ldr r1, [r0, #CLKCTL_CDHIPR]
		cmp r1, #0x0
		bne 1b

	setup_pll PLL3_BASE_ADDR, 216

	setup_pll PLL4_BASE_ADDR, 455

	/* Set the platform clock dividers */
	ldr r0, =ARM_BASE_ADDR
	ldr r1, =0x00000124
	str r1, [r0, #0x14]

	ldr r0, =CCM_BASE_ADDR
	mov r1, #0
	str r1, [r0, #CLKCTL_CACRR]

	/* Switch ARM back to PLL 1. */
	mov r1, #0x0
	str r1, [r0, #CLKCTL_CCSR]
	
	/* make uart div=6 */
	ldr r1, [r0, #CLKCTL_CSCDR1]
	and r1, r1, #0xffffffc0
	orr r1, r1, #0x0a
	str r1, [r0, #CLKCTL_CSCDR1]

	/* Restore the default values in the Gate registers */
	ldr r1, =0xFFFFFFFF
	str r1, [r0, #CLKCTL_CCGR0]
	str r1, [r0, #CLKCTL_CCGR1]
	str r1, [r0, #CLKCTL_CCGR2]
	str r1, [r0, #CLKCTL_CCGR3]
	str r1, [r0, #CLKCTL_CCGR4]
	str r1, [r0, #CLKCTL_CCGR5]
	str r1, [r0, #CLKCTL_CCGR6]
	str r1, [r0, #CLKCTL_CCGR7]

	mov r1, #0x00000
	str r1, [r0, #CLKCTL_CCDR]

	/* for cko - for ARM div by 8 */
	mov r1, #0x000A0000
	add r1, r1, #0x00000F0
	str r1, [r0, #CLKCTL_CCOSR]
	
	mov pc, r10

setup_pll_func:
	ldr r1, =0x00001232
	str r1, [r0, #PLL_DP_CTL] /* Set DPLL ON (set UPEN bit): BRMO=1 */
	mov r1, #0x2
	str r1, [r0, #PLL_DP_CONFIG] /* Enable auto-restart AREN bit */

	ldr r1, [r2, #W_DP_OP]
	str r1, [r0, #PLL_DP_OP]
	str r1, [r0, #PLL_DP_HFS_OP]

	ldr r1, [r2, #W_DP_MFD]
	str r1, [r0, #PLL_DP_MFD]
	str r1, [r0, #PLL_DP_HFS_MFD]

	ldr r1, [r2, #W_DP_MFN]
	str r1, [r0, #PLL_DP_MFN]
	str r1, [r0, #PLL_DP_HFS_MFN]

	ldr r1, =0x00001232
	str r1, [r0, #PLL_DP_CTL]
	1:  ldr r1, [r0, #PLL_DP_CTL]
		ands r1, r1, #0x1
		beq 1b

	/* r10 saved upper lr */
	mov pc, lr

W_DP_216:       .word DP_OP_216
				.word DP_MFD_216
				.word DP_MFN_216
W_DP_400:		.word DP_OP_400
				.word DP_MFD_400
				.word DP_MFN_400
W_DP_455:       .word DP_OP_455
				.word DP_MFD_455
				.word DP_MFN_455
W_DP_800:       .word DP_OP_800
				.word DP_MFD_800
				.word DP_MFN_800
    
@ ------------------------------------------------------------
@ Space reserved for secure and non-secure stacks
@ ------------------------------------------------------------
S_STACK_BASE:
	.word     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
S_STACK_LIMIT:
	
S_STACK_SP:
	.word     0

S_SVC_SP:
    .word     0
        
@.end
@ ------------------------------------------------------------
@ End
@ ------------------------------------------------------------

@------------------------------------------------------------
@ Secure world Handlers
@ -----------------------------------------------------------
UndefHandler:
    bl secure_undef
	b .
	
SWIHandler:
	push {r0, lr}                   @ save context
    ldr r0, [lr,#-4]                @ SWI number = instruction without top 8 bits
    bic r0,r0,#0xff000000
    cmp r0, #0                      @ check for SWI 0
    pop {r0}
    beq freeRtos_PortYield          @ r0 == 0: call FreeRTOS
    bne ownHandler                  @ r0 != 0: call own handler
freeRtos_PortYield:
    pop {lr}                        @ revert context
    bl  vPortYieldProcessor         @ let FreeRTOS do the work
    mov pc, lr                      @ return
ownHandler: 
    push {r1-r12}                   @ save rest of context
	bl secure_swi                   @ call own handler
    pop {r1-r12}                    @ restore context
    pop {lr}                        @ restore context
	mov pc, lr                      @ return
 
PAbortHandler:
    bl secure_pabort
	b .

DAbortHandler:
    bl secure_dabort
	b .

IRQHandler:
    push {lr}
    push {r0-r4, r12}
    bl secure_irq
    pop {r0-r4, r12}
    pop {lr}
    subs pc, lr, #4
    
FIQHandler:
    push {lr}
    push {r0-r4, r12}
    bl secure_fiq
    pop {r0-r4, r12}
    pop {lr}
    subs pc, lr, #4

@------------------------------------------------------------
@ Normal world Handlers
@ -----------------------------------------------------------
ns_ResetHandler:
	push {lr}
    push {r0-r4, r12}
	bl ns_reset
    pop {r0-r4, r12}
	pop {lr}
	mov pc, lr

ns_UndefHandler:
	bl ns_undef
    bl .        @return makes no sense after an undefined exception
	
ns_SWIHandler:
#ifdef DEBUG
    @ debug print
    push {r0-r12, lr}
    ldr r0, =newline_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
    @ IMPROVE: switch to sys mode (SVC mode cannot be blocked!)
    cps #Mode_SYS
    SMC #3    @ call monitor

#ifdef DEBUG
    @ debug print
    push {r0-r12, lr}
    ldr r0, =nshandler_string
    bl cprintf
    pop {r0-r12, lr}
#endif
    
	mov pc, lr

ns_PAbortHandler:
	bl ns_pa
    bl .        @return makes no sense after a prefetch abort

ns_DAbortHandler:
	bl ns_da
    bl .        @return makes no sense after a data abort

ns_IRQHandler:
    push {lr}
    push {r0-r4, r12}
	bl ns_irq
    pop {r0-r4, r12}
	pop {lr}
    subs pc, lr, #4

.data
col_monitor:
    .ascii "\x1B[1;34m\0"
col_reset:
    .ascii "\x1B[0m\0"
sp_string:
    .ascii "\x1B[1;34mSP %x\x1B[0m\n\0"
lr_string:
    .ascii "\x1B[1;34mLR %x\x1B[0m\n\0"
spsr_string:
    .ascii "\x1B[1;34mSPSR %x\x1B[0m\n\0"
stack_string:
    .ascii "\x1B[1;34mstack %x: %x\x1B[0m\n\0"
kernelpanic_string:
    .ascii "\x1B[1;31m### KERNEL PANIC ###\x1B[0m\n\0"
fiq_string:
    .ascii "\n\x1B[1;34m-- FIQ --\x1B[0m\n\0"
newline_string:
    .ascii "\n\0"
nshandler_string:
    .ascii "ns_SWIHandler\n\0"
nw_string:
    .ascii "\x1B[1;34mnormal world\x1B[0m\n\0"
sw_string:
    .ascii "\x1B[1;34msecure world\x1B[0m\n\0"
wrongmode_string:
    .ascii "scheduled task has wrong mode\n\0"
freertos_return_string:
    .ascii "FreeRTOS_ISR returned\n\0"
portyield_return_string:
    .ascii "portYIELD returned\n\0"
timing_string:
    .ascii "TZ timing: %d ns\n\0"
