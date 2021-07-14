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
	Type *FloatType = Type::get_float_type(module);
	//main函数
	auto mainFun=Function::create(FunctionType::get(Int32Type,{}),"main",module);
	auto bb=BasicBlock::create(module,"entry",mainFun);
	builder->set_insert_point(bb);
 
	auto retAlloca=builder->create_alloca(Int32Type);
	builder->create_store(CONST_INT(0),retAlloca);// 默认 ret 0
	//给a分配内存
	auto aAlloca=builder->create_alloca(FloatType);
	//a=5.555
	builder->create_store(CONST_FP(5.555),aAlloca);
	//将a的值load
	auto aLoad=builder->create_load(aAlloca);
	//用a和1比较
	auto icmp=builder->create_fcmp_gt(aLoad,CONST_FP(1));
	// a>1跳往true
	auto true1=BasicBlock::create(module,"ture1",mainFun);
	// 否则跳往flase
	auto false1=BasicBlock::create(module,"false1",mainFun);
	//return ,the branch of return
	auto ret1=BasicBlock::create(module,"",mainFun);
	//the condition of br
	auto br=builder->create_cond_br(icmp,true1,false1);

	//true	
	builder->set_insert_point(true1);
	//ret=233
	builder->create_store(CONST_INT(233),retAlloca);
	builder->create_br(ret1);// 注意在下一个BB之前要Br一下

	//false
	builder->set_insert_point(false1);
	//ret=0
	builder->create_store(CONST_INT(0),retAlloca);
	builder->create_br(ret1);

	//ret1
	builder->set_insert_point(ret1);
	auto retLoad=builder->create_load(retAlloca);
	builder->create_ret(retLoad);
	std::cout<<module->print();
	delete module;
	return 0;
}
