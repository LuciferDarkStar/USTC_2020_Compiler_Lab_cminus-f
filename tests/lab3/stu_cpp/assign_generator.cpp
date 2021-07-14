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
//设置 return 
	auto retAlloca=builder->create_alloca(Int32Type);
	builder->create_store(CONST_INT(0),retAlloca);   //默认ret 0
//定义a[10]
	auto * arrayType=ArrayType::get(Int32Type,10);//数组大小为10，所以参数为10
	auto initializer=ConstantZero::get(Int32Type,module);
    auto a=builder->create_alloca(arrayType);
//获得a[0]并将其设为10
	auto a0GEP=builder->create_gep(a,{CONST_INT(0),CONST_INT(0)});
	builder->create_store(CONST_INT(10),a0GEP);
//获得a[1],并且a[1]=2*a[0] 
	auto a1GEP=builder->create_gep(a,{CONST_INT(0),CONST_INT(1)});//取得a[1]
	auto a0Load=builder->create_load(a0GEP);//取得a[0]的值
	auto mul=builder->create_imul(a0Load,CONST_INT(2));//a[0]*2
	builder->create_store(mul,a1GEP);//a[1]=2*a[0]
	auto a1Load=builder->create_load(a1GEP);//取得a[1]的值
	builder->create_store(a1Load,retAlloca);//将a[1]的值存入返回值
//返回a[1]的值
	auto retLoad=builder->create_load(retAlloca);
	builder->create_ret(retLoad);
	std::cout<<module->print();
	delete module;
	return 0;
}
