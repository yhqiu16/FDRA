#ifndef __LLVM_CDFG_NODE_H__
#define __LLVM_CDFG_NODE_H__

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/CFGPrinter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/GraphTraits.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define MAX_DATA_BYTES 4
typedef int DataType;

using namespace llvm;

class LLVMCDFG;

// conditional value
enum CondVal{UNCOND, TRUE_COND, FALSE_COND};

// memory access dependence type
// Flow: RAW, Read After Write
// Anti: WAR, Write AFter Read
// Output: WAW, Write After Write
// Input: RAR, Read After Read
enum DepType{NON_DEP, FLOW_DEP, ANTI_DEP, OUTPUT_DEP, INPUT_DEP};

// memory access dependenc info
struct DependInfo
{
    DepType type;
    bool isConstDist; // is distance constant
    int distance = 0; // loop interation distance, e.g. a[i+1] = a[i]
};

struct NodeInfo
{
    int idx = -1; // connected port index of this node, -1, donot care, 0-n port index
    // bool isPHI = false;
    bool isBackEdge = false;
    CondVal cond = UNCOND;
};


class LLVMCDFGNode
{
private:
    int _id; 
    LLVMCDFG *_parent;
    BasicBlock *_BB;
    Instruction *_instruction = NULL;
    std::string _customInstruction; // customized instruction, e.g. LOOPSTART
    std::string _finalInstruction;  // final instruction supported by hardware
    std::vector<LLVMCDFGNode *> _inputNodes;  // inputs : predecessor instructions
    std::vector<LLVMCDFGNode *> _outputNodes; // outputs : successor instructions
    std::set<int> _inputEdges; // edge ids
    std::set<int> _outputEdges; // edge ids
    std::map<LLVMCDFGNode *, NodeInfo> _inputInfoMap;  // back-edge : input -> this node
    std::map<LLVMCDFGNode *, NodeInfo> _outputInfoMap; // back-edge :this node -> output
    std::map<LLVMCDFGNode *, DependInfo> _srcDepInfoMap; // loop-carried dependence source instructions
    std::map<LLVMCDFGNode *, DependInfo> _dstDepInfoMap; // loop-carried dependence destination instructions
    std::map<int, LLVMCDFGNode*> _inputport;
    // constant
    DataType _constVal;
    bool _hasConst = false;
    // data width in bits
    int _dataBits;

    // acc increment
    bool _isAcc=false;
    int _accfirst=0;
    int _initial;//initial const

    // Load/Store ins address
    Value *_LSaddress;
    // Load/Store offset for SPAD
    int _LSoffset;

    // affine access pattern for Load/Store--key:level;value:stride
    bool _isLSaffine = false;
    std::map<int, int> _LSstride;
    //start offset for pattern access(if _LSstart<0, LS is totally fixed)
    int _LSstart;
    std::map<int, int[3]> _LSbounds;
    
public:
    LLVMCDFGNode(LLVMCDFG *parent) : _parent(parent){}
    LLVMCDFGNode(Instruction *ins, LLVMCDFG *parent);
    ~LLVMCDFGNode(){}

    // get/set id
    void setId(int id){ _id = id; }
    int id(){ return _id; }
    LLVMCDFG* parent(){ return _parent; }
    BasicBlock* BB(){ return _BB; }
    void setBB(BasicBlock *BB){ _BB = BB; }
    Instruction* instruction(){ return _instruction; }
    void setInstruction(Instruction *ins);
    std::string customInstruction(){ return _customInstruction; }
    void setCustomInstruction(std::string ins){ _customInstruction = ins; }
    std::string finalInstruction(){ return _finalInstruction; }
    void setFinalInstruction(std::string ins){ _finalInstruction = ins; }
    std::string getName();
    const std::vector<LLVMCDFGNode *>& inputNodes(){ return _inputNodes; }  // inputs : predecessor nodes
    const std::vector<LLVMCDFGNode *>& outputNodes(){ return _outputNodes; } // outputs : successor nodes
    const std::map<LLVMCDFGNode *, NodeInfo>& inputInfoMap(){ return _inputInfoMap; }  // back-edge : input -> this node
    const std::map<LLVMCDFGNode *, NodeInfo>& outputInfoMap(){ return _outputInfoMap; } // back-edge :this node -> output
    void addInputNode(LLVMCDFGNode *node, int idx = -1, bool isBackEdge = false, CondVal cond = UNCOND);  // inputs : predecessor nodes
    void addOutputNode(LLVMCDFGNode *node, bool isBackEdge = false, CondVal cond = UNCOND);// outputs : successor nodes
    void setInputIdx(LLVMCDFGNode *node, int idx);                // input index
    void setInputBackEdge(LLVMCDFGNode *node, bool isBackEdge);   // input -> this node is back-edge
    void setInputCondVal(LLVMCDFGNode *node, CondVal cond);       // conditional dependence between inputs and this node
    void setOutputBackEdge(LLVMCDFGNode *node, bool isBackEdge);  // this node -> output is back-edge
    void setOutputCondVal(LLVMCDFGNode *node, CondVal cond);      // conditional dependence between this node and output
    int getInputIdx(LLVMCDFGNode *node);           // input index
    std::map<int, LLVMCDFGNode*> getInputPort(){return _inputport;}
    LLVMCDFGNode* getInputPort(int idx){return _inputport[idx];}
    bool isInputBackEdge(LLVMCDFGNode *node);      // input -> this node is back-edge
    CondVal getInputCondVal(LLVMCDFGNode *node);   // conditional dependence between inputs and this node
    bool isOutputBackEdge(LLVMCDFGNode *node);     // this node -> output is back-edge
    CondVal getOutputCondVal(LLVMCDFGNode *node);  // conditional dependence between this node and output
    int delInputNode(LLVMCDFGNode *node);  // inputs : predecessor instructions, return port index
    void delOutputNode(LLVMCDFGNode *node); // outputs : successor instructions

    const std::set<int>& inputEdges(){ return _inputEdges; }
    const std::set<int>& outputEdges(){ return _outputEdges; }
    void addInputEdge(int eid){ _inputEdges.insert(eid); }
    void addOutputEdge(int eid){ _outputEdges.insert(eid); }
    void delInputEdge(int eid){ _inputEdges.erase(eid); }
    void delOutputEdge(int eid){ _outputEdges.erase(eid); }

    const std::map<LLVMCDFGNode *, DependInfo>& srcDepInfoMap(){ return _srcDepInfoMap; } // loop-carried dependence source instructions
    const std::map<LLVMCDFGNode *, DependInfo>& dstDepInfoMap(){ return _dstDepInfoMap; } // loop-carried dependence destination instructions
    DependInfo getSrcDep(LLVMCDFGNode *node);
    DependInfo getDstDep(LLVMCDFGNode *node);
    void addSrcDep(LLVMCDFGNode *node, DependInfo info); // loop-carried dependence source instructions
    void addDstDep(LLVMCDFGNode *node, DependInfo info); // loop-carried dependence destination instructions
    void delSrcDep(LLVMCDFGNode *node); // loop-carried dependence source instructions
    void delDstDep(LLVMCDFGNode *node); // loop-carried dependence destination instructions
    bool isConditional();

    //ConstantVal
	DataType constVal(){ return _constVal; }
	void setConstVal(DataType val){ _constVal = val; _hasConst = true;}
	void delConstVal(){ _constVal = 0; _hasConst = false; }
	bool hasConst(){ return _hasConst; }

    int dataBits(){ return _dataBits; }
    void setDataBits(int bits){ _dataBits = bits; }

    //record the target address name of Load/Store
    void setLSaddress(Value* LSadd){ _LSaddress = LSadd;}
    std::string getLSArrayName(){return _LSaddress->getName().str();}

    //get offset of LSU for SPAD
    void setLSoffset(int offset){_LSoffset = offset;}
    int getLSoffset(){return _LSoffset;}

    //record affine access of Load/Store
    void setLSstride(std::map<int, int> LSpattern){ _LSstride = LSpattern;_isLSaffine = true;}
    void setLSstride(int level, int levelLSpattern){ _LSstride[level] = levelLSpattern;_isLSaffine = true;}
    int getLSstride(int level){return _LSstride[level];}
    std::map<int, int> getLSstride(){return _LSstride;}
    bool isLSaffine() {return _isLSaffine;}
    void setLSbounds(std::map<int, int[3]> bounds){_LSbounds = bounds;}
    std::map<int, int[3]> getLSbounds(){return _LSbounds;}
    void setLSstart(int startoffset){_LSstart = startoffset;}
    //start offset for pattern access(if _LSstart<0, LS is totally fixed)
    int getLSstart(){return _LSstart;}

    //phi->acc
    bool isAcc(){return _isAcc;}
    void setAcc(){_isAcc=true;}
    void setAccFirst(){_accfirst=1;}
    int isAccFirst(){return _accfirst;}
    void setInitial(int ini){_initial = ini;}
    int getInitial(){return _initial;}
};





#endif