
#include "mapper/mapper.h"


Mapper::Mapper(ADG* adg): _adg(adg) {
    initializeAdg();
    _sched = new IOScheduler(adg);
}

Mapper::Mapper(ADG* adg, DFG* dfg): _adg(adg), _dfg(dfg) {
    initializeAdg();
    initializeDfg();
    _mapping = new Mapping(adg, dfg);
    // initializeCandidates();
    _isDfgModified = false;
    sortDfgNodeInPlaceOrder();
    _sched = new IOScheduler(adg);
}

Mapper::~Mapper(){
    if(_mapping != nullptr){
        delete _mapping;
    }
    if(_dfgModified != nullptr){
        delete _dfgModified;
    }
    delete _sched;
}

// set DFG and initialize DFG
// modify: if the DFG is a modified one
void Mapper::setDFG(DFG* dfg, bool modify){ 
    _dfg = dfg; 
    initializeDfg();
    if(_mapping != nullptr){
        delete _mapping;
    }
    _mapping = new Mapping(_adg, dfg);
    // initializeCandidates();
    if(modify){
        setDfgModified(dfg);
    } else{
        _isDfgModified = false;
    }
    sortDfgNodeInPlaceOrder();
}


// set modified DFG and delete the old one
void Mapper::setDfgModified(DFG* dfg){
    if(_dfgModified != nullptr){
        delete _dfgModified;
    }
    _dfgModified = dfg;
    _isDfgModified = true;
}

// // set ADG and initialize ADG
// void Mapper::setADG(ADG* adg){ 
//     _adg = adg; 
//     initializeAdg();
// }


// initialize mapping status of ADG
void Mapper::initializeAdg(){
    // std::cout << "Initialize ADG\n";
    calAdgNodeDist();
    calAdgOpCnt();
}


// initialize mapping status of DFG
void Mapper::initializeDfg(){
    // topoSortDfgNodes();
    _dfg->topoSortNodes();
}


// // initialize candidates of DFG nodes
// void Mapper::initializeCandidates(){
//     Candidate cdt(_mapping, 50);
//     cdt.findCandidates();
//     // candidates = cdt.candidates(); // RETURN &
//     candidatesCnt = cdt.cnt();
// }


// // sort dfg nodes in reversed topological order
// // depth-first search
// void Mapper::dfs(DFGNode* node, std::map<int, bool>& visited){
//     int nodeId = node->id();
//     if(visited.count(nodeId) && visited[nodeId]){
//         return; // already visited
//     }
//     visited[nodeId] = true;
//     for(auto& in : node->inputs()){
//         int inNodeId = in.second.first;
//         if(inNodeId == _dfg->id()){ // node connected to DFG input port
//             continue;
//         }
//         dfs(_dfg->node(inNodeId), visited); // visit input node
//     }
//     dfgNodeTopo.push_back(_dfg->node(nodeId));
// }

// // sort dfg nodes in reversed topological order
// void Mapper::topoSortDfgNodes(){
//     std::map<int, bool> visited; // node visited status
//     for(auto& in : _dfg->outputs()){
//         int inNodeId = in.second.first;
//         if(inNodeId == _dfg->id()){ // node connected to DFG input port
//             continue;
//         }
//         dfs(_dfg->node(inNodeId), visited); // visit input node
//     }
// }


// calculate the shortest path among ADG nodes
void Mapper::calAdgNodeDist(){
    // map ADG node id to continuous index starting from 0
    std::map<int, int> _adgNodeId2Idx;
    // distances among ADG nodes
    std::vector<std::vector<int>> _adgNodeDist; // [node-idx][node-idx]
    int i = 0;
    // if the ADG node with the index is GIB
    std::map<int, bool> adgNodeIdx2GIB;
    for(auto& node : _adg->nodes()){
        adgNodeIdx2GIB[i] = (node.second->type() == "GIB");
        _adgNodeId2Idx[node.first] = i++;
    }
    int n = i; // total node number
    int inf = 0x7fffffff;
    _adgNodeDist.assign(n, std::vector<int>(n, inf));
    for(auto& node : _adg->nodes()){
        int idx = _adgNodeId2Idx[node.first];
        _adgNodeDist[idx][idx] = 0;
        for(auto& src : node.second->inputs()){
            int srcId = src.second.first;
            // if(srcId == _adg->id()){
            //     continue; // connected to ADG input port
            // }
            int srcIdx = _adgNodeId2Idx[srcId];
            if(_adgNodeDist[srcIdx][idx] == inf){
                int srcPort = src.second.second;
                ADGNode* srcNode = _adg->node(srcId);
                int dist = 1;
                if(srcNode->type() == "GIB" && node.second->type() == "GIB"){
                    if(dynamic_cast<GIBNode*>(srcNode)->outReged(srcPort)){ // output port reged
                        dist = 2;
                    }
                }
                _adgNodeDist[srcIdx][idx] = dist;
            }
        }
    }
    // Floyd algorithm
    for (int k = 0; k < n; ++k) {
        if(adgNodeIdx2GIB[k]){
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (_adgNodeDist[i][k] < inf && _adgNodeDist[k][j] < inf &&
                        _adgNodeDist[i][j] > _adgNodeDist[i][k] + _adgNodeDist[k][j]) {
                        _adgNodeDist[i][j] = _adgNodeDist[i][k] + _adgNodeDist[k][j];
                    }
                }
            }
        }        
    }

    // shortest distance between two ADG nodes (GPE/IOB nodes)
    for(auto& inode : _adg->nodes()){
        if(inode.second->type() == "GIB"){
            continue;
        }
        int i = _adgNodeId2Idx[inode.first];
        for(auto& jnode : _adg->nodes()){
            if(jnode.second->type() == "GIB" || (inode.second->type() == "IOB" && jnode.second->type() == "IOB")){
                continue;
            }
            int j = _adgNodeId2Idx[jnode.first];
            _adgNode2NodeDist[std::make_pair(inode.first, jnode.first)] = _adgNodeDist[i][j];
            // std::cout << inode.first << "," << jnode.first << ": " << _adgNodeDist[i][j] << "  ";
        }
        // std::cout << std::endl;
    }

    // // shortest distance between ADG node (GPE node) and the ADG IO
    // for(auto& inode : _adg->nodes()){
    //     if(inode.second->type() != "GPE"){
    //         continue;
    //     }
    //     int i = _adgNodeId2Idx[inode.first];
    //     int minDist2IB = inf;
    //     int minDist2OB = inf;
    //     for(auto& jnode : _adg->nodes()){            
    //         auto jtype = jnode.second->type();
    //         int j = _adgNodeId2Idx[jnode.first];
    //         if(jtype == "IB"){                
    //             minDist2IB = std::min(minDist2IB, _adgNodeDist[j][i]);
    //         }else if(jtype == "OB"){
    //             minDist2OB = std::min(minDist2OB, _adgNodeDist[i][j]);
    //         }                       
    //     }
    //     _adgNode2IODist[inode.first] = std::make_pair(minDist2IB, minDist2OB);
    //     // std::cout << inode.first << ": " << minDist2IB << "," << minDist2OB << std::endl;
    // }
}


// get the shortest distance between two ADG nodes
int Mapper::getAdgNodeDist(int srcId, int dstId){
    // return _adgNodeDist[_adgNodeId2Idx[srcId]][_adgNodeId2Idx[dstId]];
    return _adgNode2NodeDist[std::make_pair(srcId, dstId)];
}

// // get the shortest distance between ADG node and ADG input
// int Mapper::getAdgNode2InputDist(int id){
//     return _adgNode2IODist[id].first;
// }

// // get the shortest distance between ADG node and ADG input
// int Mapper::getAdgNode2OutputDist(int id){
//     return _adgNode2IODist[id].second;
// }


// calculate supported operation count of ADG
void Mapper::calAdgOpCnt(){
    for(auto& elem : _adg->nodes()){       
        if(elem.second->type() != "GIB"){
            auto node = dynamic_cast<FUNode*>(elem.second);
            for(auto& op : node->operations()){
                if(adgOpCnt.count(op)){
                    adgOpCnt[op] += 1;
                } else {
                    adgOpCnt[op] = 1;
                }                 
            }
        }
    }
}


// calculate the number of the candidates for one DFG node
int Mapper::calCandidatesCnt(DFGNode* dfgNode, int maxCandidates){
    int candidatesCnt = 0;
    for(auto& elem : _adg->nodes()){
        auto adgNode = elem.second;
        //select GPE node
        if(adgNode->type() != "GPE"){  
            continue;
        }
        GPENode* gpeNode = dynamic_cast<GPENode*>(adgNode);
        // check if the DFG node operationis supported
        if(gpeNode->opCapable(dfgNode->operation())){
            candidatesCnt++;
        }
    }
    return std::min(candidatesCnt, maxCandidates);
}

// sort the DFG node IDs in placing order
void Mapper::sortDfgNodeInPlaceOrder(){
    std::map<int, int> candidatesCnt; // <dfgnode-id, count>
    dfgNodeIdPlaceOrder.clear();
    // topological order
    for(auto nodeId : _dfg->topoNodes()){ 
        DFGNode *node = _dfg->node(nodeId);
        dfgNodeIdPlaceOrder.push_back(nodeId);
        // std::cout << node->id() << ", ";
        int cnt = calCandidatesCnt(node, 50);
        candidatesCnt[node->id()] = cnt;
    }
    // std::cout << std::endl;
    // sort DFG nodes according to their candidate numbers
    // std::random_shuffle(dfgNodeIds.begin(), dfgNodeIds.end()); // randomly sort will cause long routing paths
    std::stable_sort(dfgNodeIdPlaceOrder.begin(), dfgNodeIdPlaceOrder.end(), [&](int a, int b){
        return candidatesCnt[a] <  candidatesCnt[b];
    });
}


// ===== timestamp functions >>>>>>>>>
void Mapper::setStartTime(){
    _start = std::chrono::steady_clock::now();
}


void Mapper::setTimeOut(double timeout){
    _timeout = timeout;
}


//get the running time in millisecond
double Mapper::runningTimeMS(){
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-_start).count();
}



// ==== map functions below >>>>>>>>
// check if the DFG can be mapped to the ADG according to the resources
bool Mapper::preMapCheck(ADG* adg, DFG* dfg){
    // first, check the I/O node number
    if(adg->numIobNodes() < dfg->ioNodes().size()){ 
        std::cout << "This DFG has too many I/O nodes!\n";
        return false;
    }
    // second, check the computing node number
    if(adg->numGpeNodes() < (dfg->nodes().size() - dfg->ioNodes().size())){
        std::cout << "This DFG has too many computing nodes!\n";
        return false;
    }
    // third, check if there are enough ADG nodes that can map the DFG nodes
    // supported operation count of ADG
    // std::map<std::string, int> adgOpCnt; 
    // for(auto& elem : adg->nodes()){       
    //     if(elem.second->type() == "GPE"){
    //         auto node = dynamic_cast<GPENode*>(elem.second);
    //         for(auto& op : node->operations()){
    //             if(adgOpCnt.count(op)){
    //                 adgOpCnt[op] += 1;
    //             } else {
    //                 adgOpCnt[op] = 1;
    //             }                 
    //         }
    //     }
    // }
    // operation count of DFG
    std::map<std::string, int> dfgOpCnt; 
    for(auto& elem : dfg->nodes()){
        // if(dfg->isIONode(elem.first)){
        //     continue;
        // }
        auto op = elem.second->operation();
        assert(!op.empty());
        if(dfgOpCnt.count(op)){
            dfgOpCnt[op] += 1;
        } else {
            dfgOpCnt[op] = 1;
        }
    }
    for(auto& elem : dfgOpCnt){
        if(adgOpCnt[elem.first] < elem.second){ 
            std::cout << "No enough ADG nodes to support " << elem.first << std::endl;
            return false; // there should be enough ADG nodes that support this operation
        }
    }
    return true;
}

// // map the DFG to the ADG
// bool Mapper::mapping(){

// }


// mapper with running time
bool Mapper::mapperTimed(){
    setStartTime();
    // check if the DFG can be mapped to the ADG according to the resources
    if(!preMapCheck(getADG(), getDFG())){
        return false;
    }
    std::cout << "Pre-map checking passed!\n";
    bool succeed = mapper();
    std::cout << "Running time(s): " << runningTimeMS()/1000 << std::endl;
    return succeed;
}


// execute mapping, timing sceduling, visualizing, config getting
// dumpCallFunc : dump CGRA execution calll function
// dumpMappedViz : dump mapped visual graph
// resultDir: mapped result directory
bool Mapper::execute(bool dumpCallFunc, bool dumpMappedViz, std::string resultDir){
    std::cout << "Start mapping >>>>>>\n";
    bool res = mapperTimed();
    if(res){
        std::string dir;
        if(!resultDir.empty()){
            dir = resultDir;
        }else{
            dir = "results"; // default directory
        }
        if(dumpCallFunc){
            std::ofstream func_ofs(dir + "/cgra_execute.c");
            std::ofstream call_ofs(dir + "/cgra_call.txt");
            _sched->execute(_mapping, func_ofs, call_ofs);
            func_ofs.close();
            call_ofs.close();
            // std::ofstream name_ofs(dir + "/array_names.txt");
            // for(auto &name : arrayNames){
            //     name_ofs << name << std::endl;
            // }
            // name_ofs.close();
        } 
        if(dumpMappedViz){
            Graphviz viz(_mapping, dir);
            viz.drawDFG();
            viz.drawADG();
            viz.dumpDFGIO(); 
            // viz.printDFGEdgePath();
        }
        // if(dumpConfig){
        //     Configuration cfg(_mapping);
        //     std::ofstream ofs(dir + "/config.bit");
        //     cfg.dumpCfgData(ofs);
        // }       
        std::cout << "Succeed to map DFG to ADG!<<<<<<\n";
    } else{
        std::cout << "Fail to map DFG to ADG!<<<<<<\n";
    }
    return res;
}


// generate SoC application codes
// replace loop kernels with cgra call function
void Mapper::genSoCApp(std::string dir, std::string appName)
{
    std::string ori_app_name = dir + "/" + appName + ".c";
    std::string func_name = dir + "/cgra_execute.c";
    std::string call_name = dir + "/cgra_call.txt";
    std::string soc_app_name = dir + "/" + appName + "_soc.c";
    //Find the CGRA CDFG Loop Position
    std::ifstream ifs;
    ifs.open(ori_app_name);
    if(!ifs.is_open()){
        std::cout << "Cannot open application file: " << ori_app_name << std::endl;
        return;
    }
    

    std::map<int, int> loopState;
    std::map<int, std::ios::pos_type> loopStart;
    std::map<int, std::ios::pos_type> loopEnd;
    std::vector<int> loopStack;
    std::vector<int> cdfgLoops;
    std::vector<std::pair<std::ios::pos_type, std::ios::pos_type>> cdfgPosition;
    std::string s;
    int numLoop = 0;
    int currentLoop = 0;
    int currentBaseLoop = 0;
    int currentLevel = 0;
    std::ios::pos_type includeEnd = 0;

    while(getline(ifs, s))
    {
        if(s.size() > 7 && s.substr(0,8) == "#include")
        {
            includeEnd = ifs.tellg();
        }
    }
    ifs.close();

    ifs.open(ori_app_name);
    while(ifs >> s)
    {
        if((s.size() > 3 && s.substr(0, 4) == "for(") || (s == "for"))
        {
            currentLoop = numLoop;
            loopState[currentLoop] = 1;
            loopStack.emplace_back(currentLoop);
            loopStart[currentLoop] = (size_t)ifs.tellg() - s.size();

            if(currentLevel == 0)   { currentBaseLoop = currentLoop; }
            currentLevel++;

            numLoop++;
        }
        if(loopState[currentLoop] == 1 && s.back() == '{')
        {
            loopState[currentLoop] = 2;
        }
        if(currentLevel > 0 && s.back() == '}')
        {
            while(loopState[currentLoop] == 1)
            {
                loopEnd[currentLoop] = ifs.tellg();
                loopState[currentLoop] = 0;
                loopStack.pop_back();
                currentLoop = loopStack.back();
                currentLevel--;
            }
            if(currentLevel > 0)
            {
                loopEnd[currentLoop] = ifs.tellg();
                loopState[currentLoop] = 0;
                loopStack.pop_back();
                currentLoop = loopStack.back();
                currentLevel--;
            }
        }
        if(s == "please_map_me();")
        {
            cdfgLoops.emplace_back(currentBaseLoop);
        }
    }
    for(int i : cdfgLoops)
    {
        cdfgPosition.emplace_back(std::make_pair(loopStart[i], loopEnd[i]));
    }
    ifs.close();

    std::ofstream ofs_soc(soc_app_name);    
    ifs.open(ori_app_name);
    int isCDFGPos = 0;
    while(getline(ifs, s))
    {
        if(ifs.tellg() == includeEnd)
        {
            ofs_soc << s + "\n";
            // add CGRA execution function 
            ofs_soc << "#include \"include/rocc.h\"\n";
            ofs_soc << "#include \"include/ISA.h\"\n";
            std::ifstream ifs_func(func_name);
            ofs_soc << ifs_func.rdbuf();
            ifs_func.close();
        }
        else
        {
            isCDFGPos = 0;
            for(auto i : cdfgPosition)
            {
                if(i.first <= ifs.tellg() && i.first >= (size_t)ifs.tellg() - s.size())
                {
                    ifs.seekg(i.second);
                    // add CGRA execution call codes
                    std::ifstream ifs_call(call_name);
                    ofs_soc << ifs_call.rdbuf();
                    ifs_call.close();
                    isCDFGPos = 1;
                    break;
                }
            }
            if(isCDFGPos == 0)  { ofs_soc << s + "\n"; }
        }
    }
}



// get max latency of the mapped DFG
int Mapper::getMaxLat()
{
    return _mapping->maxLat();
}