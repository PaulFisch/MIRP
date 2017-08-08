/*! \file
 *
 * \brief Some useful wrapping of functionality
 */

#pragma once

#include "mirp/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \brief Computes a single 4-center integral to double precision using interval arithmetic
 *
 * This function takes double precision as input and returns double precision
 * as output. Internally, it uses interval arithmetic to ensure that no
 * precision is lost
 *
 * \param [out] integral
 *              Resulting integral output
 * \param [in]  lmn1,lmn2,lmn3,lmn4
 *              Exponents of x, y, and z that signify angular momentum. Required
 *              to be 3 elements.
 * \param [in]  A,B,C,D
 *              XYZ coordinates of the four centers (each of length 3)
 * \param [in]  alpha1,alpha2,alpha3,alpha4
 *              Exponents of the gaussian on the four centers
 * \param [in] cb
 *             The callback function that computes integrals in interval arithmetic
 */
void mirp_integral4_single_exact(double * integral,
                                 const int * lmn1, const double * A, double alpha1,
                                 const int * lmn2, const double * B, double alpha2,
                                 const int * lmn3, const double * C, double alpha3,
                                 const int * lmn4, const double * D, double alpha4,
                                 cb_single4_interval cb);




/*! \brief Computes a 4-center integral to double precision using interval arithmetic
 *
 * This function takes double precision as input and returns double precision
 * as output. Internally, it uses interval arithmetic to ensure that no
 * precision is lost
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
 * \param [in] cb
 *             The callback function that computes integrals in interval arithmetic
 */
void mirp_integral4_exact(double * integral,
                          int am1, const double * A, int nprim1, int ngeneral1, const double * alpha1, const double * coeff1, 
                          int am2, const double * B, int nprim2, int ngeneral2, const double * alpha2, const double * coeff2, 
                          int am3, const double * C, int nprim3, int ngeneral3, const double * alpha3, const double * coeff3, 
                          int am4, const double * D, int nprim4, int ngeneral4, const double * alpha4, const double * coeff4,
                          cb_integral4_interval cb);

#ifdef __cplusplus
}
#endif
