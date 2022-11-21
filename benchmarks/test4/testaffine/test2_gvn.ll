; ModuleID = 'test2.ll'
source_filename = "test2.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@A = common dso_local local_unnamed_addr global [30 x i32] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [32 x i32] zeroinitializer, align 4
@C = common dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 4
@E = common dso_local local_unnamed_addr global [16 x [16 x i32]] zeroinitializer, align 4
@D1 = common dso_local local_unnamed_addr global [10 x [16 x [16 x i32]]] zeroinitializer, align 4
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@D = common dso_local local_unnamed_addr global [10 x [16 x [16 x i32]]] zeroinitializer, align 4

; Function Attrs: nofree norecurse nounwind
define dso_local void @test() local_unnamed_addr #0 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.cond.cleanup3, %entry
  %i.045 = phi i32 [ 2, %entry ], [ %add24, %for.cond.cleanup3 ]
  %mul = mul nuw nsw i32 %i.045, 3
  %arrayidx = getelementptr inbounds [30 x i32], [30 x i32]* @A, i32 0, i32 %mul
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  br label %for.cond5.preheader

for.cond.cleanup:                                 ; preds = %for.cond.cleanup3
  ret void

for.cond5.preheader:                              ; preds = %for.cond.cleanup7, %for.cond1.preheader
  %j.044 = phi i32 [ 15, %for.cond1.preheader ], [ %dec, %for.cond.cleanup7 ]
  %mul11 = mul nsw i32 %j.044, %j.044
  %arrayidx12 = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %mul11
  %1 = load i32, i32* %arrayidx12, align 4, !tbaa !3
  br label %for.body8

for.cond.cleanup3:                                ; preds = %for.cond.cleanup7
  %add24 = add nuw nsw i32 %i.045, 2
  %cmp = icmp ult i32 %i.045, 8
  br i1 %cmp, label %for.cond1.preheader, label %for.cond.cleanup

for.cond.cleanup7:                                ; preds = %for.body8
  %dec = add nsw i32 %j.044, -1
  %cmp2 = icmp eq i32 %j.044, 0
  br i1 %cmp2, label %for.cond.cleanup3, label %for.cond5.preheader

for.body8:                                        ; preds = %for.body8, %for.cond5.preheader
  %k.043 = phi i32 [ 0, %for.cond5.preheader ], [ %add20, %for.body8 ]
  %add = add nuw nsw i32 %k.043, %j.044
  %arrayidx9 = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %add
  %2 = load i32, i32* %arrayidx9, align 4, !tbaa !3
  %add10 = add nsw i32 %2, %0
  %arrayidx14 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %j.044, i32 %k.043
  %3 = load i32, i32* %arrayidx14, align 4, !tbaa !3
  %mul15 = mul nsw i32 %3, %1
  %add16 = add nsw i32 %add10, %mul15
  %arrayidx19 = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D1, i32 0, i32 %i.045, i32 %j.044, i32 %k.043
  store i32 %add16, i32* %arrayidx19, align 4, !tbaa !3
  %add20 = add nuw nsw i32 %k.043, 3
  %cmp6 = icmp ult i32 %k.043, 13
  br i1 %cmp6, label %for.body8, label %for.cond.cleanup7
}

; Function Attrs: nofree nounwind
define dso_local i32 @main() local_unnamed_addr #1 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %i.0100 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %mul = shl nuw nsw i32 %i.0100, 1
  %add = add nuw nsw i32 %mul, 5
  %arrayidx = getelementptr inbounds [30 x i32], [30 x i32]* @A, i32 0, i32 %i.0100
  store i32 %add, i32* %arrayidx, align 4, !tbaa !3
  %inc = add nuw nsw i32 %i.0100, 1
  %exitcond106 = icmp eq i32 %inc, 30
  br i1 %exitcond106, label %for.body5.preheader, label %for.body

for.body5.preheader:                              ; preds = %for.body
  br label %for.body5

for.body5:                                        ; preds = %for.body5.preheader, %for.body5
  %i1.099 = phi i32 [ %inc9, %for.body5 ], [ 0, %for.body5.preheader ]
  %mul6 = mul nuw nsw i32 %i1.099, 3
  %arrayidx7 = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %i1.099
  store i32 %mul6, i32* %arrayidx7, align 4, !tbaa !3
  %inc9 = add nuw nsw i32 %i1.099, 1
  %exitcond105 = icmp eq i32 %inc9, 32
  br i1 %exitcond105, label %for.body15.preheader, label %for.body5

for.body15.preheader:                             ; preds = %for.body5
  br label %for.body15

for.body15:                                       ; preds = %for.body15.preheader, %for.body15
  %i11.098 = phi i32 [ %inc18, %for.body15 ], [ 0, %for.body15.preheader ]
  %arrayidx16 = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %i11.098
  store i32 %i11.098, i32* %arrayidx16, align 4, !tbaa !3
  %inc18 = add nuw nsw i32 %i11.098, 1
  %exitcond104 = icmp eq i32 %inc18, 256
  br i1 %exitcond104, label %for.cond24.preheader.preheader, label %for.body15

for.cond24.preheader.preheader:                   ; preds = %for.body15
  br label %for.cond24.preheader

for.cond24.preheader:                             ; preds = %for.cond24.preheader.preheader, %for.cond.cleanup26
  %j.097 = phi i32 [ %inc35, %for.cond.cleanup26 ], [ 0, %for.cond24.preheader.preheader ]
  br label %for.body27

for.cond1.preheader.i:                            ; preds = %for.cond1.preheader.i.preheader, %for.cond.cleanup3.i
  %i.045.i = phi i32 [ %add24.i, %for.cond.cleanup3.i ], [ 2, %for.cond1.preheader.i.preheader ]
  %mul.i = mul nuw nsw i32 %i.045.i, 3
  %arrayidx.i = getelementptr inbounds [30 x i32], [30 x i32]* @A, i32 0, i32 %mul.i
  %0 = load i32, i32* %arrayidx.i, align 4, !tbaa !3
  br label %for.cond5.preheader.i

for.cond5.preheader.i:                            ; preds = %for.cond.cleanup7.i, %for.cond1.preheader.i
  %j.044.i = phi i32 [ 15, %for.cond1.preheader.i ], [ %dec.i, %for.cond.cleanup7.i ]
  %mul11.i = mul nsw i32 %j.044.i, %j.044.i
  %arrayidx12.i = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %mul11.i
  %1 = load i32, i32* %arrayidx12.i, align 4, !tbaa !3
  br label %for.body8.i

for.cond.cleanup3.i:                              ; preds = %for.cond.cleanup7.i
  %add24.i = add nuw nsw i32 %i.045.i, 2
  %cmp.i = icmp ult i32 %i.045.i, 8
  br i1 %cmp.i, label %for.cond1.preheader.i, label %for.cond43.preheader.preheader

for.cond43.preheader.preheader:                   ; preds = %for.cond.cleanup3.i
  br label %for.cond43.preheader

for.cond.cleanup7.i:                              ; preds = %for.body8.i
  %dec.i = add nsw i32 %j.044.i, -1
  %cmp2.i = icmp eq i32 %j.044.i, 0
  br i1 %cmp2.i, label %for.cond.cleanup3.i, label %for.cond5.preheader.i

for.body8.i:                                      ; preds = %for.body8.i, %for.cond5.preheader.i
  %k.043.i = phi i32 [ 0, %for.cond5.preheader.i ], [ %add20.i, %for.body8.i ]
  %add.i = add nuw nsw i32 %k.043.i, %j.044.i
  %arrayidx9.i = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %add.i
  %2 = load i32, i32* %arrayidx9.i, align 4, !tbaa !3
  %add10.i = add nsw i32 %2, %0
  %arrayidx14.i = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %j.044.i, i32 %k.043.i
  %3 = load i32, i32* %arrayidx14.i, align 4, !tbaa !3
  %mul15.i = mul nsw i32 %3, %1
  %add16.i = add nsw i32 %add10.i, %mul15.i
  %arrayidx19.i = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D1, i32 0, i32 %i.045.i, i32 %j.044.i, i32 %k.043.i
  store i32 %add16.i, i32* %arrayidx19.i, align 4, !tbaa !3
  %add20.i = add nuw nsw i32 %k.043.i, 3
  %cmp6.i = icmp ult i32 %k.043.i, 13
  br i1 %cmp6.i, label %for.body8.i, label %for.cond.cleanup7.i

for.cond.cleanup26:                               ; preds = %for.body27
  %inc35 = add nuw nsw i32 %j.097, 1
  %exitcond103 = icmp eq i32 %inc35, 16
  br i1 %exitcond103, label %for.cond1.preheader.i.preheader, label %for.cond24.preheader

for.cond1.preheader.i.preheader:                  ; preds = %for.cond.cleanup26
  br label %for.cond1.preheader.i

for.body27:                                       ; preds = %for.body27, %for.cond24.preheader
  %k.096 = phi i32 [ 0, %for.cond24.preheader ], [ %inc32, %for.body27 ]
  %add28 = add nuw nsw i32 %k.096, %j.097
  %arrayidx30 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %j.097, i32 %k.096
  store i32 %add28, i32* %arrayidx30, align 4, !tbaa !3
  %inc32 = add nuw nsw i32 %k.096, 1
  %exitcond102 = icmp eq i32 %inc32, 16
  br i1 %exitcond102, label %for.cond.cleanup26, label %for.body27

for.cond43.preheader:                             ; preds = %for.cond43.preheader.preheader, %for.cond.cleanup45
  %i37.095 = phi i32 [ %add62, %for.cond.cleanup45 ], [ 2, %for.cond43.preheader.preheader ]
  br label %for.cond48.preheader

for.cond.cleanup40:                               ; preds = %for.cond.cleanup45
  ret i32 0

for.cond48.preheader:                             ; preds = %for.cond.cleanup50, %for.cond43.preheader
  %j42.094 = phi i32 [ 0, %for.cond43.preheader ], [ %inc59, %for.cond.cleanup50 ]
  br label %for.body51

for.cond.cleanup45:                               ; preds = %for.cond.cleanup50
  %add62 = add nuw nsw i32 %i37.095, 2
  %cmp39 = icmp ult i32 %i37.095, 8
  br i1 %cmp39, label %for.cond43.preheader, label %for.cond.cleanup40

for.cond.cleanup50:                               ; preds = %for.body51
  %inc59 = add nuw nsw i32 %j42.094, 1
  %exitcond101 = icmp eq i32 %inc59, 16
  br i1 %exitcond101, label %for.cond.cleanup45, label %for.cond48.preheader

for.body51:                                       ; preds = %for.body51, %for.cond48.preheader
  %k47.093 = phi i32 [ 0, %for.cond48.preheader ], [ %inc56, %for.body51 ]
  %arrayidx54 = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D, i32 0, i32 %i37.095, i32 %j42.094, i32 %k47.093
  %4 = load i32, i32* %arrayidx54, align 4, !tbaa !3
  %call = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %4)
  %inc56 = add nuw nsw i32 %k47.093, 1
  %exitcond = icmp eq i32 %inc56, 16
  br i1 %exitcond, label %for.cond.cleanup50, label %for.body51
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

attributes #0 = { nofree norecurse nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
