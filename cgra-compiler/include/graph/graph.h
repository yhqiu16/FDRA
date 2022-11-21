#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "graph/graph_node.h"
#include "graph/graph_edge.h"


class Graph
{
protected:
    int _id; // "This"(INPUT/OUTPUT) ID
    int _bitWidth;
    std::map<int, std::string> _inputNames; // <input-index, input-port-name>
    std::map<int, std::string> _outputNames; // <input-index, input-port-name>
    std::map<int, std::set<std::pair<int, int>>> _inputs; // <input-index, set<node-id, node-port-idx>>
    std::map<int, std::pair<int, int>> _outputs; // <output-index, <node-id, node-port-idx>>
    std::map<int, std::set<int>> _inputEdges; // <input-index, set<edge-id>>
    std::map<int, int> _outputEdges; // <output-index, edge-id>
    // std::map<int, GraphNode*> _nodes;   // <node-id, node>
    // std::map<int, GraphEdge*> _edges;   // <edge-id, edge>
public:
    Graph();
    ~Graph();
    int id(){ return _id; }
    void setId(int id){ _id = id; }
    int bitWidth(){ return _bitWidth; }
    void setBitWidth(int bitWidth){ _bitWidth = bitWidth; }
    int numInputs(){ return _inputs.size(); }
    int numOutputs(){ return _outputs.size(); }
    void setInputName(int index, std::string name);
    std::string inputName(int index);
    void setOutputName(int index, std::string name);
    std::string outputName(int index);

    const std::map<int, std::set<std::pair<int, int>>>& inputs(){ return _inputs; }
    const std::map<int, std::pair<int, int>>& outputs(){ return _outputs; }
    std::set<std::pair<int, int>> input(int index); // return <node-id, node-port-idx>
    std::pair<int, int> output(int index); // return set<node-id, node-port-idx>
    void addInput(int index, std::pair<int, int> node);  // add input
    void addOutput(int index, std::pair<int, int> node); // add output
    void delInput(int index, std::pair<int, int> node);  // delete input
    void delInput(int index);  // delete input
    void delOutput(int index); // delete output

    const std::map<int, std::set<int>>& inputEdges(){ return _inputEdges; }
    const std::map<int, int>& outputEdges(){ return _outputEdges; }
    std::set<int> inputEdge(int index); // return set<edge-id>
    int outputEdge(int index); // return <edge-id>
    void addInputEdge(int index, int edgeId);  // add input edge
    void addOutputEdge(int index, int edgeId); // add output edge
    void delInputEdge(int index, int edgeId);  // delete input edge
    void delInputEdge(int index);  // delete input edge
    void delOutputEdge(int index); // delete output edge

    // const std::map<int, GraphNode*>& nodes(){ return _nodes; }
    // const std::map<int, GraphEdge*>& edges(){ return _edges; }
    // GraphNode* node(int id);
    // GraphEdge* edge(int id);
    // void addNode(GraphNode* node);
    // void addEdge(GraphEdge* edge);
    // void delNode(int id);
    // void delEdge(int id);

    // // ====== operators >>>>>>>>>>
    // // Graph copy
    // Graph& operator=(const Graph& that);

    void printGraph();

};




#endif