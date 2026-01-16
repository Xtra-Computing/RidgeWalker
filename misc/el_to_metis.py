#!/usr/bin/env python3
import sys

import networkit as nk

import networkx as nx
import scipy as sp
import scipy.io  # for mmread() and mmwrite()
import io  # Use BytesIO as a stand-in for a Python file object

n = len(sys.argv)
if n < 3:
    print("need input file and output file")
    exit(-1)

input_file_name = sys.argv[1]
output_file_name = sys.argv[2]

G = nk.readGraph(input_file_name, nk.Format.SNAP)
nk.writeGraph(G,output_file_name, nk.Format.METIS)