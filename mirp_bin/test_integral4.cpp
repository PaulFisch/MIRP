/*! \file
 *
 * \brief Functions related to testing four-center integrals
 */

#include "mirp_bin/testfile_io.hpp"
#include "mirp_bin/test_integral4.hpp"
#include "mirp_bin/test_common.hpp"

#include <mirp/math.h>
#include <mirp/shell.h>

#include <cmath>
#include <iostream>
#include <fstream>

namespace mirp {

/*! \brief The number of integrals computed in an entry */
static size_t nintegrals(const integral_data_entry & ent)
{
    size_t nint = 1;
    for(const auto & g : ent.g)
        nint *= MIRP_NCART(g.am)*g.ngeneral;
    return nint;
}


void integral4_create_test(const std::string & input_filepath,
                           const std::string & output_filepath,
                           long ndigits, const std::string & header,
                           cb_integral4_target_str cb)
{
    integral_data data = testfile_read_integral(input_filepath, 4, true);

    data.ndigits = ndigits;
    data.header += header;

    const slong target_prec = (ndigits+8) / MIRP_LOG_10_2;

    /* Needed to unpack everything into char pointers */
    const char * ABCD[4][3];
    std::vector<const char *> alpha[4], coeff[4];

    for(auto & ent : data.entries)
    {
        const size_t nint = nintegrals(ent);
        arb_ptr integrals = _arb_vec_init(nint);

        for(int n = 0; n < 4; n++)
        {
            const auto & g = ent.g[n];

            alpha[n].clear();
            coeff[n].clear();

            /* Unpack xyz, exponents, and coefficients */
            for(int i = 0; i < 3; i++)
                ABCD[n][i] = g.xyz[i].c_str();
            for(int i = 0; i < g.nprim; i++)
                alpha[n].push_back(g.alpha[i].c_str());
            for(int i = 0; i < g.nprim*g.ngeneral; i++)
                coeff[n].push_back(g.coeff[i].c_str());
        }

        cb(integrals,
           ent.g[0].am, ABCD[0], ent.g[0].nprim, ent.g[0].ngeneral, alpha[0].data(), coeff[0].data(),
           ent.g[1].am, ABCD[1], ent.g[1].nprim, ent.g[1].ngeneral, alpha[1].data(), coeff[1].data(),
           ent.g[2].am, ABCD[2], ent.g[2].nprim, ent.g[2].ngeneral, alpha[2].data(), coeff[2].data(),
           ent.g[3].am, ABCD[3], ent.g[3].nprim, ent.g[3].ngeneral, alpha[3].data(), coeff[3].data(),
           target_prec);


        for(size_t i = 0; i < nint; i++)
        {
            char * s = arb_get_str(integrals+i, ndigits, ARB_STR_NO_RADIUS);
            ent.integrals.push_back(s);
            free(s);
        }

        _arb_vec_clear(integrals, nint);
    }

    testfile_write_integral(output_filepath, data);
}


long integral4_run_test(const std::string & filepath,
                        long target_prec,
                        cb_integral4_target_str cb)
{
    long nfailed = 0;

    integral_data data = testfile_read_integral(filepath, 4, false);

    /* Number of binary digits contained in the reference value strings
       (and the number of binary digits of accuracy, taking into account
       that the number printed is +/- 1 decimal ulp) */
    const long integral_bits = data.ndigits / MIRP_LOG_10_2;
    const long round_bits = (data.ndigits - 1) / MIRP_LOG_10_2;

    arb_t integral_ref;
    arb_init(integral_ref);

    /* Needed to unpack everything into char pointers */
    const char * ABCD[4][3];
    std::vector<const char *> alpha[4], coeff[4];

    for(auto & ent : data.entries)
    {
        const size_t nint = nintegrals(ent);
        arb_ptr integrals = _arb_vec_init(nint);

        for(int n = 0; n < 4; n++)
        {
            alpha[n].clear();
            coeff[n].clear();

            const auto & g = ent.g[n];

            /* Unpack xyz, exponents, and coefficients */
            for(int i = 0; i < 3; i++)
                ABCD[n][i] = g.xyz[i].c_str();
            for(int i = 0; i < g.nprim; i++)
                alpha[n].push_back(g.alpha[i].c_str());
            for(int i = 0; i < g.nprim*g.ngeneral; i++)
                coeff[n].push_back(g.coeff[i].c_str());
        }

        cb(integrals,
           ent.g[0].am, ABCD[0], ent.g[0].nprim, ent.g[0].ngeneral, alpha[0].data(), coeff[0].data(),
           ent.g[1].am, ABCD[1], ent.g[1].nprim, ent.g[1].ngeneral, alpha[1].data(), coeff[1].data(),
           ent.g[2].am, ABCD[2], ent.g[2].nprim, ent.g[2].ngeneral, alpha[2].data(), coeff[2].data(),
           ent.g[3].am, ABCD[3], ent.g[3].nprim, ent.g[3].ngeneral, alpha[3].data(), coeff[3].data(),
           target_prec+16);


        for(size_t i = 0; i < nint; i++)
        {
            /* Convert the reference to arb_t. The reference is printed to +/- 1 ulp (decimal). */
            if(ent.integrals[i] == "0")
                arb_zero(integral_ref);
            else
            {
                arb_set_str(integral_ref, ent.integrals[i].c_str(), integral_bits + 16);
                arf_mag_add_ulp(arb_radref(integral_ref),
                                arb_radref(integral_ref),
                                arb_midref(integral_ref),
                                round_bits);
                arb_set_round(integral_ref, integral_ref, target_prec);
            }

            /* Rounding the reference value to the target precision results in
             * an interval. Does that interval contain our (more precise) result? */
            if(!arb_equal(integral_ref, integrals+i) && !arb_contains(integral_ref, integrals+i))
            {
                std::cout << "Entry failed test:\n";
                char * s1 = arb_get_str(integrals+i, 2*data.ndigits, 0);
                char * s2 = arb_get_str(integral_ref, 2*data.ndigits, 0);
                std::cout << "   Calculated: " << s1 << "\n";
                std::cout << "    Reference: " << s2 << "\n\n";
                free(s1);
                free(s2);
                nfailed++;
            }
        }

        _arb_vec_clear(integrals, nint);

    }

    arb_clear(integral_ref);

    print_results(nfailed, data.entries.size());

    return nfailed;
}


long integral4_run_test_d(const std::string & filepath,
                          cb_integral4_d cb)
{
    long nfailed = 0;

    integral_data data = testfile_read_integral(filepath, 4, false);

    /* Needed to unpack XYZ */
    double ABCD[4][3];
    std::vector<double> alpha[4], coeff[4];
    std::vector<double> integrals;

    for(const auto & ent : data.entries)
    {
        const size_t nint = nintegrals(ent);
        integrals.resize(nint);

        for(int n = 0; n < 4; n++)
        {
            alpha[n].clear();
            coeff[n].clear();

            const auto & g = ent.g[n];

            /* Unpack xyz, exponents, and coefficients */
            for(int i = 0; i < 3; i++)
                ABCD[n][i] = std::strtod(g.xyz[i].c_str(), nullptr);
            for(int i = 0; i < g.nprim; i++)
                alpha[n].push_back(std::strtod(g.alpha[i].c_str(), nullptr));
            for(int i = 0; i < g.nprim*g.ngeneral; i++)
                coeff[n].push_back(std::strtod(g.coeff[i].c_str(), nullptr));
        }

        cb(integrals.data(),
           ent.g[0].am, ABCD[0], ent.g[0].nprim, ent.g[0].ngeneral, alpha[0].data(), coeff[0].data(),
           ent.g[1].am, ABCD[1], ent.g[1].nprim, ent.g[1].ngeneral, alpha[1].data(), coeff[1].data(),
           ent.g[2].am, ABCD[2], ent.g[2].nprim, ent.g[2].ngeneral, alpha[2].data(), coeff[2].data(),
           ent.g[3].am, ABCD[3], ent.g[3].nprim, ent.g[3].ngeneral, alpha[3].data(), coeff[3].data());

        bool failed_shell = false;
        for(size_t i = 0; i < nint; i++)
        {
            double integral_ref = std::strtod(ent.integrals[i].c_str(), nullptr);

            if(!almost_equal(integrals[i], integral_ref, 1e-13))
            {
                double reldiff = std::fabs(integral_ref - integrals[i]);
                reldiff /= std::fmax(std::fabs(integral_ref), std::fabs(integrals[i]));

                std::cout << "Entry failed test:\n";
                for(int i = 0; i < 4; i++)
                {
                    std::cout << ent.g[i].Z << " "
                              << ent.g[i].am << " "
                              << ent.g[i].xyz[0] << " "
                              << ent.g[i].xyz[1] << " "
                              << ent.g[i].xyz[2] << "\n";
                }

                auto old_cout_prec = std::cout.precision(17);
                std::cout << "   Calculated: " << integrals[i] << "\n";
                std::cout << "    Reference: " << integral_ref << "\n";
                std::cout << "Relative Diff: " << reldiff << "\n\n";
                std::cout.precision(old_cout_prec);
                failed_shell = true;
            }
        }

        if(failed_shell)
            nfailed++;
    }

    print_results(nfailed, data.entries.size());

    return nfailed;
}


long integral4_run_test_exact(const std::string & filepath,
                                     cb_integral4_exact cb,
                                     cb_integral4_target cb_mp)
{
    long nfailed = 0;

    integral_data data = testfile_read_integral(filepath, 4, false);

    /* Needed to unpack XYZ */
    double ABCD[4][3];

    arb_ptr ABCD_mp[4];
    ABCD_mp[0] = _arb_vec_init(3);
    ABCD_mp[1] = _arb_vec_init(3);
    ABCD_mp[2] = _arb_vec_init(3);
    ABCD_mp[3] = _arb_vec_init(3);

    std::vector<double> alpha[4], coeff[4], integrals;

    for(const auto & ent : data.entries)
    {
        const size_t nint = nintegrals(ent);
        integrals.resize(nint);

        arb_ptr integrals_mp = _arb_vec_init(nint);

        arb_ptr alpha_mp[4], coeff_mp[4];

        for(int n = 0; n < 4; n++)
        {
            alpha[n].clear();
            coeff[n].clear();

            const auto & g = ent.g[n];

            alpha_mp[n] = _arb_vec_init(g.nprim);
            coeff_mp[n] = _arb_vec_init(g.nprim*g.ngeneral);

            for(int i = 0; i < 3; i++)
            {
                ABCD[n][i] = std::strtod(ent.g[n].xyz[i].c_str(), nullptr);
                arb_set_d(ABCD_mp[n] + i, ABCD[n][i]);
            }

            for(int i = 0; i < g.nprim; i++)
            {
                alpha[n].push_back(std::strtod(g.alpha[i].c_str(), nullptr));
                arb_set_d(alpha_mp[n]+i, alpha[n][i]);
            }

            for(int i = 0; i < g.nprim*g.ngeneral; i++)
            {
                coeff[n].push_back(std::strtod(g.coeff[i].c_str(), nullptr));
                arb_set_d(coeff_mp[n]+i, coeff[n][i]);
            }
        }

        cb(integrals.data(),
           ent.g[0].am, ABCD[0], ent.g[0].nprim, ent.g[0].ngeneral, alpha[0].data(), coeff[0].data(),
           ent.g[1].am, ABCD[1], ent.g[1].nprim, ent.g[1].ngeneral, alpha[1].data(), coeff[1].data(),
           ent.g[2].am, ABCD[2], ent.g[2].nprim, ent.g[2].ngeneral, alpha[2].data(), coeff[2].data(),
           ent.g[3].am, ABCD[3], ent.g[3].nprim, ent.g[3].ngeneral, alpha[3].data(), coeff[3].data());

        /* Compute using very high precision */
        cb_mp(integrals_mp,
              ent.g[0].am, ABCD_mp[0], ent.g[0].nprim, ent.g[0].ngeneral, alpha_mp[0], coeff_mp[0],
              ent.g[1].am, ABCD_mp[1], ent.g[1].nprim, ent.g[1].ngeneral, alpha_mp[1], coeff_mp[1],
              ent.g[2].am, ABCD_mp[2], ent.g[2].nprim, ent.g[2].ngeneral, alpha_mp[2], coeff_mp[2],
              ent.g[3].am, ABCD_mp[3], ent.g[3].nprim, ent.g[3].ngeneral, alpha_mp[3], coeff_mp[3],
              512);

        for(int n = 0; n < 4; n++)
        {
            const auto & g = ent.g[n];
            _arb_vec_clear(alpha_mp[n], g.nprim);
            _arb_vec_clear(coeff_mp[n], g.nprim*g.ngeneral);
        }

        slong acc_bits = mirp_min_accuracy_bits(integrals_mp, nint);

        if(acc_bits < 64)
            throw std::logic_error("Not enough bits in testing exact integral function. Contact the developer");

        bool failed_shell = false;
        for(size_t i = 0; i < nint; i++)
        {
            double vref_dbl = std::strtod(ent.integrals[i].c_str(), nullptr);
            double vref2_dbl = arf_get_d(arb_midref(integrals_mp+i), ARF_RND_NEAR);

            if(integrals[i] != vref_dbl && integrals[i] != vref2_dbl)
            {
                std::cout << "Entry failed test:\n";
                for(int i = 0; i < 4; i++)
                {
                    std::cout << ent.g[i].Z << " "
                              << ent.g[i].am << " "
                              << ent.g[i].xyz[0] << " "
                              << ent.g[i].xyz[1] << " "
                              << ent.g[i].xyz[2] << "\n";
                }

                auto old_cout_prec = std::cout.precision(17);
                std::cout << "     Calculated: " << integrals[i] << "\n";
                std::cout << "      Reference: " << vref2_dbl << "\n";
                std::cout << " File Reference: " << vref_dbl << "\n\n";
                std::cout.precision(old_cout_prec);
                failed_shell = true;
            }
        }

        _arb_vec_clear(integrals_mp, nint);

        if(failed_shell)
            nfailed++;
    }

    print_results(nfailed, data.entries.size());

    _arb_vec_clear(ABCD_mp[0], 3);
    _arb_vec_clear(ABCD_mp[1], 3);
    _arb_vec_clear(ABCD_mp[2], 3);
    _arb_vec_clear(ABCD_mp[3], 3);

    return nfailed;
}


} // close namespace mirp
