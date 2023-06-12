#ifndef __ADG_NODE_H__
#define __ADG_NODE_H__

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <assert.h>
#include "graph/graph_node.h"

// configuration data location
struct CfgDataLoc{
    int low;  // lowest bit index 
    int high; // highest bit index
};


class ADG;

class ADGNode : public GraphNode
{
private:
    int _cfgBlkIdx;
    int _x;
    int _y;
    // int _cfgBitLen;
    std::vector<uint64_t> _cfgBits;
    // configuration info, <ID, CfgDataFormat>
    std::map<int, CfgDataLoc> _configInfo;
    // sub-ADG consisting of sub-modules
    ADG* _subADG = nullptr; 
protected:
    void printADGNode();
public:
    ADGNode(){}
    ADGNode(int id){ _id = id; }
    ~ADGNode(){} // cannot delete _subADG here because of pre-declare class; 

    int cfgBlkIdx(){ return _cfgBlkIdx; }
    int x(){ return _x; }
    int y(){ return _y; }
    // int cfgBitLen(){ return _cfgBitLen; }
    void setCfgBlkIdx(int cfgBlkIdx){ _cfgBlkIdx = cfgBlkIdx; }
    void setX(int x){ _x = x; }
    void setY(int y){ _y = y; }
    // void setCfgBitLen(int cfgBitLen){ _cfgBitLen = cfgBitLen; }
    ADG* subADG(){ return _subADG; }
    void setSubADG(ADG* subADG){ _subADG = subADG; }
    
    // configuration info, <ID, CfgDataFormat>
    const std::map<int, CfgDataLoc>& configInfo(){ return _configInfo;}
    // get config info for sub-module
    const CfgDataLoc& configInfo(int id);
    // add config info for sub-module
    void addConfigInfo(int id, CfgDataLoc subModuleCfg);
    // virtual void verify();
    // // analyze the connections among the internal sub-modules 
    // virtual void analyzeIntraConnect(){}
    virtual void print();
};


// Funtional Unit node, parent class of GPENode and IOBNode
class FUNode : public ADGNode
{
protected:
    // max delay cycles of the internal shared RDU
    int _maxDelay;
    // number of operands
    int _numOperands; 
    // supported operations
    std::set<std::string> _operations; 
    // indexes of input ports connected to each operand, vector<set<input-port>>
    std::vector<std::set<int>> _operandInputs;
    void printFU();
public:
    using ADGNode::ADGNode; // C++11, inherit parent constructors
    std::map<std::string, int> cfgIdMap; // config item IDs, e.g. BaseAddr, Latency, IsStore

    int maxDelay(){ return _maxDelay; }
    void setMaxDelay(int maxDelay){ _maxDelay = maxDelay; }
    int numOperands(){ return _numOperands; }
    void setNumOperands(int numOperands); // set numOperands and resize _operandInputs 
    const std::set<std::string>& operations(){ return _operations; }
    void addOperation(std::string op); // add supported operation
    void delOperation(std::string op); // delete supported operation
    bool opCapable(std::string op); // check if the operation is supported
    const std::set<int>& operandInputs(int opeIdx); // get input ports connected to this operand
    void addOperandInputs(int opeIdx, int inputIdx); // add input port connected to this operand
    void delOperandInputs(int opeIdx, int inputIdx);
    void addOperandInputs(int opeIdx, std::set<int> inputIdxs); // add input ports connected to this operand
    int getOperandIdx(int inputIdx); // get which operand this input is connected
    virtual void print();
};


class GPENode : public FUNode
{
private:
    int _numRfReg; // number of registers in RegFile 
public:
    using FUNode::FUNode; // C++11, inherit parent constructors
    int numRfReg(){ return _numRfReg; } 
    void setNumRfReg(int numRfReg){ _numRfReg = numRfReg; }
    virtual void print();
};


class IOBNode : public FUNode
{
private:
    int _index; // index in all the IOBs
    int _mode;
public:
    using FUNode::FUNode; // C++11, inherit parent constructors
    // std::map<std::string, int> iocCfgIdMap; // IO Controller config item IDs, e.g. BaseAddr, Latency, IsStore

    int index(){ return _index; }
    void setIndex(int index){ _index = index; }
    virtual void print();
};



class GIBNode : public ADGNode
{
private:
    bool _trackReged; // if there are registers in the track output ports
    std::map<int, bool> _outReged; // if there are registers in the output port 
    // inputs connected to each output, <output-index, set<input-index>>
    std::map<int, std::set<int>> _out2ins;
    // outputs connected to each input, <input-index, set<output-index>>
    std::map<int, std::set<int>> _in2outs; 
public:
    using ADGNode::ADGNode; // C++11, inherit parent constructors
    bool trackReged(){ return _trackReged; }
    void setTrackReged(bool trackReged){ _trackReged = trackReged; }
    bool outReged(int idx);
    void setOutReged(int idx, bool reged);

    // inputs connected to each output
    std::set<int> out2ins(int outPort);
    // outputs connected to each input
    std::set<int> in2outs(int inPort);
    // add input connected to the output
    void addOut2ins(int outPort, int inPort);
    // add output connected to the input
    void addIn2outs(int inPort, int outPort);
    // check if the input and output port are connected
    bool isInOutConnected(int inPort, int outPort); 

    virtual void print();
};





#endif