#ifndef __VISUALIZE_H__
#define __VISUALIZE_H__

#include "mapper/mapping.h"


class Graphviz
{
private:
    Mapping* _mapping; // from outside, not delete here
    std::string _dirname; // file directory name 
    // // create name for DFG node
    // std::string getDfgNodeName(int id, int idx = 0, bool isDfgInput = true);
    // // create name for ADG node
    // std::string getAdgNodeName(int id);
public:
    Graphviz(Mapping* mapping, std::string dirname);
    ~Graphviz(){}  
    // draw scheduled DFG with latency annotated
    void drawDFG();    
    // draw mapped ADG with mapped DFG annotated
    void drawADG();
    // dump mapped DFG IO ports with mapped ADG IO and latency annotated
    void dumpDFGIO();
    // print edge path
    void printDFGEdgePath();
};





#endif