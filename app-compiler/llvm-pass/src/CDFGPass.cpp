#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/CFGPrinter.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"

#include "llvm/Transforms/Utils.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/CaptureTracking.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/MemoryBuiltins.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/Support/CommandLine.h"

#include "llvm/ADT/GraphTraits.h"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Passes.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/Analysis/LoopAccessAnalysis.h"

#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"

#include "llvm/IR/Attributes.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <set>

#include "llvm_cdfg.h"


using namespace llvm;

// mapping unit : loop
typedef struct
{
	bool isInnerLoop = false;
	Loop *lp;
	std::set<BasicBlock *> allBlocks;
	std::set<std::pair<BasicBlock *, BasicBlock *>> entryBlocks;
	std::set<std::pair<BasicBlock *, BasicBlock *>> exitBlocks;
} MappingUnit;


// parse function annotation, attribute key-values
// sizeAttrMap[attrKey] = attrValue
void ParseSizeAttr(Function &F, std::map<std::string, int>& sizeAttrMap)
{
	auto global_annos = F.getParent()->getNamedGlobal("llvm.global.annotations");
	if (global_annos)
	{
		auto a = cast<ConstantArray>(global_annos->getOperand(0));
		for (int i = 0; i < a->getNumOperands(); i++)
		{
			auto e = cast<ConstantStruct>(a->getOperand(i));
			if (auto fn = dyn_cast<Function>(e->getOperand(0)->getOperand(0)))
			{
				auto anno = cast<ConstantDataArray>(cast<GlobalVariable>(e->getOperand(1)->getOperand(0))->getOperand(0))->getAsCString();
				fn->addFnAttr("size", anno); // <-- add function annotation here
			}
		}
	}
	if (F.hasFnAttribute("size"))
	{
		Attribute attr = F.getFnAttribute("size");
		outs() << "Size attribute : " << attr.getValueAsString() << "\n";
		StringRef sizeAttrStr = attr.getValueAsString();
		SmallVector<StringRef, 8> sizeArr;
		sizeAttrStr.split(sizeArr, ',');
		for (int i = 0; i < sizeArr.size(); ++i)
		{
			std::pair<StringRef, StringRef> splitDuple = sizeArr[i].split(':');
			uint32_t size;
			splitDuple.second.getAsInteger(10, size);
			outs() << "ParseAttr:: name:" << splitDuple.first << ",size:" << size << "\n";
			sizeAttrMap[splitDuple.first.str()] = size;
		}
	}
}


// Depth-first search for the recursively successive basic blocks except the back-edge BBs
void dfsForSuccBBs(SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 1> BackEdgeBBPs,
		std::map<const BasicBlock *, std::set<const BasicBlock *>> &SuccBBsMap,
		const BasicBlock *currBB, const BasicBlock *startBB){
	for (auto iter = succ_begin(currBB); iter != succ_end(currBB); ++iter){
		const BasicBlock *succ = *iter;
        // skip back-edge BB
		std::pair<const BasicBlock *, const BasicBlock *> bbPair(currBB, succ);
		if (std::find(BackEdgeBBPs.begin(), BackEdgeBBPs.end(), bbPair) != BackEdgeBBPs.end()){//if find back-egde
			continue;
		}
		SuccBBsMap[startBB].emplace(succ);
		dfsForSuccBBs(BackEdgeBBPs, SuccBBsMap, succ, startBB);
	}
}


// get the recursively successive basic blocks except the back-edge BBs for each BB  
// Including self-BB
void getSuccBBsMap(Function &F, SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 1> &BackEdgeBBPs, 
        std::map<const BasicBlock *, std::set<const BasicBlock *>> &SuccBBsMap){
    for(auto &BB : F){
        const BasicBlock *BBPtr = &BB;
        SuccBBsMap[BBPtr].emplace(BBPtr);
        dfsForSuccBBs(BackEdgeBBPs, SuccBBsMap, BBPtr, BBPtr);
    }
}


void printSuccBBsMap(std::map<const BasicBlock *, std::set<const BasicBlock *>> &SuccBBsMap){
    outs() << "###### SuccBBsMap ######\n";
	for (auto &elem : SuccBBsMap){
		outs() << "SuccBBs(" << elem.first->getName().str() << ") : ";
		for (auto &succ : elem.second){
			outs() << succ->getName().str() << ", ";
		}
		outs() << "\n";
	}
}


// // get BBs not in the loops
// void getNonLoopBBs(Function &F, std::vector<Loop *> &loops, std::set<BasicBlock *> &nonloopBBs){
// 	nonloopBBs.clear();
//     for (BasicBlock &BB : F){
// 		BasicBlock *BBPtr = &BB;
// 		nonloopBBs.emplace(BBPtr);
// 	}
// 	for (Loop *lp : loops){
// 		for (BasicBlock *BB : lp->getBlocks()){
// 			nonloopBBs.erase(BB);
// 		}
// 	}
// }

// get inner-most loops (mapping units) according to LoopInfo
void getInnerMostLoops(std::map<std::string, MappingUnit> &mappingUnitMap, std::map<Loop *, std::string> &loopNames, 
    std::vector<Loop *> loops, std::string lnstr)
{
	for (int i = 0; i < loops.size(); ++i)
	{
        auto& loop = loops[i];
        std::string name = lnstr + "_" + std::to_string(i);
		loopNames[loop] = name;

		if (loop->getSubLoops().size() == 0){
			// innerMostLoops.push_back(loop);
            std::string loopName = "INNERMOST_" + name;
			mappingUnitMap[loopName].isInnerLoop = true;
			mappingUnitMap[loopName].lp = loop;
            mappingUnitMap[loopName].allBlocks.insert(loop->getBlocks().begin(), loop->getBlocks().end());
            SmallVector<std::pair<BasicBlock *, BasicBlock *>, 8> exitEdges;
            loop->getExitEdges(exitEdges);
            mappingUnitMap[loopName].exitBlocks.insert(exitEdges.begin(), exitEdges.end());
            mappingUnitMap[loopName].entryBlocks.insert(std::make_pair(loop->getLoopPredecessor(), loop->getHeader()));
			// for (BasicBlock *BB : loop->getBlocks()){
			// 	// loopsExclusieBasicBlockMap[loops[i]].push_back(BB);
			// 	// mappingUnitMap[loopName].allBlocks.insert(BB);
			// 	BB2MUnitMap[BB] = loopName;
			// }
		}else{
			getInnerMostLoops(mappingUnitMap, loopNames, loop->getSubLoops(), name);

			// for (BasicBlock *BB : loop->getBlocks()){
			// 	bool BBfound = false;
			// 	for (std::pair<Loop *, std::vector<BasicBlock *>> pair : loopsExclusieBasicBlockMap)
			// 	{
			// 		if (std::find(pair.second.begin(), pair.second.end(), BB) != pair.second.end())
			// 		{
			// 			BBfound = true;
			// 			break;
			// 		}
			// 	}
			// 	if (!BBfound)
			// 	{
			// 		loopsExclusieBasicBlockMap[loops[i]].push_back(BB);
			// 	}
			// }
		}
	}
}


void printMappingUnitMap(std::map<std::string, MappingUnit> &mappingUnitMap)
{
	outs() << "###### MappingUnitMap ###### \n";
	for (std::pair<std::string, MappingUnit> pair : mappingUnitMap)
	{
		outs() << pair.first << " :: ";
		for (BasicBlock *bb : pair.second.allBlocks)
		{
			outs() << bb->getName() << ", ";
		}
		outs() << "| entry = ";
		for (std::pair<BasicBlock *, BasicBlock *> bbPair : pair.second.entryBlocks)
		{
			outs() << bbPair.first->getName() << " to " << bbPair.second->getName() << ", ";
		}
		outs() << "| exit = ";
		for (std::pair<BasicBlock *, BasicBlock *> bbPair : pair.second.exitBlocks)
		{
			outs() << bbPair.first->getName() << " to " << bbPair.second->getName() << ", ";
		}
		outs() << "\n";
	}
}

// get target loop name (mapping unit name) using the token function "please_map_me"
std::string getMappingUnitNameUsingTokenFunction(Function &F, std::map<std::string, MappingUnit> &mappingUnitMap)
{
	BasicBlock *MUBB;
	Instruction *checker_ins = NULL;
	for (auto &BB : F){
		// BB.dump();
		for (auto &I : BB){
			if (CallInst *CI = dyn_cast<CallInst>(&I)){
				std::string op_str;
				raw_string_ostream rs(op_str);
				CI->print(rs);
				outs() << "op : " << rs.str() << "\n";
				if (op_str.find("please_map_me") != std::string::npos){
					outs() << "token found in BB = " << BB.getName() << "\n";
					MUBB = &BB;
                    checker_ins = CI;
				}
			}
		}
	}
	assert(MUBB);
	assert(checker_ins);
	checker_ins->eraseFromParent();

	for (auto &elem : mappingUnitMap){
		errs() << "loop name: "<< elem.first << "\n";
		if (elem.second.lp->contains(MUBB)){
			return elem.first;
		}
	}
	assert(false);
}


namespace {
    cl::opt<std::string> targetFuncName("fn", cl::init("na"), cl::desc("function name"));
    // cl::opt<std::string> cdfgType("type", cl::init("PartPred"), cl::desc("cdfg type, valid types = PartPred, Trig, TrMap, BrMap, DFGDISE"));

    struct CDFGPass : public FunctionPass {
        static char ID;
	    CDFGPass() : FunctionPass(ID){}
        // function annotation: size attribute
        std::map<std::string, int> SizeAttrMap;
        // basic block pairs that there is a back edge in between, <srcBB, dstBB>
        SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 1> BackEdgeBBPs;    
        // the recursively successive basic blocks except the back-edge BBs    
        std::map<const BasicBlock *, std::set<const BasicBlock *>> SuccBBsMap;
        // BBs in the target loop
        std::set<const BasicBlock *> LoopBBs;
        // mapping unit to CGRA/FGRA: loop, <loop-name, MappingUnit>
        std::map<std::string, MappingUnit> MappingUnitMap;
        // BB to the corresponding loop (MappingUnit)
        // std::map<BasicBlock *, std::string> BB2MUnitMap;
		
		//a set of nested-loops
		std::map<int, Loop*> nestloops;

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {	    
	    	AU.setPreservesAll();
	    	AU.addRequired<LoopInfoWrapperPass>();
	    	AU.addRequired<ScalarEvolutionWrapperPass>();
	    	// AU.addRequired<AAResultsWrapperPass>();
	    	AU.addRequired<DominatorTreeWrapperPass>();
	    	AU.addRequired<DependenceAnalysisWrapperPass>();
	    	AU.addRequiredID(LoopSimplifyID);
	    	AU.addRequiredID(LCSSAID);
	    	AU.addPreserved<DominatorTreeWrapperPass>();
	    }

	    virtual bool runOnFunction(Function &F) {
            std::string funcName = F.getName().str();
            if((targetFuncName != "na") && (targetFuncName != funcName)){        
				//errs() << funcName <<"\n\n\n\n";        
                return false;
            }
            std::string cfgName = funcName + "_cfg.dot";
            std::error_code EC;
            // raw_fd_ostream File(cfgName, EC, sys::fs::F_Text);
            // if(!EC){
            //     WriteGraph(File, (const Function *)&F);
            // }else{
            //     errs() << "Cannot open cfg file for WriteGraph!\n";
            // }
			// File.close();
            ParseSizeAttr(F, SizeAttrMap);
            LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
		    ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
		    DependenceInfo *DI = &getAnalysis<DependenceAnalysisWrapperPass>().getDI();
		    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
		    const DataLayout &DL = F.getParent()->getDataLayout();

            // find the back edges between BBs
            FindFunctionBackedges(F, BackEdgeBBPs);
            // get SuccBBsMap
            getSuccBBsMap(F, BackEdgeBBPs, SuccBBsMap);
            printSuccBBsMap(SuccBBsMap);
            // get inner-most loops
            std::vector<Loop *> loops;
            // std::vector<Loop *> innerMostLoops;
            std::map<Loop *, std::string> loopNames;
            for(auto iter = LI.begin(); iter != LI.end(); iter++){
                loops.push_back(*iter);
            }
            getInnerMostLoops(MappingUnitMap, loopNames, loops, "LN");
            printMappingUnitMap(MappingUnitMap);
            std::string munitName = getMappingUnitNameUsingTokenFunction(F, MappingUnitMap);
			Loop *tem = MappingUnitMap[munitName].lp;
			int i = 0;
			nestloops.clear();
			while(tem != nullptr){
				nestloops[i] = tem;
				i++;
				tem = tem->getParentLoop(); 
			}
		    // std::string munitName = "INNERMOST_LN0_1_1";
		    outs() << ">>>>>> MAPPING " << munitName << " of " << funcName << "<<<<<<\n";
			LLVMCDFG *CDFG = new LLVMCDFG(funcName + "_" + munitName);
			CDFG->SE = SE;
			CDFG->DT = DT;
			CDFG->DI = DI;
			CDFG->DL = &DL;
			CDFG->setBackEdgeBBPs(BackEdgeBBPs);
			CDFG->setSuccBBMap(SuccBBsMap);
			CDFG->setLoopBBs(MappingUnitMap[munitName].allBlocks, MappingUnitMap[munitName].entryBlocks, MappingUnitMap[munitName].exitBlocks);
			CDFG->setLoops(nestloops);
			// initialize CDFG
			CDFG->initialize();

			outs() << ">>>>>> analyze Load/Store Ins with affine access\n";
			CDFG->affineAnalyze();

			///TODO: Can't handle constant address GEP(which will be inserted as operand of LSInst)
			CDFG->generateCDFG();
		
            return true;

        } // end runOnFunction
    };
} // end namespace


char CDFGPass::ID = 0;
static RegisterPass<CDFGPass> X("cdfg", "CDFGPass", false, false);