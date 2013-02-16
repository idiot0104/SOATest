// Performance.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <chrono>
#include <io.h>
#include <iostream>
#include <ppl.h>
#include <mmintrin.h>

#define OBJ_NUM  	5000000
#define OBJ_NUM_PAR 500000
#define TASK_NUM	100

__declspec(align(16)) class Vector3
{
public:
	Vector3()
	{
	}
	Vector3( float _x , float _y , float _z , float _w )
	{
		elem.m128_f32[0] = _x;
		elem.m128_f32[1] = _y;
		elem.m128_f32[2] = _z;
		elem.m128_f32[3] = _w;
	}
	inline Vector3& operator+=( const Vector3& rhs)
	{
		elem = _mm_add_ps( elem , rhs.elem );
		return *this;
	}
	inline Vector3& operator=( const Vector3& rhs)
	{
		elem = _mm_load_ps( (const float*)&rhs.elem );
		return *this;
	}
	__m128 elem;
};

__declspec(align(16)) struct ObjAOS 
{
	Vector3 acc;
	Vector3 vel;
	Vector3 pos;
	Vector3 result;
};

ObjAOS g_aos[OBJ_NUM];

struct ObjSOA
{
	Vector3 acc[OBJ_NUM];
	Vector3 vel[OBJ_NUM];
	Vector3 pos[OBJ_NUM];
	Vector3 result[OBJ_NUM];
};

ObjSOA g_soa;


void Clear()
{
	for ( int i=0; i < OBJ_NUM; ++i )
	{
		g_aos[i].acc = Vector3( 10.0f, 10.0f, 10.0f, 1.0f );
		g_aos[i].vel = Vector3( 5.0f , 5.0f , 5.0f , 1.0f );
		g_aos[i].pos = Vector3( 1.0f , 1.0f , 1.0f , 1.0f );
		g_aos[i].result = Vector3( 0.0f , 0.0f , 0.0f , 0.0f );
	}
	for ( int i=0; i < OBJ_NUM; ++i )
	{
		g_soa.acc[i] = Vector3( 10.0f, 10.0f, 10.0f, 1.0f );
		g_soa.vel[i] = Vector3( 5.0f , 5.0f , 5.0f , 1.0f );
		g_soa.pos[i] = Vector3( 1.0f , 1.0f , 1.0f , 1.0f );
		g_soa.result[i] = Vector3( 0.0f , 0.0f , 0.0f , 0.0f );
	}
}

void Case1()
{
	for ( int i=0; i < OBJ_NUM; ++i )
	{
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_aos[i].acc;
		tmp += g_aos[i].vel;
		tmp += g_aos[i].pos;
		g_aos[i].result = tmp;
	}
}

void Case2()
{
	for ( int i=0; i < OBJ_NUM; ++i )
	{
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_soa.acc[i];
		tmp += g_soa.vel[i];
		tmp += g_soa.pos[i];
		g_soa.result[i] = tmp;
	}
}



void Case3()
{
	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_aos[i].acc;
		g_aos[i].result += tmp;
	}

	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_aos[i].acc;
		g_aos[i].result += tmp;
	}

	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_aos[i].acc;
		g_aos[i].result += tmp;
	}
}

void Case4()
{
	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_soa.acc[i];
		g_soa.result[i] += tmp;
	}

	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_soa.vel[i];
		g_soa.result[i] += tmp;
	}

	for ( int i=0; i < OBJ_NUM; ++i )
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		tmp += g_soa.pos[i];
		g_soa.result[i] += tmp;
	}
}


void Case5_Par()
{
	//#pragma omp for
	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_aos[i].acc;
			tmp += g_aos[i].vel;
			tmp += g_aos[i].pos;
			g_aos[i].result += tmp;
		}
	}
	);
}

void Case6_Par()
{

	//#pragma omp for
	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_soa.acc[i];
			tmp += g_soa.vel[i];
			tmp += g_soa.pos[i];

			g_soa.result[i] = tmp;
		}
	}
	);
}

void Case7_Par()
{
	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_aos[i].acc;
			g_aos[i].result += tmp;
		}
	}
	);

	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_aos[i].vel;
			g_aos[i].result += tmp;
		}
	}
	);

	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_aos[i].pos;
			g_aos[i].result += tmp;
		}
	}
	);
}

void Case8_Par()
{
	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_soa.acc[i];
			g_soa.result[i] += tmp;
		}
	}
	);

	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_soa.vel[i];
			g_soa.result[i] += tmp;
		}
	}
	);

	//for ( int i=0; i < OBJ_NUM; ++i )
	concurrency::parallel_for( 0 , OBJ_NUM_PAR , [](int i)
	{	
		Vector3 tmp(0.0f,0.0f,0.0f,0.0f);
		for ( int j=0; j < TASK_NUM; ++j )
		{
			tmp += g_soa.pos[i];
			g_soa.result[i] += tmp;
		}
	}
	);
}


int _tmain(int argc, _TCHAR* argv[])
{
	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case1();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 1 AOS:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case2();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 2 SOA:" << milisec << std::endl;
	}
	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case3();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 3 AOS:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case4();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 4 SOA:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case5_Par();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 5_Par AOS:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case6_Par();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 6_Par SOA:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case7_Par();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 7_Par AOS:" << milisec << std::endl;
	}

	Clear();
	{
		auto time_point = std::chrono::high_resolution_clock::now();
		Case8_Par();
		auto duration = std::chrono::high_resolution_clock::now() - time_point;
		auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		std::cout << "Case 8_Par SOA:" << milisec << std::endl;
	}

	return 0;
}

//result
//Case 1 AOS:98
//Case 2 SOA:59
//Case 3 AOS:297
//Case 4 SOA:108
//Case 5_Par AOS:47
//Case 6_Par SOA:37
//Case 7_Par AOS:45
//Case 8_Par SOA:42
