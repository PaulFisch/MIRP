#!/usr/bin/env python3

import argparse
import random
import sys
from mpmath import mp
from common import print_integral_single_input
from common.randomgen import generate_basis_function

parser = argparse.ArgumentParser()
parser.add_argument("--filename",    type=str, required=True, help="Output file name")
parser.add_argument("--max-am",      type=int, required=True, help="Maximum AM of the basis functions")
parser.add_argument("--alpha-power", type=int, required=True, help="Maximum power of the exponent (range will be 1e-x to 1e+x)")
parser.add_argument("--xyz-power",   type=int, required=True, help="Maximum power of the coordinates (range will be -1e+x to 1e+x)")
parser.add_argument("--seed",        type=int, required=True, help="Seed to use for the pseudo-random number generator")
parser.add_argument("--ndigits",     type=int, required=True, help="Number of digits for the value of the integral")
parser.add_argument("--ntests",      type=int, required=True, help="Number of tests to generate")
args = parser.parse_args()

random.seed(args.seed, version=2)

with open(args.filename, 'w') as f:
    f.write("# THIS FILE IS GENERATED VIA A SCRIPT. DO NOT EDIT\n")
    f.write("#\n")
    f.write("# Input parameters for integral generated with:\n")
    f.write("#   " + " ".join(sys.argv[:]) + "\n")
    f.write("#\n")
    f.write(str(args.ntests))
    f.write("\n")

    for i in range(args.ntests):
        center1 = generate_basis_function(args.max_am, args.alpha_power, args.xyz_power, args.ndigits)
        center2 = generate_basis_function(args.max_am, args.alpha_power, args.xyz_power, args.ndigits)
        tmp = generate_basis_function(args.max_am, args.alpha_power, args.xyz_power, args.ndigits)

        # Change l,m,n and exponent to zero
        center_nuc = (0, 0, 0, tmp[3], tmp[4], tmp[5], mp.mpf("0.0"))

        entry = [center1, center2, center_nuc]
        print_integral_single_input(f, entry)