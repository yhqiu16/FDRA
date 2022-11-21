#include <iostream>
#include <set>
#include <cstdlib>
#include <ctime>
#include <regex>
#include <sstream>
#include <getopt.h>
#include "op/operations.h"
#include "ir/adg_ir.h"
#include "ir/dfg_ir.h"
#include "mapper/mapper_sa.h"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/argv.h"
#include <vector>
#include <fstream>
#include <stdexcept>

// split string using regex
std::vector<std::string> split(const std::string& str, const std::string& delim){
    std::regex re{ delim };
    return std::vector<std::string>{
        std::sregex_token_iterator(str.begin(), str.end(), re, -1),
        std::sregex_token_iterator()
    };
}

// remove the prefix path  
std::string fileNameRemovePath(const std::string& filename) {
  size_t lastindex = filename.find_last_of(".");
  std::string res = filename.substr(0, lastindex);

  lastindex = filename.find_last_of("\\/");
  if (lastindex != std::string::npos) {
    res = res.substr(lastindex + 1);
  }
  return res;
}

// get the file directory
std::string fileDir(const std::string& filename) {
  size_t lastindex = filename.find_last_of("\\/");
  if (lastindex == std::string::npos) {
    return std::string("./");
  }
  return filename.substr(0, lastindex);
}

int main(int argc, char* argv[]) {
    // spdlog load log level from argv
    // ./examlpe SPDLOG_LEVEL=info, mylogger=trace
    spdlog::cfg::load_argv_levels(argc, argv);

    static struct option long_options[] = {
        // {"verbose",        no_argument,       nullptr, 'v',},
        {"dump-call-func",  required_argument, nullptr, 'c',},  // true/false
        {"dump-mapped-viz", required_argument, nullptr, 'm',},  // true/false
        {"obj-opt",         required_argument, nullptr, 'o',},  // true/false
        {"timeout-ms",      required_argument, nullptr, 't',},
        {"max-iters",       required_argument, nullptr, 'i',},
        {"op-file",         required_argument, nullptr, 'p',},
        {"adg-file",        required_argument, nullptr, 'a',},
        {"dfg-files",       required_argument, nullptr, 'd',},  // can input multiple files, separated by " " or ","
        {0, 0, 0, 0,}
    };
    static char* const short_options = (char *)"c:m:o:t:i:p:a:d:";

    std::string op_fn;  // "resources/ops/operations.json";  // operations file name
    std::string adg_fn; // "resources/adgs/my_cgra_test.json"; // ADG filename
    std::vector<std::string> dfg_fns; // "resources/dfgs/conv3.dot"; // DFG filenames
    int timeout_ms = 3600000;
    int max_iters = 2000;
    bool dumpCallFunc = true;
    bool dumpMappedViz = true;
    bool objOpt = true;
    // std::string resultDir = "";

    int opt;
    while ((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (opt) {
        //   case 'v': verbose = true; break;
            case 'c': std::istringstream(optarg) >> std::boolalpha >> dumpCallFunc; break;
            case 'm': std::istringstream(optarg) >> std::boolalpha >> dumpMappedViz; break;
            case 'o': std::istringstream(optarg) >> std::boolalpha >> objOpt; break;
            case 't': timeout_ms = atoi(optarg); break;
            case 'i': max_iters = atoi(optarg); break;
            case 'p': op_fn = optarg; break;
            case 'a': adg_fn = optarg; break;
            case 'd': dfg_fns = split(optarg, "[\\s,?]+"); break;            
            case '?': std::cout << "Unknown option: " << optopt << std::endl; exit(1);
        }
    }
    if(op_fn.empty()){
        std::cout << "Please input operation file!" << std::endl; 
        exit(1);
    }
    if(adg_fn.empty()){
        std::cout << "Please input ADG file!" << std::endl; 
        exit(1);
    }
    if(dfg_fns.empty()){
        std::cout << "Please input at least one DFG file!" << std::endl; 
        exit(1);
    }

    unsigned seed = time(0); // random seed using current time
    srand(seed);  // set random generator seed 
    std::cout << "Parse Operations: " << op_fn << std::endl;
    Operations::Instance(op_fn);
    // Operations::print();

    std::cout << "Parse ADG: " << adg_fn << std::endl;
    ADGIR adg_ir(adg_fn);
    ADG* adg = adg_ir.getADG();
    int numGpeNodes = adg->numGpeNodes();
    int numFuNodes = numGpeNodes + adg->numIobNodes();
    std::cout << "numGpeNodes: " << numGpeNodes << ", numFuNodes(GPE+IOB): "  << numFuNodes << std::endl;
    std::vector<float>storePEusage;
    std::vector<float>storeFUusage;
    std::vector<int>bestLatency;
    // adg->print();

    // map DFG to ADG
    MapperSA mapper(adg, timeout_ms, max_iters, objOpt);
    // MapperSA mapper(adg, dfg, 3600000, 2000);
    int numDfg = dfg_fns.size();
    int numSucceed = 0;
    for(auto& dfg_fn : dfg_fns){
        std::cout << "Parse DFG: " << dfg_fn << std::endl;
        DFGIR dfg_ir(dfg_fn);
        DFG* dfg = dfg_ir.getDFG();
        int numNodes = dfg->nodes().size();
        int numOpNodes = numNodes - dfg->ioNodes().size();
        std::cout << "numOpNodes: " << numOpNodes << ", numDfgNodes(Op+IO): "  << numNodes << std::endl;
        // dfg->print();
        // map DFG to ADG
        mapper.setDFG(dfg);
        std::string resultDir = fileDir(dfg_fn);
        bool succeed = mapper.execute(dumpCallFunc, dumpMappedViz, resultDir);
        if(!succeed){
            break;
        }
        std::string appName = fileNameRemovePath(dfg_fn);
        mapper.genSoCApp(resultDir, appName);
        numSucceed++;
        float usagePE = (float)numOpNodes/numGpeNodes;
        float usageFU = (float)numNodes/numFuNodes;
        storePEusage.push_back(usagePE);
        storeFUusage.push_back(usageFU);
        std::cout << "PE usage: " << usagePE << ", FU usage: " << usageFU << std::endl;
        bestLatency.push_back(mapper.getMaxLat());
        std::cout << "DFG latency: " << mapper.getMaxLat() << std::endl;
    }
    
    std::cout << "=============================================\n";
    if(numSucceed == numDfg){
        std::cout << "Succeed to map all the DFGs, number:  " << numSucceed << std::endl;
        float avgPEUsage = std::accumulate(storePEusage.begin(), storePEusage.end(), 0.0) / storePEusage.size();
        float avgFUUsage = std::accumulate(storeFUusage.begin(), storeFUusage.end(), 0.0) / storeFUusage.size();
        float avgLatency = std::accumulate(bestLatency.begin(), bestLatency.end(), 0) / (float)bestLatency.size();
        // std::ofstream fout("PEWaste.txt");
        // fout << (1-avgPEUsage) << std::endl;
        // fout.close();
        // std::ofstream fout2("bestLatency.txt");
        // fout2 << avgLatency << std::endl;
        // fout2.close();
        std::cout << "The average usage of PE is:  " << avgPEUsage << std::endl;
        std::cout << "The average usage of FU is:  " << avgFUUsage << std::endl;
        std::cout << "The average best max latency is:  " << avgLatency << std::endl;       
    }else{
        std::cout << "Fail to map the DFG:  " << dfg_fns[numSucceed] << std::endl;
    }
    std::cout << "=============================================\n";
    
    
    return numSucceed != numDfg;
}
