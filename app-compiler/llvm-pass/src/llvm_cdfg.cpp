
#include "llvm_cdfg.h"

//#include "cgra.h"



LLVMCDFG::~LLVMCDFG()
{
    for(auto& elem : _nodes){
        delete elem.second;
    }
    for(auto& elem : _edges){
        delete elem.second;
    }
}


LLVMCDFGNode* LLVMCDFG::node(int id)
{
    if(_nodes.count(id)){
        return _nodes[id];
    }
    return NULL;
}

LLVMCDFGNode* LLVMCDFG::node(Instruction *ins)
{
    if(_insNodeMap.count(ins)){
        return _insNodeMap[ins];
    }
    return NULL;
}


void LLVMCDFG::addNode(LLVMCDFGNode *node)
{
    int id = node->id();
    if(_nodes.count(id)){
        return;
    }
    _nodes[id] = node;
    if(auto ins = node->instruction()){
        _insNodeMap[ins] = node;
    }
}

// create node according to instruction and add node
LLVMCDFGNode* LLVMCDFG::addNode(Instruction *ins)
{
    if(_insNodeMap.count(ins)){
        return _insNodeMap[ins];
    }
    // create new node
    LLVMCDFGNode *node = new LLVMCDFGNode(ins, this);
    int id = 0;
    if(!_nodes.empty()){
        id = _nodes.rbegin()->first + 1;
    }
    node->setId(id);
    node->setBB(ins->getParent());
    _nodes[id] = node;
    _insNodeMap[ins] = node;
    return node;
}

// create node according to the custom instruction and add node
LLVMCDFGNode* LLVMCDFG::addNode(std::string customIns, BasicBlock *BB)
{
    LLVMCDFGNode *node = new LLVMCDFGNode(this);
    node->setCustomInstruction(customIns);
    int id = 0;
    if(!_nodes.empty()){
        id = _nodes.rbegin()->first + 1;
    }
    node->setId(id);
    node->setBB(BB);
    _nodes[id] = node;    
    return node;
}

// create node according to the custom instruction for pattern which is not with BB: TODO ?
LLVMCDFGNode* LLVMCDFG::addNode(std::string customIns)
{
    LLVMCDFGNode *node = new LLVMCDFGNode(this);
    node->setCustomInstruction(customIns);
    int id = 0;
    if(!_nodes.empty()){
        id = _nodes.rbegin()->first + 1;
    }
    node->setId(id);
    _nodes[id] = node;    
    return node;
}


void LLVMCDFG::delNode(LLVMCDFGNode *node)
{
    for(auto inputnode:node->inputNodes()){
        inputnode->delOutputNode(node);
    }
    for(auto outputnode:node->outputNodes()){
        outputnode->delInputNode(node);
    }
    int id = node->id();
    _nodes.erase(id);
    if(auto ins = node->instruction()){
        _insNodeMap.erase(ins);
    }
    auto inputEdges = node->inputEdges();
    for(int eid : inputEdges){
        delEdge(eid);
    }
    auto outputEdges = node->outputEdges();
    for(int eid : outputEdges){
        delEdge(eid);
    }
    delete node;
}


void LLVMCDFG::delNode(Instruction *ins)
{
    if(!_insNodeMap.count(ins)){
        return;
    }
    auto node = _insNodeMap[ins];
    for(auto inputnode:node->inputNodes()){
        inputnode->delOutputNode(node);
    }
    for(auto outputnode:node->outputNodes()){
        outputnode->delInputNode(node);
    }
    _nodes.erase(node->id());
    _insNodeMap.erase(ins);
    auto inputEdges = node->inputEdges();
    for(int eid : inputEdges){
        delEdge(eid);
    }
    auto outputEdges = node->outputEdges();
    for(int eid : outputEdges){
        delEdge(eid);
    }
    delete node;
}

void LLVMCDFG::delNodeTree(LLVMCDFGNode *node){
    int id = node->id();
    if(!_nodes.count(id)) return;
    auto teminputs = node->inputNodes();
    for(auto prenode : teminputs){
        if(prenode->outputEdges().size() == 1){
            //errs()<< "\tdelete tree of "<< prenode->getName()<<"\n";
            delNodeTree(prenode);
        }
    }
    errs()<< "\tdelete node "<<node->getName()<<"\n";
    delNode(node);
}

void LLVMCDFG::delNodeTree(Instruction *ins){
    if(!_insNodeMap.count(ins)){
        return;
    }
    auto node = _insNodeMap[ins];
    auto teminputs = node->inputNodes();
    for(auto prenode : teminputs){
        if(prenode->outputEdges().size() == 1){
            delNodeTree(prenode);
        }
    }
    delNode(node);
}


LLVMCDFGEdge* LLVMCDFG::edge(int id)
{
    if(_edges.count(id)){
        return _edges[id];
    }
    return NULL;
}


LLVMCDFGEdge* LLVMCDFG::edge(LLVMCDFGNode *src, LLVMCDFGNode *dst)
{
    for(auto eid : src->outputEdges()){
        assert(_edges.count(eid));
        auto outEdge = _edges[eid];
        if(outEdge->dst() == dst){
            return outEdge;
        }
    }
    return NULL;
}


void LLVMCDFG::addEdge(LLVMCDFGEdge *edge)
{
    int id = edge->id();
    if(!_edges.count(id)){
        _edges[id] = edge;
        edge->src()->addOutputEdge(id);
        edge->dst()->addInputEdge(id);
    }
}


LLVMCDFGEdge* LLVMCDFG::addEdge(LLVMCDFGNode *src, LLVMCDFGNode *dst, EdgeType type)
{
    int eid = 0;
    if(!_edges.empty()){
        eid = _edges.rbegin()->first + 1;
    }
    LLVMCDFGEdge *edge = new LLVMCDFGEdge(eid, type, src, dst);
    _edges[eid] = edge;
    src->addOutputEdge(eid);
    dst->addInputEdge(eid);
    return edge;
}



void LLVMCDFG::delEdge(LLVMCDFGEdge *edge)
{
    int id = edge->id();
    if(_edges.count(id)){
        _edges.erase(id);
    }
    edge->src()->delOutputEdge(id);
    edge->dst()->delInputEdge(id);
    delete edge;    
}


void LLVMCDFG::delEdge(int id)
{
    if(!_edges.count(id)){
       return;
    }
    auto edge = _edges[id];
    edge->src()->delOutputEdge(id);
    edge->dst()->delInputEdge(id);
    _edges.erase(id);
    delete edge;    
}


// get GEP node info 
std::string LLVMCDFG::GEPInfo(LLVMCDFGNode* node)
{
    assert(_GEPInfoMap.count(node));
    return _GEPInfoMap[node];
}


void LLVMCDFG::setGEPInfo(LLVMCDFGNode* node, std::string name)
{
    _GEPInfoMap[node] = name;
}


// get Input/Output node info 
std::string LLVMCDFG::IOInfo(LLVMCDFGNode* node)
{
    assert(_IOInfoMap.count(node));
    return _IOInfoMap[node];
}


void LLVMCDFG::setIOInfo(LLVMCDFGNode* node, std::string name)
{
    _IOInfoMap[node] = name;
}


// initialize CDFG according to loopBBs
void LLVMCDFG::initialize()
{
    outs() << ">>>>>>>>>>>>>initialize<<<<<<<<<<<<<\n";
    auto F = (*(_loopBBs.begin()))->getParent();//to check all ConstantExpr GEP
    for(auto &BB : *F){
        //errs()<< "check BB: ";BB.dump();
        for(auto &I : BB){
            int addressidx;
            GetElementPtrInst *GEP;
            Instruction *ins = &I;
            if(auto Load = dyn_cast<LoadInst>(ins)){
                GEP = dyn_cast<GetElementPtrInst>(Load->getPointerOperand());
                addressidx = Load->getPointerOperandIndex();
            }else if(auto Store = dyn_cast<StoreInst>(ins)){
                GEP = dyn_cast<GetElementPtrInst>(Store->getPointerOperand());
                addressidx = Store->getPointerOperandIndex();
            }else{
                continue;
            }
            //errs()<< "check LS: ";ins->dump();
            if(ConstantExpr * CE = dyn_cast<ConstantExpr>(ins->getOperand(addressidx))){
                //errs()<<"LS has ConstantExpr GEP: "; ins->dump();
                std::vector<Value *> Indices;
                for (unsigned index = 1; index < CE->getNumOperands(); ++index) {
                    Indices.push_back (CE->getOperand(index));
                }
                GEP = GetElementPtrInst::CreateInBounds(CE->getOperand(0),Indices,CE->getName(),ins);
                ins->replaceUsesOfWith(CE, GEP);
            }
        }
    }
    // Create CDFG nodes
    for(auto BB : _loopBBs){
        for(auto &I : *BB){
            addNode(&I);
        }
    }
    // create connections
    for(auto BB : _loopBBs){
        for(auto &I : *BB){
            Instruction *ins = &I;
            LLVMCDFGNode *node = this->node(ins);            
            // find out-of-loop connections or constant operands for non-phi nodes
            if(!dyn_cast<PHINode>(ins)){
                int idx = 0;
                for(Use &pred : ins->operands()){
                    // add input nodes
                    LLVMCDFGNode *inputNode = NULL;
                    if(ConstantInt *CI = dyn_cast<ConstantInt>(pred)){
                        if(dyn_cast<GetElementPtrInst>(ins)){ // GEP Constant will be handled in the handleGEPNodes
                            idx++;
                            continue;
                        }
                        inputNode = addNode("CONST", node->BB());
                        inputNode->setConstVal(CI->getSExtValue());
                    }else if(Instruction *predIns = dyn_cast<Instruction>(pred)){
                        if(!_loopBBs.count(predIns->getParent())){ // out of loop BB
                            inputNode = getInputNode(predIns, BB);
                        }
                    }else if(Argument *arg = dyn_cast<Argument>(pred)){
                        if(!pred->getType()->isPointerTy()){
                            inputNode = getInputNode(arg, BB);
                        }
                    }
                    EdgeType type = EDGE_TYPE_DATA;
                    if(pred->getType()->getPrimitiveSizeInBits() == 1){ //  single-bit operand
                        type = EDGE_TYPE_CTRL;
                    }    
                    if(inputNode){
                        node->addInputNode(inputNode, idx);
                        inputNode->addOutputNode(node);
                        addEdge(inputNode, node, type);
                    }
                    idx++;
                }
            }
            for(User *succ : ins->users()){                
                if(Instruction *succIns = dyn_cast<Instruction>(succ)){
                    LLVMCDFGNode *succNode = NULL;
                    bool isBackEdge = false;
                    if(!_loopBBs.count(succIns->getParent())){ // out of loop BB
                        succNode = getOutputNode(succIns, BB);
                        //succNode->setInstruction(succIns);//record output direct ins
                    }else if(dyn_cast<PHINode>(succIns)){
                        continue; // not connected here, instead in handlePHINodes
                    }else{
                        succNode = this->node(succIns);
                        // // only if the successor is PHINode, the back edge can exist.
                        // // the following condition should never be true
                        // if(BB != succIns->getParent()){ // not in the same BB
                        //     std::pair<const BasicBlock*, const BasicBlock*> bbp(BB, succIns->getParent());
                        //     if(std::find(_backEdgeBBPs.begin(), _backEdgeBBPs.end(), bbp) != _backEdgeBBPs.end()){
                        //         isBackEdge = true;
                        //     }
                        // }
                    }
                    // get the input index
                    int idx = -1;
                    // reorder the operand of SELECT and STORE node
                    if(auto SI = dyn_cast<SelectInst>(succIns)){
                        if(ins == dyn_cast<Instruction>(SI->getCondition())){
                            idx = 2;
                        }else if(ins == dyn_cast<Instruction>(SI->getTrueValue())){
                            idx = 0;
                        }else if(ins == dyn_cast<Instruction>(SI->getFalseValue())){
                            idx = 1;
                        }else{
                            assert(false);
                        }
                    }else if(auto SI = dyn_cast<StoreInst>(succIns)){
                        if(ins == dyn_cast<Instruction>(SI->getValueOperand())){
                            idx = 0;
                        }else{
                            idx = 1;
                        }
                    }else{
                        for(int i = 0; i < succIns->getNumOperands(); i++){
                            if(ins == dyn_cast<Instruction>(succIns->getOperand(i))){
                                idx = i;
                                break;
                            }
                        }
                    }   
                    EdgeType type = EDGE_TYPE_DATA;
                    if(ins->getType()->getPrimitiveSizeInBits() == 1){ //  single-bit operand
                        type = EDGE_TYPE_CTRL;
                    }                 
                    node->addOutputNode(succNode, isBackEdge);
                    succNode->addInputNode(node, idx, isBackEdge);
                    addEdge(node, succNode, type);
                }
            }
        }
    }  
}



// add edge between two nodes that have memory dependence (loop-carried)
void LLVMCDFG::addMemDepEdges()
{
    std::vector<LLVMCDFGNode *> LSNodes; // load/store nodes
    // find the load/store nodes
    for(auto &elem : _nodes){
        auto node = elem.second;
        Instruction *ins = node->instruction();
        if(ins == NULL){
            continue;
        }
        if(LoadInst *LI = dyn_cast<LoadInst>(ins)){
            if(LI->isSimple()){
                LSNodes.push_back(node);
            }
        }else if(StoreInst *SI = dyn_cast<StoreInst>(ins)){
            if(SI->isSimple()){
                LSNodes.push_back(node);
            }
        }
    }
    // analyze dependence between every two LSNodes
    int N = LSNodes.size();
    for(int i = 0; i < N; i++){
        LLVMCDFGNode *srcNode = LSNodes[i];
        Instruction *srcIns = srcNode->instruction();
        for(int j = i + 1; j < N; j++){
            LLVMCDFGNode *dstNode = LSNodes[j];
            Instruction *dstIns = dstNode->instruction();
            if(auto D = DI->depends(srcIns, dstIns, true)){
                outs() << "Found memory dependence between " << LSNodes[i]->getName() 
                       << "(src) and " << LSNodes[j]->getName() << "(dst)\n";
                if(D->isLoopIndependent()){
                    outs() << "Loop independent(skipped)\n";
                    continue;
                }
                outs() << "Loop carried dependence\n";
                DepType type = NON_DEP; // dependence type
                if(D->isFlow()){ // RAW, read after write
                    type = FLOW_DEP;
                    outs() << "FLOW_DEP, ";
                }else if(D->isAnti()){ // WAR, write after read
                    type = ANTI_DEP;
                    outs() << "ANTI_DEP, ";
                }else if(D->isOutput()){ // WAW
                    type = OUTPUT_DEP;
                    outs() << "OUTPUT_DEP, ";
                }else if(D->isInput()){ // RAR
                    type = INPUT_DEP;
                    outs() << "INPUT_DEP, ";
                }               
                int loopIterDist; // loop-carried iteration distance, e.g. a[i+1] = a[i] : 1
                bool isLoopDistConst = 0;
                int nestedLevels = D->getLevels(); // nested loop levels, [1, nestedLevels], 1 is the outer-most loop
                assert(nestedLevels > 0);
                // target at the inner-most loop
                const SCEV *dist = D->getDistance(nestedLevels);
                const SCEVConstant *distConst = dyn_cast_or_null<SCEVConstant>(dist);
                bool reverse = false;
                if(distConst){
                    loopIterDist = distConst->getAPInt().getSExtValue();
                    outs() << "const distance: " << loopIterDist << "\n";
                    isLoopDistConst = true;
                    if(loopIterDist < 0){
                        reverse = true;
                        loopIterDist = -loopIterDist;
                        if(type == FLOW_DEP){
                            type = ANTI_DEP;
                        }else if(type == ANTI_DEP){
                            type = FLOW_DEP;
                        }
                    }                   
                }else{ /// if no subscript in the source or destination mention the induction variable associated with the loop at this level.
                    isLoopDistConst = false;   
                    outs() << "non-const distance\n";                               
                }                
                // add mem dep edges
                DependInfo dep;
                dep.type = type;
                dep.isConstDist = isLoopDistConst;
                dep.distance = loopIterDist;
                if(reverse){
                    dstNode->addDstDep(srcNode, dep);
                    srcNode->addSrcDep(dstNode, dep);
                    addEdge(dstNode, srcNode, EDGE_TYPE_MEM);
                }else{
                    srcNode->addDstDep(dstNode, dep);
                    dstNode->addSrcDep(srcNode, dep);
                    addEdge(srcNode, dstNode, EDGE_TYPE_MEM);
                }                
            }
        }
    }
}


// root nodes : no output nodes
std::vector<LLVMCDFGNode *> LLVMCDFG::getRoots()
{
	std::vector<LLVMCDFGNode *> rootNodes;
    for(auto &elem : _nodes){
        if(elem.second->outputNodes().size() == 0){
            rootNodes.push_back(elem.second);
        }
    }
	return rootNodes;
}

// leaf nodes : no input nodes
std::vector<LLVMCDFGNode *> LLVMCDFG::getLeafs()
{
	std::vector<LLVMCDFGNode *> leafNodes;
    for(auto &elem : _nodes){
        if(elem.second->inputNodes().size() == 0){
            leafNodes.push_back(elem.second);
        }
    }
	return leafNodes;
}

// leaf nodes in a BB : no input nodes in this BB or is INPUT/PHI
std::vector<LLVMCDFGNode *> LLVMCDFG::getLeafs(BasicBlock *BB)
{
	std::vector<LLVMCDFGNode *> leafNodes;
    for(auto &elem : _nodes){
        auto node = elem.second;
        if(node->BB() != BB || node->customInstruction() == "INPUT"){
            continue;
        }
        if(node->instruction() && dyn_cast<PHINode>(node->instruction())){
            continue;
        }
        bool flag = true;
        for(auto &inNode : node->inputNodes()){
            if(inNode->BB() == BB && inNode->customInstruction() != "INPUT"){
                if(inNode->instruction() == NULL || !dyn_cast<PHINode>(inNode->instruction())){
                    flag = false;
                    break;
                }
            }
        }
        if(flag){
            leafNodes.push_back(node);
        }
    }
	return leafNodes;
}


// return a map of basicblocks to their real control dependent (recursive) predecessors with the respective control value
std::map<BasicBlock*, std::set<std::pair<BasicBlock*, CondVal>>> LLVMCDFG::getCtrlDepPredBBs() {
	std::map<BasicBlock*,std::set<std::pair<BasicBlock*,CondVal>>> res;
    // BFS to find all the recursive predecessors except the out-of-loop and back-edge ones for each BB
	for(BasicBlock* BB : _loopBBs){
		std::queue<BasicBlock*> q;
		q.push(BB);
		std::map<BasicBlock*,std::set<CondVal>> visited;
		while(!q.empty()){
			BasicBlock* curr = q.front(); q.pop();
			for (auto it = pred_begin(curr); it != pred_end(curr); ++it){
				BasicBlock* predecessor = *it;
				if(_loopBBs.find(predecessor) == _loopBBs.end()){
					continue; // no need to care for out of loop BBs.
				}
				std::pair<const BasicBlock*,const BasicBlock*> bbPair = std::make_pair(predecessor, curr);
				if(std::find(_backEdgeBBPs.begin(), _backEdgeBBPs.end(), bbPair) != _backEdgeBBPs.end()){
					continue; // no need to traverse backedges;
				}
				CondVal cv;
				assert(predecessor->getTerminator());
				BranchInst* BRI = cast<BranchInst>(predecessor->getTerminator());
                // get control value
				if(!BRI->isConditional()){
					cv = UNCOND;
				}else{
					for (int i = 0; i < BRI->getNumSuccessors(); ++i) {
						if(BRI->getSuccessor(i) == curr){
							if(i==0){
								cv = TRUE_COND;
							}else if(i==1){
								cv = FALSE_COND;
							}else{
								assert(false);
							}
						}
					}
				}
				visited[predecessor].insert(cv);
				q.push(predecessor);
			}
		}

		outs() << "BasicBlock : " << BB->getName() << " :: CtrlDependentBBs = ";
		for(std::pair<BasicBlock*,std::set<CondVal>> pair : visited){
			BasicBlock* bb = pair.first;
			std::set<CondVal> brOuts = pair.second;
			outs() << bb->getName();
			if(brOuts.count(TRUE_COND)){
				res[BB].insert(std::make_pair(bb, TRUE_COND)); // TRUE control dependent predecessor BB
				outs() << "(TRUE),";
			}
			if(brOuts.count(FALSE_COND)){
				res[BB].insert(std::make_pair(bb, FALSE_COND)); // FALSE control dependent predecessor BB
				outs() << "(FALSE),";
			}
		}
		outs() << "\n";
	} // res[BB] = set<preBB, TRUE/FALSE_COND>

    // remove extra <preBB, TRUE/FALSE_COND>
	std::map<BasicBlock*, std::set<std::pair<BasicBlock*, CondVal>>> refinedRes;
	for(auto &pair : res){
		std::set<std::pair<BasicBlock*,CondVal>> tobeRemoved;
		BasicBlock* currBB = pair.first;
		outs() << "BasicBlock : " << currBB->getName() << " :: RefinedCtrlDependentBBs = ";
		for(auto &bbVal : pair.second){
			BasicBlock* depBB = bbVal.first;
			for(auto &p2 : res[depBB]){
				tobeRemoved.insert(p2);
			}
		}
        // if currBB and depBB have the same <preBB, TRUE/FALSE_COND>, remove from res[BB]
		for(auto &bbVal : pair.second){
			if(tobeRemoved.find(bbVal) == tobeRemoved.end()){
				outs() << bbVal.first->getName();
				outs() << ((bbVal.second == TRUE_COND)? "(TRUE)," : "(FALSE),");
				refinedRes[currBB].insert(bbVal);
			}
		}
		outs() << "\n";
	}

    // remove preBB with both TRUE_COND and FALSE_COND from refinedRes
	std::map<BasicBlock*,std::set<std::pair<BasicBlock*,CondVal>>> finalRes;
    for(auto &pair : refinedRes){
		BasicBlock* currBB = pair.first;
		outs() << "BasicBlock : " << currBB->getName() << " :: FinalCtrlDependentBBs = ";
		std::set<std::pair<BasicBlock*, CondVal>> bbValPairs = pair.second; // auto-sorted, first prority : BasicBlock*, second : CondVal
        assert(bbValPairs.size() > 0);
        bool changed = true;
        while(changed){
            changed = false;
            for(auto it = bbValPairs.begin(), ie = --bbValPairs.end(); it != ie;){
                auto bb1 = it->first;
                auto old_it = it;
                auto next_it = ++it;
                if(bb1 == next_it->first){
                    bbValPairs.erase(old_it, ++next_it); // remove TRUE_COND and FALSE_COND preBBs
                    if(finalRes.count(bb1)){ // already got the final control dependent BBs
                        for(auto bbp : finalRes[bb1]){
                            bbValPairs.insert(bbp);
                        }
                    }else if(refinedRes.count(bb1)){ // add control dependent BBs from refinedRes[bb1]
                        for(auto bbp : refinedRes[bb1]){
                            bbValPairs.insert(bbp);
                        }
                    }
                    changed = (bbValPairs.size() > 0); // if still have element, continue
                    break;
                }
            }
        }
        if(bbValPairs.size() > 0){
            finalRes[currBB] = bbValPairs;
            for(auto &bbVal: bbValPairs){
			    outs() << bbVal.first->getName();
			    outs() << ((bbVal.second == TRUE_COND)? "(TRUE)," : "(FALSE),");
		    }
        }
        outs() << "\n";
    }
	return finalRes;
}


// get the control dependent nodes in a BB, including unconditional BranchInst, StoreInst, OUTPUT
// only TRUE_COND, STORE can be performed
std::vector<LLVMCDFGNode*> LLVMCDFG::getCtrlDepNodes(BasicBlock *BB)
{
    std::vector<LLVMCDFGNode*> res;
    for(auto &elem : _nodes){
        auto node = elem.second;
        if(node->BB() != BB){
            continue;
        }
        if(Instruction *ins = node->instruction()){
            if(BranchInst *BRI = dyn_cast<BranchInst>(ins)){
                if(!BRI->isConditional()){
                    res.push_back(node);
                }
            }else if(dyn_cast<StoreInst>(ins)){
                res.push_back(node);
            }
        }else if(node->customInstruction() == "OUTPUT"){
            res.push_back(node);
        }
    }
    return res;
}


// Connect control dependent node pairs among BBs
void LLVMCDFG::connectCtrlDepBBs(){
    // get real control dependent (recursive) predecessors with the respective control value
	std::map<BasicBlock*, std::set<std::pair<BasicBlock*, CondVal>>> CtrlDepPredBBs = getCtrlDepPredBBs();
	for(auto &elem : CtrlDepPredBBs){
		BasicBlock* currBB = elem.first;
		std::vector<LLVMCDFGNode*> ctrlDepNodes = getCtrlDepNodes(currBB);

		for(auto &ctrlDepPredBB : elem.second){
			BasicBlock* preBB = ctrlDepPredBB.first;
			CondVal cond = ctrlDepPredBB.second;
			BranchInst* BRI = cast<BranchInst>(preBB->getTerminator());
			bool isConditional = BRI->isConditional();
			assert(isConditional);
			LLVMCDFGNode* BRNode = node(BRI); 
            assert(BRNode);
			assert(BRNode->inputNodes().size() == 1);
            auto preBRNode = BRNode->inputNodes()[0];
            bool isBackEdge = false;
            // // preBB->currBB is not backedge since it is avoided in getCtrlDepNodes
            // std::pair<const BasicBlock*, const BasicBlock*> bbp = std::make_pair(preBB, currBB);
            // if(std::find(_backEdgeBBPs.begin(), _backEdgeBBPs.end(), bbp) != _backEdgeBBPs.end()){
            //     isBackEdge = true;
            // }
			outs() << "ConnectBB :: From " << preBRNode->getName() << "(srcBB = " << preBB->getName() << ")" << ", To ";
			for(LLVMCDFGNode* succNode : ctrlDepNodes){
				outs() << succNode->getName() << ", ";
                preBRNode->addOutputNode(succNode, isBackEdge, cond);
                succNode->addInputNode(preBRNode, -1, isBackEdge, cond); // donot care the index
                addEdge(preBRNode, succNode, EDGE_TYPE_CTRL);
			}
			outs() << "(destBB = " << currBB->getName() << ")\n";
		}
	}
}


// insert Control NOT node behind the condition node of the Branch node
void LLVMCDFG::insertCtrlNotNodes()
{
    outs() << "insertCtrlNotNodes STARTED!\n";
    // for(auto &elem : _nodes){
    //     LLVMCDFGNode* node = elem.second;
    //     std::set<LLVMCDFGNode*> falseOutNodes;
    //     for(auto outNode : node->outputNodes()){
    //         if(node->getOutputCondVal(outNode) == FALSE_COND){
    //             falseOutNodes.insert(outNode);
    //         }
    //     }
        // if(falseOutNodes.empty()){
        //     continue;
        // }
        // // create CTRLNOT node
        // LLVMCDFGNode* notNode = addNode("CTRLNOT", node->BB());
		// outs() << "newNOTNode = " << notNode->getName() << "\n";
        // for(auto outNode : falseOutNodes){
        //     // add new connections
        //     bool isBackEdge = node->isOutputBackEdge(outNode);
        //     notNode->addOutputNode(outNode, isBackEdge, TRUE_COND);
        //     outNode->addInputNode(notNode, isBackEdge, TRUE_COND);
        //     addEdge(notNode, outNode, EDGE_TYPE_CTRL);
        //     // delete old connections
        //     node->delOutputNode(outNode);
        //     outNode->delInputNode(node);
        // }
    for(auto BB : _loopBBs){       
        BranchInst* BRI = cast<BranchInst>(BB->getTerminator());
        if(!BRI->isConditional()){
            continue;
        }
        Instruction *condIns = dyn_cast<Instruction>(BRI->getCondition()); // conditional predecessor
        LLVMCDFGNode *node = this->node(condIns);  
        outs() << "insert CTRLNOT node behind " << node->getName() << ", ";
        // create CTRLNOT node
        LLVMCDFGNode* notNode = addNode("CTRLNOT", node->BB());
		outs() << "newNOTNode = " << notNode->getName() << "\n";  
        std::set<LLVMCDFGNode*> falseOutNodes;                
        for(auto outNode : node->outputNodes()){
            if(node->getOutputCondVal(outNode) == FALSE_COND){
                falseOutNodes.insert(outNode);
            }
        }
        for(auto outNode : falseOutNodes){
            bool isBackEdge = node->isOutputBackEdge(outNode); //  should be false
            // delete old connections
            node->delOutputNode(outNode);
            int idx = outNode->delInputNode(node);
            // add new connections           
            notNode->addOutputNode(outNode, isBackEdge, TRUE_COND);
            outNode->addInputNode(notNode, idx, isBackEdge, TRUE_COND);
            addEdge(notNode, outNode, EDGE_TYPE_CTRL);            
        }
        // delete old edges
        auto temp = node->outputEdges();
        for(auto eid : temp){
            auto outEdge = edge(eid);
             outs() << "outEdge NULL?(1 is empty)" << (outEdge == NULL) << "\n";
             outs() << "outEdge source: " << outEdge->src()->getName() << "\n"; //
            auto dstNode = outEdge->dst();
            if(falseOutNodes.count(dstNode)){
                delEdge(outEdge);
            }
        }
        node->addOutputNode(notNode, false, FALSE_COND);
        notNode->addInputNode(node, 0, false, FALSE_COND);
        addEdge(node, notNode, EDGE_TYPE_CTRL);
    }
    outs() << "insertCtrlNotNodes ENDED!\n";
}


// transfer the multiple control predecessors (input nodes) into a inverted OR tree 
// with the root connected to a node and leaves connected to control predecessors
void LLVMCDFG::createCtrlOrTree() 
{
	outs() << "createCtrlOrTree STARTED!\n";
    std::map<LLVMCDFGNode*, std::set<LLVMCDFGNode*>> condParentsMap; // conditional parents Map
	for(auto &elem : _nodes){
        LLVMCDFGNode* node = elem.second;		
		for(LLVMCDFGNode* par : node->inputNodes()){
            auto cond = node->getInputCondVal(par);
			if( cond != UNCOND){
                condParentsMap[node].insert(par);
				// condParents.insert(par);
			}
		}
    }
    for(auto &elem : condParentsMap){
        LLVMCDFGNode* node = elem.first;
        auto &condParents = elem.second; // conditional parents
        // create CTRLOR tree
		if(condParents.size() > 1){
			std::queue<LLVMCDFGNode*> q;
			for(auto pp : condParents){
				q.push(pp);
			}
			while(!q.empty()){
				auto pp1 = q.front(); q.pop();
				if(!q.empty()){
					auto pp2 = q.front(); q.pop();
					outs() << "Connecting pp1 = " << pp1->getName() << ", ";
					outs() << "pp2 = " << pp2->getName() << ", ";                   
                    // add CTRLOR node
                    LLVMCDFGNode* orNode = addNode("CTRLOR", node->BB());
					outs() << "newORNode = " << orNode->getName() << "\n";                    
					bool isPP1BackEdge = node->isInputBackEdge(pp1);
					bool isPP2BackEdge = node->isInputBackEdge(pp2);
                    assert(node->getInputCondVal(pp1) == TRUE_COND); // the FALSE_COND should be transformed to TRUE_COND before calling this function
                    assert(node->getInputCondVal(pp2) == TRUE_COND);
                    // delete old connections
                    pp1->delOutputNode(node);
                    node->delInputNode(pp1);
                    pp2->delOutputNode(node);
                    node->delInputNode(pp2);     
                    delEdge(edge(pp1, node));     
                    delEdge(edge(pp2, node));  
                    // add new connections
                    pp1->addOutputNode(orNode, isPP1BackEdge, TRUE_COND);
                    orNode->addInputNode(pp1, 0, isPP1BackEdge, TRUE_COND);
                    pp2->addOutputNode(orNode, isPP2BackEdge, TRUE_COND);
                    orNode->addInputNode(pp2, 1, isPP2BackEdge, TRUE_COND);
                    orNode->addOutputNode(node, false, TRUE_COND);
                    node->addInputNode(orNode, -1, false, TRUE_COND);
                    addEdge(pp1, orNode, EDGE_TYPE_CTRL);
                    addEdge(pp2, orNode, EDGE_TYPE_CTRL);
                    addEdge(orNode, node, EDGE_TYPE_CTRL);   
                            
					q.push(orNode);
				}else{
					outs() << "Alone node = " << pp1->getName() << "\n";
				}
			}
		}
	}
	outs() << "createCtrlOrTree ENDED!\n";
}


// get loop start node. If not exist, create one.
LLVMCDFGNode* LLVMCDFG::getLoopStartNode(BasicBlock *BB)
{
    if(_loopStartNodeMap.count(BB)){
        return _loopStartNodeMap[BB];
    }
    // create new node and add node
    LLVMCDFGNode *node = addNode("LOOPSTART", BB);
    // int cnt = _loopStartNodeMap.size();
    _loopStartNodeMap[BB] = node;
    // node->setConstVal(cnt);
    return node;
}


// get loop exit node. If not exist, create one.
LLVMCDFGNode* LLVMCDFG::getLoopExitNode(BasicBlock *BB)
{
    if(_loopExitNodeMap.count(BB)){
        return _loopExitNodeMap[BB];
    }
    // create new node and add node
    LLVMCDFGNode *node = addNode("LOOPEXIT", BB);
    // int cnt = _loopExitNodeMap.size();
    _loopExitNodeMap[BB] = node;
    // node->setConstVal(cnt);
    return node;
}

// get input node. If not exist, create one.
LLVMCDFGNode* LLVMCDFG::getInputNode(Value *ins, BasicBlock *BB)
{
    if(_ioNodeMap.count(ins)){
        return _ioNodeMap[ins];
    }
    // create new node and add node
    LLVMCDFGNode *node = addNode("INPUT", BB);
    _ioNodeMap[ins] = node;
    // _ioNodeMapReverse[node] = ins;
    setIOInfo(node, ins->getName());
    return node;
}


// get out loop store node. If not exist, create one.
LLVMCDFGNode* LLVMCDFG::getOutputNode(Value *ins, BasicBlock *BB)
{
    if(_ioNodeMap.count(ins)){
        return _ioNodeMap[ins];
    }
    // create new node and add node
    LLVMCDFGNode *node = addNode("OUTPUT", BB);
    _ioNodeMap[ins] = node;
    // _ioNodeMapReverse[node] = ins;
    setIOInfo(node, ins->getName());
    return node;
}


/*
 * This function
 * 1) connects PHI nodes with its parents by analyzing PHI instruction[value,basic block][value, basic block]
 * 2) transfer PHI nodes to SELECT nodes
 * 3) add LOOPSTART control node, INPUT data node
 *
 * */
void LLVMCDFG::handlePHINodes() 
{
	std::vector<LLVMCDFGNode*> phiNodes;
	for(auto &elem : _nodes){
        auto &node = elem.second;
		if(node->instruction() && dyn_cast<PHINode>(node->instruction()) && node->customInstruction() != "SELECT"){
			phiNodes.push_back(node);
		}
	}
    
	for(LLVMCDFGNode* node : phiNodes){
		PHINode* PHI = dyn_cast<PHINode>(node->instruction());
        outs() << "PHI :: "; PHI->dump();
		assert(node->inputNodes().empty());
        std::vector<std::pair<LLVMCDFGNode*, LLVMCDFGNode*>> phiParents; // <value-node, control-node>>
		for (int i = 0; i < PHI->getNumIncomingValues(); ++i) {
			BasicBlock* bb = PHI->getIncomingBlock(i);
			Value* V = PHI->getIncomingValue(i);
			outs() << "IncomingValue :: "; V->dump();
			LLVMCDFGNode* previousCtrlNode = NULL;
			if(_loopBBs.find(bb) == _loopBBs.end()){ // predecessor not in loopBBs					
				std::pair<BasicBlock*,BasicBlock*> bbPair = std::make_pair(bb, node->BB());
				assert(_loopentryBBs.find(bbPair) != _loopentryBBs.end()); // should be loop entry
                LLVMCDFGNode *startNode = getLoopStartNode(bb);
				previousCtrlNode = startNode;
			}else{ // within the loopBBs
				BranchInst* BRI = cast<BranchInst>(bb->getTerminator());
				LLVMCDFGNode* BRNode = this->node(BRI);
				if(!BRI->isConditional()){
                    auto &inNodes = BRNode->inputNodes();
                    // if(inNodes.size() > 0){
                    assert(inNodes.size() == 1); // only one conditional predecessor after createCTRLORTree
                    previousCtrlNode = inNodes[0];
                    // }
				}else{
					assert(BRNode->inputNodes().size() == 1); // only one condition predecessor
                    LLVMCDFGNode *ctrlNode = BRNode->inputNodes()[0];
                    if(BRI->getSuccessor(0) == node->BB()){ // true condition
                        previousCtrlNode = ctrlNode;
                    }else{ // false condition
                        assert(BRI->getSuccessor(1) == node->BB());
                        // find the CTRLNOT node
                        for(auto outNode : ctrlNode->outputNodes()){
                            if(outNode->customInstruction() == "CTRLNOT"){
                                previousCtrlNode = outNode;
                                break;
                            }
                        }
                    }
				}
			}
			assert(previousCtrlNode != NULL);
            outs() << "previousCTRLNode : " << previousCtrlNode->getName() << "\n";
            // get operand value
            LLVMCDFGNode* phiParent = NULL;
            
            if(ConstantInt* CI = dyn_cast<ConstantInt>(V)){
				int constant = CI->getSExtValue();
                phiParent = addNode("CONST", bb);
                phiParent->setConstVal(constant);
			}else if(ConstantFP* FP = dyn_cast<ConstantFP>(V)){
				int constant = (int)FP->getValueAPF().convertToFloat();
                phiParent = addNode("CONST", bb);
                phiParent->setConstVal(constant);
			}else if(UndefValue *UND = dyn_cast<UndefValue>(V)){
                phiParent = addNode("CONST", bb);
                phiParent->setConstVal(0);
			}else if(Argument *ARG = dyn_cast<Argument>(V)){
                phiParent = addNode("CONST", bb);
                phiParent->setConstVal(0);
			}else{				
				if(Instruction* ins = dyn_cast<Instruction>(V)){
					phiParent = this->node(ins);
				}
				if(phiParent == NULL){ //not found
                    phiParent = getInputNode(V, bb);
				}
            }
            phiParents.push_back(std::make_pair(phiParent, previousCtrlNode));
        }
        for(int i = 0; i + 1 < phiParents.size(); i += 2){
            // connect two parents to a SELECT node
            // operand 0 : true data; 1 : false data; 2 : condition (conditional node of parent 1)
            auto &pp1 = phiParents[i];
            auto &pp2 = phiParents[i+1];
            // create a SELECT node
            LLVMCDFGNode *selNode = addNode("SELECT", node->BB());
            if(!PHItoLevel.count(PHI))
                selNode->setInstruction(PHI);///set Instruction if this SELECT is not index
            outs() << "new SELECT node = " << selNode->getName() << "\n";
            bool isBackEdge1 = false;
            std::pair<const BasicBlock*, const BasicBlock*> bbp1 = std::make_pair(pp1.first->BB(), node->BB());
            if(std::find(_backEdgeBBPs.begin(), _backEdgeBBPs.end(), bbp1) != _backEdgeBBPs.end()){
                isBackEdge1 = true;
            }
            bool isBackEdge2 = false;
            std::pair<const BasicBlock*, const BasicBlock*> bbp2 = std::make_pair(pp2.first->BB(), node->BB());
            if(std::find(_backEdgeBBPs.begin(), _backEdgeBBPs.end(), bbp2) != _backEdgeBBPs.end()){
                isBackEdge2 = true;
            }
            selNode->addInputNode(pp1.first, 0, isBackEdge1); // true data
            selNode->addInputNode(pp2.first, 1, isBackEdge2); // false data
            selNode->addInputNode(pp1.second, 2); // condition
            pp1.first->addOutputNode(selNode, isBackEdge1); 
            pp2.first->addOutputNode(selNode, isBackEdge2);
            pp1.second->addOutputNode(selNode);
            addEdge(pp1.first, selNode, EDGE_TYPE_DATA);
            addEdge(pp2.first, selNode, EDGE_TYPE_DATA);
            addEdge(pp1.second, selNode, EDGE_TYPE_CTRL);
            if(i+2 == phiParents.size()){ // last pair, don not crete new conditional node
                // add new parent to vector
                phiParents.push_back(std::make_pair(selNode, (LLVMCDFGNode *)NULL));
            }else{
                // create new conditional node: CTRLOR (cond1 | cond2)
                LLVMCDFGNode *newCondNode = addNode("CTRLOR", node->BB());
                outs() << "new CTRLOR node = " << newCondNode->getName() << "\n";
                newCondNode->addInputNode(pp1.second, 0);
                newCondNode->addInputNode(pp2.second, 1);
                pp1.second->addOutputNode(newCondNode);
                pp2.second->addOutputNode(newCondNode);
                addEdge(pp1.second, newCondNode, EDGE_TYPE_CTRL);
                addEdge(pp2.second, newCondNode, EDGE_TYPE_CTRL);
                // add new parent to vector
                phiParents.push_back(std::make_pair(selNode, newCondNode));
            }
        }
        // connect last node to the successor nodes of the phi node
        LLVMCDFGNode *lastNode = phiParents.rbegin()->first;
        for(auto succ : node->outputNodes()){
            int idx = succ->delInputNode(node);
            lastNode->addOutputNode(succ);
            succ->addInputNode(lastNode, idx);
            addEdge(lastNode, succ, EDGE_TYPE_DATA);
        }
        outs() << "remove PHI node = " << node->getName() << "\n";
        delNode(node);
    }
}


// add mask AND node behind the Shl node with bytewidth less than MAX_DATA_BYTES
void LLVMCDFG::addMaskAndNodes()
{
    std::vector<LLVMCDFGNode*> shlNodes;
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto ins = node->instruction();
        if(ins && ins->getOpcode() == Instruction::Shl){
            shlNodes.push_back(node);
        }
    }
    for(auto node : shlNodes){
        auto ins = node->instruction();
        BasicBlock *BB = node->BB();
        int bytes = ins->getType()->getIntegerBitWidth() / 8;
        if(bytes < MAX_DATA_BYTES){
            LLVMCDFGNode *andNode = addNode("AND", BB);
            DataType maskVal = (1 << (8 * bytes)) - 1;
            LLVMCDFGNode *constNode = addNode("CONST", BB);
            constNode->setConstVal(maskVal);
            andNode->addInputNode(node, 0);
            andNode->addInputNode(constNode, 1);            
            constNode->addOutputNode(andNode);
            auto outNodes = node->outputNodes();
            for(auto outNode : outNodes){
                node->delOutputNode(outNode);
                int idx = outNode->delInputNode(node);
                delEdge(edge(node, outNode));
                andNode->addOutputNode(outNode);
                outNode->addInputNode(andNode, idx);
                addEdge(andNode, outNode, EDGE_TYPE_DATA);
            }
        }        
    }
}


// get the offset of each element in the struct
std::map<int, int> LLVMCDFG::getStructElemOffsetMap(StructType *ST)
{
    std::map<int, int> elemOffsetMap;
    int offset = 0;
    int idx = 0;
    for(Type *type : ST->elements()){
        elemOffsetMap[idx] = offset;
        offset += DL->getTypeAllocSize(type);
        idx++;
    }
    return elemOffsetMap;
}


// transfer GetElementPtr(GEP) node to MUL/ADD/Const tree
void LLVMCDFG::handleGEPNodes()
{
    std::vector<LLVMCDFGNode*> GEPNodes;
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto ins = node->instruction();
        if(ins && dyn_cast<GetElementPtrInst>(ins)){
            GEPNodes.push_back(node);            
        }
    }
    for(auto node : GEPNodes){
        auto ins = node->instruction();
        GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(ins);
        setGEPInfo(node, GEP->getPointerOperand()->getName());
        Type *currType = GEP->getSourceElementType();
        std::vector<std::pair<Value*, int>> varOperandSizes; // <non-constant-operand, element-size>
        int offset = 0;
        int NumOperands = ins->getNumOperands();
        if(NumOperands == 1){ // only one pointer, no index, so GEP should be a Const node
            // base address, should be provided by the scheduler that allocate the address space for the memory
            outs() << "GEP has only one pointer: constant base address\n";
        }else{ // have indexes, get the operand
            for(int i = 1; i < NumOperands; i++){
                outs() << "Operand " << i << ": ";
                Value *V = ins->getOperand(i);
                V->dump();
                outs() << "currType: ";
                currType->dump();
                // check if this operand is constant
                bool isConst = false;
                int constVal;
                // Instruction *I;
                if(ConstantInt *constIns = dyn_cast<ConstantInt>(V)){
                    isConst = true;
                    constVal = constIns->getSExtValue();
                    outs() << "Const: " << constVal << ", ";
                }else{
                    // I = dyn_cast<Instruction>(V);
                    outs() << "Variable, ";
                }
                // get the type and elemnt size of this operand
                int elemSize = 0;
                if(i == 1){ // source element index
                    elemSize = DL->getTypeAllocSize(currType);
                    if(isConst){
                        offset += constVal * elemSize;
                    }                   
                }else if(StructType *ST = dyn_cast<StructType>(currType)){
                    std::map<int, int> elemOffsetMap = getStructElemOffsetMap(ST);
                    assert(isConst); // the operand should be constant
                    offset += elemOffsetMap[constVal];
                    currType = ST->getElementType(constVal);
                    outs() << "StructType, ";
                }else if(ArrayType *AT = dyn_cast<ArrayType>(currType)){                    
                    currType = AT->getElementType();
                    elemSize = DL->getTypeAllocSize(currType);
                    outs() << "ArrayType, ";
                    if(isConst){
                        offset += constVal * elemSize;
                    }
                }else{
                    outs() << "Other type\n";
                    assert(false);
                }
                outs() << "\n";
                if(!isConst){
                    varOperandSizes.push_back(std::make_pair(V, elemSize));
                }
            }
        }
        // construct MUL/ADD/Const tree
        std::vector<LLVMCDFGNode*> newNodes;
        // create MUL nodes
        for(auto &elem : varOperandSizes){
            Instruction *ins = dyn_cast<Instruction>(elem.first);
            LLVMCDFGNode *preNode = this->node(ins); // predecessor node
            if(preNode == NULL){ // out of loop node
                preNode = _ioNodeMap[elem.first];
                outs() << "Out of loop ";
            }
            // elem.first->dump();
            outs() << "preNode: " << preNode->getName() << "\n";
            // delete old connection
            preNode->delOutputNode(node);
            node->delInputNode(preNode);
            delEdge(edge(preNode, node));
            // create new nodes and edges
            auto mulNode = addNode("MUL", node->BB());
            auto constNode = addNode("CONST", node->BB());
            constNode->setConstVal(elem.second);
            preNode->addOutputNode(mulNode);
            mulNode->addInputNode(preNode, 0);
            constNode->addOutputNode(mulNode);
            mulNode->addInputNode(constNode, 1);
            addEdge(preNode, mulNode, EDGE_TYPE_DATA);
            addEdge(constNode, mulNode, EDGE_TYPE_DATA);
            newNodes.push_back(mulNode);
        }
        // create ADD nodes
        for(int i = 0; i + 1 < newNodes.size(); i += 2){
            auto n1 = newNodes[i];
            auto n2 = newNodes[i+1];
            auto newNode = addNode("ADD", node->BB());
            n1->addOutputNode(newNode);
            n2->addOutputNode(newNode);
            newNode->addInputNode(n1, 0);
            newNode->addInputNode(n2, 1);
            addEdge(n1, newNode, EDGE_TYPE_DATA);
            addEdge(n2, newNode, EDGE_TYPE_DATA);
            newNodes.push_back(newNode);
        }
        // connect the last node to GEP node and set constant
        outs() << "Total offset: " << offset << "\n";
        node->setConstVal(offset);
        if(!newNodes.empty()){
            auto lastNewNode = *(newNodes.rbegin());
            lastNewNode->addOutputNode(node);
            node->addInputNode(lastNewNode, 0);
            addEdge(lastNewNode, node, EDGE_TYPE_DATA);
        }
        // the flollowing solution will delete GEP node
        // // connect last offset constant
        // LLVMCDFGNode *gepConstNode = addNode("GEPCONST", node->BB());
        // gepConstNode->setConstVal(offset);
        // outs() << "Total offset: " << offset << "\n";
        // LLVMCDFGNode *finalNode;
        // if(newNodes.empty()){
        //     finalNode = gepConstNode;
        // }else{
        //     auto finalNode = addNode("ADD", node->BB());
        //     auto lastNewNode = *(newNodes.rbegin());
        //     lastNewNode->addOutputNode(finalNode);
        //     gepConstNode->addOutputNode(finalNode);
        //     finalNode->addInputNode(lastNewNode);
        //     finalNode->addInputNode(gepConstNode);
        //     addEdge(lastNewNode, finalNode, EDGE_TYPE_DATA);
        //     addEdge(gepConstNode, finalNode, EDGE_TYPE_DATA);
        // }
        // // connect final node to the successors of GEP node
        // for(auto outNode : node->outputNodes()){
        //     finalNode->addOutputNode(outNode);
        //     outNode->addInputNode(finalNode);
        //     addEdge(finalNode, outNode, EDGE_TYPE_DATA);
        // }
        // delNode(node);
    }
}


// add loop exit nodes
void LLVMCDFG::addLoopExitNodes()
{
    for(auto &elem : _loopexitBBs){
        BasicBlock *srcBB = elem.first;
        BasicBlock *dstBB = elem.second;        
        LLVMCDFGNode *ctrlNode;
        bool isTrueCond;
        BranchInst *BRI = cast<BranchInst>(srcBB->getTerminator());
        if(BRI->isConditional()){
            ctrlNode = node(dyn_cast<Instruction>(BRI->getCondition()));
            // check true/false condition
            for(int i = 0; i < BRI->getNumSuccessors(); i++){
                if(dstBB == BRI->getSuccessor(i)){
                    isTrueCond = (i == 0);
                    break;
                }
            }
        }else{ // find the control node in the dominating BB
            BasicBlock *domBB = DT->getNode(srcBB)->getIDom()->getBlock();
            BranchInst *domBRI = cast<BranchInst>(domBB->getTerminator());
            assert(domBRI->isConditional());
            ctrlNode = node(dyn_cast<Instruction>(domBRI->getCondition()));
            // check true/false condition
            for(int i = 0; i < domBRI->getNumSuccessors(); i++){
                if(DT->dominates(domBRI->getSuccessor(i), srcBB)){
                    isTrueCond = (i == 0);
                    break;
                }
            }
        }        
        // create LOOPEXIT node and connect ctrlNode to it
        auto exitNode = getLoopExitNode(srcBB);
        if(!isTrueCond){
            // find the CTRLNOT node
            for(auto outNode : ctrlNode->outputNodes()){
                if(outNode->customInstruction() == "CTRLNOT"){
                    ctrlNode = outNode;
                    break;
                }
            }
        }
        outs() << "Control node: " << ctrlNode->getName();
        outs() << ", exit node: " << exitNode->getName() << "\n";
        exitNode->addInputNode(ctrlNode, 0);
        ctrlNode->addOutputNode(exitNode);
        addEdge(ctrlNode, exitNode, EDGE_TYPE_CTRL);
    }
}


// remove redundant nodes, e.g. Branch
void LLVMCDFG::removeRedundantNodes()
{
    // remove redundant nodes iteratively until no node to be remove
    bool removed = true;
    while(removed){
        std::vector<LLVMCDFGNode*> rmNodes;
        for(auto &elem : _nodes){
            auto node = elem.second;
            Instruction *ins = node->instruction();
            std::string customIns = node->customInstruction();
            if(node->inputNodes().empty()){ // no input node
                if(customIns != "CONST" && customIns != "LOOPSTART" && customIns != "INPUT" && !node->isLSaffine()){
                    rmNodes.push_back(node);
                }
            }else if(node->outputNodes().empty()){ // no output node
                if(!(ins && dyn_cast<StoreInst>(ins)) && customIns != "LOOPEXIT" && customIns != "OUTPUT" && !node->isLSaffine()){
                    rmNodes.push_back(node);
                }
            }
        }
        removed = !rmNodes.empty();
        outs() << "remove node: ";
        for(auto node : rmNodes){
            outs() << node->getName() << ", ";
            delNode(node);            
        }
        outs() << "\n";
    }
}


// assign final node name
void LLVMCDFG::assignFinalNodeName()
{
    for(auto &elem : _nodes){
        auto node = elem.second;
        Instruction *ins = node->instruction();
        std::string customIns = node->customInstruction();
        if(ins){
            switch (ins->getOpcode()){
            case Instruction::Add:
                node->setFinalInstruction("ADD");
                break;
            case Instruction::FAdd:
                node->setFinalInstruction("FADD");
                break;
            case Instruction::Sub:
                node->setFinalInstruction("SUB");
                break;
            case Instruction::FSub:
                node->setFinalInstruction("FSUB");
                break;
            case Instruction::Mul:
                node->setFinalInstruction("MUL");
                break;
            case Instruction::FMul:
                node->setFinalInstruction("FMUL");
                break;
            case Instruction::UDiv:
            case Instruction::SDiv:
            case Instruction::FDiv:
                // errs() << "Do not support Div Instructions\n";
                // assert(false);
                // break;
                node->setFinalInstruction("DIV");
                break;
            case Instruction::URem:
            case Instruction::SRem:
            case Instruction::FRem:
                errs() << "Do not support Rem Instructions\n";
                assert(false);
                break;
            case Instruction::Shl:
                node->setFinalInstruction("SHL");
                break;
            case Instruction::LShr:
                node->setFinalInstruction("LSHR");
                break;
            case Instruction::AShr:
                node->setFinalInstruction("ASHR");
                break;
            case Instruction::And:
                node->setFinalInstruction("AND");
                break;
            case Instruction::Or:
                node->setFinalInstruction("OR");
                break;
            case Instruction::Xor:
                node->setFinalInstruction("XOR");
                break;
            case Instruction::Load:
                if(node->isLSaffine())
                    node->setFinalInstruction("Input");
                else
                    node->setFinalInstruction("LOAD");
                break;
            case Instruction::Store:
                if(node->isLSaffine())
                    node->setFinalInstruction("Output");
                else
                    node->setFinalInstruction("STORE");
                break;
            case Instruction::GetElementPtr:
                if(node->inputNodes().empty()){
                    node->setFinalInstruction("CONST");
                }else{
                    node->setFinalInstruction("ADD");
                }                
                break;
            case Instruction::Trunc:{
                TruncInst *TI = dyn_cast<TruncInst>(ins);
                auto bitWidth = TI->getDestTy()->getIntegerBitWidth();
                DataType mask = (DataType)(1 << bitWidth) - 1;
                node->setConstVal(mask);
                node->setFinalInstruction("AND");             
                break;    
            }        
            case Instruction::SExt:{
                SExtInst *SI = dyn_cast<SExtInst>(ins);
                auto srcBitWidth = SI->getSrcTy()->getIntegerBitWidth();
                auto dstBitWidth = SI->getDestTy()->getIntegerBitWidth();
                DataType constVal = (DataType)((dstBitWidth << 8) | srcBitWidth);
                node->setConstVal(constVal);
                node->setFinalInstruction("SEXT");         
                break;       
            }    
            case Instruction::ZExt:{
                node->setConstVal(0);
                node->setFinalInstruction("OR");         
                break;
            }
            case Instruction::ICmp:{
                CmpInst *CI = dyn_cast<CmpInst>(ins);
                switch(CI->getPredicate()){
                case CmpInst::ICMP_EQ:
                    node->setFinalInstruction("EQ");
                    break;
                case CmpInst::ICMP_NE:
                    node->setFinalInstruction("NE");
                    break;
                case CmpInst::ICMP_SGE:
                    node->setFinalInstruction("SGE");
                    break;
                case CmpInst::ICMP_UGE:
                    node->setFinalInstruction("UGE");
                    break;
                case CmpInst::ICMP_SGT:
                    node->setFinalInstruction("SGT");
                    break;
                case CmpInst::ICMP_UGT:
                    node->setFinalInstruction("UGT");
                    break;
                case CmpInst::ICMP_SLE:
                    node->setFinalInstruction("SLE");
                    break;
                case CmpInst::ICMP_ULE:
                    node->setFinalInstruction("ULE");
                    break;
                case CmpInst::ICMP_SLT:
                    node->setFinalInstruction("SLT");
                    break;
                case CmpInst::ICMP_ULT:
                    node->setFinalInstruction("ULT");
                    break;
                default:
                    assert(false);
                    break;
                }
                break;
            }
            case Instruction::Select:
                node->setFinalInstruction("SELECT");
                break;
            case Instruction::PHI:
                //node->setFinalInstruction("SELECT");
                break;
            default:
                assert(false);
                break;
            }
        }else{
            node->setFinalInstruction(customIns);
        }
    }
}

///to cast constant value to Int
int castConstInt(Constant* V){
    int integer;
    if (ConstantInt *CI = dyn_cast<ConstantInt>(V))
    {
        integer = CI->getSExtValue();
    }
    else if (ConstantFP *FP = dyn_cast<ConstantFP>(V))
    {
        integer = (int)FP->getValueAPF().convertToFloat();
    }
    else{
        assert(0&&"fail in casting const value to Int");
    }
    return integer;
}

///to get the constant operand of a BinaryOperation(first:OperandIndex; second:IntValue)
std::pair<int, int> BinaryConstOp(Instruction *Bi)
{
    int OperandIndex;
    int constOp;
    assert(Bi && (Bi->getNumOperands() == 2));
    if (auto V = dyn_cast<Constant>(Bi->getOperand(0)))
    {
        constOp = castConstInt(V);
        OperandIndex = 0;
        //errs() << "\t" << "Instructions: " << Bi->getName().str() << "\n";
    }
    else if (auto V = dyn_cast<Constant>(Bi->getOperand(1)))
    {
        constOp = castConstInt(V);
        OperandIndex = 1;
        //errs() << "\t" << "Instructions: " << Bi->getName().str() << "\n";
    }else{
        return std::make_pair(NULL, NULL);
        assert(0&&"do not have Constant");
    }
    return std::make_pair(OperandIndex, constOp);
}

LLVMCDFGNode* LLVMCDFG::addNodeTree(Value* opnode){
    if(auto CT = dyn_cast<Constant>(opnode)){
        LLVMCDFGNode* newnode = addNode("CONST");
        newnode->setConstVal(castConstInt(CT));
        return newnode;
    //PHI will be handled in arraystride()
    }else if(auto Phi = dyn_cast<PHINode>(opnode)){
        //LLVMCDFGNode *newnode = addNode("SELECT", Phi->getParent());
        return node(Phi);
    }else if(auto GEP = dyn_cast<GetElementPtrInst>(opnode)){
        LLVMCDFGNode *newnode = addNode(GEP);
        int dimension = GEP->getNumIndices();
        auto teminputs = newnode->inputNodes();
        for(auto prenode : teminputs){
            if(prenode->customInstruction() == "INPUT"){
                delNode(prenode);
            }
        }
        for(int i = 2; i <= dimension; i++){
            auto operand = GEP->getOperand(i);
            LLVMCDFGNode* prenode;
            if(auto Phi = dyn_cast<PHINode>(operand)){//PHI direct connnect to GEP handle here
                auto inputs = newnode->inputNodes();
                prenode = addIdxCycle(Phi);
                if(prenode->customInstruction().empty())///innermost phi
                    continue;
            }else if(dyn_cast<Constant>(operand)){//handled in handleGEP
                continue;
            }else{
                prenode = addNodeTree(operand);
            }
            if(newnode->getInputIdx(prenode) == i)//have been added
                continue;
            prenode->addOutputNode(newnode);
            newnode->addInputNode(prenode);
            newnode->setInputIdx(prenode, i);
            addEdge(prenode, newnode, EDGE_TYPE_DATA);
        }
        return newnode;
    }else if (auto ins = dyn_cast<Instruction>(opnode))
    {
        LLVMCDFGNode *newnode = node(ins);
        if(newnode != NULL)
            return newnode;
        else
            newnode = addNode(ins);
        for(int i = 0; i < ins->getNumOperands(); i++){
            auto operand = ins->getOperand(i);
            if(dyn_cast<PHINode>(operand))//get PHI operand.deal in arraystride
                continue;
            LLVMCDFGNode* prenode = addNodeTree(operand);
            prenode->addOutputNode(newnode);
            newnode->addInputNode(prenode);
            newnode->setInputIdx(prenode, i);
            addEdge(prenode, newnode, EDGE_TYPE_DATA);
        }
        return newnode;
    }else{
        return NULL;
    }
}

std::set<std::pair<LLVMCDFGNode*, int>> LLVMCDFG::addOutputTree(Value* user){
    std::set<std::pair<LLVMCDFGNode*, int>> resultset;
    auto ins = dyn_cast<Instruction>(user);
    for(auto succ : user->users()){
        if(auto succIns = dyn_cast<Instruction>(succ)){
            if(_loopBBs.count(succIns->getParent())){
                continue;
            }else if(auto Phi = dyn_cast<PHINode>(succIns)){//single PHI is just pass
                if(Phi->getNumOperands() != 1)///only consider lcssa new PHI:TODO ?
                    continue;
                errs() << "find phi:";Phi->dump();
                auto outputset = addOutputTree(Phi);
                resultset.insert(outputset.begin(), outputset.end());
            }else if(auto Store = dyn_cast<StoreInst>(succIns)){
                errs() << "find store:";Store->dump();
                int idx;
                for(idx = 0; idx<Store->getNumOperands(); idx++){
                    if(auto operandins = dyn_cast<Instruction>(Store->getOperand(idx))){
                        {if(operandins == ins)
                            break;
                        }
                    }
                }
                resultset.insert(std::make_pair(addNode(Store), idx));
            }else if(dyn_cast<BranchInst>(succIns)||dyn_cast<ReturnInst>(succIns)){
                succIns->dump();///TODO:can not handle like:ret...
                continue;
            }else{
                succIns->dump();
                auto detectedflag = (node(succIns) != NULL);
                auto succnode = addNode(succIns);
                int idx = -1;
                for(int i = 0; i < succIns->getNumOperands(); i++){
                    if(auto operandins = dyn_cast<Instruction>(succIns->getOperand(i))){
                        if(operandins == ins){
                            idx = i;
                            continue;
                        }else if(dyn_cast<PHINode>(operandins)){
                            continue;
                        }else if(operandins != ins && node(operandins) == NULL){
                            auto opnode = addNodeTree(operandins);
                            opnode->addOutputNode(succnode);
                            succnode->addInputNode(opnode);
                            addEdge(opnode, succnode, EDGE_TYPE_DATA);
                            succnode->setInputIdx(opnode, i);
                        }
                    }
                    else if(dyn_cast<Constant>(succIns->getOperand(i))){
                        if(!detectedflag){
                            auto opnode = addNodeTree(succIns->getOperand(i));
                            opnode->addOutputNode(succnode);
                            succnode->addInputNode(opnode);
                            addEdge(opnode, succnode, EDGE_TYPE_DATA);
                            succnode->setInputIdx(opnode, i);
                        }
                    }else{
                        assert(0&&"what's wrong");
                    }
                }
                resultset.insert(std::make_pair(succnode, idx));
                auto outputset = addOutputTree(succIns);
                for(auto temoutput:outputset){
                    Instruction* temoutputins = temoutput.first->instruction();
                    int idx=-1;
                    for(auto i = 0; i<temoutputins->getNumOperands(); i++){
                        if(temoutputins->getOperand(i) == succIns){
                            idx = i;
                            break;
                        }
                    }
                    assert(idx>=0);
                    if(edge(succnode, temoutput.first)==NULL || temoutput.first->getInputIdx(succnode) != idx){
                        succnode->addOutputNode(temoutput.first);
                        temoutput.first->addInputNode(succnode);
                        addEdge(succnode, temoutput.first, EDGE_TYPE_DATA);
                        temoutput.first->setInputIdx(succnode, idx);
                    }
                }
            }
        }else{
            succ->dump();
            assert(0&&"come assross what in add OutputTree");
            return resultset;
        }
    }
    return resultset;
}

LLVMCDFGNode* LLVMCDFG::addIdxCycle(PHINode* Phi){
    if(Phi->getNumIncomingValues() > 2){
        assert(0&&"what phi =_=");
    }
    auto exist = node(Phi);
    if(exist != NULL)
        return exist;
    LLVMCDFGNode* SELECT = addNode(Phi);
    SELECT->setCustomInstruction("SELECT");
    for(int i = 0; i < Phi->getNumIncomingValues(); i++){
        BasicBlock* bb = Phi->getIncomingBlock(i);
		Value* V = Phi->getIncomingValue(i);
		outs() << "IncomingValue :: "; V->dump();
        if(auto C = dyn_cast<Constant>(V)){///TODO:very puny constrain
            LLVMCDFGNode* phiTree = addNodeTree(V);
            SELECT->addInputNode(phiTree);
            SELECT->setInputIdx(phiTree, 0);
            phiTree->addOutputNode(SELECT);
            addEdge(phiTree, SELECT, EDGE_TYPE_DATA);
        }else if(auto ins = dyn_cast<Instruction>(V)){
            LLVMCDFGNode* phiTree = node(ins);
            if(phiTree == NULL)
                phiTree = addNodeTree(V);
            SELECT->addInputNode(phiTree);
            SELECT->setInputIdx(phiTree, 0);
            phiTree->addOutputNode(SELECT);
            addEdge(phiTree, SELECT, EDGE_TYPE_DATA);
            addEdge(SELECT, phiTree, EDGE_TYPE_DATA);
            phiTree->setOutputBackEdge(SELECT, true);
            
        }
    }
    return SELECT;
}

///check this instruction whether has Input recursively(until PHI or Const node)
void LLVMCDFG::checkInsInputs(Instruction* temins){
    LLVMCDFGNode *temnode = node(temins);
    bool hasSELECT = false;//is input has been converted to SELECT
    auto teminputs = temnode->inputNodes();
    for(auto INPUT:teminputs){
        if(INPUT->customInstruction() == "INPUT"){
            temnode->delInputNode(INPUT);
            delNode(INPUT);
        }
        if(INPUT->customInstruction() == "SELECT"){//SELECT comes from outer PHI, so is only
            hasSELECT = true;
        }
    }
    for (int i = 0; i < temins->getNumOperands(); i++)
    {
        auto preopnode = temins->getOperand(i);
        if (dyn_cast<Constant>(preopnode))
        {
            if (temnode == NULL)
            {
                temnode = addNode(temins);
                LLVMCDFGNode *prenode = addNodeTree(preopnode);
                temnode->addInputNode(prenode);
                temnode->setInputIdx(prenode, i);
                prenode->addOutputNode(temnode);
                addEdge(prenode, temnode, EDGE_TYPE_DATA);
            }
        }
        else if (auto preins = dyn_cast<Instruction>(preopnode))
        { // operand is an instruction(including PHI)
            if (temnode != NULL)
            {
                LLVMCDFGNode *prenode;
                if (auto Phi = dyn_cast<PHINode>(preins))
                {   
                    if(!hasSELECT){ 
                        prenode = addIdxCycle(Phi);//TODO:may should convert SELECT to ACC here?
                        if(temnode->getInputIdx(prenode) == i)//this edge is here
                            continue;
                        temnode->addInputNode(prenode);
                        temnode->setInputIdx(prenode, i);
                        prenode->addOutputNode(temnode);
                        addEdge(prenode, temnode, EDGE_TYPE_DATA);
                    }
                }
                else{
                    prenode = node(preins);
                    if(prenode == NULL)
                        prenode = addNodeTree(preopnode);
                    if(temnode->getInputIdx(prenode) == i)//this edge is here
                        continue;
                    else{
                        temnode->addInputNode(prenode);
                        temnode->setInputIdx(prenode, i);
                        prenode->addOutputNode(temnode);
                        addEdge(prenode, temnode, EDGE_TYPE_DATA);
                    }
                }
            }else if(temnode == NULL){
                assert(0&&"what? you get a outer ins to check input");
            }
        }
    }
}

///recursively find array assess stride & bound
std::vector<PHINode*> LLVMCDFG::arrayStride(Value* opnode, std::map<int, std::pair<double,std::pair<double,double>>>* factortable){
    
    //if return {} && factortable->clear() means non-affine operation
    if(factortable == NULL || factortable->empty()){
        if(dyn_cast<PHINode>(opnode)||dyn_cast<Constant>(opnode)){
            return {};
        }else if(auto ins = dyn_cast<Instruction>(opnode)){
            LLVMCDFGNode* temnode = node(ins);
            checkInsInputs(ins);
            for(int i = 0; i < ins->getNumOperands(); i++){
                auto preopnode = ins->getOperand(i);
                arrayStride(preopnode, NULL);
            }
        }
        factortable->clear();
        return {};
    }
    if(dyn_cast<Constant>(opnode)){
        ///mul constant will be covered in following
        errs() << "\t\thas a constant address\n";
        return {};
    }else if(auto phi = dyn_cast<PHINode>(opnode)){
        std::vector<llvm::PHINode *> singlephi;
        int level = PHItoLevel[phi];
        singlephi.push_back(phi);
        if((*factortable)[level].first == 0) (*factortable)[level].first = getLoopsAffineStrides(level);
        return singlephi;
    }else if(dyn_cast<StoreInst>(opnode)||dyn_cast<LoadInst>(opnode)){
        errs() << "\t\thas a read address\n";
        factortable->clear();
        return {};
    }else{
        Instruction* temins = dyn_cast<Instruction>(opnode);
        //errs()<<"\tcome accross ins:";temins->dump();
        int temopcode = temins->getOpcode();
        if(temins->isBinaryOp()){
            std::pair<int, int> OperandIndextoConst = BinaryConstOp(temins);
            checkInsInputs(temins);
            switch (temopcode)
            {
            case Instruction::Mul :{
                if(OperandIndextoConst.second == NULL){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }else{
                    auto innerphis = arrayStride(temins->getOperand(1-OperandIndextoConst.first), factortable);
                    for(auto phi : innerphis){
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = getLoopsAffineStrides(level);
                        (*factortable)[level].first *= OperandIndextoConst.second;
                        (*factortable)[level].second.first *= OperandIndextoConst.second;
                        (*factortable)[level].second.second *= OperandIndextoConst.second;
                    }
                    return innerphis;
                }
            }
            case Instruction::UDiv:
            case Instruction::SDiv:
            case Instruction::FDiv:{
                if(OperandIndextoConst.second == NULL){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }if(OperandIndextoConst.first == 0){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }else{
                    auto innerphis = arrayStride(temins->getOperand(0), factortable);
                    for(auto phi : innerphis){
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = getLoopsAffineStrides(level);
                        (*factortable)[level].first /= OperandIndextoConst.second;
                        (*factortable)[level].second.first /= OperandIndextoConst.second;
                        (*factortable)[level].second.second /= OperandIndextoConst.second;                        
                    }
                    return innerphis;
                }
            }
            ///TODO:div & Lshl may be unaffine when not divisible
            case Instruction::Shl :{
                if(OperandIndextoConst.second == NULL){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }if(OperandIndextoConst.first == 0){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }else{
                    auto innerphis = arrayStride(temins->getOperand(0), factortable);
                    for(auto phi : innerphis){
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = getLoopsAffineStrides(level);
                        //errs()<< "shl: " << (int)(*factortable)[level].first<<"<<"<<OperandIndextoConst.second<<"!!!!!!\n";
                        (*factortable)[level].first = (int)(*factortable)[level].first << OperandIndextoConst.second;
                        //errs()<<"result is: "<< (int)(*factortable)[level].first<<"!!!!\n";
                        (*factortable)[level].second.first = (int)(*factortable)[level].second.first << OperandIndextoConst.second;
                        (*factortable)[level].second.second = (int)(*factortable)[level].second.second << OperandIndextoConst.second;
                    }
                    return innerphis;
                }
            }
            case Instruction::LShr :{
                if(OperandIndextoConst.second == NULL){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }if(OperandIndextoConst.first == 0){
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }else{
                    auto innerphis = arrayStride(temins->getOperand(0), factortable);
                    for(auto phi : innerphis){
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = getLoopsAffineStrides(level);
                        (*factortable)[level].first /= pow(2, OperandIndextoConst.second);
                        (*factortable)[level].second.first /= pow(2, OperandIndextoConst.second);
                        (*factortable)[level].second.second /= pow(2, OperandIndextoConst.second);
                    }
                    return innerphis;
                }
            }
            case Instruction::Add :{
                if(OperandIndextoConst.second == NULL){
                    auto temphis0 = arrayStride(temins->getOperand(0), factortable);
                    auto temphis1 = arrayStride(temins->getOperand(1), factortable);
                    temphis1.insert(temphis1.end(), temphis0.begin(), temphis0.end());///TODO:10*(i+i) ???
                    return temphis1;
                }else{
                    auto innerphis = arrayStride(temins->getOperand(1-OperandIndextoConst.first), factortable);
                    auto phi = *innerphis.begin();//only add once
                    int level = PHItoLevel[phi];
                    (*factortable)[level].second.first += OperandIndextoConst.second;
                    (*factortable)[level].second.second += OperandIndextoConst.second;
                    
                    return innerphis;
                }
            }
            case Instruction::Or :{
                if(OperandIndextoConst.second == NULL){//unkonw condition
                    assert(0&&"unknow condition about Or operation");
                    factortable->clear();
                    arrayStride(temins->getOperand(0), NULL);
                    arrayStride(temins->getOperand(1), NULL);
                    return {};
                }else{
                    auto innerphis = arrayStride(temins->getOperand(1-OperandIndextoConst.first), factortable);
                    for(auto phi : innerphis){//TODO:any else condition besides add?
                        int level = PHItoLevel[phi];
                        (*factortable)[level].second.first += OperandIndextoConst.second;
                        (*factortable)[level].second.second += OperandIndextoConst.second;
                    }
                    return innerphis;
                }
            }
            case Instruction::Sub :{
                if(OperandIndextoConst.second == NULL){
                    auto temphis0 = arrayStride(temins->getOperand(0), factortable);
                    auto temphis1 = arrayStride(temins->getOperand(1), factortable);
                    for(auto phi : temphis1){///to Negate subtrahends
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = 1;
                        (*factortable)[level].first = -(*factortable)[level].first;
                    }
                    temphis1.insert(temphis1.end(), temphis0.begin(), temphis0.end());
                    return temphis1;
                }else if(OperandIndextoConst.first == 0){
                    auto temphis1 = arrayStride(temins->getOperand(1), factortable);
                    auto phi = *temphis1.begin();///to Negate subtrahends
                                                ///constant only once
                        int level = PHItoLevel[phi];
                        if((*factortable)[level].first == 0) (*factortable)[level].first = 1;
                        (*factortable)[level].first = -(*factortable)[level].first;
                        (*factortable)[level].second.first = OperandIndextoConst.second - (*factortable)[level].second.first;
                        (*factortable)[level].second.second =  OperandIndextoConst.second - (*factortable)[level].second.second;
                }else{
                    auto innerphis = arrayStride(temins->getOperand(0), factortable);
                    auto phi = *innerphis.begin();///constant only once
                        int level = PHItoLevel[phi];
                        (*factortable)[level].second.first -= OperandIndextoConst.second;
                        (*factortable)[level].second.second -= OperandIndextoConst.second;
                    return innerphis;
                }
            }
            default:{
                factortable->clear();
                return {};
                }
            }
        }else{///when will is not BinaryOp??!
            if(auto temins = dyn_cast<ZExtInst>(opnode)){
                errs()<<"finally across ZExt";temins->dump();
                auto innerphis = arrayStride(temins->getOperand(0), factortable);
                return innerphis;
            }else{
                assert(0&&"what ins?");
            }
        }
    }
}

/*
///to get the Branch of this loop 
BranchInst *LooptoBranch(std::set<llvm::BasicBlock *> detected, std::set<llvm::BasicBlock *> temNestBBs){
    for(auto BI = temNestBBs.begin(); BI != temNestBBs.end(); BI++){
        BasicBlock *temBB = *BI;
        if(detected.find(temBB) == detected.end()){
            for(auto II = temBB->begin(); II != temBB->end(); II++){
                Instruction *ins = &*II;
                if(auto br = dyn_cast<BranchInst>(ins)){
                    if(br->getNumOperands() == 3){
                        errs() << "yes: "<< br->getOperand(0)->getName().str() <<"\n";
                        return br;
                    }
                }
            }
        }
    }
    return NULL;
}
*/
PHINode* InstoPHI(Instruction* src){
    for (int i = 0; i < src->getNumOperands(); i++){
        if(auto ins = dyn_cast<Instruction>(src->getOperand(i))){
            if (auto Phi = dyn_cast<PHINode>(ins)){
                return Phi;
            }
            else{
                return InstoPHI(ins);
            }
        }
        else{
            continue;
        }
    }
    return NULL;
}

//nestloop affine Analyze
void LLVMCDFG::affineAnalyze(){
    _nestLoopisAffine = true;
    std::map<int, llvm::Loop *> nestloops = _nestloops;
    int total_level = nestloops.size();
    std::set<BasicBlock*> detected; //detected loops' BBs shouldn't be checked by outter loop
    std::map<int, int> strides;
    std::map<int, std::pair<int, int>> bounds;
    
//get every nests' idx stride & bound
    for(int level = 0; level < total_level; level++){
        ///get all basicblocks of this loop nest
        std::set<BasicBlock*> temNestBBs;
        temNestBBs.insert(nestloops[level]->getBlocks().begin(), nestloops[level]->getBlocks().end());
        Instruction* idx_gen_oprand;
        Value* indexbegin, *indexend;
        int stride,leftbound,rightbound;
        BasicBlock* exitBB = nestloops[level]->getExitBlock();
        Instruction* exitins = exitBB->getTerminator();
        auto branch = dyn_cast<BranchInst>(exitins);
        CmpInst* exitcmp;
        PHINode* idxPhi = NULL;
        //maybe outmost loop
        if(!temNestBBs.count(exitBB)){
            exitBB = exitBB->getSinglePredecessor();
            assert(exitBB&&"how does it exit?");
            exitins = exitBB->getTerminator();
            branch = dyn_cast<BranchInst>(exitins);
        }

        errs() << "nest: " << level << " have blocks: ";
        for(auto BI = temNestBBs.begin(); BI != temNestBBs.end(); BI++){
            BasicBlock *temBB = *BI;
            errs() << temBB->getName().str() << " ";
        }
        
        errs() << "\n"; 
        errs() << "exit block is: " << exitBB->getName().str() <<"\n";
        errs() << "exit branch is: "; branch->dump();
        //errs() << "Canonical Induction Variable is: ";nestloops[level]->getCanonicalInductionVariable()->dump();


        if(auto cmp = dyn_cast<CmpInst>(branch->getOperand(0))){
            exitcmp = cmp;
            errs() << "exit cmp is: "; exitcmp->dump();
        }

        idxPhi = InstoPHI(exitcmp);
/*///LLVM in-build function:PROBLEM
        //errs() << "getBoundFuntion: level step: "<< nestloops[level]->getBounds(*SE).getPointer()->getStepInst().getName().str() << "\n";
        if (auto x = (nestloops[level]->getBounds(*SE)))
        {   
            auto LoopBound = nestloops[level]->getBounds(*SE).getPointer();
            Instruction *idx_gen_oprand = &(LoopBound->getStepInst());
            errs() << "idx_gen_oprand name: "<< idx_gen_oprand->getName().str() <<"\n";
            errs() << ((LoopBound->getInitialIVValue()).getNumUses()) << "\n";
            errs() << ((&(LoopBound->getFinalIVValue())) == nullptr) << "\n";
            Value *tem = &(LoopBound->getInitialIVValue());
            auto tem2 = nestloops[level];
            //auto V = dyn_cast<Constant>(tem);
            if (auto V = dyn_cast<Constant>(idx_gen_oprand->getOperand(0)))
            {
                int stride;
                if (ConstantInt *CI = dyn_cast<ConstantInt>(V))
                {
                    stride = CI->getSExtValue();
                }
                else if (ConstantFP *FP = dyn_cast<ConstantFP>(V))
                {
                    stride = (int)FP->getValueAPF().convertToFloat();
                }
                errs() << "\t"
                       << "Instructions: " << idx_gen_oprand->getName().str() << "\n";
                strides[level] = stride;
            }
            else if (auto V = dyn_cast<ConstantInt>(idx_gen_oprand->getOperand(1)))
            {
                int stride;
                if (ConstantInt *CI = dyn_cast<ConstantInt>(V))
                {
                    stride = CI->getSExtValue();
                }
                else if (ConstantFP *FP = dyn_cast<ConstantFP>(V))
                {
                    stride = (int)FP->getValueAPF().convertToFloat();
                }
                errs() << "\t"
                       << "Instructions: " << idx_gen_oprand->getName().str() << "\n";
                strides[level] = stride;
            }
        }else{
            errs() << "\t" <<  " had non-affain operation\n";
            this->_nestLoopisAffine = false;
        }
*/

        for(auto BI = temNestBBs.begin(); BI != temNestBBs.end(); BI++){
            BasicBlock *temBB = *BI;
            if (detected.find(temBB) == detected.end())
            {
                for (auto II = temBB->begin(); II != temBB->end(); II++)
                {
                    Instruction *ins = &*II;
                    if (auto Phi = dyn_cast<PHINode>(ins))
                    {
                        if(Phi != idxPhi){
                            continue;
                        }
                        PHItoLevel[Phi] = level;
                        BasicBlock *PhiNext;
                        for (int i = 0; i < Phi->getNumOperands(); i++)
                        {
                            PhiNext = Phi->getIncomingBlock(i);
                            errs() << "\tPHINode: " << Phi->getName().str() << " getIncomingBlock(" << i << ") is: " << PhiNext->getName().str() << "\n";
                            if (temNestBBs.find(PhiNext) != temNestBBs.end())
                            {
                                idx_gen_oprand = dyn_cast<Instruction>(Phi->getIncomingValueForBlock(PhiNext)); // PROBLEM
                                if (dyn_cast<SExtInst>(idx_gen_oprand))
                                {
                                    idx_gen_oprand = dyn_cast<Instruction>(idx_gen_oprand->getOperand(0));
                                }
                                if (idx_gen_oprand->getNumOperands() != 2)
                                { // to avoid idx_gen_operand is somelike GEP
                                    continue;
                                }
                                bool affineflag = true;
                                int  stride = 0;
                                Value *op0, *op1;
                                while(op0 != Phi && op1 != Phi)
                                {
                                    if(idx_gen_oprand->getOpcode() != Instruction::Add && idx_gen_oprand->getOpcode() != Instruction::Sub){
                                        affineflag = false;
                                        break;
                                    }
                                    op0 = idx_gen_oprand->getOperand(0);
                                    op1 = idx_gen_oprand->getOperand(1);
                                    if (auto V = dyn_cast<Constant>(op0))
                                    {
                                        stride += castConstInt(V);
                                        errs() << "\t"
                                               << "Instructions: " << idx_gen_oprand->getName().str() << "\n";
                                        //strides[level] = stride;
                                        if(auto next = dyn_cast<Instruction>(idx_gen_oprand->getOperand(1)))
                                            idx_gen_oprand = next;
                                    }
                                    else if (auto V = dyn_cast<ConstantInt>(op1))
                                    {
                                        stride += castConstInt(V);
                                        errs() << "\t"
                                               << "Instructions: " << idx_gen_oprand->getName().str() << "\n";
                                        //strides[level] = stride;
                                        if(auto next = dyn_cast<Instruction>(idx_gen_oprand->getOperand(0)))
                                            idx_gen_oprand = next;
                                    }
                                    else
                                    { // index operation has non-constant oprand
                                        errs() << "\t" << Phi->getName().str() << " had affain operation but the Operand is not Const\n";
                                        this->_nestLoopisAffine = false;
                                        break;
                                    }
                                }
                                if(affineflag){
                                    strides[level] = stride;
                                }else{
                                    errs() << "\t" << Phi->getName().str() << " had non-affain operation\n";
                                    this->_nestLoopisAffine = false;
                                }
                                if (isloopsAffine())
                                {
                                    bool flag = true; // flag false means: tem phi control branch
                                    for (auto phiusers : Phi->users())
                                    {
                                        if (auto I = dyn_cast<Instruction>(phiusers))
                                        {
                                            // errs() << "PHI node users: " << I->getName().str() << "\n";
                                            if (auto cmp = dyn_cast<ICmpInst>(I))
                                            {
                                                if(cmp != exitcmp)
                                                    continue;
                                                errs() << "\tcmp node is: " << cmp->getName().str() << "\n";
                                                flag = false;
                                                bounds[level].second = BinaryConstOp(I).second + stride; 
                                            }
                                        }
                                    }
                                    if (flag)//phi do not control CMP directly
                                    {
                                        for (auto opusers : Phi->getIncomingValueForBlock(PhiNext)->users())
                                        {
                                            if (auto I = dyn_cast<Instruction>(opusers))
                                            {
                                                if (auto cmp = dyn_cast<ICmpInst>(I))
                                                {
                                                    if(cmp != exitcmp)
                                                        continue;
                                                    errs() << "\tcmp node is: " << cmp->getName().str() << "\n";
                                                    bounds[level].second = BinaryConstOp(I).second;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            { /// I want to get the Begin of affine idx, But can't confirm is this right...?
                                /// thought only two operands in PHINode
                                indexbegin = Phi->getIncomingValueForBlock(PhiNext);
                                if (auto V = dyn_cast<ConstantInt>(indexbegin))
                                {
                                    if (ConstantInt *CI = dyn_cast<ConstantInt>(V))
                                    {
                                        bounds[level].first = CI->getSExtValue();
                                    }
                                    else if (ConstantFP *FP = dyn_cast<ConstantFP>(V))
                                    {
                                        bounds[level].first = (int)FP->getValueAPF().convertToFloat();
                                    }
                                }
                                else
                                {
                                    errs() << "\t" << Phi->getName().str() << " had non certain begin index\n";
                                    this->_nestLoopisAffine = false;
                                }
                            }
                        }
                    }
                }
            }
        } 
        if (auto LoopBound = nestloops[level]->getBounds(*SE)){
            errs() << "\tdirection: " << int(LoopBound.getPointer()->getDirection()) <<"\n";
        }
        detected.insert(temNestBBs.begin(),temNestBBs.end());
    }
    // load stride information when loopsAffine is true
    // if loop is not affine--analyze END
    if(isloopsAffine()) setLoopsAffineStrides(strides);
    else return;
    
    //print loop index information
    for(int i = 0; i < getLoopsAffineStrides().size(); i++){
        int stride = strides[i];
        int left = bounds[i].first;
        int right = bounds[i].second;
        int count;
        count = (int)((right - left)/stride);
        bounds[i].second = count * stride + left;
        if(abs(stride)==1 || (right - left)%stride == 0)
            bounds[i].second -= stride;
        else
            count++;
        /*
        if(bounds[i].first > bounds[i].second){
            int tem = bounds[i].first;
            bounds[i].first = bounds[i].second;
            bounds[i].second = tem;
        }
        */
        errs() << "loop level " << i << " stride " << getLoopsAffineStrides()[i] << " per cycle\n";
        errs() << "\tleftbound is " << bounds[i].first << "\n";
        errs() << "\trightbound is " << bounds[i].second << "\n";
        setLoopsAffineBounds(i,bounds[i].first, bounds[i].second);
        setLoopsAffineCounts(i,count);
    }

    //find output node & check if store
    auto temnode = _nodes;
    for(auto &elem : temnode){
        auto node = elem.second;
        if(node->customInstruction() == "OUTPUT"){
            assert(node->inputNodes().size() == 1);
            auto result = *(node->inputNodes().begin());
            Instruction* resultins = result->instruction();
            assert(resultins != NULL);
            auto outputset = addOutputTree(result->instruction());//TODO: ret output maybe deleted
            errs() << "For output: "<<node->getName()<<"\n";
            for(auto output:outputset){
                output.first->instruction()->dump();
                result->addOutputNode(output.first);
                output.first->addInputNode(result);
                addEdge(result, output.first, EDGE_TYPE_DATA);
                Instruction* outins = output.first->instruction();
                assert(outins != NULL);
                output.first->setInputIdx(result, output.second);
            }
            errs()<< "\n";
            result->delOutputNode(node);///aviod access this node latter
            delNode(node);
        }else if(node->customInstruction() == "INPUT"){
            //auto tempoutput = node->outputNodes();
            for(auto output:node->outputNodes()){
                Instruction* ins = output->instruction();
                assert(ins != NULL);
                int idx = output->getInputIdx(node);
                auto operand = ins->getOperand(idx);
                auto newnode = addNodeTree(operand);
                if(newnode == NULL)
                    continue;
                newnode->addOutputNode(output);
                output->addInputNode(newnode);
                addEdge(newnode, output, EDGE_TYPE_DATA);
                output->setInputIdx(newnode, idx);
            }
            delNode(node);
        }
    }

    //find LSU & analyze their access pattern
    for(auto &elem : _nodes){
        auto node = elem.second;
        Instruction *ins = node->instruction();
        GetElementPtrInst *GEP;
        int startoffset = 0;
        bool isGEPaffine = true;
        std::map<int, int> LSstride;
        std::map<int, int[3]> LSbounds;
        int addressidx;
        //initial LSstride for each nest loop
        for(int i = 0; i < strides.size(); i++){
            LSstride[i] = 0;
        }
        if(ins == NULL){
            continue;
        }
        if(auto Load = dyn_cast<LoadInst>(ins)){
            GEP = dyn_cast<GetElementPtrInst>(Load->getPointerOperand());
            addressidx = Load->getPointerOperandIndex();
        }else if(auto Store = dyn_cast<StoreInst>(ins)){
            GEP = dyn_cast<GetElementPtrInst>(Store->getPointerOperand());
            addressidx = Store->getPointerOperandIndex();
        }else{
            continue;
        }
        if(GEP == NULL){
            node->setLSaddress(ins->getOperand(addressidx));
            continue;
        }
        errs()<<"come accross LSU: "<<node->getName()<<" instruction: ";ins->dump();
        int dimension = GEP->getNumIndices();
        if(!_loopBBs.count(GEP->getParent())){//GEP or its direct operand is outer
            auto teminputs = node->inputNodes();
            for(auto prenode: teminputs){
              if(prenode->customInstruction() == "INPUT"){
                  node->delInputNode(prenode);
                  delNode(prenode);
              }  
            }
            errs()<<"GEP:"<< GEP->getName().str()<< " is outGEP\n";
            LLVMCDFGNode* newnode;
            if(LLVMCDFG::node(GEP) == NULL)
                newnode = addNodeTree(dyn_cast<Value>(GEP));
            else
                newnode = LLVMCDFG::node(GEP);
            if(edge(newnode, node) == NULL){//have not been connected
                node->addInputNode(newnode);
                node->setInputIdx(newnode, addressidx);
                newnode->addOutputNode(node);
                addEdge(newnode, node, EDGE_TYPE_DATA);
            }
        }else{//for inner GEP BUT outter operand
            errs()<<"GEP:"<< GEP->getName().str()<< " is innerGEP\n";
            bool hasouterOP = false;
            for(int i = 2; i <= dimension; i++){
                if(auto ins = dyn_cast<Instruction>(GEP->getOperand(i))){
                    if(!_loopBBs.count(ins->getParent())){
                        hasouterOP = true;
                        errs() <<"add node "<<ins->getName().str()<<" which is the GEP's outer operand\n";
                    }else{
                        errs() << LLVMCDFG::node(ins)->getName()<<" is the GEP's inner operand\n";
                    }
                }
            }
            if(hasouterOP){
                addNodeTree(GEP);
            }
        }
        //initial LSbounds for each dimension
        for(int dim = 0; dim < (dimension-1); dim++){
            LSbounds[dim][0] = 0;
            LSbounds[dim][1] = 0;
            LSbounds[dim][2] = 0;
        }
        Type *currType = GEP->getSourceElementType();
        ///every dimension's scale
        std::map<int, int> arrayscale;
        for(int i = 2; i <= dimension; i++){
            ///TODO:maybe Struct Type GEP
            if(auto AT = dyn_cast<ArrayType>(currType)){
                currType = AT->getElementType();
                int elemSize = DL->getTypeAllocSize(currType);
                arrayscale[i] = elemSize;
                errs() << "dimension "<< i << " stride "<<elemSize<<" per step\n";
            }
        }
        
        bool allzerostride = true;
        errs() << "-------------\nhandle GEP: " << GEP->getName().str() << "\n";
        for(int temdi = dimension; temdi > 1; temdi--){
            /*
                here factortable's key is nestloop level; value is for this levelindex
                temporary dimension's stride & bounds: first element represent stride; second element represent boundspair
                for boundspair: first element represent leftbounds; second element represent rightbounds
            */
            std::map<int, std::pair<double, std::pair<double,double>>> factortable;
            errs() << "\tdimension "<< temdi <<" has transform: \n";
            Value *arrayindex = GEP->getOperand(temdi);
            
            ///factortable initialize is must!!
            for(int i = 0; i < strides.size(); i++){
                factortable[i].first = 0;
                factortable[i].second.first = getLoopsAffineBounds(i).first;
                factortable[i].second.second = getLoopsAffineBounds(i).second;
            }

            arrayStride(arrayindex, &factortable);
            
            if(factortable.empty()) {
                errs() << "\t\t[this dimension is not affine]\n";
                isGEPaffine = false;
                break;
            }else{
                for(int i = 0; i < factortable.size(); i++){
                    int stride, left, right;
                    stride = (int)factortable[i].first;
                    if(factortable[i].second.first > factortable[i].second.second){
                        right = factortable[i].second.first;
                        left = factortable[i].second.second;
                    }else{
                        left = factortable[i].second.first;
                        right = factortable[i].second.second;
                    }
                    LSstride[i] += (stride * arrayscale[temdi]);
                    if(stride){//if stride, this index is calculated in this dimension
                        allzerostride = false;
                        startoffset += (factortable[i].second.first * arrayscale[temdi]);
                        LSbounds[temdi-2][0] += left;
                        LSbounds[temdi-2][1] += right;
                    }
                    errs() <<"\t\tin this dimension level "<< i << " stride: " << (int)factortable[i].first << " ; leftbound: " <<\
                        (int)factortable[i].second.first << " ; rightbound: " << (int)factortable[i].second.second << "\n";
                }
                if(auto constant = dyn_cast<Constant>(arrayindex)){
                    int location =  castConstInt(constant);
                    LSbounds[temdi-2][0] = location;//save location for this constant dimension
                    LSbounds[temdi-2][1] = location;//save location for this constant dimension 
                }
                LSbounds[temdi-2][2] = arrayscale[temdi];
            }
        }
        if(allzerostride)
            startoffset = -1;///assign for totally constant GEP
        if(isGEPaffine){
            node->setLSstride(LSstride);
            node->setLSbounds(LSbounds);
            node->setLSstart(startoffset);
        }
        else{
            Type *currType = GEP->getSourceElementType();
            auto currTypesize = DL->getTypeAllocSize(currType);
            for(int i = 2; i <= dimension; i++){
                currTypesize = currTypesize/arrayscale[i];
                LSbounds[i-2][0] = 0;
                LSbounds[i-2][1] = currTypesize-1;
                LSbounds[i-2][2] = arrayscale[i];
            }
            node->setLSbounds(LSbounds);
        }
    }

}


// remove Br nodes, GEP nodes(when pattern)
void LLVMCDFG::handleAffineLSNodes(){
    bool allaffine = true;
    std::set<LLVMCDFGNode*> GEPsToDelnode, GEPsToDeltree;
    std::map<int, LLVMCDFGNode *> nodes = _nodes;
    for(auto &elem : nodes){
        auto node = elem.second;
        Instruction *ins = node->instruction();
        std::string customIns = node->customInstruction();
        if(node->isConditional()){ // is br nodes
            delNodeTree(node);// to delete BrNode & all of its parents nodes recursively
        }
        if((node->customInstruction() == "CTRLNOT") || (node->customInstruction() == "CTRLOR")){
            delNodeTree(node);
        }
        if(node->isLSaffine()){
            auto teminputs = node->inputNodes();
            for(auto pre : teminputs){
                if(pre->customInstruction() == "INPUT")
                    delNodeTree(pre);
                else{
                    Instruction *preins = pre->instruction();
                    if(preins != NULL){
                        if(dyn_cast<GetElementPtrInst>(preins)){
                            //get pattern access offset
                            int offset = 0;
                            auto LSbounds = node->getLSbounds();
                            for(int i = 0; i < LSbounds.size(); i++){
                                offset += (LSbounds[i][2] * LSbounds[i][0]);
                            }
                            //offset += pre->constVal();
                            node->setLSoffset(offset);
                            if(pre->constVal()!=0)
                                node->setLSstart(node->getLSstart()+pre->constVal());
                            errs() << node->getName() << " is affine, delete its GEP tree\n";
                            //errs() << "nodes size" << _nodes.size()<<"\n";
                            GEPsToDeltree.insert(pre);//delete GEP nodetree
                        }
                    }
                }
            }
        }else if(ins != NULL){
            if(dyn_cast<LoadInst>(ins) || dyn_cast<StoreInst>(ins)){
                allaffine = false;
                auto temp =node->inputNodes();
                for(auto pre : temp){
                    auto preins = pre->instruction();
                    if(preins != NULL){
                        if(dyn_cast<GetElementPtrInst>(preins)){
                            node->setLSoffset(pre->constVal());
                            if(pre->constVal()!=0)
                                node->setLSstart(node->getLSstart()+pre->constVal());
                            errs() << node->getName() << " is not affine, delete its GEP node\n";
                            //delete GEP & connect lastmul of address claculation with Load/Store
                            if(pre->inputNodes().size() == 0)
                                continue;
                            auto lastmul = *(pre->inputNodes().begin());
                            lastmul->addOutputNode(node);
                            node->addInputNode(lastmul);
                            addEdge(lastmul, node, EDGE_TYPE_DATA);
                            node->setInputIdx(lastmul, node->getInputIdx(pre));
                            GEPsToDelnode.insert(pre);
                        }
                    }
                }
            } 
        }
        if((node->customInstruction() == "LOOPSTART") || (node->customInstruction() == "LOOPEXIT")){
            delNode(node);
        }
    }
    ///finally del to avoid muiti LSU using the same GEP
    for(auto del:GEPsToDelnode){
        delNode(del);
    }
    for(auto deltree:GEPsToDeltree){
        delNodeTree(deltree);
    }

    std::vector<LLVMCDFGNode*> rmnodes;
    /*
    if(allaffine){//all LS affine---delete all SELECT
        for(auto &elem : _nodes){
            auto node = elem.second;
            if(node->customInstruction() == "SELECT"){
                for(auto pre : node->inputNodes()){
                    rmnodes.push_back(pre);
                }
                delNode(node);//delete SELECT when all LS are affine
            }
        }
    }else{*/
    for (auto &elem : _nodes)//Condition(all LS affine)'s SELECT delete has been dealed integrated below
    {
        auto node = elem.second;
        if (node->customInstruction() == "SELECT")
        {
            errs() << "check SELECT: " << node->getName() << "\n";
            bool hasinitial = false;
            LLVMCDFGNode* INPUT;//record INPUT if exist
            auto teminputs = node->inputNodes();
            for (auto pre : teminputs)
            {
                errs() << "\tcurrent pre is: " << pre->getName() << "\n";
                /// whether delete LOOPSTART & initial Constant
                if (pre->customInstruction() == "LOOPSTART")
                    rmnodes.push_back(pre);
                else if (pre->hasConst())///if exist CONST, it will be Phi's initial
                {
                    rmnodes.push_back(pre);
                    node->setInitial(pre->constVal());
                    hasinitial = true;
                }else if(pre->customInstruction() == "INPUT"){///if come accross INPUT node, try to find initial out of innermost
                    INPUT = pre;                              //TODO:is here any exeception?
                }else if (pre->isOutputBackEdge(node))
                {
                    Instruction *preins = pre->instruction();
                    if (preins == NULL)
                        assert(0 && "what's wrong with backedge");
                    else
                    {
                        if (preins->getOpcode() == Instruction::Add)
                        {
                            auto temoutputedges = node->outputEdges();
                            LLVMCDFGNode* cycleout;
                            bool findcycle = false;
                            bool legalexchange = true;
                            std::set<LLVMCDFGNode*> detected;
                            for(auto eid: temoutputedges){
                                legalexchange = true;
                                auto selectout = edge(eid)->dst();
                                if(selectout->instruction()->getOpcode() != Instruction::Add&&selectout->instruction()->getOpcode() != Instruction::Mul){
                                    legalexchange = false;//TODO:how to detect cycle path RIGHT?
                                    continue;
                                }
                                errs()<<"tem SELECT out: "<<selectout->getName()<<"\n";
                                std::vector<LLVMCDFGNode*> todetect = selectout->outputNodes();
                                while(!findcycle && !todetect.empty()){
                                    auto temnode = todetect.back();
                                    detected.insert(temnode);
                                    todetect.pop_back();
                                    if(temnode == node){
                                        findcycle = true;
                                        break;
                                    }/*else if(temnode->instruction()->getOpcode() != Instruction::Add && temnode->instruction()->getOpcode() != Instruction::Mul){
                                        legalexchange = false;
                                        continue;
                                    }*/else{
                                        for(auto nextoutput:temnode->outputNodes()){
                                            todetect.push_back(nextoutput);
                                        }
                                    }
                                }
                                if(findcycle){
                                    errs()<<"find cycle out: "<<selectout->getName()<<"!!!!!!!!!!!!\n";
                                    cycleout = selectout;
                                    break;
                                }
                            }
                            assert(cycleout != NULL);
                            if(cycleout != pre && legalexchange){
                                LLVMCDFGNode* transoperand;
                                int preidx, cycleoutidx;
                                for(auto preinput:pre->inputNodes()){
                                    if(detected.count(preinput)){
                                        continue;
                                    }else{
                                        transoperand = preinput;
                                        preidx = pre->getInputIdx(preinput);
                                    }
                                }
                                cycleoutidx = cycleout->getInputIdx(node);
                                node->addOutputNode(pre);
                                pre->addInputNode(node);
                                addEdge(node, pre, EDGE_TYPE_DATA);
                                pre->setInputIdx(node, preidx);
                                
                                cycleout->addInputNode(transoperand);
                                transoperand->addOutputNode(cycleout);
                                addEdge(transoperand, cycleout, EDGE_TYPE_DATA);
                                cycleout->setInputIdx(transoperand, cycleoutidx);
                                
                                node->delOutputNode(cycleout);
                                cycleout->delInputNode(node);
                                auto temedge = edge(node, cycleout);
                                delEdge(temedge);

                                transoperand->delOutputNode(pre);
                                pre->delInputNode(transoperand);
                                temedge = edge(transoperand, pre);
                                delEdge(temedge);
                            }
                            node->setAcc();
                            node->setCustomInstruction("ACC");
                            for (auto findinc : pre->inputNodes())
                            {
                                if (findinc == node)
                                    continue;
                                else
                                {
                                    node->addInputNode(findinc);
                                    findinc->addOutputNode(node);
                                    node->setInputIdx(findinc, 0);
                                    addEdge(findinc, node, EDGE_TYPE_DATA); /// TODO:here EDGE_TYPE_DATA?
                                }
                            }
                            node->setCustomInstruction("ACC");
                            if (node->outputEdges().size() == 1)//TODO:copy add node when both SELECT and add have outputnodes
                            { // only output to addself
                                node->setAccFirst();
                                // pre->delInputNode(node);//delete node form add's IOmap
                                pre->delOutputNode(node);
                                for (auto addsucc : pre->outputNodes())
                                {
                                    addsucc->addInputNode(node);
                                    node->addOutputNode(addsucc);
                                    addsucc->setInputIdx(node, addsucc->getInputIdx(pre));
                                    addEdge(node, addsucc, EDGE_TYPE_DATA);
                                }
                            }
                            else
                            {
                                errs() << "SELECT has succ: ";
                                for (auto succ : node->outputNodes())
                                {
                                    errs() << succ->getName() << " ";
                                }
                                errs() << "\n";
                            }
                            errs() << "delete acc calculate: " << pre->getName() << "\n";
                            delNode(pre);
                        }
                    }
                }
                if (node->outputEdges().size() == 0)
                { // if SELECT is dangling
                    errs() << node->getName() << " is dangling\n";
                    rmnodes.push_back(node);
                    continue;
                }
            }
            if(!hasinitial && INPUT != NULL){
                Instruction* inputIns;
                for(auto innode:_ioNodeMap){
                    if(INPUT == innode.second){
                        if(dyn_cast<Instruction>(innode.first))
                            inputIns = dyn_cast<Instruction>(innode.first);
                    }
                }
                if(inputIns != NULL){
                    if(auto Phi = dyn_cast<PHINode>(inputIns)){
                        while(Phi->getNumOperands() == 1){
                            if(dyn_cast<PHINode>(Phi->getOperand(0))){
                                Phi = dyn_cast<PHINode>(Phi->getOperand(0));
                            }
                        }
                        assert(Phi->getNumOperands() == 2 && "any else PHI");
                        node->setInitial(BinaryConstOp(Phi).second);
                        delNode(INPUT);
                    }
                }
            }
        }
    }
    errs()<< "final rm::\n";
    for(auto rm : rmnodes){
        errs() << "remove node tree: "<<rm->getName()<< "\n";
        delNodeTree(rm);
    }
}



// print DOT file of CDFG
void LLVMCDFG::printDOT(std::string fileName) {
	std::ofstream ofs;
	ofs.open(fileName.c_str());
    ofs << "Digraph G {\n";
    // nodes
	assert(_nodes.size() != 0);
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto name = node->getName();
        auto ins = node->instruction();
        ofs << name << "[label = \"" << name;
        //add address name of LSU
        if(ins != NULL){
            if(dyn_cast<LoadInst>(ins) || dyn_cast<StoreInst>(ins))
            ofs << ", ArrayName=" << node->getLSArrayName();
            //errs() << "sueecss land\n";
        }
        if(node->isLSaffine()){
            ofs << ", Stride=[ ";
            auto stride = node->getLSstride();
            for(int i = 0; i < stride.size(); i++){
                ofs << i<< ", " << stride[i] <<"; ";
            }
            ofs << "], Bounds=[ ";
            auto LSbounds = node->getLSbounds();
            for(int dim = 0; dim < LSbounds.size(); dim++){
                ofs << "(" << LSbounds[dim][0]<<", "<<LSbounds[dim][1]<<")*"<<LSbounds[dim][2]<<"; ";
            }
            ofs << "]";
        }
        if(node->isAcc()){
            ofs << ", inc="<<"1";
            ofs << ", range=[" << this->getLoopsAffineBounds()[0].first <<", "<<this->getLoopsAffineBounds()[0].second<<"]";
        }
        if(node->hasConst()){
            ofs << ", Const=" << node->constVal();
        }
        ofs << "\", shape = box, color = black];\n";
    }
	// edges
    std::map<std::pair<LLVMCDFGNode*, LLVMCDFGNode*>, int> edgestack;    
    for(auto &elem : _edges){
        auto edge = elem.second;
        auto srcName = edge->src()->getName();
        auto dstName = edge->dst()->getName();
        ofs << srcName << " -> " << dstName;
        auto type = edge->type();
        if(type == EDGE_TYPE_CTRL){
            ofs << "[color = red";
        }else if(type == EDGE_TYPE_MEM){
            ofs << "[color = blue";
        }else{
            ofs << "[color = black";
        }
        bool isBackEdge = edge->src()->isOutputBackEdge(edge->dst());
        if(isBackEdge){
            ofs << ", style = dashed";
        }else{
            ofs << ", style = bold";
        }
        int opIdx = edge->dst()->getInputIdx(edge->src());
        auto pair = std::make_pair(edge->src(), edge->dst());
        if(edgestack.count(pair)){
            opIdx = 1 - edgestack[pair];
        }else{
            edgestack[pair] = opIdx;
        }
        ofs << ", label = \"Op=" << opIdx;
        if(type == EDGE_TYPE_MEM){
            ofs << ", DepDist = " << edge->dst()->getSrcDep(edge->src()).distance;
        }
        ofs << "\"];\n";
    }
	ofs << "}\n";
	ofs.close();
}

void LLVMCDFG::printAffineDOT(std::string fileName) {
	std::ofstream ofs;
    auto loops = _nestloops;
    std::map<std::string, int> minoffset;
    std::map<std::string, int> maxsize;
    //aligent load (min) offset
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto ins = node->instruction();
        if(ins != NULL){
            if(dyn_cast<LoadInst>(ins)){
                auto arrayname = node->getLSArrayName();
                if(!minoffset.count(arrayname)){
                    minoffset[arrayname] = node->getLSoffset();
                }else if(minoffset[arrayname] > node->getLSoffset()){
                    minoffset[arrayname] = node->getLSoffset();
                }
            }
        }
    }
    //aligent load (max) size
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto ins = node->instruction();
        if(ins != NULL){
            if(dyn_cast<LoadInst>(ins)){
                auto LoadBound = node->getLSbounds();
                auto arrayname = node->getLSArrayName();
                int maxdim = LoadBound.size()-1;
                int size = 0;
                if(node->isLSaffine()){
                    for(int dim = 0; dim <= maxdim; dim++){
                        size += (LoadBound[dim][1] - LoadBound[dim][0])*LoadBound[dim][2];
                    }
                    size += LoadBound[maxdim][2];
                }else{
                    size = (LoadBound[0][1]-LoadBound[0][0]+1)*LoadBound[0][2];
                }
                if(minoffset[arrayname] < node->getLSoffset()){
                    size += (node->getLSoffset()-minoffset[arrayname]);
                }
                if(!maxsize.count(arrayname)){
                    maxsize[arrayname] = size;
                }else if(maxsize[arrayname] < size){
                    maxsize[arrayname] = size;
                }
            }
        }
    }
	ofs.open(fileName.c_str());
    ofs << "Digraph G {\n";
    // nodes
	assert(_nodes.size() != 0);
    for(auto &elem : _nodes){
        auto node = elem.second;
        auto name = node->getName();
        auto ins = node->instruction();
        ofs << name<<"[";
        if(node->hasConst()){
            ofs << "opcode=const, value=" << node->constVal();
        }
        else if(node->isAcc()){
            int repeat = 1;
            int internal = 1;
            int temlevel;
            auto ins = node->instruction();
            if(ins == NULL){//innermost PHI
                repeat = 1;
                internal = 1;
            }else if(auto PHI = dyn_cast<PHINode>(ins)){
                for(temlevel = 0; temlevel < loops.size(); temlevel++){
                    auto bbs = loops[temlevel]->getBlocks().vec();
                    auto parentBB = PHI->getParent();
                    bool here = false;
                    for(auto bb:bbs){
                        if(bb == parentBB){
                            here = true;
                            break;
                        }
                    }
                    if(here)
                        break;
                }
                //errs() << "temlevel level of "<<node->getName()<<" is: "<<temlevel<<"\n";
                for(int i = temlevel+1; i < getLoopsAffineCounts().size(); i++){
                    //errs()<<"repeat from level "<<i<<"\n";
                    repeat *= getLoopsAffineCounts(i);
                }
                for(int i = temlevel-1; i >= 0; i--){
                    internal *= getLoopsAffineCounts(i);
                }
            }else{
                assert(0&&"any else ACC");
            }
            ofs << "opcode=acc, acc_params=\""<< node->getInitial();
            ofs << ", " << getLoopsAffineCounts(temlevel);
            ofs << ", " << internal;      
            ofs << ", " << repeat <<"\"";
            ofs << ", acc_first=" << node->isAccFirst();///TODO:here acc_first is always initial
        }
        else if(node->isLSaffine()){
            std::string arrayname = node->getLSArrayName();
            ofs << "opcode=";
            if(dyn_cast<LoadInst>(ins))
                ofs << "input, ref_name=\"" << arrayname<<"\"";
            else if(dyn_cast<StoreInst>(ins))
                ofs << "output, ref_name=\"" << arrayname<<"\"";
            auto LSbounds = node->getLSbounds();
            int maxdim = LSbounds.size()-1;
            int size = 0;
            int offset;
            int reduceoffset;
            if(minoffset.count(arrayname) && dyn_cast<LoadInst>(ins)){
                offset = minoffset[arrayname];
            }else{
                offset = node->getLSoffset();
            }
            if(maxsize.count(arrayname) && dyn_cast<LoadInst>(ins)){
                size = maxsize[arrayname];
            }
            else{
                for(int dim = 0; dim <= maxdim; dim++){
                    size += (LSbounds[dim][1] - LSbounds[dim][0])*LSbounds[dim][2];
                }
                size += LSbounds[maxdim][2];
            }
            if(offset % BITWIDTH == 0){
                //offset = node->getLSoffset();
                reduceoffset = node->getLSstart() - offset;
            }else{
                size += offset%BITWIDTH;
                offset = offset-offset%BITWIDTH;
                reduceoffset = node->getLSstart() - offset;
            }
            errs()<<arrayname<<" LSstart: "<<node->getLSstart()<<"\n";
            ofs << ", size="<<size;
            ofs << ", offset=\"" << offset <<", ";
            //assert(node->getLSstart()>=0);
            if(node->getLSstart()<0)///LS totally fixed
                reduceoffset = node->getLSoffset()%BITWIDTH; 
            ofs <<reduceoffset<<"\"";
            ofs << ", pattern=\"";
            auto stride = node->getLSstride();
            int resetstride = 0;
            for(int i = 0; i < stride.size(); i++){
                //if(i == 0 || stride[i] == 0)
                //    ofs << stride[i] <<", " << getLoopsAffineCounts(i);
                //else
                ofs << stride[i]-resetstride<<", " << getLoopsAffineCounts(i);
                resetstride += (getLoopsAffineCounts(i)-1)*stride[i];
                if(i < stride.size()-1)
                    ofs << ", ";
            }
            ofs << "\"";
        }
        else if(node->customInstruction() == "SELECT"){
            ofs << "opcode=sel";
        }
        else if(node->customInstruction() != ""){
            ofs << "opcode=" <<node->customInstruction();
        }
        else if(ins != NULL){
            if(dyn_cast<CmpInst>(ins)){
                ofs << "opcode=" << node->finalInstruction();
            }else if(dyn_cast<SelectInst>(ins)){
                ofs << "opcode=sel";
            }
            else{
                ofs << "opcode=" << ins->getOpcodeName();
            }
            if(dyn_cast<LoadInst>(ins) || dyn_cast<StoreInst>(ins)){
                auto LSBounds = node->getLSbounds();
                auto LoopsCounts = getLoopsAffineCounts();
                int size = 0;
                int offset;
                int reduceoffset;
                int cycle = 1;
                std::string arrayname = node->getLSArrayName();
                for(int level = 0; level < LoopsCounts.size(); level++){
                    cycle *= LoopsCounts[level];
                }
                if(minoffset.count(arrayname) && dyn_cast<LoadInst>(ins)){
                    if(node->getLSoffset() > minoffset[arrayname]){
                        offset = minoffset[arrayname];
                    }else{
                        offset = node->getLSoffset();
                    }
                }else{
                    offset = node->getLSoffset();
                }
                if(maxsize.count(arrayname) && dyn_cast<LoadInst>(ins)){
                    size = maxsize[arrayname]  - offset;
                }else{ 
                    size = (LSBounds[0][1]-LSBounds[0][0]+1)*LSBounds[0][2] - offset;
                }
                if(offset%BITWIDTH == 0){
                    //offset = node->getLSoffset();
                    reduceoffset = 0;
                }else{
                    size += offset%BITWIDTH;
                    reduceoffset = offset%BITWIDTH;
                    offset = offset-offset%BITWIDTH;
                }
                ofs << ", ref_name=\"" << arrayname<<"\"";
                ofs << ", size="<<size;
                ofs << ", offset=\""<<offset<<", "<<reduceoffset<<"\"";
                ofs << ", cycles="<<cycle;
            }
        }
        ofs << "];\n";
        //ofs << ", shape = box, color = black];\n";
    }
	// edges
    std::map<std::pair<LLVMCDFGNode*, LLVMCDFGNode*>, int> edgestack;    
    for(auto &elem : _edges){
        auto edge = elem.second;
        if(edge->type() == EDGE_TYPE_MEM){
            continue;
        }
        auto srcName = edge->src()->getName();
        auto dstName = edge->dst()->getName();
        ofs << srcName << "->" << dstName;
        auto type = edge->type();
        int opIdx = edge->dst()->getInputIdx(edge->src());
        auto pair = std::make_pair(edge->src(), edge->dst());
        if(edgestack.count(pair)){
            opIdx = 1 - edgestack[pair];
        }else{
            edgestack[pair] = opIdx;
        }
        if(opIdx < 0)
            ofs << "[operand=0];\n";
        else
            ofs << "[operand=" << opIdx <<"];\n";
        /*
        if(type == EDGE_TYPE_MEM){
            ofs << ", DepDist = " << edge->dst()->getSrcDep(edge->src()).distance;
        }
        ofs << "\"];\n";
        */
    }
	ofs << "}\n";
	ofs.close();
}


// generate CDFG
void LLVMCDFG::generateCDFG()
{
    outs() << "########################################################\n";
    outs() << "Generate CDFG Started\n";
    // initialized CDFG
    //printDOT(_name + "_init.dot");

    auto nestloops = this->nestloops();
    errs() << nestloops.size() << "\n";

    outs() << ">>>>>> add edge between two nodes that have memory dependence (loop-carried)\n";
    addMemDepEdges();
    //printDOT(_name + "_after_addMemDepEdges.dot");

    // Connect control dependent node pairs among BBs
    outs() << ">>>>>> Connect control dependent node pairs among BBs\n";
    connectCtrlDepBBs();
    //printDOT(_name + "_after_connectCtrlDepBBs.dot");
    // insert Control NOT node behind the node with FALSE_COND control output edge
    outs() << ">>>>>> Insert CTRLNOT node behind the node with FALSE_COND control output edge\n";
    insertCtrlNotNodes();
    //printDOT(_name + "_after_insertCtrlNotNodes.dot");
    // transfer the multiple control predecessors (input nodes) into a inverted OR tree 
    // with the root connected to a node and leaves connected to control predecessors
    outs() << ">>>>>> Transfer multiple control predecessors (input nodes) into a inverted OR tree\n";
    createCtrlOrTree();
    //printDOT(_name + "_after_createCtrlOrTree.dot");

    outs() << ">>>>>> Transfer GEP node to MUL/ADD/Const tree\n";
    handleGEPNodes();
    //printDOT(_name + "_after_handleGEPNodes.dot");

    outs() << ">>>>>> Transfer PHINode to SELECT nodes\n";
    handlePHINodes(); 
    //printDOT(_name + "_after_handlePHINodes.dot");

    outs() << ">>>>>> affine Access Node\n";
    handleAffineLSNodes(); 
    //printDOT(_name + "_after_handleAffineLSNodes.dot");

    outs() << ">>>>>> Remove redundant nodes\n";
    removeRedundantNodes();
    //printDOT(_name + "_after_removeRedundantNodes.dot");


    outs() << ">>>>>> Assign final node name\n";
    assignFinalNodeName();
    //printDOT(_name + "_after_assignFinalNodeName.dot");
    printAffineDOT("affine.dot");

    outs() << "Generate CDFG Ended\n";
    outs() << "########################################################\n";

/*
    // add edge between two nodes that have memory dependence (loop-carried)
    outs() << ">>>>>> add edge between two nodes that have memory dependence (loop-carried)\n";
    addMemDepEdges();
    printDOT(_name + "_after_addMemDepEdges.dot");
    // Connect control dependent node pairs among BBs
    outs() << ">>>>>> Connect control dependent node pairs among BBs\n";
    connectCtrlDepBBs();
    printDOT(_name + "_after_connectCtrlDepBBs.dot");
    // insert Control NOT node behind the node with FALSE_COND control output edge
    outs() << ">>>>>> Insert CTRLNOT node behind the node with FALSE_COND control output edge\n";
    insertCtrlNotNodes();
    printDOT(_name + "_after_insertCtrlNotNodes.dot");
    // transfer the multiple control predecessors (input nodes) into a inverted OR tree 
    // with the root connected to a node and leaves connected to control predecessors
    outs() << ">>>>>> Transfer multiple control predecessors (input nodes) into a inverted OR tree\n";
    createCtrlOrTree();
    printDOT(_name + "_after_createCtrlOrTree.dot");
    // transfer GetElementPtr(GEP) node to MUL/ADD/Const tree
    outs() << ">>>>>> Transfer GEP node to MUL/ADD/Const tree\n";
    handleGEPNodes();
    printDOT(_name + "_after_handleGEPNodes.dot");
    // Transfer PHINode to SELECT nodes
    outs() << ">>>>>> Transfer PHINode to SELECT nodes\n";
    handlePHINodes(); 
    printDOT(_name + "_after_handlePHINodes.dot");
    // add mask AND node behind the Shl node with bytewidth less than MAX_DATA_BYTE
    outs() << ">>>>>> Add mask AND node behind the Shl node with bytewidth less than MAX_DATA_BYTE\n";
    addMaskAndNodes();
    printDOT(_name + "_after_addMaskAndNodes.dot");
    // add loop exit nodes
    outs() << ">>>>>> Add loop exit nodes\n";
    addLoopExitNodes();
    printDOT(_name + "_after_addLoopExitNodes.dot");
    // remove redundant nodes, e.g. Branch
    outs() << ">>>>>> Remove redundant nodes\n";
    removeRedundantNodes();
    printDOT(_name + "_after_removeRedundantNodes.dot");
    // assign final node name
    outs() << ">>>>>> Assign final node name\n";
    assignFinalNodeName();
    printDOT(_name + "_after_assignFinalNodeName.dot");
*/
}