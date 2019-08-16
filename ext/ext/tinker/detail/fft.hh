#ifndef TINKER_MOD_FFT_HH_
#define TINKER_MOD_FFT_HH_

#include "macro.h"

TINKER_NAMESPACE_BEGIN namespace fft {
const int maxprime = 15;
extern int (&iprime)[3][maxprime];
extern unsigned long long& planf;
extern unsigned long long& planb;
extern double*& ffttable;
extern char (&ffttyp)[7];

#ifdef TINKER_MOD_CPP_
extern "C" int TINKER_MOD(fft, iprime)[3][maxprime];
extern "C" unsigned long long TINKER_MOD(fft, planf);
extern "C" unsigned long long TINKER_MOD(fft, planb);
extern "C" double* TINKER_MOD(fft, ffttable);
extern "C" char TINKER_MOD(fft, ffttyp)[7];

int (&iprime)[3][maxprime] = TINKER_MOD(fft, iprime);
unsigned long long& planf = TINKER_MOD(fft, planf);
unsigned long long& planb = TINKER_MOD(fft, planb);
double*& ffttable = TINKER_MOD(fft, ffttable);
char (&ffttyp)[7] = TINKER_MOD(fft, ffttyp);
#endif
} TINKER_NAMESPACE_END

#endif