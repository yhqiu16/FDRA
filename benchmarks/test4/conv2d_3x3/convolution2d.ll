; ModuleID = 'convolution2d.c'
source_filename = "convolution2d.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@A = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [1024 x i32] zeroinitializer, align 4

; Function Attrs: noinline nounwind
define dso_local void @convolution2d() local_unnamed_addr #0 {
entry:
  br label %for.cond1.preheader

for.cond1.preheader:                              ; preds = %for.cond.cleanup3, %entry
  %i.094 = phi i32 [ 1, %entry ], [ %inc67, %for.cond.cleanup3 ]
  %sub = shl i32 %i.094, 5
  %mul = add nsw i32 %sub, -32
  %mul15 = add nuw nsw i32 %sub, 32
  br label %for.body4

for.cond.cleanup:                                 ; preds = %for.cond.cleanup3
  ret void

for.cond.cleanup3:                                ; preds = %for.body4
  %inc67 = add nuw nsw i32 %i.094, 1
  %exitcond95 = icmp eq i32 %inc67, 31
  br i1 %exitcond95, label %for.cond.cleanup, label %for.cond1.preheader

for.body4:                                        ; preds = %for.body4, %for.cond1.preheader
  %j.093 = phi i32 [ 1, %for.cond1.preheader ], [ %add44, %for.body4 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %sub5 = add nsw i32 %j.093, -1
  %add = add nuw nsw i32 %sub5, %mul
  %arrayidx = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %mul6 = shl i32 %0, 1
  %add10 = add nuw nsw i32 %sub5, %sub
  %arrayidx11 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add10
  %1 = load i32, i32* %arrayidx11, align 4, !tbaa !3
  %mul12 = mul nsw i32 %1, -3
  %add13 = add nsw i32 %mul12, %mul6
  %add17 = add nuw nsw i32 %sub5, %mul15
  %arrayidx18 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add17
  %2 = load i32, i32* %arrayidx18, align 4, !tbaa !3
  %mul19 = shl i32 %2, 2
  %add20 = add nsw i32 %add13, %mul19
  %add24 = add nuw nsw i32 %j.093, %mul
  %arrayidx25 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add24
  %3 = load i32, i32* %arrayidx25, align 4, !tbaa !3
  %mul26 = mul nsw i32 %3, 5
  %add27 = add nsw i32 %add20, %mul26
  %add31 = add nuw nsw i32 %j.093, %sub
  %arrayidx32 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add31
  %4 = load i32, i32* %arrayidx32, align 4, !tbaa !3
  %mul33 = mul nsw i32 %4, 6
  %add34 = add nsw i32 %add27, %mul33
  %add38 = add nuw nsw i32 %j.093, %mul15
  %arrayidx39 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add38
  %5 = load i32, i32* %arrayidx39, align 4, !tbaa !3
  %mul40 = mul nsw i32 %5, 7
  %add41 = add nsw i32 %add34, %mul40
  %add44 = add nuw nsw i32 %j.093, 1
  %add45 = add nuw nsw i32 %add44, %mul
  %arrayidx46 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add45
  %6 = load i32, i32* %arrayidx46, align 4, !tbaa !3
  %mul47 = mul nsw i32 %6, -8
  %add48 = add nsw i32 %add41, %mul47
  %add52 = add nuw nsw i32 %add44, %sub
  %arrayidx53 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add52
  %7 = load i32, i32* %arrayidx53, align 4, !tbaa !3
  %mul54 = mul nsw i32 %7, -9
  %add55 = add nsw i32 %add48, %mul54
  %add59 = add nuw nsw i32 %add44, %mul15
  %arrayidx60 = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %add59
  %8 = load i32, i32* %arrayidx60, align 4, !tbaa !3
  %mul61 = mul nsw i32 %8, 10
  %add62 = add nsw i32 %add55, %mul61
  %arrayidx65 = getelementptr inbounds [1024 x i32], [1024 x i32]* @B, i32 0, i32 %add31
  store i32 %add62, i32* %arrayidx65, align 4, !tbaa !3
  %exitcond = icmp eq i32 %add44, 31
  br i1 %exitcond, label %for.cond.cleanup3, label %for.body4
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #2 {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  tail call void @convolution2d()
  ret i32 0

for.body:                                         ; preds = %for.body, %entry
  %i.05 = phi i32 [ 0, %entry ], [ %add, %for.body ]
  %add = add nuw nsw i32 %i.05, 1
  %arrayidx = getelementptr inbounds [1024 x i32], [1024 x i32]* @A, i32 0, i32 %i.05
  store i32 %add, i32* %arrayidx, align 4, !tbaa !3
  %exitcond = icmp eq i32 %add, 1024
  br i1 %exitcond, label %for.cond.cleanup, label %for.body
}

attributes #0 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
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
