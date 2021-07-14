#include <algorithm>
#include "logging.hpp"
#include "LoopSearch.hpp"
#include "LoopInvHoist.hpp"

void LoopInvHoist::run()
{
	// 先通过LoopSearch获取循环的相关信息
	LoopSearch loop_searcher(m_, false);
	loop_searcher.run();
	for (auto f : m_->get_functions())
	{
		auto func_ = f;
		auto all_loops = loop_searcher.get_loops_in_func(func_);
		for (auto loop : all_loops)
		{
			std::cout << "1" << std::endl;
			auto loop_base = loop_searcher.get_loop_base(loop);
			auto loop_inest = loop_searcher.get_inner_loop(loop_base);//找到最内侧循环
		   // auto loop_bb=loop_inest;

			for (auto loop_bb = loop_inest; loop_bb != nullptr; loop_bb = loop_searcher.get_parent_loop(loop_bb))//对每一层循环分析
			{
				std::vector<Instruction*> need_to_out;
				std::vector<Value*> nocon_value;
				std::vector<Value*> con_cycle_value;
				std::vector<Instruction*> con_instr;
				std::vector<Value*> delete_phi;
				auto bb_base = loop_searcher.get_loop_base(loop_bb);
				for (auto bb : *loop_bb)
				{

					for (auto instr : bb->get_instructions())
					{
						if (instr->is_phi())
						{
							auto instr_value = static_cast<Value*> (instr);
							nocon_value.push_back(instr_value);


						}

					}

				}
				for (auto bb : *loop_bb)
				{
					std::vector<Instruction*>  wait_delete;
					for (auto instr : bb->get_instructions())
					{
						if (instr->isBinary())
						{
							auto value_0 = instr->get_operand(0);
							auto value_1 = instr->get_operand(1);
							if (find(nocon_value.begin(), nocon_value.end(), value_0) != nocon_value.end() || find(nocon_value.begin(), nocon_value.end(), value_1) != nocon_value.end())//右值是被赋值改变过的
							{
								auto instr_value = static_cast<Value*> (instr);
								nocon_value.push_back(instr_value);

							}
							else
							{
								auto instr_value = static_cast<Value*> (instr);
								con_cycle_value.push_back(instr_value);
								need_to_out.push_back(instr);
								wait_delete.push_back(instr);
								delete_phi.push_back(instr_value);

							}


						}


					}
					for (auto instr : wait_delete)
					{
						bb->delete_instr(instr);

					}

				}

				/*for(auto bb:*loop_bb)
				{         std::vector<Instruction*>  wait_delete;

						 for(auto instr:bb->get_instructions())
						 {
								  if(instr->is_phi())
								  {
								  auto value_0=instr->get_operand(0);
								  auto value_1=instr->get_operand(1);
								  auto value_2=instr->get_operand(2);
								  auto value_3=instr->get_operand(3);
								  //std::cout<<instr->get_name()<<std::endl;

								  //auto instr_value=static_cast<Value*>(instr);
								  //con_instr.push_back(instr);
								  //if(find(con_value.begin(),con_value.end(),value_0)==con_value.end()&&(find(con_value.begin(),con_value.end(),value_1)==con_value.end()))
								  if(find(delete_phi.begin(),delete_phi.end(),value_0)!=delete_phi.end()||find(delete_phi.begin(),delete_phi.end(),value_2)!=delete_phi.end())
								   {

									  //bb->delete_instr(instr);
									  wait_delete.push_back(instr);


								   }
								  }


						 }
						 for(auto instr:wait_delete)
						 {
							bb->delete_instr(instr);

						 }

			   }*/
				for (auto base_pre : bb_base->get_pre_basic_blocks())
				{
					if ((*loop_bb).find(base_pre) == (*loop_bb).end())
					{

						while (!need_to_out.empty())
						{
							auto instr_add = need_to_out.back();
							need_to_out.pop_back();
							base_pre->add_instr_begin(instr_add);

						}

					}

				}




			}


		}
	}
	// 接下来由你来补充啦！
}
