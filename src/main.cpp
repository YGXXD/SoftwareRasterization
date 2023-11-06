#include <iostream>
#include <SDL.h>
#include "math/MathLibrary.h"
#include "math/Matrix4X4.h"
#include "math/Vector3.h"
#include "render/DepthStencilBuffer.h"
#include "render/RenderTargetBuffer.h"
#include "render/DrawLibrary.h"

struct RenderItem
{
	Vector3 Location;
	static GeometryInfo Box;
};
GeometryInfo RenderItem::Box = Geometry::GetBoxGeometryData();

const int w = 1280, h = 720;
SDL_Window* window;
RenderItem Item = {Vector3(0.f, 0.f, 0.f)};
RenderTargetBuffer* render_buffer;
DepthStencilBuffer* depth_stencil_buffer;
Matrix4X4* CameraMatrix;

float Raduis = 15;

void Update(float DeltaTime);
void Render(float DeltaTime);

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
				if(event.key.keysym.sym == SDLK_i)
				{
					Raduis = Math::Clamp<float>(10, 20, Raduis + 1); 
				}
				if(event.key.keysym.sym == SDLK_k)
				{
					Raduis = Math::Clamp<float>(10, 20, Raduis - 1); 
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

	delete render_buffer;
	delete depth_stencil_buffer;
	delete CameraMatrix;
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void Update(float DeltaTime) 
{
	static float A = 0;
	static float B = 0;

	B += DeltaTime;
	A += DeltaTime;

	float Pitch = Math::Sin(A) * 1.5707963; 
	float Yaw = B;

	Vector3 EyePosition(Raduis * Math::Cos(Pitch) * Math::Cos(Yaw), Raduis * Math::Cos(Pitch) * Math::Sin(Yaw), Raduis * Math::Sin(Pitch));
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

	Matrix4X4 WorldViewProj = Math::MatrixTransition(Item.Location) * (*CameraMatrix);

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