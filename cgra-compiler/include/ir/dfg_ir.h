#ifndef __DFG_IR_H__
#define __DFG_IR_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cctype>
#include "dfg/dfg.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json; 

// struct OpTypeCount
// {
//     float numaddsub=0;
//     float nummul=0;
//     float numlogic=0;
//     float numcomp=0;
// };


class DFGIR
{
private:
    DFG* _dfg;
    std::map<std::string, int> _nodeName2id; // node name to ID map
    // std::map<std::string, int> _inputName2idx; // input name to input index map
    // std::map<std::string, int> _outputName2idx; // output name to output index map
    std::map<std::string, uint64_t> _name2Consts; // constant name to value
    // std::map<int, int> _inputId2idx; // input ID to input index map
    // std::map<int, int> _outputId2idx; // output ID to output index map
    std::map<int, uint64_t> _id2Consts; // constant ID to value
    DFG* parseDFG(std::string filename, std::string format = "json");
    // DFG* parseDFGDot(std::string filename); // deprecated
    // Json file transformed from dot file using graphviz
    DFG* parseDFGJson(std::string filename);
public:
    DFGIR(std::string filename);
    ~DFGIR();
    DFG* getDFG(){ return _dfg; }
    int nodeId(std::string name); // get node ID according to name
    void setNodeId(std::string name, int id);
    // int inputIdx(std::string name); // get input index according to name
    // void setInputIdx(std::string name, int idx);
    // int outputIdx(std::string name); // get output index according to name
    // void setOutputIdx(std::string name, int idx);
    uint64_t constValue(std::string name);  // get constant value according to name
    void setConst(std::string name, uint64_t value);
    bool isConst(std::string name);
    // int inputIdx(int id); // get input index according to ID
    // void setInputIdx(int id, int idx);
    // int outputIdx(int id); // get output index according to ID
    // void setOutputIdx(int id, int idx);
    uint64_t constValue(int id);  // get constant value according to ID
    void setConst(int id, uint64_t value);
    bool isConst(int id);
    // std::vector<std::string> addsub {"ADD","SUB","PASS"};
	// std::vector<std::string> logic {"AND","OR","XOR","SHL","LSHR","ASHR","PASS"};
	// std::vector<std::string> multiplier {"MUL","PASS"};
	// std::vector<std::string> comp {"EQ","NE","LT","LE","PASS"};
    // OpTypeCount optypecount;
    // OpTypeCount getNumType(){return optypecount;}
};






#endif