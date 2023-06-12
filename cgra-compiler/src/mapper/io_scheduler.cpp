#include "mapper/io_scheduler.h"

IOScheduler::IOScheduler(ADG *adg)
{
    _adg = adg;
    int banks = adg->numIobNodes();
    _cur_bank_status.assign(banks, {0, 0, 0, 0});
    _old_bank_status.assign(banks, {0, 0, 0, 0});
    _older_bank_status.assign(banks, {0, 0, 0, 0});
    _old_cfg_status = {0, 0};
}

IOScheduler::~IOScheduler()
{
}

void IOScheduler::ioSchedule(Mapping *mapping)
{
    // std::map<int, dfgIoInfo> res;
    _dfg_io_infos.clear();
    _ex_dep = 0;
    _iob_ens = 0;
    _dep_cost = 0;
    int bankNum = _adg->numIobNodes();
    _cur_bank_status.assign(bankNum, {0, 0, 0, 0});
    DFG* dfg = mapping->getDFG();
    auto outNodeIds = dfg->getOutNodes(); // OUTPUT/STORE nodes
    // LD_DEP_ST_LAST_SEC_TASK cost = 1
    long inNodeNum = dfg->ioNodes().size() - outNodeIds.size(); // LD_DEP_EX_LAST_TASK cost
    long inNodeNum_2 = inNodeNum * inNodeNum;   // EX_DEP_ST_LAST_TASK dep cost
    long inNodeNum_3 = inNodeNum_2 * inNodeNum; // LD_DEP_ST_LAST_TASK dep cost
    int sizeofBank = _adg->iobSpadBankSize();
    int dataByte = _adg->bitWidth() / 8;
    int depthofBank = sizeofBank / dataByte; // accomodate total data number
    std::vector<int> sortedIoNodes;
    sortedIoNodes.assign(dfg->ioNodes().begin(), dfg->ioNodes().end());
    std::map<std::string, int> memNum;
    for(auto &elem : sortedIoNodes){
        auto name = dynamic_cast<DFGIONode*>(dfg->node(elem))->memRefName();
        if(!memNum.count(name)){
            memNum[name] = 1;
        }else{
            memNum[name] += 1;
        }
    }
    // sort by memRefName and memSize
    // std::sort(sortedIoNodes.begin(), sortedIoNodes.end(), [&](int ida, int idb){
    //     return dynamic_cast<DFGIONode*>(dfg->node(ida))->memRefName() < dynamic_cast<DFGIONode*>(dfg->node(idb))->memRefName();
    // });
    std::sort(sortedIoNodes.begin(), sortedIoNodes.end(), [&](int ida, int idb){
        auto nodea = dynamic_cast<DFGIONode*>(dfg->node(ida));
        auto nodeb = dynamic_cast<DFGIONode*>(dfg->node(idb));
        if(nodea->memSize() > nodeb->memSize()){
            return true;
        }else if(nodea->memSize() == nodeb->memSize()){
            auto namea = nodea->memRefName();
            auto nameb = nodeb->memRefName();
            if(memNum[namea] < memNum[nameb]){
                return true;
            }else if(memNum[namea] == memNum[nameb]){
                return namea < nameb;
            }
        }
        return false;
    });
    // sort by io type, input nodes rank before output nodes
    std::stable_sort(sortedIoNodes.begin(), sortedIoNodes.end(), [&](int ida, int idb){
        return outNodeIds.count(ida) < outNodeIds.count(idb);
    });
    std::map<std::string, int> memDep;
    for(auto id : sortedIoNodes){
        // std::cout << dfg->node(id)->name() << ": " << dynamic_cast<DFGIONode*>(dfg->node(id))->memRefName() << std::endl;
        dfgIoInfo ioInfo;
        // auto dfgIONode = dfg->node(id);
        bool isStore = false;
        if(outNodeIds.count(id)){ 
            isStore = true;
        }
        ioInfo.isStore = isStore;
        DFGIONode* ionode = dynamic_cast<DFGIONode*>(dfg->node(id));
        int memSize = ionode->memSize();
        std::string memName = ionode->memRefName();
        int spadDataByte = _adg->cfgSpadDataWidth() / 8; // dual ports of cfg-spad have the same width 
        memSize = (memSize + spadDataByte - 1) / spadDataByte * spadDataByte; // align to spadDataByte
        auto& attr =  mapping->dfgNodeAttr(id);
        int iobId = attr.adgNode->id();
        int iobIdx = dynamic_cast<IOBNode*>(_adg->node(iobId))->index();
        _iob_ens |= 1 << iobIdx;
        std::vector<int> banks = _adg->iobToSpadBanks(iobIdx); // spad banks connected to this IOB
        int minBank = *(std::min_element(banks.begin(), banks.end()));
        std::vector<int> availBanks;
        for(int bank : banks){ // two IOs of the same DFG cannot access the same bank
            if(_cur_bank_status[bank].used == 0){
                availBanks.push_back(bank);
            }
        }
        assert(!availBanks.empty());
        bool allocated = false;
        int selBank;
        int selStart = 0;
        std::vector<std::pair<int, int>> bankStatus; // <status, start-addr>
        // 0: both available; 1: old available, older not; 2: older available, old not; 3: both not
        for(int bank : availBanks){            
            int oldUsed = _old_bank_status[bank].used;
            int olderUsed = _older_bank_status[bank].used;
            int oldStart = _old_bank_status[bank].start;
            int olderStart = _older_bank_status[bank].start;
            int oldEnd = _old_bank_status[bank].end;
            int olderEnd = _older_bank_status[bank].end;
            int oldStatus = 0; // 0: free; 1: used but available; 2: not available
            int oldSelStart = 0;
            if(oldUsed == 0){               
                oldStatus = 0;
                oldSelStart = 0;
            }else if((oldUsed == 1 && !isStore) || (oldUsed == 2 && isStore)){ // the same operation
                if(memSize <= sizeofBank - oldEnd){
                    oldStatus = 1;
                    oldSelStart = oldEnd;
                }else if(memSize <= oldStart){
                    oldStatus = 1;
                    oldSelStart = 0;
                }else{
                    oldStatus = 2;
                }
            }else if(oldUsed == 1 && isStore){ // old load, current store
                oldStatus = 0;
                oldSelStart = 0;
            }else{ // old store, current load; cannot access the same bank simultaneously
                oldStatus = 2;
            }

            int olderStatus = 0; // 0: free; 1: used but available; 2: not available
            int olderSelStart = 0;
            if(olderUsed == 2 && !isStore){ // the same operation
                olderStatus = 2;                
            }else{
                olderStatus = 0;
                olderSelStart = 0;
            }

            int status = 0; // 0: both available; 1: old available, older not; 2: older available, old not; 3: both not            
            if(oldStatus < 2 && olderStatus == 0){
                status = 0;
                selStart = oldSelStart;              
            }else if(oldStatus < 2 && olderStatus == 2){
                status = 1;
                selStart = oldSelStart;
            }else if(oldStatus == 2 && olderStatus == 0){
                status = 2;
                selStart = olderSelStart;
            }else{ // (oldStatus == 2 && olderStatus == 2)
                status = 3;
                selStart = 0;
            }            
            if(status == 0){
                selBank = bank;
                ioInfo.dep = 0;
                allocated = true;
                break;
            }
            bankStatus.push_back(std::make_pair(status, selStart));
        }            
        
        int availBankNum = bankStatus.size();
        if(!allocated){
            for(int i = 0; i < availBankNum; i++){
                if(bankStatus[i].first == 1){
                    selBank = availBanks[i];
                    selStart = bankStatus[i].second;
                    assert((!isStore) && _older_bank_status[selBank].used == 2);
                    ioInfo.dep = LD_DEP_ST_LAST_SEC_TASK;
                    if(!memDep.count(memName) || memDep[memName] < 1){ // IO nodes access to the same array have only one cost
                        memDep[memName] = 1;
                        _dep_cost += 1; 
                    }
                    // _dep_cost += 1;               
                    allocated = true;
                    break;
                }
            }
        }
        if(!allocated && isStore){
            selBank = availBanks[0];
            selStart = bankStatus[0].second;
            ioInfo.dep = EX_DEP_ST_LAST_TASK;
            _ex_dep = EX_DEP_ST_LAST_TASK;
            allocated = true;
        }else if(!allocated){
            for(int i = 0; i < availBankNum; i++){
                selBank = availBanks[i];
                if(_old_bank_status[selBank].used == 1){                    
                    selStart = bankStatus[i].second;
                    ioInfo.dep = LD_DEP_EX_LAST_TASK;
                    if(!memDep.count(memName) || memDep[memName] < inNodeNum){ // IO nodes access to the same array have only one cost
                        memDep[memName] = inNodeNum;
                        _dep_cost += inNodeNum; 
                    }
                    // _dep_cost += inNodeNum;          
                    allocated = true;
                    break;
                }
            }
            if(!allocated){
                selBank = availBanks[0];
                selStart = bankStatus[0].second;
                ioInfo.dep = LD_DEP_ST_LAST_TASK;
                if(!memDep.count(memName) || memDep[memName] < inNodeNum_3){ // IO nodes access to the same array have only one cost
                    memDep[memName] = inNodeNum_3;
                    _dep_cost += inNodeNum_3; 
                }
                // _dep_cost += inNodeNum_3;
                allocated = true;
            }
        }
        assert(allocated); 
        ioInfo.addr = selBank * sizeofBank + selStart;
        ioInfo.iobAddr = ((selBank - minBank) * sizeofBank + selStart) / dataByte;        
        _dfg_io_infos[id] = ioInfo;        
        // std::cout << id << ": " << ioInfo.addr << std::endl;
        _cur_bank_status[selBank].used = isStore ? 2 : 1;
        _cur_bank_status[selBank].iob = iobIdx;
        _cur_bank_status[selBank].start = selStart;
        _cur_bank_status[selBank].end = selStart + memSize;        
    }
    if(_ex_dep > 0){
        _dep_cost += inNodeNum_2;
    }
    // return res;
}


// get the config data and dependence
void IOScheduler::genCfgData(Mapping *mapping, std::ostream &os)
{
    Configuration cfg(mapping);
    for(auto &elem : _dfg_io_infos){
        cfg.setDfgIoSpadAddr(elem.first, elem.second.iobAddr);
    }
    std::vector<CfgDataPacket> cfgData;
    cfg.getCfgData(cfgData);
    int cfgSpadDataByte = _adg->cfgSpadDataWidth() / 8;
    int cfgAddrWidth = _adg->cfgAddrWidth();
    int cfgDataWidth = _adg->cfgDataWidth();
    int alignWidth = (cfgAddrWidth > 16) ? 32 : 16;
    assert(alignWidth >= cfgAddrWidth && cfgDataWidth >= alignWidth);
    int cfgNum = 0;
    for(auto& cdp : cfgData){
        cfgNum += cdp.data.size() * 32 / cfgDataWidth;
    }

    if(cfgAddrWidth > 16){
        os << "\tstatic unsigned int ";
    }else{
        os << "\tstatic unsigned short ";
    }
    os << "cin[" << cfgNum << "][" << (1 + cfgDataWidth / alignWidth) << "] __attribute__((aligned(" << cfgSpadDataByte << "))) = {\n";
    os << std::hex;
    int alignWidthHex = alignWidth/4;
    for(auto& cdp : cfgData){
        os << "\t\t{";
        for(auto data : cdp.data){      
            if(alignWidth == 32){
                os << "0x" << std::setw(alignWidthHex) << std::setfill('0') << data << ", ";
            }else{
                os << "0x" << std::setw(alignWidthHex) << std::setfill('0') << (data & 0xffff) << ", ";
                os << "0x" << std::setw(alignWidthHex) << std::setfill('0') << (data >> 16) << ", ";
            }
            
        }
        os << "0x" << std::setw(alignWidthHex) << std::setfill('0') << (cdp.addr) << "},\n";
    }
    os << std::dec << "\t};\n\n";

    _cfg_num = cfgNum;
    _cfg_len = cfgNum * (alignWidth + cfgDataWidth) / 8; // length of config_addr and config_data in bytes
    int cfgSpadSize = _adg->cfgSpadSize();
    int cfgBaseAddr;
    _ld_cfg_dep = 0;
    if(_cfg_len <= cfgSpadSize - _old_cfg_status.end){
        cfgBaseAddr = _old_cfg_status.end;
    }else if(_cfg_len <= _old_cfg_status.start){
        cfgBaseAddr = 0;
    }else{ // cfg data space overlap last cfg data space
        cfgBaseAddr = 0;
        _ld_cfg_dep = LD_DEP_EX_LAST_TASK;
    }
    _old_cfg_status.start = cfgBaseAddr;
    _old_cfg_status.end = cfgBaseAddr + (_cfg_len +  cfgSpadDataByte - 1) / cfgSpadDataByte * cfgSpadDataByte;
    _old_cfg_status.end = std::min(_old_cfg_status.end, cfgSpadSize);
}


// generate CGRA instructions
std::pair<std::vector<std::string>, std::vector<std::string>> IOScheduler::genInstructions(Mapping *mapping, std::ostream &os)
{
    DFG *dfg = mapping->getDFG();
    std::vector<std::string> ldArrayNames;
    std::vector<std::string> stArrayNames;
    int banks = _adg->numIobNodes();
    int sizeofBank = _adg->iobSpadBankSize();
    int cfgBaseAddrSpad = _old_cfg_status.start + banks * sizeofBank; // cfg spad on top of iob spad
    int cfgSpadDataByte = _adg->cfgSpadDataWidth() / 8;
    int cfgBaseAddrCtrl = _old_cfg_status.start / cfgSpadDataByte; // config base address the controller access
    os << "\tload_cfg((void*)cin, 0x" << std::hex << cfgBaseAddrSpad << std::dec << ", " 
       << _cfg_len << ", " << _task_id << ", " << _ld_cfg_dep << ");\n";
    std::map<int, DFGIONode*> dfgIoNodes;
    std::vector<std::pair<int, int>> ld_data_deps; // <id, dep>
    std::vector<int> st_ids; // store node ids  
    std::map<std::string, int> arrayDeps; // store the max dependence type of an array 
    for(auto &elem : _dfg_io_infos){
        DFGIONode* dfgIONode = dynamic_cast<DFGIONode*>(dfg->node(elem.first));
        dfgIoNodes[elem.first] = dfgIONode;
        if(elem.second.isStore){
            st_ids.push_back(elem.first);
        }else{
            int ori_dep = elem.second.dep;
            int sort_dep = 0;
            if(ori_dep == LD_DEP_ST_LAST_SEC_TASK){
                sort_dep = 1;
            }else if(ori_dep == LD_DEP_EX_LAST_TASK){
                sort_dep = 2;
            }else if(ori_dep == LD_DEP_ST_LAST_TASK){
                sort_dep = 3;
            }
            ld_data_deps.push_back(std::make_pair(elem.first, sort_dep));
            std::string memRefName = dfgIONode->memRefName(); 
            if(!arrayDeps.count(memRefName)){
                arrayDeps[memRefName] = sort_dep;
            }else if(arrayDeps[memRefName] < sort_dep){
                arrayDeps[memRefName] = sort_dep;
            }
        }        
    }

    // sort the load data commands according to array name to make same array access adjacent
    std::sort(ld_data_deps.begin(), ld_data_deps.end(), [&](std::pair<int, int> a, std::pair<int, int> b){
        std::string memRefNameA = dfgIoNodes[a.first]->memRefName(); 
        std::string memRefNameB = dfgIoNodes[b.first]->memRefName(); 
        // return memRefNameA < memRefNameB;
        return memRefNameA < memRefNameB || (memRefNameA == memRefNameB && a.second < b.second);
    });
    // sort the load data commands according to dependence type
    std::stable_sort(ld_data_deps.begin(), ld_data_deps.end(), [&](std::pair<int, int> a, std::pair<int, int> b){
        std::string memRefNameA = dfgIoNodes[a.first]->memRefName(); 
        std::string memRefNameB = dfgIoNodes[b.first]->memRefName(); 
        return (arrayDeps[memRefNameA] < arrayDeps[memRefNameB]);
        // return (a.second < b.second);
    });
    int i = 0;
    int ld_num = ld_data_deps.size();

    for(auto &elem : ld_data_deps){
        DFGIONode* dfgIONode = dfgIoNodes[elem.first];
        int dataLen = dfgIONode->memSize();
        std::string memRefName = dfgIONode->memRefName(); 
        int offset = dfgIONode->memOffset();        
        // the next load command info
        int fused = 0; // fused with the next command
        if(i < ld_num - 1){
            DFGIONode* dfgIONodeNext = dfgIoNodes[ld_data_deps[i+1].first];
            int dataLenNext = dfgIONodeNext->memSize();
            std::string memRefNameNext = dfgIONodeNext->memRefName(); 
            int offsetNext = dfgIONodeNext->memOffset();
            if(memRefName == memRefNameNext && dataLen == dataLenNext && offset == offsetNext){
                fused = 1;
            }
        }
        if(offset > 0){
            memRefName = "(void*)" + memRefName + "+" + std::to_string(offset);
        }
        ldArrayNames.push_back(memRefName);
        // void* remoteAddr = ioName2AddrLen[memRefName].first;        
        auto& ioInfo = _dfg_io_infos[elem.first];
        os << "\tload_data(din_addr[" << i << "], 0x" << std::hex << ioInfo.addr << std::dec << ", " 
           << dataLen << ", " << fused << ", " << _task_id << ", " << ioInfo.dep << ");\n";
        i++;
    }
    os << "\tconfig(0x" << std::hex << cfgBaseAddrCtrl << std::dec << ", " << _cfg_num << ", " << _task_id << ", " << 0 << ");\n";
    os << "\texecute(0x" << std::hex << _iob_ens << std::dec << ", " << _task_id << ", " << _ex_dep << ");\n";
    i = 0;
    for(auto id : st_ids){
        DFGIONode* dfgIONode = dfgIoNodes[id];
        int dataLen = dfgIONode->memSize();
        std::string memRefName = dfgIONode->memRefName();
        int offset = dfgIONode->memOffset();
        if(offset > 0){
            memRefName = "(void*)" + memRefName + "+" + std::to_string(offset);
        }
        stArrayNames.push_back(memRefName);
        // void* remoteAddr = ioName2AddrLen[memRefName].first;
        // int dataLen = ioName2AddrLen[memRefName].second;
        auto& ioInfo = _dfg_io_infos[id];
        os << "\tstore(dout_addr[" << i << "], 0x" << std::hex << ioInfo.addr << std::dec << ", " 
           << dataLen << ", " << _task_id << ", " << 0 << ");\n";
        i++;
    }
    return std::make_pair(ldArrayNames, stArrayNames);
}



// analyze dependence, allocate spad space, dump execution call function
void IOScheduler::execute(Mapping *mapping, std::ostream &os_func, std::ostream &os_call)
{
    // dump execution call function head
    os_func << "void cgra_execute(void** din_addr, void** dout_addr)\n{\n";
    // analyze the dependence and allocate space in the spad
    ioSchedule(mapping);
    // update bank status
    int banks = _adg->numIobNodes();
    for (int i = 0; i < banks; i++) { 
        _older_bank_status[i] = _old_bank_status[i];
        _old_bank_status[i] = _cur_bank_status[i];        
    }
    // generate config data and dump to ostream
    genCfgData(mapping, os_func);
    // generate CGRA execution instructions and dump
    auto arrayNames = genInstructions(mapping, os_func);
    os_func << "}\n";
    int ldNum = arrayNames.first.size();
    int stNum = arrayNames.second.size();
    if(ldNum > 0){
        os_call << "void* cgra_din_addr[" << ldNum << "] = {";
        for(int i = 0; i < ldNum-1; i++){
            os_call << arrayNames.first[i] << ", ";
        }
        os_call << arrayNames.first[ldNum-1] << "};\n";
    }
    if(stNum > 0){
        os_call << "void* cgra_dout_addr[" << stNum << "] = {";
        for(int i = 0; i < stNum-1; i++){
            os_call << arrayNames.second[i] << ", ";
        }
        os_call << arrayNames.second[stNum-1] << "};\n";
    }
    os_call << "cgra_execute(cgra_din_addr, cgra_dout_addr);\n";
    _task_id++;
    // return arrayNames;
}