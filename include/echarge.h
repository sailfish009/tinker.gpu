#pragma once
#include "elec.h"
#include "energy_buffer.h"
#include "rc_man.h"


namespace tinker {
extern real ebuffer;
extern real c2scale, c3scale, c4scale, c5scale;
extern int ncexclude;
extern int (*cexclude)[2];
extern real* cexclude_scale;
extern count_buffer nec;
extern energy_buffer ec;
extern virial_buffer vir_ec;
extern grad_prec *decx, *decy, *decz;
extern energy_prec energy_ec;


void echarge_data(rc_op);


void echarge(int vers);
void echarge_nonewald(int vers);
void echarge_ewald(int vers);


void echarge_nonewald_acc(int);
void echarge_nonewald_cu(int);
void echarge_ewald_real_acc(int);
void echarge_ewald_real_cu(int);
void echarge_ewald_fphi_self_acc(int);
void echarge_ewald_fphi_self_cu(int);
}
