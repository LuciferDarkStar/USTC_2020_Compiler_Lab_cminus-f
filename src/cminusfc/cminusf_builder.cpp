#include "cminusf_builder.hpp"
#include <iostream>

enum var_op {
	LOAD,
	STORE
};

BasicBlock* curr_block;
BasicBlock* return_block;
Value* return_alloca;
Function* curr_func;
Value* expression;
bool is_returned = false;
bool is_returned_record = false;
int label_cnt = 0;

var_op curr_op;

int flag;
int flag_2;
// You can define global variables here
// to store state

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

void CminusfBuilder::visit(ASTProgram &node)
{
	for (auto decl : node.declarations)
	{
		decl->accept(*this);
	}

}

void CminusfBuilder::visit(ASTNum &node)
{
	if (node.type == TYPE_INT)
		expression = ConstantInt::get(node.i_val, module.get());
	else if (node.type == TYPE_FLOAT)
		expression = ConstantFP::get(node.f_val, module.get());
}

void CminusfBuilder::visit(ASTVarDeclaration &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	auto initializer_Int = ConstantZero::get(Int32Type, module.get());
	auto initializer_FP = ConstantZero::get(FloatType, module.get());
	GlobalVariable* gvar;
	// declaration is variable
	if (node.num == nullptr)
	{
		//ConstantInt* const_int = ConstantInt::get(0, module.get());
		if (node.type == TYPE_INT)
			gvar = GlobalVariable::create(node.id, module.get(), PointerType::get_int32_type(module.get()), false, initializer_Int);
		else if (node.type == TYPE_FLOAT)
			gvar = GlobalVariable::create(node.id, module.get(), PointerType::get_float_type(module.get()), false, initializer_FP);
	}
	// declaration is array
	else
	{
		if (node.type == TYPE_INT)
		{
			ArrayType* arrType = ArrayType::get(IntegerType::get(32, module.get()), node.num->i_val);
			ConstantZero* constarr = ConstantZero::get(arrType, module.get());
			///	gvar=GlobalVariable::create(node.id, module.get(),PointerType::get_int32_type(module.get()),false,initializer_Int);
			gvar = GlobalVariable::create(node.id, module.get(), arrType, false, constarr);
			//gvar =  GlobalVariable::create(node.id,module.get(),PointerType::get_int32_ptr_type(module.get()),false,initializer_Int);
		}
		else if (node.type == TYPE_FLOAT)
		{
			ArrayType* arrType = ArrayType::get(FloatType, node.num->i_val);
			ConstantZero* constarr = ConstantZero::get(arrType, module.get());
			//gvar=GlobalVariable::create(node.id,module.get(),PointerType::get_float_type(module.get()),false,initializer_FP);  
			gvar = GlobalVariable::create(node.id, module.get(), arrType, false, constarr);
		}
	}
	scope.push(node.id, gvar);
	std::cout << std::endl;
}

void CminusfBuilder::visit(ASTFunDeclaration &node)
{      //std::cout<<"fun"<<std::endl;
	//std::cout << "funfunfunfun" << std::endl;
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	std::vector<Type *> Vars;
	for (auto param : node.params)
	{
		if (param->isarray)
		{
			if (param->type == TYPE_INT)
				Vars.push_back(PointerType::get_int32_ptr_type(module.get()));
			else
				Vars.push_back(PointerType::get_float_ptr_type(module.get()));
		}
		else
		{
			if (param->type == TYPE_INT)
				Vars.push_back(Type::get_int32_type(module.get()));
			else
				Vars.push_back(Type::get_float_type(module.get()));
		}
	}
	//Type *functype;
	auto functype = node.type == TYPE_INT ? Type::get_int32_type(module.get()) : (node.type == TYPE_FLOAT ? Type::get_float_type(module.get()) : Type::get_void_type(module.get()));
	auto function = Function::create(FunctionType::get(functype, Vars), node.id, module.get());
	scope.push(node.id, function);
	scope.enter();
	//std::cout << "funfunfunfunbefore1" << std::endl;
	curr_func = function;

	//return_block = BasicBlock::create(module.get(), "returnBB", curr_func);
	//return_block = BasicBlock::create(module.get(), "returnBB",return_block->get_terminator()->get_function());
	//std::cout << "funfunfunfunover1" << std::endl;
	auto entrybb = BasicBlock::create(module.get(), "entry", function);

	builder->set_insert_point(entrybb);
	curr_block = entrybb;
	// allocate space for function params, and add to symbol table(scope)
	//auto arg = function->arg_begin();
	std::vector<Value *> args;
	for (auto arg = function->arg_begin(); arg != function->arg_end(); arg++)
	{
		args.push_back(*arg);   // * 号运算符是从迭代器中取出迭代器当前指向的元素
	}
	auto arg = function->arg_begin();
	int i = 0;
	for (auto param : node.params)
	{
		if (arg == function->arg_end())
		{
			std::cout << "Fatal error: parameter number different!!" << std::endl;
		}
		if (param->isarray)
		{
			if (param->type == TYPE_INT)
			{
				auto param_var = builder->create_alloca(PointerType::get_int32_ptr_type(module.get()));
				scope.push(param->id, param_var);
				builder->create_store(args[i], param_var);
			}
			else
			{
				auto param_var = builder->create_alloca(PointerType::get_float_ptr_type(module.get()));
				scope.push(param->id, param_var);
				builder->create_store(args[i], param_var);
			}
		}
		else
		{
			//if(node.type == TYPE_INT)
			if (param->type == TYPE_INT)
			{
				auto param_var = builder->create_alloca(Type::get_int32_type(module.get()));
				scope.push(param->id, param_var);
				builder->create_store(args[i], param_var);
			}
			else
			{
				auto param_var = builder->create_alloca(Type::get_float_type(module.get()));
				scope.push(param->id, param_var);
				builder->create_store(args[i], param_var);
			}
		}
		arg++;
		i++;
	}
	// allocate the return register
	if (node.type != TYPE_VOID)
	{
		if (node.type == TYPE_INT)
		{
			return_alloca = builder->create_alloca(Type::get_int32_type(module.get()));
			flag = 1;
		}
		else
		{
			return_alloca = builder->create_alloca(Type::get_float_type(module.get()));
			flag = 2;
		}
	}
	else
		flag = 0;

	// reset label counter for each new function
	label_cnt = 0;
	return_block = BasicBlock::create(module.get(), "returnBB", curr_func);
	node.compound_stmt->accept(*this);
	// return_block = BasicBlock::create(module.get(), "returnBB",curr_block->get_parent());
	 //return_block->add_instruction(function); ???
	 //return_block->add_instruction(return_block->get_terminator());//???
	 //return_block = BasicBlock::create(module.get(), "returnBB", curr_func);

	builder->set_insert_point(return_block);
	if (node.type != TYPE_VOID)
	{
		if (node.type == TYPE_INT)
		{
			auto retLoad = builder->create_load(Type::get_int32_type(module.get()), return_alloca);
			builder->create_ret(retLoad);
		}
		else
		{
			auto retLoad = builder->create_load(Type::get_float_type(module.get()), return_alloca);
			builder->create_ret(retLoad);
		}
	}
	else
	{
		builder->create_void_ret();
	}
	if (curr_block->empty())
	{
		builder->set_insert_point(curr_block);
		builder->create_br(return_block);
	}

	scope.exit();
	//std::cout << "funfunfunfunover2" << std::endl;
}


void CminusfBuilder::visit(ASTParam &node) { }

void CminusfBuilder::visit(ASTCompoundStmt &node)
{   //std::cout << "com" << std::endl;
	scope.enter();
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	// process var-declaration
	for (auto var_decl : node.local_declarations)
	{
		if (var_decl->type == TYPE_VOID)
		{
			std::cout << "Error: no void type variable or array is allowed!" << std::endl;
		}
		// array declaration
		if (var_decl->num != nullptr)
		{

			if (var_decl->type == TYPE_INT)
			{
				auto * arrType = ArrayType::get(IntegerType::get(32, module.get()), var_decl->num->i_val);
				auto arrptr = builder->create_alloca(arrType);
				scope.push(var_decl->id, arrptr);
			}
			else if (var_decl->type == TYPE_FLOAT)
			{
				auto * arrType = ArrayType::get(FloatType, var_decl->num->i_val);
				auto arrptr = builder->create_alloca(arrType);
				scope.push(var_decl->id, arrptr);
			}

		}
		// normal variable declaration
		else
		{
			if (var_decl->type == TYPE_INT)
			{
				auto var = builder->create_alloca(Int32Type);
				scope.push(var_decl->id, var);
			}
			else if (var_decl->type == TYPE_FLOAT)
			{
				auto var = builder->create_alloca(FloatType);
				scope.push(var_decl->id, var);
			}
		}
	}
	is_returned = false;
	for (auto stmt : node.statement_list)
	{
		stmt->accept(*this);
		if (is_returned)
			break;
	}
	is_returned_record = is_returned;
	is_returned = false;
	scope.exit();
}

void CminusfBuilder::visit(ASTExpressionStmt &node)
{
	node.expression->accept(*this);
}

void CminusfBuilder::visit(ASTSelectionStmt &node)
{
	curr_op = LOAD;
	node.expression->accept(*this);
	char labelname[100];
	BasicBlock* trueBB;
	BasicBlock* falseBB;
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	Value* expr1;
	if (expression->get_type() == Type::get_int32_type(module.get()))
	{
		expr1 = builder->create_icmp_ne(expression, ConstantZero::get(Int32Type, module.get()));
	}
	else if (expression->get_type() == Type::get_float_type(module.get()))
	{
		expr1 = builder->create_icmp_ne(expression, ConstantZero::get(FloatType, module.get()));
	}
	else
	{
		expr1 = expression;
	}
	// record the current block, for add the br later
	BasicBlock* orig_block = curr_block;
	// create the conditional jump CondBr
	// if-statement
	label_cnt++;
	int label_now = label_cnt;
	sprintf(labelname, "selTrueBB_%d", label_now);
	trueBB = BasicBlock::create(module.get(), labelname, curr_func);
	builder->set_insert_point(trueBB);
	curr_block = trueBB;
	BasicBlock* trueBB_location;
	BasicBlock* falseBB_location;
	bool trueBB_returned;
	bool falseBB_returned;
	node.if_statement->accept(*this);
	trueBB_location = curr_block;
	trueBB_returned = is_returned_record;
	bool flag_flase = false;
	// optional else-statement
	if (node.else_statement != nullptr)
	{
		sprintf(labelname, "selFalseBB_%d", label_now);
		falseBB = BasicBlock::create(module.get(), labelname, curr_func);
	}
	if (node.else_statement != nullptr)
	{
		builder->set_insert_point(falseBB);
		curr_block = falseBB;
		node.else_statement->accept(*this);
		falseBB_location = curr_block;
		falseBB_returned = is_returned_record;
		flag_flase = true;
	}
	sprintf(labelname, "selEndBB_%d", label_now);
	//std::cout<<label_now<<std::endl;
	auto endBB = BasicBlock::create(module.get(), labelname, curr_func);
	builder->set_insert_point(orig_block);
	if (node.else_statement != nullptr)
	{
		builder->create_cond_br(expr1, trueBB, falseBB);
	}
	else
	{
		builder->create_cond_br(expr1, trueBB, endBB);
	}
	// unconditional jump to make ends meet
	if (!trueBB_returned)
	{
		builder->set_insert_point(trueBB_location);
		builder->create_br(endBB);
	}
	if (node.else_statement != nullptr && !falseBB_returned)
	{
		builder->set_insert_point(falseBB_location);
		builder->create_br(endBB);
	}
	/*if (trueBB_returned)
	{
		builder->set_insert_point(trueBB_location);
		builder->create_br(endBB);
	}
	if (node.else_statement != nullptr && falseBB_returned)
	{
		builder->set_insert_point(falseBB_location);
		builder->create_br(endBB);
	}*/
	/*if(node.else_statement!=nullptr&&falseBB_returned)
	{
	builder->set_insert_point(falseBB_location);
		builder->create_br(endBB);
	}*/
	if (flag_flase)
	{
		if (falseBB_location->empty())
		{
			builder->set_insert_point(falseBB_location);
			builder->create_br(endBB);
		}
	}
	builder->set_insert_point(endBB);
	//builder->create_br(endBB);
	curr_block = endBB;
	//std::cout<<endBB->get_num_of_instr()<<std::endl;
	/*if(endBB->empty()&&label_now==1)
		builder->create_br(return_block);
	 else if(endBB->empty())
		builder->create_br(endBB);*/

	is_returned = false;
}

void CminusfBuilder::visit(ASTIterationStmt &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	label_cnt++;
	int label_now = label_cnt;
	char labelname[100];
	sprintf(labelname, "loopStartBB_%d", label_now);
	auto startBB = BasicBlock::create(module.get(), labelname, curr_func);
	// goto start, end former block
	builder->create_br(startBB);
	builder->set_insert_point(startBB);
	curr_block = startBB;
	curr_op = LOAD;//?
	node.expression->accept(*this);
	Value* expr1;
	if (expression->get_type() == Int32Type)
	{
		expr1 = builder->create_icmp_ne(expression, ConstantZero::get(Int32Type, module.get()));
	}//expression为整数，则与0比较
	else if (expression->get_type() == FloatType)
	{
		expr1 = builder->create_icmp_ne(expression, ConstantZero::get(FloatType, module.get()));
	}
	else
	{
		expr1 = expression;
	}
	sprintf(labelname, "loopBodyBB_%d", label_now);
	auto bodyBB = BasicBlock::create(module.get(), labelname, curr_func);
	builder->set_insert_point(bodyBB);
	curr_block = bodyBB;
	node.statement->accept(*this);
	if (!is_returned_record)
	{
		builder->create_br(startBB);
	}
	/*if (is_returned_record)
	{
		builder->create_br(startBB);
	}*/
	/*if (is_returned_record)
	{
		if (flag != 0)
		{
			auto retLoad = builder->create_load(return_alloca);
			builder->create_ret(retLoad);
		}
		else
		{
			builder->create_void_ret();
		}
	}*/
	sprintf(labelname, "loopEndBB_%d", label_now);
	auto endBB = BasicBlock::create(module.get(), labelname, curr_func);
	// go back to create the CondBr in it's right location
	builder->set_insert_point(startBB);
	builder->create_cond_br(expr1, bodyBB, endBB);
	builder->set_insert_point(endBB);
	curr_block = endBB;
	is_returned = false;
}

void CminusfBuilder::visit(ASTReturnStmt &node)
{  // std::cout << "return" << std::endl;
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	if (node.expression != nullptr)
	{// type change??
		curr_op = LOAD;
		node.expression->accept(*this);
		Value* retVal;
		Value* temp;
		if (expression->get_type() == Type::get_int1_type(module.get()))
		{
			// cast i1 boolean true or false result to i32 0 or 1
			auto retCast = builder->create_zext(expression, Type::get_int32_type(module.get()));//???? get return value
			retVal = retCast;
			//builder.CreateRet(retCast);
		}
		else if (expression->get_type()->is_integer_type() || (expression->get_type()->is_float_type()))
		{
			retVal = expression;
			//builder.CreateRet(expression);
		}
		else
		{

			//std::cout << alloca->get_type()->get_pointer_element_type()->get_type_id() << std::endl;
			std::cout << "Error: unknown expression return type!" << std::endl;
		}
		//builder.CreateRet(retVal);
		if (retVal->get_type()->is_integer_type())
			flag_2 = 1;
		else if (retVal->get_type()->is_float_type())
			flag_2 = 2;
		else
			flag_2 = 0;
		if (flag != flag_2)
		{
			if (retVal->get_type()->is_integer_type())
				temp = builder->create_sitofp(retVal, FloatType);
			else if (retVal->get_type()->is_float_type())
				temp = builder->create_fptosi(retVal, Int32Type);
			else
				temp = retVal;
		}
		else
			temp = retVal;
		builder->create_store(temp, return_alloca);
	}
	else
	{
		//builder.CreateRetVoid();
		//builder->create_ret(ConstantZero::get(Type::get_void_type(module.get()),module.get()));
	}
	builder->create_br(return_block);
	is_returned = true;
	is_returned_record = true;
}

void CminusfBuilder::visit(ASTVar &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	int flag_3;
	switch (curr_op)
	{
	case LOAD:
	{
		if (node.expression == nullptr)
		{
			auto alloca = scope.find(node.id);
			// std::cout << alloca->get_type()->get_type_id() << std::endl;
			// std::cout << alloca->get_type()->get_pointer_element_type()->get_type_id() << std::endl;
			if (alloca->get_type()->get_pointer_element_type()->is_integer_type())
			{
				// normal variable,
				expression = builder->create_load(Type::get_int32_type(module.get()), alloca);
			}
			else if (alloca->get_type()->get_pointer_element_type()->is_float_type())
			{
				expression = builder->create_load(Type::get_float_type(module.get()), alloca);
			}
			else if (alloca->get_type()->get_pointer_element_type()->is_array_type())
			{
				// array reference(pointer to int) variable, treat differently
				// this is resulted from using array as function paramter, 
				// while the array itself is passed to the caller function by reference
				//expression = builder->create_load(PointerType::get_int32_ptr_type(module.get()), alloca);
				std::vector<Value *> idx;

				idx.push_back(ConstantZero::get(Int32Type, module.get()));

				idx.push_back(ConstantZero::get(Int32Type, module.get()));

				expression = builder->create_gep(alloca, idx);
			}
			else
			{
				//it's an array paramter used in call, do a GEP to change arr type to pointer type
				auto arrptr = builder->create_load(alloca);
				std::vector<Value *> idx;
				//idx.push_back(ConstantInt::get(context, APInt(32, 0)));
				//idx.push_back(ConstantZero::get(Int32Type, module.get()));
				//idx.push_back(ConstantInt::get(context, APInt(32, 0)));
				idx.push_back(ConstantZero::get(Int32Type, module.get()));
				//expression = builder.create_gep(alloca->getType()->getPointerElementType(), alloca, idx);
				//expression = builder->create_gep(alloca, idx);
				expression = builder->create_gep(arrptr, idx);
			}
		}
		else
		{
			// array
			auto alloca = scope.find(node.id);
			curr_op = LOAD;
			node.expression->accept(*this);
			Value* expr;
			if (expression->get_type() == Type::get_int1_type(module.get()))
			{
				expr = builder->create_zext(expression, Type::get_int32_type(module.get()));
			}
			else
			{
				expr = expression;
			}

			// check if array index is negative
			Value* idx_temp;//数组下标转整数
			if (expr->get_type()->is_float_type())
				idx_temp = builder->create_fptosi(expr, Int32Type);
			else
				idx_temp = expr;
			expression = idx_temp;
			auto neg = builder->create_icmp_lt(expression, ConstantZero::get(Int32Type, module.get()));
			char labelname[100];
			label_cnt++;
			sprintf(labelname, "arr_neg_%d", label_cnt);
			auto arrnegBB = BasicBlock::create(module.get(), labelname, curr_func);
			sprintf(labelname, "arr_ok_%d", label_cnt);
			auto arrokBB = BasicBlock::create(module.get(), labelname, curr_func);
			builder->create_cond_br(neg, arrnegBB, arrokBB);
			builder->set_insert_point(arrnegBB);
			std::vector<Value*> argdum;
			builder->create_call(scope.find("neg_idx_except"), argdum);
			//add this just to make llvm happy, actually program will abort in call
			builder->create_br(arrokBB);
			builder->set_insert_point(arrokBB);
			curr_block = arrokBB;

			/*std::cout << alloca->get_type()->get_type_id() << std::endl;
			std::cout << alloca->get_type()->get_pointer_element_type()->get_type_id() << std::endl;
			std::cout << static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->get_type_id() << std::endl;*/

			if (alloca->get_type()->get_pointer_element_type()->is_pointer_type())
			{//函数传进来的数组
				if (static_cast<PointerType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_integer_type())
				{
					// array reference as pointer
					//auto arrptr = builder->create_load(alloca);
					auto arrptr = builder->create_load(alloca);
					//arrptr = builder.CreateLoad(PointerType::getInt32PtrTy(context), alloca);
					std::vector<Value *> idx;
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(arrptr, idx);
					expression = builder->create_load(gep);
				}
				else if (static_cast<PointerType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_float_type())
				{
					// array reference as pointer
					//auto arrptr = builder->create_load(alloca);
					auto arrptr = builder->create_load(alloca);
					std::vector<Value *> idx;
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(arrptr, idx);
					expression = builder->create_load(gep);
				}
			}
			else if (alloca->get_type()->get_pointer_element_type()->is_array_type())
			{//局部声明的数组
				if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_integer_type())
				{
					// array reference as pointer
					//auto arrptr = builder->create_load(alloca);
					//std::cout << "load_int_array" << std::endl;
					std::vector<Value *> idx;
					idx.push_back(ConstantInt::get(0, module.get()));
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(alloca, idx);
					//std::cout << expression->get_type()->get_type_id() << std::endl;
					expression = builder->create_load(gep);
				}
				else if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_float_type())
				{
					// array reference as pointer
					//auto arrptr = builder->create_load(alloca);
					//std::cout << "load_float_array" << std::endl;
					std::vector<Value *> idx;
					idx.push_back(ConstantInt::get(0, module.get()));
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(alloca, idx);
					expression = builder->create_load(gep);
				}
			}
			/*else
			{
				std::cout << "in else" << std::endl;
				std::vector<Value *> idx;
				idx.push_back(ConstantZero::get(Int32Type, module.get()));
				idx.push_back(expression);
				auto gep = builder->create_gep(alloca, idx);
				expression = builder->create_load(gep);
			}*/
		}
		break;
	}
	case STORE:
	{
		if (node.expression == nullptr)
		{
			//variable
			auto alloca = scope.find(node.id);
			Value* expr;
			Value* temp;
			if (expression->get_type() == Type::get_int1_type(module.get()))
			{
				expr = builder->create_zext(expression, Type::get_int32_type(module.get()));
			}
			else
			{
				expr = expression;
			}
			if (alloca->get_type()->get_pointer_element_type()->is_integer_type())
				flag_3 = 1;
			else if (alloca->get_type()->get_pointer_element_type()->is_float_type())
				flag_3 = 2;
			if ((flag_3 != 1 && expr->get_type()->is_integer_type()) || (flag_3 != 2 && expr->get_type()->is_float_type()))
			{
				if (expr->get_type()->is_float_type())
					temp = builder->create_fptosi(expr, Int32Type);
				else if (expr->get_type()->is_integer_type())
					temp = builder->create_sitofp(expr, FloatType);
			}
			else
				temp = expr;

			builder->create_store(temp, alloca);
			expression = temp;
		}
		else
		{
			curr_op = LOAD;
			auto rhs = expression;
			node.expression->accept(*this);
			Value* expr;
			Value* temp;
			auto alloca = scope.find(node.id);
			if (expression->get_type() == Type::get_int1_type(module.get()))
			{
				expr = builder->create_zext(expression, Type::get_int32_type(module.get()));
			}
			else
			{
				expr = expression;
			}
			if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_integer_type())
				flag_3 = 1;
			else if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_float_type())
				flag_3 = 2;
			if ((flag_3 != 1 && rhs->get_type()->is_integer_type()) || (flag_3 != 2 && rhs->get_type()->is_float_type()))
			{
				if (rhs->get_type()->is_float_type())
					temp = builder->create_fptosi(rhs, Int32Type);
				else if (rhs->get_type()->is_integer_type())
					temp = builder->create_sitofp(rhs, FloatType);
			}
			else
				temp = rhs;

			rhs = temp;
			// check if array index is negative
			Value* idx_temp;//数组下标转整数
			if (expr->get_type()->is_float_type())
				idx_temp = builder->create_fptosi(expr, Int32Type);
			else
				idx_temp = expr;
			expression = idx_temp;
			auto neg = builder->create_icmp_lt(expression, ConstantZero::get(Int32Type, module.get()));
			char labelname[100];
			label_cnt++;
			sprintf(labelname, "arr_neg_%d", label_cnt);
			auto arrnegBB = BasicBlock::create(module.get(), labelname, curr_func);
			sprintf(labelname, "arr_ok_%d", label_cnt);
			auto arrokBB = BasicBlock::create(module.get(), labelname, curr_func);
			builder->create_cond_br(neg, arrnegBB, arrokBB);
			builder->set_insert_point(arrnegBB);
			std::vector<Value*> argdum;
			builder->create_call(scope.find("neg_idx_except"), argdum);
			// add this just to make llvm happy, actually program will abort in call
			builder->create_br(arrokBB);
			builder->set_insert_point(arrokBB);
			curr_block = arrokBB;

			//auto alloca = scope.find(node.id);
			if (alloca->get_type()->get_pointer_element_type()->is_pointer_type())
			{//函数传进来的数组
				if (static_cast<PointerType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_integer_type())
				{
					//array passed by reference, treat as pointer
					//function parameter makes it pointer of pointer, so load first
					//auto arrptr = builder.CreateLoad(PointerType::getInt32PtrTy(context), alloca);
					auto arrptr = builder->create_load(alloca);
					std::vector<Value *> idx;
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(arrptr, idx);
					builder->create_store(rhs, gep);
					expression = expr;
				}
				else if (static_cast<PointerType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_float_type())
				{
					// array passed by reference, treat as pointer
					// function parameter makes it pointer of pointer, so load first 
					//std::cout<<"store_float"<<std::endl;
					auto arrptr = builder->create_load(alloca);
					std::vector<Value *> idx;
					//idx.push_back(ConstantZero::get(Int32Type, module.get()));
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(arrptr, idx);
					builder->create_store(rhs, gep);
					expression = expr;
				}
			}
			else if (alloca->get_type()->get_pointer_element_type()->is_array_type())
			{//局部声明的数组
				if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_integer_type()) ///????? wrong judge
				{
					// array passed by reference, treat as pointer
					// function parameter makes it pointer of pointer, so load first
					std::vector<Value *> idx;
					idx.push_back(ConstantZero::get(Int32Type, module.get()));
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(alloca, idx);
					builder->create_store(rhs, gep);
					expression = expr;
				}
				else if (static_cast<ArrayType*>(alloca->get_type()->get_pointer_element_type())->get_element_type()->is_float_type())
				{
					// array passed by reference, treat as pointer
					// function parameter makes it pointer of pointer, so load first 
					std::vector<Value *> idx;
					idx.push_back(ConstantZero::get(Int32Type, module.get()));
					Value* idx_temp;//数组下标转整数
					if (expr->get_type()->is_float_type())
						idx_temp = builder->create_fptosi(expr, Int32Type);
					else
						idx_temp = expr;
					idx.push_back(idx_temp);
					auto gep = builder->create_gep(alloca, idx);
					builder->create_store(rhs, gep);
					expression = expr;
				}
				/*else
				{
					std::cout<<"store_else"<<std::endl;
					// local array or global array, type of which is [100 x i32]* like
					std::vector<Value *> idx;
					idx.push_back(ConstantZero::get(Int32Type, module.get()));
					idx.push_back(expr);
					auto gep = builder->create_gep(alloca, idx);
					builder->create_store(rhs, gep);
					expression = expr;
				}*/
			}
		}
		break;
	}
	default:
	{
		std::cout << "ERROR: wrong var op!" << std::endl;
	}
	}
}

void CminusfBuilder::visit(ASTAssignExpression &node)
{
	curr_op = LOAD;
	node.expression->accept(*this);

	curr_op = STORE;
	node.var->accept(*this);
}

void CminusfBuilder::visit(ASTSimpleExpression &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	curr_op = LOAD;
	node.additive_expression_l->accept(*this);
	if (node.additive_expression_r != nullptr)
	{
		Value* lhs = expression;
		curr_op = LOAD;
		node.additive_expression_r->accept(*this);
		Value* rhs = expression;
		Value* temp_l, *temp_r;
		if (lhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_l = builder->create_zext(lhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_l = lhs;
		}
		if (rhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_r = builder->create_zext(rhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_r = rhs;
		}
		rhs = temp_r;
		lhs = temp_l;
		if ((lhs->get_type()->is_integer_type()) && (rhs->get_type()->is_integer_type()))
		{
			switch (node.op)
			{
			case OP_LE:
				expression = builder->create_icmp_le(lhs, rhs);
				break;
			case OP_LT:
				expression = builder->create_icmp_lt(lhs, rhs);
				break;
			case OP_GT:
				expression = builder->create_icmp_gt(lhs, rhs);
				break;
			case OP_GE:
				expression = builder->create_icmp_ge(lhs, rhs);
				break;
			case OP_EQ:
				expression = builder->create_icmp_eq(lhs, rhs);
				break;
			case OP_NEQ:
				expression = builder->create_icmp_ne(lhs, rhs);
				break;
			}
		}
		else
		{
			if (!lhs->get_type()->is_float_type())
				lhs = builder->create_sitofp(lhs, FloatType);
			if (!rhs->get_type()->is_float_type())
				rhs = builder->create_sitofp(rhs, FloatType);
			switch (node.op) //????????
			{
			case OP_LE:
				expression = builder->create_fcmp_le(lhs, rhs);
				break;
			case OP_LT:
				expression = builder->create_fcmp_lt(lhs, rhs);
				break;
			case OP_GT:
				expression = builder->create_fcmp_gt(lhs, rhs);
				break;
			case OP_GE:
				expression = builder->create_fcmp_ge(lhs, rhs);
				break;
			case OP_EQ:
				expression = builder->create_fcmp_eq(lhs, rhs);
				break;
			case OP_NEQ:
				expression = builder->create_fcmp_ne(lhs, rhs);
				break;
			}
		}
	}
}//处理类型转换

void CminusfBuilder::visit(ASTAdditiveExpression &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	if (node.additive_expression == nullptr)
	{
		curr_op = LOAD;
		node.term->accept(*this);
	}
	else
	{
		curr_op = LOAD;
		node.additive_expression->accept(*this);
		Value* lhs = expression;
		curr_op = LOAD;
		node.term->accept(*this);
		Value* rhs = expression;
		Value* temp_l, *temp_r;
		if (lhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_l = builder->create_zext(lhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_l = lhs;
		}
		if (rhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_r = builder->create_zext(rhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_r = rhs;
		}
		rhs = temp_r;
		lhs = temp_l;
		if ((lhs->get_type()->is_integer_type()) && (rhs->get_type()->is_integer_type()))
		{
			switch (node.op)
			{
			case OP_PLUS:
				expression = builder->create_iadd(lhs, rhs);
				break;
			case OP_MINUS:
				expression = builder->create_isub(lhs, rhs);
				break;
			}
		}
		else
		{
			if (!lhs->get_type()->is_float_type())
				lhs = builder->create_sitofp(lhs, FloatType);
			if (!rhs->get_type()->is_float_type())
				rhs = builder->create_sitofp(rhs, FloatType);
			switch (node.op)
			{
			case OP_PLUS:
				expression = builder->create_fadd(lhs, rhs);
				break;
			case OP_MINUS:
				expression = builder->create_fsub(lhs, rhs);
				break;
			}
		}
	}

}//处理类型转换

void CminusfBuilder::visit(ASTTerm &node)
{
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	if (node.term == nullptr)
	{
		curr_op = LOAD;
		node.factor->accept(*this);
	}
	else
	{
		curr_op = LOAD;
		node.term->accept(*this);
		Value* lhs = expression;
		curr_op = LOAD;
		node.factor->accept(*this);
		Value* rhs = expression;
		Value* temp_l, *temp_r;
		if (lhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_l = builder->create_zext(lhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_l = lhs;
		}
		if (rhs->get_type() == Type::get_int1_type(module.get()))
		{
			temp_r = builder->create_zext(rhs, Type::get_int32_type(module.get()));
		}
		else
		{
			temp_r = rhs;
		}
		rhs = temp_r;
		lhs = temp_l;
		if ((lhs->get_type()->is_float_type()) || (rhs->get_type()->is_float_type()))
		{
			if (!lhs->get_type()->is_float_type())
				lhs = builder->create_sitofp(lhs, FloatType);
			if (!rhs->get_type()->is_float_type())
				rhs = builder->create_sitofp(rhs, FloatType);

			switch (node.op)
			{
			case OP_MUL:
				expression = builder->create_fmul(lhs, rhs);
				break;
			case OP_DIV:
				expression = builder->create_fdiv(lhs, rhs);
				break;
			}
		}
		else
			switch (node.op)
			{
			case OP_MUL:
				expression = builder->create_imul(lhs, rhs);
				break;
			case OP_DIV:
				expression = builder->create_isdiv(lhs, rhs);
				break;
			}
	}
}

void CminusfBuilder::visit(ASTCall &node)
{
	auto func = scope.find(node.id);
	int flag_4 = 0;
	int i = 0;
	Value* temp;
	Value* expr;
	Type *Int32Type = Type::get_int32_type(module.get());
	Type *FloatType = Type::get_float_type(module.get());
	if (func == nullptr)
	{
		std::cout << "ERROR: Unknown function: " << node.id << std::endl;
		exit(1);
	}
	std::vector<Value*> args;
	for (auto arg : node.args)
	{
		auto arg_func = static_cast<FunctionType*>(func->get_type())->get_param_type(i);
		if (arg_func->is_integer_type())
			flag_4 = 1;
		else if (arg_func->is_float_type())
			flag_4 = 2;
		else if (arg_func->is_pointer_type())
			flag_4 = 3;
		arg->accept(*this);
		expr = expression;
		if ((flag_4 != 1 && expr->get_type()->is_integer_type()) || (flag_4 != 2 && expr->get_type()->is_float_type()))
		{
			if (expr->get_type()->is_float_type())
				temp = builder->create_fptosi(expr, Int32Type);
			else if (expr->get_type()->is_integer_type())
				temp = builder->create_sitofp(expr, FloatType);
		}
		else
			temp = expr;
		args.push_back(temp);
		i++;
	}
	expression = builder->create_call(func, args);
}
