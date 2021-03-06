# Procedures


## Main Program
   - Initialize the Fortran runtime library.
   - Initialize the canonical Tinker library.
      - `call initial`, `call getxyz`, `call mechanic`, etc.
   - Initialize `tinker.gpu`.
      - Assign/hard code the global flag `rc_flag`.
      - `initialize()`, for details see [**Initialize**](#ctor).
   - MM/MD calculation, often needs `energy()`.
     For details, see [**Evaluate**](#evaleng).
   - Stop `tinker.gpu`.
      - `finish()`: recursively unset the data structures
        set in [**Initialize**](#ctor).
   - Stop the canonical Tinker library.
      - `call final`.
   - Stop the Fortran runtime library.


<a name='ctor'></a>
## Initialize
   - `device_data()`:
      - `n_data()`: set number of atoms.
      - `box_data()`: set the shape of the periodic boundary condition (PBC),
         including non-PBC.
      - `xyz_data()`, `vel_data()`, `mass_data()`: set coordinates,
         velocity, and mass data.
      - `energy_data()`: for details see
         [**Create Energy Terms**](#ctoreng).
      - `nblist_data()`: set the neighbor list, which must be set after
         the energy terms.
         - The vdw list needs `xred`, `yred`, and `zred`, which are set
           in `evdw_data()`.
         - The preconditioner needs to know the `uscale` exclusion pairs
           which are processed in `epolar_data()`.
      - `md_data()`: return if `!calc::md`.


<a name='ctoreng'></a>
## Create Energy Terms
   - Return if `rc_flag & calc::vmask == 0`.
   - `egv_data()`:
      - `ev_data()`:
         - Return if `!calc::analyz` and `!calc::energy` and `!calc::virial`.
         - If `calc::analyz`, the global energy and virial buffers are set to
           `nullptr`.
         - If `!calc::analyz`, allocate the global energy and virial buffers,
           then register them in the global bookkeepings.
      - `grad_data()`: Return if `!calc::grad`.
   - Set bonded terms.
   - Set nonbonded terms.
      - `evdw_data()`.
      - `elec_data()`: return if `!use_elec()`.
         - Set permanent multipoles, induced dipoles, and torques.
         - Only if `use_ewald()`, set PME parameters, PME arrays,
           and FFT plans.
      - `empole_data()`.
      - `epolar_data()`.


<a name='evaleng'></a>
## Evaluate
   - `zero_egv()`:
      - If use `calc::analyz`, `calc::energy`, `calc::virial`, iterate and
        zero out over all of the count, energy, and virial buffers,
        respectivley.
   - Evaluate bonded terms.
   - Evaluate nonbonded terms.
      - `evdw()`.
      - `mpole_init()`:
         - Return if `!use_elec()`.
         - If `calc::grad`, zero out torques.
         - If `calc::virial`, zero out torque virial buffer.
         - Check and rotate multipoles.
         - If `use_ewald()`:
            - Copy `rpole` to `cmp`.
            - May or may not need to zero out the PME work virial buffer
              (`vir_m`).
      - `empole()` and `epolar()`.
      - `torque()`:
         - Return if `!use_elec()`.
         - If `calc::virial`, convert torques to gradients and virials;
           Else if `calc::grad`, only convert torques to gradients.
   - `sum_energy()`:
      - If `calc::energy`, `calc::virial`, iterate and sum over all of the
        energy and virial buffers, respectively.
      - Of course, if `!calc::analyz`, there is only one buffer each for
        energy and virial so no need to iterate over the bookkeepings.
