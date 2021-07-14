# Lab5 实验报告-阶段一

小组成员 

姓名 学号

魏钊    PB18111699（队长）

孙宇鹏  PB18111695

杨嘉诚  PB18030996
## 实验要求

阅读loopSearch和Mem2Reg的相关源代码，理解CFG和强连通分量算法，理解phi函数和支配树在复写传播和SSA中的作用和使用。

## 思考题
### LoopSearch
1. 使用Tarjan算法找到强连通图，连通分量保存在sccs中。对于每个成员使用find_loop_base函数，对于循环子图如果有一个节点的前驱不在当前循环子图内，则这个节点就是此循环的入口，否则通过已找到的其他循环入口的后继来找到当前循环的入口（此情况发生是由于每次找到一个循环的入口后会将该入口节点删除，可能出现某个循环的所有节点的前驱均在循环之内）。

    每个循环唯一对应一个入口。

2. 因为使用Tarjan算法得到的是极大连通子图，所以若出现循环嵌套，则每次将会先得到最大循环的入口，在此之后，会从该连通子图中删去此入口节点，再次调用find_loop_base，即可得到内部循环的入口。重复上述过程即可处理所有循环嵌套。

### Mem2reg
1.  在离开n的每条CFG路径上，从结点n可达但不支配的第一个结点，就是n的支配边界。
2. 
    概念：PHI函数是由SSA形式衍生而来的，由于变量在每次定值的时候，都要有一个编号，如果一个基本快有多个前驱，而每个前驱都对一个x进行定值了，分别编号为x1，x2，，xn。那么到达本基本快的时候我究竟该使用哪一个版本的x，可以在本基本块的开头添加PHI函数来进行取值。
    
    意义：使用phi便无需在当前基本块的各个前驱块中都对同一变量进行赋值，可以在此基本块中根据之前跳转到了哪一前驱块来决定变量的最终取值。
3. 
   ```
   complex3.cminus:
   
   int gcd (int u, int v)
   { 
       if (v == 0) 
   		return u;
       else 
   		return gcd(v, u - u / v * v); 
   }
   
   void main()
   {
   	int a, b;
   	a = input();
   	b = input();
   	if(a < b)
   		swap(a, b);
   	output(gcd(a, b));
   }
   ```
    After `Mem2Reg`：

   ```c
   ; ModuleID = 'cminus'
   source_filename = "complex3.cminus"
   
   declare i32 @input()
   
   declare void @output(i32)
   
   declare void @outputFloat(float)
   
   declare void @neg_idx_except()
   
   define i32 @gcd(i32 %arg0, i32 %arg1) {
   label_entry:
     %op5 = icmp eq i32 %arg1, 0 //删去了读取参数的代码
     %op6 = zext i1 %op5 to i32
     %op7 = icmp ne i32 %op6, 0
     br i1 %op7, label %label8, label %label10
   label8:                                                ; preds = %label_entry
     ret i32 %arg0 //删去了取数的代码， 直接使用形参
   label10:                                                ; preds = %label_entry
     %op15 = sdiv i32 %arg0, %arg1 //同上
     %op17 = mul i32 %op15, %arg1
     %op18 = sub i32 %arg0, %op17
     %op19 = call i32 @gcd(i32 %arg1, i32 %op18)
     ret i32 %op19
   }
   define void @main() {
   label_entry:
     %op3 = call i32 @input() //删去了分配空间的代码
     %op4 = call i32 @input()
     %op7 = icmp slt i32 %op3, %op4 //删去了取数的代码
     %op8 = zext i1 %op7 to i32
     %op9 = icmp ne i32 %op8, 0
     br i1 %op9, label %label10, label %label14
   label10:                                                ; preds = %label_entry
     br label %label14 //删去了第一个分支的swap操作，此分支现不做任何操作
   label14:                                                ; preds = %label_entry, %label10
     %op19 = phi i32 [ %op3, %label10 ], [ undef, %label_entry ]
     //使用phi实现gcd(a, b) / gcd(b, a)的分支
     %op20 = phi i32 [ %op4, %label_entry ], [ %op3, %label10 ]
     //label14有两个前驱block: label_entry和label10，分别对a进行赋值（值为a/b），使用phi函数来确定在此块中的a应该使用哪个结果。
     %op21 = phi i32 [ %op3, %label_entry ], [ %op4, %label10 ]
     //同上对b的值进行选择
     %op17 = call i32 @gcd(i32 %op21, i32 %op20)
     call void @output(i32 %op17)
     ret void
   }
   ```
4. 基本函数块中对x的定义，则要求在支配树得到的该基本块的每个支配边境起始处都放置一个对应的PHI函数。

5.  通过var_val_stack的键值对保存左值最新的value值。当遇到load指令时，如果左值可以在var_val_stack找到，用var_val_stack中的最新值，来代替将来使用到load指令的所有value，并且将该load指令删除。对于var_val_stack的维护，每次遇到新的PHI指令和STORE指令时，将新的左值和对应的最新值压入；在最后，再将PHI指令和STORE指令的左值的最新值弹出。

### 代码阅读总结

理解了CFG和强连通分量算法，掌握了phi函数和支配树在复写传播和SSA中的作用和使用。

### 实验反馈 （可选 不会评分）

对本次实验的建议

### 组间交流 （可选）

本次实验和哪些组（记录组长学号）交流了哪一部分信息
