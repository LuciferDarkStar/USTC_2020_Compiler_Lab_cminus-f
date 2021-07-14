#ifndef ACTIVEVARS_HPP
#define ACTIVEVARS_HPP
#include "PassManager.hpp"
#include "Constant.h"
#include "Instruction.h"
#include "Module.h"

#include "Value.h"
#include "IRBuilder.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <map>
#include <queue>
#include <fstream>

#include <set>
#include <string>
#include <algorithm>

typedef std::set<Value*> ValueSet;

class ActiveVars : public Pass
{
public:
	ActiveVars(Module *m) : Pass(m) {}
	void run();
	void PostOrder(BasicBlock* bb);
	void Get_used_v(BasicBlock* bb);
	void I_Get_used_v(BasicBlock* bb, Instruction* inst);
	std::string print();
private:
	Function *func_;
	std::map<BasicBlock *, std::set<Value *> > live_in, live_out;
};

#endif