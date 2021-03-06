#include "add.h"
#include "epolar.h"
#include "error.h"
#include "field.h"
#include "gpu_card.h"
#include "image.h"
#include "induce.h"
#include "io_print.h"
#include "nblist.h"
#include "seq_damp.h"
#include "tinker_rt.h"
#include <tinker/detail/inform.hh>
#include <tinker/detail/polpcg.hh>
#include <tinker/detail/polpot.hh>
#include <tinker/detail/units.hh>

namespace tinker {
void diag_precond(const real (*rsd)[3], const real (*rsdp)[3], real (*zrsd)[3],
                  real (*zrsdp)[3])
{
   #pragma acc parallel loop independent async\
               deviceptr(polarity,rsd,rsdp,zrsd,zrsdp)
   for (int i = 0; i < n; ++i) {
      real poli = polarity[i];
      #pragma acc loop seq
      for (int j = 0; j < 3; ++j) {
         zrsd[i][j] = poli * rsd[i][j];
         zrsdp[i][j] = poli * rsdp[i][j];
      }
   }
}

#define APPLY_DPTRS rsd, rsdp, zrsd, zrsdp, x, y, z, polarity, pdamp, thole
void sparse_precond_apply_acc(const real (*rsd)[3], const real (*rsdp)[3],
                              real (*zrsd)[3], real (*zrsdp)[3])
{
   #pragma acc parallel loop independent async\
               deviceptr(polarity,rsd,rsdp,zrsd,zrsdp)
   for (int i = 0; i < n; ++i) {
      real poli = udiag * polarity[i];
      #pragma acc loop seq
      for (int j = 0; j < 3; ++j) {
         zrsd[i][j] = poli * rsd[i][j];
         zrsdp[i][j] = poli * rsdp[i][j];
      }
   }

   const int maxnlst = ulist_unit->maxnlst;
   const auto* ulst = ulist_unit.deviceptr();

   MAYBE_UNUSED int GRID_DIM = get_grid_size(BLOCK_DIM);
   #pragma acc parallel async num_gangs(GRID_DIM) vector_length(BLOCK_DIM)\
               deviceptr(APPLY_DPTRS,ulst)
   #pragma acc loop gang independent
   for (int i = 0; i < n; ++i) {
      real xi = x[i];
      real yi = y[i];
      real zi = z[i];
      real pdi = pdamp[i];
      real pti = thole[i];
      real poli = polarity[i];

      int nulsti = ulst->nlst[i];
      int base = i * maxnlst;
      real gxi = 0, gyi = 0, gzi = 0;
      real txi = 0, tyi = 0, tzi = 0;

      #pragma acc loop vector independent reduction(+:gxi,gyi,gzi,txi,tyi,tzi)
      for (int kk = 0; kk < nulsti; ++kk) {
         int k = ulst->lst[base + kk];
         real xr = x[k] - xi;
         real yr = y[k] - yi;
         real zr = z[k] - zi;

         real r2 = image2(xr, yr, zr);
         real r = REAL_SQRT(r2);

         real scale3, scale5;
         damp_thole2(r, pdi, pti, pdamp[k], thole[k], scale3, scale5);

         real polik = poli * polarity[k];
         real rr3 = scale3 * polik * REAL_RECIP(r * r2);
         real rr5 = 3 * scale5 * polik * REAL_RECIP(r * r2 * r2);

         real m0 = rr5 * xr * xr - rr3;
         real m1 = rr5 * xr * yr;
         real m2 = rr5 * xr * zr;
         real m3 = rr5 * yr * yr - rr3;
         real m4 = rr5 * yr * zr;
         real m5 = rr5 * zr * zr - rr3;

         gxi += m0 * rsd[k][0] + m1 * rsd[k][1] + m2 * rsd[k][2];
         gyi += m1 * rsd[k][0] + m3 * rsd[k][1] + m4 * rsd[k][2];
         gzi += m2 * rsd[k][0] + m4 * rsd[k][1] + m5 * rsd[k][2];

         atomic_add(m0 * rsd[i][0] + m1 * rsd[i][1] + m2 * rsd[i][2],
                    &zrsd[k][0]);
         atomic_add(m1 * rsd[i][0] + m3 * rsd[i][1] + m4 * rsd[i][2],
                    &zrsd[k][1]);
         atomic_add(m2 * rsd[i][0] + m4 * rsd[i][1] + m5 * rsd[i][2],
                    &zrsd[k][2]);

         txi += m0 * rsdp[k][0] + m1 * rsdp[k][1] + m2 * rsdp[k][2];
         tyi += m1 * rsdp[k][0] + m3 * rsdp[k][1] + m4 * rsdp[k][2];
         tzi += m2 * rsdp[k][0] + m4 * rsdp[k][1] + m5 * rsdp[k][2];

         atomic_add(m0 * rsdp[i][0] + m1 * rsdp[i][1] + m2 * rsdp[i][2],
                    &zrsdp[k][0]);
         atomic_add(m1 * rsdp[i][0] + m3 * rsdp[i][1] + m4 * rsdp[i][2],
                    &zrsdp[k][1]);
         atomic_add(m2 * rsdp[i][0] + m4 * rsdp[i][1] + m5 * rsdp[i][2],
                    &zrsdp[k][2]);
      }

      atomic_add(gxi, &zrsd[i][0]);
      atomic_add(gyi, &zrsd[i][1]);
      atomic_add(gzi, &zrsd[i][2]);

      atomic_add(txi, &zrsdp[i][0]);
      atomic_add(tyi, &zrsdp[i][1]);
      atomic_add(tzi, &zrsdp[i][2]);
   }

   #pragma acc parallel loop async\
               deviceptr(APPLY_DPTRS,uexclude,uexclude_scale)
   for (int ii = 0; ii < nuexclude; ++ii) {
      int i = uexclude[ii][0];
      int k = uexclude[ii][1];
      real uscale = uexclude_scale[ii];

      real xi = x[i];
      real yi = y[i];
      real zi = z[i];
      real pdi = pdamp[i];
      real pti = thole[i];
      real poli = polarity[i];

      real xr = x[k] - xi;
      real yr = y[k] - yi;
      real zr = z[k] - zi;

      real r2 = image2(xr, yr, zr);
      real r = REAL_SQRT(r2);

      real scale3, scale5;
      damp_thole2(r, pdi, pti, pdamp[k], thole[k], scale3, scale5);
      scale3 *= uscale;
      scale5 *= uscale;

      real polik = poli * polarity[k];
      real rr3 = scale3 * polik * REAL_RECIP(r * r2);
      real rr5 = 3 * scale5 * polik * REAL_RECIP(r * r2 * r2);

      real m0 = rr5 * xr * xr - rr3;
      real m1 = rr5 * xr * yr;
      real m2 = rr5 * xr * zr;
      real m3 = rr5 * yr * yr - rr3;
      real m4 = rr5 * yr * zr;
      real m5 = rr5 * zr * zr - rr3;

      atomic_add(m0 * rsd[k][0] + m1 * rsd[k][1] + m2 * rsd[k][2], &zrsd[i][0]);
      atomic_add(m1 * rsd[k][0] + m3 * rsd[k][1] + m4 * rsd[k][2], &zrsd[i][1]);
      atomic_add(m2 * rsd[k][0] + m4 * rsd[k][1] + m5 * rsd[k][2], &zrsd[i][2]);

      atomic_add(m0 * rsd[i][0] + m1 * rsd[i][1] + m2 * rsd[i][2], &zrsd[k][0]);
      atomic_add(m1 * rsd[i][0] + m3 * rsd[i][1] + m4 * rsd[i][2], &zrsd[k][1]);
      atomic_add(m2 * rsd[i][0] + m4 * rsd[i][1] + m5 * rsd[i][2], &zrsd[k][2]);

      atomic_add(m0 * rsdp[k][0] + m1 * rsdp[k][1] + m2 * rsdp[k][2],
                 &zrsdp[i][0]);
      atomic_add(m1 * rsdp[k][0] + m3 * rsdp[k][1] + m4 * rsdp[k][2],
                 &zrsdp[i][1]);
      atomic_add(m2 * rsdp[k][0] + m4 * rsdp[k][1] + m5 * rsdp[k][2],
                 &zrsdp[i][2]);

      atomic_add(m0 * rsdp[i][0] + m1 * rsdp[i][1] + m2 * rsdp[i][2],
                 &zrsdp[k][0]);
      atomic_add(m1 * rsdp[i][0] + m3 * rsdp[i][1] + m4 * rsdp[i][2],
                 &zrsdp[k][1]);
      atomic_add(m2 * rsdp[i][0] + m4 * rsdp[i][1] + m5 * rsdp[i][2],
                 &zrsdp[k][2]);
   }
}

/**
 * PCG
 *
 * M = preconditioner
 * T = inv_alpha + Tu, -Tu = ufield
 *
 * r(0) = E - T u(0)
 * p(0) (or conj(0)) = M r(0)
 * ------------------------------
 * gamma (or a) = r(i) M r(i) / p(i) T p(i)
 * u(i+1) = u(i) + a p(i)
 * r(i+1) = r(i) - a T p(i)
 * beta (or b(i+1)) = r(i+1) M r(i+1) / r(i) M r(i)
 * p(i+1) = M r(r+1) + b(i+1) p(i)
 * ------------------------------
 *
 * subroutine induce0a in induce.f
 * rsd = r
 * zrsd = M r
 * conj = p
 * vec = T P
 */
void induce_mutual_pcg1_acc(real (*uind)[3], real (*uinp)[3])
{
   auto* field = work01_;
   auto* fieldp = work02_;
   auto* rsd = work03_;
   auto* rsdp = work04_;
   auto* zrsd = work05_;
   auto* zrsdp = work06_;
   auto* conj = work07_;
   auto* conjp = work08_;
   auto* vec = work09_;
   auto* vecp = work10_;

   const bool dirguess = polpcg::pcgguess;
   // use sparse matrix preconditioner
   // or just use diagonal matrix preconditioner
   const bool sparse_prec = polpcg::pcgprec;

   // zero out the induced dipoles at each site

   darray::zero(PROCEED_NEW_Q, n, uind, uinp);

   // get the electrostatic field due to permanent multipoles

   dfield(field, fieldp);

   // direct induced dipoles

   #pragma acc parallel loop independent async\
               deviceptr(polarity,udir,udirp,field,fieldp)
   for (int i = 0; i < n; ++i) {
      real poli = polarity[i];
      #pragma acc loop seq
      for (int j = 0; j < 3; ++j) {
         udir[i][j] = poli * field[i][j];
         udirp[i][j] = poli * fieldp[i][j];
      }
   }

   if (dirguess) {
      darray::copy(PROCEED_NEW_Q, n, uind, udir);
      darray::copy(PROCEED_NEW_Q, n, uinp, udirp);
   }

   // initial residual r(0)

   // if do not use pcgguess, r(0) = E - T Zero = E
   // if use pcgguess, r(0) = E - (inv_alpha + Tu) alpha E
   //                       = E - E -Tu udir
   //                       = -Tu udir

   if (dirguess) {
      ufield(udir, udirp, rsd, rsdp);
   } else {
      darray::copy(PROCEED_NEW_Q, n, rsd, field);
      darray::copy(PROCEED_NEW_Q, n, rsdp, fieldp);
   }
   #pragma acc parallel loop independent async deviceptr(polarity,rsd,rsdp)
   for (int i = 0; i < n; ++i) {
      if (polarity[i] == 0) {
         rsd[i][0] = 0;
         rsd[i][1] = 0;
         rsd[i][2] = 0;
         rsdp[i][0] = 0;
         rsdp[i][1] = 0;
         rsdp[i][2] = 0;
      }
   }

   // initial M r(0) and p(0)

   if (sparse_prec) {
      sparse_precond_build();
      sparse_precond_apply(rsd, rsdp, zrsd, zrsdp);
   } else {
      diag_precond(rsd, rsdp, zrsd, zrsdp);
   }
   darray::copy(PROCEED_NEW_Q, n, conj, zrsd);
   darray::copy(PROCEED_NEW_Q, n, conjp, zrsdp);

   // initial r(0) M r(0)

   real sum, sump;
   sum = darray::dot(WAIT_NEW_Q, n, rsd, zrsd);
   sump = darray::dot(WAIT_NEW_Q, n, rsdp, zrsdp);

   // conjugate gradient iteration of the mutual induced dipoles

   const bool debug = inform::debug;
   const int politer = polpot::politer;
   const real poleps = polpot::poleps;
   const real debye = units::debye;
   const real pcgpeek = polpcg::pcgpeek;
   const int maxiter = 100; // see also subroutine induce0a in induce.f

   bool done = false;
   int iter = 0;
   real eps = 100;
   real epsold;

   while (!done) {
      ++iter;

      // vec = (inv_alpha + Tu) conj, field = -Tu conj
      // vec = inv_alpha * conj - field
      ufield(conj, conjp, field, fieldp);
      #pragma acc parallel loop independent async\
                  deviceptr(polarity_inv,vec,vecp,conj,conjp,field,fieldp)
      for (int i = 0; i < n; ++i) {
         real poli_inv = polarity_inv[i];
         #pragma acc loop seq
         for (int j = 0; j < 3; ++j) {
            vec[i][j] = poli_inv * conj[i][j] - field[i][j];
            vecp[i][j] = poli_inv * conjp[i][j] - fieldp[i][j];
         }
      }

      // a <- p T p
      real a, ap;
      a = darray::dot(WAIT_NEW_Q, n, conj, vec);
      ap = darray::dot(WAIT_NEW_Q, n, conjp, vecp);
      // a <- r M r / p T p
      if (a != 0)
         a = sum / a;
      if (ap != 0)
         ap = sump / ap;

      // u <- u + a p
      // r <- r - a T p
      #pragma acc parallel loop independent async\
                  deviceptr(polarity,uind,uinp,conj,conjp,rsd,rsdp,vec,vecp)
      for (int i = 0; i < n; ++i) {
         #pragma acc loop seq
         for (int j = 0; j < 3; ++j) {
            uind[i][j] += a * conj[i][j];
            uinp[i][j] += ap * conjp[i][j];
            rsd[i][j] -= a * vec[i][j];
            rsdp[i][j] -= ap * vecp[i][j];
         }
         if (polarity[i] == 0) {
            rsd[i][0] = 0;
            rsd[i][1] = 0;
            rsd[i][2] = 0;
            rsdp[i][0] = 0;
            rsdp[i][1] = 0;
            rsdp[i][2] = 0;
         }
      }

      // calculate/update M r
      if (sparse_prec)
         sparse_precond_apply(rsd, rsdp, zrsd, zrsdp);
      else
         diag_precond(rsd, rsdp, zrsd, zrsdp);

      real b, bp;
      real sum1, sump1;
      sum1 = darray::dot(WAIT_NEW_Q, n, rsd, zrsd);
      sump1 = darray::dot(WAIT_NEW_Q, n, rsdp, zrsdp);
      if (sum != 0)
         b = sum1 / sum;
      if (sump != 0)
         bp = sump1 / sump;


      // calculate/update p
      #pragma acc parallel loop independent async\
                  deviceptr(conj,conjp,zrsd,zrsdp)
      for (int i = 0; i < n; ++i) {
         #pragma acc loop seq
         for (int j = 0; j < 3; ++j) {
            conj[i][j] = zrsd[i][j] + b * conj[i][j];
            conjp[i][j] = zrsdp[i][j] + bp * conjp[i][j];
         }
      }


      sum = sum1;
      sump = sump1;

      real epsd;
      real epsp;
      epsd = darray::dot(WAIT_NEW_Q, n, rsd, rsd);
      epsp = darray::dot(WAIT_NEW_Q, n, rsdp, rsdp);

      epsold = eps;
      eps = REAL_MAX(epsd, epsp);
      eps = debye * REAL_SQRT(eps / n);

      if (debug) {
         if (iter == 1) {
            print(stdout,
                  "\n Determination of SCF Induced Dipole Moments\n\n"
                  "    Iter    RMS Residual (Debye)\n\n");
         }
         print(stdout, " %8d       %-16.10f\n", iter, eps);
      }

      if (eps < poleps)
         done = true;
      if (eps > epsold)
         done = true;
      if (iter >= politer)
         done = true;

      // apply a "peek" iteration to the mutual induced dipoles

      if (done) {
         #pragma acc parallel loop independent async\
                     deviceptr(polarity,uind,uinp,rsd,rsdp)
         for (int i = 0; i < n; ++i) {
            real term = pcgpeek * polarity[i];
            #pragma acc loop seq
            for (int j = 0; j < 3; ++j) {
               uind[i][j] += term * rsd[i][j];
               uinp[i][j] += term * rsdp[i][j];
            }
         }
      }
   }

   // print the results from the conjugate gradient iteration

   if (debug) {
      print(stdout,
            " Induced Dipoles :    Iterations %4d      RMS "
            "Residual %14.10f\n",
            iter, eps);
   }

   // terminate the calculation if dipoles failed to converge

   if (iter >= maxiter || eps > epsold) {
      TINKER_RT(prterr)();
      TINKER_THROW("INDUCE  --  Warning, Induced Dipoles are not Converged");
   }
}


void induce_mutual_pcg1(real (*uind)[3], real (*uinp)[3])
{
#if TINKER_CUDART
   if (pltfm_config & CU_PLTFM)
      induce_mutual_pcg1_cu(uind, uinp);
   else
#endif
      induce_mutual_pcg1_acc(uind, uinp);
}
}
