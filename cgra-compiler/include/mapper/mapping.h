#ifndef __MAPPING_H__
#define __MAPPING_H__

#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include "adg/adg.h"
#include "dfg/dfg.h"
#include "spdlog/spdlog.h"


// DFG node attributes used for mapping
struct DFGNodeAttr
{
    int minLat = 0; // min latency of the input port
    int maxLat = 0; // max latency of the input port
    int lat = 0;    // latency of the output port
    // int vio = 0;
    ADGNode* adgNode = nullptr;
};

// Edge link attributes : pass-through ADG node
// including inter-node and intra-node links
struct EdgeLinkAttr // internal link
{
    int srcPort; // source port of the passed node
    int dstPort; // destination port of the passed node
    ADGNode* adgNode; // pass-through node or src/dst node
};

// DFG edge attributes used for mapping
struct DFGEdgeAttr
{
    int lat = 0; // not including the delay pipe latency
    int delay = 0; // delay pipe latency
    int vio = 0;
    std::vector<EdgeLinkAttr> edgeLinks;
};

// pass-through DFG edge attributes, used for ADGNodeAttr
struct DfgEdgePassAttr
{
    int srcPort; // source port of the passed node
    int dstPort; // destination port of the passed node
    DFGEdge* edge;
}; 


// struct DFGIOAttr
// {
//     // int adgIOPort; // mapped ADG Input/Outut port 
//     int iobId; // mapped IOB ID
//     int lat;   // DFG I/O Port latency
//     std::set<int> routedEdgeIds; // IDs of the routed DFG edges
// };

// ADG node attributes used for mapping
struct ADGNodeAttr
{  
    DFGNode* dfgNode; 
    std::vector<DfgEdgePassAttr> dfgEdgePass;
    std::map<int, bool> inPortUsed;  // <port-idx, used>
    std::map<int, bool> outPortUsed; // <port-idx, used>
};

// // ADG link attributes used for mapping
// // one link can route multiple edges, but they should have the same srcId and srcPortIdx
// struct ADGLinkAttr
// {
//     std::vector<DFGEdge*> dfgEdges;
// };

// status of DelayPipe in FU node(GPE/IOB)
// the max reconfigurable delay cycles are shared among the operands
struct FUDelayAttr
{  
    int totalDelayUsed = 0; 
    std::map<int, int> delayUsed; // delay cycles used by each operand, <op-index, delay>
};

// Mapping App. DFG to CGRA ADG
// 1. provide basic mapping kit,
// 2. cache mapping results
class Mapping
{
private:
    ADG* _adg; // from outside, not delete here
    DFG* _dfg; // from outside, not delete here
    int _totalViolation; // total edge latency violation
    int _maxViolation; // max edge latency violation
    int _maxLat;    // max latency of DFG
    int _maxLatNodeId; // DFG node with max latency
    // int _maxLatMis; // max timing mismatch
    int _numNodeMapped = 0; // number of mapped DFG nodes
    // the ADG information of each mapped DFG node
    std::map<int, DFGNodeAttr> _dfgNodeAttr;
    // the ADG information of each mapped DFG edge
    std::map<int, DFGEdgeAttr> _dfgEdgeAttr;
    // // the ADG information of each mapped DFG input ports
    // std::map<int, DFGIOAttr> _dfgInputAttr;
    // // the ADG information of each mapped DFG output ports
    // std::map<int, DFGIOAttr> _dfgOutputAttr;
    // the DFG information of each occupied ADG node
    std::map<int, ADGNodeAttr> _adgNodeAttr;
    // // the DFG information of each occupied ADG link
    // std::map<int, ADGLinkAttr> _adgLinkAttr;
    // status of DelayPipe in each FU node(GPE/IOB)
    std::map<int, FUDelayAttr> _fuDelayAttr;

    // DFG edges with latency violation
    std::vector<int> _vioDfgEdges;

    // route DFG edge to passthrough ADG node, currently only GIB
    // if srcPort/dstPort == -1, auto-assign port; else assign provided port
    // one internal link can route multiple edges, but they should have the same srcId and srcPortIdx
    // isTry: just try to route, not change the routing status
    bool routeDfgEdgePass(DFGEdge* edge, ADGNode* passNode, int srcPort, int dstPort, bool isTry = false);
    // route DFG edge between srcNode and dstNode
    // dstPortRange: the input port index range of the dstNode
    // find a routable path from srcNode to dstNode/OB by BFS
    // dstNode = nullptr: OB, map output node
    bool routeDfgEdgeFromSrc(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode, const std::set<int>& dstPortRange);
    // find a routable path from dstNode to srcNode/IB by BFS
    // srcNode = nullptr: IB, map input node
    bool routeDfgEdgeFromDst(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode, const std::set<int>& dstPortRange);
    // find the available input ports in the dstNode to route edge
    std::set<int> availDstPorts(DFGEdge* edge, ADGNode* dstNode); 
public:
    Mapping(ADG* adg, DFG* dfg): _adg(adg), _dfg(dfg) {}
    ~Mapping(){}
    // void setDFG(DFG* dfg){ _dfg = dfg; }
    DFG* getDFG(){ return _dfg; }
    // void setADG(ADG* adg){ _adg = adg; }
    ADG* getADG(){ return _adg; }
    const DFGNodeAttr& dfgNodeAttr(int id){ return _dfgNodeAttr[id]; }
    const DFGEdgeAttr& dfgEdgeAttr(int id){ return _dfgEdgeAttr[id]; }
    const ADGNodeAttr& adgNodeAttr(int id){ return _adgNodeAttr[id]; }
    // const DFGIOAttr& dfgInputAttr(int idx){ return _dfgInputAttr[idx]; };
    // const DFGIOAttr& dfgOutputAttr(int idx){ return _dfgOutputAttr[idx]; };
    // reset mapping intermediate result and status
    void reset();
    int numNodeMapped(){ return _numNodeMapped; }
    // if this input port of this ADG node is used
    bool isAdgNodeInPortUsed(int nodeId, int portIdx);
    // if this output port of this ADG node is used
    bool isAdgNodeOutPortUsed(int nodeId, int portIdx);
    // if the DFG node is already mapped
    bool isMapped(DFGNode* dfgNode);
    // if the ADG node is already mapped
    bool isMapped(ADGNode* adgNode);
    // if the IOB node is free to use 
    bool isIOBFree(ADGNode* adgNode);
    // // if the IOB node is already used as IB 
    // bool isUsedAsIB(ADGNode* adgNode);
    // // if the IOB node is already used as OB 
    // bool isUsedAsOB(ADGNode* adgNode);
    // // if the IB node is available 
    // bool isIBAvail(ADGNode* adgNode);
    // // if the OB node is available 
    // bool isOBAvail(ADGNode* adgNode);
    // if the DFG input port is mapped
    // bool isDfgInputMapped(int idx);
    // // if the DFG output port is mapped
    // bool isDfgOutputMapped(int idx);
    // occupied ADG node of this DFG node
    ADGNode* mappedNode(DFGNode* dfgNode);
    // mapped DFG node of this ADG node
    DFGNode* mappedNode(ADGNode* adgNode);
    // just map DFG node to ADG node without routing the input/output edges
    bool mapDfgNodeNoRoute(DFGNode* dfgNode, ADGNode* adgNode);
    // map DFG node to ADG node and route the input/output edges connected to already-mapped DFG nodes
    bool mapDfgNode(DFGNode* dfgNode, ADGNode* adgNode);
    // unmap DFG node without unrouting the input/output edges
    void unmapDfgNodeNoUnroute(DFGNode* dfgNode);
    // unmap DFG node, unroute its input/output edges
    void unmapDfgNode(DFGNode* dfgNode);
    // if the DFG Edge is already routed
    bool isRouted(int dfgEdgeId);
    // routed DFG edge links, including inter-node and intra-node links
    const std::vector<EdgeLinkAttr>& routedEdgeLinks(DFGEdge* edge);
    // // route DFG edge to ADG link
    // // one link can route multiple edges, but they should have the same srcId and srcPortIdx
    // bool routeDfgEdge(DFGEdge* edge, ADGLink* link);
    // route DFG edge between srcNode and dstNode
    // find a routable path from srcNode to dstNode by BFS
    bool routeDfgEdge(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode);
    // route DFG edge between adgNode and IOB, map input/output node to IOB
    // is2Input: whether connected to IB or OB 
    // map the connected input/output nodes alongside
    bool routeDfgEdge(DFGEdge* edge, ADGNode* adgNode, bool is2Input);
    // unroute DFG edge and unmap the to-be-free input/output nodes if any
    void unrouteDfgEdge(DFGEdge* edge);
    // if succeed to map all DFG nodes
    bool success();
    // total/max edge length (link number)
    void getEdgeLen(int& totalLen, int& maxLen);
    // number of total mapped ADG nodes
    int getMappedAdgNodeNum();


    // ==== Data synchronization: schedule latencies of DFG node inputs and outputs >>>>>>>>>>>>>
    int totalViolation(){ return _totalViolation; }
    int maxViolation(){ return _maxViolation; }
    int maxLat(){ return _maxLat; }
    int numVioEdges(){ return _vioDfgEdges.size(); }
    // int maxLatMis(){ return _maxLatMis; }
    // get currently available delay cycles in the FU node according to the mapped DFG node
    int getAvailDelay(FUNode* fuNode, DFGNode* dfgNode);
    // // reset the latency bounds of each DFG node
    // void resetBound();
    // calculate the routing latency of each edge, not inlcuding the delay pipe
    void calEdgeRouteLat();
    // calculate the DFG node latency bounds not considering the Delay components 
    // including min latency of the output ports
    void latencyBound();
    // schedule the latency of each DFG node based on current mapping status
    void latencySchedule();
    // calculate the latency of DFG IO
    void calIOLat();
    // calculate the latency violation of each edge
    void calEdgeLatVio();
    // insert pass-through DFG nodes into a copy of current DFG
    void insertPassDfgNodes(DFG* newDfg);
};







#endif