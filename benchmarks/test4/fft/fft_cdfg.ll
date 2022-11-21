; ModuleID = 'fft_gvn.ll'
source_filename = "fft.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@XR0 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XI0 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XR1 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XI1 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XR2 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XI2 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XR3 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@XI3 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YR0 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YI0 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YR1 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YI1 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YR2 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YI2 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YR3 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@YI3 = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4

; Function Attrs: nounwind
define dso_local void @fft4() local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %i.0119 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %arrayidx = getelementptr inbounds [1024 x i32], [1024 x i32]* @XR0, i32 0, i32 %i.0119
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %arrayidx1 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XI0, i32 0, i32 %i.0119
  %1 = load i32, i32* %arrayidx1, align 4, !tbaa !3
  %arrayidx2 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XR1, i32 0, i32 %i.0119
  %2 = load i32, i32* %arrayidx2, align 4, !tbaa !3
  %arrayidx3 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XI1, i32 0, i32 %i.0119
  %3 = load i32, i32* %arrayidx3, align 4, !tbaa !3
  %arrayidx4 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XR2, i32 0, i32 %i.0119
  %4 = load i32, i32* %arrayidx4, align 4, !tbaa !3
  %arrayidx5 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XI2, i32 0, i32 %i.0119
  %5 = load i32, i32* %arrayidx5, align 4, !tbaa !3
  %arrayidx6 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XR3, i32 0, i32 %i.0119
  %6 = load i32, i32* %arrayidx6, align 4, !tbaa !3
  %arrayidx7 = getelementptr inbounds [1024 x i32], [1024 x i32]* @XI3, i32 0, i32 %i.0119
  %7 = load i32, i32* %arrayidx7, align 4, !tbaa !3
  %add = add nsw i32 %4, %0
  %add8 = add nsw i32 %5, %1
  %sub = sub nsw i32 %0, %4
  %sub9 = sub nsw i32 %1, %5
  %add12 = add nsw i32 %6, %2
  %add13 = add nsw i32 %7, %3
  %sub15116117 = sub i32 %3, %7
  %sub14118 = sub i32 %6, %2
  %add23 = add nsw i32 %add12, %add
  %add24 = add nsw i32 %add13, %add8
  %sub25 = sub nsw i32 %add, %add12
  %sub26 = sub nsw i32 %add8, %add13
  %add29 = add nsw i32 %sub15116117, %sub
  %add30 = add nsw i32 %sub14118, %sub9
  %sub31 = sub nsw i32 %sub, %sub15116117
  %sub32 = sub nsw i32 %sub9, %sub14118
  %arrayidx33 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YR0, i32 0, i32 %i.0119
  store i32 %add23, i32* %arrayidx33, align 4, !tbaa !3
  %arrayidx34 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YI0, i32 0, i32 %i.0119
  store i32 %add24, i32* %arrayidx34, align 4, !tbaa !3
  %arrayidx35 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YR1, i32 0, i32 %i.0119
  store i32 %sub25, i32* %arrayidx35, align 4, !tbaa !3
  %arrayidx36 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YI1, i32 0, i32 %i.0119
  store i32 %sub26, i32* %arrayidx36, align 4, !tbaa !3
  %arrayidx37 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YR2, i32 0, i32 %i.0119
  store i32 %add29, i32* %arrayidx37, align 4, !tbaa !3
  %arrayidx38 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YI2, i32 0, i32 %i.0119
  store i32 %add30, i32* %arrayidx38, align 4, !tbaa !3
  %arrayidx39 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YR3, i32 0, i32 %i.0119
  store i32 %sub31, i32* %arrayidx39, align 4, !tbaa !3
  %arrayidx40 = getelementptr inbounds [1024 x i32], [1024 x i32]* @YI3, i32 0, i32 %i.0119
  store i32 %sub32, i32* %arrayidx40, align 4, !tbaa !3
  %inc = add nuw nsw i32 %i.0119, 1
  %exitcond = icmp eq i32 %inc, 1024
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body
  ret void
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
