#pragma once

#define ZZ_CAT_(x,y) x ## y  
#define ZT_CAT(x,y) ZZ_CAT_(x,y)  
#define ZZ_STR_(x) #x  
#define ZT_STR(x) ZZ_STR_(x)  
#define ZT_WSTR(x) ZT_CAT(L,ZT_STR(x))

#define ZZ_NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, TOTAL, ...) TOTAL 
#define ZT_NUM_ARGS(...) ZZ_NUM_ARGS_(__VA_ARGS__,12_,11_,10_,9_,8_,7_,6_,5_,4_,3_,2_,1_) 
#define ZT_DISPATCH_VA(macro, ...) ZT_CAT(macro,ZT_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__) 