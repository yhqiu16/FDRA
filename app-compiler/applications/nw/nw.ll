; ModuleID = 'nw.c'
source_filename = "nw.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@M = common dso_local local_unnamed_addr global [16641 x i32] zeroinitializer, align 4
@SEQA = common dso_local local_unnamed_addr global [128 x i8] zeroinitializer, align 1
@SEQB = common dso_local local_unnamed_addr global [128 x i8] zeroinitializer, align 1
@ptr = common dso_local local_unnamed_addr global [16641 x i8] zeroinitializer, align 1
@alignedA = common dso_local local_unnamed_addr global [256 x i8] zeroinitializer, align 1
@alignedB = common dso_local local_unnamed_addr global [256 x i8] zeroinitializer, align 1

; Function Attrs: nounwind
define dso_local void @needwun(i8* nocapture readnone %0, i8* nocapture readnone %1, i8* nocapture readnone %2, i8* nocapture readnone %3, i32* nocapture readnone %4, i8* nocapture readnone %5) local_unnamed_addr #0 {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %a_idx.0220 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %mul = sub nsw i32 0, %a_idx.0220
  %arrayidx = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %a_idx.0220
  store i32 %mul, i32* %arrayidx, align 4, !tbaa !3
  %inc = add nuw nsw i32 %a_idx.0220, 1
  %exitcond225 = icmp eq i32 %inc, 129
  br i1 %exitcond225, label %for.body8, label %for.body

for.body8:                                        ; preds = %for.body, %for.body8
  %b_idx.0219 = phi i32 [ %inc13, %for.body8 ], [ 0, %for.body ]
  %mul9 = sub nsw i32 0, %b_idx.0219
  %mul10 = mul nuw nsw i32 %b_idx.0219, 129
  %arrayidx11 = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %mul10
  store i32 %mul9, i32* %arrayidx11, align 4, !tbaa !3
  %inc13 = add nuw nsw i32 %b_idx.0219, 1
  %exitcond224 = icmp eq i32 %inc13, 129
  br i1 %exitcond224, label %for.cond18.preheader, label %for.body8

for.cond18.preheader:                             ; preds = %for.body8, %for.inc75
  %b_idx.1218 = phi i32 [ %inc76, %for.inc75 ], [ 1, %for.body8 ]
  %sub22 = add nsw i32 %b_idx.1218, -1
  %arrayidx23 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQB, i32 0, i32 %sub22
  %mul28 = mul nuw nsw i32 %sub22, 129
  %mul29 = mul nuw nsw i32 %b_idx.1218, 129
  br label %for.body20

for.body20:                                       ; preds = %for.inc72, %for.cond18.preheader
  %a_idx.1217 = phi i32 [ 1, %for.cond18.preheader ], [ %inc73, %for.inc72 ]
  %call = tail call i32 bitcast (i32 (...)* @please_map_me to i32 ()*)() #3
  %sub = add nsw i32 %a_idx.1217, -1
  %arrayidx21 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQA, i32 0, i32 %sub
  %6 = load i8, i8* %arrayidx21, align 1, !tbaa !7
  %7 = load i8, i8* %arrayidx23, align 1, !tbaa !7
  %cmp25 = icmp eq i8 %6, %7
  %. = select i1 %cmp25, i32 1, i32 -1
  %add = add nuw nsw i32 %sub, %mul28
  %arrayidx31 = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %add
  %8 = load i32, i32* %arrayidx31, align 4, !tbaa !3
  %add32 = add nsw i32 %., %8
  %add33 = add nuw nsw i32 %a_idx.1217, %mul28
  %arrayidx34 = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %add33
  %9 = load i32, i32* %arrayidx34, align 4, !tbaa !3
  %add35 = add nsw i32 %9, -1
  %add37 = add nuw nsw i32 %sub, %mul29
  %arrayidx38 = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %add37
  %10 = load i32, i32* %arrayidx38, align 4, !tbaa !3
  %add39 = add nsw i32 %10, -1
  %cmp40 = icmp sgt i32 %9, %10
  %cond = select i1 %cmp40, i32 %add35, i32 %add39
  %cmp42 = icmp sgt i32 %add32, %cond
  %cond53 = select i1 %cmp42, i32 %add32, i32 %cond
  %add54 = add nuw nsw i32 %a_idx.1217, %mul29
  %arrayidx55 = getelementptr inbounds [16641 x i32], [16641 x i32]* @M, i32 0, i32 %add54
  store i32 %cond53, i32* %arrayidx55, align 4, !tbaa !3
  %cmp56 = icmp eq i32 %cond53, %add39
  br i1 %cmp56, label %if.then58, label %if.else61

if.then58:                                        ; preds = %for.body20
  %arrayidx60 = getelementptr inbounds [16641 x i8], [16641 x i8]* @ptr, i32 0, i32 %add54
  store i8 60, i8* %arrayidx60, align 1, !tbaa !7
  br label %for.inc72

if.else61:                                        ; preds = %for.body20
  %cmp62 = icmp eq i32 %cond53, %add35
  %arrayidx66 = getelementptr inbounds [16641 x i8], [16641 x i8]* @ptr, i32 0, i32 %add54
  br i1 %cmp62, label %if.then64, label %if.else67

if.then64:                                        ; preds = %if.else61
  store i8 94, i8* %arrayidx66, align 1, !tbaa !7
  br label %for.inc72

if.else67:                                        ; preds = %if.else61
  store i8 92, i8* %arrayidx66, align 1, !tbaa !7
  br label %for.inc72

for.inc72:                                        ; preds = %if.then58, %if.else67, %if.then64
  %inc73 = add nuw nsw i32 %a_idx.1217, 1
  %exitcond = icmp eq i32 %inc73, 129
  br i1 %exitcond, label %for.inc75, label %for.body20

for.inc75:                                        ; preds = %for.inc72
  %inc76 = add nuw nsw i32 %b_idx.1218, 1
  %exitcond223 = icmp eq i32 %inc76, 129
  br i1 %exitcond223, label %while.body, label %for.cond18.preheader

for.cond122.preheader:                            ; preds = %if.end121
  %cmp123211 = icmp ult i32 %b_str_idx.0216, 255
  br i1 %cmp123211, label %for.body133.preheader, label %for.end137

while.body:                                       ; preds = %for.inc75, %if.end121
  %b_str_idx.0216 = phi i32 [ %b_str_idx.1, %if.end121 ], [ 0, %for.inc75 ]
  %b_idx.2214 = phi i32 [ %b_idx.3, %if.end121 ], [ 128, %for.inc75 ]
  %a_idx.2213 = phi i32 [ %a_idx.3, %if.end121 ], [ 128, %for.inc75 ]
  %mul82 = mul nsw i32 %b_idx.2214, 129
  %add83 = add nsw i32 %mul82, %a_idx.2213
  %arrayidx84 = getelementptr inbounds [16641 x i8], [16641 x i8]* @ptr, i32 0, i32 %add83
  %11 = load i8, i8* %arrayidx84, align 1, !tbaa !7
  switch i8 %11, label %if.else112 [
    i8 92, label %if.then88
    i8 60, label %if.then104
  ]

if.then88:                                        ; preds = %while.body
  %sub89 = add nsw i32 %a_idx.2213, -1
  %arrayidx90 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQA, i32 0, i32 %sub89
  %12 = load i8, i8* %arrayidx90, align 1, !tbaa !7
  %arrayidx92 = getelementptr inbounds [256 x i8], [256 x i8]* @alignedA, i32 0, i32 %b_str_idx.0216
  store i8 %12, i8* %arrayidx92, align 1, !tbaa !7
  %sub93 = add nsw i32 %b_idx.2214, -1
  %arrayidx94 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQB, i32 0, i32 %sub93
  %13 = load i8, i8* %arrayidx94, align 1, !tbaa !7
  br label %if.end121

if.then104:                                       ; preds = %while.body
  %sub105 = add nsw i32 %a_idx.2213, -1
  %arrayidx106 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQA, i32 0, i32 %sub105
  %14 = load i8, i8* %arrayidx106, align 1, !tbaa !7
  %arrayidx108 = getelementptr inbounds [256 x i8], [256 x i8]* @alignedA, i32 0, i32 %b_str_idx.0216
  store i8 %14, i8* %arrayidx108, align 1, !tbaa !7
  br label %if.end121

if.else112:                                       ; preds = %while.body
  %arrayidx114 = getelementptr inbounds [256 x i8], [256 x i8]* @alignedA, i32 0, i32 %b_str_idx.0216
  store i8 45, i8* %arrayidx114, align 1, !tbaa !7
  %sub115 = add nsw i32 %b_idx.2214, -1
  %arrayidx116 = getelementptr inbounds [128 x i8], [128 x i8]* @SEQB, i32 0, i32 %sub115
  %15 = load i8, i8* %arrayidx116, align 1, !tbaa !7
  br label %if.end121

if.end121:                                        ; preds = %if.then104, %if.else112, %if.then88
  %.sink = phi i8 [ 45, %if.then104 ], [ %15, %if.else112 ], [ %13, %if.then88 ]
  %a_idx.3 = phi i32 [ %sub105, %if.then104 ], [ %a_idx.2213, %if.else112 ], [ %sub89, %if.then88 ]
  %b_idx.3 = phi i32 [ %b_idx.2214, %if.then104 ], [ %sub115, %if.else112 ], [ %sub93, %if.then88 ]
  %arrayidx110 = getelementptr inbounds [256 x i8], [256 x i8]* @alignedB, i32 0, i32 %b_str_idx.0216
  store i8 %.sink, i8* %arrayidx110, align 1, !tbaa !7
  %b_str_idx.1 = add nuw nsw i32 %b_str_idx.0216, 1
  %cmp78 = icmp sgt i32 %a_idx.3, 0
  %cmp80 = icmp sgt i32 %b_idx.3, 0
  %16 = or i1 %cmp78, %cmp80
  br i1 %16, label %while.body, label %for.cond122.preheader

for.body133.preheader:                            ; preds = %for.cond122.preheader
  %scevgep221 = getelementptr [256 x i8], [256 x i8]* @alignedA, i32 0, i32 %b_str_idx.1
  %17 = sub nuw nsw i32 255, %b_str_idx.0216
  call void @llvm.memset.p0i8.i32(i8* align 1 %scevgep221, i8 95, i32 %17, i1 false)
  %scevgep = getelementptr [256 x i8], [256 x i8]* @alignedB, i32 0, i32 %b_str_idx.1
  %18 = sub nuw nsw i32 255, %b_str_idx.0216
  call void @llvm.memset.p0i8.i32(i8* align 1 %scevgep, i8 95, i32 %18, i1 false)
  br label %for.end137

for.end137:                                       ; preds = %for.cond122.preheader, %for.body133.preheader
  ret void
}

declare dso_local i32 @please_map_me(...) local_unnamed_addr #1

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i32(i8* nocapture writeonly, i8, i32, i1 immarg) #2

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind willreturn }
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
!7 = !{!5, !5, i64 0}
