;int main(){
;  int a[10];
;  a[0] = 10;
;  a[1] = a[0] * 2;
;  return a[1];
;}
source_filename = "assign.c"
;target的开始
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
;target的结束
;全局函数main的定义
define i32 @main() {
  ;定义数组a[10]
  %1 = alloca [10 x i32], align 16
  %2 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  ;a[0]=10
  store i32 10, i32* %2, align 16
  %3 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  ;取出a[0],并a[0]*2
  %4 = load i32, i32* %3, align 16
  %5 = mul nsw i32 %4, 2
  ;a[1]=a[0]*2
  %6 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  store i32 %5, i32* %6, align 4
  ;取出a[1]并返回
  %7 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  %8 = load i32, i32* %7, align 4
  ret i32 %8
}
