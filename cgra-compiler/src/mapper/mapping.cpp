
#include "mapper/mapping.h"


// reset mapping intermediate result and status
void Mapping::reset(){
    _dfgNodeAttr.clear();
    _dfgEdgeAttr.clear();
    // _dfgInputAttr.clear();
    // _dfgOutputAttr.clear();
    _adgNodeAttr.clear();    
    // _adgLinkAttr.clear();
    _fuDelayAttr.clear();
    _vioDfgEdges.clear();
    // _totalViolation = 0;
    _numNodeMapped = 0;
}


// if this input port of this ADG node is used
bool Mapping::isAdgNodeInPortUsed(int nodeId, int portIdx){
    if(_adgNodeAttr.count(nodeId)){
        auto& status = _adgNodeAttr[nodeId].inPortUsed;
        if(status.count(portIdx)){
            return status[portIdx];
        }
    }
    return false;
}


// if this output port of this ADG node is used
bool Mapping::isAdgNodeOutPortUsed(int nodeId, int portIdx){
    if(_adgNodeAttr.count(nodeId)){
        auto& status = _adgNodeAttr[nodeId].outPortUsed;
        if(status.count(portIdx)){
            return status[portIdx];
        }
    }
    return false;
}


// if the DFG node is already mapped
bool Mapping::isMapped(DFGNode* dfgNode){
    if(_dfgNodeAttr.count(dfgNode->id())){
        return _dfgNodeAttr[dfgNode->id()].adgNode != nullptr;
    }
    return false;
    
}

// if the ADG node is already mapped
bool Mapping::isMapped(ADGNode* adgNode){
    if(_adgNodeAttr.count(adgNode->id())){
        return _adgNodeAttr[adgNode->id()].dfgNode != nullptr;
    }
    return false;
}

// if the IOB node is available 
bool Mapping::isIOBFree(ADGNode* adgNode){
    return !isMapped(adgNode);
    // // assert(adgNode->type() == "IOB");
    // int id = adgNode->id();
    // if(_adgNodeAttr.count(id)){
    //     for(auto& elem : _adgNodeAttr[id].inPortUsed){
    //         if(elem.second){ // used as IB
    //             return false;
    //         }
    //     }
    //     for(auto& elem : _adgNodeAttr[id].outPortUsed){
    //         if(elem.second){ // used as OB
    //             return false;
    //         }
    //     }
    // }
    // return true;
}


// // if the IOB node is already used as IB 
// bool Mapping::isUsedAsIB(ADGNode* adgNode){
//     // assert(adgNode->type() == "IOB");
//     int id = adgNode->id();
//     if(_adgNodeAttr.count(id)){
//         for(auto& elem : _adgNodeAttr[id].inPortUsed){
//             if(elem.second){ // used as IB
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// // if the IOB node is already used as OB 
// bool Mapping::isUsedAsOB(ADGNode* adgNode){
//     // assert(adgNode->type() == "IOB");
//     int id = adgNode->id();
//     if(_adgNodeAttr.count(id)){
//         for(auto& elem : _adgNodeAttr[id].outPortUsed){
//             if(elem.second){ // used as OB
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// // if the IB node is available 
// bool Mapping::isIBAvail(ADGNode* adgNode){
//     assert(adgNode->type() == "IB");
//     int id = adgNode->id();
//     if(_adgNodeAttr.count(id)){
//         auto& inPortUsed = _adgNodeAttr[id].inPortUsed;
//         for(int i = 0; i < adgNode->numInputs(); i++){
//             if(!inPortUsed.count(i)){
//                 return true;
//             }else if(!inPortUsed[i]){
//                 return true;
//             }
//         }
//         return false;
//     }
//     return true;
// }

// // if the OB node is available 
// bool Mapping::isOBAvail(ADGNode* adgNode){
//     assert(adgNode->type() == "OB");
//     int id = adgNode->id();
//     if(_adgNodeAttr.count(id)){
//         auto& outPortUsed = _adgNodeAttr[id].outPortUsed;
//         for(int i = 0; i < adgNode->numOutputs(); i++){
//             if(!outPortUsed.count(i) || !outPortUsed[i]){
//                 return true;
//             }
//         }
//         return false;
//     }
//     return true;
// }

// // if the DFG input port is mapped
// bool Mapping::isDfgInputMapped(int idx){
//     if(_dfgInputAttr.count(idx) && !_dfgInputAttr[idx].routedEdgeIds.empty()){
//         return true;
//     }
//     return false;
// }

// // if the DFG output port is mapped
// bool Mapping::isDfgOutputMapped(int idx){
//     if(_dfgOutputAttr.count(idx) && !_dfgOutputAttr[idx].routedEdgeIds.empty()){
//         return true;
//     }
//     return false;
// }

// occupied ADG node of this DFG node
ADGNode* Mapping::mappedNode(DFGNode* dfgNode){
    int dfgNodeId = dfgNode->id();
    if(_dfgNodeAttr.count(dfgNodeId)){
        return _dfgNodeAttr[dfgNodeId].adgNode;
    }else{
        return nullptr;
    } 
}


// mapped DFG node of this ADG node
DFGNode* Mapping::mappedNode(ADGNode* adgNode){
    int adgNodeId = adgNode->id();
    if(_adgNodeAttr.count(adgNodeId)){
        return _adgNodeAttr[adgNodeId].dfgNode;
    }else{
        return nullptr;
    }
}


// just map DFG node to ADG node without routing the input/output edges
bool Mapping::mapDfgNodeNoRoute(DFGNode* dfgNode, ADGNode* adgNode){
    int dfgNodeId = dfgNode->id();
    // if(_dfgNodeAttr.count(dfgNodeId)){
    //     if(_dfgNodeAttr[dfgNodeId].adgNode == adgNode){
    //         return true;
    //     } else if(_dfgNodeAttr[dfgNodeId].adgNode != nullptr){
    //         return false;
    //     }
    // } 
    _dfgNodeAttr[dfgNodeId].adgNode = adgNode;
    // constant are only processed in configuration stage since it does not affect mapping
    // one operand is immediate and the operands are not commutative
    // if(dfgNode->hasImm() && !dfgNode->commutative()){ // assign constant port
    //     GPENode* gpeNode = dynamic_cast<GPENode*>(adgNode);
    //     for(auto inPort : gpeNode->operandInputs(dfgNode->immIdx())){
    //         if(adgAttr.inPortUsed.count(inPort) && adgAttr.inPortUsed[inPort] == true){ // already used
    //             return false;
    //         }
    //         adgAttr.inPortUsed[inPort] = true; // set all the input ports connetced to this operand as used
    //     }
    // }
    _adgNodeAttr[adgNode->id()].dfgNode = dfgNode;
    _numNodeMapped++;
    spdlog::debug("Map DFG node {0} to ADG node {1}", dfgNode->name(), adgNode->name()); 
    return true;
}


// map DFG node to ADG node and route the input/output edges connected to already-mapped DFG nodes
// map the connected input/output nodes alongside
bool Mapping::mapDfgNode(DFGNode* dfgNode, ADGNode* targetAdgNode){
    bool succeed = true;
    std::vector<DFGEdge*> routedEdges;
    // std::vector<DFGEdge*> inEdges;  // edges connected to DFG input node
    // std::vector<DFGEdge*> outEdges; // edges connected to DFG output node
    // route the src edges whose src nodes have been mapped or connected to DFG input node
    for(auto& edgePair : dfgNode->inputEdges()){
        DFGEdge* edge = _dfg->edge(edgePair.second);
        DFGNode* inNode = _dfg->node(edge->srcId());
        ADGNode* adgNode = mappedNode(inNode);
        bool routed = false;
        if(adgNode){
            succeed = routeDfgEdge(edge, adgNode, targetAdgNode); // route edge between targetAdgNode and adgNode       
            routed = true;          
        // }else if(_dfg->isIONode(edge->srcId())){ // connected to DFG input node which has not been placed
        //     succeed = routeDfgEdge(edge, targetAdgNode, true); // route edge between targetAdgNode and ADG IOB
        //     inEdges.push_back(edge);
        //     routed = true; 
        }
        if(!succeed){
            break;
        }else if(routed){
            routedEdges.push_back(edge); // cache the routed edge
        }
    }
    if(succeed){
        // route the dst edge whose dst nodes have been mapped or connected to DFG output port
        for(auto& elem : dfgNode->outputEdges()){
            for(int outEdgeId : elem.second){
                DFGEdge* edge = _dfg->edge(outEdgeId);
                DFGNode* outNode = _dfg->node(edge->dstId());
                ADGNode* adgNode = mappedNode(outNode);
                bool routed = false;
                if(adgNode){
                    succeed = routeDfgEdge(edge, targetAdgNode, adgNode); // route edge between targetAdgNode and adgNode  
                    routed = true;                      
                // }else if(_dfg->isIONode(edge->dstId())){ // connected to DFG output node which has not been placed
                //     succeed = routeDfgEdge(edge, targetAdgNode, false); // route edge between targetAdgNode and ADG IOB
                //     outEdges.push_back(edge);
                //     routed = true; 
                }
                if(!succeed){
                    break;
                }else if(routed){
                    routedEdges.push_back(edge); // cache the routed edge
                }
            } 
            if(!succeed) break;  
        }
    }
    if(!succeed){
        for(auto re : routedEdges){ // unroute all the routed edges
            unrouteDfgEdge(re);
        }
        return false;
    }
    // map the DFG node to this targetAdgNode
    bool res = mapDfgNodeNoRoute(dfgNode, targetAdgNode);
    // spdlog::debug("Map DFG node {0} to ADG node {1}", dfgNode->name(), targetAdgNode->name()); 
    // // map DFG input and output nodes
    // for(auto& edge : inEdges){
    //     DFGNode* inDfgNode = _dfg->node(edge->srcId());
    //     auto inLinkAttr = dfgEdgeAttr(edge->id()).edgeLinks.begin(); // the link across the IOB
    //     ADGNode* inAdgNode = inLinkAttr->adgNode;
    //     res &= mapDfgNodeNoRoute(inDfgNode, inAdgNode);
    //     spdlog::debug("Map DFG node {0} to ADG node {1}", inDfgNode->name(), inAdgNode->name()); 
    // }
    // for(auto& edge : outEdges){
    //     DFGNode* outDfgNode = _dfg->node(edge->dstId());
    //     auto outLinkAttr = dfgEdgeAttr(edge->id()).edgeLinks.rbegin(); // the link across the IOB
    //     ADGNode* outAdgNode = outLinkAttr->adgNode;
    //     res &= mapDfgNodeNoRoute(outDfgNode, outAdgNode);
    //     spdlog::debug("Map DFG node {0} to ADG node {1}", outDfgNode->name(), outAdgNode->name()); 
    // }
    assert(res);
    return true;
}


// unmap DFG node without unrouting the input/output edges
void Mapping::unmapDfgNodeNoUnroute(DFGNode* dfgNode){
    ADGNode* adgNode = mappedNode(dfgNode);
    if(adgNode == nullptr) return;
    _adgNodeAttr.erase(adgNode->id());
    _dfgNodeAttr.erase(dfgNode->id());
    _numNodeMapped--;
    
}


// unmap DFG node, unroute its input/output edges and unmap the to-be-free input/output nodes if any
void Mapping::unmapDfgNode(DFGNode* dfgNode){
    // std::vector<DFGNode*> unmapInNodes;
    // unroute the input edges of this node
    for(auto& elem : dfgNode->inputEdges()){ // input-idx, edge-id
        DFGEdge* edge = _dfg->edge(elem.second);
        unrouteDfgEdge(edge);  
        // int inNodeId = edge->srcId();      
        // DFGNode* inNode = _dfg->node(inNodeId);    
        // if(_dfg->isIONode(inNodeId) && isMapped(inNode)){ // connected to mapped DFG input node
        //     unmapInNodes.push_back(inNode);
        // }
    }
    // unroute the output edges of this node
    for(auto& elem : dfgNode->outputEdges()){ // input-idx, edge-id
        for(auto& outEdgeId : elem.second){   // edge-id
            DFGEdge* edge = _dfg->edge(outEdgeId);
            unrouteDfgEdge(edge);  
            // int outNodeId = edge->dstId();          
            // if(_dfg->isIONode(outNodeId)){ // connected to DFG output node
            //     unmapDfgNodeNoUnroute(_dfg->node(outNodeId));
            // }
        }
    }
    // unmap this DFG node
    unmapDfgNodeNoUnroute(dfgNode);
    // // unmap the connected input nodes
    // for(auto& inNode : unmapInNodes){
    //     bool mapped = false;
    //     for(auto& elem : inNode->outputs()){
    //         for(auto& out : elem.second){
    //             if(isMapped(_dfg->node(out.first))){ // there are still routed edges connected to the dfg input node
    //                 mapped = true;
    //                 break;
    //             }
    //         }
    //         if(mapped) break;
    //     }
    //     if(!mapped){
    //         unmapDfgNodeNoUnroute(inNode);
    //     }
    // }
}


// // if the ADG link is already routed
// bool Mapping::isRouted(ADGLink* link){
//     return !_adgLinkAttr[link->id()].dfgEdges.empty();
// }
// if the DFG Edge is already routed
bool Mapping::isRouted(int dfgEdgeId){
    if(_dfgEdgeAttr.count(dfgEdgeId)){
        if(!_dfgEdgeAttr[dfgEdgeId].edgeLinks.empty()){
            return true;
        }
    }
    return false;
}

// routed DFG edge links, including inter-node and intra-node links
const std::vector<EdgeLinkAttr>& Mapping::routedEdgeLinks(DFGEdge* edge){
    assert(_dfgEdgeAttr.count(edge->id()));
    return _dfgEdgeAttr[edge->id()].edgeLinks;
}

// /* route DFG edge to ADG link
// *  one link can route multiple edges, but they should have the same srcId and srcPortIdx 
// */
// bool Mapping::routeDfgEdge(DFGEdge* edge, ADGLink* link){
//     if(_adgLinkAttr.count(link->id())){
//         for(auto& linkEdge : _adgLinkAttr[link->id()].dfgEdges){
//             if(linkEdge->srcId() != edge->srcId() || linkEdge->srcPortIdx() != edge->srcPortIdx()){
//                 return false; 
//             }
//         }
//     }
//     _adgLinkAttr[link->id()].dfgEdges.push_back(edge);
//     EdgeLinkAttr attr;
//     attr.isLink = true;
//     attr.edgeLink.adgLink = link;
//     _dfgEdgeAttr[edge->id()].edgeLinks.push_back(attr);
//     return true;
// }


// route DFG edge to passthrough GIB node
// one internal link can passthrough multiple edges, but they should have the same srcId and srcPortIdx
// isTry: just try to route, not change the routing status
bool Mapping::routeDfgEdgePass(DFGEdge* edge, ADGNode* passNode, int srcPort, int dstPort, bool isTry){
    assert(passNode->type() == "GIB");
    GIBNode* gibNode = dynamic_cast<GIBNode*>(passNode);
    int passNodeId = passNode->id();
    int routeSrcPort = srcPort;
    int routeDstPort = dstPort;
    bool hasSameSrcEdge = false;
    if(_adgNodeAttr.count(passNodeId)){
        auto& passNodeAttr = _adgNodeAttr[passNodeId];
        // check if conflict with current routed edges
        for(auto& edgeLink : passNodeAttr.dfgEdgePass){
            auto passEdge = edgeLink.edge;
            if((edgeLink.srcPort == srcPort || edgeLink.dstPort == dstPort) &&  // occupy the same port
               (passEdge->srcId() != edge->srcId() || passEdge->srcPortIdx() != edge->srcPortIdx())){
                return false; 
            } else if(edgeLink.srcPort == srcPort || edgeLink.dstPort == dstPort){ // try to route same-source edge to same internal link
                routeSrcPort = edgeLink.srcPort; // default route link
                routeDstPort = edgeLink.dstPort;
                hasSameSrcEdge = true;
            } 
        }
        if(srcPort >= 0 && dstPort >= 0){ // manually assign srcPort and dstPort
            if(!gibNode->isInOutConnected(srcPort, dstPort)){
                return false;
            } else if(isAdgNodeOutPortUsed(passNodeId, dstPort) && (!hasSameSrcEdge || (routeSrcPort != srcPort) || (routeDstPort != dstPort))){
                return false; // if dstPort used, must have same-source edge with same srcPort and dstPort
            } else if(isAdgNodeInPortUsed(passNodeId, srcPort) && (!hasSameSrcEdge || (routeSrcPort != srcPort))){
                return false; // if srcPort used, must have same-source edge with same srcPort
            }   
            routeSrcPort = srcPort;
            routeDstPort = dstPort;         
        } else if(srcPort >= 0){ // auto-assign dstPort           
            if(isAdgNodeInPortUsed(passNodeId, srcPort) && (!hasSameSrcEdge || (routeSrcPort != srcPort))){ 
                return false; // no same-source edge or have different srcPort
            }
            if(!isAdgNodeInPortUsed(passNodeId, srcPort)){                    
                bool flag = false;
                for(auto port : gibNode->in2outs(srcPort)){
                    if(isAdgNodeOutPortUsed(passNodeId, port)){ // already used
                        continue;
                    }
                    // find one available port
                    routeSrcPort = srcPort;
                    routeDstPort == port;
                    flag = true;
                    break;
                }
                if(!flag){ // cannot find one available port
                    return false;
                }
            }           
            // if have same-source edge and same srcPort, select the same dstPort            
        } else if(dstPort >= 0){ // auto-assign srcPort            
            if(isAdgNodeOutPortUsed(passNodeId, dstPort) && (!hasSameSrcEdge || (routeDstPort != dstPort))){
                return false;
            }
            if(!isAdgNodeOutPortUsed(passNodeId, dstPort)){ // no same-source edge or have different dstPort
                bool flag = false;
                for(auto port : gibNode->out2ins(dstPort)){
                    if(isAdgNodeInPortUsed(passNodeId, port)){ // already used
                        continue;
                    }
                    // find one available port
                    routeSrcPort == port;
                    routeDstPort = dstPort;
                    flag = true;
                    break;                    
                }
                if(!flag){ // cannot find one available port
                    return false;
                }                
            }            
            // if have same-source edge with same dstPort, select the same srcPort and dstPort          
        } else { // auto-assign srcPort and dstPort
            if(!hasSameSrcEdge){
                bool outflag = false;
                for(auto& elem : passNode->outputs()){
                    int outPort = elem.first;
                    if(isAdgNodeOutPortUsed(passNodeId, outPort)){ // already used
                        continue;
                    }
                    bool inflag = false;
                    for(auto inPort : gibNode->out2ins(outPort)){
                        if(isAdgNodeInPortUsed(passNodeId, inPort)){ // already used
                            continue;
                        }
                        // find one available inport
                        routeSrcPort == inPort;
                        routeDstPort = outPort;
                        inflag = true;
                        break;                        
                    }
                    if(inflag){ // find one available port
                        outflag = true;
                        break;
                    }
                }
                if(!outflag){ // cannot find one available port
                    return false;
                }
            }
        }
    } else { // _adgNodeAttr.count(passNodeId) = 0; this passNode has not been used
        bool outflag = false;
        for(auto& elem : passNode->outputs()){
            int outPort = elem.first;
            auto inPorts = gibNode->out2ins(outPort);
            if(!inPorts.empty()){ // find one available inport
                routeSrcPort == *(inPorts.begin());
                routeDstPort = outPort;
                outflag = true;
                break;
            }
        }
        if(!outflag){ // cannot find one available port
            return false;
        }
    }

    if(!isTry){
        EdgeLinkAttr edgeLinkAttr;    
        edgeLinkAttr.srcPort = routeSrcPort;
        edgeLinkAttr.dstPort = routeDstPort;
        edgeLinkAttr.adgNode = passNode;
        _dfgEdgeAttr[edge->id()].edgeLinks.push_back(edgeLinkAttr);
        DfgEdgePassAttr edgePassAttr;
        edgePassAttr.edge = edge;
        edgePassAttr.srcPort = routeSrcPort;
        edgePassAttr.dstPort = routeDstPort;
        _adgNodeAttr[passNodeId].inPortUsed[routeSrcPort] = true;
        _adgNodeAttr[passNodeId].outPortUsed[routeDstPort] = true;
        _adgNodeAttr[passNodeId].dfgEdgePass.push_back(edgePassAttr);        
    }
    return true;
}


// route DFG edge between srcNode and dstNode/OB
// find a routable path from srcNode to dstNode/OB by BFS
// dstNode = nullptr: OB, map output node
// dstPortRange: the input port index range of the dstNode
bool Mapping::routeDfgEdgeFromSrc(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode, const std::set<int>& dstPortRange){
    struct VisitNodeInfo{
        // int inPortIdx;  // input port index of this node 
        int srcNodeId;  // src node ID
        int srcInPortIdx; // input port index of src node
        int srcOutPortIdx; // output port index of src node
    };
    // cache visited node information, <<node-id, inport-index>, VisitNodeInfo>
    std::map<std::pair<int, int>, VisitNodeInfo> visitNodes; 
    // cache visited nodes, <node, inport-index>
    std::queue<std::pair<ADGNode*, int>> nodeQue; 
    // Breadth first search for possible routing path
    // assign the index of the output port of the srcNode
    int srcNodePortIdx = edge->srcPortIdx();
    int srcNodeId = edge->srcId();
    std::string dstNodeOp = _dfg->node(edge->dstId())->operation();
    // int dstNodePortIdx;
    // int mappedAdgOutPort;
    std::pair<int, int> finalDstNode; // <node-id, inport-index>
    bool success = false;
    nodeQue.push(std::make_pair(srcNode, -1));
    while(!nodeQue.empty()){
        auto queHead = nodeQue.front();
        ADGNode* adgNode = queHead.first;
        int inPortIdx = queHead.second;
        nodeQue.pop();
        VisitNodeInfo info;
        std::vector<int> outPortIdxs;
        if(inPortIdx == -1){ // srcNode
            outPortIdxs.push_back(srcNodePortIdx);
        }else{ // intermediate node, can only be GIB
            for(int outPortIdx : dynamic_cast<GIBNode*>(adgNode)->in2outs(inPortIdx)){
                if(!routeDfgEdgePass(edge, adgNode, inPortIdx, outPortIdx, true)){ // try to find an internal link to route the edge
                    continue;
                }
                outPortIdxs.push_back(outPortIdx); // collect all available outPort 
            }   
        } 
        // search this layer of nodes
        for(int outPortIdx : outPortIdxs){
            for(auto& elem : adgNode->output(outPortIdx)){
                int nextNodeId = elem.first;
                int nextSrcPort = elem.second;
                auto nextId = std::make_pair(nextNodeId, nextSrcPort);
                ADGNode* nextNode = _adg->node(nextNodeId);
                auto nextNodeType = nextNode->type();
                if((dstNode != nullptr) && (dstNode->id() == nextNodeId) && dstPortRange.count(nextSrcPort)){ // get to the dstNode
                    success = true;
                    finalDstNode = nextId;
                } else if((dstNode != nullptr) && // route to dstNode
                         ((dstNode->id() == nextNodeId) ||
                          (nextNodeType == "GPE") || // not use GPE node to route
                          (nextNodeType == "IOB") || // cannot route to the dstNode through IOB
                          visitNodes.count(nextId))){ // the <node-id, inport-index> already visited
                    continue;
                }else if((dstNode == nullptr) && nextNodeType == "IOB" && isIOBFree(nextNode) && 
                         dynamic_cast<IOBNode*>(nextNode)->opCapable(dstNodeOp)){ // route to OB
                    success = true;
                    finalDstNode = nextId;
                } else if((dstNode == nullptr) && // route to OB
                         ((nextNodeType == "IOB") || // already used
                          (nextNodeType == "GPE") || // not use GPE node to route
                          visitNodes.count(nextId))){ // the <node-id, inport-index> already visited
                    continue;
                } else if(isAdgNodeInPortUsed(nextNodeId, nextSrcPort)){ // the input port is already used
                    // if has the same srcId and srcPortIdx, nextId can still be used
                    auto& nextNodeAttr = _adgNodeAttr[nextNodeId];
                    bool conflict = false;
                    for(auto& edgeLink : nextNodeAttr.dfgEdgePass){
                        auto passEdge = edgeLink.edge;
                        if(edgeLink.srcPort == nextSrcPort && (passEdge->srcId() != srcNodeId || passEdge->srcPortIdx() != srcNodePortIdx)){
                            conflict = true; // the edge with different srcId or srcPortIdx occupied nextSrcPort
                            break;
                        }
                    }
                    if(conflict) continue;
                }
                nodeQue.push(std::make_pair(nextNode, nextSrcPort)); // cache in queue
                info.srcNodeId = adgNode->id(); //  current node ID
                info.srcInPortIdx = inPortIdx;  // input port index of src node
                info.srcOutPortIdx = outPortIdx;   //  output port index of current node 
                visitNodes[nextId] = info;
                if(success){
                    break;
                }  
            }
            if(success){
                break;
            }
        }
        if(success){
            break;
        }
    }
    if(!success){
        return false;
    }
    // route the found path
    auto routeNode = finalDstNode;
    auto& edgeLinks = _dfgEdgeAttr[edge->id()].edgeLinks;
    int dstPort = -1;
    while (true){
        int nodeId = routeNode.first;
        int srcPort = routeNode.second;
        // keep the DFG edge routing status
        EdgeLinkAttr edgeAttr;
        edgeAttr.srcPort = srcPort;
        edgeAttr.dstPort = dstPort;
        edgeAttr.adgNode = _adg->node(nodeId);        
        edgeLinks.push_back(edgeAttr);
        // keep the ADG Node routing status
        ADGNodeAttr& nodeAttr = _adgNodeAttr[nodeId];
        if(routeNode == finalDstNode){ // dstNode
         // std::cout << "Set ADG node " << routeNode.first << " inport " << srcPort << " used\n";
            nodeAttr.inPortUsed[srcPort] = true;  // only change the input port status
        } else if(nodeId == srcNode->id()){ // srcNode
            nodeAttr.outPortUsed[dstPort] = true; // only change the output port status
            break; // get to the srcNode
        } else{ // intermediate routing nodes
            nodeAttr.outPortUsed[dstPort] = true; 
            nodeAttr.inPortUsed[srcPort] = true;
            DfgEdgePassAttr passAttr;
            passAttr.edge = edge;
            passAttr.srcPort = srcPort;
            passAttr.dstPort = dstPort;
            nodeAttr.dfgEdgePass.push_back(passAttr);  
        }  
        dstPort = visitNodes[routeNode].srcOutPortIdx;
        routeNode = std::make_pair(visitNodes[routeNode].srcNodeId, visitNodes[routeNode].srcInPortIdx);
    }
    // reverse the passthrough nodes from srcNode to dstNode
    std::reverse(edgeLinks.begin(), edgeLinks.end());
    // map DFG output node
    if(dstNode == nullptr){
        bool res = mapDfgNodeNoRoute(_dfg->node(edge->dstId()), _adg->node(finalDstNode.first));
        assert(res);
    }
    return true;
}


// find a routable path from dstNode to srcNode/IB by BFS
// srcNode = nullptr: IB
bool Mapping::routeDfgEdgeFromDst(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode, const std::set<int>& dstPortRange){
    struct VisitNodeInfo{
        int dstNodeId;     // dst node ID
        int dstInPortIdx;  // input port index of dst node
        int dstOutPortIdx; // output port index of dst node
    };
    // cache visited node information, <<node-id, outport-index>, VisitNodeInfo>
    std::map<std::pair<int, int>, VisitNodeInfo> visitNodes; 
    // cache visited nodes, <node, outport-index>
    std::queue<std::pair<ADGNode*, int>> nodeQue; 
    // Breadth first search for possible routing path
    // assign the index of the output port of the srcNode
    int srcNodeOutPortIdx = edge->srcPortIdx();
    int srcNodeId = edge->srcId();
    std::string srcNodeOp = _dfg->node(srcNodeId)->operation();
    // int srcNodeInPortIdx = -1;
    // int mappedAdgInPort;
    std::pair<int, int> finalSrcNode; // <node-id, outport-index>
    bool success = false;
    DFGEdge* sameSrcEdge = nullptr; // the already-routed edge with the same srcId and srcPortIdx
    nodeQue.push(std::make_pair(dstNode, -1));
    while(!nodeQue.empty()){
        auto queHead = nodeQue.front();
        ADGNode* adgNode = queHead.first;
        int outPortIdx = queHead.second;
        nodeQue.pop();        
        std::vector<int> inPortIdxs;
        if(outPortIdx == -1){ // dstNode
            inPortIdxs.assign(dstPortRange.begin(), dstPortRange.end());
        }else{ // intermediate node, can only be GIB
            for(int inPortIdx : dynamic_cast<GIBNode*>(adgNode)->out2ins(outPortIdx)){
                if(!routeDfgEdgePass(edge, adgNode, inPortIdx, outPortIdx, true)){ // try to find an internal link to route the edge
                    continue;
                }
                inPortIdxs.push_back(inPortIdx); // collect all available inPort 
            }   
        } 
        // search this layer of nodes
        for(int inPortIdx : inPortIdxs){
            auto elem = adgNode->input(inPortIdx);
            int nextNodeId = elem.first;
            int nextDstPort = elem.second;
            auto nextId = std::make_pair(nextNodeId, nextDstPort);
            ADGNode* nextNode = _adg->node(nextNodeId);
            auto nextNodeType = nextNode->type();
            if((srcNode != nullptr) && (srcNode->id() == nextNodeId) && srcNodeOutPortIdx == nextDstPort){ // get to the srcNode
                success = true;
                finalSrcNode = nextId;
            } else if((srcNode != nullptr) && // route to srcNode
                     ((srcNode->id() == nextNodeId) ||
                      (nextNodeType == "GPE") || // not use GPE node to route
                      (nextNodeType == "IOB") || // cannot route to the srcNode through IOB
                      visitNodes.count(nextId))){ // the <node-id, outport-index> already visited
                continue;
            }else if((srcNode == nullptr) && nextNodeType == "IOB" && isIOBFree(nextNode) && 
                     dynamic_cast<IOBNode*>(nextNode)->opCapable(srcNodeOp)){ // route to free IOB
                success = true;
                finalSrcNode = nextId;
            } else if((srcNode == nullptr) && // route to IB
                     ((nextNodeType == "IOB") || // already used as OB
                      (nextNodeType == "GPE") || // not use GPE node to route
                      visitNodes.count(nextId))){ // the <node-id, outport-index> already visited
                continue;
            } else if(isAdgNodeOutPortUsed(nextNodeId, nextDstPort)){ // the output port is already used
                // if has the same srcId and srcPortIdx, nextId can still be used
                auto& nextNodeAttr = _adgNodeAttr[nextNodeId];
                bool conflict = false;
                for(auto& edgeLink : nextNodeAttr.dfgEdgePass){
                    auto passEdge = edgeLink.edge;
                    if(edgeLink.dstPort == nextDstPort && (passEdge->srcId() != srcNodeId || passEdge->srcPortIdx() != srcNodeOutPortIdx)){
                        conflict = true; // the edge with different srcId or srcPortIdx occupied nextDstPort
                        break;
                    }else if(edgeLink.dstPort == nextDstPort){
                        sameSrcEdge = passEdge; // the edge with the same srcId and srcPortIdx
                        finalSrcNode = nextId;
                        success = true; // REUSE part of the routing path of the edge
                        break;
                    }
                }
                if(conflict) continue;
            }
            nodeQue.push(std::make_pair(nextNode, nextDstPort)); // cache in queue
            VisitNodeInfo info;
            info.dstNodeId = adgNode->id(); //  current node ID
            info.dstInPortIdx = inPortIdx;  // input port index of src node
            info.dstOutPortIdx = outPortIdx;   //  output port index of current node 
            visitNodes[nextId] = info;
            if(success){
                break;
            }  
        }
        if(success){
            break;
        } 
    }
    if(!success){
        return false;
    }
    // route the found path
    auto routeNode = finalSrcNode;
    auto& edgeLinks = _dfgEdgeAttr[edge->id()].edgeLinks;
    int srcPort = -1; // (srcNode != nullptr)? -1 : srcNodeInPortIdx;
    // reuse part of the routing path of the edge
    if(sameSrcEdge != nullptr){
        auto& sameSrcEdgeLinks = _dfgEdgeAttr[sameSrcEdge->id()].edgeLinks;
        for(auto& link : sameSrcEdgeLinks){ // search from the source node
            // keep the DFG edge routing status
            edgeLinks.push_back(link);
            // keep the ADG Node routing status
            ADGNodeAttr& nodeAttr = _adgNodeAttr[link.adgNode->id()];
            DfgEdgePassAttr passAttr;
            passAttr.edge = edge;
            passAttr.srcPort = link.srcPort;
            passAttr.dstPort = link.dstPort;
            nodeAttr.dfgEdgePass.push_back(passAttr);
            if(link.adgNode->id() == finalSrcNode.first && link.dstPort == finalSrcNode.second){
                break;
            }
        }
        srcPort = visitNodes[finalSrcNode].dstInPortIdx;
        routeNode = std::make_pair(visitNodes[finalSrcNode].dstNodeId, visitNodes[finalSrcNode].dstOutPortIdx);
    }
    while (true){
        int nodeId = routeNode.first;
        int dstPort = routeNode.second;
        // keep the DFG edge routing status
        EdgeLinkAttr edgeAttr;
        edgeAttr.srcPort = srcPort;
        edgeAttr.dstPort = dstPort;
        edgeAttr.adgNode = _adg->node(nodeId);        
        edgeLinks.push_back(edgeAttr);
        // keep the ADG Node routing status
        ADGNodeAttr& nodeAttr = _adgNodeAttr[nodeId];
        if(routeNode == finalSrcNode){ // srcNode
            nodeAttr.outPortUsed[dstPort] = true; // only change the output port status 
        } else if(nodeId == dstNode->id()){ // dstNode
        // std::cout << "Set ADG node " << nodeId << " inport " << srcPort << " used\n";
            nodeAttr.inPortUsed[srcPort] = true;  // only change the input port status 
            break; // get to the dstNode
        } else{
            nodeAttr.inPortUsed[srcPort] = true;
            nodeAttr.outPortUsed[dstPort] = true;
            DfgEdgePassAttr passAttr;
            passAttr.edge = edge;
            passAttr.srcPort = srcPort;
            passAttr.dstPort = dstPort;
            nodeAttr.dfgEdgePass.push_back(passAttr);
        }        
        srcPort = visitNodes[routeNode].dstInPortIdx;
        routeNode = std::make_pair(visitNodes[routeNode].dstNodeId, visitNodes[routeNode].dstOutPortIdx);
    }
    // map DFG input node
    if(srcNode == nullptr){
        bool res = mapDfgNodeNoRoute(_dfg->node(srcNodeId), _adg->node(finalSrcNode.first));
        assert(res);
    }
    
    return true;
}


// find the available input ports in the dstNode to route edge
std::set<int> Mapping::availDstPorts(DFGEdge* edge, ADGNode* dstNode){
    DFGNode* dstDfgNode = _dfg->node(edge->dstId());
    int edgeDstPort = edge->dstPortIdx();
    int opereandNum = dstDfgNode->numInputs();
    FUNode* dstFuNode = dynamic_cast<FUNode*>(dstNode);
    std::set<int> dstPortRange; // the input port index range of the dstNode
    std::vector<int> opIdxs; // operand indexes
    if(dstDfgNode->commutative()){ // operands are commutative, use all the operand indexes
        for(int opIdx = 0; opIdx < opereandNum; opIdx++){
            opIdxs.push_back(opIdx);
        }
    } else{ // operands are not commutative, use the edgeDstPort as the operand index
        opIdxs.push_back(edgeDstPort);
    }
    // select all the ports connected to available operand
    for(int opIdx : opIdxs){
        bool operandUsed = false; // if this operand is used
        auto& inPorts = dstFuNode->operandInputs(opIdx);
        for(int inPort : inPorts){ // input ports connected to the operand with index of opIdx
            if(isAdgNodeInPortUsed(dstFuNode->id(), inPort)){
                operandUsed = true;
                break;
            }
        }
        if(!operandUsed){ 
            for(int inPort : inPorts){ // all the ports are available
                dstPortRange.emplace(inPort);
            }                
        }
    }
    return dstPortRange;
}


// route DFG edge between srcNode and dstNode
// find a routable path from srcNode to dstNode by BFS
bool Mapping::routeDfgEdge(DFGEdge* edge, ADGNode* srcNode, ADGNode* dstNode){
    std::set<int> dstPortRange = availDstPorts(edge, dstNode); // the input port index range of the dstNode
    if(dstPortRange.empty()){ // no available input port in the dstNode
        return false;
    }
    return routeDfgEdgeFromDst(edge, srcNode, dstNode, dstPortRange);
    // if(!routeDfgEdgeFromDst(edge, srcNode, dstNode, dstPortRange)){
    //     return routeDfgEdgeFromSrc(edge, srcNode, dstNode, dstPortRange);
    // }
    // return true;
}


// route DFG edge between adgNode and IOB
// is2Input: whether connected to IB or OB
// bool Mapping::routeDfgEdge(DFGEdge* edge, ADGNode* adgNode, bool is2Input){
//     std::set<int> dstPortRange; // the input port index range of the dstNode
//     if(!is2Input){ // route to OB
//         return routeDfgEdgeFromSrc(edge, adgNode, nullptr, dstPortRange);       
//     } 
//     dstPortRange = availDstPorts(edge, adgNode); // the input port index range of the dstNode
//     if(dstPortRange.empty()){ // no available input port in the dstNode
//         return false;
//     }
//     return routeDfgEdgeFromDst(edge, nullptr, adgNode, dstPortRange);
// }


// unroute DFG edge and unmap the to-be-free input/output nodes if any
void Mapping::unrouteDfgEdge(DFGEdge* edge){
    int eid = edge->id();
    if(!_dfgEdgeAttr.count(eid)) return;
    auto& edgeAttr = _dfgEdgeAttr[eid]; 
    // std::cout << "unroute DFG edge " << eid << " (" << _dfg->edge(eid)->srcId() << ", " << _dfg->edge(eid)->dstId() << ")\n";
    // remove this edge from all the routed ADG links
    for(auto& edgeLink : edgeAttr.edgeLinks){
        auto node = edgeLink.adgNode;
        if(!_adgNodeAttr.count(node->id())) continue; 
        auto& nodeAttr = _adgNodeAttr[node->id()];                   
        auto& nodeEdges = nodeAttr.dfgEdgePass;
        auto iter = std::remove_if(nodeEdges.begin(), nodeEdges.end(), [&](DfgEdgePassAttr& x){ return (x.edge == edge); });
        nodeEdges.erase(iter, nodeEdges.end());
        bool setInPortUnused = (edgeLink.srcPort != -1);
        bool setOutPortUnused = (edgeLink.dstPort != -1);
        for(auto& nodeEdge : nodeEdges){ // the srcPort/dstPort may be used by other edges
            if(setInPortUnused && (nodeEdge.srcPort == edgeLink.srcPort)){
                setInPortUnused = false; // do not set unused
            }
            if(setOutPortUnused && (nodeEdge.dstPort == edgeLink.dstPort)){
                setOutPortUnused = false;
            }
            if(!setInPortUnused && !setOutPortUnused){
                break;
            }
        }
        if(setInPortUnused){
            nodeAttr.inPortUsed[edgeLink.srcPort] = false;
            // if(node->type() == "GPE"){
            //     std::cout << "Set ADG node " << node->id() << " inport " << edgeLink.srcPort << " unused\n";
            // }
        }
        if(setOutPortUnused){
            nodeAttr.outPortUsed[edgeLink.dstPort] = false;
        }            
    }
    _dfgEdgeAttr.erase(eid);
    // // unmap DFG IO Node
    // std::vector<int> ioNodeIds;
    // ioNodeIds.push_back(edge->dstId());     
    // ioNodeIds.push_back(edge->srcId());       
    // for(int ioNodeId : ioNodeIds){       
    //     if(_dfg->isIONode(ioNodeId)){ // connected to DFG IO node
    //         DFGNode *ioNode = _dfg->node(ioNodeId);
    //         bool mapped = false;
    //         std::vector<int> eids;
    //         for(auto& elem : ioNode->inputEdges()){
    //             eids.push_back(elem.second);            
    //         }
    //         for(auto& elem : ioNode->outputEdges()){
    //             for(auto& out : elem.second){
    //                 eids.push_back(out);
    //             }
    //         }
    //         for(auto eid : eids){
    //             if(isRouted(eid)){ // there are still routed edges connected to the dfg io node
    //                 mapped = true;
    //                 break;
    //             }
    //         }
    //         if(!mapped){
    //             unmapDfgNodeNoUnroute(ioNode);
    //         }
    //     }
    // }
}


// if succeed to map all DFG nodes
bool Mapping::success(){
    return _dfg->nodes().size() == _numNodeMapped;
}


// total/max edge length (link number)
void Mapping::getEdgeLen(int& totalLen, int& maxLen){
    int total = 0;
    int maxVal = 0;
    for(auto& elem : _dfgEdgeAttr){
        int num = elem.second.edgeLinks.size();
        total += num;
        maxVal = std::max(maxVal, num);
    }
    totalLen = total;
    maxLen = maxVal;
}


// number of total mapped ADG nodes
int Mapping::getMappedAdgNodeNum(){
    return _adgNodeAttr.size();
}



// ==== tming schedule >>>>>>>>>>>>>
// // reset the timing bounds of each DFG node
// void Mapping::resetBound(){
//     // int INF = 0x3fffffff;
//     for(auto& elem : _dfg->nodes()){
//         auto& attr = _dfgNodeAttr[elem.second->id()];
//         attr.minLat = 0;
//         attr.maxLat = 0;
//     }
// }

// get currently available delay cycles in the FU node according to the mapped DFG node
int Mapping::getAvailDelay(FUNode* fuNode, DFGNode* dfgNode){
    if(fuNode->numOperands() < 2){
        return 0; // no delay unit
    }
    int maxDelay = fuNode->maxDelay();
    int numOp = dfgNode->inputs().size();
    if(numOp == 0) return 0;
    int availDelay;
    if(_fuDelayAttr.count(fuNode->id())){
        auto& attr = _fuDelayAttr[fuNode->id()];
        int leftOps = numOp - attr.delayUsed.size();
        availDelay = (maxDelay - attr.totalDelayUsed) / std::max(leftOps, 1); // left delay cycles divided by the left operands
    }else{
        availDelay = maxDelay / numOp;
    }
    return availDelay;
}


// calculate the routing latency of each edge, not inlcuding the RDU
void Mapping::calEdgeRouteLat(){
    for(auto& elem : _dfgEdgeAttr){
        auto& attr = elem.second;
        int lat = 0;
        for(auto& linkAttr : attr.edgeLinks){
            if(linkAttr.adgNode->type() == "GIB"){ // edge only pass-through GIB nodes
                bool reged = dynamic_cast<GIBNode*>(linkAttr.adgNode)->outReged(linkAttr.dstPort); // output port reged
                lat += reged;
            }
        }
        attr.lat = lat;
        // attr.latNoDelay = lat;
    }
}


// calculate the DFG node latency bounds not considering the Delay components, including 
// min latency of the output ports
// ID of the DFG node with the max latency
void Mapping::latencyBound(){
    int maxLatDfg = 0; // max latency of DFG
    int maxLatNodeId;  // ID of the DFG node with the max latency
    // calculate the LOWER bounds in topological order
    // int inputLat = _adg->loadLatency(); // IOB input latency
    // auto dfgInNodes = _dfg->getInNodes(); // In nodes: INPUT node, LOAD node without input
    for(auto nodeId : _dfg->topoNodes()){
        DFGNode *node = _dfg->node(nodeId);
        int maxLat = 0; // max latency of the input ports
        int nodeOpLat = _dfg->node(nodeId)->opLatency(); // operation latency
        for(auto& elem : node->inputEdges()){
            int eid = elem.second;
            int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
            int srcNodeId = _dfg->edge(eid)->srcId();   
            int srcNodeLat = _dfgNodeAttr[srcNodeId].lat;                
            int inPortLat = srcNodeLat + routeLat; 
            maxLat = std::max(maxLat, inPortLat);
        }
        int lat = maxLat + nodeOpLat;
        _dfgNodeAttr[nodeId].lat = lat;
        if(lat >= maxLatDfg){
            maxLatDfg = lat;
            maxLatNodeId = nodeId;
        }       
    }
    _maxLat = maxLatDfg;
    _maxLatNodeId = maxLatNodeId;
}


// schedule the latency of each DFG node based on the mapping status
// DFG node latency: output port latency
// DFG edge latency: latency from the output port of src node to the ALU Input port of dst Node, including RDU
void Mapping::latencySchedule(){
    _fuDelayAttr.clear();
    std::set<int> scheduledNodeIds;  
    std::vector<int> unscheduledNodes = _dfg->topoNodes();
    std::reverse(unscheduledNodes.begin(), unscheduledNodes.end()); // in reversed topological order
    // calculate the routing latency of each edge, not inlcuding the RDU
    calEdgeRouteLat();
    // calculate the DFG node latency bounds, finding the max-latency path 
    latencyBound();
    // schedule the DFG nodes in the max-latency path
    DFGNode* dfgNode = _dfg->node(_maxLatNodeId);
    scheduledNodeIds.emplace(dfgNode->id());
    auto iterEnd = std::remove(unscheduledNodes.begin(), unscheduledNodes.end(), dfgNode->id());
    // unscheduledNodes.erase(std::remove(unscheduledNodes.begin(), unscheduledNodes.end(), dfgNode), unscheduledNodes.end());
    // int inputLat = _adg->loadLatency(); // IOB input latency 
    while(dfgNode){ // until getting to the input port
        int nodeId = dfgNode->id();     
        // std::cout << "id: " << nodeId << std::endl;   
        // use the latency lower bound as the target latency
        int inPortLat = _dfgNodeAttr[nodeId].lat - _dfg->node(nodeId)->opLatency(); // input port latency
        FUNode* fuNode = dynamic_cast<FUNode*>(_dfgNodeAttr[nodeId].adgNode); // mapped FU node (GPE/IOB)
        DFGNode* srcNode = nullptr;
        int inPort;
        for(auto& elem : dfgNode->inputEdges()){
            int eid = elem.second;
            inPort = elem.first;
            int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
            int srcNodeId = _dfg->edge(eid)->srcId();
            int srcNodeLat = _dfgNodeAttr[srcNodeId].lat;             
            if(inPortLat == srcNodeLat + routeLat){ 
                scheduledNodeIds.emplace(srcNodeId); // latency fixed
                srcNode = _dfg->node(srcNodeId);
                iterEnd = std::remove(unscheduledNodes.begin(), iterEnd, srcNodeId);
                // unscheduledNodes.erase(std::remove(unscheduledNodes.begin(), unscheduledNodes.end(), srcNode), unscheduledNodes.end());
                break; // only find one path
            } 
        }
        _fuDelayAttr[fuNode->id()].delayUsed[inPort] = 0;
        _dfgNodeAttr[nodeId].maxLat = inPortLat; // input port max latency 
        _dfgNodeAttr[nodeId].minLat = std::max(inPortLat - getAvailDelay(fuNode, dfgNode), 0); // input port min latency >= IOB Input latency
        dfgNode = srcNode;
    }
    unscheduledNodes.erase(iterEnd, unscheduledNodes.end());
    // schedule the DFG nodes not in the max-latency path
    while(!unscheduledNodes.empty()){
        for(auto iter = unscheduledNodes.begin(); iter != unscheduledNodes.end();){           
            int nodeId = *iter;
            dfgNode = _dfg->node(nodeId);
            // std::cout << "id: " << nodeId << std::endl;
            int maxLat = 0x3fffffff;
            int minLat = 0;
            bool fail = false; // fail to schedule this DFG node
            bool updated = false; // maxLat/minLat UPDATED
            for(auto& outsPerPort : dfgNode->outputEdges()){
                for(auto& eid : outsPerPort.second){
                    int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
                    int dstNodeId = _dfg->edge(eid)->dstId();
                    // if(dstNodeId == _dfg->id()){ // connected to DFG output port
                    //     continue;
                    // }
                    if(scheduledNodeIds.count(dstNodeId)){ // already scheduled                     
                        maxLat = std::min(maxLat, _dfgNodeAttr[dstNodeId].maxLat - routeLat);
                        minLat = std::max(minLat, _dfgNodeAttr[dstNodeId].minLat - routeLat);
                        updated = true;
                    }else{
                        fail = true;
                        break;
                    }
                }
                if(fail) break;
            }
            if(fail){
                iter++;
                continue;
            } 
            // if all its output ports are connected to DFG output ports, keep original latency
            // otherwise, update the latency
            int targetLat = _dfgNodeAttr[nodeId].lat; 
            if(updated){
                targetLat = std::max(targetLat, std::min(maxLat, minLat));
                _dfgNodeAttr[nodeId].lat = targetLat;
            }            
            // update             
            int maxInportLat = targetLat - _dfg->node(nodeId)->opLatency(); // input port max latency   
            _dfgNodeAttr[nodeId].maxLat = maxInportLat;   
            scheduledNodeIds.emplace(nodeId); // latency fixed
            iter = unscheduledNodes.erase(iter);
            // update the delay status and minLat of the this node if its srcNode already scheduled
            FUNode* fuNode = dynamic_cast<FUNode*>(_dfgNodeAttr[nodeId].adgNode); // mapped FU node (GPE/IOB)
            for(auto& elem : dfgNode->inputEdges()){
                int eid = elem.second;
                int inPort = elem.first;               
                int srcNodeId = _dfg->edge(eid)->srcId();
                if(scheduledNodeIds.count(srcNodeId)){ // already scheduled 
                    int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
                    int srcNodeLat = _dfgNodeAttr[srcNodeId].lat;     
                    int delayRequired = maxInportLat - routeLat - srcNodeLat;   
                    int delayUsed = std::min(delayRequired, getAvailDelay(fuNode, dfgNode));         
                    _fuDelayAttr[fuNode->id()].delayUsed[inPort] = delayUsed;
                    _fuDelayAttr[fuNode->id()].totalDelayUsed += delayUsed;
                }
            }
            _dfgNodeAttr[nodeId].minLat = std::max(maxInportLat - getAvailDelay(fuNode, dfgNode), 0); // input port min latency         
            // update the delay status and minLat of the dstNodes
            for(auto& outsPerPort : dfgNode->outputEdges()){
                for(auto& eid : outsPerPort.second){
                    int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
                    int dstNodeId = _dfg->edge(eid)->dstId();
                    // if(dstNodeId == _dfg->id()){ // connected to DFG output port
                    //     continue;
                    // }
                    DFGNode* dstNode = _dfg->node(dstNodeId);
                    int inPort = _dfg->edge(eid)->dstPortIdx();
                    int delayRequired = _dfgNodeAttr[dstNodeId].maxLat - routeLat - targetLat;                                  
                    FUNode* dstFuNode = dynamic_cast<FUNode*>(_dfgNodeAttr[dstNodeId].adgNode); // mapped FU node
                    int delayUsed = std::min(delayRequired, getAvailDelay(dstFuNode, dstNode));  
                    _fuDelayAttr[dstFuNode->id()].delayUsed[inPort] = delayUsed;
                    _fuDelayAttr[dstFuNode->id()].totalDelayUsed += delayUsed;
                    _dfgNodeAttr[dstNodeId].minLat = std::max(_dfgNodeAttr[dstNodeId].maxLat - getAvailDelay(dstFuNode, dstNode), 0); // input port min latency                     
                }
            }
        }
    }
    // calculate the latency of DFG IO
    // calIOLat();
    // calculate the latency violation of each edge
    calEdgeLatVio();
}


// // calculate the latency of DFG IO
// void Mapping::calIOLat(){
//     // DFG input port latency
//     // int inputLat = _adg->loadLatency(); // IOB input latency
//     for(auto& insPerPort : _dfg->inputEdges()){
//         int minLat = 0;
//         int maxLat = 0x3fffffff;
//         for(auto& eid : insPerPort.second){
//             int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
//             int dstNodeId = _dfg->edge(eid)->dstId();
//             // if(dstNodeId == _dfg->id()){ // connected to DFG output port
//             //     continue;
//             // }
//             minLat = std::max(minLat, _dfgNodeAttr[dstNodeId].minLat - routeLat);
//             maxLat = std::min(maxLat, _dfgNodeAttr[dstNodeId].maxLat - routeLat);
//         }
//         int targetLat = std::min(maxLat, minLat);
//         _dfgInputAttr[insPerPort.first].lat = targetLat;
//     }
//     int maxLat = 0;
//     // DFG output port latency
//     for(auto& elem : _dfg->outputEdges()){
//         int eid = elem.second;
//         int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
//         int srcNodeId = _dfg->edge(eid)->srcId();
//         int srcNodeLat;
//         if(srcNodeId == _dfg->id()){ // connected to DFG input port
//             srcNodeLat = _dfgInputAttr[_dfg->edge(eid)->srcPortIdx()].lat;               
//         } else{
//             srcNodeLat = _dfgNodeAttr[srcNodeId].lat;
//         }
//         int targetLat = srcNodeLat + routeLat;
//         _dfgOutputAttr[elem.first].lat = targetLat;
//         maxLat = std::max(maxLat, targetLat);
//     }
//     _maxLat = maxLat;
// }


// calculate the latency violation of each edge
void Mapping::calEdgeLatVio(){
    int dfgSumVio = 0; // total edge latency violation
    int dfgMaxVio = 0; // max edge latency violation
    _vioDfgEdges.clear(); // DFG edges with latency violation
    for(auto nodeId : _dfg->topoNodes()){
        DFGNode *node = _dfg->node(nodeId);
        // int minLat = _dfgNodeAttr[nodeId].minLat; // min latency of the input ports
        int maxLat = _dfgNodeAttr[nodeId].maxLat; // max latency of the input ports =  latency - operation_latency
        FUNode* fuNode = dynamic_cast<FUNode*>(_dfgNodeAttr[nodeId].adgNode);
        int fuNodeId = fuNode->id();
        auto& fuDelayAttr = _fuDelayAttr[fuNodeId];
        auto& delayUsed = fuDelayAttr.delayUsed;
        for(auto& elem : node->inputEdges()){
            int eid = elem.second;
            int routeLat = _dfgEdgeAttr[eid].lat; // latNoDelay;
            int srcNodeId = _dfg->edge(eid)->srcId();
            // int srcNodeLat;
            // if(srcNodeId != _dfg->id()){ // not connected to DFG input port
            int srcNodeLat = _dfgNodeAttr[srcNodeId].lat;                
            // } else{ // connected to DFG input port
            //     srcNodeLat = _dfgInputAttr[_dfg->edge(eid)->srcPortIdx()].lat;  
            // }
            // _dfgEdgeAttr[eid].lat = maxLat - srcNodeLat; // including RDU latency   
            int requiredDelay = maxLat - srcNodeLat - routeLat; // RDU latency  
            _dfgEdgeAttr[eid].delay = requiredDelay;    
            int assignedDelay; 
            if(!delayUsed.count(elem.first)){ // not assign delay for this port
                assignedDelay = std::min(getAvailDelay(fuNode, node), requiredDelay);
                delayUsed[elem.first] = assignedDelay;
                fuDelayAttr.totalDelayUsed += assignedDelay;
                _dfgNodeAttr[nodeId].minLat = std::max(_dfgNodeAttr[nodeId].maxLat - getAvailDelay(fuNode, node), 0); // input port min latency 
            }else{
                assignedDelay = delayUsed[elem.first];
            }         
            assert(assignedDelay <= requiredDelay && assignedDelay >= 0); 
            if(assignedDelay < requiredDelay){ // need to add pass node to compensate the latency gap
                int vio = requiredDelay - assignedDelay;
                _dfgEdgeAttr[eid].vio = vio;
                _vioDfgEdges.push_back(eid);
                dfgSumVio += vio;
                dfgMaxVio = std::max(dfgMaxVio, vio);
            } else {
                _dfgEdgeAttr[eid].vio = 0;
            }
        }    
    }
    _totalViolation = dfgSumVio;
    _maxViolation = dfgMaxVio;
}


// insert pass-through DFG nodes into a copy of current DFG
bool Mapping::insertPassDfgNodes(DFG* newDfg){
    int numGpeNodes = _adg->numGpeNodes();
    int numOpNodes = _dfg->nodes().size() - _dfg->ioNodes().size();
    int numAvail = numGpeNodes - numOpNodes;
    if(numAvail <= 0){ // cannot insert pass node
        return false;
    }
    *newDfg = *_dfg;
    int maxNodeId = newDfg->nodes().rbegin()->first; // std::map auto sort the key
    int maxEdgeId = newDfg->edges().rbegin()->first; 
    if(numAvail < _vioDfgEdges.size()){
        std::sort(_vioDfgEdges.begin(), _vioDfgEdges.end(), [&](int a, int b){
            return _dfgEdgeAttr[a].vio > _dfgEdgeAttr[b].vio;
        });
    }
    // int maxDelay = 1;
    // for(auto& elem : _dfgNodeAttr){
    //     auto adgNode = elem.second.adgNode;
    //     if(adgNode){
    //         maxDelay = dynamic_cast<GPENode*>(adgNode)->maxDelay() + 1;
    //         break;
    //     }
    // }
    // int maxInsertNodesPerEdge = 2;
    int n = 0;
    for(int eid : _vioDfgEdges){ // DFG edges with latency violation
        if(n >= numAvail){
            break;
        }
        n++;
        int vio = _dfgEdgeAttr[eid].vio; // maybe add multiple nodes according to vio   
        int num = 1; // std::min(maxInsertNodesPerEdge, std::max(1, vio/maxDelay));  
        DFGEdge* e = newDfg->edge(eid);
        int srcId = e->srcId();
        int dstId = e->dstId();
        int srcPortIdx = e->srcPortIdx();
        int dstPortIdx = e->dstPortIdx();
        newDfg->delEdge(eid);
        int lastId = srcId;
        int lastPort = srcPortIdx;
        for(int i = 0; i < num; i++){
            DFGNode* newNode = new DFGNode();
            newNode->setId(++maxNodeId);
            newNode->setName("pass"+std::to_string(maxNodeId));
            newNode->setOperation("PASS");
            newDfg->addNode(newNode);
            DFGEdge* e1 = new DFGEdge(lastId, maxNodeId);
            e1->setId(++maxEdgeId); 
            e1->setSrcPortIdx(lastPort);
            e1->setDstPortIdx(0);
            newDfg->addEdge(e1);
            lastId = maxNodeId;
            lastPort = 0;
        }           
        DFGEdge* e2 = new DFGEdge(maxNodeId, dstId);
        e2->setId(++maxEdgeId); 
        e2->setSrcPortIdx(0);
        e2->setDstPortIdx(dstPortIdx);        
        newDfg->addEdge(e2);
    }
    return true;
}