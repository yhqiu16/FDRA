; ModuleID = 'test_unroll.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@a = dso_local local_unnamed_addr global [20 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20], align 4
@b = dso_local local_unnamed_addr global [20 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20], align 4
@result = common dso_local local_unnamed_addr global i32 0, align 4
@N = common dso_local local_unnamed_addr global i32* null, align 4

; Function Attrs: nounwind
define dso_local i32 @test() local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %sum.011 = phi i32 [ 0, %entry ], [ %add, %for.body ]
  %i.010 = phi i32 [ 1, %entry ], [ %inc, %for.body ]
  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* @a, i32 0, i32 %i.010
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %arrayidx1 = getelementptr inbounds [20 x i32], [20 x i32]* @b, i32 0, i32 %i.010
  %1 = load i32, i32* %arrayidx1, align 4, !tbaa !3
  %mul = mul nsw i32 %1, %0
  %add = add nsw i32 %mul, %sum.011
  %inc = add nuw nsw i32 %i.010, 1
  %exitcond = icmp eq i32 %inc, 19
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body
  %add.lcssa = phi i32 [ %add, %for.body ]
  store i32 %add.lcssa, i32* @result, align 4, !tbaa !3
  ret i32 %add.lcssa
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
