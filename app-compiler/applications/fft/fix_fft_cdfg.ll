; ModuleID = 'fix_fft_gvn.ll'
source_filename = "fix_fft.c"
target datalayout = "e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-unknown-linux-gnu"

@Sinewave = dso_local local_unnamed_addr global [768 x i16] [i16 0, i16 201, i16 402, i16 603, i16 804, i16 1005, i16 1206, i16 1406, i16 1607, i16 1808, i16 2009, i16 2209, i16 2410, i16 2610, i16 2811, i16 3011, i16 3211, i16 3411, i16 3611, i16 3811, i16 4011, i16 4210, i16 4409, i16 4608, i16 4807, i16 5006, i16 5205, i16 5403, i16 5601, i16 5799, i16 5997, i16 6195, i16 6392, i16 6589, i16 6786, i16 6982, i16 7179, i16 7375, i16 7571, i16 7766, i16 7961, i16 8156, i16 8351, i16 8545, i16 8739, i16 8932, i16 9126, i16 9319, i16 9511, i16 9703, i16 9895, i16 10087, i16 10278, i16 10469, i16 10659, i16 10849, i16 11038, i16 11227, i16 11416, i16 11604, i16 11792, i16 11980, i16 12166, i16 12353, i16 12539, i16 12724, i16 12909, i16 13094, i16 13278, i16 13462, i16 13645, i16 13827, i16 14009, i16 14191, i16 14372, i16 14552, i16 14732, i16 14911, i16 15090, i16 15268, i16 15446, i16 15623, i16 15799, i16 15975, i16 16150, i16 16325, i16 16499, i16 16672, i16 16845, i16 17017, i16 17189, i16 17360, i16 17530, i16 17699, i16 17868, i16 18036, i16 18204, i16 18371, i16 18537, i16 18702, i16 18867, i16 19031, i16 19194, i16 19357, i16 19519, i16 19680, i16 19840, i16 20000, i16 20159, i16 20317, i16 20474, i16 20631, i16 20787, i16 20942, i16 21096, i16 21249, i16 21402, i16 21554, i16 21705, i16 21855, i16 22004, i16 22153, i16 22301, i16 22448, i16 22594, i16 22739, i16 22883, i16 23027, i16 23169, i16 23311, i16 23452, i16 23592, i16 23731, i16 23869, i16 24006, i16 24143, i16 24278, i16 24413, i16 24546, i16 24679, i16 24811, i16 24942, i16 25072, i16 25201, i16 25329, i16 25456, i16 25582, i16 25707, i16 25831, i16 25954, i16 26077, i16 26198, i16 26318, i16 26437, i16 26556, i16 26673, i16 26789, i16 26905, i16 27019, i16 27132, i16 27244, i16 27355, i16 27466, i16 27575, i16 27683, i16 27790, i16 27896, i16 28001, i16 28105, i16 28208, i16 28309, i16 28410, i16 28510, i16 28608, i16 28706, i16 28802, i16 28897, i16 28992, i16 29085, i16 29177, i16 29268, i16 29358, i16 29446, i16 29534, i16 29621, i16 29706, i16 29790, i16 29873, i16 29955, i16 30036, i16 30116, i16 30195, i16 30272, i16 30349, i16 30424, i16 30498, i16 30571, i16 30643, i16 30713, i16 30783, i16 30851, i16 30918, i16 30984, i16 31049, i16 31113, i16 31175, i16 31236, i16 31297, i16 31356, i16 31413, i16 31470, i16 31525, i16 31580, i16 31633, i16 31684, i16 31735, i16 31785, i16 31833, i16 31880, i16 31926, i16 31970, i16 32014, i16 32056, i16 32097, i16 32137, i16 32176, i16 32213, i16 32249, i16 32284, i16 32318, i16 32350, i16 32382, i16 32412, i16 32441, i16 32468, i16 32495, i16 32520, i16 32544, i16 32567, i16 32588, i16 32609, i16 32628, i16 32646, i16 32662, i16 32678, i16 32692, i16 32705, i16 32717, i16 32727, i16 32736, i16 32744, i16 32751, i16 32757, i16 32761, i16 32764, i16 32766, i16 32767, i16 32766, i16 32764, i16 32761, i16 32757, i16 32751, i16 32744, i16 32736, i16 32727, i16 32717, i16 32705, i16 32692, i16 32678, i16 32662, i16 32646, i16 32628, i16 32609, i16 32588, i16 32567, i16 32544, i16 32520, i16 32495, i16 32468, i16 32441, i16 32412, i16 32382, i16 32350, i16 32318, i16 32284, i16 32249, i16 32213, i16 32176, i16 32137, i16 32097, i16 32056, i16 32014, i16 31970, i16 31926, i16 31880, i16 31833, i16 31785, i16 31735, i16 31684, i16 31633, i16 31580, i16 31525, i16 31470, i16 31413, i16 31356, i16 31297, i16 31236, i16 31175, i16 31113, i16 31049, i16 30984, i16 30918, i16 30851, i16 30783, i16 30713, i16 30643, i16 30571, i16 30498, i16 30424, i16 30349, i16 30272, i16 30195, i16 30116, i16 30036, i16 29955, i16 29873, i16 29790, i16 29706, i16 29621, i16 29534, i16 29446, i16 29358, i16 29268, i16 29177, i16 29085, i16 28992, i16 28897, i16 28802, i16 28706, i16 28608, i16 28510, i16 28410, i16 28309, i16 28208, i16 28105, i16 28001, i16 27896, i16 27790, i16 27683, i16 27575, i16 27466, i16 27355, i16 27244, i16 27132, i16 27019, i16 26905, i16 26789, i16 26673, i16 26556, i16 26437, i16 26318, i16 26198, i16 26077, i16 25954, i16 25831, i16 25707, i16 25582, i16 25456, i16 25329, i16 25201, i16 25072, i16 24942, i16 24811, i16 24679, i16 24546, i16 24413, i16 24278, i16 24143, i16 24006, i16 23869, i16 23731, i16 23592, i16 23452, i16 23311, i16 23169, i16 23027, i16 22883, i16 22739, i16 22594, i16 22448, i16 22301, i16 22153, i16 22004, i16 21855, i16 21705, i16 21554, i16 21402, i16 21249, i16 21096, i16 20942, i16 20787, i16 20631, i16 20474, i16 20317, i16 20159, i16 20000, i16 19840, i16 19680, i16 19519, i16 19357, i16 19194, i16 19031, i16 18867, i16 18702, i16 18537, i16 18371, i16 18204, i16 18036, i16 17868, i16 17699, i16 17530, i16 17360, i16 17189, i16 17017, i16 16845, i16 16672, i16 16499, i16 16325, i16 16150, i16 15975, i16 15799, i16 15623, i16 15446, i16 15268, i16 15090, i16 14911, i16 14732, i16 14552, i16 14372, i16 14191, i16 14009, i16 13827, i16 13645, i16 13462, i16 13278, i16 13094, i16 12909, i16 12724, i16 12539, i16 12353, i16 12166, i16 11980, i16 11792, i16 11604, i16 11416, i16 11227, i16 11038, i16 10849, i16 10659, i16 10469, i16 10278, i16 10087, i16 9895, i16 9703, i16 9511, i16 9319, i16 9126, i16 8932, i16 8739, i16 8545, i16 8351, i16 8156, i16 7961, i16 7766, i16 7571, i16 7375, i16 7179, i16 6982, i16 6786, i16 6589, i16 6392, i16 6195, i16 5997, i16 5799, i16 5601, i16 5403, i16 5205, i16 5006, i16 4807, i16 4608, i16 4409, i16 4210, i16 4011, i16 3811, i16 3611, i16 3411, i16 3211, i16 3011, i16 2811, i16 2610, i16 2410, i16 2209, i16 2009, i16 1808, i16 1607, i16 1406, i16 1206, i16 1005, i16 804, i16 603, i16 402, i16 201, i16 0, i16 -201, i16 -402, i16 -603, i16 -804, i16 -1005, i16 -1206, i16 -1406, i16 -1607, i16 -1808, i16 -2009, i16 -2209, i16 -2410, i16 -2610, i16 -2811, i16 -3011, i16 -3211, i16 -3411, i16 -3611, i16 -3811, i16 -4011, i16 -4210, i16 -4409, i16 -4608, i16 -4807, i16 -5006, i16 -5205, i16 -5403, i16 -5601, i16 -5799, i16 -5997, i16 -6195, i16 -6392, i16 -6589, i16 -6786, i16 -6982, i16 -7179, i16 -7375, i16 -7571, i16 -7766, i16 -7961, i16 -8156, i16 -8351, i16 -8545, i16 -8739, i16 -8932, i16 -9126, i16 -9319, i16 -9511, i16 -9703, i16 -9895, i16 -10087, i16 -10278, i16 -10469, i16 -10659, i16 -10849, i16 -11038, i16 -11227, i16 -11416, i16 -11604, i16 -11792, i16 -11980, i16 -12166, i16 -12353, i16 -12539, i16 -12724, i16 -12909, i16 -13094, i16 -13278, i16 -13462, i16 -13645, i16 -13827, i16 -14009, i16 -14191, i16 -14372, i16 -14552, i16 -14732, i16 -14911, i16 -15090, i16 -15268, i16 -15446, i16 -15623, i16 -15799, i16 -15975, i16 -16150, i16 -16325, i16 -16499, i16 -16672, i16 -16845, i16 -17017, i16 -17189, i16 -17360, i16 -17530, i16 -17699, i16 -17868, i16 -18036, i16 -18204, i16 -18371, i16 -18537, i16 -18702, i16 -18867, i16 -19031, i16 -19194, i16 -19357, i16 -19519, i16 -19680, i16 -19840, i16 -20000, i16 -20159, i16 -20317, i16 -20474, i16 -20631, i16 -20787, i16 -20942, i16 -21096, i16 -21249, i16 -21402, i16 -21554, i16 -21705, i16 -21855, i16 -22004, i16 -22153, i16 -22301, i16 -22448, i16 -22594, i16 -22739, i16 -22883, i16 -23027, i16 -23169, i16 -23311, i16 -23452, i16 -23592, i16 -23731, i16 -23869, i16 -24006, i16 -24143, i16 -24278, i16 -24413, i16 -24546, i16 -24679, i16 -24811, i16 -24942, i16 -25072, i16 -25201, i16 -25329, i16 -25456, i16 -25582, i16 -25707, i16 -25831, i16 -25954, i16 -26077, i16 -26198, i16 -26318, i16 -26437, i16 -26556, i16 -26673, i16 -26789, i16 -26905, i16 -27019, i16 -27132, i16 -27244, i16 -27355, i16 -27466, i16 -27575, i16 -27683, i16 -27790, i16 -27896, i16 -28001, i16 -28105, i16 -28208, i16 -28309, i16 -28410, i16 -28510, i16 -28608, i16 -28706, i16 -28802, i16 -28897, i16 -28992, i16 -29085, i16 -29177, i16 -29268, i16 -29358, i16 -29446, i16 -29534, i16 -29621, i16 -29706, i16 -29790, i16 -29873, i16 -29955, i16 -30036, i16 -30116, i16 -30195, i16 -30272, i16 -30349, i16 -30424, i16 -30498, i16 -30571, i16 -30643, i16 -30713, i16 -30783, i16 -30851, i16 -30918, i16 -30984, i16 -31049, i16 -31113, i16 -31175, i16 -31236, i16 -31297, i16 -31356, i16 -31413, i16 -31470, i16 -31525, i16 -31580, i16 -31633, i16 -31684, i16 -31735, i16 -31785, i16 -31833, i16 -31880, i16 -31926, i16 -31970, i16 -32014, i16 -32056, i16 -32097, i16 -32137, i16 -32176, i16 -32213, i16 -32249, i16 -32284, i16 -32318, i16 -32350, i16 -32382, i16 -32412, i16 -32441, i16 -32468, i16 -32495, i16 -32520, i16 -32544, i16 -32567, i16 -32588, i16 -32609, i16 -32628, i16 -32646, i16 -32662, i16 -32678, i16 -32692, i16 -32705, i16 -32717, i16 -32727, i16 -32736, i16 -32744, i16 -32751, i16 -32757, i16 -32761, i16 -32764, i16 -32766], align 2
@.str = private unnamed_addr constant [14 x i8] c"fr:128,fi:128\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [10 x i8] c"fix_fft.c\00", section "llvm.metadata"
@.str.3 = private unnamed_addr constant [12 x i8] c"%d: %d, %d\0A\00", align 1
@llvm.global.annotations = appending global [2 x { i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i32 } { i8* bitcast (i32 (i16*, i16*, i16, i16)* @fix_fft to i8*), i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0), i32 170 }, { i8*, i8*, i8*, i32 } { i8* bitcast (i32 (i16*, i16*, i16)* @fix_fft_128 to i8*), i8* getelementptr inbounds ([14 x i8], [14 x i8]* @.str, i32 0, i32 0), i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.1, i32 0, i32 0), i32 293 }], section "llvm.metadata"
@str = private unnamed_addr constant [18 x i8] c"Original Samples:\00", align 1
@str.6 = private unnamed_addr constant [13 x i8] c"FFT Samples:\00", align 1
@str.7 = private unnamed_addr constant [23 x i8] c"Filtered IFFT Samples:\00", align 1

; Function Attrs: nounwind
define dso_local i32 @fix_fft(i16* nocapture %fr, i16* nocapture %fi, i16 signext %m1, i16 signext %inverse) #0 {
entry:
  br label %do.body.preheader

do.body.preheader:                                ; preds = %for.inc, %entry
  %indvars.iv = phi i32 [ 1, %entry ], [ %indvars.iv.next, %for.inc ]
  %mr.0252 = phi i32 [ 0, %entry ], [ %add8, %for.inc ]
  br label %do.body

while.cond.preheader:                             ; preds = %for.inc
  %tobool = icmp ne i16 %inverse, 0
  %not.tobool = xor i1 %tobool, true
  %0 = zext i1 %not.tobool to i16
  br label %while.body

do.body:                                          ; preds = %do.body, %do.body.preheader
  %l.0 = phi i32 [ %shr, %do.body ], [ 128, %do.body.preheader ]
  %shr = ashr i32 %l.0, 1
  %add = add nsw i32 %shr, %mr.0252
  %cmp5 = icmp sgt i32 %add, 127
  br i1 %cmp5, label %do.body, label %do.end

do.end:                                           ; preds = %do.body
  %shr.lcssa = phi i32 [ %shr, %do.body ]
  %sub7 = add nsw i32 %shr.lcssa, -1
  %and = and i32 %sub7, %mr.0252
  %add8 = add nsw i32 %and, %shr.lcssa
  %cmp10 = icmp sgt i32 %add8, %indvars.iv
  br i1 %cmp10, label %if.end13, label %for.inc

if.end13:                                         ; preds = %do.end
  %arrayidx = getelementptr inbounds i16, i16* %fr, i32 %indvars.iv
  %1 = load i16, i16* %arrayidx, align 2, !tbaa !3
  %arrayidx14 = getelementptr inbounds i16, i16* %fr, i32 %add8
  %2 = load i16, i16* %arrayidx14, align 2, !tbaa !3
  store i16 %2, i16* %arrayidx, align 2, !tbaa !3
  store i16 %1, i16* %arrayidx14, align 2, !tbaa !3
  %arrayidx19 = getelementptr inbounds i16, i16* %fi, i32 %indvars.iv
  %3 = load i16, i16* %arrayidx19, align 2, !tbaa !3
  %arrayidx20 = getelementptr inbounds i16, i16* %fi, i32 %add8
  %4 = load i16, i16* %arrayidx20, align 2, !tbaa !3
  store i16 %4, i16* %arrayidx19, align 2, !tbaa !3
  store i16 %3, i16* %arrayidx20, align 2, !tbaa !3
  br label %for.inc

for.inc:                                          ; preds = %if.end13, %do.end
  %indvars.iv.next = add nuw nsw i32 %indvars.iv, 1
  %exitcond = icmp eq i32 %indvars.iv.next, 128
  br i1 %exitcond, label %while.cond.preheader, label %do.body.preheader

while.body:                                       ; preds = %for.end115, %while.cond.preheader
  %k.0251 = phi i32 [ 9, %while.cond.preheader ], [ %dec, %for.end115 ]
  %l.1250 = phi i32 [ 1, %while.cond.preheader ], [ %shl28, %for.end115 ]
  %shl28 = shl i32 %l.1250, 1
  %cmp31247 = icmp sgt i32 %l.1250, 0
  br i1 %cmp31247, label %for.body33.preheader, label %for.end115

for.body33.preheader:                             ; preds = %while.body
  br label %for.body33

for.body33:                                       ; preds = %for.body33.preheader, %for.inc113
  %conv30249 = phi i32 [ %conv30, %for.inc113 ], [ 0, %for.body33.preheader ]
  %m.1248 = phi i16 [ %inc114, %for.inc113 ], [ 0, %for.body33.preheader ]
  %shl35 = shl i32 %conv30249, %k.0251
  %arrayidx38 = getelementptr inbounds [768 x i16], [768 x i16]* @Sinewave, i32 0, i32 %shl35
  %5 = load i16, i16* %arrayidx38, align 2, !tbaa !3
  %cmp59245 = icmp slt i32 %conv30249, 128
  br i1 %cmp59245, label %for.body61.lr.ph, label %for.inc113

for.body61.lr.ph:                                 ; preds = %for.body33
  %sub40 = sub i16 0, %5
  %spec.select = select i1 %tobool, i16 %5, i16 %sub40
  %wi.1 = ashr i16 %spec.select, %0
  %add36 = add nsw i32 %shl35, 256
  %arrayidx37 = getelementptr inbounds [768 x i16], [768 x i16]* @Sinewave, i32 0, i32 %add36
  %6 = load i16, i16* %arrayidx37, align 2, !tbaa !3
  %wr.0 = ashr i16 %6, %0
  %conv.i = sext i16 %wr.0 to i32
  %conv.i238 = sext i16 %wi.1 to i32
  br label %for.body61

for.body61:                                       ; preds = %for.body61, %for.body61.lr.ph
  %i.0246 = phi i32 [ %conv30249, %for.body61.lr.ph ], [ %add111, %for.body61 ]
  %add62 = add nsw i32 %i.0246, %l.1250
  %arrayidx63 = getelementptr inbounds i16, i16* %fr, i32 %add62
  %7 = load i16, i16* %arrayidx63, align 2, !tbaa !3
  %conv1.i = sext i16 %7 to i32
  %mul.i = mul nsw i32 %conv1.i, %conv.i
  %shr10.i = lshr i32 %mul.i, 14
  %conv2.i = and i32 %shr10.i, 1
  %8 = lshr i32 %mul.i, 15
  %add.i = add nuw nsw i32 %conv2.i, %8
  %conv5.i = trunc i32 %add.i to i16
  %arrayidx66 = getelementptr inbounds i16, i16* %fi, i32 %add62
  %9 = load i16, i16* %arrayidx66, align 2, !tbaa !3
  %conv1.i239 = sext i16 %9 to i32
  %mul.i240 = mul nsw i32 %conv1.i239, %conv.i238
  %shr10.i241 = lshr i32 %mul.i240, 14
  %conv2.i242 = and i32 %shr10.i241, 1
  %10 = lshr i32 %mul.i240, 15
  %add.i243 = add nuw nsw i32 %conv2.i242, %10
  %conv5.i244 = trunc i32 %add.i243 to i16
  %sub69 = sub i16 %conv5.i, %conv5.i244
  %mul.i233 = mul nsw i32 %conv1.i239, %conv.i
  %shr10.i234 = lshr i32 %mul.i233, 14
  %conv2.i235 = and i32 %shr10.i234, 1
  %11 = lshr i32 %mul.i233, 15
  %add.i236 = add nuw nsw i32 %conv2.i235, %11
  %conv5.i237 = trunc i32 %add.i236 to i16
  %mul.i226 = mul nsw i32 %conv1.i, %conv.i238
  %shr10.i227 = lshr i32 %mul.i226, 14
  %conv2.i228 = and i32 %shr10.i227, 1
  %12 = lshr i32 %mul.i226, 15
  %add.i229 = add nuw nsw i32 %conv2.i228, %12
  %conv5.i230 = trunc i32 %add.i229 to i16
  %add77 = add i16 %conv5.i237, %conv5.i230
  %arrayidx79 = getelementptr inbounds i16, i16* %fr, i32 %i.0246
  %13 = load i16, i16* %arrayidx79, align 2, !tbaa !3
  %arrayidx80 = getelementptr inbounds i16, i16* %fi, i32 %i.0246
  %14 = load i16, i16* %arrayidx80, align 2, !tbaa !3
  %qr.0 = ashr i16 %13, %0
  %qi.0 = ashr i16 %14, %0
  %sub92 = sub i16 %qr.0, %sub69
  store i16 %sub92, i16* %arrayidx63, align 2, !tbaa !3
  %sub97 = sub i16 %qi.0, %add77
  store i16 %sub97, i16* %arrayidx66, align 2, !tbaa !3
  %add102 = add i16 %sub69, %qr.0
  store i16 %add102, i16* %arrayidx79, align 2, !tbaa !3
  %add107 = add i16 %add77, %qi.0
  store i16 %add107, i16* %arrayidx80, align 2, !tbaa !3
  %add111 = add nsw i32 %i.0246, %shl28
  %cmp59 = icmp slt i32 %add111, 128
  br i1 %cmp59, label %for.body61, label %for.inc113.loopexit

for.inc113.loopexit:                              ; preds = %for.body61
  br label %for.inc113

for.inc113:                                       ; preds = %for.inc113.loopexit, %for.body33
  %inc114 = add i16 %m.1248, 1
  %conv30 = sext i16 %inc114 to i32
  %cmp31 = icmp sgt i32 %l.1250, %conv30
  br i1 %cmp31, label %for.body33, label %for.end115.loopexit

for.end115.loopexit:                              ; preds = %for.inc113
  br label %for.end115

for.end115:                                       ; preds = %for.end115.loopexit, %while.body
  %dec = add nsw i32 %k.0251, -1
  %cmp24 = icmp slt i32 %shl28, 128
  br i1 %cmp24, label %while.body, label %cleanup

cleanup:                                          ; preds = %for.end115
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #1

declare dso_local i32 @please_map_me(...) local_unnamed_addr #2

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #1

; Function Attrs: nofree norecurse nounwind
define dso_local i32 @fix_fft_128(i16* nocapture %fr, i16* nocapture %fi, i16 signext %inverse) #3 {
entry:
  br label %do.body.preheader

do.body.preheader:                                ; preds = %for.inc, %entry
  %indvars.iv = phi i32 [ 1, %entry ], [ %indvars.iv.next, %for.inc ]
  %mr.0306 = phi i32 [ 0, %entry ], [ %add8, %for.inc ]
  br label %do.body

while.cond.preheader:                             ; preds = %for.inc
  %tobool = icmp ne i16 %inverse, 0
  br label %while.body

do.body:                                          ; preds = %do.body, %do.body.preheader
  %l.0 = phi i32 [ %shr, %do.body ], [ 128, %do.body.preheader ]
  %shr = ashr i32 %l.0, 1
  %add = add nsw i32 %shr, %mr.0306
  %cmp5 = icmp sgt i32 %add, 127
  br i1 %cmp5, label %do.body, label %do.end

do.end:                                           ; preds = %do.body
  %shr.lcssa = phi i32 [ %shr, %do.body ]
  %sub7 = add nsw i32 %shr.lcssa, -1
  %and = and i32 %sub7, %mr.0306
  %add8 = add nsw i32 %and, %shr.lcssa
  %cmp10 = icmp sgt i32 %add8, %indvars.iv
  br i1 %cmp10, label %if.end13, label %for.inc

if.end13:                                         ; preds = %do.end
  %arrayidx = getelementptr inbounds i16, i16* %fr, i32 %indvars.iv
  %0 = load i16, i16* %arrayidx, align 2, !tbaa !3
  %arrayidx14 = getelementptr inbounds i16, i16* %fr, i32 %add8
  %1 = load i16, i16* %arrayidx14, align 2, !tbaa !3
  store i16 %1, i16* %arrayidx, align 2, !tbaa !3
  store i16 %0, i16* %arrayidx14, align 2, !tbaa !3
  %arrayidx19 = getelementptr inbounds i16, i16* %fi, i32 %indvars.iv
  %2 = load i16, i16* %arrayidx19, align 2, !tbaa !3
  %arrayidx20 = getelementptr inbounds i16, i16* %fi, i32 %add8
  %3 = load i16, i16* %arrayidx20, align 2, !tbaa !3
  store i16 %3, i16* %arrayidx19, align 2, !tbaa !3
  store i16 %2, i16* %arrayidx20, align 2, !tbaa !3
  br label %for.inc

for.inc:                                          ; preds = %if.end13, %do.end
  %indvars.iv.next = add nuw nsw i32 %indvars.iv, 1
  %exitcond309 = icmp eq i32 %indvars.iv.next, 128
  br i1 %exitcond309, label %while.cond.preheader, label %do.body.preheader

while.body:                                       ; preds = %for.end148, %while.cond.preheader
  %scale.0305 = phi i32 [ 0, %while.cond.preheader ], [ %scale.1, %for.end148 ]
  %k.0304 = phi i32 [ 9, %while.cond.preheader ], [ %dec, %for.end148 ]
  %l.1303 = phi i32 [ 1, %while.cond.preheader ], [ %shl62, %for.end148 ]
  br i1 %tobool, label %for.body30.preheader, label %if.end61

for.body30.preheader:                             ; preds = %while.body
  br label %for.body30

for.cond27:                                       ; preds = %for.body30
  %inc55 = add nuw nsw i32 %i.0297, 1
  %exitcond = icmp eq i32 %inc55, 128
  br i1 %exitcond, label %for.end56, label %for.body30

for.body30:                                       ; preds = %for.body30.preheader, %for.cond27
  %i.0297 = phi i32 [ %inc55, %for.cond27 ], [ 0, %for.body30.preheader ]
  %arrayidx31 = getelementptr inbounds i16, i16* %fr, i32 %i.0297
  %4 = load i16, i16* %arrayidx31, align 2, !tbaa !3
  %conv32 = sext i16 %4 to i32
  %cmp33 = icmp slt i16 %4, 0
  %sub36 = sub nsw i32 0, %conv32
  %spec.select = select i1 %cmp33, i32 %sub36, i32 %conv32
  %arrayidx38 = getelementptr inbounds i16, i16* %fi, i32 %i.0297
  %5 = load i16, i16* %arrayidx38, align 2, !tbaa !3
  %cmp40 = icmp slt i16 %5, 0
  %sub44 = sub i16 0, %5
  %m.1 = select i1 %cmp40, i16 %sub44, i16 %5
  %cmp47 = icmp sgt i32 %spec.select, 16383
  %cmp50 = icmp sgt i16 %m.1, 16383
  %or.cond = or i1 %cmp47, %cmp50
  br i1 %or.cond, label %for.end56, label %for.cond27

for.end56:                                        ; preds = %for.body30, %for.cond27
  %shift.0 = phi i32 [ 0, %for.cond27 ], [ 1, %for.body30 ]
  %spec.select272 = add nsw i32 %shift.0, %scale.0305
  br label %if.end61

if.end61:                                         ; preds = %for.end56, %while.body
  %scale.1 = phi i32 [ %spec.select272, %for.end56 ], [ %scale.0305, %while.body ]
  %shift.1 = phi i32 [ %shift.0, %for.end56 ], [ 1, %while.body ]
  %shl62 = shl i32 %l.1303, 1
  %cmp65300 = icmp sgt i32 %l.1303, 0
  br i1 %cmp65300, label %for.body67.lr.ph, label %for.end148

for.body67.lr.ph:                                 ; preds = %if.end61
  %tobool82 = icmp ne i32 %shift.1, 0
  %6 = zext i1 %tobool82 to i16
  br label %for.body67

for.body67:                                       ; preds = %for.inc146, %for.body67.lr.ph
  %conv64302 = phi i32 [ 0, %for.body67.lr.ph ], [ %conv64, %for.inc146 ]
  %m.2301 = phi i16 [ 0, %for.body67.lr.ph ], [ %inc147, %for.inc146 ]
  %shl69 = shl i32 %conv64302, %k.0304
  %arrayidx72 = getelementptr inbounds [768 x i16], [768 x i16]* @Sinewave, i32 0, i32 %shl69
  %7 = load i16, i16* %arrayidx72, align 2, !tbaa !3
  %cmp93298 = icmp slt i32 %conv64302, 128
  br i1 %cmp93298, label %for.body95.lr.ph, label %for.inc146

for.body95.lr.ph:                                 ; preds = %for.body67
  %sub74 = sub i16 0, %7
  %spec.select274 = select i1 %tobool, i16 %7, i16 %sub74
  %wi.1 = ashr i16 %spec.select274, %6
  %add70 = add nsw i32 %shl69, 256
  %arrayidx71 = getelementptr inbounds [768 x i16], [768 x i16]* @Sinewave, i32 0, i32 %add70
  %8 = load i16, i16* %arrayidx71, align 2, !tbaa !3
  %wr.0 = ashr i16 %8, %6
  %conv.i = sext i16 %wr.0 to i32
  %conv.i289 = sext i16 %wi.1 to i32
  br label %for.body95

for.body95:                                       ; preds = %for.body95, %for.body95.lr.ph
  %i.1299 = phi i32 [ %conv64302, %for.body95.lr.ph ], [ %add144, %for.body95 ]
  %add96 = add nsw i32 %i.1299, %l.1303
  %arrayidx97 = getelementptr inbounds i16, i16* %fr, i32 %add96
  %9 = load i16, i16* %arrayidx97, align 2, !tbaa !3
  %conv1.i = sext i16 %9 to i32
  %mul.i = mul nsw i32 %conv1.i, %conv.i
  %shr10.i = lshr i32 %mul.i, 14
  %conv2.i = and i32 %shr10.i, 1
  %10 = lshr i32 %mul.i, 15
  %add.i = add nuw nsw i32 %conv2.i, %10
  %conv5.i = trunc i32 %add.i to i16
  %arrayidx99 = getelementptr inbounds i16, i16* %fi, i32 %add96
  %11 = load i16, i16* %arrayidx99, align 2, !tbaa !3
  %conv1.i290 = sext i16 %11 to i32
  %mul.i291 = mul nsw i32 %conv1.i290, %conv.i289
  %shr10.i292 = lshr i32 %mul.i291, 14
  %conv2.i293 = and i32 %shr10.i292, 1
  %12 = lshr i32 %mul.i291, 15
  %add.i294 = add nuw nsw i32 %conv2.i293, %12
  %conv5.i295 = trunc i32 %add.i294 to i16
  %sub102 = sub i16 %conv5.i, %conv5.i295
  %mul.i284 = mul nsw i32 %conv1.i290, %conv.i
  %shr10.i285 = lshr i32 %mul.i284, 14
  %conv2.i286 = and i32 %shr10.i285, 1
  %13 = lshr i32 %mul.i284, 15
  %add.i287 = add nuw nsw i32 %conv2.i286, %13
  %conv5.i288 = trunc i32 %add.i287 to i16
  %mul.i277 = mul nsw i32 %conv1.i, %conv.i289
  %shr10.i278 = lshr i32 %mul.i277, 14
  %conv2.i279 = and i32 %shr10.i278, 1
  %14 = lshr i32 %mul.i277, 15
  %add.i280 = add nuw nsw i32 %conv2.i279, %14
  %conv5.i281 = trunc i32 %add.i280 to i16
  %add110 = add i16 %conv5.i288, %conv5.i281
  %arrayidx112 = getelementptr inbounds i16, i16* %fr, i32 %i.1299
  %15 = load i16, i16* %arrayidx112, align 2, !tbaa !3
  %arrayidx113 = getelementptr inbounds i16, i16* %fi, i32 %i.1299
  %16 = load i16, i16* %arrayidx113, align 2, !tbaa !3
  %qr.0 = ashr i16 %15, %6
  %qi.0 = ashr i16 %16, %6
  %sub125 = sub i16 %qr.0, %sub102
  store i16 %sub125, i16* %arrayidx97, align 2, !tbaa !3
  %sub130 = sub i16 %qi.0, %add110
  store i16 %sub130, i16* %arrayidx99, align 2, !tbaa !3
  %add135 = add i16 %sub102, %qr.0
  store i16 %add135, i16* %arrayidx112, align 2, !tbaa !3
  %add140 = add i16 %add110, %qi.0
  store i16 %add140, i16* %arrayidx113, align 2, !tbaa !3
  %add144 = add nsw i32 %i.1299, %shl62
  %cmp93 = icmp slt i32 %add144, 128
  br i1 %cmp93, label %for.body95, label %for.inc146.loopexit

for.inc146.loopexit:                              ; preds = %for.body95
  br label %for.inc146

for.inc146:                                       ; preds = %for.inc146.loopexit, %for.body67
  %inc147 = add i16 %m.2301, 1
  %conv64 = sext i16 %inc147 to i32
  %cmp65 = icmp sgt i32 %l.1303, %conv64
  br i1 %cmp65, label %for.body67, label %for.end148.loopexit

for.end148.loopexit:                              ; preds = %for.inc146
  br label %for.end148

for.end148:                                       ; preds = %for.end148.loopexit, %if.end61
  %dec = add nsw i32 %k.0304, -1
  %cmp24 = icmp slt i32 %shl62, 128
  br i1 %cmp24, label %while.body, label %cleanup

cleanup:                                          ; preds = %for.end148
  %scale.1.lcssa = phi i32 [ %scale.1, %for.end148 ]
  ret i32 %scale.1.lcssa
}

; Function Attrs: nounwind
define dso_local i32 @fix_fftr(i16* nocapture %f, i32 %m, i32 %inverse) local_unnamed_addr #4 {
entry:
  %sub = add nsw i32 %m, -1
  %shl = shl nuw i32 1, %sub
  %arrayidx = getelementptr inbounds i16, i16* %f, i32 %shl
  %tobool = icmp ne i32 %inverse, 0
  br i1 %tobool, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %conv2 = trunc i32 %inverse to i16
  %call = tail call i32 @fix_fft(i16* nonnull %arrayidx, i16* %f, i16 signext undef, i16 signext %conv2)
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %cmp45 = icmp sgt i32 %shl, 1
  br i1 %cmp45, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %if.end
  %add = add nsw i32 %shl, -1
  br label %for.body

for.body:                                         ; preds = %for.body, %for.body.lr.ph
  %i.046 = phi i32 [ 1, %for.body.lr.ph ], [ %add11, %for.body ]
  %sub4 = add i32 %add, %i.046
  %arrayidx5 = getelementptr inbounds i16, i16* %f, i32 %sub4
  %0 = load i16, i16* %arrayidx5, align 2, !tbaa !3
  %arrayidx6 = getelementptr inbounds i16, i16* %f, i32 %i.046
  %1 = load i16, i16* %arrayidx6, align 2, !tbaa !3
  store i16 %1, i16* %arrayidx5, align 2, !tbaa !3
  store i16 %0, i16* %arrayidx6, align 2, !tbaa !3
  %add11 = add nuw nsw i32 %i.046, 2
  %cmp = icmp slt i32 %add11, %shl
  br i1 %cmp, label %for.body, label %for.end.loopexit

for.end.loopexit:                                 ; preds = %for.body
  br label %for.end

for.end:                                          ; preds = %for.end.loopexit, %if.end
  br i1 %tobool, label %if.end18, label %if.then13

if.then13:                                        ; preds = %for.end
  %call17 = tail call i32 @fix_fft(i16* nonnull %arrayidx, i16* %f, i16 signext undef, i16 signext 0)
  br label %if.end18

if.end18:                                         ; preds = %if.then13, %for.end
  ret i32 0
}

; Function Attrs: nofree norecurse nounwind writeonly
define dso_local void @filter(i32 %startF, i32 %endF, i16* nocapture %real, i16* nocapture %imag, i32 %N) local_unnamed_addr #5 {
entry:
  %div = sdiv i32 %N, 2
  %cmp33 = icmp sgt i32 %N, 1
  br i1 %cmp33, label %for.body.lr.ph, label %for.end

for.body.lr.ph:                                   ; preds = %entry
  %sub5 = sub nsw i32 %N, %endF
  %sub8 = sub nsw i32 %N, %startF
  br label %for.body

for.body:                                         ; preds = %if.end13, %for.body.lr.ph
  %i.034 = phi i32 [ 0, %for.body.lr.ph ], [ %inc, %if.end13 ]
  %cmp1 = icmp slt i32 %i.034, %startF
  %cmp2 = icmp sgt i32 %i.034, %endF
  %or.cond = or i1 %cmp1, %cmp2
  br i1 %or.cond, label %if.then, label %if.end

if.then:                                          ; preds = %for.body
  %arrayidx = getelementptr inbounds i16, i16* %real, i32 %i.034
  store i16 0, i16* %arrayidx, align 2, !tbaa !3
  %arrayidx3 = getelementptr inbounds i16, i16* %imag, i32 %i.034
  store i16 0, i16* %arrayidx3, align 2, !tbaa !3
  br label %if.end

if.end:                                           ; preds = %if.then, %for.body
  %0 = xor i32 %i.034, -1
  %sub4 = add i32 %0, %N
  %cmp6 = icmp slt i32 %sub4, %sub5
  %cmp9 = icmp sgt i32 %sub4, %sub8
  %or.cond32 = or i1 %cmp6, %cmp9
  br i1 %or.cond32, label %if.then10, label %if.end13

if.then10:                                        ; preds = %if.end
  %arrayidx11 = getelementptr inbounds i16, i16* %real, i32 %sub4
  store i16 0, i16* %arrayidx11, align 2, !tbaa !3
  %arrayidx12 = getelementptr inbounds i16, i16* %imag, i32 %sub4
  store i16 0, i16* %arrayidx12, align 2, !tbaa !3
  br label %if.end13

if.end13:                                         ; preds = %if.then10, %if.end
  %inc = add nuw nsw i32 %i.034, 1
  %exitcond = icmp eq i32 %inc, %div
  br i1 %exitcond, label %for.end.loopexit, label %for.body

for.end.loopexit:                                 ; preds = %if.end13
  br label %for.end

for.end:                                          ; preds = %for.end.loopexit, %entry
  ret void
}

; Function Attrs: nounwind
define dso_local i32 @main() local_unnamed_addr #4 {
entry:
  %real = alloca [128 x i16], align 2
  %imag = alloca [128 x i16], align 2
  %0 = bitcast [128 x i16]* %imag to i8*
  %1 = bitcast [128 x i16]* %real to i8*
  call void @llvm.lifetime.start.p0i8(i64 256, i8* nonnull %1) #8
  call void @llvm.lifetime.start.p0i8(i64 256, i8* nonnull %0) #8
  call void @llvm.memset.p0i8.i32(i8* nonnull align 2 dereferenceable(256) %0, i8 0, i32 256, i1 false)
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %i.073 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %arrayidx = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %i.073
  store i16 500, i16* %arrayidx, align 2, !tbaa !3
  %inc = add nuw nsw i32 %i.073, 1
  %exitcond76 = icmp eq i32 %inc, 128
  br i1 %exitcond76, label %for.end, label %for.body

for.end:                                          ; preds = %for.body
  %puts = tail call i32 @puts(i8* nonnull dereferenceable(1) getelementptr inbounds ([18 x i8], [18 x i8]* @str, i32 0, i32 0))
  %arrayidx580 = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 0
  %2 = load i16, i16* %arrayidx580, align 2, !tbaa !3
  %conv81 = sext i16 %2 to i32
  %call882 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([12 x i8], [12 x i8]* @.str.3, i32 0, i32 0), i32 0, i32 %conv81, i32 0)
  br label %for.body4.for.body4_crit_edge

for.body4.for.body4_crit_edge:                    ; preds = %for.body4.for.body4_crit_edge, %for.end
  %inc1083 = phi i32 [ 1, %for.end ], [ %inc10, %for.body4.for.body4_crit_edge ]
  %arrayidx6.phi.trans.insert = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 %inc1083
  %.pre = load i16, i16* %arrayidx6.phi.trans.insert, align 2, !tbaa !3
  %arrayidx5 = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %inc1083
  %3 = load i16, i16* %arrayidx5, align 2, !tbaa !3
  %conv = sext i16 %3 to i32
  %conv7 = sext i16 %.pre to i32
  %call8 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([12 x i8], [12 x i8]* @.str.3, i32 0, i32 0), i32 %inc1083, i32 %conv, i32 %conv7)
  %inc10 = add nuw nsw i32 %inc1083, 1
  %exitcond75 = icmp eq i32 %inc10, 128
  br i1 %exitcond75, label %for.end11, label %for.body4.for.body4_crit_edge

for.end11:                                        ; preds = %for.body4.for.body4_crit_edge
  %arraydecay12 = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 0
  %call13 = call i32 @fix_fft_128(i16* nonnull %arrayidx580, i16* nonnull %arraydecay12, i16 signext 0)
  %puts68 = tail call i32 @puts(i8* nonnull dereferenceable(1) getelementptr inbounds ([13 x i8], [13 x i8]* @str.6, i32 0, i32 0))
  br label %for.body18

for.body18:                                       ; preds = %for.body18, %for.end11
  %i.271 = phi i32 [ 0, %for.end11 ], [ %inc25, %for.body18 ]
  %arrayidx19 = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %i.271
  %4 = load i16, i16* %arrayidx19, align 2, !tbaa !3
  %conv20 = sext i16 %4 to i32
  %arrayidx21 = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 %i.271
  %5 = load i16, i16* %arrayidx21, align 2, !tbaa !3
  %conv22 = sext i16 %5 to i32
  %call23 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([12 x i8], [12 x i8]* @.str.3, i32 0, i32 0), i32 %i.271, i32 %conv20, i32 %conv22)
  %inc25 = add nuw nsw i32 %i.271, 1
  %exitcond74 = icmp eq i32 %inc25, 128
  br i1 %exitcond74, label %for.body.i.preheader, label %for.body18

for.body.i.preheader:                             ; preds = %for.body18
  br label %for.body.i

for.body.i:                                       ; preds = %for.body.i.preheader, %if.end13.i
  %i.034.i = phi i32 [ %inc.i, %if.end13.i ], [ 0, %for.body.i.preheader ]
  %cmp2.i = icmp ugt i32 %i.034.i, 1
  br i1 %cmp2.i, label %if.end.i.thread, label %if.end.i

if.end.i.thread:                                  ; preds = %for.body.i
  %arrayidx.i = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %i.034.i
  store i16 0, i16* %arrayidx.i, align 2, !tbaa !3
  %arrayidx3.i = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 %i.034.i
  store i16 0, i16* %arrayidx3.i, align 2, !tbaa !3
  br label %if.then10.i

if.end.i:                                         ; preds = %for.body.i
  %cmp6.i = icmp eq i32 %i.034.i, 0
  br i1 %cmp6.i, label %if.end13.i, label %if.then10.i

if.then10.i:                                      ; preds = %if.end.i, %if.end.i.thread
  %sub4.i79 = sub nuw nsw i32 127, %i.034.i
  %arrayidx11.i = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %sub4.i79
  store i16 0, i16* %arrayidx11.i, align 2, !tbaa !3
  %arrayidx12.i = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 %sub4.i79
  store i16 0, i16* %arrayidx12.i, align 2, !tbaa !3
  br label %if.end13.i

if.end13.i:                                       ; preds = %if.then10.i, %if.end.i
  %inc.i = add nuw nsw i32 %i.034.i, 1
  %exitcond.i = icmp eq i32 %inc.i, 64
  br i1 %exitcond.i, label %filter.exit, label %for.body.i

filter.exit:                                      ; preds = %if.end13.i
  %call31 = call i32 @fix_fft_128(i16* nonnull %arrayidx580, i16* nonnull %arraydecay12, i16 signext 1)
  %puts69 = tail call i32 @puts(i8* nonnull dereferenceable(1) getelementptr inbounds ([23 x i8], [23 x i8]* @str.7, i32 0, i32 0))
  br label %for.body36

for.body36:                                       ; preds = %for.body36, %filter.exit
  %i.370 = phi i32 [ 0, %filter.exit ], [ %inc43, %for.body36 ]
  %arrayidx37 = getelementptr inbounds [128 x i16], [128 x i16]* %real, i32 0, i32 %i.370
  %6 = load i16, i16* %arrayidx37, align 2, !tbaa !3
  %conv38 = sext i16 %6 to i32
  %arrayidx39 = getelementptr inbounds [128 x i16], [128 x i16]* %imag, i32 0, i32 %i.370
  %7 = load i16, i16* %arrayidx39, align 2, !tbaa !3
  %conv40 = sext i16 %7 to i32
  %call41 = tail call i32 (i8*, ...) @printf(i8* nonnull dereferenceable(1) getelementptr inbounds ([12 x i8], [12 x i8]* @.str.3, i32 0, i32 0), i32 %i.370, i32 %conv38, i32 %conv40)
  %inc43 = add nuw nsw i32 %i.370, 1
  %exitcond = icmp eq i32 %inc43, 128
  br i1 %exitcond, label %for.end44, label %for.body36

for.end44:                                        ; preds = %for.body36
  %call47 = call i32 @fix_fft(i16* nonnull %arrayidx580, i16* nonnull %arraydecay12, i16 signext undef, i16 signext 0)
  call void @llvm.lifetime.end.p0i8(i64 256, i8* nonnull %0) #8
  call void @llvm.lifetime.end.p0i8(i64 256, i8* nonnull %1) #8
  ret i32 0
}

; Function Attrs: nofree nounwind
declare dso_local i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #6

; Function Attrs: nofree nounwind
declare i32 @puts(i8* nocapture readonly) local_unnamed_addr #7

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i32(i8* nocapture writeonly, i8, i32, i1 immarg) #1

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "size"="fr:128,fi:128" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nofree norecurse nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "size"="fr:128,fi:128" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nofree norecurse nounwind writeonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nofree nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="pentium4" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nofree nounwind }
attributes #8 = { nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"NumRegisterParameters", i32 0}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{!"clang version 10.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"short", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
