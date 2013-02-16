
// Study Structure Of Array
#include "stdafx.h"
#include <xmmintrin.h>

int _tmain(int argc, _TCHAR* argv[])
{
  __m128 x1= {1.0f , 3.0f , 1.0f , 2.0f };
	__m128 y1= {2.0f , 2.0f , 1.0f , 2.0f };
	__m128 z1= {3.0f , 1.0f , 1.0f , 2.0f };

	__m128 x2= {1.0f , -3.0f , -1.0f , -2.0f };
	__m128 y2= {2.0f , -2.0f , -1.0f , -2.0f };
	__m128 z2= {3.0f , -1.0f , -1.0f , -2.0f };

	__m128 tmpX = _mm_mul_ps( x1 , x2 );
	__m128 tmpY = _mm_mul_ps( y1 , y2 );
	__m128 tmpZ = _mm_mul_ps( z1 , z2 );
	__m128 tmpXY = _mm_add_ps( tmpX , tmpY );
	__m128 res = _mm_add_ps( tmpXY , tmpZ );
	printf( "%f %f %f %f" , res.m128_f32[0] , res.m128_f32[1] , res.m128_f32[2] , res.m128_f32[3] );

	return 0;
}

