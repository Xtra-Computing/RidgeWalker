#!/usr/bin/env python3

import networkx as nx
import pygraphviz as pgv
from networkx.drawing.nx_agraph import graphviz_layout

from graphviz import Digraph
import matplotlib.pyplot as plt
import numpy as np
pos ={}
def butterfly_network(n):
    G = nx.DiGraph()


    n_node = 2**(n)
    for level in range(n):
        group =  2**(n - 1)

        for i in range(n_node):
            in_id = i
            out_id = i
            G.add_edge(("l" + str(level), out_id), ("l" + str(level + 1), in_id ))
            pos[("l" + str(level), out_id)] = np.array([out_id, level])
            pos[("l" + str(level + 1), in_id )] = np.array([in_id, level + 1])
            mask =  2**(level)

            target_id = i ^ int(mask)
            print(i, target_id, mask)
            G.add_edge(("l" + str(level), i ), ("l" + str(level + 1), target_id ))
    return G

# Generate a butterfly network with 4 stages
G = butterfly_network(3)

#pos = nx.spring_layout(G, scale=2, seed=84)
print(pos)
# Draw the graph using graphviz
#A = nx.nx_agraph.to_agraph(G)
#A.layout(prog='dot')

plt.figure()
nx.draw(
    G,
    pos,
    node_size=1200, alpha=0.5, labels={node:node for node in G.nodes()}
)
plt.savefig('butterfly_network.png')
plt.show()



