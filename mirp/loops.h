/*! \file
 *
 * \brief Helpers for looping over cartesian and primitive integrals
 */

#pragma once

#include "mirp/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************
 * Double precision
 *******************************************/

/*! \brief Compute all cartesian components of a single primitive integral
 *         (four center, double precision)
 *
 * The \p output buffer is expected to be able to hold all primitive integrals
 * (ie, it can hold ncart(am1) * ncart(am2) * ncart(am3) * ncart(am4) elements).
 *
 * \param [out] output 
 *              Resulting integral output
 * \param [in]  am1,am2,am3,am4
 *              Angular momentum of the four centers
 * \param [in]  A,B,C,D
 *              XYZ coordinates of the four centers (each of length 3)
 * \param [in]  alpha1,alpha2,alpha3,alpha4
 *              Exponents of the gaussian on the four centers
 * \param [in]  cb
 *              Callback that calculates a single cartesian component of a
 *              primitive integral
 */
void mirp_cartloop4_double(double * output,
                           int am1, const double * A, double alpha1,
                           int am2, const double * B, double alpha2,
                           int am3, const double * C, double alpha3,
                           int am4, const double * D, double alpha4,
                           cb_single4_double cb);


/*! \brief Compute all cartesian components of a contracted shell quartet
 *         (four center, double precision)
 *
 * The coefficients (\p coeff1, \p coeff2, \p coeff3, \p coeff4) are expected
 * to be unnormalized.
 *
 * The \p output buffer is expected to be able to hold all the resulting
 * integrals
 *
 * For the coefficient arrays, the index of the primitive/exponent is the fastest index.
 * (The index is `[g*nprim+p]` where `g` is the index of the general contraction and `p`
 * the index of the primitive).
 *
 *
 * \param [out] output
 *              Completed integral output
 * \param [in]  am1,am2,am3,am4
 *              Angular momentum of the four centers
 * \param [in]  A,B,C,D
 *              XYZ coordinates of the four centers (each of length 3)
 * \param [in]  nprim1,nprim2,nprim3,nprim4
 *              Number of primitives on each center
 * \param [in]  ngeneral1,ngeneral2,ngeneral3,ngeneral4
 *              Number of general contractions on each center
 * \param [in]  alpha1,alpha2,alpha3,alpha4
 *              Exponents of the primitive gaussians on the four centers
 *              (lengths \p nprim1, \p nprim2, \p nprim3, \p nprim4)
 * \param [in]  coeff1,coeff2,coeff3,coeff4
 *              Coefficients for each primitive gaussian on the four centers
 *              (\p nprim1 * \p ngeneral1, \p nprim2 * \p ngeneral2,
 *              \p nprim3 * \p ngeneral3, \p nprim4 * \p ngeneral4)
 * \param [in]  cb
 *              Callback that calculates all cartesian components of a single
 *              primitive integral
 */
void mirp_loop4_double(double * output,
                       int am1, const double * A, int nprim1, int ngeneral1, const double * alpha1, const double * coeff1,
                       int am2, const double * B, int nprim2, int ngeneral2, const double * alpha2, const double * coeff2,
                       int am3, const double * C, int nprim3, int ngeneral3, const double * alpha3, const double * coeff3,
                       int am4, const double * D, int nprim4, int ngeneral4, const double * alpha4, const double * coeff4,
                       cb_prim4_double cb);



/*******************************************
 * Interval arithmetic
 *******************************************/

/*! \brief Compute all cartesian components of a single primitive integral
 *         (interval arithmetic)
 *
 * \copydetails mirp_cartloop4_double
 * \param [in] working_prec The working precision (binary digits/bits) to use in the calculation
 */
void mirp_cartloop4_interval(arb_ptr output,
                             int am1, arb_srcptr A, const arb_t alpha1,
                             int am2, arb_srcptr B, const arb_t alpha2,
                             int am3, arb_srcptr C, const arb_t alpha3,
                             int am4, arb_srcptr D, const arb_t alpha4,
                             slong working_prec, cb_single4_interval cb);


/*! \brief Compute all cartesian components of a contracted shell quartet
 *         (interval arithmetic)
 *
 * \copydetails mirp_loop4_double
 * \param [in] working_prec The working precision (binary digits/bits) to use in the calculation
 */
void mirp_loop4_interval(arb_ptr output,
                         int am1, arb_srcptr A, int nprim1, int ngeneral1, arb_srcptr alpha1, arb_srcptr coeff1,
                         int am2, arb_srcptr B, int nprim2, int ngeneral2, arb_srcptr alpha2, arb_srcptr coeff2,
                         int am3, arb_srcptr C, int nprim3, int ngeneral3, arb_srcptr alpha3, arb_srcptr coeff3,
                         int am4, arb_srcptr D, int nprim4, int ngeneral4, arb_srcptr alpha4, arb_srcptr coeff4,
                         slong working_prec, cb_prim4_interval cb);


#ifdef __cplusplus
}
#endif

