#ifndef __LLVM_CDFG_EDGE_H__
#define __LLVM_CDFG_EDGE_H__

#include "llvm_cdfg_node.h"



enum EdgeType{ 
    EDGE_TYPE_DATA, // data dependence
    EDGE_TYPE_CTRL, // control dependence
    EDGE_TYPE_MEM   // loop-carried memory dependence 
};


class LLVMCDFGEdge
{
private:
    int _id;
    EdgeType _type;
    int dstport;
    LLVMCDFGNode *_src;
    LLVMCDFGNode *_dst;
public:
    LLVMCDFGEdge(){}
    LLVMCDFGEdge(int id, EdgeType type = EDGE_TYPE_DATA) : _id(id), _type(type){}
    LLVMCDFGEdge(int id, EdgeType type, LLVMCDFGNode *src, LLVMCDFGNode *dst) : _id(id), _type(type), _src(src), _dst(dst){}
    ~LLVMCDFGEdge(){}
    int id(){ return _id; }
    void setId(int id){ _id = id; }
    EdgeType type(){ return _type; }
    void setType(EdgeType type){ _type = type; }
    LLVMCDFGNode* src(){ return _src; }
    void setSrc( LLVMCDFGNode *src){ _src = src; }
    LLVMCDFGNode* dst(){ return _dst; }
    void setDst( LLVMCDFGNode *dst){ _dst = dst; }
};





#endif