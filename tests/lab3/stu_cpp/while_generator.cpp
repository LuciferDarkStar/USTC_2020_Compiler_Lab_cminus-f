#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFP::get(num, module) // 得到常数值的表示,方便后面多次用到

int main() {
	auto module = new Module("Cminus code");  // module name是什么无关紧要
	auto builder = new IRBuilder(nullptr, module);
	Type *Int32Type = Type::get_int32_type(module);
	//main函数
	auto mainFun=Function::create(FunctionType::get(Int32Type,{}),"main",module);
	auto bb=BasicBlock::create(module,"entry",mainFun);
	builder->set_insert_point(bb);

	auto retAlloca=builder->create_alloca(Int32Type);
	builder->create_store(CONST_INT(0),retAlloca);// 默认 ret 0
	//a=10
	auto aAlloca=builder->create_alloca(Int32Type);
	builder->create_store(CONST_INT(10),aAlloca);
	auto aLoad=builder->create_load(aAlloca);
	//i=0
	auto iAlloca=builder->create_alloca(Int32Type);
	builder->create_store(CONST_INT(0),iAlloca);
	auto iLoad=builder->create_load(iAlloca);
	//将i和10比较
	auto icmp=builder->create_icmp_lt(iLoad,CONST_INT(10));

	auto true1=BasicBlock::create(module,"true1",mainFun);
	auto end1=BasicBlock::create(module,"end1",mainFun); 
	// 根据结果跳转
	auto br=builder->create_cond_br(icmp,true1,end1);

	//true1
	builder->set_insert_point(true1);
	// i=i+1
	iLoad=builder->create_load(iAlloca);
	auto inew=builder->create_iadd(iLoad,CONST_INT(1));
	builder->create_store(inew,iAlloca);
	// a=a+i
	aLoad=builder->create_load(aAlloca);
	auto anew=builder->create_iadd(aLoad,inew);
	builder->create_store(anew,aAlloca);
	//再次比较
	icmp=builder->create_icmp_lt(inew,CONST_INT(10));
	br=builder->create_cond_br(icmp,true1,end1);// 根据结果跳转

	//end
	builder->set_insert_point(end1);
	aLoad=builder->create_load(aAlloca);
	builder->create_store(aLoad,retAlloca);
	auto retLoad=builder->create_load(retAlloca);
	builder->create_ret(retLoad);
	std::cout<<module->print();
	delete module;
	return 0;
}
