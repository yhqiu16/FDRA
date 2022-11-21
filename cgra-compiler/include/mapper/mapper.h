#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "mapper/mapping.h"
#include "mapper/visualize.h"
// #include "mapper/configuration.h"
#include "mapper/io_scheduler.h"
#include "spdlog/spdlog.h"


// CGRA Mapper: mapping App. DFG to CGRA ADG
class Mapper
{
private:
    ADG* _adg; // from outside, not delete here
    DFG* _dfg; // from outside, not delete here
    DFG* _dfgModified = nullptr; // DFG inserted passthrough nodes 
    bool _isDfgModified; // if DFG is inserted passthrough nodes 
    // // map ADG node id to continuous index starting from 0
    // std::map<int, int> _adgNodeId2Idx;
    // // distances among ADG nodes
    // std::vector<std::vector<int>> _adgNodeDist; // [node-idx][node-idx]
    // shortest distance between two ADG nodes (GPE nodes)
    std::map<std::pair<int, int>, int> _adgNode2NodeDist; // <<node-id, node-id>, dist>
    // shortest distance between ADG node (GPE node) and the ADG IO
    // std::map<int, std::pair<int, int>> _adgNode2IODist; // <node-id, <2input-dist, 2output-dist>>

protected:
    Mapping* _mapping = nullptr;
    IOScheduler* _sched;
    // mapping timeout in ms
    double _timeout; 
    // mapping start time point
    std::chrono::time_point<std::chrono::steady_clock> _start;
    // number of the candidate mapping ADG nodes of each DFG node in the _mapping->dfg
    // std::map<int, int> candidatesCnt; // <dfgnode-id, count>
    // std::map<int, std::vector<ADGNode*>> candidates; // <dfgnode-id, vector<adgnode>>
    // the DFG node IDs in placing order
    std::vector<int> dfgNodeIdPlaceOrder;
    // supported operation count of ADG
    std::map<std::string, int> adgOpCnt; 

public:
    // Mapper(){}
    Mapper(ADG* adg);
    Mapper(ADG* adg, DFG* dfg);
    ~Mapper();
    // set DFG and initialize DFG
    // modify: if the DFG is a modified one
    void setDFG(DFG* dfg, bool modify = false);
    DFG* getDFG(){ return _dfg; }
    // set modified DFG and delete the old one
    void setDfgModified(DFG* dfg);
    // if DFG is inserted passthrough nodes 
    bool isDfgModified(){ return _dfgModified; } 
    // set ADG and initialize ADG
    // void setADG(ADG* adg);
    ADG* getADG(){ return _adg; }
    // initialize mapping status of ADG
    void initializeAdg();
    // initialize mapping status of DFG
    void initializeDfg();
    // calculate the shortest path among ADG nodes
    void calAdgNodeDist();
    // get the shortest distance between two ADG nodes
    int getAdgNodeDist(int srcId, int dstId);
    // // get the shortest distance between ADG node and ADG input
    // int getAdgNode2InputDist(int id);
    // // get the shortest distance between ADG node and ADG input
    // int getAdgNode2OutputDist(int id);
    // calculate supported operation count of ADG
    void calAdgOpCnt();
    // initialize candidates of DFG nodes
    // void initializeCandidates();
    // calculate the number of the candidates for one DFG node
    int calCandidatesCnt(DFGNode* dfgNode, int maxCandidates);
    // sort the DFG node IDs in placing order
    void sortDfgNodeInPlaceOrder();

    // timestamp functions
    void setStartTime();
    void setTimeOut(double timeout);
    double getTimeOut(){ return _timeout; }
    //get the running time in millisecond
    double runningTimeMS();

    
    // check if the DFG can be mapped to the ADG according to the resources
    bool preMapCheck(ADG* adg, DFG* dfg);
    // map the DFG to the ADG
    virtual bool mapper() = 0;
    // mapper with running time
    bool mapperTimed();
    // execute mapping, timing sceduling, visualizing, config getting
    // dumpCallFunc : dump CGRA execution calll function
    // dumpMappedViz : dump mapped visual graph
    // resultDir: mapped result directory
    bool execute(bool dumpCallFunc = true, bool dumpMappedViz = true, std::string resultDir = "");

    // generate SoC application codes
    void genSoCApp(std::string resultDir, std::string appName);

    // get max latency of the mapped DFG
    int getMaxLat();
};







#endif