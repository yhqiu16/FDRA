; ModuleID = 'gemm.ll'
source_filename = "gemm.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@m1 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@m2 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@prod = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4

; Function Attrs: nounwind
define dso_local void @gemm(i32* nocapture readnone %0, i32* nocapture readnone %1, i32* nocapture readnone %2) local_unnamed_addr #0 {
entry:
  br label %for.cond3.preheader

for.cond3.preheader:                              ; preds = %for.inc19, %entry
  %i.041 = phi i32 [ 0, %entry ], [ %inc20, %for.inc19 ]
  %mul = shl nsw i32 %i.041, 5
  br label %for.body5

for.body5:                                        ; preds = %for.end, %for.cond3.preheader
  %j.040 = phi i32 [ 0, %for.cond3.preheader ], [ %inc17, %for.end ]
  br label %for.body8

for.body8:                                        ; preds = %for.body8, %for.body5
  %sum.039 = phi i32 [ 0, %for.body5 ], [ %add13.3, %for.body8 ]
  %k.038 = phi i32 [ 0, %for.body5 ], [ %inc.3, %for.body8 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %mul9 = shl nuw nsw i32 %k.038, 5
  %add = add nuw nsw i32 %k.038, %mul
  %arrayidx = getelementptr inbounds [1024 x i32], [1024 x i32]* @m1, i32 0, i32 %add
  %3 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %add10 = add nuw nsw i32 %mul9, %j.040
  %arrayidx11 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m2, i32 0, i32 %add10
  %4 = load i32, i32* %arrayidx11, align 4, !tbaa !3
  %mul12 = mul nsw i32 %4, %3
  %add13 = add nsw i32 %mul12, %sum.039
  %inc = or i32 %k.038, 1
  %call.1 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %mul9.1 = shl nuw nsw i32 %inc, 5
  %add.1 = add nuw nsw i32 %inc, %mul
  %arrayidx.1 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m1, i32 0, i32 %add.1
  %5 = load i32, i32* %arrayidx.1, align 4, !tbaa !3
  %add10.1 = add nuw nsw i32 %mul9.1, %j.040
  %arrayidx11.1 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m2, i32 0, i32 %add10.1
  %6 = load i32, i32* %arrayidx11.1, align 4, !tbaa !3
  %mul12.1 = mul nsw i32 %6, %5
  %add13.1 = add nsw i32 %mul12.1, %add13
  %inc.1 = or i32 %k.038, 2
  %call.2 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %mul9.2 = shl nuw nsw i32 %inc.1, 5
  %add.2 = add nuw nsw i32 %inc.1, %mul
  %arrayidx.2 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m1, i32 0, i32 %add.2
  %7 = load i32, i32* %arrayidx.2, align 4, !tbaa !3
  %add10.2 = add nuw nsw i32 %mul9.2, %j.040
  %arrayidx11.2 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m2, i32 0, i32 %add10.2
  %8 = load i32, i32* %arrayidx11.2, align 4, !tbaa !3
  %mul12.2 = mul nsw i32 %8, %7
  %add13.2 = add nsw i32 %mul12.2, %add13.1
  %inc.2 = or i32 %k.038, 3
  %call.3 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %mul9.3 = shl nuw nsw i32 %inc.2, 5
  %add.3 = add nuw nsw i32 %inc.2, %mul
  %arrayidx.3 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m1, i32 0, i32 %add.3
  %9 = load i32, i32* %arrayidx.3, align 4, !tbaa !3
  %add10.3 = add nuw nsw i32 %mul9.3, %j.040
  %arrayidx11.3 = getelementptr inbounds [1024 x i32], [1024 x i32]* @m2, i32 0, i32 %add10.3
  %10 = load i32, i32* %arrayidx11.3, align 4, !tbaa !3
  %mul12.3 = mul nsw i32 %10, %9
  %add13.3 = add nsw i32 %mul12.3, %add13.2
  %inc.3 = add nuw nsw i32 %k.038, 4
  %exitcond.3 = icmp eq i32 %inc.3, 32
  br i1 %exitcond.3, label %for.end, label %for.body8, !llvm.loop !7

for.end:                                          ; preds = %for.body8
  %add13.3.lcssa = phi i32 [ %add13.3, %for.body8 ]
  %add14 = add nuw nsw i32 %j.040, %mul
  %arrayidx15 = getelementptr inbounds [1024 x i32], [1024 x i32]* @prod, i32 0, i32 %add14
  store i32 %add13.3.lcssa, i32* %arrayidx15, align 4, !tbaa !3
  %inc17 = add nuw nsw i32 %j.040, 1
  %exitcond = icmp eq i32 %inc17, 32
  br i1 %exitcond, label %for.inc19, label %for.body5

for.inc19:                                        ; preds = %for.end
  %inc20 = add nuw nsw i32 %i.041, 1
  %exitcond42 = icmp eq i32 %inc20, 32
  br i1 %exitcond42, label %for.end21, label %for.cond3.preheader

for.end21:                                        ; preds = %for.inc19
  ret void
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

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
