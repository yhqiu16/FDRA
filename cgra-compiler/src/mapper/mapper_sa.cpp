
#include "mapper/mapper_sa.h"

MapperSA::MapperSA(ADG* adg, int timeout, int maxIter, bool objOpt) : Mapper(adg){
    setTimeOut(timeout);
    setMaxIters(maxIter);
    setObjOpt(objOpt);
}

// MapperSA::MapperSA(ADG* adg, DFG* dfg) : Mapper(adg, dfg){}

MapperSA::MapperSA(ADG* adg, DFG* dfg, int timeout, int maxIter, bool objOpt) : Mapper(adg, dfg){
    setTimeOut(timeout);
    setMaxIters(maxIter);
    setObjOpt(objOpt);
}

MapperSA::~MapperSA(){}

// map the DFG to the ADG, mapper API
bool MapperSA::mapper(){
    bool succeed;
    if(_objOpt){ // objective optimization
        succeed = pnrSyncOpt();
    }else{
        succeed = pnrSync(_maxIters, MAX_TEMP, true) == 1;
    }
    return succeed;
}


// PnR, Data Synchronization, and objective optimization
bool MapperSA::pnrSyncOpt(){
    int temp = MAX_TEMP; // temperature
    int maxItersMapSched = 500; // pnrSync iteration number
    int maxItersNoImprv = 50;  // if not improved for maxItersNoImprv, end
    int restartIters = 20;     // if not improved for restartIters, restart from the cached status
    int lastImprvIter = 0;
    int lastRestartIter = 0;
    float newObj;
    float oldObj = 10.0;
    float minObj = 10.0;
    bool succeed = false;
    Mapping* bestMapping = new Mapping(getADG(), getDFG());
    Mapping* lastAcceptMapping = new Mapping(getADG(), getDFG());
    bool initObj = true;
    for(int iter = 0; iter < _maxIters; iter++){
        if(runningTimeMS() > getTimeOut()){
            break;
        }
        // PnR and Data Synchronization 
        int res = pnrSync(maxItersMapSched, temp, (!succeed));
        if(res == 0){ // fail to map
            spdlog::debug("PnR and Data Synchronization failed!");
            continue;
        }else if(res == -1){ // preMapCheck fail
            break;
        }
        succeed = true;
        spdlog::info("PnR and Data Synchronization succeed, start optimization");
        // Objective function
        newObj = objFunc(_mapping, initObj);
        spdlog::debug("Object: {:f}", newObj);
        float difObj = newObj - oldObj;
        if(metropolis(difObj, temp)){ // accept new solution according to the Metropolis rule
            if(newObj < minObj){ // get better result
                minObj = newObj;
                *bestMapping = *_mapping; // cache better mapping status, ##### DEFAULT "=" IS OK #####
                lastImprvIter = iter; 
                lastRestartIter = iter; 
                if(!initObj){
                    temp = annealFunc(temp); //  annealling
                }                
                spdlog::warn("###### Better object: {:f} ######", newObj);
            }
            *lastAcceptMapping = *_mapping; // can keep trying based on current status          
            oldObj = newObj;
        }else{
            *_mapping = *lastAcceptMapping; // restart from the cached status 
        }
        initObj = false;
        if(iter - lastImprvIter > maxItersNoImprv){ // if not improved for long time, STOP            
            break;
        }
        if(iter - lastRestartIter > restartIters){ // if not improved for some time, restart from the cached status 
            *_mapping = *bestMapping;
            lastRestartIter = iter;    
        } 
    }
    *_mapping = *bestMapping;
    delete bestMapping; 
    delete lastAcceptMapping;
    if(succeed){
        spdlog::warn("######## Best object: {:f} ########", minObj);
        std::cout << "Best max latency: " << _mapping->maxLat() << std::endl;
    }    
    return succeed;
}


// PnR and Data Synchronization
// return -1 : preMapCheck failed; 0 : fail; 1 : success
int MapperSA::pnrSync(int maxIters, int temp, bool modifyDfg){
    int initTemp = temp;
    ADG* adg = _mapping->getADG();
    DFG* dfg = _mapping->getDFG();
    Mapping* curMapping = new Mapping(adg, dfg);
    Mapping* lastAcceptMapping = new Mapping(adg, dfg);
    int numNodes = dfg->nodes().size();
    int maxItersNoImprv = 20 + numNodes/5; // if not improved for maxItersNoImprv, end
    // int restartIters = 20;     // if not improved for restartIters, restart from the cached status
    int lastImprvIter = 0;
    // int lastRestartIter = 0;
    int succeed = 0;
    bool update = false;
    int newVio;
    int oldVio = 0x7fffffff;
    int minVio = 0x7fffffff;
    for(int iter = 0; iter < maxIters; iter++){
        if(runningTimeMS() > getTimeOut()){
            break;
        }
        // if(iter & 0xf == 0){
        //     std::cout << ".";
        // }
        // PnR without latency scheduling of DFG nodes
        // int status = pnr(curMapping, temp);
        if(!incrPnR(curMapping)){ // fail to map
            spdlog::debug("PnR failed once!");
            unmapSA(curMapping, temp); 
            continue;
        }
        spdlog::info("PnR succeed, start data synchronization");
        // Data synchronization : schedule the latency of DFG nodes
        curMapping->latencySchedule();
        spdlog::info("Complete data synchronization, check latency violation");
        newVio = curMapping->totalViolation() * curMapping->numVioEdges(); // latency violations
        if(newVio == 0){
            succeed = 1;
            *_mapping = *curMapping; // keep better mapping status, ##### DEFAULT "=" IS OK #####
            break;
        }
        int difVio = newVio - oldVio;
        if(metropolis(difVio, temp)){ // accept new solution according to the Metropolis rule
            if(newVio < minVio){ // get better result
                minVio = newVio;
                *_mapping = *curMapping; // cache better mapping status, ##### DEFAULT "=" IS OK #####
                lastImprvIter = iter; 
                // lastRestartIter = iter; 
                temp = annealFunc(temp); //  annealling
                update = true;
                spdlog::warn("#### Smaller violation: {} ####", minVio);
            }
            *lastAcceptMapping = *curMapping; // can keep trying based on current status            
            oldVio = newVio;
        }else{
            *curMapping = *lastAcceptMapping; 
        }
        // if not improved for long time, insert pass-through nodes
        if(iter - lastImprvIter > maxItersNoImprv){ 
            if(!modifyDfg){ // cannot modify DFG, stop iteration
                break;
            }                                  
            DFG* newDfg = new DFG();
            int totalVio, maxVio;
            if(update){
                totalVio = _mapping->totalViolation();
                maxVio = _mapping->maxViolation();
                _mapping->insertPassDfgNodes(newDfg); // insert pass-through nodes into DFG
            }else{
                totalVio = lastAcceptMapping->totalViolation();
                maxVio = lastAcceptMapping->maxViolation();
                lastAcceptMapping->insertPassDfgNodes(newDfg); // insert pass-through nodes into DFG
            }            
            spdlog::warn("Min total latency violation: {}", minVio);
            spdlog::warn("Current total latency violation: {}", totalVio); 
            spdlog::warn("Current max latency violation: {}", maxVio);  
            spdlog::warn("Insert pass-through nodes into DFG");
            // newDfg->print();                
            setDFG(newDfg, true);  //  update the _dfg and initialize                                                           
            if(!preMapCheck(adg, newDfg)){
                succeed = -1;
                break;
            }
            delete curMapping; 
            curMapping = new Mapping(adg, newDfg);
            *lastAcceptMapping = *curMapping;
            lastImprvIter = iter; 
            // lastRestartIter = iter; 
            temp = initTemp;
            oldVio = 0x7fffffff;
            minVio = 0x7fffffff;
            update = false;
            int numNodesNew = _mapping->getDFG()->nodes().size();
            maxItersNoImprv = 20 + numNodesNew/5 + numNodesNew - numNodes;
            spdlog::warn("DFG node number: {}", numNodesNew);
            // continue;
        }
        // if(iter - lastRestartIter > restartIters){ // if not improved for some time, restart from the cached status 
        //     *curMapping = *_mapping;
        //     lastRestartIter = iter;    
        // } 
    }
    delete curMapping; 
    delete lastAcceptMapping;
    if(succeed > 0){
        spdlog::info("Max latency: {}", _mapping->maxLat());
    }    
    return succeed;
}



// PnR with SA temperature(max = 100)
// int MapperSA::pnr(Mapping* mapping, int temp){
//     unmapSome(mapping, temp);
//     return incrPnR(mapping);
// }


// unmap some DFG nodes
// void MapperSA::unmapSome(Mapping* mapping, int temp){
//     for(auto& elem : mapping->getDFG()->nodes()){
//         auto node = elem.second;
//         if((rand()%MAX_TEMP < temp) && mapping->isMapped(node)){
//             mapping->unmapDfgNode(node);
//         }
//     }
// }

// unmap some DFG nodes with SA temperature(max = 100)
void MapperSA::unmapSA(Mapping* mapping, int temp){
    auto dfg = mapping->getDFG();
    int N = mapping->numNodeMapped();
    int cnt = 0;
    for(int i = 0; i < N; i++){
        if(rand()%MAX_TEMP < temp){
            cnt++;
        }
    }
    int resvNum = N - cnt; // nodes keeping previous mapped location
    int numNodes = dfgNodeIdPlaceOrder.size();
    for(int i = resvNum; i < numNodes; i++){     
        auto dfgNode = dfg->node(dfgNodeIdPlaceOrder[i]);
        if(mapping->isMapped(dfgNode)){
            mapping->unmapDfgNode(dfgNode);
        }
    }
}


// incremental PnR, try to map all the left DFG nodes based on current mapping status
bool MapperSA::incrPnR(Mapping* mapping){
    auto dfg = mapping->getDFG();
    // start mapping
    bool succeed = true;
    for(int id : dfgNodeIdPlaceOrder){     
        if(dfg->isIONode(id)){ // IO node is mapped during mapping computing nodes
            continue;
        }     
        auto dfgNode = dfg->node(id);
        spdlog::debug("Mapping DFG node {0}, id: {1}", dfgNode->name(), id);        
        if(!mapping->isMapped(dfgNode)){
            // find candidate ADG nodes for this DFG node
            auto nodeCandidates = findCandidates(mapping, dfgNode, 30, 10);
            if(nodeCandidates.empty() || tryCandidates(mapping, dfgNode, nodeCandidates) == -1){
                // std::cout << "Cannot map DFG node " << dfgNode->id() << std::endl;
                spdlog::debug("Cannot map DFG node {0} : {1}", dfgNode->id(), dfgNode->name());
                // Graphviz viz(mapping, "results");
                // viz.printDFGEdgePath();
                succeed = false;
                break;
            }
        }
        // spdlog::debug("Mapping DFG node {0} : {1} to ADG node {2}", dfgNode->name(), id, mapping->mappedNode(dfgNode)->id());
    }
    return succeed;    
}



// try to map one DFG node to one of its candidates
// return selected candidate index
int MapperSA::tryCandidates(Mapping* mapping, DFGNode* dfgNode, const std::vector<ADGNode*>& candidates){
    // // sort candidates according to their distances with the mapped src and dst ADG nodes of this DFG node 
    // std::vector<int> sortedIdx = sortCandidates(mapping, dfgNode, candidates);
    int idx = 0;
    for(auto& candidate : candidates){
        if(mapping->mapDfgNode(dfgNode, candidate)){         
            // spdlog::debug("Map DFG node {0} to ADG node {1}", dfgNode->name(), candidate->name());   
            return idx;
        }
        idx++;
        spdlog::debug("Cannot map DFG node {0} to ADG node {1}", dfgNode->name(), candidate->name());
    }
    return -1;
}

// find candidates for one DFG node based on current mapping status
std::vector<ADGNode*> MapperSA::findCandidates(Mapping* mapping, DFGNode* dfgNode, int range, int maxCandidates){
    std::vector<ADGNode*> candidates;
    for(auto& elem : mapping->getADG()->nodes()){
        auto adgNode = elem.second;
        //select GPE node
        if(adgNode->type() != "GPE"){  
            continue;
        }
        GPENode* gpeNode = dynamic_cast<GPENode*>(adgNode);
        // check if the DFG node operationis supported
        if(!gpeNode->opCapable(dfgNode->operation())){
            continue;
        }
        if(!mapping->isMapped(gpeNode)){
            candidates.push_back(gpeNode);
        }
    }
    // randomly select candidates
    std::random_shuffle(candidates.begin(), candidates.end());
    int num = std::min((int)candidates.size(), range);
    candidates.erase(candidates.begin()+num, candidates.end());
    // sort candidates according to their distances with the mapped src and dst ADG nodes of this DFG node 
    std::vector<int> sortedIdx = sortCandidates(mapping, dfgNode, candidates);
    int cdtNum = std::min(num, maxCandidates);
    std::vector<ADGNode*> sortedCandidates;
    for(int i = 0; i < cdtNum; i++){
        sortedCandidates.push_back(candidates[sortedIdx[i]]);
    }
    return sortedCandidates;
}


// get the shortest distance between ADG node and the available IOB
int MapperSA::getAdgNode2IODist(Mapping* mapping, int id){
    // shortest distance between ADG node (GPE node) and the ADG IO
    int minDist = 0x7fffffff;
    for(auto& jnode : getADG()->nodes()){            
        if(jnode.second->type() == "IOB" && mapping->isIOBFree(jnode.second)){                
            minDist = std::min(minDist, getAdgNodeDist(jnode.first, id));
        }                   
    }
    return minDist;
}

// // get the shortest distance between ADG node and the available ADG output
// int MapperSA::getAdgNode2OutputDist(Mapping* mapping, int id){
//     int minDist = 0x7fffffff;
//     for(auto& jnode : getADG()->nodes()){            
//         if(jnode.second->type() == "OB" && mapping->isOBAvail(jnode.second)){                
//             minDist = std::min(minDist, getAdgNodeDist(id, jnode.first));
//         }                   
//     }
//     return minDist;
// }

// sort candidates according to their distances with the mapped src and dst ADG nodes of this DFG node 
// return sorted index of candidates
std::vector<int> MapperSA::sortCandidates(Mapping* mapping, DFGNode* dfgNode, const std::vector<ADGNode*>& candidates){
    // mapped ADG node IDs of the source and destination node of this DFG node
    std::vector<int> srcAdgNodeId, dstAdgNodeId; 
    int num2in = 0;  // connected to DFG input port
    int num2out = 0; // connected to DFG output port
    DFG* dfg = mapping->getDFG();
    for(auto& elem : dfgNode->inputs()){
        int inNodeId = elem.second.first;
        auto inNode = dfg->node(inNodeId);
        auto adgNode = mapping->mappedNode(inNode);
        if(adgNode){
            srcAdgNodeId.push_back(adgNode->id());
        }else if(dfg->isIONode(inNodeId)){ // connected to DFG input node
            num2in++;
        }
    }
    for(auto& elem : dfgNode->outputs()){
        for(auto outNode : elem.second){
            auto adgNode = mapping->mappedNode(dfg->node(outNode.first));
            if(adgNode){
                dstAdgNodeId.push_back(adgNode->id());
            }else if(dfg->isIONode(outNode.first)){ // connected to DFG output node
                num2out++;
            }
        }        
    }
    // sum distance between candidate and the srcAdgNode & dstAdgNode & IO
    std::vector<int> sortedIdx, sumDist; // <candidate-index, sum-distance>
    for(int i = 0; i < candidates.size(); i++){
        int sum = 0;
        int cdtId = candidates[i]->id();
        for(auto id : srcAdgNodeId){
            sum += getAdgNodeDist(id, cdtId);
        }
        for(auto id : dstAdgNodeId){
            sum += getAdgNodeDist(cdtId, id);
        }
        sum += (num2in + num2out) * getAdgNode2IODist(mapping, cdtId);
        sumDist.push_back(sum);
        sortedIdx.push_back(i);
    }
    std::sort(sortedIdx.begin(), sortedIdx.end(), [&sumDist](int a, int b){
        return sumDist[a] < sumDist[b];
    });
    return sortedIdx;
}



// objective funtion
float MapperSA::objFunc(Mapping* mapping, bool init){
    float w_lat = 0.15;  // weight of _dfgLat
    float w_node = 0.25; // weight of _mappedAdgNodeNum
    float w_dep = 0.6;  // weight of _ioDeps
    float obj = 1.0;
    _sched->ioSchedule(mapping);
    if(init){
        _dfgLat = mapping->maxLat();
        _mappedAdgNodeNum = mapping->getMappedAdgNodeNum();
        _ioDeps = _sched->getDepCost();
        if(_ioDeps == 0){
            obj = 1.0 - w_dep;
        }
    }else{
        obj = w_lat * mapping->maxLat() / _dfgLat +
              w_node * mapping->getMappedAdgNodeNum() / _mappedAdgNodeNum;              
        if(_ioDeps > 0){
            obj += w_dep * _sched->getDepCost() / _ioDeps;
        }else{
            obj += _sched->getDepCost();
        }
    }
    return obj;
}


// SA the probablity of accepting new solution
bool MapperSA::metropolis(double diff, double temp){
    if(diff < 0){
        return true;
    }else{
        double val = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        return val < exp(-diff/temp);
    }
}


// annealing funtion
int MapperSA::annealFunc(int temp){
    float k = 0.9;
    return int(k*temp);
}