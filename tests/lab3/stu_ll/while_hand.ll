;int main(){
;  int a;
;  int i;
;  a = 10;
;  i = 0;
;  while(i < 10){
;    i = i + 1;
;    a = a + i;
;  }
;  return a;
;}
source_filename = "while.c"
;target的开始
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
;target的结束
;全局函数main的定义

define i32 @main() {
entry:
  %0 = alloca i32
  %1 = alloca i32
  store i32 10, i32* %0
  store i32 0, i32* %1
  ; a=10
  %2 = load i32, i32* %0
  ; i=0
  %3 = load i32, i32* %1
  ; i是否小于10
  %4 = icmp slt i32 %3, 10
  br i1 %4, label %true, label %end
true:
  ; i=i + 1
  %5 = load i32, i32* %1
  %6 = add nsw i32 %5, 1
  store i32 %6, i32* %1
  ; a=a + i
  %7 = load i32, i32* %0
  %8 = add nsw i32 %7, %6
  store i32 %8, i32* %0
  ;再次判断i是否小于10
  %9 = icmp slt i32 %6, 10
  ;根据结果结束循环或者继续循环
  br i1 %9, label %true, label %end
end:
  %10 = load i32, i32* %0
  ret i32 %10
}
