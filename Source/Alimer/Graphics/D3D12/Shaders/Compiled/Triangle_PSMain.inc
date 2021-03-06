#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// COLOR                    0   xyzw        1     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
//
// Level9 shader bytecode:
//
    ps_2_0
    dcl t0  // input<4,5,6,7>

#line 30 "C:\Users\amer.koleci\Documents\Code\alimer\Source\Alimer\Graphics\D3D12\Shaders\Triangle.hlsl"
    mov oC0, t0  // ::PSMain<0,1,2,3>

// approximately 1 instruction slot used
ps_4_0
dcl_input_ps linear v1.xyzw
dcl_output o0.xyzw
mov o0.xyzw, v1.xyzw
ret 
// Approximately 0 instruction slots used
#endif

const BYTE Triangle_PSMain[] =
{
     68,  88,  66,  67,   1,  77, 
    193, 126, 159, 163, 113,  63, 
     62, 252,   6,  67,  25,  32, 
    233, 186,   1,   0,   0,   0, 
    176,   2,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    232,   1,   0,   0,  40,   2, 
      0,   0, 124,   2,   0,   0, 
     65, 111, 110,  57, 176,   1, 
      0,   0, 176,   1,   0,   0, 
      0,   2, 255, 255, 140,   1, 
      0,   0,  36,   0,   0,   0, 
      0,   0,  36,   0,   0,   0, 
     36,   0,   0,   0,  36,   0, 
      0,   0,  36,   0,   0,   0, 
     36,   0,   0,   2, 255, 255, 
    254, 255,  90,   0,  68,  66, 
     85,  71,  40,   0,   0,   0, 
     60,   1,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
    136,   0,   0,   0,   2,   0, 
      0,   0, 140,   0,   0,   0, 
      2,   0,   0,   0,  20,   1, 
      0,   0, 156,   0,   0,   0, 
     67,  58,  92,  85, 115, 101, 
    114, 115,  92,  97, 109, 101, 
    114,  46, 107, 111, 108, 101, 
     99, 105,  92,  68, 111,  99, 
    117, 109, 101, 110, 116, 115, 
     92,  67, 111, 100, 101,  92, 
     97, 108, 105, 109, 101, 114, 
     92,  83, 111, 117, 114,  99, 
    101,  92,  65, 108, 105, 109, 
    101, 114,  92,  71, 114,  97, 
    112, 104, 105,  99, 115,  92, 
     68,  51,  68,  49,  50,  92, 
     83, 104,  97, 100, 101, 114, 
    115,  92,  84, 114, 105,  97, 
    110, 103, 108, 101,  46, 104, 
    108, 115, 108,   0, 171, 171, 
     40,   0,   0,   0,   0,   0, 
    255, 255, 112,   1,   0,   0, 
     30,   0,   0,   0, 124,   1, 
      0,   0,  80,  83,  77,  97, 
    105, 110,   0, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   1,   0,   2,   0, 
      3,   0, 105, 110, 112, 117, 
    116,   0, 112, 111, 115, 105, 
    116, 105, 111, 110,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  99, 111, 
    108, 111, 114,   0, 171, 171, 
    198,   0,   0,   0, 208,   0, 
      0,   0, 224,   0,   0,   0, 
    208,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   8,   0, 
      1,   0,   2,   0, 232,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   5,   0,   6,   0, 
      7,   0,   0,   0,   0,   0, 
    156,   0,   0,   0, 164,   0, 
      0,   0,   1,   0,   0,   0, 
    180,   0,   0,   0, 156,   0, 
      0,   0, 192,   0,   0,   0, 
    248,   0,   0,   0,   1,   0, 
      0,   0,   8,   1,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      0,   0,  15, 176,   1,   0, 
      0,   2,   0,   8,  15, 128, 
      0,   0, 228, 176, 255, 255, 
      0,   0,  83,  72,  68,  82, 
     56,   0,   0,   0,  64,   0, 
      0,   0,  14,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     73,  83,  71,  78,  76,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  68,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0, 171, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  65,  82,  71,  69, 
     84,   0, 171, 171
};
