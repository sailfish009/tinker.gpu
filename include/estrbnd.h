#pragma once
#include "darray.h"
#include "energy_buffer.h"
#include "rc_man.h"

namespace tinker {
TINKER_EXTERN int nstrbnd;
TINKER_EXTERN pointer<int, 3> isb;
TINKER_EXTERN pointer<real, 2> sbk;
TINKER_EXTERN real stbnunit;

TINKER_EXTERN energy_buffer eba;
TINKER_EXTERN virial_buffer vir_eba;
TINKER_EXTERN grad_prec *debax, *debay, *debaz;
TINKER_EXTERN energy_prec energy_eba;

void estrbnd_data(rc_op op);

void estrbnd(int vers);
void estrbnd_acc(int);
}
