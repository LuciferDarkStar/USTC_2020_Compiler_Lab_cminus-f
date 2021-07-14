#include "ActiveVars.hpp"
#include <iostream>
std::vector<BasicBlock *> BBlist;
std::vector<Value *> Temp_phi;
std::set<BasicBlock *> visited;
std::set<Value *> Temp_list;
std::map<BasicBlock *, std::set<Value *> > pre_live_in, pre_live_out;
std::map<BasicBlock *, std::set<Value *> > Used_v, Def_v;
std::map<BasicBlock *, std::set<Instruction *> > BB_phi;



using namespace std;

void ActiveVars::run()
{
	std::ofstream output_active_vars;
	output_active_vars.open("active_vars.json", std::ios::out);
	output_active_vars << "[";
	for (auto &func : this->m_->get_functions()) {
		if (func->get_basic_blocks().empty()) {
			continue;
		}
		else
		{
			func_ = func;

			func_->set_instr_name();
			live_in.clear();
			live_out.clear();

			// 在此分析 func_ 的每个bb块的活跃变量，并存储在 live_in live_out 结构内
			pre_live_in.clear();
			pre_live_out.clear();
			Used_v.clear();
			Def_v.clear();
			BBlist.clear();
			visited.clear();
			BB_phi.clear();
			Temp_phi.clear();

			BasicBlock* bb = func_->get_entry_block();
			ActiveVars::PostOrder(bb);

			for (auto &b : func_->get_basic_blocks())
				ActiveVars::Get_used_v(b);

			bool flag = true;
			while (flag)
			{
				for (size_t i = 0; i < BBlist.size(); i++)
				{
					auto temp_bb = BBlist[i];
					//cout<<temp_bb->get_name()<<endl;
					live_out[temp_bb].clear();
					for (auto b : temp_bb->get_succ_basic_blocks())
					{
						Temp_list.clear();
						//live_out[temp_bb].clear();
						set_union(live_out[temp_bb].begin(), live_out[temp_bb].end(), live_in[b].begin(), live_in[b].end(), inserter(Temp_list, Temp_list.begin()));
						live_out[temp_bb].clear();
						live_out[temp_bb].insert(Temp_list.begin(), Temp_list.end());
						Temp_phi.clear();
						if (!BB_phi[b].empty())
							for (auto &I : BB_phi[b])
							{
								for (auto &v : I->get_operands())
								{
									if (dynamic_cast<BasicBlock *>(v))
									{
										BasicBlock* t = dynamic_cast<BasicBlock *>(v);
										if (t != temp_bb)
											live_out[temp_bb].erase(Temp_phi.back());

									}
									else
									{
										Temp_phi.push_back(v);
									}
								}
							}
					}
					/*cout<<"LiveOut:";
					for(auto v : live_out[temp_bb])
						cout<<v->get_name()<<" ";
					cout<<endl;*/
					Temp_list.clear();
					set_difference(live_out[temp_bb].begin(), live_out[temp_bb].end(), Def_v[temp_bb].begin(), Def_v[temp_bb].end(), inserter(Temp_list, Temp_list.begin()));
					live_in[temp_bb].clear();
					set_union(Used_v[temp_bb].begin(), Used_v[temp_bb].end(), Temp_list.begin(), Temp_list.end(), inserter(live_in[temp_bb], live_in[temp_bb].begin()));
					/*cout<<"LiveIn:";
					for(auto v : live_in[temp_bb])
						cout<<v->get_name()<<" ";
					cout<<endl;
					cout<<endl;*/
				}
				flag = false;
				for (size_t i = 0; i < BBlist.size(); i++)
				{
					//Temp_list.clear();
					auto temp_bb = BBlist[i];
					//set_difference(live_out[temp_bb].begin(), live_out[temp_bb].end(), pre_live_out[temp_bb].begin(), pre_live_out[temp_bb].end(), inserter(Temp_list, Temp_list.begin()));
					if (std::equal(live_in[temp_bb].begin(), live_in[temp_bb].end(), pre_live_in[temp_bb].begin(), pre_live_in[temp_bb].end()))
					{
						pre_live_out[temp_bb].clear();
						pre_live_in[temp_bb].clear();
						pre_live_out[temp_bb].insert(live_out[temp_bb].begin(), live_out[temp_bb].end());
						pre_live_in[temp_bb].insert(live_in[temp_bb].begin(), live_in[temp_bb].end());
					}
					else
					{
						pre_live_out[temp_bb].clear();
						pre_live_in[temp_bb].clear();
						pre_live_out[temp_bb].insert(live_out[temp_bb].begin(), live_out[temp_bb].end());
						pre_live_in[temp_bb].insert(live_in[temp_bb].begin(), live_in[temp_bb].end());
						flag = true;
					}
				}
			}


			output_active_vars << print();
			output_active_vars << ",";
		}
	}
	output_active_vars << "]";
	output_active_vars.close();
	return;
}

void ActiveVars::PostOrder(BasicBlock* bb)
{
	visited.insert(bb);
	for (BasicBlock * succ : bb->get_succ_basic_blocks())
	{
		if (visited.find(succ) == visited.end())
			PostOrder(succ);
	}
	//cout<<bb->get_name()<<endl;
	BBlist.push_back(bb);
}

void ActiveVars::Get_used_v(BasicBlock* bb)
{
	//cout<<bb->get_name()<<endl;
	for (auto &inst : bb->get_instructions())
		I_Get_used_v(bb, inst);
}

void ActiveVars::I_Get_used_v(BasicBlock* bb, Instruction* inst)
{
	//cout<<inst->get_name()<<endl;
	if (inst->get_instr_type() == 15)//是否为PHI指令
	{
		BB_phi[bb].insert(inst);
		//cout<<inst->get_name()<<endl;
	}
	for (auto &v : inst->get_operands())
	{
		if (dynamic_cast<Instruction*>(v) || dynamic_cast<Argument*>(v) || dynamic_cast<GlobalVariable*>(v))
			if (Def_v[bb].find(v) == Def_v[bb].end())
			{
				Used_v[bb].insert(v);
				//cout<<"U:"<<v->get_name()<<endl;
			}
	}
	if (inst->get_instr_type() != 1 && inst->get_instr_type() != 0)//br和ret指令不会产生变量
		if (static_cast<Value *>(inst) || dynamic_cast<Argument*>(inst) || dynamic_cast<GlobalVariable*>(inst))
		{
			Value* u = static_cast<Value *>(inst);
			if (Used_v[bb].find(u) == Used_v[bb].end())
			{
				Def_v[bb].insert(u);
				//cout<<"D:"<<u->get_name()<<endl;
			}
		}
	//cout<<endl;
}



std::string ActiveVars::print()
{
	std::string active_vars;
	active_vars += "{\n";
	active_vars += "\"function\": \"";
	active_vars += func_->get_name();
	active_vars += "\",\n";

	active_vars += "\"live_in\": {\n";
	for (auto &p : live_in) {
		if (p.second.size() == 0) {
			continue;
		}
		else {
			active_vars += "  \"";
			active_vars += p.first->get_name();
			active_vars += "\": [";
			for (auto &v : p.second) {
				active_vars += "\"%";
				active_vars += v->get_name();
				active_vars += "\",";
			}
			active_vars += "]";
			active_vars += ",\n";
		}
	}
	active_vars += "\n";
	active_vars += "    },\n";

	active_vars += "\"live_out\": {\n";
	for (auto &p : live_out) {
		if (p.second.size() == 0) {
			continue;
		}
		else {
			active_vars += "  \"";
			active_vars += p.first->get_name();
			active_vars += "\": [";
			for (auto &v : p.second) {
				active_vars += "\"%";
				active_vars += v->get_name();
				active_vars += "\",";
			}
			active_vars += "]";
			active_vars += ",\n";
		}
	}
	active_vars += "\n";
	active_vars += "    }\n";

	active_vars += "}\n";
	active_vars += "\n";
	return active_vars;
}