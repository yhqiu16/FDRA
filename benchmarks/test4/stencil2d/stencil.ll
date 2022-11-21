; ModuleID = 'stencil.c'
source_filename = "stencil.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@filter = common dso_local local_unnamed_addr global [9 x i32] zeroinitializer, align 4
@orig = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@sol = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4

; Function Attrs: nounwind
define dso_local void @stencil() local_unnamed_addr #0 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.inc27, %entry
  %r.053 = phi i32 [ 0, %entry ], [ %inc28, %for.inc27 ]
  %mul21 = shl i32 %r.053, 5
  br label %for.cond4.preheader

for.cond4.preheader:                              ; preds = %for.end20, %for.cond1.preheader
  %c.052 = phi i32 [ 0, %for.cond1.preheader ], [ %inc25, %for.end20 ]
  br label %for.body6

for.body6:                                        ; preds = %for.body6, %for.cond4.preheader
  %temp.051 = phi i32 [ 0, %for.cond4.preheader ], [ %add17.2, %for.body6 ]
  %k1.050 = phi i32 [ 0, %for.cond4.preheader ], [ %inc19, %for.body6 ]
  %call = call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %mul10 = mul nuw nsw i32 %k1.050, 3
  %add11 = add nuw nsw i32 %k1.050, %r.053
  %mul12 = shl i32 %add11, 5
  %add13 = add nuw nsw i32 %mul12, %c.052
  %arrayidx = getelementptr inbounds [9 x i32], [9 x i32]* @filter, i32 0, i32 %mul10
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %arrayidx15 = getelementptr inbounds [1024 x i32], [1024 x i32]* @orig, i32 0, i32 %add13
  %1 = load i32, i32* %arrayidx15, align 4, !tbaa !3
  %mul16 = mul nsw i32 %1, %0
  %add17 = add nsw i32 %mul16, %temp.051
  %add.1 = add nuw nsw i32 %mul10, 1
  %arrayidx.1 = getelementptr inbounds [9 x i32], [9 x i32]* @filter, i32 0, i32 %add.1
  %2 = load i32, i32* %arrayidx.1, align 4, !tbaa !3
  %add14.1 = add nuw nsw i32 %add13, 1
  %arrayidx15.1 = getelementptr inbounds [1024 x i32], [1024 x i32]* @orig, i32 0, i32 %add14.1
  %3 = load i32, i32* %arrayidx15.1, align 4, !tbaa !3
  %mul16.1 = mul nsw i32 %3, %2
  %add17.1 = add nsw i32 %mul16.1, %add17
  %add.2 = add nuw nsw i32 %mul10, 2
  %arrayidx.2 = getelementptr inbounds [9 x i32], [9 x i32]* @filter, i32 0, i32 %add.2
  %4 = load i32, i32* %arrayidx.2, align 4, !tbaa !3
  %add14.2 = add nuw nsw i32 %add13, 2
  %arrayidx15.2 = getelementptr inbounds [1024 x i32], [1024 x i32]* @orig, i32 0, i32 %add14.2
  %5 = load i32, i32* %arrayidx15.2, align 4, !tbaa !3
  %mul16.2 = mul nsw i32 %5, %4
  %add17.2 = add nsw i32 %mul16.2, %add17.1
  %inc19 = add nuw nsw i32 %k1.050, 1
  %exitcond = icmp eq i32 %inc19, 3
  br i1 %exitcond, label %for.end20, label %for.body6

for.end20:                                        ; preds = %for.body6
  %add22 = add nuw nsw i32 %c.052, %mul21
  %arrayidx23 = getelementptr inbounds [1024 x i32], [1024 x i32]* @sol, i32 0, i32 %add22
  store i32 %add17.2, i32* %arrayidx23, align 4, !tbaa !3
  %inc25 = add nuw nsw i32 %c.052, 1
  %exitcond54 = icmp eq i32 %inc25, 30
  br i1 %exitcond54, label %for.inc27, label %for.cond4.preheader

for.inc27:                                        ; preds = %for.end20
  %inc28 = add nuw nsw i32 %r.053, 1
  %exitcond55 = icmp eq i32 %inc28, 30
  br i1 %exitcond55, label %for.end29, label %for.cond1.preheader

for.end29:                                        ; preds = %for.inc27
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
