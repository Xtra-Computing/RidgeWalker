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

nv= m.get_shape()[0];
ne= m.count_nonzero();
print(nv, ne)

f_offset = open(file_name + "_offset.txt", "wt")
f_offset.write(str(nv) + '\n')
row_pointers = m.indptr

for i in row_pointers:
    f_offset.write(str(i) + '\n')



f_column = open(file_name + "_column.txt", "wt")
f_column.write(str(ne) + '\n')
column_index = m.indices

for i in column_index:
    f_column.write(str(i) + '\n')




#sp.io.mmwrite(file_name + ".mtx", m)