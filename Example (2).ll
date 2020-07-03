; ModuleID = 'Example.cpp'
source_filename = "Example.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%EFFECTIVE_INFO = type { %EFFECTIVE_TID*, i8*, i32, i32, i32, %EFFECTIVE_INFO*, [0 x %EFFECTIVE_INFO_ENTRY] }
%EFFECTIVE_TID = type { i64, i64 }
%EFFECTIVE_INFO_ENTRY = type { %EFFECTIVE_INFO*, i32, i64, i64 }
%EFFECTIVE_TYPE_3 = type { i64, i64, i32, i32, i32, i32, i64, i64, %EFFECTIVE_INFO*, i64, [3 x %EFFECTIVE_ENTRY] }
%EFFECTIVE_ENTRY = type { i64, i64, <2 x i64> }
%EFFECTIVE_INFO_2 = type { %EFFECTIVE_TID*, i8*, i32, i32, i32, %EFFECTIVE_INFO*, [2 x %EFFECTIVE_INFO_ENTRY] }
%EFFECTIVE_TYPE_33 = type { i64, i64, i32, i32, i32, i32, i64, i64, %EFFECTIVE_INFO*, i64, [33 x %EFFECTIVE_ENTRY] }
%EFFECTIVE_TYPE_17 = type { i64, i64, i32, i32, i32, i32, i64, i64, %EFFECTIVE_INFO*, i64, [17 x %EFFECTIVE_ENTRY] }
%EFFECTIVE_TYPE = type { i64, i64, i32, i32, i32, i32, i64, i64, %EFFECTIVE_INFO*, i64, [0 x %EFFECTIVE_ENTRY] }
%struct.S = type { float, %struct.T }
%struct.T = type { [3 x i32], i8* }
%class.MyClass = type { %struct.T*, double }

@EFFECTIVE_INFO_INT8 = external global %EFFECTIVE_INFO
@EFFECTIVE_TYPE_INT8 = weak local_unnamed_addr constant %EFFECTIVE_TYPE_3 { i64 8088147823597479805, i64 8088147823597479805, i32 1, i32 1, i32 0, i32 1340864923, i64 -9223372036854775808, i64 1, %EFFECTIVE_INFO* @EFFECTIVE_INFO_INT8, i64 8088147823597479805, [3 x %EFFECTIVE_ENTRY] [%EFFECTIVE_ENTRY { i64 3080687966, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }
@EFFECTIVE_STRING = private unnamed_addr constant [9 x i8] c"struct T\00"
@EFFECTIVE_INFO_INT32 = external global %EFFECTIVE_INFO
@EFFECTIVE_STRING.1 = private unnamed_addr constant [9 x i8] c"int8_t *\00"
@EFFECTIVE_TID_a481e2de8ae4613074fac0bfec5c40a = weak global %EFFECTIVE_TID { i64 12, i64 0 }
@EFFECTIVE_INFO_a481e2de8ae4613074fac0bfec5c40a = weak constant %EFFECTIVE_INFO { %EFFECTIVE_TID* @EFFECTIVE_TID_a481e2de8ae4613074fac0bfec5c40a, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @EFFECTIVE_STRING.1, i32 0, i32 0), i32 8, i32 0, i32 0, %EFFECTIVE_INFO* null, [0 x %EFFECTIVE_INFO_ENTRY] zeroinitializer }
@EFFECTIVE_TID_3c6134c1f3f9ef3660f1e3b80d346185 = weak global %EFFECTIVE_TID { i64 12, i64 0 }
@EFFECTIVE_INFO_3c6134c1f3f9ef3660f1e3b80d346185 = weak constant %EFFECTIVE_INFO_2 { %EFFECTIVE_TID* @EFFECTIVE_TID_3c6134c1f3f9ef3660f1e3b80d346185, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @EFFECTIVE_STRING, i32 0, i32 0), i32 24, i32 2, i32 0, %EFFECTIVE_INFO* null, [2 x %EFFECTIVE_INFO_ENTRY] [%EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* @EFFECTIVE_INFO_INT32, i32 0, i64 0, i64 12 }, %EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* @EFFECTIVE_INFO_a481e2de8ae4613074fac0bfec5c40a, i32 0, i64 16, i64 24 }] }
@EFFECTIVE_TYPE_3c6134c1f3f9ef3660f1e3b80d346185 = weak constant %EFFECTIVE_TYPE_33 { i64 -8835723760383037088, i64 -8835723760383037088, i32 24, i32 24, i32 0, i32 1340864923, i64 384307168202282326, i64 31, %EFFECTIVE_INFO* bitcast (%EFFECTIVE_INFO_2* @EFFECTIVE_INFO_3c6134c1f3f9ef3660f1e3b80d346185 to %EFFECTIVE_INFO*), i64 8088147823597479805, [33 x %EFFECTIVE_ENTRY] [%EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1182908642411952126, i64 0, <2 x i64> <i64 -4, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 607098447528405577, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -6451725993040529014, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1182908642964911505, i64 0, <2 x i64> <i64 0, i64 12> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 3453077973, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, %EFFECTIVE_ENTRY { i64 -1182908644070699339, i64 0, <2 x i64> <i64 -8, i64 4> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }
@EFFECTIVE_STRING.2 = private unnamed_addr constant [9 x i8] c"struct S\00"
@EFFECTIVE_INFO_FLOAT32 = external global %EFFECTIVE_INFO
@EFFECTIVE_TID_595d50ef73362a16107c3d51e3ff989f = weak global %EFFECTIVE_TID { i64 13, i64 0 }
@EFFECTIVE_INFO_595d50ef73362a16107c3d51e3ff989f = weak constant %EFFECTIVE_INFO_2 { %EFFECTIVE_TID* @EFFECTIVE_TID_595d50ef73362a16107c3d51e3ff989f, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @EFFECTIVE_STRING.2, i32 0, i32 0), i32 32, i32 2, i32 0, %EFFECTIVE_INFO* null, [2 x %EFFECTIVE_INFO_ENTRY] [%EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* @EFFECTIVE_INFO_FLOAT32, i32 0, i64 0, i64 4 }, %EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* bitcast (%EFFECTIVE_INFO_2* @EFFECTIVE_INFO_3c6134c1f3f9ef3660f1e3b80d346185 to %EFFECTIVE_INFO*), i32 0, i64 8, i64 32 }] }
@EFFECTIVE_TYPE_595d50ef73362a16107c3d51e3ff989f = weak constant %EFFECTIVE_TYPE_33 { i64 -6946521073433347056, i64 -6946521073433347056, i32 32, i32 32, i32 0, i32 1340864923, i64 288230376151711744, i64 31, %EFFECTIVE_INFO* bitcast (%EFFECTIVE_INFO_2* @EFFECTIVE_INFO_595d50ef73362a16107c3d51e3ff989f to %EFFECTIVE_INFO*), i64 8088147823597479805, [33 x %EFFECTIVE_ENTRY] [%EFFECTIVE_ENTRY { i64 -4859640686798983136, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 1338997616515130339, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 1280657732, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, %EFFECTIVE_ENTRY { i64 -762026197613558843, i64 0, <2 x i64> <i64 0, i64 12> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -762026198167567960, i64 0, <2 x i64> <i64 -4, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -762026200863420757, i64 0, <2 x i64> <i64 -8, i64 4> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -5595124967339514509, i64 0, <2 x i64> <i64 0, i64 4> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 1943808938964435071, i64 0, <2 x i64> <i64 0, i64 24> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }
@EFFECTIVE_STRING.3 = private unnamed_addr constant [14 x i8] c"class MyClass\00"
@EFFECTIVE_STRING.4 = private unnamed_addr constant [11 x i8] c"struct T *\00"
@EFFECTIVE_TID_ed9d1e262f3cfa34b553ddd10e2ab458 = weak global %EFFECTIVE_TID { i64 14, i64 0 }
@EFFECTIVE_INFO_ed9d1e262f3cfa34b553ddd10e2ab458 = weak constant %EFFECTIVE_INFO { %EFFECTIVE_TID* @EFFECTIVE_TID_ed9d1e262f3cfa34b553ddd10e2ab458, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @EFFECTIVE_STRING.4, i32 0, i32 0), i32 8, i32 0, i32 0, %EFFECTIVE_INFO* null, [0 x %EFFECTIVE_INFO_ENTRY] zeroinitializer }
@EFFECTIVE_INFO_FLOAT64 = external global %EFFECTIVE_INFO
@EFFECTIVE_TID_60bec0f67d6986decc4d1dced3506490 = weak global %EFFECTIVE_TID { i64 15, i64 0 }
@EFFECTIVE_INFO_60bec0f67d6986decc4d1dced3506490 = weak constant %EFFECTIVE_INFO_2 { %EFFECTIVE_TID* @EFFECTIVE_TID_60bec0f67d6986decc4d1dced3506490, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @EFFECTIVE_STRING.3, i32 0, i32 0), i32 16, i32 2, i32 0, %EFFECTIVE_INFO* null, [2 x %EFFECTIVE_INFO_ENTRY] [%EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* @EFFECTIVE_INFO_ed9d1e262f3cfa34b553ddd10e2ab458, i32 0, i64 0, i64 8 }, %EFFECTIVE_INFO_ENTRY { %EFFECTIVE_INFO* @EFFECTIVE_INFO_FLOAT64, i32 0, i64 8, i64 16 }] }
@EFFECTIVE_TYPE_60bec0f67d6986decc4d1dced3506490 = weak constant %EFFECTIVE_TYPE_17 { i64 -8042214163950514740, i64 -8042214163950514740, i32 16, i32 16, i32 0, i32 1340864923, i64 576460752303423488, i64 15, %EFFECTIVE_INFO* bitcast (%EFFECTIVE_INFO_2* @EFFECTIVE_INFO_60bec0f67d6986decc4d1dced3506490 to %EFFECTIVE_INFO*), i64 8088147823597479805, [17 x %EFFECTIVE_ENTRY] [%EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -3976543378853807615, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -5515860907162170222, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 1164077333, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 4661320220429578043, i64 0, <2 x i64> <i64 0, i64 8> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }

@EFFECTIVE_TYPE_FLOAT64 = weak constant %EFFECTIVE_TYPE_3 { 
  i64 -3399084293635617453, 
  i64 -3399084293635617453, 
  i32 8, 
  i32 8, 
  i32 0, 
  i32 1340864923, 
  i64 1152921504606846976, 
  i64 1, 
  %EFFECTIVE_INFO* @EFFECTIVE_INFO_FLOAT64, 
  i64 8088147823597479805, 
  [3 x %EFFECTIVE_ENTRY] 
  [%EFFECTIVE_ENTRY { i64 2870471550, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, 
  %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, 
  %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }


@EFFECTIVE_TYPE_INT32 = weak constant %EFFECTIVE_TYPE_3 { i64 7706858352182509634, i64 7706858352182509634, i32 4, i32 4, i32 0, i32 1340864923, i64 2305843009213693952, i64 1, %EFFECTIVE_INFO* @EFFECTIVE_INFO_INT32, i64 8088147823597479805, [3 x %EFFECTIVE_ENTRY] [%EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }, %EFFECTIVE_ENTRY { i64 1319304013, i64 0, <2 x i64> <i64 -17179869184, i64 17179869184> }, %EFFECTIVE_ENTRY { i64 -1, i64 0, <2 x i64> zeroinitializer }] }

; Function Attrs: norecurse noreturn uwtable
define i32 @main() local_unnamed_addr #0 !dbg !48 {
  %1 = tail call <2 x i64> @effective__Znam(i64 240, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_33* @EFFECTIVE_TYPE_3c6134c1f3f9ef3660f1e3b80d346185 to %EFFECTIVE_TYPE*)), !dbg !50
  %2 = tail call <2 x i64> @effective__Znam(i64 3200, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_33* @EFFECTIVE_TYPE_595d50ef73362a16107c3d51e3ff989f to %EFFECTIVE_TYPE*)), !dbg !51
  %3 = extractelement <2 x i64> %2, i32 0, !dbg !51
  %4 = inttoptr i64 %3 to %struct.S*, !dbg !51
  %5 = getelementptr inbounds %struct.S, %struct.S* %4, i64 10, i32 1, !dbg !52
  %6 = getelementptr inbounds %struct.T, %struct.T* %5, i64 0, i32 0, i64 0, !dbg !53
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 0), !dbg !54
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 1), !dbg !55
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 2), !dbg !56
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 3), !dbg !57
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 -1), !dbg !58
  %7 = bitcast %struct.T* %5 to double*, !dbg !59
  %8 = bitcast %struct.T* %5 to i8*, !dbg !60
  %9 = tail call <2 x i64> @effective_type_check(i8* %8, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_3* @EFFECTIVE_TYPE_FLOAT64 to %EFFECTIVE_TYPE*)), !dbg !60
  %10 = ptrtoint %struct.T* %5 to i64, !dbg !61
  %11 = insertelement <2 x i64> undef, i64 %10, i32 0, !dbg !61
  %12 = shufflevector <2 x i64> %11, <2 x i64> undef, <2 x i32> zeroinitializer, !dbg !61
  %13 = add <2 x i64> %9, <i64 -1, i64 0>, !dbg !61
  %14 = icmp sgt <2 x i64> %12, %13, !dbg !61
  %15 = sext <2 x i1> %14 to <2 x i64>, !dbg !61
  %16 = bitcast <2 x i64> %15 to <16 x i8>, !dbg !61
  %17 = tail call i32 @llvm.x86.sse2.pmovmskb.128(<16 x i8> %16) #5, !dbg !61
  %18 = icmp eq i32 %17, 255, !dbg !61
  br i1 %18, label %20, label %19, !dbg !61, !prof !63

; <label>:19:                                     ; preds = %0
  tail call void @effective_bounds_error(<2 x i64> %9, i8* %8, i64 0) #5, !dbg !61
  br label %20, !dbg !61

; <label>:20:                                     ; preds = %0, %19
  tail call fastcc void @_ZL8getValueIdET_PS0_i(double* %7), !dbg !64
  %21 = icmp eq i64 %3, 0, !dbg !66
  br i1 %21, label %24, label %22, !dbg !66

; <label>:22:                                     ; preds = %20
  %23 = inttoptr i64 %3 to i8*, !dbg !51
  tail call void @effective__ZdaPv(i8* %23), !dbg !67
  br label %24, !dbg !67

; <label>:24:                                     ; preds = %22, %20
  tail call fastcc void @_ZL8getValueIiET_PS0_i(i32* %6, i32 0), !dbg !68
  %25 = tail call <2 x i64> @effective__Znwm(i64 16, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_17* @EFFECTIVE_TYPE_60bec0f67d6986decc4d1dced3506490 to %EFFECTIVE_TYPE*)), !dbg !69
  %26 = extractelement <2 x i64> %25, i32 0, !dbg !69
  %27 = inttoptr i64 %26 to i8*, !dbg !69
  %28 = inttoptr i64 %26 to %class.MyClass*, !dbg !69
  tail call void @llvm.memset.p0i8.i64(i8* %27, i8 0, i64 16, i32 16, i1 false), !dbg !70
  %29 = getelementptr inbounds %class.MyClass, %class.MyClass* %28, i64 0, i32 0, !dbg !71
  store %struct.T* %5, %struct.T** %29, align 8, !dbg !72, !tbaa !73
  store i32 12, i32* %6, align 8, !dbg !79, !tbaa !80
  %30 = getelementptr inbounds %class.MyClass, %class.MyClass* %28, i64 0, i32 1, !dbg !82
  store double 1.230000e+01, double* %30, align 8, !dbg !83, !tbaa !84
  tail call void @llvm.trap(), !dbg !85
  unreachable
}

; Function Attrs: noinline norecurse nounwind uwtable
define internal fastcc void @_ZL8getValueIiET_PS0_i(i32*, i32) unnamed_addr #1 !dbg !86 {
  %3 = sext i32 %1 to i64, !dbg !87
  %4 = bitcast i32* %0 to i8*, !dbg !87
  %5 = tail call <2 x i64> @effective_type_check(i8* %4, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_3* @EFFECTIVE_TYPE_INT32 to %EFFECTIVE_TYPE*)), !dbg !87
  %6 = getelementptr inbounds i32, i32* %0, i64 %3, !dbg !87
  %7 = ptrtoint i32* %6 to i64, !dbg !88
  %8 = insertelement <2 x i64> undef, i64 %7, i32 0, !dbg !88
  %9 = shufflevector <2 x i64> %8, <2 x i64> undef, <2 x i32> zeroinitializer, !dbg !88
  %10 = add <2 x i64> %5, <i64 -1, i64 -4>, !dbg !88
  %11 = icmp sgt <2 x i64> %9, %10, !dbg !88
  %12 = sext <2 x i1> %11 to <2 x i64>, !dbg !88
  %13 = bitcast <2 x i64> %12 to <16 x i8>, !dbg !88
  %14 = tail call i32 @llvm.x86.sse2.pmovmskb.128(<16 x i8> %13) #5, !dbg !88
  %15 = icmp eq i32 %14, 255, !dbg !88
  br i1 %15, label %18, label %16, !dbg !88, !prof !63

; <label>:16:                                     ; preds = %2
  %17 = bitcast i32* %6 to i8*, !dbg !87
  tail call void @effective_bounds_error(<2 x i64> %5, i8* %17, i64 4) #5, !dbg !88
  br label %18, !dbg !88

; <label>:18:                                     ; preds = %2, %16
  ret void
}

; Function Attrs: noinline norecurse nounwind uwtable
define internal fastcc void @_ZL8getValueIdET_PS0_i(double*) unnamed_addr #1 !dbg !90 {
  %2 = bitcast double* %0 to i8*, !dbg !91
  %3 = tail call <2 x i64> @effective_type_check(i8* %2, %EFFECTIVE_TYPE* bitcast (%EFFECTIVE_TYPE_3* @EFFECTIVE_TYPE_FLOAT64 to %EFFECTIVE_TYPE*)), !dbg !91
  %4 = ptrtoint double* %0 to i64, !dbg !92
  %5 = insertelement <2 x i64> undef, i64 %4, i32 0, !dbg !92
  %6 = shufflevector <2 x i64> %5, <2 x i64> undef, <2 x i32> zeroinitializer, !dbg !92
  %7 = add <2 x i64> %3, <i64 -1, i64 -8>, !dbg !92
  %8 = icmp sgt <2 x i64> %6, %7, !dbg !92
  %9 = sext <2 x i1> %8 to <2 x i64>, !dbg !92
  %10 = bitcast <2 x i64> %9 to <16 x i8>, !dbg !92
  %11 = tail call i32 @llvm.x86.sse2.pmovmskb.128(<16 x i8> %10) #5, !dbg !92
  %12 = icmp eq i32 %11, 255, !dbg !92
  br i1 %12, label %14, label %13, !dbg !92, !prof !63

; <label>:13:                                     ; preds = %1
  tail call void @effective_bounds_error(<2 x i64> %3, i8* %2, i64 8) #5, !dbg !92
  br label %14, !dbg !92

; <label>:14:                                     ; preds = %1, %13
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #2

declare <2 x i64> @effective__Znam(i64, %EFFECTIVE_TYPE*) local_unnamed_addr

declare void @effective__ZdaPv(i8*) local_unnamed_addr

declare <2 x i64> @effective__Znwm(i64, %EFFECTIVE_TYPE*) local_unnamed_addr

; Function Attrs: nounwind readnone
declare <2 x i64> @effective_type_check(i8*, %EFFECTIVE_TYPE*) local_unnamed_addr #3

; Function Attrs: nounwind readnone
declare i32 @llvm.x86.sse2.pmovmskb.128(<16 x i8>) #3

declare void @effective_bounds_error(<2 x i64>, i8*, i64) local_unnamed_addr

; Function Attrs: noreturn nounwind
declare void @llvm.trap() #4

attributes #0 = { norecurse noreturn uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+popcnt,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline norecurse nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+popcnt,+sse,+sse2,+sse3,+sse4.1,+sse4.2,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind }
attributes #3 = { nounwind readnone }
attributes #4 = { noreturn nounwind }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!46}
!llvm.ident = !{!47}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 4.0.1 (tags/RELEASE_401/final)", isOptimized: true, runtimeVersion: 0, emissionKind: NoDebug, enums: !2, retainedTypes: !3)
!1 = !DIFile(filename: "Example.cpp", directory: "/opt/Research/EffSan/test")
!2 = !{}
!3 = !{!4, !6, !16, !18, !7, !19, !20, !26, !27, !5, !28, !29, !30, !31, !32, !37, !39, !40}
!4 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !5, size: 64)
!5 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!6 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !7, size: 64)
!7 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !8, size: 64)
!8 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "T", file: !9, line: 4, size: 192, elements: !10, identifier: "_ZTS1T")
!9 = !DIFile(filename: "./Example.h", directory: "/opt/Research/EffSan/test")
!10 = !{!11, !15}
!11 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !8, file: !9, line: 6, baseType: !12, size: 96)
!12 = !DICompositeType(tag: DW_TAG_array_type, baseType: !5, size: 96, elements: !13)
!13 = !{!14}
!14 = !DISubrange(count: 3)
!15 = !DIDerivedType(tag: DW_TAG_member, name: "s", scope: !8, file: !9, line: 7, baseType: !16, size: 64, offset: 128)
!16 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !17, size: 64)
!17 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!18 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!19 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !20, size: 64)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "S", file: !9, line: 9, size: 256, elements: !22, identifier: "_ZTS1S")
!22 = !{!23, !25}
!23 = !DIDerivedType(tag: DW_TAG_member, name: "f", scope: !21, file: !9, line: 11, baseType: !24, size: 32)
!24 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "t", scope: !21, file: !9, line: 12, baseType: !8, size: 192, offset: 64)
!26 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !4, size: 64)
!27 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64)
!28 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !29, size: 64)
!29 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !30, size: 64)
!30 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!31 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !32, size: 64)
!32 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !33, size: 64)
!33 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "MyClass", file: !9, line: 15, size: 128, elements: !34, identifier: "_ZTS7MyClass")
!34 = !{!35, !36}
!35 = !DIDerivedType(tag: DW_TAG_member, name: "a", scope: !33, file: !9, line: 18, baseType: !7, size: 64, flags: DIFlagPublic)
!36 = !DIDerivedType(tag: DW_TAG_member, name: "b", scope: !33, file: !9, line: 19, baseType: !30, size: 64, offset: 64, flags: DIFlagPublic)
!37 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !38, size: 64)
!38 = !DIDerivedType(tag: DW_TAG_reference_type, baseType: !30, size: 64)
!39 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !40, size: 64)
!40 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !41, size: 64)
!41 = distinct !DICompositeType(tag: DW_TAG_class_type, name: "A", file: !42, line: 4, size: 320, elements: !43, identifier: "_ZTS1A")
!42 = !DIFile(filename: "./Example_related.cpp", directory: "/opt/Research/EffSan/test")
!43 = !{!44, !45}
!44 = !DIDerivedType(tag: DW_TAG_member, name: "t1", scope: !41, file: !42, line: 6, baseType: !8, size: 192, flags: DIFlagPublic)
!45 = !DIDerivedType(tag: DW_TAG_member, name: "MyClass", scope: !41, file: !42, line: 7, baseType: !33, size: 128, offset: 192, flags: DIFlagPublic)
!46 = !{i32 2, !"Debug Info Version", i32 3}
!47 = !{!"clang version 4.0.1 (tags/RELEASE_401/final)"}
!48 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 34, type: !49, isLocal: false, isDefinition: true, scopeLine: 35, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !2)
!49 = !DISubroutineType(types: !2)
!50 = !DILocation(line: 36, column: 12, scope: !48)
!51 = !DILocation(line: 37, column: 12, scope: !48)
!52 = !DILocation(line: 39, column: 20, scope: !48)
!53 = !DILocation(line: 39, column: 14, scope: !48)
!54 = !DILocation(line: 41, column: 5, scope: !48)
!55 = !DILocation(line: 42, column: 5, scope: !48)
!56 = !DILocation(line: 43, column: 5, scope: !48)
!57 = !DILocation(line: 44, column: 5, scope: !48)
!58 = !DILocation(line: 45, column: 5, scope: !48)
!59 = !DILocation(line: 47, column: 17, scope: !48)
!60 = !DILocation(line: 49, column: 5, scope: !48)
!61 = !DILocation(line: 49, column: 5, scope: !62)
!62 = !DILexicalBlockFile(scope: !48, file: !1, discriminator: 1)
!63 = !{!"branch_weights", i32 2000000000, i32 1}
!64 = !DILocation(line: 49, column: 5, scope: !65)
!65 = !DILexicalBlockFile(scope: !48, file: !1, discriminator: 2)
!66 = !DILocation(line: 51, column: 5, scope: !48)
!67 = !DILocation(line: 51, column: 5, scope: !62)
!68 = !DILocation(line: 53, column: 5, scope: !48)
!69 = !DILocation(line: 55, column: 23, scope: !48)
!70 = !DILocation(line: 55, column: 27, scope: !48)
!71 = !DILocation(line: 56, column: 13, scope: !48)
!72 = !DILocation(line: 56, column: 15, scope: !48)
!73 = !{!74, !75, i64 0}
!74 = !{!"_ZTS7MyClass", !75, i64 0, !78, i64 8}
!75 = !{!"any pointer", !76, i64 0}
!76 = !{!"omnipotent char", !77, i64 0}
!77 = !{!"Simple C++ TBAA"}
!78 = !{!"double", !76, i64 0}
!79 = !DILocation(line: 57, column: 21, scope: !48)
!80 = !{!81, !81, i64 0}
!81 = !{!"int", !76, i64 0}
!82 = !DILocation(line: 59, column: 25, scope: !48)
!83 = !DILocation(line: 60, column: 7, scope: !48)
!84 = !{!78, !78, i64 0}
!85 = !DILocation(line: 63, column: 16, scope: !48)
!86 = distinct !DISubprogram(name: "getValue<int>", scope: !1, file: !1, line: 29, type: !49, isLocal: true, isDefinition: true, scopeLine: 30, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !2)
!87 = !DILocation(line: 31, column: 12, scope: !86)
!88 = !DILocation(line: 31, column: 12, scope: !89)
!89 = !DILexicalBlockFile(scope: !86, file: !1, discriminator: 1)
!90 = distinct !DISubprogram(name: "getValue<double>", scope: !1, file: !1, line: 29, type: !49, isLocal: true, isDefinition: true, scopeLine: 30, flags: DIFlagPrototyped, isOptimized: true, unit: !0, variables: !2)
!91 = !DILocation(line: 31, column: 12, scope: !90)
!92 = !DILocation(line: 31, column: 12, scope: !93)
!93 = !DILexicalBlockFile(scope: !90, file: !1, discriminator: 1)
