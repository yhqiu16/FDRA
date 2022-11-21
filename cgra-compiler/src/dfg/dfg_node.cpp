
#include "dfg/dfg_node.h"

// ===================================================
//   DFGNode functions
// ===================================================

// set operation, latency, commutative according to operation name
void DFGNode::setOperation(std::string operation){ 
    if(!Operations::opCapable(operation)){
        std::cout << operation << " is not supported!" << std::endl;
        exit(1);
    }
    // TODO: add bitwidth check
    _operation = operation; 
    setOpLatency(Operations::latency(operation));
    setCommutative(Operations::isCommutative(operation));
    setAccumulative(Operations::isAccumulative(operation));
}


int DFGNode::numInputs(){ 
    return (hasImm())? (_inputs.size()+1) : _inputs.size(); 
}

void DFGNode::printDfgNode(){
    printGraphNode();
    std::cout << "operation: " << _operation << ", opLatency: " << _opLatency << std::endl;
    std::cout << "commutative: " << _commutative << std::endl;
    std::cout << "accumulative: " << _accumulative << std::endl;
    std::cout << "imm: " << _imm << ", immIdx: " << _immIdx << std::endl;    
}

void DFGNode::print(){
    printDfgNode();
}



// ===================================================
//   DFGIONode functions
// ===================================================

void DFGIONode::print(){
    printDfgNode();
    std::cout << "memRefName: " << _memRefName << std::endl;
    std::cout << "pattern: ";
    for(auto &elem : _pattern){
        std::cout << elem.first << " " << elem.second << " ";
    }
    std::cout << std::endl;    
}
