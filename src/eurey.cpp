#include "eurey.h"
#include "md.h"
#include "potent.h"
#include <tinker/detail/urey.hh>
#include <tinker/detail/urypot.hh>

namespace tinker {
void eurey_data(rc_op op)
{
   if (!use_potent(urey_term))
      return;

   if (op & rc_dealloc) {
      darray::deallocate(iury, uk, ul);

      buffer_deallocate(rc_flag, eub, deubx, deuby, deubz, vir_eub);
   }

   if (op & rc_alloc) {
      int nangle = count_bonded_term(angle_term);
      darray::allocate(nangle, &iury, &uk, &ul);

      nurey = count_bonded_term(urey_term);
      buffer_allocate(rc_flag, &eub, &deubx, &deuby, &deubz, &vir_eub,
                      &energy_eub);
   }

   if (op & rc_init) {
      int nangle = count_bonded_term(angle_term);
      std::vector<int> ibuf(3 * nangle);
      for (int i = 0; i < 3 * nangle; ++i)
         ibuf[i] = urey::iury[i] - 1;
      darray::copyin(WAIT_NEW_Q, nangle, iury, ibuf.data());
      darray::copyin(WAIT_NEW_Q, nangle, uk, urey::uk);
      darray::copyin(WAIT_NEW_Q, nangle, ul, urey::ul);

      cury = urypot::cury;
      qury = urypot::qury;
      ureyunit = urypot::ureyunit;
   }
}

void eurey(int vers)
{
   eurey_acc(vers);
}
}
