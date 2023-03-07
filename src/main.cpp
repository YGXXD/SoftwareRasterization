#include "SDL.h"
#include "math/MathLibrary.h"
#include "math/Matrix4X4.h"
#include "math/Vector3.h"
#include "render/DepthStencilBuffer.h"
#include "render/RenderTargetBuffer.h"
#include "render/DrawLibrary.h"
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <array>
#include <iostream>

const int w = 1280, h = 720;
SDL_Window* window;
RenderTargetBuffer* render_buffer;
DepthStencilBuffer* depth_stencil_buffer;
Matrix4X4* CameraMatrix;

float R = 15;
float A = 0;
float B = 0;


struct RenderItem
{
	Vector3 Location;
	static GeometryInfo Box;
};
GeometryInfo RenderItem::Box = Geometry::GetBoxGeometryData();

RenderItem Item = {Vector3(0.f, 0.f, 0.f)};

void Update(float DeltaTime);
void Render(float DeltaTime);
void Test();

int main()
{	
	// Init
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("hello SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,w,h,SDL_WINDOW_SHOWN);
	render_buffer = new RenderTargetBuffer(w, h);
	depth_stencil_buffer = new DepthStencilBuffer(w, h);
	CameraMatrix = new Matrix4X4();

	bool is_quit = false;
	SDL_Event event;
	while(!is_quit)
	{
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				is_quit = true;
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_w)
				{
					A =  Math::Clamp<float>(-1.5, 1.5, A + 0.1);
				}		
				if(event.key.keysym.sym == SDLK_s)
				{
					A =  Math::Clamp<float>(-1.5, 1.5, A - 0.1);
				}		
				if(event.key.keysym.sym == SDLK_d)
				{
					B += 0.1;
				}		
				if(event.key.keysym.sym == SDLK_a)
				{
					B -= 0.1; 
				}

				if(event.key.keysym.sym == SDLK_i)
				{
					R = Math::Clamp<float>(10, 20, R + 1); 
				}
				if(event.key.keysym.sym == SDLK_k)
				{
					R = Math::Clamp<float>(10, 20, R - 1); 
				}
			}
		}
		
		static uint64_t CurrCounter = SDL_GetPerformanceCounter(), LastCounter = SDL_GetPerformanceCounter();
		CurrCounter = SDL_GetPerformanceCounter();

		uint64_t DeltaCounter = CurrCounter - LastCounter;
		float DeltaTime = (float)DeltaCounter / SDL_GetPerformanceFrequency();

		LastCounter = SDL_GetPerformanceCounter();

		std::cout << "DeltaTime:" << DeltaTime << std::endl;

		Update(DeltaTime);	
		Render(DeltaTime);
	}

	/*
	Uint32* canval_ptr = (Uint32*)canval->pixels;
	for(int y = 0; y < h; y++)
	{
		for(int x = 0;x < w; x++)
		{
			canval_ptr[x + y * w] = render_buffer->GetBuffer(x, h - 1 - y); 
			std::cout << std::hex << canval_ptr[x + y * w] << " ";
		}
		std::cout << std::endl;
	}
	*/
	delete render_buffer;
	delete depth_stencil_buffer;
	delete CameraMatrix;
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void Update(float DeltaTime) 
{
	B += DeltaTime;
	Vector3 EyePosition(R * Math::Cos(A) * Math::Cos(B), R * Math::Cos(A) * Math::Sin(B), R * Math::Sin(A));
	Vector3 FocusPosition(0, 0, 0);
	Vector3 UpDirection(0, 0, 1);

	Matrix4X4 WorldToCamera = Math::MatrixLookAt(EyePosition, FocusPosition, UpDirection);	
	Matrix4X4 CameraProj = Math::MatrixPerspectiveFov(45.f, (float)w / (float)h, 5, 15);
	*CameraMatrix = WorldToCamera * CameraProj;
}

void Render(float DeltaTime)
{
	render_buffer->Clear(32);	
	depth_stencil_buffer->Clear(0);

	Matrix4X4 WorldViewProj = Math::MatrixTranslation(Item.Location) * (*CameraMatrix);

	Draw3D::BindRenderTarget(render_buffer);
	Draw3D::BindDepthStencil(depth_stencil_buffer);
	Draw3D::BindCameraMatrix(&WorldViewProj);	
	Draw3D::BindVertex(RenderItem::Box.Vertices.data(), RenderItem::Box.Vertices.size());
	Draw3D::BindIndex(RenderItem::Box.Indices.data(), RenderItem::Box.Indices.size());
	Draw3D::DrawGraph(0, RenderItem::Box.Indices.size());


	SDL_Surface* surface = SDL_GetWindowSurface(window);	
	Uint32* pixels_ptr = (Uint32*)surface->pixels;

	for(int y = 0; y < h; y++)
	{
		for(int x = 0;x < w; x++)
		{
			uint32_t Value = render_buffer->GetBuffer(x, h - 1 - y); 
			pixels_ptr[x + y * w] = SDL_MapRGBA(surface->format, Value >> 24, Value >> 16, Value >> 8, Value); 
		}
	}
	SDL_UpdateWindowSurface(window);
}

void Test()
{
	Matrix4X4 M = {2,1,4,5,
				   1,2,1,6,
				   2,1,2,1,
				   5,3,0,2};
	std::cout << "原矩阵" << std::endl;
	std::cout << M.m11 << "," << M.m12 << "," << M.m13 << "," << M.m14 << std::endl;	
	std::cout << M.m21 << "," << M.m22 << "," << M.m23 << "," << M.m24 << std::endl;	
	std::cout << M.m31 << "," << M.m32 << "," << M.m33 << "," << M.m34 << std::endl;	
	std::cout << M.m41 << "," << M.m42 << "," << M.m43 << "," << M.m44 << std::endl;	

	std::cout << "---------------------------------" << std::endl;

	Matrix4X4 MI = Math::MatrixInverse(M); // Math::MatrixLookAt(Vector3(10,0,10), Vector3(0,0,0), Vector3(0,1,0));
	
	std::cout << "逆矩阵" << std::endl;
	std::cout << MI.m11 << "," << MI.m12 << "," << MI.m13 << "," << MI.m14 << std::endl;	
	std::cout << MI.m21 << "," << MI.m22 << "," << MI.m23 << "," << MI.m24 << std::endl;	
	std::cout << MI.m31 << "," << MI.m32 << "," << MI.m33 << "," << MI.m34 << std::endl;	
	std::cout << MI.m41 << "," << MI.m42 << "," << MI.m43 << "," << MI.m44 << std::endl;	

	std::cout << "---------------------------------" << std::endl;

	MI = MI * M;	

	std::cout << "原*逆矩阵" << std::endl;
	std::cout << MI.m11 << "," << MI.m12 << "," << MI.m13 << "," << MI.m14 << std::endl;	
	std::cout << MI.m21 << "," << MI.m22 << "," << MI.m23 << "," << MI.m24 << std::endl;	
	std::cout << MI.m31 << "," << MI.m32 << "," << MI.m33 << "," << MI.m34 << std::endl;	
	std::cout << MI.m41 << "," << MI.m42 << "," << MI.m43 << "," << MI.m44 << std::endl;	

	Pixel p1 =  {90,30,0,255,0,255};
	Pixel p2 =  {30,80,0,0,255,255};
	Pixel p3 =  {10,40,255,0,0,255};
	Draw2D::DrawTriangleByBarycentic(render_buffer, &p1, &p2, &p3);

	Pixel p4 =  {400,400,0,255,0,255};
	Pixel p5 =  {300,200,0,0,255,255};
	Pixel p6 =  {20,100,255,0,0,255};
	Draw2D::DrawTriangleByBarycentic(render_buffer, &p4, &p5, &p6);

	Pixel p7 =  {300,300,0,255,0,255};
	Pixel p8 =  {100,100,0,0,255,255};
	Pixel p9 =  {600,60,255,0,0,255};
	Draw2D::DrawTriangleByBarycentic(render_buffer, &p7, &p8, &p9);
}
