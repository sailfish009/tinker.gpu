#pragma once
#include "darray.h"
#include "energy_buffer.h"
#include "rc_man.h"

namespace tinker {
TINKER_EXTERN int ntors;
TINKER_EXTERN pointer<int, 4> itors;
TINKER_EXTERN pointer<real, 4> tors1, tors2, tors3, tors4, tors5, tors6;
TINKER_EXTERN real torsunit;

TINKER_EXTERN energy_buffer et;
TINKER_EXTERN virial_buffer vir_et;
TINKER_EXTERN grad_prec *detx, *dety, *detz;
TINKER_EXTERN energy_prec energy_et;

void etors_data(rc_op op);

void etors(int vers);
void etors_acc(int);
}
