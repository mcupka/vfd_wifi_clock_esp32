//This file defines constants used to easily construct characters on the VFD grids

#ifndef VFD_CHARS_H
#define VFD_CHARS_H

#define VFD_GRID0 (0x1 << 19)
#define VFD_GRID1 (0x1 << 18)
#define VFD_GRID2 (0x1 << 17)
#define VFD_GRID3 (0x1 << 16)
#define VFD_GRID4 (0x1 << 15)
#define VFD_GRID5 (0x1 << 14)
#define VFD_GRID6 (0x1 << 13)
#define VFD_GRID7 (0x1 << 12)

#define VFD_AN_SSEG_D (0x1 << 4)
#define VFD_AN_SSEG_E (0x1 << 5)
#define VFD_AN_SSEG_C (0x1 << 6)
#define VFD_AN_SSEG_G (0x1 << 7)
#define VFD_AN_SSEG_F (0x1 << 8)
#define VFD_AN_SSEG_B (0x1 << 9)
#define VFD_AN_SSEG_A (0x1 << 10)

#define VFD_S (VFD_AN_SSEG_A | VFD_AN_SSEG_F | VFD_AN_SSEG_G | VFD_AN_SSEG_C | VFD_AN_SSEG_D)
#define VFD_O (VFD_AN_SSEG_A | VFD_AN_SSEG_B | VFD_AN_SSEG_F | VFD_AN_SSEG_E |VFD_AN_SSEG_C | VFD_AN_SSEG_D)
#define VFD_P (VFD_AN_SSEG_A | VFD_AN_SSEG_B | VFD_AN_SSEG_C | VFD_AN_SSEG_D | VFD_AN_SSEG_E | VFD_AN_SSEG_F | VFD_AN_SSEG_G)


#define VFD_TOPIND = (0x1 << 11)
#define VFD_TOPIND = (0x1 << 1)

#endif

