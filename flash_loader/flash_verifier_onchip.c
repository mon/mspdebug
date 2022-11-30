// Compile this file using the commandline:
/*
  cl430 -vmsp --define=__MSP430F247__ --opt_level=4 --keep_asm flash_verifier_onchip.c `
    -I C:/ti/ccs930/ccs/ccs_base/msp430/include `
    -I C:/ti/ccs930/ccs/tools/compiler/ti-cgt-msp430_18.12.8.LTS/include `
    --run_linker --stack_size 0 -o flash_verifier_onchip.elf `
    -I C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\lib

*/
// replace backtick with backslash if using a linuxy shell
// We define MSP430F247 but it supports 249/2410 as the registers match
// (powershell)
// $Env:PATH += ";C:\ti\ccs930\ccs\tools\compiler\ti-cgt-msp430_18.12.8.LTS\bin"

// Then grab the raw bytes of the function:
// ofd430 --obj_display=rawdata -o raw.txt flash_verifier_onchip.elf
// Search for " Raw Data for Section ".TI.bound:flash_verify""
// There's your bytes!

// Note: we totally ignore c_init and the stack. Check the assembly and ensure
// the stack is not used (it shouldn't be). Look for:
// "Local Frame Size  : 0 Args + 0 Auto + 0 Save = 0 byte"
// Also make sure there are no CALL instructions

// This *could* have been assembly, but it's more pleasant to massage C instead

// Include files
#include <msp430.h>
#include <stdint.h>

// all our devices have at least 0x09FF to 0x0200 (2KB) in the same place
#define RAM_START 0x0200

__attribute__((location(RAM_START))) volatile struct {
    uint8_t *data;
    uint16_t len;
    uint16_t crc;
} param;

// dummy
void main() {}

// section() so we can rip it out of the binary
__attribute__((location(RAM_START + sizeof(param)), retain, ramfunc, noreturn))
void flash_verify()
{
    while(1) {
        // in case we broke out
        while(param.data == 0 && param.len == 0);

        param.crc = 0xFFFF;
        uint8_t x;
        uint16_t y;
        uint8_t i;

        while(param.len--) {
            x = ((param.crc >> 8) ^ *param.data++) & 0xFF;
            // this is tricky actually - x >> 4 pulls in ABI functions and calls
            // them, and #pragma FORCEINLINE doesn't work. So just loop ourselves
            // x ^= x >> 4;
            y = x;
            for(i = 0; i < 4; i++) y >>= 1;
            x ^= y;
            // param.crc = (param.crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
            param.crc = (param.crc << 8);
            param.crc ^= x;
            y = x;
            for(i = 0; i < 5; i++) y <<= 1;
            param.crc ^= y;
            for(i = 0; i < 7; i++) y <<= 1;
            param.crc ^= y;
        }

        // in case we somehow break out
        param.len = 0;
        // signal to parent that we are done by clearing first var
        param.data = 0;

        // chillout
        while(1);
    }
}
