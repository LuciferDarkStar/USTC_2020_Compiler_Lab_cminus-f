;int callee(int a){
;  return 2 * a;
;}
;int main(){
;  return callee(110);
;}
source_filename = "fun.c"
;target的开始
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
;target的结束
;callee函数的定义
define i32 @callee(i32 ) {
  ;%0 存有参数
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  ;参数a*2
  %3 = load i32, i32* %2, align 4
  %4 = mul nsw i32 2, %3
  ;返回结果
  ret i32 %4
}

;全局函数main的定义
define i32 @main() {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  ;调用calle并将返回值存入%2
  %2 = call i32 @callee(i32 110)
  ret i32 %2
}

