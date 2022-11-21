# import argparse

# parser = argparse.ArgumentParser(description="DFG-filenames")
# parser.add_argument('-f', required=True, type=str)
# args = parser.parse_args()
# print(args.f)

import sys
import os
import pydot

def get_filenames():
    names = []
    for i in range (1, len(sys.argv)):
        names.append(str(sys.argv[i]))
    return names

def get_pure_filename(filename : str):
    tail = filename.rfind("/")
    head = filename.rfind("/", 0, tail)
    if(head == -1):
        head = 0
    return filename[head+1:tail]

dfg_names = get_filenames()
# print(dfg_names)

new_dfg = pydot.Dot("merge-dfg")
new_dfg_name = ""
for filename in dfg_names:
    dfg_name = get_pure_filename(filename)
    new_dfg_name += dfg_name
    # print(dfg_name)
    graphs = pydot.graph_from_dot_file(filename)
    dfg = graphs[0]
    for node in dfg.get_nodes():
        ori_node_name = str(node.get_name())
        # print(ori_node_name)
        if(ori_node_name == "\"\\n\""):
            continue
        node.set_name(dfg_name + "_" + ori_node_name)
        ref_name = node.get("ref_name")
        if(ref_name):
            ref_name = ref_name[1:len(ref_name)-1]
            node.set("ref_name", "\"" + dfg_name + "_" + ref_name + "\"")
        new_dfg.add_node(node)
    for edge in dfg.get_edges():
        src_name = dfg_name + "_" + edge.get_source()
        dst_name = dfg_name + "_" + edge.get_destination()
        attrs = edge.get_attributes()
        new_edge = pydot.Edge(src_name, dst_name)
        for key in attrs.keys():
            new_edge.set(key, attrs[key])
        new_dfg.add_edge(new_edge)


# print(new_dfg.to_string())
os.makedirs(new_dfg_name, exist_ok=True)
new_filename = new_dfg_name + "/affine.dot"
# print(new_filename)
new_dfg.write_raw(new_filename)

    
