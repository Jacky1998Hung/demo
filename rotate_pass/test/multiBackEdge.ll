; ModuleID = 'multiBackEdge.c'
source_filename = "multiBackEdge.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  br label %4

4:                                                ; preds = %27, %0
  %5 = load i32, ptr %2, align 4
  %6 = icmp slt i32 %5, 10
  br i1 %6, label %7, label %28

7:                                                ; preds = %4
  %8 = load i32, ptr %2, align 4
  %9 = srem i32 %8, 2
  %10 = icmp eq i32 %9, 0
  br i1 %10, label %11, label %14

11:                                               ; preds = %7
  %12 = load i32, ptr %2, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, ptr %2, align 4
  br label %27

14:                                               ; preds = %7
  %15 = load i32, ptr %3, align 4
  %16 = icmp sgt i32 %15, 15
  br i1 %16, label %17, label %20

17:                                               ; preds = %14
  %18 = load i32, ptr %2, align 4
  %19 = add nsw i32 %18, 2
  store i32 %19, ptr %2, align 4
  br label %26

20:                                               ; preds = %14
  %21 = load i32, ptr %2, align 4
  %22 = load i32, ptr %3, align 4
  %23 = add nsw i32 %22, %21
  store i32 %23, ptr %3, align 4
  %24 = load i32, ptr %2, align 4
  %25 = add nsw i32 %24, 1
  store i32 %25, ptr %2, align 4
  br label %26

26:                                               ; preds = %20, %17
  br label %27

27:                                               ; preds = %26, %11
  br label %4, !llvm.loop !6

28:                                               ; preds = %4
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git 916bae2d921705c8ce78a4ddec4503c61bc8220c)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
