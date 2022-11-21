; ModuleID = 'adapt_pedometer_step_is_array_gvn.ll'
source_filename = "adapt_pedometer_step_is_array.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

%struct.Acceleration = type { i32, i32, i32 }

@second = dso_local local_unnamed_addr global i8 0, align 1
@minute = dso_local local_unnamed_addr global i8 0, align 1
@hour = dso_local local_unnamed_addr global i8 0, align 1
@state = dso_local local_unnamed_addr global i8 0, align 1
@I1_flag = dso_local local_unnamed_addr global i8 0, align 1
@I2_flag = dso_local local_unnamed_addr global i8 0, align 1
@timerFlag1 = dso_local local_unnamed_addr global i8 0, align 1
@timerFlag2 = dso_local local_unnamed_addr global i8 0, align 1
@aux = dso_local local_unnamed_addr global i8 0, align 1
@Int_SourceSystem = dso_local local_unnamed_addr global i8 0, align 1
@Int_SourceTrans = dso_local local_unnamed_addr global i8 0, align 1
@step = dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@km = dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acc_vector = dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acceleration_x = dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acc_avg_x = dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@xx = dso_local local_unnamed_addr global i32 0, align 4
@n = dso_local local_unnamed_addr global i32 10, align 4
@acceleration_y = common dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acc_avg_y = common dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acceleration_z = common dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@acc_avg_z = common dso_local local_unnamed_addr global [16 x i32] zeroinitializer, align 4
@sub_x = common dso_local local_unnamed_addr global i32 0, align 4
@sub_y = common dso_local local_unnamed_addr global i32 0, align 4
@sub_z = common dso_local local_unnamed_addr global i32 0, align 4
@acceleration = common dso_local local_unnamed_addr global %struct.Acceleration zeroinitializer, align 4
@acc_avg = common dso_local local_unnamed_addr global %struct.Acceleration zeroinitializer, align 4
@length = common dso_local local_unnamed_addr global i8 0, align 1
@buffer = common dso_local local_unnamed_addr global [14 x i8] zeroinitializer, align 1
@i = common dso_local local_unnamed_addr global i32 0, align 4
@xhalf = common dso_local local_unnamed_addr global i32 0, align 4
@A = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@C = common dso_local local_unnamed_addr global [10 x [10 x i32]] zeroinitializer, align 4
@j = common dso_local local_unnamed_addr global i32 0, align 4

; Function Attrs: nounwind
define dso_local void @pedometer() local_unnamed_addr #0 {
entry:
  store i8 0, i8* @state, align 1, !tbaa !3
  store i8 0, i8* @I1_flag, align 1, !tbaa !3
  store i8 0, i8* @I2_flag, align 1, !tbaa !3
  store i8 0, i8* @aux, align 1, !tbaa !3
  store i8 0, i8* @timerFlag2, align 1, !tbaa !3
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.inc
  ret void

for.body:                                         ; preds = %for.inc, %entry
  %i.063 = phi i32 [ 0, %entry ], [ %inc, %for.inc ]
  %arrayidx = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_x, i32 0, i32 %i.063
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !6
  %arrayidx1 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_x, i32 0, i32 %i.063
  %1 = load i32, i32* %arrayidx1, align 4, !tbaa !6
  %sub = sub nsw i32 %0, %1
  %mul = mul nsw i32 %sub, %sub
  %arrayidx5 = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_y, i32 0, i32 %i.063
  %2 = load i32, i32* %arrayidx5, align 4, !tbaa !6
  %arrayidx6 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_y, i32 0, i32 %i.063
  %3 = load i32, i32* %arrayidx6, align 4, !tbaa !6
  %sub7 = sub nsw i32 %2, %3
  %mul11 = mul nsw i32 %sub7, %sub7
  %add = add nuw nsw i32 %mul11, %mul
  %arrayidx12 = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_z, i32 0, i32 %i.063
  %4 = load i32, i32* %arrayidx12, align 4, !tbaa !6
  %arrayidx13 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_z, i32 0, i32 %i.063
  %5 = load i32, i32* %arrayidx13, align 4, !tbaa !6
  %sub14 = sub nsw i32 %4, %5
  %mul18 = mul nsw i32 %sub14, %sub14
  %add19 = add nuw nsw i32 %add, %mul18
  %arrayidx20 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_vector, i32 0, i32 %i.063
  store i32 %add19, i32* %arrayidx20, align 4, !tbaa !6
  %cmp22 = icmp ugt i32 %add19, 15
  %cmp23 = icmp ugt i32 %i.063, 1
  %or.cond = and i1 %cmp23, %cmp22
  br i1 %or.cond, label %if.then, label %for.inc

if.then:                                          ; preds = %for.body
  %sub24 = add nsw i32 %i.063, -1
  %arrayidx25 = getelementptr inbounds [16 x i32], [16 x i32]* @step, i32 0, i32 %sub24
  %6 = load i32, i32* %arrayidx25, align 4, !tbaa !6
  %add26 = add nsw i32 %6, 2
  %arrayidx27 = getelementptr inbounds [16 x i32], [16 x i32]* @step, i32 0, i32 %i.063
  store i32 %add26, i32* %arrayidx27, align 4, !tbaa !6
  %cmp29 = icmp ugt i32 %add19, 100
  %arrayidx32 = getelementptr inbounds [16 x i32], [16 x i32]* @km, i32 0, i32 %sub24
  %7 = load i32, i32* %arrayidx32, align 4, !tbaa !6
  %. = select i1 %cmp29, i32 2, i32 1
  %add37 = add nsw i32 %7, %.
  %arrayidx38 = getelementptr inbounds [16 x i32], [16 x i32]* @km, i32 0, i32 %i.063
  store i32 %add37, i32* %arrayidx38, align 4, !tbaa !6
  br label %for.inc

for.inc:                                          ; preds = %if.then, %for.body
  %inc = add nuw nsw i32 %i.063, 1
  %exitcond = icmp eq i32 %inc, 16
  br i1 %exitcond, label %for.cond.cleanup, label %for.body
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nofree norecurse nounwind writeonly
define dso_local void @Interrupt() local_unnamed_addr #2 {
entry:
  store i8 1, i8* @I1_flag, align 1, !tbaa !3
  ret void
}

; Function Attrs: nofree norecurse nounwind writeonly
define dso_local void @Interrupt2() local_unnamed_addr #2 {
entry:
  store i8 1, i8* @I2_flag, align 1, !tbaa !3
  ret void
}

; Function Attrs: norecurse nounwind readnone
define dso_local void @error() local_unnamed_addr #3 {
entry:
  ret void
}

; Function Attrs: nofree norecurse nounwind writeonly
define dso_local void @TimerExpired1() local_unnamed_addr #2 {
entry:
  store i8 1, i8* @timerFlag1, align 1, !tbaa !3
  ret void
}

; Function Attrs: nofree norecurse nounwind writeonly
define dso_local void @TimerExpired2() local_unnamed_addr #2 {
entry:
  store i8 1, i8* @timerFlag2, align 1, !tbaa !3
  ret void
}

; Function Attrs: nofree norecurse nounwind
define dso_local void @TimerExpired3() local_unnamed_addr #4 {
entry:
  %0 = load i8, i8* @second, align 1, !tbaa !3
  %add = add i8 %0, 1
  store i8 %add, i8* @second, align 1, !tbaa !3
  %cmp = icmp ugt i8 %add, 60
  br i1 %cmp, label %if.then, label %if.end14

if.then:                                          ; preds = %entry
  store i8 0, i8* @second, align 1, !tbaa !3
  %1 = load i8, i8* @minute, align 1, !tbaa !3
  %add5 = add i8 %1, 1
  store i8 %add5, i8* @minute, align 1, !tbaa !3
  %cmp8 = icmp ugt i8 %add5, 60
  br i1 %cmp8, label %if.then10, label %if.end14

if.then10:                                        ; preds = %if.then
  %2 = load i8, i8* @hour, align 1, !tbaa !3
  %add12 = add i8 %2, 1
  store i8 %add12, i8* @hour, align 1, !tbaa !3
  store i8 0, i8* @minute, align 1, !tbaa !3
  br label %if.end14

if.end14:                                         ; preds = %if.then10, %if.then, %entry
  ret void
}

; Function Attrs: norecurse nounwind readnone
define dso_local void @writeDataToFile(i8* nocapture %date, i32 %data1, i32 %data2) local_unnamed_addr #3 {
entry:
  ret void
}

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  store i32 0, i32* @sub_x, align 4, !tbaa !6
  store i32 0, i32* @sub_y, align 4, !tbaa !6
  store i32 0, i32* @sub_z, align 4, !tbaa !6
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  store i8 0, i8* @state, align 1, !tbaa !3
  store i8 0, i8* @I1_flag, align 1, !tbaa !3
  store i8 0, i8* @I2_flag, align 1, !tbaa !3
  store i8 0, i8* @aux, align 1, !tbaa !3
  store i8 0, i8* @timerFlag2, align 1, !tbaa !3
  br label %for.body.i

for.body.i:                                       ; preds = %for.inc.i, %for.cond.cleanup
  %i.063.i = phi i32 [ 0, %for.cond.cleanup ], [ %inc.i, %for.inc.i ]
  %call.i = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #5
  %arrayidx.i = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_x, i32 0, i32 %i.063.i
  %0 = load i32, i32* %arrayidx.i, align 4, !tbaa !6
  %arrayidx1.i = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_x, i32 0, i32 %i.063.i
  %1 = load i32, i32* %arrayidx1.i, align 4, !tbaa !6
  %sub.i = sub nsw i32 %0, %1
  %mul.i = mul nsw i32 %sub.i, %sub.i
  %arrayidx5.i = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_y, i32 0, i32 %i.063.i
  %2 = load i32, i32* %arrayidx5.i, align 4, !tbaa !6
  %arrayidx6.i = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_y, i32 0, i32 %i.063.i
  %3 = load i32, i32* %arrayidx6.i, align 4, !tbaa !6
  %sub7.i = sub nsw i32 %2, %3
  %mul11.i = mul nsw i32 %sub7.i, %sub7.i
  %add.i = add nuw nsw i32 %mul11.i, %mul.i
  %arrayidx12.i = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_z, i32 0, i32 %i.063.i
  %4 = load i32, i32* %arrayidx12.i, align 4, !tbaa !6
  %arrayidx13.i = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_z, i32 0, i32 %i.063.i
  %5 = load i32, i32* %arrayidx13.i, align 4, !tbaa !6
  %sub14.i = sub nsw i32 %4, %5
  %mul18.i = mul nsw i32 %sub14.i, %sub14.i
  %add19.i = add nuw nsw i32 %add.i, %mul18.i
  %arrayidx20.i = getelementptr inbounds [16 x i32], [16 x i32]* @acc_vector, i32 0, i32 %i.063.i
  store i32 %add19.i, i32* %arrayidx20.i, align 4, !tbaa !6
  %cmp22.i = icmp ugt i32 %add19.i, 15
  %cmp23.i = icmp ugt i32 %i.063.i, 1
  %or.cond.i = and i1 %cmp23.i, %cmp22.i
  br i1 %or.cond.i, label %if.then.i, label %for.inc.i

if.then.i:                                        ; preds = %for.body.i
  %sub24.i = add nsw i32 %i.063.i, -1
  %arrayidx25.i = getelementptr inbounds [16 x i32], [16 x i32]* @step, i32 0, i32 %sub24.i
  %6 = load i32, i32* %arrayidx25.i, align 4, !tbaa !6
  %add26.i = add nsw i32 %6, 2
  %arrayidx27.i = getelementptr inbounds [16 x i32], [16 x i32]* @step, i32 0, i32 %i.063.i
  store i32 %add26.i, i32* %arrayidx27.i, align 4, !tbaa !6
  %cmp29.i = icmp ugt i32 %add19.i, 100
  %arrayidx32.i = getelementptr inbounds [16 x i32], [16 x i32]* @km, i32 0, i32 %sub24.i
  %7 = load i32, i32* %arrayidx32.i, align 4, !tbaa !6
  %. = select i1 %cmp29.i, i32 2, i32 1
  %add37.i = add nsw i32 %7, %.
  %arrayidx38.i = getelementptr inbounds [16 x i32], [16 x i32]* @km, i32 0, i32 %i.063.i
  store i32 %add37.i, i32* %arrayidx38.i, align 4, !tbaa !6
  br label %for.inc.i

for.inc.i:                                        ; preds = %if.then.i, %for.body.i
  %inc.i = add nuw nsw i32 %i.063.i, 1
  %exitcond.i = icmp eq i32 %inc.i, 16
  br i1 %exitcond.i, label %pedometer.exit, label %for.body.i

pedometer.exit:                                   ; preds = %for.inc.i
  ret i32 0

for.body:                                         ; preds = %for.body, %entry
  %i.023 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %mul = mul nuw nsw i32 %i.023, 3
  %arrayidx = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_x, i32 0, i32 %i.023
  store i32 %mul, i32* %arrayidx, align 4, !tbaa !6
  %add = add nuw nsw i32 %mul, 1
  %arrayidx2 = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_y, i32 0, i32 %i.023
  store i32 %add, i32* %arrayidx2, align 4, !tbaa !6
  %add4 = add nuw nsw i32 %mul, 2
  %arrayidx5 = getelementptr inbounds [16 x i32], [16 x i32]* @acceleration_z, i32 0, i32 %i.023
  store i32 %add4, i32* %arrayidx5, align 4, !tbaa !6
  %arrayidx6 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_x, i32 0, i32 %i.023
  store i32 %i.023, i32* %arrayidx6, align 4, !tbaa !6
  %arrayidx7 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_y, i32 0, i32 %i.023
  store i32 %i.023, i32* %arrayidx7, align 4, !tbaa !6
  %arrayidx8 = getelementptr inbounds [16 x i32], [16 x i32]* @acc_avg_z, i32 0, i32 %i.023
  store i32 %i.023, i32* %arrayidx8, align 4, !tbaa !6
  %inc = add nuw nsw i32 %i.023, 1
  %exitcond = icmp eq i32 %inc, 16
  br i1 %exitcond, label %for.cond.cleanup, label %for.body
}

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree norecurse nounwind writeonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { norecurse nounwind readnone "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nofree norecurse nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !7, i64 0}
!7 = !{!"int", !4, i64 0}
