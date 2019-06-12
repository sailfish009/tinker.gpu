#ifndef TINKER_GPU_E_POLAR_H_
#define TINKER_GPU_E_POLAR_H_

#include "decl_real.h"
#include "gpu/decl_elec.h"
#include "gpu/decl_polgrp.h"

TINKER_NAMESPACE_BEGIN
namespace gpu {
extern int epolar_electyp;
extern std::string epolar_electyp_str;

extern real* polarity;
extern real* thole;
extern real* pdamp;
extern real* polarity_inv;

extern real* ep;
extern int* nep;
extern real* vir_ep;

extern real (*ufld)[3];
extern real (*dufld)[6];

extern real (*work01__)[3];
extern real (*work02__)[3];
extern real (*work03__)[3];
extern real (*work04__)[3];
extern real (*work05__)[3];
extern real (*work06__)[3];
extern real (*work07__)[3];
extern real (*work08__)[3];
extern real (*work09__)[3];
extern real (*work10__)[3];

int use_epolar();
void get_epolar_type(int& typ, std::string& typ_str);
void e_polar_data(int op);

// see also subroutine epolar0e in epolar.f
void epolar0_dotprod(const real (*gpu_uind)[3], const real (*gpu_udirp)[3]);

// electrostatic field due to permanent multipoles
void dfield_coulomb(real* gpu_field, real* gpu_fieldp);
void dfield_ewald(real* gpu_field, real* gpu_fieldp);
void dfield_ewald_recip_self(real* gpu_field);
void dfield_ewald_real(real* gpu_field, real* gpu_fieldp);

// mutual electrostatic field due to induced dipole moments
void ufield_coulomb(const real* gpu_uind, const real* gpu_uinp, real* gpu_field,
                    real* gpu_fieldp);
void ufield_ewald(const real* gpu_uind, const real* gpu_uinp, real* gpu_field,
                  real* gpu_fieldp);
void ufield_ewald_recip_self(const real* gpu_uind, const real* gpu_uinp,
                             real* gpu_field, real* gpu_fieldp);
void ufield_ewald_real(const real* gpu_uind, const real* gpu_uinp,
                       real* gpu_field, real* gpu_fieldp);

void dfield(real* gpu_field, real* gpu_fieldp);
// -Tu operator
void ufield(const real* gpu_uind, const real* gpu_uinp, real* gpu_field,
            real* gpu_fieldp);

// different induction algorithms
void induce_mutual_pcg1(real* gpu_ud, real* gpu_up);
void induce(real* gpu_ud, real* gpu_up);
}
TINKER_NAMESPACE_END

extern "C" {
void tinker_gpu_epolar_coulomb0();
void tinker_gpu_epolar_coulomb1();
void tinker_gpu_epolar_coulomb3();
void tinker_gpu_epolar_coulomb4();
void tinker_gpu_epolar_coulomb5();
void tinker_gpu_epolar_coulomb6();

void tinker_gpu_epolar_ewald0();
void tinker_gpu_epolar_ewald1();
void tinker_gpu_epolar_ewald3();
void tinker_gpu_epolar_ewald4();
void tinker_gpu_epolar_ewald5();
void tinker_gpu_epolar_ewald6();

void tinker_gpu_epolar0();
void tinker_gpu_epolar1();
void tinker_gpu_epolar3();
void tinker_gpu_epolar4();
void tinker_gpu_epolar5();
void tinker_gpu_epolar6();
}

#endif
