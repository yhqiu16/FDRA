#ifndef __GRAPH_NODE_H__
#define __GRAPH_NODE_H__

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <assert.h>


class GraphNode
{
protected:
    int _id;
    std::string _name;
    std::string _type;
    int _bitWidth;
    std::map<int, std::pair<int, int>> _inputs; // <input-index, <node-id, node-port-idx>>
    std::map<int, std::set<std::pair<int, int>>> _outputs; // <output-index, set<node-id, node-port-idx>>
    std::map<int, int> _inputEdges; // <input-index, edge-id>
    std::map<int, std::set<int>> _outputEdges; // <output-index, set<edge-id>>
public:
    GraphNode(){}
    ~GraphNode(){}
    int id(){ return _id; }
    void setId(int id){ _id = id; }
    std::string name();
    void setName(std::string name){ _name = name; }
    std::string type(){ return _type; }
    void setType(std::string type){ _type = type; }
    int bitWidth(){ return _bitWidth; }
    void setBitWidth(int bitWidth){ _bitWidth = bitWidth; }
    virtual int numInputs(){ return _inputs.size(); }
    int numOutputs(){ return _outputs.size(); }

    const std::map<int, std::pair<int, int>>& inputs(){ return _inputs; }
    const std::map<int, std::set<std::pair<int, int>>>& outputs(){ return _outputs; }
    std::pair<int, int> input(int index); // return <node-id, node-port-idx>
    std::set<std::pair<int, int>> output(int index); // return set<node-id, node-port-idx>
    void addInput(int index, std::pair<int, int> node);  // add input
    void addOutput(int index, std::pair<int, int> node); // add output
    void delInput(int index);  // delete input
    void delOutput(int index, std::pair<int, int> node); // delete output
    void delOutput(int index); // delete output

    const std::map<int, int>& inputEdges(){ return _inputEdges; }
    const std::map<int, std::set<int>>& outputEdges(){ return _outputEdges; }
    int inputEdge(int index); // return edge id
    std::set<int> outputEdge(int index); // return set<edge-id>
    void addInputEdge(int index, int edgeId);  // add input edge
    void addOutputEdge(int index, int edgeId); // add output edge
    void delInputEdge(int index); // delete input edge
    void delOutputEdge(int index, int edgeId); // delete output edge
    void delOutputEdge(int index); // delete output edge

    virtual void printGraphNode();
};





#endif