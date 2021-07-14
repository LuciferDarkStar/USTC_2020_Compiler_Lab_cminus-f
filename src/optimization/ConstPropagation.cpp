#include "ConstPropagation.hpp"
#include "logging.hpp"
#include<algorithm>
// 给出了返回整形值的常数折叠实现，大家可以参考，在此基础上拓展
// 当然如果同学们有更好的方式，不强求使用下面这种方式
ConstantInt *ConstFolder::compute(
    Instruction::OpID op,
    ConstantInt *value1,
    ConstantInt *value2)
{

    int c_value1 = value1->get_value();
    int c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::add:
        return ConstantInt::get(c_value1 + c_value2, module_);

        break;
    case Instruction::sub:
        return ConstantInt::get(c_value1 - c_value2, module_);
        break;
    case Instruction::mul:
        return ConstantInt::get(c_value1 * c_value2, module_);
        break;
    case Instruction::sdiv:
        return ConstantInt::get((int)(c_value1 / c_value2), module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantInt *ConstFolder::compare(
    CmpInst::CmpOp op,
    ConstantInt *value1,
    ConstantInt *value2)
{
    int c_value1 = value1->get_value();
    int c_value2 = value2->get_value();
    switch (op)
    {
    case CmpInst::EQ:
        return ConstantInt::get(c_value1 == c_value2, module_);
        break;
    case CmpInst::NE:
        return ConstantInt::get(c_value1 != c_value2, module_);
        break;
    case CmpInst::GT:
        return ConstantInt::get(c_value1 > c_value2, module_);
        break;
    case CmpInst::GE:
        return ConstantInt::get(c_value1 >= c_value2, module_);
        break;
    case CmpInst::LT:
        return ConstantInt::get(c_value1 < c_value2, module_);
        break;
    case CmpInst::LE:
        return ConstantInt::get(c_value1 <= c_value2, module_);
        break;
    default:
        return nullptr;
        break;
    }
}

ConstantInt *ConstFolder::compare_fp(
    FCmpInst::CmpOp op,
    ConstantFP *value1,
    ConstantFP *value2)
{
    //std::cout<<"result is "<<std::endl;
    float c_value1 = value1->get_value();
    float c_value2 = value2->get_value();
    //std::cout<<"result is "<<(c_value1 > c_value2)<<std::endl;
    switch (op)
    {
    case FCmpInst::EQ:
        return ConstantInt::get(c_value1 == c_value2, module_);
        break;
    case FCmpInst::NE:
        return ConstantInt::get(c_value1 != c_value2, module_);
        break;
    case FCmpInst::GT:
       // std::cout<<"result is "<<(c_value1 > c_value2)<<std::endl;
        return ConstantInt::get(c_value1 > c_value2, module_);
        break;
    case FCmpInst::GE:
        return ConstantInt::get(c_value1 >= c_value2, module_);
        break;
    case FCmpInst::LT:
        return ConstantInt::get(c_value1 < c_value2, module_);
        break;
    case FCmpInst::LE:
        return ConstantInt::get(c_value1 <= c_value2, module_);
        break;
    default:
        return nullptr;
        break;
    }
}

 ConstantFP *ConstFolder::compute_fp(Instruction::OpID op,ConstantFP *value1, ConstantFP* value2)
 {
    float c_value1 = value1->get_value();
    float c_value2 = value2->get_value();
    switch (op)
    {
    case Instruction::fadd:
        return ConstantFP::get(c_value1 + c_value2, module_);

        break;
    case Instruction::fsub:
        return ConstantFP::get(c_value1 - c_value2, module_);
        break;
    case Instruction::fmul:
        return ConstantFP::get(c_value1 * c_value2, module_);
        break;
    case Instruction::fdiv:
        return ConstantFP::get((float)(c_value1 / c_value2), module_);
        break;
    default:
        return nullptr;
        break;
    } 
 
 
 }
// 用来判断value是否为ConstantFP，如果不是则会返回nullptr
ConstantFP *cast_constantfp(Value *value)
{
    auto constant_fp_ptr = dynamic_cast<ConstantFP *>(value);
    if (constant_fp_ptr)
    {
        return constant_fp_ptr;
    }
    else
    {
        return nullptr;
    }
}
ConstantInt *cast_constantint(Value *value)
{
    auto constant_int_ptr = dynamic_cast<ConstantInt *>(value);
    if (constant_int_ptr)
    {
        return constant_int_ptr;
    }
    else
    {
        return nullptr;
    }
}
ConstantZero *cast_constantzero(Value *value)
{
    auto constant_int_ptr = dynamic_cast<ConstantZero *>(value);
    if (constant_int_ptr)
    {
        return constant_int_ptr;
    }
    else
    {
        return nullptr;
    }
}

void ConstPropagation::run()
{
    // 从这里开始吧！
    std::map<Value* ,std::vector<Value*>>con_val_stack;
    for(auto f:m_->get_functions())
    {
          auto func_=f;
          std::vector<Value *>true_block;
          std::vector<Value *>flase_block;
          for(auto bb:func_->get_basic_blocks())
          {
                   std::vector<Instruction*> wait_delete;//要删除的指令
                   
                   for(auto instr:bb->get_instructions())
                   {   
                        
                       if(instr->isBinary())
                       {    auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_0_fp=cast_constantfp(value_0);
                            auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            ConstantFP *    value_con_fp;
                            if((constant_0_int!=nullptr&&constant_1_int!=nullptr)||(constant_0_fp!=nullptr&&constant_1_fp!=nullptr))
                            {
                                 switch(instr->get_instr_type())
                                {
                                 case Instruction::add:
                                 //std::cout<<"add"<<std::endl;
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      //instr->set_operand();
                                      break;
                                 case Instruction::sub:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      break;
                                 case Instruction::mul:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                       break;
                                 case Instruction::sdiv:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      break;
                                 case Instruction::fadd:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                  case Instruction::fsub:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                 case Instruction::fmul:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                 case Instruction::fdiv:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                       break;
                                 default :
                                 break;
                                }
                                wait_delete.push_back(instr);
                            }
                            
                       }//删除或更改加减乘除指令
                       else if(instr->is_cmp())
                        {
                            
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_1_zero=cast_constantzero(value_1);
                            //auto constant_0_fp=cast_constantfp(value_0);
                            //auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            //ConstantFP *    value_con_fp;
                            //std::cout<<"icmp"<<static_cast<CmpInst*>(instr)->get_cmp_op()<<std::endl;
                            if((constant_0_int!=nullptr&&constant_1_int!=nullptr))
                            {
                            value_con_int = constfolder.compare(static_cast<CmpInst*>(instr)->get_cmp_op(),constant_0_int,constant_1_int);
                            instr->replace_all_use_with(value_con_int);
                            wait_delete.push_back(instr);
                            }
                            if(constant_0_int!=nullptr&&constant_1_zero!=nullptr)
                            {   int value_temp=constant_0_int->get_value();
                                value_con_int = ConstantInt::get(value_temp != 0, m_);
                                instr->replace_all_use_with(value_con_int);
                                wait_delete.push_back(instr);

                            }

                        }
                        else if(instr->is_fcmp())
                        {
                            
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_1_zero=cast_constantzero(value_1);
                            auto constant_0_fp=cast_constantfp(value_0);
                            auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            //ConstantFP *    value_con_fp;
                           // std::cout<<"fcmp"<<static_cast<FCmpInst*>(instr)->get_cmp_op()<<std::endl;
                            if((constant_0_fp!=nullptr&&constant_1_fp!=nullptr))
                            {
                            value_con_int = constfolder.compare_fp(static_cast<FCmpInst*>(instr)->get_cmp_op(),constant_0_fp,constant_1_fp);
                          //  std::cout<<"sb"<<std::endl;
                            instr->replace_all_use_with(value_con_int);
                            wait_delete.push_back(instr);
                            }
                            if(constant_0_fp!=nullptr&&constant_1_zero!=nullptr)
                            {   float value_temp=constant_0_fp->get_value();
                                //std::cout<<"sb"<<std::endl;
                                value_con_int = ConstantInt::get(value_temp != 0, m_);
                                instr->replace_all_use_with(value_con_int);
                                wait_delete.push_back(instr);

                            }
                        }
                        else if(instr->is_si2fp())
                        {
                          //std::cout<<instr->get_num_operand()<<std::endl;
                          auto value_0=instr->get_operand(0);
                           auto constant_0_int=cast_constantint(value_0);
                           ConstantFP * value_fp;
                           if(constant_0_int!=nullptr)
                           {
                                int value_new=constant_0_int->get_value();
                                value_fp=ConstantFP::get((float)value_new, m_);
                                instr->replace_all_use_with(value_fp);
                                wait_delete.push_back(instr);
                           }
                        }
                        else if(instr->is_fp2si())
                        {
                            auto value_0=instr->get_operand(0);
                           auto constant_0_fp=cast_constantfp(value_0);
                           ConstantInt * value_int;
                           if(constant_0_fp!=nullptr)
                           {
                                float value_new=constant_0_fp->get_value();
                                value_int=ConstantInt::get((int)value_new, m_);
                                instr->replace_all_use_with(value_int);
                                wait_delete.push_back(instr);
                           }
                        }
                        else if(instr->is_zext())
                        {
                          //std::cout<<instr->get_num_operand()<<std::endl;
                          auto value_0=instr->get_operand(0);
                           auto constant_0_int=cast_constantint(value_0);
                           ConstantInt * value_int;
                           if(constant_0_int!=nullptr)
                           {
                                int value_new=constant_0_int->get_value();
                                value_int=ConstantInt::get((int)value_new, m_);
                                instr->replace_all_use_with(value_int);
                                wait_delete.push_back(instr);
                           }
                        }
                          else if(instr->is_br())
                        {
                           // std::cout<<instr->get_num_operand()<<std::endl;
                            /*Value * value_0;
                            Value * value_1;
                            Value * value_2;*/
                            if(instr->get_num_operand()==1)
                            {
                              auto  value_0=instr->get_operand(0);
                            }else
                            {
                              auto value_0=instr->get_operand(0);
                              auto  value_1=instr->get_operand(1);
                              auto value_2=instr->get_operand(2);
                               auto constant_0_int=cast_constantint(value_0);
                               if(constant_0_int!=nullptr)//ready to improve
                               {
                                   int value_temp=constant_0_int->get_value();
                                   if(value_temp==1)
                                   {
                                      //instr->set_operand(0,value_1);
                                      //wait_delete.push_back(instr);
                                      true_block.push_back(value_1);
                                      flase_block.push_back(value_2);
                                    
                                   }
                                   else
                                   {
                                      // instr->set_operand(0,value_2);
                                       true_block.push_back(value_2);
                                       flase_block.push_back(value_1);
                                     // end for 
                                     //func_->remove(Block_flase);
                                     // wait_delete.push_back(instr);
                                   }
                               }
                            }
                            
                            
                        }else if(instr->is_phi())
                        {
                           auto value_0=instr->get_operand(0);
                           auto value_1=instr->get_operand(1);
                           auto value_2=instr->get_operand(2);
                           auto value_3=instr->get_operand(3);
                          // std::cout<<delete_block.size()<<std::endl;
                           if(find(true_block.begin(),true_block.end(),value_1)!=true_block.end())
                           {   instr->replace_all_use_with(value_0);
                               wait_delete.push_back(instr);
                           }else if(find(true_block.begin(),true_block.end(),value_3)!=true_block.end())
                           {
                              instr->replace_all_use_with(value_2);
                               wait_delete.push_back(instr);
                           }else if(find(flase_block.begin(),flase_block.end(),value_1)!=flase_block.end())
                           {   instr->replace_all_use_with(value_2);
                               wait_delete.push_back(instr);
                           }else if(find(flase_block.begin(),flase_block.end(),value_3)!=flase_block.end())
                           {
                              instr->replace_all_use_with(value_0);
                               wait_delete.push_back(instr);
                           }
                        
                        }
                        
                      
                  } 
                   
                   for(auto instr:wait_delete)
                   {
                   bb->delete_instr(instr);
                   }
          }
          for(auto bb:func_->get_basic_blocks())
          {
                 std::vector<Instruction*> wait_delete;//要删除的指令
                  for(auto instr:bb->get_instructions())
                  {     if(instr->isBinary())
                       {    auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_0_fp=cast_constantfp(value_0);
                            auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            ConstantFP *    value_con_fp;
                            if((constant_0_int!=nullptr&&constant_1_int!=nullptr)||(constant_0_fp!=nullptr&&constant_1_fp!=nullptr))
                            {
                                 switch(instr->get_instr_type())
                                {
                                 case Instruction::add:
                                 //std::cout<<"add"<<std::endl;
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      //instr->set_operand();
                                      break;
                                 case Instruction::sub:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      break;
                                 case Instruction::mul:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                       break;
                                 case Instruction::sdiv:
                                       value_con_int=constfolder.compute(instr->get_instr_type(),constant_0_int,constant_1_int);
                                       instr->replace_all_use_with(value_con_int);
                                      break;
                                 case Instruction::fadd:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                  case Instruction::fsub:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                 case Instruction::fmul:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                      break;
                                 case Instruction::fdiv:
                                       value_con_fp=constfolder.compute_fp(instr->get_instr_type(),constant_0_fp,constant_1_fp);
                                       instr->replace_all_use_with(value_con_fp);
                                       break;
                                 default :
                                 break;
                                }
                                wait_delete.push_back(instr);
                            }
                            
                       }//删除或更改加减乘除指令
                       else if(instr->is_cmp())
                        {
                            
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_1_zero=cast_constantzero(value_1);
                            //auto constant_0_fp=cast_constantfp(value_0);
                            //auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            //ConstantFP *    value_con_fp;
                            //std::cout<<"icmp"<<static_cast<CmpInst*>(instr)->get_cmp_op()<<std::endl;
                            if((constant_0_int!=nullptr&&constant_1_int!=nullptr))
                            {
                            value_con_int = constfolder.compare(static_cast<CmpInst*>(instr)->get_cmp_op(),constant_0_int,constant_1_int);
                            instr->replace_all_use_with(value_con_int);
                            wait_delete.push_back(instr);
                            }
                            if(constant_0_int!=nullptr&&constant_1_zero!=nullptr)
                            {   int value_temp=constant_0_int->get_value();
                                value_con_int = ConstantInt::get(value_temp != 0, m_);
                                instr->replace_all_use_with(value_con_int);
                                wait_delete.push_back(instr);

                            }

                        }
                        else if(instr->is_fcmp())
                        {
                            
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            auto value_0=instr->get_operand(0);
                            auto value_1=instr->get_operand(1);
                            
                            
                            auto constant_0_int=cast_constantint(value_0);
                            auto constant_1_int=cast_constantint(value_1);
                            auto constant_1_zero=cast_constantzero(value_1);
                            auto constant_0_fp=cast_constantfp(value_0);
                            auto constant_1_fp=cast_constantfp(value_1);
                            auto constfolder=ConstFolder(m_);
                            //std::cout<<instr->get_instr_type()<<std::endl;
                            ConstantInt *  value_con_int;
                            //ConstantFP *    value_con_fp;
                           // std::cout<<"fcmp"<<static_cast<FCmpInst*>(instr)->get_cmp_op()<<std::endl;
                            if((constant_0_fp!=nullptr&&constant_1_fp!=nullptr))
                            {
                            value_con_int = constfolder.compare_fp(static_cast<FCmpInst*>(instr)->get_cmp_op(),constant_0_fp,constant_1_fp);
                          //  std::cout<<"sb"<<std::endl;
                            instr->replace_all_use_with(value_con_int);
                            wait_delete.push_back(instr);
                            }
                            if(constant_0_fp!=nullptr&&constant_1_zero!=nullptr)
                            {   float value_temp=constant_0_fp->get_value();
                                //std::cout<<"sb"<<std::endl;
                                value_con_int = ConstantInt::get(value_temp != 0, m_);
                                instr->replace_all_use_with(value_con_int);
                                wait_delete.push_back(instr);

                            }
                        }
                        else if(instr->is_si2fp())
                        {
                          //std::cout<<instr->get_num_operand()<<std::endl;
                          auto value_0=instr->get_operand(0);
                           auto constant_0_int=cast_constantint(value_0);
                           ConstantFP * value_fp;
                           if(constant_0_int!=nullptr)
                           {
                                int value_new=constant_0_int->get_value();
                                value_fp=ConstantFP::get((float)value_new, m_);
                                instr->replace_all_use_with(value_fp);
                                wait_delete.push_back(instr);
                           }
                        }
                        else if(instr->is_fp2si())
                        {
                            auto value_0=instr->get_operand(0);
                           auto constant_0_fp=cast_constantfp(value_0);
                           ConstantInt * value_int;
                           if(constant_0_fp!=nullptr)
                           {
                                float value_new=constant_0_fp->get_value();
                                value_int=ConstantInt::get((int)value_new, m_);
                                instr->replace_all_use_with(value_int);
                                wait_delete.push_back(instr);
                           }
                        }
                        else if(instr->is_zext())
                        {
                          //std::cout<<instr->get_num_operand()<<std::endl;
                          auto value_0=instr->get_operand(0);
                           auto constant_0_int=cast_constantint(value_0);
                           ConstantInt * value_int;
                           if(constant_0_int!=nullptr)
                           {
                                int value_new=constant_0_int->get_value();
                                value_int=ConstantInt::get((int)value_new, m_);
                                instr->replace_all_use_with(value_int);
                                wait_delete.push_back(instr);
                           }
                        }
                        else if(instr->is_br())
                        {
                           // std::cout<<instr->get_num_operand()<<std::endl;
                            /*Value * value_0;
                            Value * value_1;
                            Value * value_2;*/
                            if(instr->get_num_operand()==1)
                            {
                              auto  value_0=instr->get_operand(0);
                            }else
                            {
                              auto value_0=instr->get_operand(0);
                              auto  value_1=instr->get_operand(1);
                              auto value_2=instr->get_operand(2);
                               auto constant_0_int=cast_constantint(value_0);
                               if(constant_0_int!=nullptr)//ready to improve
                               {
                                   int value_temp=constant_0_int->get_value();
                                   if(value_temp==1)
                                   {
                                      //instr->set_operand(0,value_1);
                                      wait_delete.push_back(instr);
                                      true_block.push_back(value_1);
                                      flase_block.push_back(value_2);
                                    auto Block_flase=dynamic_cast<BasicBlock*>(value_2);
                                      //func_->remove(Block_flase);
                                     // std::cout<<B_1->empty()<<std::endl;
                                     // instr->remove_operands(1,2);
                                     auto Block_true=dynamic_cast<BasicBlock*>(value_1);
                                     auto B_next=Block_true->get_succ_basic_blocks();
                                    if(find(B_next.begin(),B_next.end(),Block_flase)==B_next.end())
                                    {
                                        func_->remove(Block_flase);//true he flase
                                    }
                                     
                                     int size=Block_true->get_num_of_instr();
                                     int i=0;
                                     for(auto instr_true: Block_true->get_instructions())
                                     {
                                          bb->add_instruction(instr_true);//合并true
                                          i++;
                                         /* if(i==size-1)
                                          {     
                                                if(instr_true->is_br()&&instr_true->get_num_operand()==1)
                                                {    //std::cout<<value_0_temp->get_name()<<std::endl;
                                                     auto value_0_temp=instr_true->get_operand(0);
                                                     std::cout<<value_0_temp->get_name()<<std::endl;
                                                     auto Block_end=dynamic_cast<BasicBlock*>(value_0_temp);
                                                     for(auto instr_end:Block_end->get_instructions())
                                                        bb->add_instruction(instr_end);
                                                }
                                          }*/
                                          
                                     }//end for
                                      func_->remove(Block_true);
                                   }
                                   else
                                   {
                                      // instr->set_operand(0,value_2);
                                       true_block.push_back(value_2);
                                       flase_block.push_back(value_1);
                                     auto Block_true=dynamic_cast<BasicBlock*>(value_1);
                                      func_->remove(Block_true);
                                      //instr->remove_operands(1,2);
                                      auto Block_flase=dynamic_cast<BasicBlock*>(value_2);
                                      auto B_next=Block_true->get_succ_basic_blocks();
                                      if(find(B_next.begin(),B_next.end(),Block_flase)==B_next.end())
                                    {
                                        //true he flase
                                        func_->remove(Block_flase);
                                    }
                                      int size=Block_flase->get_num_of_instr();
                                     int i=0;
                                     for(auto instr_flase: Block_flase->get_instructions())
                                     {
                                          bb->add_instruction(instr_flase);//合并flase
                                          i++;
                                          /*if(i==size-1)
                                          {
                                                if(instr_flase->is_br()&&instr_flase->get_num_operand()==1)
                                                {    auto value_0_temp=instr_flase->get_operand(0);
                                                     auto Block_end=dynamic_cast<BasicBlock*>(value_0_temp);
                                                     for(auto instr_end:Block_end->get_instructions())
                                                        bb->add_instruction(instr_end);
                                                }
                                          }*/
                                          
                                          
                                          
                                                     
                                     }// end for 
                                     //func_->remove(Block_flase);
                                      wait_delete.push_back(instr);
                                   }
                               }
                            }
                            
                            
                        }else if(instr->is_phi())
                        {
                           auto value_0=instr->get_operand(0);
                           auto value_1=instr->get_operand(1);
                           auto value_2=instr->get_operand(2);
                           auto value_3=instr->get_operand(3);
                          // std::cout<<delete_block.size()<<std::endl;
                           if(find(true_block.begin(),true_block.end(),value_1)!=true_block.end())
                           {   instr->replace_all_use_with(value_0);
                               wait_delete.push_back(instr);
                           }else if(find(true_block.begin(),true_block.end(),value_3)!=true_block.end())
                           {
                              instr->replace_all_use_with(value_2);
                               wait_delete.push_back(instr);
                           }else if(find(flase_block.begin(),flase_block.end(),value_1)!=flase_block.end())
                           {   instr->replace_all_use_with(value_2);
                               wait_delete.push_back(instr);
                           }else if(find(flase_block.begin(),flase_block.end(),value_3)!=flase_block.end())
                           {
                              instr->replace_all_use_with(value_0);
                               wait_delete.push_back(instr);
                           }
                        
                        }
                  
                  }
                  for(auto instr:wait_delete)
                   {
                   bb->delete_instr(instr);
                   }
          }
          
        
    }
    
}
