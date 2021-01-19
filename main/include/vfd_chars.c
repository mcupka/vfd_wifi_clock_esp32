#include "vfd_chars.h"`
#include "vfd_driver.h"


uint temp_asciitable[128] = {
    VFD_BLANK, //0
    VFD_BLANK, //1
    VFD_BLANK, //2
    VFD_BLANK, //3
    VFD_BLANK, //4
    VFD_BLANK, //5
    VFD_BLANK, //6
    VFD_BLANK, //7
    VFD_BLANK, //8
    VFD_BLANK, //9
    VFD_BLANK, //10
    VFD_BLANK, //11
    VFD_BLANK, //12
    VFD_BLANK, //13
    VFD_BLANK, //14
    VFD_BLANK, //15
    VFD_BLANK, //16
    VFD_BLANK, //17
    VFD_BLANK, //18
    VFD_BLANK, //19
    VFD_BLANK, //20
    VFD_BLANK, //21
    VFD_BLANK, //22
    VFD_BLANK, //23
    VFD_BLANK, //24
    VFD_BLANK, //25
    VFD_BLANK, //26
    VFD_BLANK, //27
    VFD_BLANK, //28
    VFD_BLANK, //29
    VFD_BLANK, //30
    VFD_BLANK, //31
    VFD_BLANK, //32
    VFD_BLANK, //33
    VFD_BLANK, //34
    VFD_BLANK, //35
    VFD_BLANK, //36
    VFD_BLANK, //37
    VFD_BLANK, //38
    VFD_BLANK, //39
    VFD_BLANK, //40
    VFD_BLANK, //41
    VFD_BLANK, //42
    VFD_BLANK, //43
    VFD_BLANK, //44
    VFD_BLANK, //45
    VFD_BLANK, //46
    VFD_BLANK, //47
    VFD_0, //48
    VFD_1, //49
    VFD_2, //50
    VFD_3, //51
    VFD_4, //52
    VFD_5, //53
    VFD_6, //54
    VFD_7, //55
    VFD_8, //56
    VFD_9, //57
    VFD_BLANK, //58
    VFD_BLANK, //59
    VFD_BLANK, //60
    VFD_BLANK, //61
    VFD_BLANK, //62
    VFD_BLANK, //63
    VFD_BLANK, //64
    VFD_A, //65
    VFD_B, //66
    VFD_C,//67
    VFD_D,//68
    VFD_E,//69
    VFD_F,//70
    VFD_G,//71
    VFD_H,//72
    VFD_I,//73
    VFD_J,//74
    VFD_BLANK, //75
    VFD_L, //76
    VFD_BLANK, //77
    VFD_N, //78
    VFD_O,//79
    VFD_P,//80
    VFD_Q,//81
    VFD_R,//82
    VFD_S,//83
    VFD_T,//84
    VFD_U,//85
    VFD_BLANK, //86
    VFD_BLANK, //87
    VFD_BLANK, //88
    VFD_Y, //89
    VFD_BLANK, //90
    VFD_BLANK, //91
    VFD_BLANK, //92
    VFD_BLANK, //93
    VFD_BLANK, //94
    VFD_BLANK, //95
    VFD_BLANK, //96
    VFD_BLANK, //97
    VFD_BLANK, //98
    VFD_BLANK, //99
    VFD_BLANK, //100
    VFD_BLANK, //101
    VFD_BLANK, //102
    VFD_BLANK, //103
    VFD_BLANK, //104
    VFD_BLANK, //105
    VFD_BLANK, //106
    VFD_BLANK, //107
    VFD_BLANK, //108
    VFD_BLANK, //109
    VFD_BLANK, //110
    VFD_BLANK, //111
    VFD_BLANK, //112
    VFD_BLANK, //113
    VFD_BLANK, //114
    VFD_BLANK, //115
    VFD_BLANK, //116
    VFD_BLANK, //117
    VFD_BLANK, //118
    VFD_BLANK, //119
    VFD_BLANK, //120
    VFD_BLANK, //121
    VFD_BLANK, //122
    VFD_BLANK, //123
    VFD_BLANK, //124
    VFD_BLANK, //125
    VFD_BLANK, //126
    VFD_BLANK //127
};

void vfd_init_asciitable() {

    for (int a = 0; a < 128; a++) {
        vfd_asciitable[a] = temp_asciitable[a];
    }

}
