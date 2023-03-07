#pragma once
#include <cstdint>
#include <vector>
#include "../math/Vector3.h"
#include "../math/Vector2.h"
#include "../math/Vector4.h"
#include "../math/Matrix4X4.h"

class RenderTargetBuffer;
class DepthStencilBuffer;

struct Pixel
{
	int x;
	int y;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct Vertex
{
	Vector3 Position;
	Vector3 Color;
};

struct GeometryInfo
{
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
};

namespace Draw2D
{
	void DrawPixel(RenderTargetBuffer* Buffer, Pixel* P);
	void DrawLineByBrasenham(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2);
	void DrawLineByMidPointError(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2);
	void DrawTriangleByBarycentic(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2, Pixel* P3);
	void DrawTriangleByScanningLine(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2, Pixel* P3);
}

namespace Draw3D
{
	void BindRenderTarget(RenderTargetBuffer* RenderTarget);
	void BindDepthStencil(DepthStencilBuffer* DepthStencil);
	void BindCameraMatrix(Matrix4X4* CameraMatrix); // 变换投影矩阵
	void BindVertex(Vertex* VertexData, uint32_t VertexNum);
	void BindIndex(uint32_t* IndexData, uint32_t IndexNum);
	void DrawGraph(uint32_t IndexStart, uint32_t IndexNum);

	class Pipeline final
	{
	private:
		struct VertexIn
		{
			Vector3 Position;
			Vector3 Color;
		};

		struct VertexOut
		{
			Vector4 ProjectPos; // NDC空间
			Vector4 ScreenColor;
		};

	public:
		friend void BindRenderTarget(RenderTargetBuffer* RenderTarget);
		friend void BindDepthStencil(DepthStencilBuffer* DepthStencil);
		friend void BindCameraMatrix(Matrix4X4* CameraMatrix);
		friend void BindVertex(Vertex* VertexData, uint32_t VertexNum);
		friend void BindIndex(uint32_t* IndexData, uint32_t IndexNum);
		friend void DrawGraph(uint32_t IndexStart, uint32_t IndexNum);

		Pipeline() = delete;
		~Pipeline() = delete;
	private:
		static VertexOut VertexShader(VertexIn* In);
		static Vector4 FragmentShader(VertexOut* Out);
		static bool DepthTest(int x, int y, uint32_t Value); 
		static void TriangleRasterize(VertexOut* Out1, VertexOut* Out2, VertexOut* Out3); //
		static RenderTargetBuffer* RenderTarget;
		static DepthStencilBuffer* DepthStencil;
		static Matrix4X4* CameraMatrix;
		static Vertex* VertexData;
		static uint32_t VertexNum;
		static uint32_t* IndexData;
		static uint32_t IndexNum;
	};
}

namespace Geometry
{
	inline GeometryInfo GetBoxGeometryData()
	{
		static GeometryInfo Box = 
		{
			{
				Vertex({Vector3(-1, -1, -1), Vector3(1, 1, 1)}),
				Vertex({Vector3(-1, 1, -1), Vector3(0, 0, 0)}),
				Vertex({Vector3(1, 1, -1), Vector3(1, 0, 0)}),
				Vertex({Vector3(1, -1, -1), Vector3(0, 1, 0)}),
				Vertex({Vector3(-1, -1, 1), Vector3(0, 0, 1)}),
				Vertex({Vector3(-1, 1, 1), Vector3(1, 1, 0)}),
				Vertex({Vector3(1, 1, 1), Vector3(0, 1, 1)}),
				Vertex({Vector3(1, -1, 1), Vector3(1, 0, 1)})
			},
			{
				0, 1, 2, 0, 2, 3,
				4, 6, 5, 4, 7, 6,
				4, 5, 1, 4, 1, 0,
				3, 2, 6, 3, 6, 7,
				1, 5, 6, 1, 6, 2,
				4, 0, 3, 4, 3, 7
			}
		};

		return Box;
	}
}
