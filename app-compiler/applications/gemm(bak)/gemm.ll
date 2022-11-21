; ModuleID = 'gemm.c'
source_filename = "gemm.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@n = dso_local local_unnamed_addr global i32 10, align 4
@A = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@C = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@i = common dso_local local_unnamed_addr global i32 0, align 4
@j = common dso_local local_unnamed_addr global i32 0, align 4

; Function Attrs: nounwind
define dso_local void @gemm() local_unnamed_addr #0 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.inc17, %entry
  %i.036 = phi i32 [ 0, %entry ], [ %inc18, %for.inc17 ]
  br label %for.cond4.preheader

for.cond4.preheader:                              ; preds = %for.inc14, %for.cond1.preheader
  %j.035 = phi i32 [ 0, %for.cond1.preheader ], [ %inc15, %for.inc14 ]
  %arrayidx7 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %i.036, i32 %j.035
  br label %for.body6

for.body6:                                        ; preds = %for.body6, %for.cond4.preheader
  %k.034 = phi i32 [ 0, %for.cond4.preheader ], [ %inc, %for.body6 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %0 = load i32, i32* %arrayidx7, align 4, !tbaa !3
  %arrayidx9 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %i.036, i32 %k.034
  %1 = load i32, i32* %arrayidx9, align 4, !tbaa !3
  %arrayidx11 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %k.034, i32 %j.035
  %2 = load i32, i32* %arrayidx11, align 4, !tbaa !3
  %mul = mul nsw i32 %2, %1
  %add = add nsw i32 %mul, %0
  store i32 %add, i32* %arrayidx7, align 4, !tbaa !3
  %inc = add nuw nsw i32 %k.034, 1
  %exitcond = icmp eq i32 %inc, 10
  br i1 %exitcond, label %for.inc14, label %for.body6

for.inc14:                                        ; preds = %for.body6
  %inc15 = add nuw nsw i32 %j.035, 1
  %exitcond37 = icmp eq i32 %inc15, 10
  br i1 %exitcond37, label %for.inc17, label %for.cond4.preheader

for.inc17:                                        ; preds = %for.inc14
  %inc18 = add nuw nsw i32 %i.036, 1
  %exitcond38 = icmp eq i32 %inc18, 10
  br i1 %exitcond38, label %for.end19, label %for.cond1.preheader

for.end19:                                        ; preds = %for.inc17
  ret void
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nounwind
define dso_local void @main() local_unnamed_addr #0 {
entry:
  %0 = load i32, i32* @n, align 4, !tbaa !3
  %cmp51 = icmp sgt i32 %0, 0
  br i1 %cmp51, label %for.cond1.preheader.lr.ph, label %for.cond1.preheader.i.preheader

for.cond1.preheader.lr.ph:                        ; preds = %entry
  %1 = shl nuw i32 %0, 2
  br label %for.body3.preheader

for.body3.preheader:                              ; preds = %for.inc9, %for.cond1.preheader.lr.ph
  %i.052 = phi i32 [ 0, %for.cond1.preheader.lr.ph ], [ %inc10, %for.inc9 ]
  %scevgep = getelementptr [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %i.052, i32 0
  %scevgep54 = bitcast i32* %scevgep to i8*
  call void @llvm.memset.p0i8.i32(i8* align 4 %scevgep54, i8 0, i32 %1, i1 false)
  br label %for.body3

for.body3:                                        ; preds = %for.body3, %for.body3.preheader
  %j.050 = phi i32 [ %inc, %for.body3 ], [ 0, %for.body3.preheader ]
  %add = add nuw nsw i32 %j.050, %i.052
  %arrayidx6 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %i.052, i32 %j.050
  store i32 %add, i32* %arrayidx6, align 4, !tbaa !3
  %mul = mul nsw i32 %j.050, %i.052
  %arrayidx8 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %i.052, i32 %j.050
  store i32 %mul, i32* %arrayidx8, align 4, !tbaa !3
  %inc = add nuw nsw i32 %j.050, 1
  %exitcond = icmp eq i32 %inc, %0
  br i1 %exitcond, label %for.inc9, label %for.body3

for.inc9:                                         ; preds = %for.body3
  %inc10 = add nuw nsw i32 %i.052, 1
  %cmp = icmp slt i32 %inc10, %0
  br i1 %cmp, label %for.body3.preheader, label %for.cond1.preheader.i.preheader

for.cond1.preheader.i.preheader:                  ; preds = %for.inc9, %entry
  br label %for.cond1.preheader.i

for.cond1.preheader.i:                            ; preds = %for.cond1.preheader.i.preheader, %for.inc17.i
  %i.036.i = phi i32 [ %inc18.i, %for.inc17.i ], [ 0, %for.cond1.preheader.i.preheader ]
  br label %for.cond4.preheader.i

for.cond4.preheader.i:                            ; preds = %for.inc14.i, %for.cond1.preheader.i
  %j.035.i = phi i32 [ 0, %for.cond1.preheader.i ], [ %inc15.i, %for.inc14.i ]
  %arrayidx7.i = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %i.036.i, i32 %j.035.i
  br label %for.body6.i

for.body6.i:                                      ; preds = %for.body6.i, %for.cond4.preheader.i
  %k.034.i = phi i32 [ 0, %for.cond4.preheader.i ], [ %inc.i, %for.body6.i ]
  %call.i = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %2 = load i32, i32* %arrayidx7.i, align 4, !tbaa !3
  %arrayidx9.i = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %i.036.i, i32 %k.034.i
  %3 = load i32, i32* %arrayidx9.i, align 4, !tbaa !3
  %arrayidx11.i = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %k.034.i, i32 %j.035.i
  %4 = load i32, i32* %arrayidx11.i, align 4, !tbaa !3
  %mul.i = mul nsw i32 %4, %3
  %add.i = add nsw i32 %mul.i, %2
  store i32 %add.i, i32* %arrayidx7.i, align 4, !tbaa !3
  %inc.i = add nuw nsw i32 %k.034.i, 1
  %exitcond.i = icmp eq i32 %inc.i, 10
  br i1 %exitcond.i, label %for.inc14.i, label %for.body6.i

for.inc14.i:                                      ; preds = %for.body6.i
  %inc15.i = add nuw nsw i32 %j.035.i, 1
  %exitcond37.i = icmp eq i32 %inc15.i, 10
  br i1 %exitcond37.i, label %for.inc17.i, label %for.cond4.preheader.i

for.inc17.i:                                      ; preds = %for.inc14.i
  %inc18.i = add nuw nsw i32 %i.036.i, 1
  %exitcond38.i = icmp eq i32 %inc18.i, 10
  br i1 %exitcond38.i, label %for.cond12.preheader, label %for.cond1.preheader.i

for.cond12.preheader:                             ; preds = %for.inc17.i
  %5 = load i32, i32* @n, align 4, !tbaa !3
  %cmp1346 = icmp sgt i32 %5, 0
  br i1 %cmp1346, label %for.cond15.preheader, label %for.end25

for.cond15.preheader:                             ; preds = %for.cond12.preheader, %for.inc23
  %6 = phi i32 [ %9, %for.inc23 ], [ %5, %for.cond12.preheader ]
  %i.147 = phi i32 [ %inc24, %for.inc23 ], [ 0, %for.cond12.preheader ]
  %cmp1644 = icmp sgt i32 %6, 0
  br i1 %cmp1644, label %for.body17, label %for.inc23

for.body17:                                       ; preds = %for.cond15.preheader, %for.body17
  %j.145 = phi i32 [ %inc21, %for.body17 ], [ 0, %for.cond15.preheader ]
  %arrayidx19 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %i.147, i32 %j.145
  %7 = load i32, i32* %arrayidx19, align 4, !tbaa !3
  %call = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %7)
  %inc21 = add nuw nsw i32 %j.145, 1
  %8 = load i32, i32* @n, align 4, !tbaa !3
  %cmp16 = icmp slt i32 %inc21, %8
  br i1 %cmp16, label %for.body17, label %for.inc23

for.inc23:                                        ; preds = %for.body17, %for.cond15.preheader
  %9 = phi i32 [ %6, %for.cond15.preheader ], [ %8, %for.body17 ]
  %inc24 = add nuw nsw i32 %i.147, 1
  %cmp13 = icmp slt i32 %inc24, %9
  br i1 %cmp13, label %for.cond15.preheader, label %for.end25

for.end25:                                        ; preds = %for.inc23, %for.cond12.preheader
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i32(i8* nocapture writeonly, i8, i32, i1 immarg) #3

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind willreturn }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
