; ModuleID = 'test_gvn.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@A = common dso_local local_unnamed_addr global [32 x i32] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [32 x i32] zeroinitializer, align 4
@C = common dso_local local_unnamed_addr global [256 x i32] zeroinitializer, align 4
@E = common dso_local local_unnamed_addr global [16 x [16 x i32]] zeroinitializer, align 4
@D = common dso_local local_unnamed_addr global [10 x [16 x [16 x i32]]] zeroinitializer, align 4
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind
define dso_local void @test() local_unnamed_addr #0 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.cond.cleanup3, %entry
  %i.045 = phi i32 [ 2, %entry ], [ %add24, %for.cond.cleanup3 ]
  %mul = mul nuw nsw i32 %i.045, 3
  %arrayidx = getelementptr inbounds [32 x i32], [32 x i32]* @A, i32 0, i32 %mul
  br label %for.cond5.preheader

for.cond.cleanup:                                 ; preds = %for.cond.cleanup3
  ret void

for.cond5.preheader:                              ; preds = %for.cond.cleanup7, %for.cond1.preheader
  %j.044 = phi i32 [ 15, %for.cond1.preheader ], [ %dec, %for.cond.cleanup7 ]
  br label %for.body8

for.cond.cleanup3:                                ; preds = %for.cond.cleanup7
  %add24 = add nuw nsw i32 %i.045, 2
  %cmp = icmp ult i32 %i.045, 7
  br i1 %cmp, label %for.cond1.preheader, label %for.cond.cleanup

for.cond.cleanup7:                                ; preds = %for.body8
  %dec = add nsw i32 %j.044, -1
  %cmp2 = icmp eq i32 %j.044, 0
  br i1 %cmp2, label %for.cond.cleanup3, label %for.cond5.preheader

for.body8:                                        ; preds = %for.body8, %for.cond5.preheader
  %k.043 = phi i32 [ 0, %for.cond5.preheader ], [ %add20, %for.body8 ]
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %add = add nuw nsw i32 %k.043, %j.044
  %arrayidx9 = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %add
  %1 = load i32, i32* %arrayidx9, align 4, !tbaa !3
  %add10 = add nsw i32 %1, %0
  %mul11 = mul nuw nsw i32 %k.043, %k.043
  %arrayidx12 = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %mul11
  %2 = load i32, i32* %arrayidx12, align 4, !tbaa !3
  %arrayidx14 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %j.044, i32 %k.043
  %3 = load i32, i32* %arrayidx14, align 4, !tbaa !3
  %mul15 = mul nsw i32 %3, %2
  %add16 = add nsw i32 %add10, %mul15
  %arrayidx19 = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D, i32 0, i32 %i.045, i32 %j.044, i32 %k.043
  store i32 %add16, i32* %arrayidx19, align 4, !tbaa !3
  %add20 = add nuw nsw i32 %k.043, 3
  %cmp6 = icmp ult i32 %k.043, 13
  br i1 %cmp6, label %for.body8, label %for.cond.cleanup7
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %i.089 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %mul = shl nuw nsw i32 %i.089, 1
  %add = add nuw nsw i32 %mul, 5
  %arrayidx = getelementptr inbounds [32 x i32], [32 x i32]* @A, i32 0, i32 %i.089
  store i32 %add, i32* %arrayidx, align 4, !tbaa !3
  %mul1 = mul nuw nsw i32 %i.089, 3
  %arrayidx2 = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %i.089
  store i32 %mul1, i32* %arrayidx2, align 4, !tbaa !3
  %inc = add nuw nsw i32 %i.089, 1
  %exitcond94 = icmp eq i32 %inc, 32
  br i1 %exitcond94, label %for.body7.preheader, label %for.body

for.body7.preheader:                              ; preds = %for.body
  br label %for.body7

for.body7:                                        ; preds = %for.body7, %for.body7.preheader
  %i3.088 = phi i32 [ %inc10, %for.body7 ], [ 0, %for.body7.preheader ]
  %arrayidx8 = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %i3.088
  store i32 %i3.088, i32* %arrayidx8, align 4, !tbaa !3
  %inc10 = add nuw nsw i32 %i3.088, 1
  %exitcond93 = icmp eq i32 %inc10, 256
  br i1 %exitcond93, label %for.cond17.preheader.preheader, label %for.body7

for.cond17.preheader.preheader:                   ; preds = %for.body7
  br label %for.cond17.preheader

for.cond17.preheader:                             ; preds = %for.cond.cleanup19, %for.cond17.preheader.preheader
  %i12.087 = phi i32 [ %inc28, %for.cond.cleanup19 ], [ 0, %for.cond17.preheader.preheader ]
  br label %for.body20

for.cond1.preheader.i:                            ; preds = %for.cond1.preheader.i.preheader, %for.cond.cleanup3.i
  %i.045.i = phi i32 [ %add24.i, %for.cond.cleanup3.i ], [ 2, %for.cond1.preheader.i.preheader ]
  %mul.i = mul nuw nsw i32 %i.045.i, 3
  %arrayidx.i = getelementptr inbounds [32 x i32], [32 x i32]* @A, i32 0, i32 %mul.i
  br label %for.cond5.preheader.i

for.cond5.preheader.i:                            ; preds = %for.cond.cleanup7.i, %for.cond1.preheader.i
  %j.044.i = phi i32 [ 15, %for.cond1.preheader.i ], [ %dec.i, %for.cond.cleanup7.i ]
  br label %for.body8.i

for.cond.cleanup3.i:                              ; preds = %for.cond.cleanup7.i
  %add24.i = add nuw nsw i32 %i.045.i, 2
  %cmp.i = icmp ult i32 %i.045.i, 7
  br i1 %cmp.i, label %for.cond1.preheader.i, label %for.cond36.preheader.preheader

for.cond36.preheader.preheader:                   ; preds = %for.cond.cleanup3.i
  br label %for.cond36.preheader

for.cond.cleanup7.i:                              ; preds = %for.body8.i
  %dec.i = add nsw i32 %j.044.i, -1
  %cmp2.i = icmp eq i32 %j.044.i, 0
  br i1 %cmp2.i, label %for.cond.cleanup3.i, label %for.cond5.preheader.i

for.body8.i:                                      ; preds = %for.body8.i, %for.cond5.preheader.i
  %k.043.i = phi i32 [ 0, %for.cond5.preheader.i ], [ %add20.i, %for.body8.i ]
  %call.i = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %0 = load i32, i32* %arrayidx.i, align 4, !tbaa !3
  %add.i = add nuw nsw i32 %k.043.i, %j.044.i
  %arrayidx9.i = getelementptr inbounds [32 x i32], [32 x i32]* @B, i32 0, i32 %add.i
  %1 = load i32, i32* %arrayidx9.i, align 4, !tbaa !3
  %add10.i = add nsw i32 %1, %0
  %mul11.i = mul nuw nsw i32 %k.043.i, %k.043.i
  %arrayidx12.i = getelementptr inbounds [256 x i32], [256 x i32]* @C, i32 0, i32 %mul11.i
  %2 = load i32, i32* %arrayidx12.i, align 4, !tbaa !3
  %arrayidx14.i = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %j.044.i, i32 %k.043.i
  %3 = load i32, i32* %arrayidx14.i, align 4, !tbaa !3
  %mul15.i = mul nsw i32 %3, %2
  %add16.i = add nsw i32 %add10.i, %mul15.i
  %arrayidx19.i = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D, i32 0, i32 %i.045.i, i32 %j.044.i, i32 %k.043.i
  store i32 %add16.i, i32* %arrayidx19.i, align 4, !tbaa !3
  %add20.i = add nuw nsw i32 %k.043.i, 3
  %cmp6.i = icmp ult i32 %k.043.i, 13
  br i1 %cmp6.i, label %for.body8.i, label %for.cond.cleanup7.i

for.cond.cleanup19:                               ; preds = %for.body20
  %inc28 = add nuw nsw i32 %i12.087, 1
  %exitcond92 = icmp eq i32 %inc28, 256
  br i1 %exitcond92, label %for.cond1.preheader.i.preheader, label %for.cond17.preheader

for.cond1.preheader.i.preheader:                  ; preds = %for.cond.cleanup19
  br label %for.cond1.preheader.i

for.body20:                                       ; preds = %for.body20, %for.cond17.preheader
  %j.086 = phi i32 [ 0, %for.cond17.preheader ], [ %inc25, %for.body20 ]
  %add21 = add nuw nsw i32 %j.086, %i12.087
  %arrayidx23 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @E, i32 0, i32 %i12.087, i32 %j.086
  store i32 %add21, i32* %arrayidx23, align 4, !tbaa !3
  %inc25 = add nuw nsw i32 %j.086, 1
  %exitcond91 = icmp eq i32 %inc25, 16
  br i1 %exitcond91, label %for.cond.cleanup19, label %for.body20

for.cond36.preheader:                             ; preds = %for.cond.cleanup38, %for.cond36.preheader.preheader
  %i30.085 = phi i32 [ %add54, %for.cond.cleanup38 ], [ 2, %for.cond36.preheader.preheader ]
  br label %for.cond40.preheader

for.cond.cleanup33:                               ; preds = %for.cond.cleanup38
  ret i32 0

for.cond40.preheader:                             ; preds = %for.cond.cleanup42, %for.cond36.preheader
  %j35.084 = phi i32 [ 0, %for.cond36.preheader ], [ %inc51, %for.cond.cleanup42 ]
  br label %for.body43

for.cond.cleanup38:                               ; preds = %for.cond.cleanup42
  %add54 = add nuw nsw i32 %i30.085, 2
  %cmp32 = icmp ult i32 %i30.085, 7
  br i1 %cmp32, label %for.cond36.preheader, label %for.cond.cleanup33

for.cond.cleanup42:                               ; preds = %for.body43
  %inc51 = add nuw nsw i32 %j35.084, 1
  %exitcond90 = icmp eq i32 %inc51, 16
  br i1 %exitcond90, label %for.cond.cleanup38, label %for.cond40.preheader

for.body43:                                       ; preds = %for.body43, %for.cond40.preheader
  %k.083 = phi i32 [ 0, %for.cond40.preheader ], [ %inc48, %for.body43 ]
  %arrayidx46 = getelementptr inbounds [10 x [16 x [16 x i32]]], [10 x [16 x [16 x i32]]]* @D, i32 0, i32 %i30.085, i32 %j35.084, i32 %k.083
  %4 = load i32, i32* %arrayidx46, align 4, !tbaa !3
  %call = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %4)
  %inc48 = add nuw nsw i32 %k.083, 1
  %exitcond = icmp eq i32 %inc48, 16
  br i1 %exitcond, label %for.cond.cleanup42, label %for.body43
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
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
