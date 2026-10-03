; ModuleID = '..\test\Algebraic_1.c'
source_filename = "..\\test\\Algebraic_1.c"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @compute(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %6 = load i32, ptr %3, align 4
  %7 = load i32, ptr %3, align 4
  %8 = sdiv i32 %6, %7
  store i32 %8, ptr %5, align 4
  %9 = load i32, ptr %4, align 4
  %10 = load i32, ptr %4, align 4
  %11 = sdiv i32 %9, %10
  %12 = load i32, ptr %5, align 4
  %13 = mul nsw i32 %12, %11
  store i32 %13, ptr %5, align 4
  %14 = load i32, ptr %4, align 4
  %15 = load i32, ptr %4, align 4
  %16 = sub nsw i32 %14, %15
  %17 = load i32, ptr %5, align 4
  %18 = add nsw i32 %17, %16
  store i32 %18, ptr %5, align 4
  %19 = load i32, ptr %5, align 4
  %20 = load i32, ptr %5, align 4
  %21 = sdiv i32 %20, %19
  store i32 %21, ptr %5, align 4
  %22 = load i32, ptr %5, align 4
  %23 = load i32, ptr %5, align 4
  %24 = sub nsw i32 %23, %22
  store i32 %24, ptr %5, align 4
  %25 = load i32, ptr %5, align 4
  %26 = add nsw i32 %25, 23
  store i32 %26, ptr %5, align 4
  %27 = load i32, ptr %5, align 4
  ret i32 %27
}

attributes #0 = { noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6}
!llvm.ident = !{!7}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang version 22.1.7 (https://github.com/msys2/MINGW-packages b8f306ad57e0c8f3f15aed0dc809d7e3e8e2eee9)", isOptimized: false, runtimeVersion: 0, emissionKind: NoDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "..\\test/Algebraic_1.c", directory: "C:/Users/mcw/Documents/Adil/Compilers_class/llvm-project/build")
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 2}
!4 = !{i32 8, !"PIC Level", i32 2}
!5 = !{i32 7, !"uwtable", i32 2}
!6 = !{i32 1, !"MaxTLSAlign", i32 65536}
!7 = !{!"clang version 22.1.7 (https://github.com/msys2/MINGW-packages b8f306ad57e0c8f3f15aed0dc809d7e3e8e2eee9)"}
