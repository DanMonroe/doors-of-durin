#ifndef _dodPixelStrip_h
#define _dodPixelStrip_h

#include <DOD_Pixel.h>

class DOD_PixelStrips
{
private:
  DOD_Pixel left[287];
  DOD_Pixel right[276];

public:
  ~DOD_PixelStrips() {}
  DOD_PixelStrips()
  {
    left[0] = DOD_Pixel(1, 5, 13);
    left[1] = DOD_Pixel(2, 5, 15);
    left[2] = DOD_Pixel(3, 5, 18);
    left[3] = DOD_Pixel(4, 5, 20);
    left[4] = DOD_Pixel(5, 5, 23);
    left[5] = DOD_Pixel(6, 5, 27);
    left[6] = DOD_Pixel(7, 5, 29);
    left[7] = DOD_Pixel(8, 5, 32);
    left[8] = DOD_Pixel(9, 5, 34);
    left[9] = DOD_Pixel(10, 5, 36);
    left[10] = DOD_Pixel(11, 5, 39);
    left[11] = DOD_Pixel(12, 5, 42);
    left[12] = DOD_Pixel(13, 5, 44);
    left[13] = DOD_Pixel(14, 5, 47);
    left[14] = DOD_Pixel(15, 5, 49);
    left[15] = DOD_Pixel(16, 5, 52);
    left[16] = DOD_Pixel(17, 5, 55);
    left[17] = DOD_Pixel(18, 5, 57);
    left[18] = DOD_Pixel(19, 5, 60);
    left[19] = DOD_Pixel(20, 5, 62);
    left[20] = DOD_Pixel(21, 5, 65);
    left[21] = DOD_Pixel(22, 5, 68);
    left[22] = DOD_Pixel(23, 5, 70);
    left[23] = DOD_Pixel(24, 5, 72);
    left[24] = DOD_Pixel(25, 5, 75);
    left[25] = DOD_Pixel(26, 5, 78);
    left[26] = DOD_Pixel(27, 2, 77);
    left[27] = DOD_Pixel(28, 5, 69);
    left[28] = DOD_Pixel(29, 7, 68);
    left[29] = DOD_Pixel(30, 8, 66);
    left[30] = DOD_Pixel(31, 10, 64);
    left[31] = DOD_Pixel(32, 12, 63);
    left[32] = DOD_Pixel(33, 14, 61);
    left[33] = DOD_Pixel(34, 15, 41);
    left[34] = DOD_Pixel(35, 13, 43);
    left[35] = DOD_Pixel(36, 10, 46);
    left[36] = DOD_Pixel(37, 8, 48);
    left[37] = DOD_Pixel(38, 6, 50);
    left[38] = DOD_Pixel(39, 4, 52);
    left[39] = DOD_Pixel(40, 3, 82);
    left[40] = DOD_Pixel(41, 6, 82);
    left[41] = DOD_Pixel(42, 8, 82);
    left[42] = DOD_Pixel(43, 11, 82);
    left[43] = DOD_Pixel(44, 13, 82);
    left[44] = DOD_Pixel(45, 16, 82);
    left[45] = DOD_Pixel(46, 19, 82);
    left[46] = DOD_Pixel(47, 19, 86);
    left[47] = DOD_Pixel(48, 16, 86);
    left[48] = DOD_Pixel(49, 13, 86);
    left[49] = DOD_Pixel(50, 11, 86);
    left[50] = DOD_Pixel(51, 8, 86);
    left[51] = DOD_Pixel(52, 6, 86);
    left[52] = DOD_Pixel(53, 3, 86);
    left[53] = DOD_Pixel(54, 5, 90);
    left[54] = DOD_Pixel(55, 6, 92);
    left[55] = DOD_Pixel(56, 7, 94);
    left[56] = DOD_Pixel(57, 8, 96);
    left[57] = DOD_Pixel(58, 10, 99);
    left[58] = DOD_Pixel(59, 11, 101);
    left[59] = DOD_Pixel(60, 12, 103);
    left[60] = DOD_Pixel(61, 13, 106);
    left[61] = DOD_Pixel(62, 15, 108);
    left[62] = DOD_Pixel(63, 16, 110);

    left[63] = DOD_Pixel(64, 11, 116);
    left[64] = DOD_Pixel(65, 9, 118);
    left[65] = DOD_Pixel(66, 7, 120);

    left[66] = DOD_Pixel(67, 20, 112);
    left[67] = DOD_Pixel(68, 22, 113);
    left[68] = DOD_Pixel(69, 24, 114);
    left[69] = DOD_Pixel(70, 27, 115);
    left[70] = DOD_Pixel(71, 30, 116);
    left[71] = DOD_Pixel(72, 32, 117);
    left[72] = DOD_Pixel(73, 34, 118);
    left[73] = DOD_Pixel(74, 37, 119);
    left[74] = DOD_Pixel(75, 39, 120);
    left[75] = DOD_Pixel(76, 42, 121);
    left[76] = DOD_Pixel(77, 45, 122);

    left[77] = DOD_Pixel(78, 45, 119);
    left[78] = DOD_Pixel(79, 42, 118);
    left[79] = DOD_Pixel(80, 39, 117);
    left[80] = DOD_Pixel(81, 37, 116);
    left[81] = DOD_Pixel(82, 34, 115);
    left[82] = DOD_Pixel(83, 32, 114);
    left[83] = DOD_Pixel(84, 30, 113);
    left[84] = DOD_Pixel(85, 27, 112);
    left[85] = DOD_Pixel(86, 24, 111);
    left[86] = DOD_Pixel(87, 22, 110);
    left[87] = DOD_Pixel(88, 20, 109);

    left[88] = DOD_Pixel(89, 20, 108);
    left[89] = DOD_Pixel(90, 18, 106);
    left[90] = DOD_Pixel(91, 16, 104);
    left[91] = DOD_Pixel(92, 15, 102);
    left[92] = DOD_Pixel(93, 14, 100);
    left[93] = DOD_Pixel(94, 13, 98);
    left[94] = DOD_Pixel(95, 12, 95);
    left[95] = DOD_Pixel(96, 10, 92);
    left[96] = DOD_Pixel(97, 8, 89);

    left[97] = DOD_Pixel(98, 13, 89);
    left[98] = DOD_Pixel(99, 14, 91);
    left[99] = DOD_Pixel(100, 15, 94);
    left[100] = DOD_Pixel(101, 17, 96);
    left[101] = DOD_Pixel(102, 18, 98);
    left[102] = DOD_Pixel(103, 19, 100);
    left[103] = DOD_Pixel(104, 20, 102);
    left[104] = DOD_Pixel(105, 21, 105);

    left[105] = DOD_Pixel(106, 26, 106);
    left[106] = DOD_Pixel(107, 28, 107);
    left[107] = DOD_Pixel(108, 31, 108);
    left[108] = DOD_Pixel(109, 33, 109);
    left[109] = DOD_Pixel(110, 36, 110);
    left[110] = DOD_Pixel(111, 38, 111);
    left[111] = DOD_Pixel(112, 40, 112);
    left[112] = DOD_Pixel(113, 42, 113);
    left[113] = DOD_Pixel(114, 45, 114);

    left[114] = DOD_Pixel(115, 45, 106);
    left[115] = DOD_Pixel(116, 45, 104);
    left[116] = DOD_Pixel(117, 45, 101);
    left[117] = DOD_Pixel(118, 45, 99);
    left[118] = DOD_Pixel(119, 45, 96);
    left[119] = DOD_Pixel(120, 45, 94);
    left[120] = DOD_Pixel(121, 45, 91);
    left[121] = DOD_Pixel(122, 45, 89);
    left[122] = DOD_Pixel(123, 45, 86);
    left[123] = DOD_Pixel(124, 45, 83);
    left[124] = DOD_Pixel(125, 45, 80);

    left[125] = DOD_Pixel(126, 41, 83);
    left[126] = DOD_Pixel(127, 41, 85);
    left[127] = DOD_Pixel(128, 41, 88);
    left[128] = DOD_Pixel(129, 41, 90);
    left[129] = DOD_Pixel(130, 41, 93);
    left[130] = DOD_Pixel(131, 41, 95);
    left[131] = DOD_Pixel(132, 41, 98);
    left[132] = DOD_Pixel(133, 41, 100);

    left[133] = DOD_Pixel(134, 40, 103);
    left[134] = DOD_Pixel(135, 38, 102);
    left[135] = DOD_Pixel(136, 36, 101);
    left[136] = DOD_Pixel(137, 34, 99);
    left[137] = DOD_Pixel(138, 32, 98);
    left[138] = DOD_Pixel(139, 30, 96);

    left[139] = DOD_Pixel(140, 41, 70);
    left[140] = DOD_Pixel(141, 41, 68);
    left[141] = DOD_Pixel(142, 41, 65);
    left[142] = DOD_Pixel(143, 41, 63);
    left[143] = DOD_Pixel(144, 41, 60);
    left[144] = DOD_Pixel(145, 42, 56);
    left[145] = DOD_Pixel(146, 42, 53);
    left[146] = DOD_Pixel(147, 42, 50);
    left[147] = DOD_Pixel(148, 42, 47);
    left[148] = DOD_Pixel(149, 43, 44);
    left[149] = DOD_Pixel(150, 43, 42);
    left[150] = DOD_Pixel(151, 44, 40);

    left[151] = DOD_Pixel(152, 38, 51);
    left[152] = DOD_Pixel(153, 38, 53);
    left[153] = DOD_Pixel(154, 37, 55);
    left[154] = DOD_Pixel(155, 37, 58);
    left[155] = DOD_Pixel(156, 36, 60);
    left[156] = DOD_Pixel(157, 36, 62);
    left[157] = DOD_Pixel(158, 36, 65);
    left[158] = DOD_Pixel(159, 35, 68);
    left[159] = DOD_Pixel(160, 35, 71);

    left[160] = DOD_Pixel(161, 33, 71);
    left[161] = DOD_Pixel(162, 33, 68);
    left[162] = DOD_Pixel(163, 34, 65);
    left[163] = DOD_Pixel(164, 34, 62);
    left[164] = DOD_Pixel(165, 34, 60);
    left[165] = DOD_Pixel(166, 35, 58);
    left[166] = DOD_Pixel(167, 35, 55);
    left[167] = DOD_Pixel(168, 36, 53);
    left[168] = DOD_Pixel(169, 36, 51);

    left[169] = DOD_Pixel(170, 34, 45);
    left[170] = DOD_Pixel(171, 34, 47);
    left[171] = DOD_Pixel(172, 33, 50);
    left[172] = DOD_Pixel(173, 33, 52);
    left[173] = DOD_Pixel(174, 33, 55);
    left[174] = DOD_Pixel(175, 32, 57);
    left[175] = DOD_Pixel(176, 32, 59);
    left[176] = DOD_Pixel(177, 32, 61);
    left[177] = DOD_Pixel(178, 31, 64);
    left[178] = DOD_Pixel(179, 31, 67);
    left[179] = DOD_Pixel(180, 30, 70);
    left[180] = DOD_Pixel(181, 30, 73);

    left[181] = DOD_Pixel(182, 25, 79);
    left[182] = DOD_Pixel(183, 25, 77);
    left[183] = DOD_Pixel(184, 25, 74);
    left[184] = DOD_Pixel(185, 26, 71);
    left[185] = DOD_Pixel(186, 26, 69);
    left[186] = DOD_Pixel(187, 26, 66);
    left[187] = DOD_Pixel(188, 27, 64);
    left[188] = DOD_Pixel(189, 27, 61);
    left[189] = DOD_Pixel(190, 27, 59);
    left[190] = DOD_Pixel(191, 28, 56);
    left[191] = DOD_Pixel(192, 28, 54);
    left[192] = DOD_Pixel(193, 28, 51);
    left[193] = DOD_Pixel(194, 29, 49);
    left[194] = DOD_Pixel(195, 29, 46);
    left[195] = DOD_Pixel(196, 29, 44);
    left[196] = DOD_Pixel(197, 30, 41);
    left[197] = DOD_Pixel(198, 30, 39);

    left[198] = DOD_Pixel(199, 24, 9);
    left[199] = DOD_Pixel(200, 24, 11);
    left[200] = DOD_Pixel(201, 24, 14);
    left[201] = DOD_Pixel(202, 24, 16);
    left[202] = DOD_Pixel(203, 24, 19);
    left[203] = DOD_Pixel(204, 24, 21);
    left[204] = DOD_Pixel(205, 24, 24);
    left[205] = DOD_Pixel(206, 24, 26);
    left[206] = DOD_Pixel(207, 24, 29);
    left[207] = DOD_Pixel(208, 24, 31);
    left[208] = DOD_Pixel(209, 24, 34);
    left[209] = DOD_Pixel(210, 24, 36);
    left[210] = DOD_Pixel(211, 24, 39);
    left[211] = DOD_Pixel(212, 24, 41);
    left[212] = DOD_Pixel(213, 24, 44);
    left[213] = DOD_Pixel(214, 24, 46);
    left[214] = DOD_Pixel(215, 24, 49);
    left[215] = DOD_Pixel(216, 24, 51);
    left[216] = DOD_Pixel(217, 24, 54);
    left[217] = DOD_Pixel(218, 24, 56);
    left[218] = DOD_Pixel(219, 24, 59);
    left[219] = DOD_Pixel(220, 24, 61);
    left[220] = DOD_Pixel(221, 24, 64);
    left[221] = DOD_Pixel(222, 24, 66);
    left[222] = DOD_Pixel(223, 24, 69);
    left[223] = DOD_Pixel(224, 24, 71);
    left[224] = DOD_Pixel(225, 24, 74);
    left[225] = DOD_Pixel(226, 24, 76);

    left[226] = DOD_Pixel(227, 20, 76);
    left[227] = DOD_Pixel(228, 20, 74);
    left[228] = DOD_Pixel(229, 20, 71);
    left[229] = DOD_Pixel(230, 20, 69);
    left[230] = DOD_Pixel(231, 20, 66);
    left[231] = DOD_Pixel(232, 20, 64);
    left[232] = DOD_Pixel(233, 20, 61);
    left[233] = DOD_Pixel(234, 20, 59);
    left[234] = DOD_Pixel(235, 20, 56);
    left[235] = DOD_Pixel(236, 20, 54);
    left[236] = DOD_Pixel(237, 20, 51);
    left[237] = DOD_Pixel(238, 20, 49);
    left[238] = DOD_Pixel(239, 20, 46);
    left[239] = DOD_Pixel(240, 20, 44);
    left[240] = DOD_Pixel(241, 20, 41);
    left[241] = DOD_Pixel(242, 20, 39);
    left[242] = DOD_Pixel(243, 20, 36);
    left[243] = DOD_Pixel(244, 20, 34);
    left[244] = DOD_Pixel(245, 20, 31);
    left[245] = DOD_Pixel(246, 20, 29);
    left[246] = DOD_Pixel(247, 20, 26);
    left[247] = DOD_Pixel(248, 20, 24);
    left[248] = DOD_Pixel(249, 20, 21);
    left[249] = DOD_Pixel(250, 20, 19);
    left[250] = DOD_Pixel(251, 20, 16);
    left[251] = DOD_Pixel(252, 20, 14);
    left[252] = DOD_Pixel(253, 20, 11);
    left[253] = DOD_Pixel(254, 20, 9);

    left[254] = DOD_Pixel(255, 36, 5);
    left[255] = DOD_Pixel(256, 33, 5);
    left[256] = DOD_Pixel(257, 30, 5);
    left[257] = DOD_Pixel(258, 27, 5);
    left[258] = DOD_Pixel(259, 25, 5);
    left[259] = DOD_Pixel(260, 22, 5);
    left[260] = DOD_Pixel(261, 20, 5);
    left[261] = DOD_Pixel(262, 17, 5);
    left[262] = DOD_Pixel(263, 15, 5);
    left[263] = DOD_Pixel(264, 12, 5);
    left[264] = DOD_Pixel(265, 10, 5);
    left[265] = DOD_Pixel(266, 7, 5);
    left[266] = DOD_Pixel(267, 5, 5);

    left[267] = DOD_Pixel(268, 4, 2);
    left[268] = DOD_Pixel(269, 6, 2);
    left[269] = DOD_Pixel(270, 9, 2);
    left[270] = DOD_Pixel(271, 12, 2);
    left[271] = DOD_Pixel(272, 15, 2);
    left[272] = DOD_Pixel(273, 17, 2);
    left[273] = DOD_Pixel(274, 19, 2);
    left[274] = DOD_Pixel(275, 21, 2);
    left[275] = DOD_Pixel(276, 24, 2);
    left[276] = DOD_Pixel(277, 26, 2);
    left[277] = DOD_Pixel(278, 29, 2);
    left[278] = DOD_Pixel(279, 31, 2);
    left[279] = DOD_Pixel(280, 33, 2);
    left[280] = DOD_Pixel(281, 36, 2);
    left[281] = DOD_Pixel(282, 38, 2);
    left[282] = DOD_Pixel(283, 41, 2);
    left[283] = DOD_Pixel(284, 43, 2);

    left[284] = DOD_Pixel(285, 44, 12);
    left[285] = DOD_Pixel(286, 44, 14);
    left[286] = DOD_Pixel(287, 44, 17);
  };
};

#endif