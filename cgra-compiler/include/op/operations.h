#ifndef __OPERATIONS_H__
#define __OPERATIONS_H__

#include <iostream>
#include <fstream>
#include <map>
#include "nlohmann/json.hpp"
using json = nlohmann::json; 


// operation infomation
struct OpInfo
{
    int OPC;
    int numOperands;
    int numRes;
    int latency;
    int bitWidth;
    // float area;
    // float power;
    bool commutative;
    bool accumulative;
};

// singleton class
class Operations
{
private:
    // Map for operation info
    static std::map<std::string, OpInfo> _OpInfoMap;
    // initialize _OpInfoMap
    Operations(std::string& filename);
    ~Operations();
public:
    // public singleton intstance api
    static Operations& Instance(std::string filename){
        static Operations instance(filename);
        return instance;
    }
    static void print();
    static int numOps();
    static int OPC(const std::string& op);
    static int numOperands(const std::string& op);
    static int numRes(const std::string& op);
    static int latency(const std::string& op);
    static int bitWidth(const std::string& op);
    // static float area(const std::string& op);
    // static float power(const std::string& op);
    // if operands are commutative
    static bool isCommutative(const std::string& op);
    // if operands are accumulative
    static bool isAccumulative(const std::string& op);
    // if the operation is supported
    static bool opCapable(const std::string& op);
};



#endif