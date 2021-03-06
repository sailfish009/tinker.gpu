#include "echarge.h"
#include "mdcalc.h"
#include "mdpq.h"
#include "nblist.h"
#include "pmestuf.h"
#include "potent.h"
#include <tinker/detail/chgpot.hh>
#include <tinker/detail/couple.hh>
#include <tinker/detail/sizes.hh>


namespace tinker {
real ebuffer;
real c2scale, c3scale, c4scale, c5scale;
int ncexclude;
int (*cexclude)[2];
real* cexclude_scale;
count_buffer nec;
energy_buffer ec;
virial_buffer vir_ec;
grad_prec *decx, *decy, *decz;
energy_prec energy_ec;


void echarge_data(rc_op op)
{
   if (!use_potent(charge_term))
      return;


   if (op & rc_dealloc) {
      ncexclude = 0;
      darray::deallocate(cexclude, cexclude_scale);

      if (rc_flag & calc::analyz) {
         buffer_deallocate(calc::analyz, nec);
      }
      buffer_deallocate(rc_flag | calc::analyz, ec, decx, decy, decz, vir_ec);
   }


   if (op & rc_alloc) {
      ebuffer = chgpot::ebuffer;


      c2scale = chgpot::c2scale;
      c3scale = chgpot::c3scale;
      c4scale = chgpot::c4scale;
      c5scale = chgpot::c5scale;


      std::vector<int> exclik;
      std::vector<real> excl;
      // see attach.f
      const int maxn13 = 3 * sizes::maxval;
      const int maxn14 = 9 * sizes::maxval;
      const int maxn15 = 27 * sizes::maxval;
      for (int i = 0; i < n; ++i) {
         int nn;
         int bask;
         if (c2scale != 1) {
            nn = couple::n12[i];
            for (int j = 0; j < nn; ++j) {
               int k = couple::i12[i][j];
               k -= 1;
               if (k > i) {
                  exclik.push_back(i);
                  exclik.push_back(k);
                  excl.push_back(c2scale - 1);
               }
            }
         }
         if (c3scale != 1) {
            nn = couple::n13[i];
            bask = i * maxn13;
            for (int j = 0; j < nn; ++j) {
               int k = couple::i13[bask + j];
               k -= 1;
               if (k > i) {
                  exclik.push_back(i);
                  exclik.push_back(k);
                  excl.push_back(c3scale - 1);
               }
            }
         }
         if (c4scale != 1) {
            nn = couple::n14[i];
            bask = i * maxn14;
            for (int j = 0; j < nn; ++j) {
               int k = couple::i14[bask + j];
               k -= 1;
               if (k > i) {
                  exclik.push_back(i);
                  exclik.push_back(k);
                  excl.push_back(c4scale - 1);
               }
            }
         }
         if (c5scale != 1) {
            nn = couple::n15[i];
            bask = i * maxn15;
            for (int j = 0; j < nn; ++j) {
               int k = couple::i15[bask + j];
               k -= 1;
               if (k > i) {
                  exclik.push_back(i);
                  exclik.push_back(k);
                  excl.push_back(c5scale - 1);
               }
            }
         }
      }
      ncexclude = excl.size();
      darray::allocate(ncexclude, &cexclude, &cexclude_scale);
      darray::copyin(WAIT_NEW_Q, ncexclude, cexclude, exclik.data());
      darray::copyin(WAIT_NEW_Q, ncexclude, cexclude_scale, excl.data());


      if (rc_flag & calc::analyz) {
         buffer_allocate(calc::analyz, &nec);
      }
      buffer_allocate(rc_flag | calc::analyz, &ec, &decx, &decy, &decz, &vir_ec,
                      &energy_ec);
   }


   if (op & rc_init) {
   }
}


void echarge(int vers)
{
   if (use_ewald())
      echarge_ewald(vers);
   else
      echarge_nonewald(vers);
}


void echarge_nonewald(int vers)
{
#if TINKER_CUDART
   if (clist_version() & NBL_SPATIAL)
      echarge_nonewald_cu(vers);
   else
#endif
      echarge_nonewald_acc(vers);
}


void echarge_ewald(int vers)
{
   // ewald recip space, self term
   // ewald real space


   const PMEUnit pu = epme_unit;
   grid_pchg(pu, pchg);
   fftfront(pu);
   if (vers & calc::virial) {
      if (vers & calc::energy) {
         pme_conv(pu, ec, vir_ec);
      } else {
         pme_conv(pu, vir_ec);
      }
   } else {
      if (vers & calc::energy) {
         pme_conv(pu, ec);
      } else {
         pme_conv(pu);
      }
   }
   fftback(pu);

   // fphi_pchg, recip, self

#if TINKER_CUDART
   if (pltfm_config & CU_PLTFM)
      echarge_ewald_fphi_self_cu(vers);
   else
#endif
      echarge_ewald_fphi_self_acc(vers);


#if TINKER_CUDART
   if (clist_version() & NBL_SPATIAL)
      echarge_ewald_real_cu(vers);
   else
#endif
      echarge_ewald_real_acc(vers);
}
}
