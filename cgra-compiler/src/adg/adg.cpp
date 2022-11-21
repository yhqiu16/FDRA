#include "adg/adg.h"

ADG::ADG(){}

ADG::~ADG()
{
    for(auto& elem : _nodes){
        auto node = elem.second;
        auto sub_adg = node->subADG();
        if(sub_adg){
            delete sub_adg;
        }
        delete node;
    }
    for(auto& elem : _edges){
        delete elem.second;
    }
}


ADGNode* ADG::node(int id){
    if(_nodes.count(id)){
        return _nodes[id];
    } else {
        return nullptr;
    }  
}


ADGEdge* ADG::edge(int id){
    if(_edges.count(id)){
        return _edges[id];
    } else {
        return nullptr;
    }  
}


void ADG::addNode(int id, ADGNode* node){
    _nodes[id] = node;
}


void ADG::addEdge(int id, ADGEdge* edge){
    _edges[id] = edge;
    int srcId = edge->srcId();
    int dstId = edge->dstId();
    int srcPort = edge->srcPortIdx();
    int dstPort = edge->dstPortIdx();
    if(srcId == _id){ // source is input port
        addInput(srcPort, std::make_pair(dstId, dstPort));
    } else {
        ADGNode* src = node(srcId);
        assert(src);
        src->addOutput(srcPort, std::make_pair(dstId, dstPort));
    }
    if(dstId == _id){ // destination is output port
        addOutput(dstPort, std::make_pair(srcId, srcPort));
    } else{        
        ADGNode* dst = node(dstId);
        assert(dst);
        dst->addInput(dstPort, std::make_pair(srcId, srcPort));
    }
}


void ADG::delNode(int id){
    ADGNode *adgNode = node(id);
    _nodes.erase(id);
    delete adgNode;
}


void ADG::delEdge(int id){
    ADGEdge *adgEdge = edge(id);
    _edges.erase(id);
    delete adgEdge;
}


ADG& ADG::operator=(const ADG& that){
    if(this == &that) return *this;
    this->_id = that._id;
    this->_bitWidth = that._bitWidth;
    this->_numGpeNodes = that._numGpeNodes;
    this->_numIobNodes = that._numIobNodes;
    this->_cfgDataWidth = that._cfgDataWidth;
    this->_cfgAddrWidth = that._cfgAddrWidth;
    this->_cfgBlkOffset = that._cfgBlkOffset;
    // this->_loadLatency = that._loadLatency;
    // this->_storeLatency = that._storeLatency;
    this->_cfgSpadSize = that._cfgSpadSize;
    this->_iobAgNestLevels = that._iobAgNestLevels;
    this->_iobSpadBankSize = that._iobSpadBankSize;
    this->_iobToSpadBanks = that._iobToSpadBanks;
    this->_cfgBits = that._cfgBits;
    this->_inputs = that._inputs;
    this->_outputs = that._outputs;
    // this->_input_used = that._input_used;
    // this->_output_used = that._output_used;
    for(auto& elem : that._nodes){
        ADGNode* node = new ADGNode();
        *node = *(elem.second);
        this->_nodes[elem.first] = node;
    }
    for(auto& elem : that._edges){
        ADGEdge* edge = new ADGEdge();
        *edge = *(elem.second);
        this->_edges[elem.first] = edge;
    }
    return *this;
}



void ADG::print(){
    printGraph();
    std::cout << "numGpeNodes: " << _numGpeNodes << std::endl;
    std::cout << "numIobNodes: " << _numIobNodes << std::endl;
    std::cout << "cfgDataWidth: " << _cfgDataWidth << std::endl;
    std::cout << "cfgAddrWidth: " << _cfgAddrWidth << std::endl;
    std::cout << "cfgBlkOffset: " << _cfgBlkOffset << std::endl;
    std::cout << "cfgSpadSize: " << _cfgSpadSize << std::endl;
    std::cout << "iobAgNestLevels: " << _iobAgNestLevels << std::endl;
    std::cout << "iobSpadBankSize: " << _iobSpadBankSize << std::endl;
    std::cout << "iobToSpadBanks: " << std::endl;   
    for(auto& elem : _iobToSpadBanks){
        std::cout << "IOB(" << elem.first << "): ";
        for(auto bank : elem.second)
            std::cout << bank << " ";
        std::cout << std::endl;
    }
    std::cout << "cfgBits: ";
    for(auto& elem : _cfgBits){
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    for(auto& elem : _nodes){
        elem.second->print();
    }
}