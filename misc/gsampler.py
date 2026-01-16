# docker run --gpus all -it --rm nvcr.io/nvidia/dgl:24.11-py3
# docker run --gpus all --cpus=32  --runtime=nvidia -v /ssd2/hongshi/share:/home/hongshi -v /shared/ssd/hongshi/graph_at_scale:/graph -it --rm dgl:v2
# docker run --gpus all   --runtime=nvidia -v ~/share:/home/hongshi -v /shared/ssd/hongshi/graph_at_scale:/graph -it --rm dgl:v1
# python -u  eva.py  2>&1 | tee  "$(date '+logfile_%Y-%m-%d_%H-%M-%S').log"
import dgl
import gs
import torch
from gs.utils import load_graph
from gs.utils import load_ogb
from typing import List

import time

import logging
import sys

root = logging.getLogger()
root.setLevel(logging.DEBUG)

handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
root.addHandler(handler)

fh = logging.FileHandler('eva.log')
fh.setLevel(logging.DEBUG)
root.addHandler(fh)

                

def create_graph_from_file(file_path, directed=True):
    """
    Create a DGL graph from an edge list file, ignoring lines that start with #.

    Args:
        file_path (str): Path to the edge list file. 
                         Each line in the file represents an edge in the format "source destination".
                         Lines starting with # are treated as comments and ignored.
        directed (bool): Whether the graph should be directed. Default is True.

    Returns:
        dgl.DGLGraph: The created graph.
    """
    src_nodes, dst_nodes = [], []

    # Read the edge list from the file
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith("#") or not line:
                # Ignore comment lines and empty lines
                continue
            # Parse the source and destination nodes
            src, dst = map(int, line.split())
            src_nodes.append(src)
            dst_nodes.append(dst)

    # Convert the lists to tensors
    src_tensor = torch.tensor(src_nodes)
    dst_tensor = torch.tensor(dst_nodes)

    # Create the graph
    graph = dgl.graph((src_tensor, dst_tensor))
    
    # If the graph is undirected, make it bidirectional
    if not directed:
        graph = dgl.to_bidirected(graph)
    
    # Print the size of nodes and edges
    num_nodes = graph.num_nodes()
    num_edges = graph.num_edges()
    print(f"Graph created with {num_nodes} nodes and {num_edges} edges.")
    
    return graph

def node2vec_sampler(A: gs.matrix_api.Matrix, seeds: torch.Tensor,
                     walk_length: int, p: float, q: float):
    paths = A.node2vec(seeds, walk_length, p, q)
    return paths



def randomwalk_sampler(A: gs.matrix_api.Matrix, seeds: torch.Tensor, walk_length: int):
    paths = A.random_walk(seeds, walk_length)
    return paths

def run_random_walk(file_path, is_directed=True, loop=1):
    torch.manual_seed(42)
    #dataset = load_graph.load_reddit()
    #dataset = load_ogb("ogbn-products", ".")
    #dgl_graph = dataset[0]
    print("dataset: %s"%(file_path + "/b_edge_list.bin"))
    dgl_graph = create_graph_from_file(file_path + "/b_edge_list.bin", is_directed)
    csc_indptr, csc_indices, _ = dgl_graph.adj_tensors("csc")

    node2vec = True

    m = gs.matrix_api.Matrix()
    m.load_graph("CSC", [csc_indptr.cuda(), csc_indices.cuda()])
    n = int(dgl_graph.num_nodes())
    for loop_counter in range(loop):
        seeds = torch.randint(0, n, (n,)).cuda()
        start_time = time.time()

        if node2vec:
            compile_func = gs.jit.compile(func=node2vec_sampler,
                                  args=(m, seeds, 80, 2.0, 0.5))
        else:
            compile_func = gs.jit.compile(func=randomwalk_sampler, args=(m, seeds, 80))
          
        path_start_time = time.time()
        if node2vec:
            paths = compile_func(m, seeds, 80, 2.0, 0.5)
        else:
            paths = compile_func(m, seeds, 80)
        
        diff = time.time() - start_time
        path_diff = time.time() - path_start_time
        count_neg_ones = torch.sum(paths == -1).item()
        print("--- %s seconds ---" % (diff))
        print("--- %s seconds ---" % (path_diff))
        print(paths)
        print(len(paths) )
        print(len(paths[0]))
        print("--- number of invalid %d ---" %(count_neg_ones))
        print("--- %s M step/s ---" % ((n * 80)/(diff)/1000/1000))
        print("--- node2vec %d real thr of %s at loop %d is : %s M step/s ---" % (node2vec ,file_path, loop_counter, (n * 80 - count_neg_ones)/(diff)/1000/1000))

dataset=[
    ("WG",True), 
    ("CP",True), 
    ("AS",True), 
    ("LJ",False), 
    ("AB",True), 
    ("UK",True)
    ]
if __name__ == "__main__":
    for d in dataset:
        for i  in range(5):
            run_random_walk("/graph/" + d[0], d[1])
    

