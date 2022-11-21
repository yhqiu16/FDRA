
#include "graph/graph_node.h"

// ===================================================
//   GraphNode functions
// ===================================================

std::string GraphNode::name(){ 
    if(!_name.empty()){
        return _name; 
    }else{
        return _type + std::to_string(_id);
    }
}


// return <node-id, node-port-idx>
std::pair<int, int> GraphNode::input(int index){
    if(_inputs.count(index)){
        return _inputs[index];
    }else{
        return {}; // std::make_pair(-1, -1);
    }
}

// return set<node-id, node-port-idx>
std::set<std::pair<int, int>> GraphNode::output(int index){
    if(_outputs.count(index)){
        return _outputs[index];
    }else{
        return {}; // return empty set
    }
}


// add input
void GraphNode::addInput(int index, std::pair<int, int> node){
    _inputs[index] = node;
}

// add output
void GraphNode::addOutput(int index, std::pair<int, int> node){
    _outputs[index].emplace(node);
}

// delete input
void GraphNode::delInput(int index){
    _inputs.erase(index);
}

// delete output
void GraphNode::delOutput(int index, std::pair<int, int> node){
    _outputs[index].erase(node);
}

// delete output
void GraphNode::delOutput(int index){
    _outputs.erase(index);
}

// return edge id
int GraphNode::inputEdge(int index){
    if(_inputEdges.count(index)){
        return _inputEdges[index];
    }else{
        return -1; 
    }
}

// return set<edge-id>
std::set<int> GraphNode::outputEdge(int index){
    if(_outputEdges.count(index)){
        return _outputEdges[index];
    }else{
        return {}; // return empty set
    }
}

// add input edge
void GraphNode::addInputEdge(int index, int edgeId){
    _inputEdges[index] = edgeId;
}

// add output edge
void GraphNode::addOutputEdge(int index, int edgeId){
    _outputEdges[index].emplace(edgeId);
}

// delete input edge
void GraphNode::delInputEdge(int index){
    _inputEdges.erase(index);
}

// delete output edge
void GraphNode::delOutputEdge(int index, int edgeId){
    assert(_outputEdges.count(index));
    _outputEdges[index].erase(edgeId);
}

// delete output edge
void GraphNode::delOutputEdge(int index){
    _outputEdges.erase(index);
}


void GraphNode::printGraphNode(){
    std::cout << "=====================================\n";
    std::cout << "id: " << _id << std::endl;
    std::cout << "type: " << _type << std::endl;
    std::cout << "name: " << _name << std::endl;
    std::cout << "bitWidth: " << _bitWidth << std::endl;
    std::cout << "numInputs: " << numInputs() << std::endl;
    std::cout << "numOutputs: " << numOutputs() << std::endl;
    std::cout << "inputs: " << std::endl;
    for(auto& elem : _inputs){
        std::cout << elem.first << ": (" << elem.second.first << ", " << elem.second.second << ")\n";
    }
    std::cout << "outputs: " << std::endl;
    for(auto& elem : _outputs){
        std::cout << elem.first << ": ";
        auto& s = elem.second;
        for(auto it = s.begin(); it != s.end(); it++){
            std::cout << "(" << it->first << ", " << it->second << ") ";
        }
        std::cout << std::endl;
    }
}