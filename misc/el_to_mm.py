#!/usr/bin/env python3
import sys

import networkx as nx
import scipy as sp
import scipy.io  # for mmread() and mmwrite()
import io  # Use BytesIO as a stand-in for a Python file object

n = len(sys.argv)
if n < 2:
    print("need a file")
    exit(-1)

file_name = sys.argv[1]
G = nx.read_edgelist(file_name)
m = nx.to_scipy_sparse_matrix(G)
sp.io.mmwrite(file_name + ".mtx", m)