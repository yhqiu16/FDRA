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
  br label %1

1:                                                ; preds = %21, %0
  %2 = phi i32 [ 0, %0 ], [ %22, %21 ]
  br label %3

3:                                                ; preds = %18, %1
  %4 = phi i32 [ 0, %1 ], [ %19, %18 ]
  %5 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %2, i32 %4
  br label %6

6:                                                ; preds = %6, %3
  %7 = phi i32 [ 0, %3 ], [ %16, %6 ]
  %8 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %9 = load i32, i32* %5, align 4, !tbaa !3
  %10 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %2, i32 %7
  %11 = load i32, i32* %10, align 4, !tbaa !3
  %12 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %7, i32 %4
  %13 = load i32, i32* %12, align 4, !tbaa !3
  %14 = mul nsw i32 %13, %11
  %15 = add nsw i32 %14, %9
  store i32 %15, i32* %5, align 4, !tbaa !3
  %16 = add nuw nsw i32 %7, 1
  %17 = icmp eq i32 %16, 10
  br i1 %17, label %18, label %6

18:                                               ; preds = %6
  %19 = add nuw nsw i32 %4, 1
  %20 = icmp eq i32 %19, 10
  br i1 %20, label %21, label %3

21:                                               ; preds = %18
  %22 = add nuw nsw i32 %2, 1
  %23 = icmp eq i32 %22, 10
  br i1 %23, label %24, label %1

24:                                               ; preds = %21
  ret void
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: nounwind
define dso_local void @main() local_unnamed_addr #0 {
  %1 = load i32, i32* @n, align 4, !tbaa !3
  %2 = icmp sgt i32 %1, 0
  br i1 %2, label %3, label %20

3:                                                ; preds = %0
  %4 = shl nuw i32 %1, 2
  br label %5

5:                                                ; preds = %17, %3
  %6 = phi i32 [ 0, %3 ], [ %18, %17 ]
  %7 = getelementptr [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %6, i32 0
  %8 = bitcast i32* %7 to i8*
  call void @llvm.memset.p0i8.i32(i8* align 4 %8, i8 0, i32 %4, i1 false)
  br label %9

9:                                                ; preds = %9, %5
  %10 = phi i32 [ %15, %9 ], [ 0, %5 ]
  %11 = add nuw nsw i32 %10, %6
  %12 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %6, i32 %10
  store i32 %11, i32* %12, align 4, !tbaa !3
  %13 = mul nsw i32 %10, %6
  %14 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %6, i32 %10
  store i32 %13, i32* %14, align 4, !tbaa !3
  %15 = add nuw nsw i32 %10, 1
  %16 = icmp eq i32 %15, %1
  br i1 %16, label %17, label %9

17:                                               ; preds = %9
  %18 = add nuw nsw i32 %6, 1
  %19 = icmp slt i32 %18, %1
  br i1 %19, label %5, label %20

20:                                               ; preds = %17, %0
  br label %21

21:                                               ; preds = %20, %41
  %22 = phi i32 [ %42, %41 ], [ 0, %20 ]
  br label %23

23:                                               ; preds = %38, %21
  %24 = phi i32 [ 0, %21 ], [ %39, %38 ]
  %25 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %22, i32 %24
  br label %26

26:                                               ; preds = %26, %23
  %27 = phi i32 [ 0, %23 ], [ %36, %26 ]
  %28 = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #4
  %29 = load i32, i32* %25, align 4, !tbaa !3
  %30 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @B, i32 0, i32 %22, i32 %27
  %31 = load i32, i32* %30, align 4, !tbaa !3
  %32 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @C, i32 0, i32 %27, i32 %24
  %33 = load i32, i32* %32, align 4, !tbaa !3
  %34 = mul nsw i32 %33, %31
  %35 = add nsw i32 %34, %29
  store i32 %35, i32* %25, align 4, !tbaa !3
  %36 = add nuw nsw i32 %27, 1
  %37 = icmp eq i32 %36, 10
  br i1 %37, label %38, label %26

38:                                               ; preds = %26
  %39 = add nuw nsw i32 %24, 1
  %40 = icmp eq i32 %39, 10
  br i1 %40, label %41, label %23

41:                                               ; preds = %38
  %42 = add nuw nsw i32 %22, 1
  %43 = icmp eq i32 %42, 10
  br i1 %43, label %44, label %21

44:                                               ; preds = %41
  %45 = load i32, i32* @n, align 4, !tbaa !3
  %46 = icmp sgt i32 %45, 0
  br i1 %46, label %47, label %63

47:                                               ; preds = %44, %59
  %48 = phi i32 [ %60, %59 ], [ %45, %44 ]
  %49 = phi i32 [ %61, %59 ], [ 0, %44 ]
  %50 = icmp sgt i32 %48, 0
  br i1 %50, label %51, label %59

51:                                               ; preds = %47, %51
  %52 = phi i32 [ %56, %51 ], [ 0, %47 ]
  %53 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @A, i32 0, i32 %49, i32 %52
  %54 = load i32, i32* %53, align 4, !tbaa !3
  %55 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %54)
  %56 = add nuw nsw i32 %52, 1
  %57 = load i32, i32* @n, align 4, !tbaa !3
  %58 = icmp slt i32 %56, %57
  br i1 %58, label %51, label %59

59:                                               ; preds = %51, %47
  %60 = phi i32 [ %48, %47 ], [ %57, %51 ]
  %61 = add nuw nsw i32 %49, 1
  %62 = icmp slt i32 %61, %60
  br i1 %62, label %47, label %63

63:                                               ; preds = %59, %44
  ret void
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i32(i8* nocapture writeonly, i8, i32, i1 immarg) #3

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="i686" "target-features"="+cx8,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="i686" "target-features"="+cx8,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="i686" "target-features"="+cx8,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind willreturn }
attributes #4 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0-4ubuntu1 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
