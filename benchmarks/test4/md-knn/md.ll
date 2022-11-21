; ModuleID = 'md.c'
source_filename = "md.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@position_x = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4
@position_y = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4
@position_z = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4
@NL = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@force_x = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4
@force_y = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4
@force_z = common dso_local local_unnamed_addr global [64 x i32] zeroinitializer, align 4

; Function Attrs: nounwind
define dso_local void @md_kernel(i32* nocapture readnone %0, i32* nocapture readnone %1, i32* nocapture readnone %2, i32* nocapture readnone %3, i32* nocapture readnone %4, i32* nocapture readnone %5, i32* nocapture readnone %6) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.end, %entry
  %i.091 = phi i32 [ 0, %entry ], [ %inc39, %for.end ]
  %arrayidx = getelementptr inbounds [64 x i32], [64 x i32]* @position_x, i32 0, i32 %i.091
  %7 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %arrayidx7 = getelementptr inbounds [64 x i32], [64 x i32]* @position_y, i32 0, i32 %i.091
  %8 = load i32, i32* %arrayidx7, align 4, !tbaa !3
  %arrayidx8 = getelementptr inbounds [64 x i32], [64 x i32]* @position_z, i32 0, i32 %i.091
  %9 = load i32, i32* %arrayidx8, align 4, !tbaa !3
  %mul = shl i32 %i.091, 4
  br label %for.body11

for.body11:                                       ; preds = %for.body11, %for.body
  %fx.090 = phi i32 [ 0, %for.body ], [ %add30, %for.body11 ]
  %j.089 = phi i32 [ 0, %for.body ], [ %inc, %for.body11 ]
  %fz.088 = phi i32 [ 0, %for.body ], [ %add34, %for.body11 ]
  %fy.087 = phi i32 [ 0, %for.body ], [ %add32, %for.body11 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #2
  %add = add nuw nsw i32 %j.089, %mul
  %arrayidx12 = getelementptr inbounds [1024 x i32], [1024 x i32]* @NL, i32 0, i32 %add
  %10 = load i32, i32* %arrayidx12, align 4, !tbaa !3
  %arrayidx13 = getelementptr inbounds [64 x i32], [64 x i32]* @position_x, i32 0, i32 %10
  %11 = load i32, i32* %arrayidx13, align 4, !tbaa !3
  %arrayidx14 = getelementptr inbounds [64 x i32], [64 x i32]* @position_y, i32 0, i32 %10
  %12 = load i32, i32* %arrayidx14, align 4, !tbaa !3
  %arrayidx15 = getelementptr inbounds [64 x i32], [64 x i32]* @position_z, i32 0, i32 %10
  %13 = load i32, i32* %arrayidx15, align 4, !tbaa !3
  %sub = sub nsw i32 %7, %11
  %sub16 = sub nsw i32 %8, %12
  %sub17 = sub nsw i32 %9, %13
  %mul18 = mul nsw i32 %sub, %sub
  %mul19 = mul nsw i32 %sub16, %sub16
  %add20 = add nuw nsw i32 %mul19, %mul18
  %mul21 = mul nsw i32 %sub17, %sub17
  %add22 = add nuw nsw i32 %add20, %mul21
  %div = udiv i32 10, %add22
  %mul23 = mul nuw nsw i32 %div, %div
  %mul24 = mul nuw nsw i32 %mul23, %div
  %sub26 = add nuw nsw i32 %mul24, -2000
  %mul27 = mul nuw nsw i32 %mul24, %div
  %mul28 = mul nsw i32 %mul27, %sub26
  %mul29 = mul nsw i32 %mul28, %sub
  %add30 = add nsw i32 %mul29, %fx.090
  %mul31 = mul nsw i32 %mul28, %sub16
  %add32 = add nsw i32 %mul31, %fy.087
  %mul33 = mul nsw i32 %mul28, %sub17
  %add34 = add nsw i32 %mul33, %fz.088
  %inc = add nuw nsw i32 %j.089, 1
  %exitcond = icmp eq i32 %inc, 16
  br i1 %exitcond, label %for.end, label %for.body11

for.end:                                          ; preds = %for.body11
  %arrayidx35 = getelementptr inbounds [64 x i32], [64 x i32]* @force_x, i32 0, i32 %i.091
  store i32 %add30, i32* %arrayidx35, align 4, !tbaa !3
  %arrayidx36 = getelementptr inbounds [64 x i32], [64 x i32]* @force_y, i32 0, i32 %i.091
  store i32 %add32, i32* %arrayidx36, align 4, !tbaa !3
  %arrayidx37 = getelementptr inbounds [64 x i32], [64 x i32]* @force_z, i32 0, i32 %i.091
  store i32 %add34, i32* %arrayidx37, align 4, !tbaa !3
  %inc39 = add nuw nsw i32 %i.091, 1
  %exitcond92 = icmp eq i32 %inc39, 64
  br i1 %exitcond92, label %for.end40, label %for.body

for.end40:                                        ; preds = %for.end
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
