/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDEROPT2FILTERLOOKUP_H
#define TILERECUTILS_TILERAWCHANNELBUILDEROPT2FILTERLOOKUP_H

//////////////////////////////////////////////////////////////////////
//
// copied from online to for DSP OF reconstruction emulation
// by      Andrei.Artamonov@cern.ch
// ------------ original comments:
// LUT calculated in
// afs_ific@dev/LUT
// 25-jul-2007
//////////////////////////////////////////////////////////////////////

unsigned short lookup[2401]={
0x000e,
0x8000, 0x4000, 0x2aab, 0x2000, 0x199a, 0x1555, 0x1249, 0x1000, 0x0e39, 0x0ccd, 
0x0ba3, 0x0aab, 0x09d9, 0x0925, 0x0889, 0x0800, 0x0788, 0x071c, 0x06bd, 0x0666, 
0x0618, 0x05d1, 0x0591, 0x0555, 0x051f, 0x04ec, 0x04be, 0x0492, 0x046a, 0x0444, 
0x0421, 0x0400, 0x03e1, 0x03c4, 0x03a8, 0x038e, 0x0376, 0x035e, 0x0348, 0x0333, 
0x031f, 0x030c, 0x02fa, 0x02e9, 0x02d8, 0x02c8, 0x02b9, 0x02ab, 0x029d, 0x028f, 
0x0283, 0x0276, 0x026a, 0x025f, 0x0254, 0x0249, 0x023f, 0x0235, 0x022b, 0x0222, 
0x0219, 0x0211, 0x0208, 0x0200, 0x01f8, 0x01f0, 0x01e9, 0x01e2, 0x01db, 0x01d4, 
0x01ce, 0x01c7, 0x01c1, 0x01bb, 0x01b5, 0x01af, 0x01aa, 0x01a4, 0x019f, 0x019a, 
0x0195, 0x0190, 0x018b, 0x0186, 0x0182, 0x017d, 0x0179, 0x0174, 0x0170, 0x016c, 
0x0168, 0x0164, 0x0160, 0x015d, 0x0159, 0x0155, 0x0152, 0x014e, 0x014b, 0x0148, 
0x0144, 0x0141, 0x013e, 0x013b, 0x0138, 0x0135, 0x0132, 0x012f, 0x012d, 0x012a, 
0x0127, 0x0125, 0x0122, 0x011f, 0x011d, 0x011a, 0x0118, 0x0116, 0x0113, 0x0111, 
0x010f, 0x010d, 0x010a, 0x0108, 0x0106, 0x0104, 0x0102, 0x0100, 0x00fe, 0x00fc, 
0x00fa, 0x00f8, 0x00f6, 0x00f5, 0x00f3, 0x00f1, 0x00ef, 0x00ed, 0x00ec, 0x00ea, 
0x00e8, 0x00e7, 0x00e5, 0x00e4, 0x00e2, 0x00e0, 0x00df, 0x00dd, 0x00dc, 0x00da, 
0x00d9, 0x00d8, 0x00d6, 0x00d5, 0x00d3, 0x00d2, 0x00d1, 0x00cf, 0x00ce, 0x00cd, 
0x00cc, 0x00ca, 0x00c9, 0x00c8, 0x00c7, 0x00c5, 0x00c4, 0x00c3, 0x00c2, 0x00c1, 
0x00c0, 0x00bf, 0x00bd, 0x00bc, 0x00bb, 0x00ba, 0x00b9, 0x00b8, 0x00b7, 0x00b6, 
0x00b5, 0x00b4, 0x00b3, 0x00b2, 0x00b1, 0x00b0, 0x00af, 0x00ae, 0x00ad, 0x00ac, 
0x00ac, 0x00ab, 0x00aa, 0x00a9, 0x00a8, 0x00a7, 0x00a6, 0x00a5, 0x00a5, 0x00a4, 
0x00a3, 0x00a2, 0x00a1, 0x00a1, 0x00a0, 0x009f, 0x009e, 0x009e, 0x009d, 0x009c, 
0x009b, 0x009b, 0x009a, 0x0099, 0x0098, 0x0098, 0x0097, 0x0096, 0x0096, 0x0095, 
0x0094, 0x0094, 0x0093, 0x0092, 0x0092, 0x0091, 0x0090, 0x0090, 0x008f, 0x008e, 
0x008e, 0x008d, 0x008d, 0x008c, 0x008b, 0x008b, 0x008a, 0x008a, 0x0089, 0x0089, 
0x0088, 0x0087, 0x0087, 0x0086, 0x0086, 0x0085, 0x0085, 0x0084, 0x0084, 0x0083, 
0x0083, 0x0082, 0x0082, 0x0081, 0x0081, 0x0080, 0x0080, 0x007f, 0x007f, 0x007e, 
0x007e, 0x007d, 0x007d, 0x007c, 0x007c, 0x007b, 0x007b, 0x007a, 0x007a, 0x0079, 
0x0079, 0x0078, 0x0078, 0x0078, 0x0077, 0x0077, 0x0076, 0x0076, 0x0075, 0x0075, 
0x0075, 0x0074, 0x0074, 0x0073, 0x0073, 0x0073, 0x0072, 0x0072, 0x0071, 0x0071, 
0x0071, 0x0070, 0x0070, 0x006f, 0x006f, 0x006f, 0x006e, 0x006e, 0x006e, 0x006d, 
0x006d, 0x006d, 0x006c, 0x006c, 0x006b, 0x006b, 0x006b, 0x006a, 0x006a, 0x006a, 
0x0069, 0x0069, 0x0069, 0x0068, 0x0068, 0x0068, 0x0067, 0x0067, 0x0067, 0x0066, 
0x0066, 0x0066, 0x0065, 0x0065, 0x0065, 0x0065, 0x0064, 0x0064, 0x0064, 0x0063, 
0x0063, 0x0063, 0x0062, 0x0062, 0x0062, 0x0062, 0x0061, 0x0061, 0x0061, 0x0060, 
0x0060, 0x0060, 0x0060, 0x005f, 0x005f, 0x005f, 0x005e, 0x005e, 0x005e, 0x005e, 
0x005d, 0x005d, 0x005d, 0x005d, 0x005c, 0x005c, 0x005c, 0x005c, 0x005b, 0x005b, 
0x005b, 0x005b, 0x005a, 0x005a, 0x005a, 0x005a, 0x0059, 0x0059, 0x0059, 0x0059, 
0x0058, 0x0058, 0x0058, 0x0058, 0x0057, 0x0057, 0x0057, 0x0057, 0x0056, 0x0056, 
0x0056, 0x0056, 0x0056, 0x0055, 0x0055, 0x0055, 0x0055, 0x0054, 0x0054, 0x0054, 
0x0054, 0x0054, 0x0053, 0x0053, 0x0053, 0x0053, 0x0053, 0x0052, 0x0052, 0x0052, 
0x0052, 0x0052, 0x0051, 0x0051, 0x0051, 0x0051, 0x0051, 0x0050, 0x0050, 0x0050, 
0x0050, 0x0050, 0x004f, 0x004f, 0x004f, 0x004f, 0x004f, 0x004e, 0x004e, 0x004e, 
0x004e, 0x004e, 0x004d, 0x004d, 0x004d, 0x004d, 0x004d, 0x004d, 0x004c, 0x004c, 
0x004c, 0x004c, 0x004c, 0x004c, 0x004b, 0x004b, 0x004b, 0x004b, 0x004b, 0x004a, 
0x004a, 0x004a, 0x004a, 0x004a, 0x004a, 0x0049, 0x0049, 0x0049, 0x0049, 0x0049, 
0x0049, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0048, 0x0047, 0x0047, 
0x0047, 0x0047, 0x0047, 0x0047, 0x0046, 0x0046, 0x0046, 0x0046, 0x0046, 0x0046, 
0x0046, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0044, 0x0044, 
0x0044, 0x0044, 0x0044, 0x0044, 0x0044, 0x0043, 0x0043, 0x0043, 0x0043, 0x0043, 
0x0043, 0x0043, 0x0042, 0x0042, 0x0042, 0x0042, 0x0042, 0x0042, 0x0042, 0x0042, 
0x0041, 0x0041, 0x0041, 0x0041, 0x0041, 0x0041, 0x0041, 0x0041, 0x0040, 0x0040, 
0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x0040, 0x003f, 0x003f, 0x003f, 0x003f, 
0x003f, 0x003f, 0x003f, 0x003f, 0x003e, 0x003e, 0x003e, 0x003e, 0x003e, 0x003e, 
0x003e, 0x003e, 0x003d, 0x003d, 0x003d, 0x003d, 0x003d, 0x003d, 0x003d, 0x003d, 
0x003d, 0x003c, 0x003c, 0x003c, 0x003c, 0x003c, 0x003c, 0x003c, 0x003c, 0x003c, 
0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 0x003b, 
0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x003a, 0x0039, 
0x0039, 0x0039, 0x0039, 0x0039, 0x0039, 0x0039, 0x0039, 0x0039, 0x0039, 0x0038, 
0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 0x0038, 
0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 0x0037, 
0x0037, 0x0036, 0x0036, 0x0036, 0x0036, 0x0036, 0x0036, 0x0036, 0x0036, 0x0036, 
0x0036, 0x0036, 0x0035, 0x0035, 0x0035, 0x0035, 0x0035, 0x0035, 0x0035, 0x0035, 
0x0035, 0x0035, 0x0035, 0x0035, 0x0034, 0x0034, 0x0034, 0x0034, 0x0034, 0x0034, 
0x0034, 0x0034, 0x0034, 0x0034, 0x0034, 0x0034, 0x0033, 0x0033, 0x0033, 0x0033, 
0x0033, 0x0033, 0x0033, 0x0033, 0x0033, 0x0033, 0x0033, 0x0033, 0x0032, 0x0032, 
0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 0x0032, 
0x0032, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 
0x0031, 0x0031, 0x0031, 0x0031, 0x0031, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 
0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x002f, 
0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 0x002f, 
0x002f, 0x002f, 0x002f, 0x002f, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 
0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 0x002e, 
0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 
0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002d, 0x002c, 0x002c, 0x002c, 0x002c, 
0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 0x002c, 
0x002c, 0x002c, 0x002c, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 
0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 0x002b, 
0x002b, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 
0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x002a, 0x0029, 
0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 
0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0029, 0x0028, 
0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 
0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0027, 
0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 
0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 0x0027, 
0x0027, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 
0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 0x0026, 
0x0026, 0x0026, 0x0026, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 
0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 
0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0025, 0x0024, 0x0024, 0x0024, 
0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 
0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 0x0024, 
0x0024, 0x0024, 0x0024, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 
0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0023, 0x0022, 
0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 
0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 
0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0022, 0x0021, 0x0021, 
0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 
0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 
0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0021, 0x0020, 0x0020, 
0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 
0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 
0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 
0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 
0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 
0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 0x001f, 
0x001f, 0x001f, 0x001f, 0x001f, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 
0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 
0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 
0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 0x001e, 
0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 
0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 
0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 
0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001d, 0x001c, 
0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 
0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 
0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 
0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 0x001c, 
0x001c, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 
0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 
0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 
0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 
0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001b, 0x001a, 0x001a, 0x001a, 0x001a, 
0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 
0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 
0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 
0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 
0x001a, 0x001a, 0x001a, 0x001a, 0x001a, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 
0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 
0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 
0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 
0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 
0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0019, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
0x0018, 0x0018, 0x0018, 0x0018, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 
0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0017, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 0x0016, 
0x0016, 0x0016, 0x0016, 0x0016, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 
0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0015, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 0x0014, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 0x0013, 
0x0013, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 0x0012, 
0x0012, 0x0012, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 
0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 
0x0010, 0x0010, 0x0010, 0x0010, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 
0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000f, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 
0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e, 0x000e  }; 

#endif
