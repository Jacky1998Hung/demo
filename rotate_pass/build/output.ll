; ModuleID = 'test.ll'
source_filename = "test.c"
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
  %indvar.load = load i32, ptr %3, align 4
  %guard.cond = icmp slt i32 %indvar.load, 4
  br i1 %guard.cond, label %4, label %11

4:                                                ; preds = %8, %0
  %5 = load i32, ptr %3, align 4
  %6 = load i32, ptr %2, align 4
  %7 = add nsw i32 %6, %5
  store i32 %7, ptr %2, align 4
  br label %8

8:                                                ; preds = %4
  %9 = load i32, ptr %3, align 4
  %10 = add nsw i32 %9, 1
  store i32 %10, ptr %3, align 4
  %latchindvar.load = load i32, ptr %3, align 4
  %latchguard.cond = icmp slt i32 %latchindvar.load, 4
  br i1 %latchguard.cond, label %4, label %11

11:                                               ; preds = %8, %0
  %12 = load i32, ptr %1, align 4
  ret i32 %12
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
