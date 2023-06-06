

// AD9833 registers
#define AD9833_REG_CTRL 0x0000 // control register
#define AD9833_REG_FREQ0 0x4000 // frequency register 0
#define AD9833_REG_FREQ1 0x8000 // frequency register 1
#define AD9833_REG_PHASE0 0xC000 // phase register 0
#define AD9833_REG_PHASE1 0xE000 // phase register 1

// AD9833 command bits
#define AD9833_CMD_B28 0x2000 // enable 28-bit mode
#define AD9833_CMD_HLB 0x1000 // enable half-amplitude mode
#define AD9833_CMD_FSEL0 0x0000 // select frequency register 0
#define AD9833_CMD_FSEL1 0x0800 // select frequency register 1
#define AD9833_CMD_PSEL0 0x0000 // select phase register 0
#define AD9833_CMD_PSEL1 0x0100 // select phase register 1
#define AD9833_CMD_RESET 0x0100 // reset the device
#define AD9833_CMD_SLEEP1 0x0080 // sleep mode, output disabled
#define AD9833_CMD_SLEEP12 0x0040 // sleep mode, output enabled
#define AD9833_CMD_OPBITEN 0x0020 // enable output, disable internal clock
#define AD9833_CMD_NOP 0x0000 // no operation

// AD9833 clock frequency (Hz)
#define AD9833_CLK_FREQ 25000000
