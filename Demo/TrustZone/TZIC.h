/**
 * TrustZone Interrupt Controller (TZIC) registers for i.MX53
 *
 * @author:       Michael Denzel
 * @email:        research@michael-denzel.de
 * @license:      GNU General Public License 2.0 or later
 * @date:         2017-03-17
 * @version:      0.1
 **/

#ifndef _TZIC_INTC_H_
#define _TZIC_INTC_H_

//REMARK: iMX53 has no GIC (generic interrupt controller)
//        only TZIC (trustzone interrupt controller)

#define __REG(x)    (*((volatile unsigned int *)(x)))

#define TZIC_TOTAL_IRQ         128

#define TZIC_BASE              (0x0FFFC000)

//Basic Controls
#define TZIC_INTCTRL           (TZIC_BASE + 0x000)
#define TZIC_INTTYPE           (TZIC_BASE + 0x004)
//according to documentation 0x008 is skipped!
#define TZIC_PRIOMASK          (TZIC_BASE + 0x00C)

#define TZIC_SYNCCTRL          (TZIC_BASE + 0x010)
#define TZIC_DSMINT            (TZIC_BASE + 0x014)

//Interrupt Security FIQ
#define TZIC_INTSEC0           (TZIC_BASE + 0x080)
#define TZIC_INTSEC1           (TZIC_BASE + 0x084)
#define TZIC_INTSEC2           (TZIC_BASE + 0x088)
#define TZIC_INTSEC3           (TZIC_BASE + 0x08C)

//Enable Set/Clear Register
#define TZIC_ENSET0            (TZIC_BASE + 0x100)
#define TZIC_ENSET1            (TZIC_BASE + 0x104)
#define TZIC_ENSET2            (TZIC_BASE + 0x108)
#define TZIC_ENSET3            (TZIC_BASE + 0x10C)
#define TZIC_ENCLEAR0          (TZIC_BASE + 0x180)
#define TZIC_ENCLEAR1          (TZIC_BASE + 0x184)
#define TZIC_ENCLEAR2          (TZIC_BASE + 0x188)
#define TZIC_ENCLEAR3          (TZIC_BASE + 0x18C)

//Source Set/Clear Register
#define TZIC_SRCSET0           (TZIC_BASE + 0x200)
#define TZIC_SRCSET1           (TZIC_BASE + 0x204)
#define TZIC_SRCSET2           (TZIC_BASE + 0x208)
#define TZIC_SRCSET3           (TZIC_BASE + 0x20C)
#define TZIC_SRCCLEAR0         (TZIC_BASE + 0x280)
#define TZIC_SRCCLEAR1         (TZIC_BASE + 0x284)
#define TZIC_SRCCLEAR2         (TZIC_BASE + 0x288)
#define TZIC_SRCCLEAR3         (TZIC_BASE + 0x28C)

//Priority Register
#define TZIC_PRIORITY0         (TZIC_BASE + 0x400)
#define TZIC_PRIORITY1         (TZIC_BASE + 0x404)
#define TZIC_PRIORITY2         (TZIC_BASE + 0x408)
#define TZIC_PRIORITY3         (TZIC_BASE + 0x40C)
#define TZIC_PRIORITY4         (TZIC_BASE + 0x410)
#define TZIC_PRIORITY5         (TZIC_BASE + 0x414)
#define TZIC_PRIORITY6         (TZIC_BASE + 0x418)
#define TZIC_PRIORITY7         (TZIC_BASE + 0x41C)
#define TZIC_PRIORITY8         (TZIC_BASE + 0x420)
#define TZIC_PRIORITY9         (TZIC_BASE + 0x424)
#define TZIC_PRIORITY10        (TZIC_BASE + 0x428)
#define TZIC_PRIORITY11        (TZIC_BASE + 0x42C)
#define TZIC_PRIORITY12        (TZIC_BASE + 0x430)
#define TZIC_PRIORITY13        (TZIC_BASE + 0x434)
#define TZIC_PRIORITY14        (TZIC_BASE + 0x438)
#define TZIC_PRIORITY15        (TZIC_BASE + 0x43C)
#define TZIC_PRIORITY16        (TZIC_BASE + 0x440)
#define TZIC_PRIORITY17        (TZIC_BASE + 0x444)
#define TZIC_PRIORITY18        (TZIC_BASE + 0x448)
#define TZIC_PRIORITY19        (TZIC_BASE + 0x44C)
#define TZIC_PRIORITY20        (TZIC_BASE + 0x450)
#define TZIC_PRIORITY21        (TZIC_BASE + 0x454)
#define TZIC_PRIORITY22        (TZIC_BASE + 0x458)
#define TZIC_PRIORITY23        (TZIC_BASE + 0x45C)
#define TZIC_PRIORITY24        (TZIC_BASE + 0x460)
#define TZIC_PRIORITY25        (TZIC_BASE + 0x464)
#define TZIC_PRIORITY26        (TZIC_BASE + 0x468)
#define TZIC_PRIORITY27        (TZIC_BASE + 0x46C)
#define TZIC_PRIORITY28        (TZIC_BASE + 0x470)
#define TZIC_PRIORITY29        (TZIC_BASE + 0x474)
#define TZIC_PRIORITY30        (TZIC_BASE + 0x478)
#define TZIC_PRIORITY31        (TZIC_BASE + 0x47C)

//Pending Register
//low priority
#define TZIC_PND0              (TZIC_BASE + 0xD00)
#define TZIC_PND1              (TZIC_BASE + 0xD04)
#define TZIC_PND2              (TZIC_BASE + 0xD08)
#define TZIC_PND3              (TZIC_BASE + 0xD0C)
//high priority
#define TZIC_HIPND0            (TZIC_BASE + 0xD80)
#define TZIC_HIPND1            (TZIC_BASE + 0xD84)
#define TZIC_HIPND2            (TZIC_BASE + 0xD88)
#define TZIC_HIPND3            (TZIC_BASE + 0xD8C)

//Wakeup Config Register
#define TZIC_WAKEUP0           (TZIC_BASE + 0xE00)
#define TZIC_WAKEUP1           (TZIC_BASE + 0xE04)
#define TZIC_WAKEUP2           (TZIC_BASE + 0xE08)
#define TZIC_WAKEUP3           (TZIC_BASE + 0xE0C)

//Software Interrupt Trigger Register
#define TZIC_SWINT             (TZIC_BASE + 0xF00)

#endif
