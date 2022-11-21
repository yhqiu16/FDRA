; ModuleID = 'array_add.c'
source_filename = "array_add.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@A = common dso_local local_unnamed_addr global [20 x i32] zeroinitializer, align 4
@B = common dso_local local_unnamed_addr global [40 x [50 x i32]] zeroinitializer, align 4
@C = common dso_local local_unnamed_addr global [20 x i32] zeroinitializer, align 4
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind
define dso_local void @array_add() local_unnamed_addr #0 {
entry:
  br label %for.body

for.cond.cleanup:                                 ; preds = %for.body
  ret void

for.body:                                         ; preds = %for.body, %entry
  %i.010 = phi i32 [ 0, %entry ], [ %add, %for.body ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %add = add nuw nsw i32 %i.010, 1
  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* @A, i32 0, i32 %add
  %0 = load i32, i32* %arrayidx, align 4, !tbaa !3
  %sub = add nsw i32 %i.010, -1
  %arrayidx1 = getelementptr inbounds [20 x i32], [20 x i32]* @A, i32 0, i32 %sub
  %1 = load i32, i32* %arrayidx1, align 4, !tbaa !3
  %add2 = add nsw i32 %1, %0
  %arrayidx4 = getelementptr inbounds [40 x [50 x i32]], [40 x [50 x i32]]* @B, i32 0, i32 %i.010, i32 0
  store i32 %add2, i32* %arrayidx4, align 4, !tbaa !3
  %exitcond = icmp eq i32 %add, 20
  br i1 %exitcond, label %for.cond.cleanup, label %for.body
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #0 {
entry:
  call void @llvm.memset.p0i8.i32(i8* nonnull align 4 dereferenceable(80) bitcast ([20 x i32]* @C to i8*), i8 0, i32 80, i1 false)
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %i.023 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %mul = shl nuw nsw i32 %i.023, 1
  %add = add nuw nsw i32 %mul, 5
  %arrayidx = getelementptr inbounds [20 x i32], [20 x i32]* @A, i32 0, i32 %i.023
  store i32 %add, i32* %arrayidx, align 4, !tbaa !3
  %mul1 = mul nuw nsw i32 %i.023, 3
  %arrayidx3 = getelementptr inbounds [40 x [50 x i32]], [40 x [50 x i32]]* @B, i32 0, i32 %i.023, i32 0
  store i32 %mul1, i32* %arrayidx3, align 4, !tbaa !3
  %inc = add nuw nsw i32 %i.023, 1
  %exitcond24 = icmp eq i32 %inc, 20
  br i1 %exitcond24, label %for.body.i, label %for.body

for.body.i:                                       ; preds = %for.body, %for.body.i
  %i.010.i = phi i32 [ %add.i, %for.body.i ], [ 0, %for.body ]
  %call.i = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %add.i = add nuw nsw i32 %i.010.i, 1
  %arrayidx.i = getelementptr inbounds [20 x i32], [20 x i32]* @A, i32 0, i32 %add.i
  %0 = load i32, i32* %arrayidx.i, align 4, !tbaa !3
  %sub.i = add nsw i32 %i.010.i, -1
  %arrayidx1.i = getelementptr inbounds [20 x i32], [20 x i32]* @A, i32 0, i32 %sub.i
  %1 = load i32, i32* %arrayidx1.i, align 4, !tbaa !3
  %add2.i = add nsw i32 %1, %0
  %arrayidx4.i = getelementptr inbounds [40 x [50 x i32]], [40 x [50 x i32]]* @B, i32 0, i32 %i.010.i, i32 0
  store i32 %add2.i, i32* %arrayidx4.i, align 4, !tbaa !3
  %exitcond.i = icmp eq i32 %add.i, 20
  br i1 %exitcond.i, label %for.body7, label %for.body.i

for.body7:                                        ; preds = %for.body.i, %for.body7
  %i.122 = phi i32 [ %inc10, %for.body7 ], [ 0, %for.body.i ]
  %arrayidx8 = getelementptr inbounds [20 x i32], [20 x i32]* @C, i32 0, i32 %i.122
  %2 = load i32, i32* %arrayidx8, align 4, !tbaa !3
  %call = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %2)
  %inc10 = add nuw nsw i32 %i.122, 1
  %exitcond = icmp eq i32 %inc10, 20
  br i1 %exitcond, label %for.end11, label %for.body7

for.end11:                                        ; preds = %for.body7
  ret i32 0
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
