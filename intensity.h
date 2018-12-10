const int FRAMES = 8;
const int SHADES = 32;
#define RGB_MASK        ((uint32_t)0xffffc0ff)
#define OE_MASK         ((uint32_t)0xfffffff7)
#define CLK_MASK        ((uint32_t)0xffffffef)
#define LAT_MASK        ((uint32_t)0xffffffdf)
#define OE_EN           ((uint32_t)0x00000008)
#define CLK_EN          ((uint32_t)0x00000010)
#define LAT_EN          ((uint32_t)0x00000020)
#define ROW_MASK        ((uint32_t)0xfffffff0)

float z[32][32];
float z_max;
float z_min;

//from intensity 0 to 31
//each intensity (shade) is a sequence of 8 frames
//HUE: BLUE(1) ==> CYAN(3) => GREEN(2) ==> YELLOW(6) ==> RED(4)
const int intensity[32][8] = {
    {1, 1, 1, 1, 1, 1, 1, 3},
    {1, 1, 1, 1, 1, 1, 3, 3},
    {1, 1, 1, 1, 1, 3, 3, 3},
    {1, 1, 1, 1, 3, 3, 3, 3},
    {1, 1, 1, 3, 3, 3, 3, 3},
    {1, 1, 3, 3, 3, 3, 3, 3},
    {1, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 2},
    {3, 3, 3, 3, 3, 3, 2, 2},
    {3, 3, 3, 3, 3, 2, 2, 2},
    {3, 3, 3, 3, 2, 2, 2, 2},
    {3, 3, 3, 2, 2, 2, 2, 2},
    {3, 3, 2, 2, 2, 2, 2, 2},
    {3, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 2},
    {2, 2, 2, 2, 2, 2, 2, 6},
    {2, 2, 2, 2, 2, 2, 6, 6},
    {2, 2, 2, 2, 2, 6, 6, 6},
    {2, 2, 2, 2, 6, 6, 6, 6},
    {2, 2, 2, 6, 6, 6, 6, 6},
    {2, 2, 6, 6, 6, 6, 6, 6},
    {2, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 6},
    {6, 6, 6, 6, 6, 6, 6, 4},
    {6, 6, 6, 6, 6, 6, 4, 4},
    {6, 6, 6, 6, 6, 4, 4, 4},
    {6, 6, 6, 6, 4, 4, 4, 4},
    {6, 6, 6, 4, 4, 4, 4, 4},
    {6, 6, 4, 4, 4, 4, 4, 4},
    {6, 4, 4, 4, 4, 4, 4, 4},
    {4, 4, 4, 4, 4, 4, 4, 4}
};
