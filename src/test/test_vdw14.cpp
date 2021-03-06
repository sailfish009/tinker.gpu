#include "files.h"
#include "test.h"
#include "test_rt.h"


using namespace tinker;


TEST_CASE("Vdw14-Trpcage", "[ff][evdw][lj][trpcage]")
{
   rc_flag = calc::xyz | calc::vmask;


   const char* kname = "test_vdw14.key";
   std::string k0 = trpcage_charmm19_key;
   k0 += "\nVDWTERM ONLY\n";
   const char* xname = "test_vdw14.xyz";
   const char* x0 = trpcage_charmm19_xyz;
   const char* pname = "charmm19.prm";
   const char* p0 = commit_11e84c69::charmm19_prm;


   const double eps_e = 0.0001;
   const double eps_g = 0.0001;
   const double eps_v = 0.001;
   const char* argv[] = {"dummy", xname};
   int argc = 2;


   SECTION("  - elj -- no pbc, no cutoff")
   {
      TestFile fxy(xname, x0);
      TestFile fke(kname, k0);
      TestFile fpr(pname, p0);


      const double ref_e = -80.5672;
      const double ref_v[][3] = {{-648.117, -119.517, 31.637},
                                 {-119.517, -491.019, 30.124},
                                 {31.637, 30.124, -462.209}};
      const int ref_count = 17290;
      const double ref_g[][3] = {
         {-2.2181, -0.7271, -1.6957},   {-10.1362, -2.4631, 1.2675},
         {4.9909, 0.5483, 1.3153},      {13.8537, 2.2715, 4.3927},
         {-0.1063, 0.1344, 0.1491},     {-0.1045, 0.1801, 0.1591},
         {-0.0273, 0.1058, 0.1362},     {0.4154, 0.1678, -0.1770},
         {0.7763, 0.6405, 1.0702},      {10.2851, 2.0888, -0.1432},
         {-0.3098, 0.0101, -0.1943},    {-0.0169, 0.0213, -0.0631},
         {-0.0422, -0.0127, 0.0208},    {0.4041, 2.2101, 0.7306},
         {-2.8671, -2.9919, 0.9844},    {-13.7816, -4.5893, 0.7610},
         {-0.5485, -1.5951, 7.8434},    {1.0280, 0.6618, 0.1470},
         {-0.3800, -0.1703, 0.2622},    {-1.8486, -5.8453, 0.1829},
         {3.0747, -3.6272, -0.6128},    {-0.1075, 0.6764, -0.2330},
         {-2.5112, 1.6007, -2.1177},    {1.5389, -0.1141, -4.5896},
         {0.4238, 1.0432, -3.7929},     {-0.3212, -11.0308, -3.0182},
         {0.2168, 0.7016, -0.1472},     {-0.2878, -0.0805, 0.1126},
         {9.2776, -1.8543, 7.8900},     {7.1655, 5.7040, -2.8097},
         {0.3603, -10.2688, 7.3193},    {0.5372, 7.7704, -8.4576},
         {-8.4060, -8.7219, 0.3948},    {-8.2368, -1.0829, -8.8960},
         {0.5319, -1.1008, -0.3123},    {-0.2878, 0.2589, 0.0075},
         {-3.8423, 1.5674, -2.2731},    {1.7614, 2.9965, 2.3723},
         {2.7093, 3.8507, 2.4145},      {7.1809, -4.2217, -5.0594},
         {-0.8611, -0.1191, -0.3862},   {1.0762, 2.2366, -2.5600},
         {3.6770, 1.5686, -1.7688},     {2.7008, 1.5134, 0.5944},
         {2.0391, -2.2248, -3.2561},    {0.5257, 2.0751, 2.8507},
         {-1.7171, -3.5917, 2.9263},    {-3.2608, -1.1767, 3.8862},
         {1.9089, 4.0662, 3.8926},      {-2.0591, 1.8369, -0.1764},
         {0.0581, -0.1893, -0.3985},    {0.8622, -0.4418, 0.5635},
         {0.5115, 0.0853, 0.6254},      {3.7112, -4.3339, -0.1381},
         {0.2836, 0.2326, 0.1238},      {0.0096, 0.0221, -0.0639},
         {-0.0639, -0.0362, -0.0413},   {-3.2525, -0.5591, 0.5133},
         {-1.9698, -2.8100, -3.8648},   {1.5460, 3.7421, -0.0049},
         {-5.9790, -1.7027, 6.0753},    {-0.0185, 0.9455, -0.4325},
         {-0.1224, 1.4952, 1.0177},     {-3.1841, -2.7478, -0.3924},
         {-0.0173, -0.2770, -0.9497},   {4.3326, -1.8158, 8.6674},
         {2.0542, 1.1350, -0.6746},     {-7.6330, 0.6147, 14.3566},
         {10.1943, -0.5687, 1.9712},    {-11.7939, -7.0075, -0.5588},
         {3.9993, 2.1400, -10.2842},    {-3.0550, -4.2131, -9.0049},
         {8.9764, -0.9023, -0.5023},    {-1.4543, 1.3500, -0.7716},
         {4.5504, 1.6078, -1.7006},     {4.2562, 0.4855, -7.0634},
         {2.9028, -7.3332, -13.3951},   {-4.0426, 6.5323, 2.2738},
         {-0.3496, -0.3689, -0.2839},   {0.2835, 2.4906, -0.8420},
         {0.2934, 1.5441, 0.7713},      {1.1891, 1.1560, -2.4006},
         {0.9786, 0.8226, -1.2410},     {3.3898, 0.6925, 3.8232},
         {-2.7460, 6.0950, 0.5656},     {2.5846, 2.1659, -1.5304},
         {-5.2184, 4.7826, -1.1892},    {0.0712, -0.6284, 0.3818},
         {4.7320, 0.0975, 3.3153},      {5.3714, -0.8394, 4.4242},
         {11.5825, 3.7678, 1.5514},     {-1.1779, 1.5571, 0.1776},
         {-0.0351, -0.0506, -0.1088},   {-0.1087, 0.0516, -0.1092},
         {0.0260, -0.0950, -0.1203},    {-2.0858, 0.9036, 4.9338},
         {-4.8278, -1.2162, -3.1337},   {1.1560, -0.2972, -0.7871},
         {98.0652, 57.8969, -61.0493},  {0.0617, -0.2323, 0.0108},
         {1.3978, -1.5191, 0.7929},     {-0.3429, -0.4194, -0.4878},
         {-15.0431, -3.3859, 0.9624},   {-0.8837, -0.4781, -1.0994},
         {-3.5678, 2.5696, 0.0173},     {0.0454, -0.9175, -3.9445},
         {2.1905, 3.4251, 0.5081},      {1.7792, 11.7366, -1.5305},
         {-6.8599, 2.8144, 4.4079},     {0.0184, -2.0585, -1.3403},
         {4.6182, 3.8586, 5.7880},      {-0.3093, -5.9719, 3.3825},
         {6.3663, -2.1138, 7.8256},     {0.1212, -0.0675, 0.0267},
         {1.2760, -1.0073, 0.1322},     {-3.3410, -0.4519, -4.3426},
         {-8.0290, -0.1023, -7.7069},   {-1.4716, -4.7326, 0.6895},
         {0.5396, -0.3211, -0.0809},    {-0.1099, 0.3668, 0.2834},
         {-3.8722, 2.4219, -5.0040},    {-2.5818, 2.0105, -1.2765},
         {1.7396, 4.1714, 0.0224},      {0.5811, 1.0689, 0.3569},
         {-1.7973, 3.1572, 1.7284},     {-0.6945, -0.3208, -0.0604},
         {0.5439, -0.2725, -4.4112},    {0.0625, 0.6378, -0.3861},
         {0.0367, -0.0807, 0.0531},     {3.0166, 2.7055, 2.9575},
         {1.6545, -3.9618, 2.7235},     {1.2303, 2.5543, -0.5014},
         {3.6048, 2.7669, 4.1697},      {0.6458, 0.4755, 2.6036},
         {-2.6733, -2.2028, -2.0983},   {-7.3287, -5.0432, 4.5236},
         {-89.4157, -49.1483, 60.4433}, {-1.7304, 0.3371, 1.1864},
         {-4.0836, -1.9287, -2.4582},   {-3.3622, -3.0018, -0.6560},
         {-3.2872, 2.2427, 1.5720},     {-0.0542, -0.1949, -0.0784},
         {-2.3505, -2.7044, 0.3277},    {4.9742, 3.8868, -1.6239},
         {-3.4623, 2.0968, -0.1880},    {-10.1460, 6.3150, -3.7407},
         {0.2339, -0.0912, -0.0384},    {1.4033, -0.7238, -0.9126},
         {5.2820, 2.6779, 4.4389},      {1.4825, 0.0450, -3.4650},
         {0.2170, 0.4866, -0.0668},     {0.8995, 1.3307, 1.1776},
         {-1.2973, 1.1114, 3.4090},     {0.2806, -0.2020, -0.8737},
         {0.1154, 0.0397, 0.0459},      {-0.1713, 0.0431, 0.2383},
         {0.0685, 0.1213, -0.1540},     {0.1191, -0.1956, -0.2146},
         {0.0389, -0.1585, -0.1140},    {-2.5879, 1.5630, -0.0709},
         {-3.6259, -3.7842, 4.5166},    {1.0220, -2.0319, -1.7933},
         {1.7736, 1.2655, 5.3351},      {-3.8957, -0.7766, 4.6440},
         {-1.5791, -0.7411, 0.8524},    {-2.4118, -5.9789, -0.3620},
         {0.0363, 2.8128, -0.1731},     {-3.5930, -6.4554, -8.2569},
         {3.2062, -2.1194, -0.4310},    {-4.3593, -2.9817, 0.0194},
         {-1.4818, -1.3442, -1.8555},   {-0.3415, 1.3223, -2.0361},
         {5.9293, -0.2375, -9.7649},    {-0.8503, 1.0788, 0.2015},
         {5.3757, -0.2620, 2.3448},     {1.3725, -2.8192, -0.6848},
         {1.7703, -1.3085, -4.7080},    {-1.3644, 1.1616, 1.2148},
         {-1.7357, 5.7745, 2.7888},     {6.6723, -2.7169, 5.1616},
         {-0.4704, 3.3630, -0.4653},    {-0.7513, -3.0978, 0.7184},
         {0.4044, -1.3963, 0.1138},     {4.6854, -1.0345, 3.4165},
         {0.1145, -0.0216, -0.0369},    {-3.7497, -0.0668, -2.6245},
         {-0.5078, -0.5396, -0.4368},   {-0.3862, 0.0957, -0.2250},
         {-0.0592, 0.9370, 0.2084}};


      test_begin_with_args(argc, argv);
      initialize();


      energy(calc::v0);
      COMPARE_REALS(esum, ref_e, eps_e);


      energy(calc::v1);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_GRADIENT(ref_g, eps_g);
      for (int i = 0; i < 3; ++i)
         for (int j = 0; j < 3; ++j)
            COMPARE_REALS(vir[i * 3 + j], ref_v[i][j], eps_v);


      energy(calc::v3);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_INTS(count_reduce(nev), ref_count);


      energy(calc::v4);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_GRADIENT(ref_g, eps_g);


      energy(calc::v5);
      COMPARE_GRADIENT(ref_g, eps_g);


      energy(calc::v6);
      COMPARE_GRADIENT(ref_g, eps_g);
      for (int i = 0; i < 3; ++i)
         for (int j = 0; j < 3; ++j)
            COMPARE_REALS(vir[i * 3 + j], ref_v[i][j], eps_v);


      finish();
      test_end();
   }


   SECTION("  - elj -- pbc, cutoff")
   {
      std::string k1 = k0 +
         "\nNEIGHBOR-LIST"
         "\nLIST-BUFFER      0.5"
         "\nCUTOFF           9.0"
         "\nA-AXIS            30"
         "\nB-AXIS            25"
         "\nC-AXIS            20"
         "\n";


      TestFile fxy(xname, x0);
      TestFile fke(kname, k1);
      TestFile fpr(pname, p0);


      const double ref_e = -75.9422;
      const double ref_v[][3] = {{-648.914, -118.602, 31.392},
                                 {-118.602, -489.997, 29.829},
                                 {31.392, 29.829, -460.902}};
      const int ref_count = 7611;
      const double ref_g[][3] = {
         {-2.2164, -0.7234, -1.6869},   {-10.1319, -2.4701, 1.2680},
         {5.0046, 0.5482, 1.3207},      {13.8546, 2.2818, 4.3928},
         {-0.1064, 0.1347, 0.1492},     {-0.1040, 0.1800, 0.1592},
         {-0.0267, 0.1056, 0.1362},     {0.4284, 0.1657, -0.1696},
         {0.7676, 0.6395, 1.0704},      {10.2842, 2.0886, -0.1420},
         {-0.3111, 0.0133, -0.1921},    {-0.0176, 0.0221, -0.0637},
         {-0.0420, -0.0131, 0.0208},    {0.4029, 2.2051, 0.7303},
         {-2.8770, -3.0140, 0.9725},    {-13.7846, -4.6079, 0.7474},
         {-0.5527, -1.5826, 7.8415},    {1.0282, 0.6615, 0.1475},
         {-0.3715, -0.1666, 0.2631},    {-1.8493, -5.8499, 0.1802},
         {3.0889, -3.6117, -0.6172},    {-0.1005, 0.6834, -0.2405},
         {-2.5080, 1.5875, -2.1174},    {1.5359, -0.1195, -4.5871},
         {0.4414, 1.0460, -3.7952},     {-0.3191, -11.0238, -3.0084},
         {0.2163, 0.7015, -0.1474},     {-0.2809, -0.0992, 0.1100},
         {9.2727, -1.8561, 7.8910},     {7.1618, 5.6986, -2.8074},
         {0.3712, -10.2762, 7.3166},    {0.5440, 7.7609, -8.4506},
         {-8.3945, -8.7248, 0.4016},    {-8.2258, -1.0892, -8.8946},
         {0.5298, -1.1042, -0.3092},    {-0.2879, 0.2581, 0.0075},
         {-3.8380, 1.5709, -2.2684},    {1.7740, 2.9981, 2.3811},
         {2.7063, 3.8526, 2.4079},      {7.1812, -4.2263, -5.0605},
         {-0.8609, -0.1197, -0.3865},   {1.0757, 2.2374, -2.5554},
         {3.6870, 1.5636, -1.7723},     {2.7113, 1.5189, 0.5956},
         {2.0283, -2.2248, -3.2507},    {0.5219, 2.0708, 2.8476},
         {-1.7172, -3.5993, 2.9187},    {-3.2700, -1.1798, 3.8772},
         {1.9138, 4.0640, 3.8890},      {-2.0587, 1.8374, -0.1766},
         {0.0732, -0.1973, -0.4031},    {0.8707, -0.4480, 0.5633},
         {0.5267, 0.0871, 0.6281},      {3.7160, -4.3344, -0.1370},
         {0.2814, 0.2360, 0.1205},      {0.0099, 0.0217, -0.0637},
         {-0.0637, -0.0366, -0.0416},   {-3.2581, -0.5651, 0.5125},
         {-1.9839, -2.8229, -3.8719},   {1.5296, 3.7383, 0.0034},
         {-5.9875, -1.7030, 6.0722},    {-0.0185, 0.9456, -0.4329},
         {-0.1215, 1.4975, 1.0204},     {-3.1902, -2.7405, -0.3882},
         {-0.0031, -0.2755, -0.9480},   {4.3191, -1.8089, 8.6858},
         {2.0626, 1.1380, -0.6755},     {-7.6311, 0.6236, 14.3690},
         {10.1965, -0.5748, 1.9787},    {-11.8018, -6.9841, -0.5524},
         {3.9867, 2.1539, -10.2709},    {-3.0713, -4.2017, -9.0012},
         {8.9766, -0.9022, -0.5028},    {-1.4541, 1.3437, -0.7712},
         {4.5496, 1.6097, -1.6994},     {4.2564, 0.4866, -7.0587},
         {2.8942, -7.3298, -13.3921},   {-4.0424, 6.5316, 2.2744},
         {-0.3333, -0.3636, -0.2753},   {0.2933, 2.4976, -0.8547},
         {0.2817, 1.5513, 0.7701},      {1.2047, 1.1613, -2.3921},
         {0.9794, 0.8230, -1.2446},     {3.3966, 0.6967, 3.8238},
         {-2.7489, 6.1146, 0.5687},     {2.5827, 2.1783, -1.5333},
         {-5.2188, 4.7827, -1.1892},    {0.0800, -0.6153, 0.3820},
         {4.7281, 0.0818, 3.2952},      {5.3757, -0.8374, 4.4280},
         {11.5749, 3.7712, 1.5512},     {-1.1730, 1.5600, 0.1748},
         {-0.0349, -0.0502, -0.1086},   {-0.1088, 0.0522, -0.1090},
         {0.0260, -0.0945, -0.1203},    {-2.0847, 0.9106, 4.9337},
         {-4.8335, -1.1902, -3.1371},   {1.1545, -0.2698, -0.7977},
         {98.0660, 57.9110, -61.0547},  {0.0607, -0.2317, 0.0111},
         {1.3703, -1.5175, 0.7714},     {-0.3506, -0.4142, -0.4853},
         {-15.0512, -3.3838, 0.9625},   {-0.9020, -0.4775, -1.1090},
         {-3.5734, 2.5923, 0.0149},     {0.0488, -0.8733, -3.9410},
         {2.1754, 3.4427, 0.5241},      {1.7739, 11.7414, -1.5246},
         {-6.8603, 2.8165, 4.4077},     {0.0134, -2.0730, -1.3332},
         {4.6285, 3.8544, 5.7936},      {-0.3064, -5.9778, 3.3960},
         {6.3692, -2.1176, 7.8289},     {0.1206, -0.0692, 0.0270},
         {1.2702, -1.0129, 0.1311},     {-3.3381, -0.4484, -4.3424},
         {-8.0209, -0.1080, -7.6984},   {-1.4746, -4.7346, 0.6907},
         {0.5215, -0.3216, -0.0785},    {-0.1097, 0.3752, 0.2766},
         {-3.8793, 2.4231, -4.9964},    {-2.5769, 2.0113, -1.2772},
         {1.7273, 4.1809, 0.0190},      {0.5762, 1.0705, 0.3637},
         {-1.7960, 3.1622, 1.7322},     {-0.6945, -0.3201, -0.0609},
         {0.5397, -0.2686, -4.4058},    {0.0575, 0.6424, -0.3884},
         {0.0361, -0.0798, 0.0530},     {3.0223, 2.6996, 2.9573},
         {1.6540, -3.9686, 2.7224},     {1.2319, 2.5549, -0.5045},
         {3.6040, 2.7639, 4.1621},      {0.6466, 0.4753, 2.6037},
         {-2.6864, -2.2174, -2.0986},   {-7.3210, -5.0406, 4.5259},
         {-89.4161, -49.1468, 60.4433}, {-1.7303, 0.3310, 1.1883},
         {-4.0663, -1.9192, -2.4672},   {-3.3498, -2.9918, -0.6629},
         {-3.2942, 2.2410, 1.5740},     {-0.0538, -0.1952, -0.0782},
         {-2.3368, -2.7033, 0.3217},    {4.9764, 3.8990, -1.6356},
         {-3.4690, 2.0952, -0.1840},    {-10.1458, 6.3251, -3.7385},
         {0.2347, -0.0918, -0.0387},    {1.4105, -0.7155, -0.9389},
         {5.2703, 2.6790, 4.4210},      {1.5028, 0.0483, -3.4667},
         {0.2302, 0.4931, -0.0688},     {0.8923, 1.3315, 1.1714},
         {-1.2989, 1.1141, 3.4130},     {0.2827, -0.2033, -0.8727},
         {0.1152, 0.0403, 0.0461},      {-0.1718, 0.0434, 0.2392},
         {0.0681, 0.1215, -0.1526},     {0.1191, -0.1953, -0.2149},
         {0.0385, -0.1585, -0.1124},    {-2.5899, 1.5642, -0.0689},
         {-3.6237, -3.7692, 4.5169},    {1.0153, -2.0233, -1.7913},
         {1.7714, 1.2664, 5.3244},      {-3.8968, -0.7671, 4.6320},
         {-1.5865, -0.7425, 0.8589},    {-2.4031, -5.9633, -0.3625},
         {0.0380, 2.8054, -0.1753},     {-3.5981, -6.4705, -8.2678},
         {3.2007, -2.1180, -0.4318},    {-4.3545, -2.9885, 0.0202},
         {-1.4727, -1.3504, -1.8508},   {-0.3440, 1.3187, -2.0310},
         {5.9243, -0.2389, -9.7631},    {-0.8509, 1.0687, 0.2025},
         {5.3733, -0.2768, 2.3440},     {1.3701, -2.8251, -0.6849},
         {1.7661, -1.3146, -4.7080},    {-1.3811, 1.1651, 1.2168},
         {-1.7421, 5.7911, 2.7910},     {6.6776, -2.7290, 5.1516},
         {-0.4653, 3.3768, -0.4683},    {-0.7487, -3.1300, 0.7180},
         {0.4081, -1.4233, 0.1184},     {4.6940, -1.0748, 3.4169},
         {0.1153, -0.0190, -0.0371},    {-3.7532, -0.1327, -2.6158},
         {-0.5147, -0.5606, -0.4352},   {-0.3874, 0.0934, -0.2250},
         {-0.0650, 0.9019, 0.2167}};


      test_begin_with_args(argc, argv);
      initialize();


      energy(calc::v0);
      COMPARE_REALS(esum, ref_e, eps_e);


      energy(calc::v1);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_GRADIENT(ref_g, eps_g);
      for (int i = 0; i < 3; ++i)
         for (int j = 0; j < 3; ++j)
            COMPARE_REALS(vir[i * 3 + j], ref_v[i][j], eps_v);


      energy(calc::v3);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_INTS(count_reduce(nev), ref_count);


      energy(calc::v4);
      COMPARE_REALS(esum, ref_e, eps_e);
      COMPARE_GRADIENT(ref_g, eps_g);


      energy(calc::v5);
      COMPARE_GRADIENT(ref_g, eps_g);


      energy(calc::v6);
      COMPARE_GRADIENT(ref_g, eps_g);
      for (int i = 0; i < 3; ++i)
         for (int j = 0; j < 3; ++j)
            COMPARE_REALS(vir[i * 3 + j], ref_v[i][j], eps_v);


      finish();
      test_end();
   }
}
