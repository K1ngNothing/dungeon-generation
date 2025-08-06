#pragma once

#include <petsc.h>
#include <petsc/private/taoimpl.h>

typedef struct {
    Tao subsolver, parent;          /* subsolver for aug-lag subproblem */
    PetscErrorCode (*sub_obj)(Tao); /* subsolver objective function */
    TaoALMMType type;               /* subsolver objective type */

    IS *Pis, *Yis;                   /* index sets to separate primal and dual vector spaces */
    VecScatter *Pscatter, *Yscatter; /* scatter objects to write into combined vector spaces */

    Mat Ae, Ai;                              /* aliased constraint Jacobians (do not destroy!) */
    Vec Px, LgradX, Ce, Ci, G;               /* aliased vectors (do not destroy!) */
    Vec Ps, LgradS, Yi, Ye;                  /* sub-vectors for primal variables */
    Vec *Parr, P, PL, PU, *Yarr, Y, C;       /* arrays and vectors for combined vector spaces */
    Vec Psub, Xwork, Cework, Ciwork, Cizero; /* work vectors */

    PetscReal Lval, fval, gnorm, cnorm, cenorm, cinorm, cnorm_old; /* scalar variables */
    PetscReal mu0, mu, mu_fac, mu_pow_good, mu_pow_bad;            /* penalty parameters */
    PetscReal ytol0, ytol, gtol0, gtol;                            /* convergence parameters */
    PetscReal mu_max, ye_min, yi_min, ye_max, yi_max;              /* parameter safeguards */

    PetscBool info;
} TAO_ALMM;
