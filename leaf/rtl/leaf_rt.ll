; ModuleID = 'leaf_rt.ll'
@format0 = internal constant [4 x i8] c"%d\0A\00"
@format1 = internal constant [3 x i8] c"%c\00"

declare i32 @printf(i8*, ...)

define i32 @puti(i32 %arg0) {
	%format0_address = getelementptr [4 x i8]* @format0, i32 0, i32 0
	call i32 (i8*, ...)* @printf( i8* %format0_address, i32 %arg0 )
	ret i32 %arg0
}

define i32 @putc(i32 %arg0) {
	%format1_address = getelementptr [3 x i8]* @format1, i32 0, i32 0
	call i32 (i8*, ...)* @printf( i8* %format1_address, i32 %arg0 )
	ret i32 %arg0
}
