#ifndef __DFG_H__
#define __DFG_H__

#include "dfg/dfg_node.h"
#include "dfg/dfg_edge.h"
#include "graph/graph.h"


class DFG : public Graph
{
private:
    std::map<int, DFGNode*> _nodes;   // <node-id, node>
    std::map<int, DFGEdge*> _edges;   // <edge-id, edge>
    std::set<int> _ioNodes; // IO Node IDs, including INPUT, OUTPUT, LOAD, STORE nodes
    DFG(const DFG&) = delete; // disable the default copy construct function
protected:
    // DFG nodes in topological order, DFG should be DAG
    std::vector<int> _topoNodes;

    // depth-first search, sort dfg nodes in topological order
    void dfs(DFGNode* node, std::map<int, bool>& visited);

public:
    DFG();
    ~DFG();

    const std::map<int, DFGNode*>& nodes(){ return _nodes; }
    const std::map<int, DFGEdge*>& edges(){ return _edges; }
    DFGNode* node(int id);
    DFGEdge* edge(int id);
    void addNode(DFGNode* node);
    void addEdge(DFGEdge* edge);
    void delNode(int id);
    void delEdge(int id);

    const std::set<int>& ioNodes(){ return _ioNodes; }
    void addIONode(int id){ _ioNodes.insert(id); }
    void delIONode(int id){ _ioNodes.erase(id); }
    bool isIONode(int id){ return _ioNodes.count(id); }
    // In nodes: INPUT/LOAD node
    std::set<int> getInNodes();
    // Out nodes: OUTPUT/STORE node
    std::set<int> getOutNodes();

    // DFG nodes in topological order
    const std::vector<int>& topoNodes(){ return _topoNodes; }
    // sort dfg nodes in topological order
    void topoSortNodes();

    // ====== operators >>>>>>>>>>
    // DFG copy
    DFG& operator=(const DFG& that);

    void print();

};




#endif