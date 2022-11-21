#ifndef __DFG_NODE_H__
#define __DFG_NODE_H__

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <assert.h>
#include "graph/graph_node.h"
#include "op/operations.h"


// INPUT : input data to CGRA;  data = input()
// OUTPUT : output data from CGRA; void output(data)
// LOAD :  load data from external memory with address; data = load(addr)
// STORE : store data to external memory with address; void store(data, addr)

class DFGNode : public GraphNode
{
private:
    std::string _operation;
    int _opLatency = 1; // operation latency
    bool _commutative;  // if the inputs(operands) are commutative
    uint64_t _imm;      // immedaite operand (not exceed 64 bits)
    int _immIdx = -1;   // immedaite operand index, -1: no immediate 
    // Accumulate operation varibales; e.g. for(i = 2; i < 10; i += 2)
    bool _accumulative = false;
    bool _isAccFirst;  // if the accumulative value is the first result; e.g. false
    uint64_t _initVal;  // initial value, 2
    int _cycles;        // update number, (10-2)/2 = 2
    int _interval;      // update interval, 1
    int _repeats;       // repeat number
public:
    DFGNode(){}
    ~DFGNode(){}
    std::string operation(){ return _operation; }
    // set operation, latency, commutative according to operation name
    void setOperation(std::string operation);
    void setOpLatency(int opLat){ _opLatency = opLat; }
    int opLatency(){ return _opLatency; }
    bool commutative(){ return _commutative; }
    void setCommutative(bool cmu){ _commutative = cmu; }
    bool isAccFirst(){ return _isAccFirst; }
    void setIsAccFirst(bool isFirst){ _isAccFirst = isFirst; }
    uint64_t imm(){ return _imm; }
    void setImm(uint64_t imm){ _imm = imm; }
    int immIdx(){ return _immIdx; }
    void setImmIdx(int immIdx){ _immIdx = immIdx; }
    bool hasImm(){ return _immIdx >= 0; }

    bool accumulative(){ return _accumulative; }
    void setAccumulative(bool acc){ _accumulative = acc; }
    uint64_t initVal(){ return _initVal; }
    void setInitVal(uint64_t val){ _initVal = val; }
    void setCycles(int cycles){ _cycles = cycles; }
    int cycles(){ return _cycles; }
    void setInterval(int interval){ _interval = interval; }
    int interval(){ return _interval; }
    void setRepeats(int repeats){ _repeats = repeats; }
    int repeats(){ return _repeats; }

    virtual int numInputs();
    
    void printDfgNode();
    virtual void print();
};


// DFG I/O node: INPUT/OUTPUT/LOAD/STORE
class DFGIONode : public DFGNode
{
private:
    std::string _memRefName; // referred memory name for binding, eg. int A[20]; array name is A
    int _memOffset = 0;          // access memory address offset to the array base address, e.g. access A[15]~A[4], offset is 4*4 
    int _reducedMemOffset = 0;   // access address offset to the reduced memory block, e.g. access A[15]~A[4], offset is (15-4)*4 
    int _memSize;                // referred memory size in byte, e.g. size is (15+1-4)*4
    std::vector<std::pair<int, int>> _pattern; // memory access pattern, nested <stride, loop-cycles>
public:
    DFGIONode(){}
    // virtual ~DFGIONode(){}
    std::string memRefName(){ return _memRefName; }
    void setMemRefName(std::string name){ _memRefName = name; }
    int memOffset(){ return _memOffset; }
    void setReducedMemOffset(int offset){ _reducedMemOffset = offset; }
    int reducedMemOffset(){ return _reducedMemOffset; }
    void setMemOffset(int offset){ _memOffset = offset; }
    int memSize(){ return _memSize; }
    void setMemSize(int size){ _memSize = size; }
    const std::vector<std::pair<int, int>>& pattern(){ return _pattern; }
    int getNestedLevels(){ return _pattern.size(); }
    void addPatternLevel(int stride, int cycles){ _pattern.push_back(std::make_pair(stride, cycles)); }
    
    virtual void print();
};




#endif