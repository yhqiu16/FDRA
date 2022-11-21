
#include "graph/graph.h"

Graph::Graph(){}

Graph::~Graph(){
    // for(auto& elem : _nodes){
    //     delete elem.second;
    // }
    // for(auto& elem : _edges){
    //     delete elem.second;
    // }
}


void Graph::setInputName(int index, std::string name){
    _inputNames[index] = name;
}

std::string Graph::inputName(int index){
    if(_inputNames.count(index)){
        return _inputNames[index];
    }else{
        return "";
    }
}

void Graph::setOutputName(int index, std::string name){
    _outputNames[index] = name;
}

std::string Graph::outputName(int index){
    if(_outputNames.count(index)){
        return _outputNames[index];
    }else{
        return "";
    }
}

// return set<node-id, node-port-idx>
std::set<std::pair<int, int>> Graph::input(int index){
    if(_inputs.count(index)){
        return _inputs[index];
    }else{
        return {};
    }
}

// return set<node-id, node-port-idx>
std::pair<int, int> Graph::output(int index){
    if(_outputs.count(index)){
        return _outputs[index];
    }else{
        return {}; // std::make_pair(-1, -1); // return empty set
    }
}

// add input
void Graph::addInput(int index, std::pair<int, int> node){
    _inputs[index].emplace(node);
}

// add output
void Graph::addOutput(int index, std::pair<int, int> node){
    _outputs[index] = node;
}


void Graph::delInput(int index, std::pair<int, int> node){
    _inputs[index].erase(node);
}

void Graph::delInput(int index){
    _inputs.erase(index);
} 

void Graph::delOutput(int index){
    _outputs.erase(index);
}

// return edge id
std::set<int> Graph::inputEdge(int index){
    if(_inputEdges.count(index)){
        return _inputEdges[index];
    }else{
        return {}; // return empty set
    }
}

// return set<edge-id>
int Graph::outputEdge(int index){
    if(_outputEdges.count(index)){
        return _outputEdges[index];
    }else{
        return -1; 
    }
}

// add input edge
void Graph::addInputEdge(int index, int edgeId){
    _inputEdges[index].emplace(edgeId);
}

// add output edge
void Graph::addOutputEdge(int index, int edgeId){
    _outputEdges[index] = edgeId;
}

// delete input edge
void Graph::delInputEdge(int index, int edgeId){
    _inputEdges[index].erase(edgeId);
}

// delete input edge
void Graph::delInputEdge(int index){
    _inputEdges.erase(index);
}

// delete output edge
void Graph::delOutputEdge(int index){
    _outputEdges.erase(index);
}


// GraphNode* Graph::node(int id){
//     if(_nodes.count(id)){
//         return _nodes[id];
//     } else {
//         return nullptr;
//     }  
// }


// GraphEdge* Graph::edge(int id){
//     if(_edges.count(id)){
//         return _edges[id];
//     } else {
//         return nullptr;
//     }  
// }


// void Graph::addNode(GraphNode* node){
//     int id = node->id();
//     _nodes[id] = node;
// }


// void Graph::addEdge(GraphEdge* edge){
//     int id = edge->id();
//     _edges[id] = edge;
//     int srcId = edge->srcId();
//     int dstId = edge->dstId();
//     int srcPort = edge->srcPortIdx();
//     int dstPort = edge->dstPortIdx();
//     if(srcId == _id){ // source is input port
//         addInput(srcPort, std::make_pair(dstId, dstPort));
//         addInputEdge(srcPort, id);
//     } else {
//         GraphNode* src = node(srcId);
//         assert(src);
//         src->addOutput(srcPort, std::make_pair(dstId, dstPort));
//         src->addOutputEdge(srcPort, id);
//     }
//     if(dstId == _id){ // destination is output port
//         addOutput(dstPort, std::make_pair(srcId, srcPort));
//         addOutputEdge(dstPort, id);
//     } else{        
//         GraphNode* dst = node(dstId);
//         assert(dst);
//         dst->addInput(dstPort, std::make_pair(srcId, srcPort));
//         dst->addInputEdge(dstPort, id);
//     }
// }


// void Graph::delNode(int id){
//     GraphNode* dfgNode = node(id);
//     for(auto& elem : dfgNode->inputEdges()){
//         delEdge(elem.second);
//     }
//     for(auto& elem : dfgNode->outputEdges()){
//         for(auto eid : elem.second){
//             delEdge(eid);
//         }        
//     }
//     _nodes.erase(id);
//     delete dfgNode;
// }


// void Graph::delEdge(int id){
//     GraphEdge* e = edge(id);
//     int srcId = e->srcId();
//     int dstId = e->dstId();
//     int srcPortIdx = e->srcPortIdx();
//     int dstPortIdx = e->dstPortIdx();
//     if(srcId == _id){
//         delInputEdge(srcPortIdx, id);
//         delInput(srcPortIdx, std::make_pair(dstId, dstPortIdx));
//     }else{
//         GraphNode* srcNode = node(srcId);       
//         srcNode->delOutputEdge(srcPortIdx, id);
//         srcNode->delOutput(srcPortIdx, std::make_pair(dstId, dstPortIdx));
//     }
//     if(dstId == _id){
//         delOutputEdge(dstPortIdx);
//         delOutput(dstPortIdx);
//     }else{
//         GraphNode* dstNode = node(dstId);
//         dstNode->delInputEdge(dstPortIdx);
//         dstNode->delInput(dstPortIdx);
//     }
//     _edges.erase(id);
//     delete e;
// }


// ====== operators >>>>>>>>>>
// Graph copy
// Graph& Graph::operator=(const Graph& that){
//     if(this == &that) return *this;
//     this->_id = that._id;
//     this->_bitWidth = that._bitWidth;
//     this->_inputNames = that._inputNames;
//     this->_outputNames = that._outputNames;
//     this->_inputs = that._inputs;
//     this->_outputs = that._outputs;
//     this->_inputEdges = that._inputEdges;
//     this->_outputEdges = that._outputEdges;
//     // this->_maxLatMis = that._maxLatMis;
//     // this->_nodesTiming = that._nodesTiming;
//     // this->_nodes = that._nodes;
//     for(auto& elem : that._nodes){
//         int id = elem.first;
//         GraphNode* node = new GraphNode();
//         *node = *(elem.second);
//         this->_nodes[id] = node;
//     }
//     // this->_edges = that._edges;
//     for(auto& elem : that._edges){
//         int id = elem.first;
//         GraphEdge* edge = new GraphEdge();
//         *edge = *(elem.second);
//         this->_edges[id] = edge;
//     }
//     return *this;
// }



void Graph::printGraph(){
    std::cout << "======================================================================\n";
    std::cout << "Graph(id): " << _id << std::endl;
    std::cout << "bitWidth: " << _bitWidth << std::endl;
    std::cout << "numInputs: " << numInputs() << std::endl;
    std::cout << "numOutputs: " << numOutputs() << std::endl;
    std::cout << "inputNames: " << std::endl;
    for(auto& elem : _inputNames){
        std::cout << "(" << elem.first << ", " << elem.second << ") ";
    }
    std::cout << "\noutputNames: " << std::endl;
    for(auto& elem : _outputNames){
        std::cout << "(" << elem.first << ": " << elem.second << ") ";
    }
    std::cout << "\ninputs: " << std::endl;   
    for(auto& elem : _inputs){
        std::cout << elem.first << ": ";
        auto& s = elem.second;
        for(auto it = s.begin(); it != s.end(); it++)
            std::cout << "(" << it->first << ", " << it->second << ") ";
        std::cout << std::endl;
    }
    std::cout << "outputs: " << std::endl;
    for(auto& elem : _outputs){
        std::cout << elem.first << ": (" << elem.second.first << ", " << elem.second.second << ")\n";
    }
    // for(auto& elem : _nodes){
    //     elem.second->print();
    // }
}



