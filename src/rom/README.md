This folder contains the dump of the ROM of the emulated computers.

For each ROM/computer, a set of patches is defined to help the emulation of some devices
(tape drive, floppy disk drive, light pen...).
Most of the patches use an illegal opcode followed by a RTS (Return from Subroutine) opcode.
These illegal opcodes are processed by function RunIoOpcode() in devices.c to emulate the devices.

The following table gives a summary of the functions of the "Monitor" program that are patched on a TO computer.

| Feature | Monitor's function | Start Address | Comments |
|---------|--------------------|---------------|----------|
| Tape read/write | K7CO | $E815 | Operation depends on value of K7.OPC($6029): $01(open in read mode), $02(read byte), $04(open in write mode), $08(write byte), $10(close) |
| Floppy disk read/write | DKCO | $E004 | Operation depends on value of DK.OPC($6048): $01(reset controller), $02(read sector), $08(write sector) |
| Floppy disk formatting | DKFORM | $E00A | DK.OPC($6048)=$00 |
| Mouse coordinates | GEPE | $EC06 | |
| Mouse click | PEIN | $EC09 | |
| Light pen coordinates | GETI | $E818 | |
| RS-232 interface | RSCO | $E812 | |
