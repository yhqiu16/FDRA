
#include "mapper/configuration.h"

// get config data for GPE, return<LSB-location, CfgData>
std::map<int, CfgData> Configuration::getGpeCfgData(GPENode* node){
    if(!_mapping->isMapped(node)){
        return {};
    }
    int adgNodeId = node->id();
    ADG* subAdg = node->subADG();
    auto& adgNodeAttr = _mapping->adgNodeAttr(adgNodeId);
    DFGNode* dfgNode = adgNodeAttr.dfgNode;
    // if(dfgNode == nullptr){
    //     return {};
    // }
    std::map<int, CfgData> cfg;
    // operation
    int opc = Operations::OPC(dfgNode->operation());
    int aluId = -1;
    int rduId;
    // bool flag = false;
    std::set<int> usedOperands;
    std::map<int, int> delayUsed;
    for(auto& elem : dfgNode->inputEdges()){
        int eid = elem.second;
        auto& edgeAttr = _mapping->dfgEdgeAttr(eid);
        int inputIdx = edgeAttr.edgeLinks.rbegin()->srcPort; // last edgeLInk, dst port
        auto muxPair = subAdg->input(inputIdx).begin(); // one input only connected to one Mux
        int muxId = muxPair->first;
        int muxCfgData = muxPair->second;
        auto mux = subAdg->node(muxId);
        auto rduPair = mux->output(0).begin();
        rduId = rduPair->first; 
        int rduPort = rduPair->second;
        auto rdu = subAdg->node(rduId);   
        delayUsed[rduPort] = edgeAttr.delay; // delay cycles used by this port
        auto aluPair = rdu->output(rduPort).begin();  // RDU has the same input/output index
        aluId = aluPair->first;
        int aluPort = aluPair->second;
        usedOperands.emplace(aluPort); // operand index
        CfgDataLoc muxCfgLoc = node->configInfo(muxId);
        CfgData muxCfg((muxCfgLoc.high - muxCfgLoc.low + 1), (uint32_t)muxCfgData);
        cfg[muxCfgLoc.low] = muxCfg;    
    }
    if(aluId == -1){ // in case that some node has no input (not completed, should be avoided)
        auto muxPair = subAdg->input(0).begin(); // one input only connected to one Mux
        int muxId = muxPair->first;
        auto mux = subAdg->node(muxId);
        rduId = mux->output(0).begin()->first; 
        auto rdu = subAdg->node(rduId);   
        auto aluPair = rdu->output(0).begin();   
        aluId = aluPair->first;
    }
    CfgDataLoc aluCfgLoc = node->configInfo(aluId);
    CfgData aluCfg(aluCfgLoc.high - aluCfgLoc.low + 1, (uint32_t)opc);
    cfg[aluCfgLoc.low] = aluCfg;
    // RDU
    CfgDataLoc rduCfgLoc = node->configInfo(rduId);
    uint32_t delayCfg = 0;
    int eachDelayWidth = (rduCfgLoc.high - rduCfgLoc.low + 1) / node->numOperands();
    for(auto& elem : delayUsed){
        delayCfg |= elem.second << (eachDelayWidth * elem.first);
    }
    CfgData rduCfg(rduCfgLoc.high - rduCfgLoc.low + 1, delayCfg);
    cfg[rduCfgLoc.low] = rduCfg;  
    // accumulative node
    if(dfgNode->accumulative()){
        int initValId = node->cfgIdMap["InitVal"];
        CfgDataLoc InitValCfgLoc = node->configInfo(initValId);                
        int len = InitValCfgLoc.high - InitValCfgLoc.low + 1;
        CfgData InitValCfg(len);
        uint64_t InitVal = dfgNode->initVal() & (((uint64_t)1 << len) - 1);
        while(len > 0){
            InitValCfg.data.push_back(uint32_t(InitVal&0xffffffff));
            len -= 32;
            InitVal >> 32;
        }
        cfg[InitValCfgLoc.low] = InitValCfg;
        int II = 1;
        int WI = dfgNode->interval() * II;
        int wiId = node->cfgIdMap["WI"];
        CfgDataLoc wiCfgLoc = node->configInfo(wiId);
        int wiCfgLen = wiCfgLoc.high - wiCfgLoc.low + 1;
        CfgData wiCfg(wiCfgLen);
        wiCfg.data.push_back((uint32_t)WI); 
        cfg[wiCfgLoc.low] = wiCfg;
        auto& dfgNodeAttr = _mapping->dfgNodeAttr(dfgNode->id());
        int latency = dfgNodeAttr.lat - dfgNode->opLatency(); 
        int latencyId = node->cfgIdMap["Latency"];
        CfgDataLoc latencyCfgLoc = node->configInfo(latencyId);
        int latencyCfgLen = latencyCfgLoc.high - latencyCfgLoc.low + 1;
        CfgData latencyCfg(latencyCfgLen);
        latencyCfg.data.push_back((uint32_t)latency); 
        cfg[latencyCfgLoc.low] = latencyCfg;
        int cycles = dfgNode->cycles();
        int cyclesId = node->cfgIdMap["Cycles"];
        CfgDataLoc cyclesCfgLoc = node->configInfo(cyclesId);
        int cyclesCfgLen = cyclesCfgLoc.high - cyclesCfgLoc.low + 1;
        CfgData cyclesCfg(cyclesCfgLen);
        cyclesCfg.data.push_back((uint32_t)cycles); 
        cfg[cyclesCfgLoc.low] = cyclesCfg;
        int repeats = dfgNode->repeats();
        int repeatsId = node->cfgIdMap["Repeats"];
        CfgDataLoc repeatsCfgLoc = node->configInfo(repeatsId);
        int repeatsCfgLen = repeatsCfgLoc.high - repeatsCfgLoc.low + 1;
        CfgData repeatsCfg(repeatsCfgLen);
        repeatsCfg.data.push_back((uint32_t)repeats); 
        cfg[repeatsCfgLoc.low] = repeatsCfg;
        bool skipfisrt = !dfgNode->isAccFirst();
        int skipfisrtId = node->cfgIdMap["SkipFirst"];
        CfgDataLoc skipfisrtCfgLoc = node->configInfo(skipfisrtId);
        int skipfisrtCfgLen = skipfisrtCfgLoc.high - skipfisrtCfgLoc.low + 1;
        CfgData skipfisrtCfg(skipfisrtCfgLen);
        skipfisrtCfg.data.push_back((uint32_t)skipfisrt); 
        cfg[skipfisrtCfgLoc.low] = skipfisrtCfg;
    }
    // Constant
    if(dfgNode->hasImm()){
        // find unused operand
        int i = 0;
        for(; i < dfgNode->numInputs(); i++){
            if(!usedOperands.count(i)){ 
                break;
            }
        }
        assert(i < dfgNode->numInputs());
        // auto alu = subAdg->node(aluId); 
        auto rdu = subAdg->node(rduId); // used default delay 
        int muxId = rdu->input(i).first;
        for(auto& elem : subAdg->node(muxId)->inputs()){
            int id = elem.second.first;
            if(id == subAdg->id()) continue;
            if(subAdg->node(id)->type() == "Const"){
                CfgDataLoc muxCfgLoc = node->configInfo(muxId);
                CfgData muxCfg(muxCfgLoc.high - muxCfgLoc.low + 1, (uint32_t)elem.first);
                cfg[muxCfgLoc.low] = muxCfg;
                CfgDataLoc constCfgLoc = node->configInfo(id);                
                int len = constCfgLoc.high - constCfgLoc.low + 1;
                CfgData constCfg(len);
                uint64_t imm = dfgNode->imm() & (((uint64_t)1 << len) - 1);
                while(len > 0){
                    constCfg.data.push_back(uint32_t(imm&0xffffffff));
                    len -= 32;
                    imm >> 32;
                }
                cfg[constCfgLoc.low] = constCfg;
                break;
            }
        }  
    }
    return cfg;
}

// get config data for IOB, return<LSB-location, CfgData>
std::map<int, CfgData> Configuration::getIobCfgData(IOBNode* node){
    if(!_mapping->isMapped(node)){
        return {};
    }
    int adgNodeId = node->id();
    ADG* subAdg = node->subADG();
    auto& adgNodeAttr = _mapping->adgNodeAttr(adgNodeId);
    DFGNode* dfgNode = adgNodeAttr.dfgNode;
    auto& dfgNodeAttr = _mapping->dfgNodeAttr(dfgNode->id());
    DFGIONode* dfgIONode = dynamic_cast<DFGIONode*>(dfgNode);
    // IOBNode *iobNode = dynamic_cast<IOBNode*>(node);
    std::map<int, CfgData> cfg;
    // int ioctrlId = subAdg->output(0).first; // IOB has only one output connected to IOController
    // CfgDataLoc ioctrlCfgLoc = node->configInfo(ioctrlId);
    // int ioctrlCfgLen = ioctrlCfgLoc.high - ioctrlCfgLoc.low + 1;
    // CfgData ioctrlCfg(ioctrlCfgLen);
    // while(ioctrlCfgLen > 0){ // config data of IOController is set by the host
    //     ioctrlCfg.data.push_back((uint32_t)0);
    //     ioctrlCfgLen -= 32;
    // }
    // cfg[ioctrlCfgLoc.low] = ioctrlCfg;
    int isStore = 0;
    bool isUsedAsOB = _mapping->getDFG()->getOutNodes().count(dfgNode->id());
    if(isUsedAsOB){ // IOB used as OB
        isStore = 1;     
    }else{ // IOB used as IB
        isStore = 0;
    }
    int II = 1;
    int latency = dfgNodeAttr.lat - dfgNode->opLatency(); // substract load/store latency
    int dataBytes = _mapping->getADG()->bitWidth() / 8;
    int baseAddrId = node->cfgIdMap["BaseAddr"];
    CfgDataLoc baseAddrCfgLoc = node->configInfo(baseAddrId);
    int baseAddrCfgLen = baseAddrCfgLoc.high - baseAddrCfgLoc.low + 1;
    CfgData baseAddrCfg(baseAddrCfgLen);
    int baseAddr = _dfgIoSpadAddrs[dfgNode->id()];
    int offset = dfgIONode->reducedMemOffset() / dataBytes;
    baseAddrCfg.data.push_back((uint32_t)(baseAddr+offset)); 
    cfg[baseAddrCfgLoc.low] = baseAddrCfg;
    int dfgNestedLevels = dfgIONode->getNestedLevels();
    int iobNestedLevels = _mapping->getADG()->iobAgNestLevels();
    assert(dfgNestedLevels <= iobNestedLevels);
    auto& pattern = dfgIONode->pattern();
    for(int i = 0; i < iobNestedLevels; i++){
        int stride = 0;
        int cycles = 0;
        if(i < dfgNestedLevels){
            stride = pattern[i].first;
            assert(stride % dataBytes == 0);
            stride = stride / dataBytes;
            cycles = pattern[i].second;
        }
        std::string strideName = "Stride" + std::to_string(i);
        int strideId = node->cfgIdMap[strideName];
        CfgDataLoc strideCfgLoc = node->configInfo(strideId);
        int strideCfgLen = strideCfgLoc.high - strideCfgLoc.low + 1;
        CfgData strideCfg(strideCfgLen);
        uint32_t strideAlign = stride & ((1 << strideCfgLen) - 1);
        strideCfg.data.push_back(strideAlign); 
        cfg[strideCfgLoc.low] = strideCfg;
        std::string cyclesName = "Cycles" + std::to_string(i);
        int cyclesId = node->cfgIdMap[cyclesName];
        CfgDataLoc cyclesCfgLoc = node->configInfo(cyclesId);
        int cyclesCfgLen = cyclesCfgLoc.high - cyclesCfgLoc.low + 1;
        CfgData cyclesCfg(cyclesCfgLen);
        cyclesCfg.data.push_back((uint32_t)cycles); 
        cfg[cyclesCfgLoc.low] = cyclesCfg;
    }
    int iiId = node->cfgIdMap["II"];
    CfgDataLoc iiCfgLoc = node->configInfo(iiId);
    int iiCfgLen = iiCfgLoc.high - iiCfgLoc.low + 1;
    CfgData iiCfg(iiCfgLen);
    iiCfg.data.push_back((uint32_t)II); 
    cfg[iiCfgLoc.low] = iiCfg;
    int latencyId = node->cfgIdMap["Latency"];
    CfgDataLoc latencyCfgLoc = node->configInfo(latencyId);
    int latencyCfgLen = latencyCfgLoc.high - latencyCfgLoc.low + 1;
    CfgData latencyCfg(latencyCfgLen);
    latencyCfg.data.push_back((uint32_t)latency); 
    cfg[latencyCfgLoc.low] = latencyCfg;
    int isStoreId = node->cfgIdMap["IsStore"];
    CfgDataLoc isStoreCfgLoc = node->configInfo(isStoreId);
    int isStoreCfgLen = isStoreCfgLoc.high - isStoreCfgLoc.low + 1;
    CfgData isStoreCfg(isStoreCfgLen);
    isStoreCfg.data.push_back((uint32_t)isStore); 
    cfg[isStoreCfgLoc.low] = isStoreCfg;
    if(node->cfgIdMap.count("UseAddr")){
        int useAddr = dfgNode->operation() == "LOAD" || dfgNode->operation() == "STORE";
        int useAddrId = node->cfgIdMap["UseAddr"];
        CfgDataLoc useAddrCfgLoc = node->configInfo(useAddrId);
        int useAddrCfgLen = useAddrCfgLoc.high - useAddrCfgLoc.low + 1;
        CfgData useAddrCfg(useAddrCfgLen);
        useAddrCfg.data.push_back((uint32_t)useAddr); 
        cfg[useAddrCfgLoc.low] = useAddrCfg;
    }
    if(dfgNode->operation() != "INPUT"){ // only INPUT node donot use Mux     
        int rduId;
        std::map<int, int> delayUsed;
        for(auto& elem : dfgNode->inputEdges()){
            int eid = elem.second;
            auto& edgeAttr = _mapping->dfgEdgeAttr(eid);
            int delay = edgeAttr.delay; // delay cycles
            int inputIdx = edgeAttr.edgeLinks.rbegin()->srcPort; // last edgeLInk, dst port
            auto muxPair = subAdg->input(inputIdx).begin(); // one input only connected to one Mux
            int muxId = muxPair->first;
            int muxCfgData = muxPair->second;
            auto mux = subAdg->node(muxId);
            CfgDataLoc muxCfgLoc = node->configInfo(muxId);
            CfgData muxCfg((muxCfgLoc.high - muxCfgLoc.low + 1), (uint32_t)muxCfgData);
            cfg[muxCfgLoc.low] = muxCfg;
            auto rduPair = mux->output(0).begin();
            rduId = rduPair->first; 
            int rduPort = rduPair->second;
            auto rdu = subAdg->node(rduId);            
            if(rdu->type() == "RDU"){
                delayUsed[rduPort] = edgeAttr.delay; // delay cycles used by this port
                CfgDataLoc rduCfgLoc = node->configInfo(rduId);
                CfgData rduCfg(rduCfgLoc.high - rduCfgLoc.low + 1, (uint32_t)delay);
                cfg[rduCfgLoc.low] = rduCfg;      
            }     
        }  
        // RDU
        if(!delayUsed.empty()){
            CfgDataLoc rduCfgLoc = node->configInfo(rduId);
            uint32_t delayCfg = 0;
            int eachDelayWidth = (rduCfgLoc.high - rduCfgLoc.low + 1) / node->numOperands();
            for(auto& elem : delayUsed){
                delayCfg |= elem.second << (eachDelayWidth * elem.first);
            }
            CfgData rduCfg(rduCfgLoc.high - rduCfgLoc.low + 1, delayCfg);
            cfg[rduCfgLoc.low] = rduCfg;  
        }
    }
    return cfg;
}


// get config data for GIB, return<LSB-location, CfgData>
std::map<int, CfgData> Configuration::getGibCfgData(GIBNode* node){
    int adgNodeId = node->id();
    ADG* subAdg = node->subADG();
    auto& adgNodeAttr = _mapping->adgNodeAttr(adgNodeId);
    auto& passEdges = adgNodeAttr.dfgEdgePass;
    if(passEdges.empty()){
        return {};
    }
    std::map<int, CfgData> cfg;
    for(auto& elem : passEdges){
        int muxId = subAdg->output(elem.dstPort).first; // one output connected to one mux
        if(muxId == subAdg->id()){ // actually connected to input port
            continue;
        }
        auto mux = subAdg->node(muxId);        
        // find srcPort
        for(auto in : mux->inputs()){
            if(in.second.second == elem.srcPort){ 
                CfgDataLoc muxCfgLoc = node->configInfo(muxId);
                CfgData muxCfg(muxCfgLoc.high - muxCfgLoc.low + 1, (uint32_t)in.first);
                cfg[muxCfgLoc.low] = muxCfg;
                break;
            }
        }
    }
    return cfg;
}




// get config data for ADG node
void Configuration::getNodeCfgData(ADGNode* node, std::vector<CfgDataPacket>& cfg){
    std::map<int, CfgData> cfgMap;
    if(node->type() == "GPE"){
        cfgMap = getGpeCfgData(dynamic_cast<GPENode*>(node));
    }else if(node->type() == "GIB"){
        cfgMap = getGibCfgData(dynamic_cast<GIBNode*>(node));
     }else if(node->type() == "IOB"){
        cfgMap = getIobCfgData(dynamic_cast<IOBNode*>(node));
    }
    if(cfgMap.empty()){
        return;
    }
    ADG* adg = _mapping->getADG();
    int cfgDataWidth = adg->cfgDataWidth();
    int totalLen = cfgMap.rbegin()->first + cfgMap.rbegin()->second.len;
    int num = (totalLen+31)/32;
    std::vector<uint32_t> cfgDataVec(num, 0);
    std::set<uint32_t> addrs;
    for(auto& elem : cfgMap){ // std::map auto-sort keys
        int lsb = elem.first;
        int len = elem.second.len;
        auto& data = elem.second.data;
        // cache valid address
        uint32_t targetAddr = lsb/cfgDataWidth;
        int addrNum = (len + (lsb%cfgDataWidth) + cfgDataWidth - 1)/cfgDataWidth;
        for(int i = 0; i < addrNum; i++){
            addrs.emplace(targetAddr+i);
        } 
        // cache data from 0 to MSB   
        int targetIdx = lsb/32;
        int offset = lsb%32;
        uint64_t tmpData = data[0];
        int dataIdx = 0;
        int dataLenLeft = 32;
        while(len > 0){
            if(len <= 32 - offset){
                len = 0;
                cfgDataVec[targetIdx] |= (tmpData << offset);
            }else{                          
                dataLenLeft -= 32 - offset; 
                cfgDataVec[targetIdx] |= (tmpData << offset);                
                targetIdx++;
                dataIdx++;
                tmpData >>= 32 - offset;
                if(dataIdx < data.size()){
                    tmpData |= data[dataIdx] << dataLenLeft;
                    dataLenLeft += 32;
                }
                len -= 32 - offset;
                offset = 0;
            }
        }
    }
    // construct CfgDataPacket
    int cfgBlkOffset = adg->cfgBlkOffset();
    int cfgBlkIdx = node->cfgBlkIdx();
    // int x = node->x();
    uint32_t highAddr = uint32_t(cfgBlkIdx << cfgBlkOffset);
    int n;
    int mask;
    if(cfgDataWidth >= 32){
        assert(cfgDataWidth%32 == 0);
        n = cfgDataWidth/32;
    }else{
        assert(32%cfgDataWidth == 0);
        n = 32/cfgDataWidth;
        mask = (1 << cfgDataWidth) - 1;
    }
    for(auto addr : addrs){
        CfgDataPacket cdp(highAddr|addr);
        if(cfgDataWidth >= 32){
            int size = cfgDataVec.size();
            for(int i = 0; i < n; i++){
                int idx = addr*n+i;
                uint32_t data = (idx < size)? cfgDataVec[idx] : 0;
                cdp.data.push_back(data);
            }
        }else{
            uint32_t data = (cfgDataVec[addr/n] >> ((addr%n)*cfgDataWidth)) & mask;
            cdp.data.push_back(data);
        }
        cfg.push_back(cdp);
    }
}


// get config data for ADG
void Configuration::getCfgData(std::vector<CfgDataPacket>& cfg){
    cfg.clear();
    for(auto& elem : _mapping->getADG()->nodes()){
        getNodeCfgData(elem.second, cfg);
    }
}


// dump config data
void Configuration::dumpCfgData(std::ostream& os){
    std::vector<CfgDataPacket> cfg;
    getCfgData(cfg);
    ADG* adg = _mapping->getADG();
    int cfgAddrWidth = adg->cfgAddrWidth();
    int cfgDataWidth = adg->cfgDataWidth();
    int addrWidthHex = (cfgAddrWidth+3)/4;
    int dataWidthHex = std::min(cfgDataWidth/4, 8);
    os << std::hex;
    for(auto& cdp : cfg){
        os << std::setw(addrWidthHex) << std::setfill('0') << (cdp.addr) << " ";
        for(int i = cdp.data.size() - 1; i >= 0; i--){
            os << std::setw(dataWidthHex) << std::setfill('0') << cdp.data[i];
        }
        os << std::endl;
    }
    os << std::dec;
}
