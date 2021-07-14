;int main(){
;  float a = 5.555;
;  if(a > 1)
;    return 233;
;  return 0;
;}
source_filename = "if.c"
;target的开始
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
;target的结束
;全局函数main的定义
define dso_local i32 @main() #0 {
  ;定义浮点数a
  %1 = alloca float, align 4
  store float 0x40163851E0000000, float* %1, align 4
  %2 = load float, float* %1, align 4
  ;a与1比较
  %3 = fcmp ogt float %2, 1.000000e+00
  ;根据比较结果跳转
  br i1 %3, label %true, label %false

true:                                                
  ;返回233
  %4 = alloca i32
  store i32 233, i32* %4
  %5 = load i32, i32* %4
  ret i32 %5
  

false:
  ;返回0                                               
  %6 = alloca i32
  store i32 0, i32* %6
  %7 = load i32, i32* %6
  ret i32 %7
}
