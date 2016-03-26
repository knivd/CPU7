# CPU7

CPU7 is a concept for a simple RPN processor with built-in features such as creatable and destroyable virtual cores for multhithreded execution, self-packing and self-unpacking code and data for less memory resources, Forth-like assembly language with instructions for direct operation with strings, as well as 'system function calls' which are hardware-dependant and take care of support to the hardware resources. This allows the creation of a hardware-level BIOS with CPU7 words performing various operations with system ports, data transfer, etc., thus cancelling the current register-oriented model, existing in all systems now.

The entire structure of CPU7 is optimised to make it easy for hardware implementation into a FPGA or ASIC with minimum hardware requirements.

The file CPU7.pdf gives a good initial overview to the model.
