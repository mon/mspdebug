// Compile this file using the commandline:
/*
  cl430 -vmsp --define=__MSP430F247__ --opt_level=4 --keep_asm flash_loader_onchip.c `
    -I C:/ti/ccs930/ccs/ccs_base/msp430/include `
    -I C:/ti/ccs930/ccs/tools/compiler/ti-cgt-msp430_18.12.8.LTS/include `
    --run_linker --stack_size 0 -o flash_loader_onchip.elf `
    -I C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\lib

*/
// replace backtick with backslash if using a linuxy shell
// We define MSP430F247 but it supports 249/2410 as the registers match
// (powershell)
// $Env:PATH += ";C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\bin"

// Then grab the raw bytes of the function:
// ofd430 --obj_display=rawdata -o raw.txt flash_loader_onchip.elf
// Search for " Raw Data for Section ".TI.bound:flash_write""
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

__attribute__((location(RAM_START))) volatile struct {
    uint16_t wrtLen;
    const uint16_t *pSrc;
    uint16_t *pDst;
    uint16_t wrtLenThis;
} param;

// these don't get linked for some reason
__attribute__((location(0x0128))) volatile unsigned int FCTL1;
__attribute__((location(0x012A))) volatile unsigned int FCTL2;
__attribute__((location(0x012C))) volatile unsigned int FCTL3;

// dummy
void main() {}

// section() so we can rip it out of the binary
__attribute__((location(RAM_START + sizeof(param)), retain, ramfunc, noreturn))
void flash_write()
{
    // Clock source for flash timing generator
	FCTL2 = FWKEY | FSSEL_1 | 0x002B;		// MCLK/44 clock source for flash timing generator

    while(1) {
        // Wait while busy
        while ( FCTL3 & BUSY );

        // in case we broke out
        while(param.wrtLen == 0 && param.pSrc == 0 && param.pDst == 0);

        // Clear lock
        FCTL3 = FWKEY;

        // Loop over blocks
        while ( param.wrtLen > 0 )
        {
            // Set write length to be up to next block boundary
            param.wrtLenThis = (64 - ((unsigned int)param.pDst % 64)) / 2;

            // Limit write length to remaining length
            if ( param.wrtLenThis > param.wrtLen ) param.wrtLenThis = param.wrtLen;

            // Enable block write
            FCTL1 = FWKEY | BLKWRT | WRT;

            // Do write
            while ( param.wrtLenThis > 0 )
            {
                *(param.pDst++) = *(param.pSrc++);

                // Wait while WAIT=0
                while ( ( FCTL3 & WAIT ) == 0x0000 );

                param.wrtLen--;
                param.wrtLenThis--;
            }

            // Clear block write
            FCTL1 = FWKEY | WRT;

            // Wait while busy
            while ( FCTL3 & BUSY );
        }

        // End write
        FCTL1 = FWKEY;
        FCTL3 = FWKEY | LOCK;

        // in case we somehow break out
        param.pSrc = 0;
        param.pDst = 0;
        // signal to parent that we are done by clearing first var
        param.wrtLen = 0;

        // chillout
        while(1);
    }
}
