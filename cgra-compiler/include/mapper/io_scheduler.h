#ifndef __IO_SHEDULER_H__
#define __IO_SHEDULER_H__

#include "mapper/mapping.h"
#include "mapper/configuration.h"

// These Macro variables shoudl be consistent with those in cgra-mg
#define LD_DEP_ST_LAST_TASK 1
#define LD_DEP_EX_LAST_TASK 2
#define LD_DEP_ST_LAST_SEC_TASK 3
#define EX_DEP_ST_LAST_TASK 1

/* IOScheduler
* analyze IO conflicts between current task and last two tasks
* allocate scratchpad space for each IO trying to minimize conflicts
* schedule load/store commands according the conflict level
* generate a call function for the task 
*/

struct spadBankStatus
{
    int iob; // index of IOB occupying this bank
    int used; // if this bank is used; 0 : free, 1 : load data, 2 : store data
    int start; // start address in byte
    int end;   // end address in byte
};

struct spadCfgStatus
{
    int start; // start address in byte
    int end;   // end address in byte
};

struct dfgIoInfo
{
    // int iob; // index of mapped IOB 
    int addr;   // base address in spad
    int iobAddr; // access address by IOB
    int dep;     // dependent type
    bool isStore;
};


class IOScheduler
{
private:
    ADG *_adg;
    int _task_id = 0;
    // cache spad bank status for cotinuous three tasks
    std::vector<spadBankStatus> _cur_bank_status;
    std::vector<spadBankStatus> _old_bank_status;
    std::vector<spadBankStatus> _older_bank_status;
    // cache config spad status for last task
    spadCfgStatus _old_cfg_status;
    std::map<int, dfgIoInfo> _dfg_io_infos;
    int _cfg_num;
    int _cfg_len;
    int _ld_cfg_dep;
    int _ex_dep;
    uint64_t _iob_ens;
    long _dep_cost; // dependece cost

public:
    IOScheduler(ADG *adg);
    ~IOScheduler();
    // get dependence cost
    long getDepCost(){ return _dep_cost; }
    // get the base address and dependence of each DFG IO
    void ioSchedule(Mapping *mapping);
    // get the config data and dependence
    void genCfgData(Mapping *mapping, std::ostream &os);
    // generate CGRA instructions, return referred array names, the order should be consistent with the din_addr and dout_addr
    std::pair<std::vector<std::string>, std::vector<std::string>> genInstructions(Mapping *mapping, std::ostream &os);
    // void genInstructions(Mapping *mapping, std::map<std::string, std::pair<void*, int>>& ioName2AddrLen, std::ostream &os);
    // analyze dependence, allocate spad space, dump execution call function
    // std::vector<std::string> execute(Mapping *mapping, std::ostream &os);
    void execute(Mapping *mapping, std::ostream &os_func, std::ostream &os_call);
};





#endif