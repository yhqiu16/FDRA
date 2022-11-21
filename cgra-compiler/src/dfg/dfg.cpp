
#include "dfg/dfg.h"

DFG::DFG(){}

DFG::~DFG()
{
    for(auto& elem : _nodes){
        delete elem.second;
    }
    for(auto& elem : _edges){
        delete elem.second;
    }
}


DFGNode* DFG::node(int id){
    if(_nodes.count(id)){
        return _nodes[id];
    } else {
        return nullptr;
    }  
}


DFGEdge* DFG::edge(int id){
    if(_edges.count(id)){
        return _edges[id];
    } else {
        return nullptr;
    }  
}


void DFG::addNode(DFGNode* node){
    int id = node->id();
    _nodes[id] = node;
}


void DFG::addEdge(DFGEdge* edge){
    int id = edge->id();
    _edges[id] = edge;
    int srcId = edge->srcId();
    int dstId = edge->dstId();
    int srcPort = edge->srcPortIdx();
    int dstPort = edge->dstPortIdx();
    if(srcId == _id){ // source is input port
        addInput(srcPort, std::make_pair(dstId, dstPort));
        addInputEdge(srcPort, id);
    } else {
        DFGNode* src = node(srcId);
        assert(src);
        src->addOutput(srcPort, std::make_pair(dstId, dstPort));
        src->addOutputEdge(srcPort, id);
    }
    if(dstId == _id){ // destination is output port
        addOutput(dstPort, std::make_pair(srcId, srcPort));
        addOutputEdge(dstPort, id);
    } else{        
        DFGNode* dst = node(dstId);
        assert(dst);
        dst->addInput(dstPort, std::make_pair(srcId, srcPort));
        dst->addInputEdge(dstPort, id);
    }
}


void DFG::delNode(int id){
    DFGNode* dfgNode = node(id);
    for(auto& elem : dfgNode->inputEdges()){
        delEdge(elem.second);
    }
    for(auto& elem : dfgNode->outputEdges()){
        for(auto eid : elem.second){
            delEdge(eid);
        }        
    }
    _nodes.erase(id);
    delete dfgNode;
}


void DFG::delEdge(int id){
    DFGEdge* e = edge(id);
    int srcId = e->srcId();
    int dstId = e->dstId();
    int srcPortIdx = e->srcPortIdx();
    int dstPortIdx = e->dstPortIdx();
    if(srcId == _id){
        delInputEdge(srcPortIdx, id);
        delInput(srcPortIdx, std::make_pair(dstId, dstPortIdx));
    }else{
        DFGNode* srcNode = node(srcId);       
        srcNode->delOutputEdge(srcPortIdx, id);
        srcNode->delOutput(srcPortIdx, std::make_pair(dstId, dstPortIdx));
    }
    if(dstId == _id){
        delOutputEdge(dstPortIdx);
        delOutput(dstPortIdx);
    }else{
        DFGNode* dstNode = node(dstId);
        dstNode->delInputEdge(dstPortIdx);
        dstNode->delInput(dstPortIdx);
    }
    _edges.erase(id);
    delete e;
}


// In nodes: INPUT/LOAD node
std::set<int> DFG::getInNodes(){
    std::set<int> inNodes;
    for(int ioNodeId : _ioNodes){
        DFGNode *ioNode = _nodes[ioNodeId];
        std::string opName = ioNode->operation();
        // if((opName == "INPUT") || (opName == "LOAD" && (ioNode->inputs().size() == 0))){
        if((opName == "INPUT") || (opName == "LOAD")){
            inNodes.insert(ioNodeId);
        }
    }
    return inNodes;
}

// Out nodes: OUTPUT/STORE node
std::set<int> DFG::getOutNodes(){
    std::set<int> outNodes;
    for(int ioNodeId : _ioNodes){
        DFGNode *ioNode = _nodes[ioNodeId];
        std::string opName = ioNode->operation();
        if((opName == "OUTPUT") || (opName == "STORE")){
            outNodes.insert(ioNodeId);
        }
    }
    return outNodes;
}


// sort dfg nodes in topological order
// depth-first search
void DFG::dfs(DFGNode* node, std::map<int, bool>& visited){
    int nodeId = node->id();
    if(visited.count(nodeId) && visited[nodeId]){
        return; // already visited
    }
    visited[nodeId] = true;
    for(auto& in : node->inputs()){
        int inNodeId = in.second.first;
        if(inNodeId == _id){ // node connected to DFG input port
            continue;
        }
        dfs(_nodes[inNodeId], visited); // visit input node
    }
    _topoNodes.push_back(nodeId);
}

// sort dfg nodes in topological order
void DFG::topoSortNodes(){
    _topoNodes.clear();
    std::map<int, bool> visited; // node visited status
    for(auto& outNodeId : getOutNodes()){
        dfs(_nodes[outNodeId], visited); // visit output node
    }
}


// ====== operators >>>>>>>>>>
// DFG copy
DFG& DFG::operator=(const DFG& that){
    if(this == &that) return *this;
    this->_id = that._id;
    this->_bitWidth = that._bitWidth;
    this->_inputNames = that._inputNames;
    this->_outputNames = that._outputNames;
    this->_inputs = that._inputs;
    this->_outputs = that._outputs;
    this->_inputEdges = that._inputEdges;
    this->_outputEdges = that._outputEdges;
    this->_ioNodes = that._ioNodes;
    this->_topoNodes = that._topoNodes;
    for(auto& elem : that._nodes){
        int id = elem.first;
        DFGNode* node;
        if(that._ioNodes.count(id)){
            DFGIONode *ioNode = new DFGIONode();
            *ioNode = *(dynamic_cast<DFGIONode*>(elem.second));
            node = ioNode;
        }else{
            node = new DFGNode();
            *node = *(elem.second);
        }        
        this->_nodes[id] = node;
    }
    // this->_edges = that._edges;
    for(auto& elem : that._edges){
        int id = elem.first;
        DFGEdge* edge = new DFGEdge();
        *edge = *(elem.second);
        this->_edges[id] = edge;
    }
    return *this;
}



void DFG::print(){
    printGraph();
    for(auto& elem : _nodes){
        elem.second->print();
    }
}



