; ModuleID = 'kernel.c'
source_filename = "kernel.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@coefficients = dso_local local_unnamed_addr global [32 x i32] [i32 21, i32 150, i32 375, i32 -225, i32 40, i32 61, i32 -300, i32 125, i32 21, i32 150, i32 375, i32 -225, i32 40, i32 61, i32 -300, i32 125, i32 21, i32 150, i32 375, i32 -225, i32 40, i32 61, i32 -300, i32 125, i32 21, i32 150, i32 375, i32 -225, i32 40, i32 61, i32 -300, i32 125], align 4
@input = common dso_local local_unnamed_addr global [32 x i32] zeroinitializer, align 4
@output = common dso_local local_unnamed_addr global [32 x i32] zeroinitializer, align 4

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  tail call void @kernel()
  ret i32 0
}

; Function Attrs: noinline nounwind
define dso_local void @kernel() local_unnamed_addr #1 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.end, %entry
  %j.021 = phi i32 [ 0, %entry ], [ %inc8, %for.end ]
  br label %for.body3

for.body3:                                        ; preds = %for.body3, %for.cond1.preheader
  %sum.020 = phi i32 [ 0, %for.cond1.preheader ], [ %add.3, %for.body3 ]
  %i.019 = phi i32 [ 0, %for.cond1.preheader ], [ %inc.3, %for.body3 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %arrayidx = getelementptr inbounds [32 x i32], [32 x i32]* @input, i32 0, i32 %i.019
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %arrayidx4 = getelementptr inbounds [32 x i32], [32 x i32]* @coefficients, i32 0, i32 %i.019
  %1 = load i32, i32* %arrayidx4, align 4, !tbaa !3
  %mul = mul nsw i32 %1, %0
  %add = add nsw i32 %mul, %sum.020
  %inc = or i32 %i.019, 1
  %call.1 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %arrayidx.1 = getelementptr inbounds [32 x i32], [32 x i32]* @input, i32 0, i32 %inc
  %2 = load i32, i32* %arrayidx.1, align 4, !tbaa !3
  %arrayidx4.1 = getelementptr inbounds [32 x i32], [32 x i32]* @coefficients, i32 0, i32 %inc
  %3 = load i32, i32* %arrayidx4.1, align 4, !tbaa !3
  %mul.1 = mul nsw i32 %3, %2
  %add.1 = add nsw i32 %mul.1, %add
  %inc.1 = or i32 %i.019, 2
  %call.2 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %arrayidx.2 = getelementptr inbounds [32 x i32], [32 x i32]* @input, i32 0, i32 %inc.1
  %4 = load i32, i32* %arrayidx.2, align 4, !tbaa !3
  %arrayidx4.2 = getelementptr inbounds [32 x i32], [32 x i32]* @coefficients, i32 0, i32 %inc.1
  %5 = load i32, i32* %arrayidx4.2, align 4, !tbaa !3
  %mul.2 = mul nsw i32 %5, %4
  %add.2 = add nsw i32 %mul.2, %add.1
  %inc.2 = or i32 %i.019, 3
  %call.3 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %arrayidx.3 = getelementptr inbounds [32 x i32], [32 x i32]* @input, i32 0, i32 %inc.2
  %6 = load i32, i32* %arrayidx.3, align 4, !tbaa !3
  %arrayidx4.3 = getelementptr inbounds [32 x i32], [32 x i32]* @coefficients, i32 0, i32 %inc.2
  %7 = load i32, i32* %arrayidx4.3, align 4, !tbaa !3
  %mul.3 = mul nsw i32 %7, %6
  %add.3 = add nsw i32 %mul.3, %add.2
  %inc.3 = add nuw nsw i32 %i.019, 4
  %exitcond.3 = icmp eq i32 %inc.3, 32
  br i1 %exitcond.3, label %for.end, label %for.body3, !llvm.loop !7

for.end:                                          ; preds = %for.body3
  %arrayidx5 = getelementptr inbounds [32 x i32], [32 x i32]* @output, i32 0, i32 %j.021
  %8 = load i32, i32* %arrayidx5, align 4, !tbaa !3
  %add6 = add nsw i32 %8, %add.3
  store i32 %add6, i32* %arrayidx5, align 4, !tbaa !3
  %inc8 = add nuw nsw i32 %j.021, 1
  %exitcond = icmp eq i32 %inc8, 32
  br i1 %exitcond, label %for.end9, label %for.cond1.preheader

for.end9:                                         ; preds = %for.end
  ret void
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #2

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
!7 = distinct !{!7, !8}
!8 = !{!"llvm.loop.unroll.disable"}
