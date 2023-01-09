// Compile this file using the commandline:
/*
  cl430 -vmsp --define=__MSP430F247__ --opt_level=4 --keep_asm flash_eraser_onchip.c `
    -I C:/ti/ccs930/ccs/ccs_base/msp430/include `
    -I C:/ti/ccs930/ccs/tools/compiler/ti-cgt-msp430_18.12.8.LTS/include `
    --run_linker --stack_size 0 -o flash_eraser_onchip.elf `
    -I C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\lib

*/
// replace backtick with backslash if using a linuxy shell
// We define MSP430F247 but it supports 249/2410 as the registers match
// (powershell)
// $Env:PATH += ";C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\bin"

// Then grab the raw bytes of the function:
// ofd430 --obj_display=rawdata -o raw.txt flash_eraser_onchip.elf
// Search for " Raw Data for Section ".TI.bound:flash_erase""
// There's your bytes!

// Note: we totally ignore c_init and the stack. Check the assembly and ensure
// the stack is not used (it shouldn't be). Look for:
// "Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte"

// This *could* have been assembly, but it's more pleasant to massage C instead

// Include files
#include <msp430.h>
#include <stdint.h>

// all our devices have at least 0x09FF to 0x0200 (2KB) in the same place
#define RAM_START 0x0200

#define ERASE_TYPE_ALL     0 // Erase main and information flash memory
#define ERASE_TYPE_MAIN    1 // Erase only main flash memory
#define ERASE_TYPE_SEGMENT 2 // Erase a single segment

__attribute__((location(RAM_START))) volatile struct {
    uint8_t done;
    uint8_t eraseType;
    // set to 0x0FC10 for MAIN or ALL erase types
    volatile uint16_t *segmentAddr;
} param;

// these don't get linked for some reason
__attribute__((location(0x0056))) volatile unsigned char DCOCTL;
__attribute__((location(0x0057))) volatile unsigned char BCSCTL1;
__attribute__((location(0x0053))) volatile unsigned char BCSCTL3;
__attribute__((location(0x10F9))) volatile unsigned char CALBC1_16MHZ;
__attribute__((location(0x10F8))) volatile unsigned char CALDCO_16MHZ;
__attribute__((location(0x0128))) volatile unsigned int FCTL1;
__attribute__((location(0x012A))) volatile unsigned int FCTL2;
__attribute__((location(0x012C))) volatile unsigned int FCTL3;

// dummy
void main() {}

// section() so we can rip it out of the binary
__attribute__((location(RAM_START + sizeof(param)), retain, ramfunc, noreturn))
void flash_erase()
{
    // in case we broke out
    while(param.done);

    // ACLK  = LPLF OSC for WD Timer (~12kHz)
    // MCLK  = 16 MHz internal oscillator
    // SMCLK = 16 MHz internal oscillator
    BCSCTL1 = CALBC1_16MHZ;
    BCSCTL3 = LFXT1S1;
    DCOCTL = CALDCO_16MHZ;

    // Clock source for flash timing generator
    FCTL2 = FWKEY | FSSEL_1 | 0x002B;		// MCLK/44 clock source for flash timing generator

    // Wait while busy
    while ( FCTL3 & BUSY );

    // clear LOCK and set LOCKA if not wiping everything
    if(param.eraseType == ERASE_TYPE_ALL)
    {
        FCTL3 = FWKEY;
    }
    else
    {
        FCTL3 = FWKEY | LOCKA;
    }

    // Setup erase mode
    if(param.eraseType == ERASE_TYPE_SEGMENT)
    {
        FCTL1 = FWKEY | ERASE;
    }
    else
    {
        FCTL1 = FWKEY | ERASE | MERAS;
    }

    // Perform erase by doing a dummy write
    *param.segmentAddr = 0;

    // Wait while busy
    while ( FCTL3 & BUSY );

    // End write
    FCTL1 = FWKEY;
    FCTL3 = FWKEY | LOCK;

    // signal to parent that we are done
    param.done = 1;

    // chillout
    while(1);
}
