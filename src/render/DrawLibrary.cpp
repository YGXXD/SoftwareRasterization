#include "DrawLibrary.h"
#include "RenderTargetBuffer.h"
#include "DepthStencilBuffer.h"
#include "../math/MathLibrary.h"
#include <iostream>

using namespace Math;

void Draw2D::DrawPixel(RenderTargetBuffer* Buffer, Pixel* P)
{
	uint32_t value = 0;
	value |= P->r;
	value <<= 8;
	value |= P->g;
	value <<= 8;
	value |= P->b;
	value <<= 8;
	value |= P->a;
	Buffer->SetBuffer(P->x, P->y, value);
}

void Draw2D::DrawLineByBrasenham(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2)
{
	//两个点是同一个点
	if(P1->x == P2->x && P1->y == P2->y)
	{
		return;
	}

	DrawPixel(Buffer, P1);
	DrawPixel(Buffer, P2);
	
	int dx = Abs(P1->x - P2->x), dy = Abs(P1->y - P2->y);
	if(dx >= dy)
	{
		if(P1->x > P2->x)
		{
			Pixel* temp = P1;
			P1 = P2;
			P2 = temp;
		}
		int sign = P2->y - P1->y < 0 ? -1 : 1;
		for (int x = P1->x + 1, y = P1->y, e = -dx; x < P2->x; x++)
   		{
			e += 2 * dy;
			if(e >= 0)
			{
				e -= 2 * dx;
				y += sign;
			}
			float linearValue = (float)(x - P1->x) / dx; 	
			Pixel P = {x, y, 
				Linear(P1->r, P2->r, linearValue), 
				Linear(P1->g, P2->g, linearValue), 
				Linear(P1->b, P2->b, linearValue), 
				Linear(P1->a, P2->a, linearValue), };
			DrawPixel(Buffer, &P);
		}
	}	
	else
	{
		if(P1->y > P2->y)
		{
			Pixel* temp = P1;
			P1 = P2;
			P2 = temp;
		}
		int sign = P2->x - P1->x < 0 ? -1 : 1;
		for (int y = P1->y + 1, x = P1->x, e = -dy; y < P2->y; y++)
		{
			e += 2 * dx;
			if(e >= 0)
			{
				e -= 2 * dy;
				x += sign;
			}
			float linearValue = (float)(y - P1->y) / dy; 	
			Pixel P = {x, y, 
				Linear(P1->r, P2->r, linearValue), 
				Linear(P1->g, P2->g, linearValue), 
				Linear(P1->b, P2->b, linearValue), 
				Linear(P1->a, P2->a, linearValue), };
			DrawPixel(Buffer, &P);			
		}
	}
}

void Draw2D::DrawLineByMidPointError(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2)
{
	//两个点是同一个点
	if(P1->x == P2->x && P1->y == P2->y)
	{
		return;
	}
	
	DrawPixel(Buffer, P1);
	DrawPixel(Buffer, P2);
	
	int dx = Abs(P1->x - P2->x), dy = Abs(P1->y - P2->y);
	if(dx >= dy)
	{
		if(P1->x > P2->x)
		{
			Pixel* temp = P1;
			P1 = P2;
			P2 = temp;
		}
		int sign = P2->y - P1->y < 0 ? -1 : 1;
		for (int x = P1->x + 1, y = P1->y, e = dx - 2 * dy; x < P2->x; x++)
   		{
			if(e <= 0)
			{
				e += 2 * dx - 2 * dy;
				y += sign;
			}
			else 
			{
				e -= 2 * dy;
			}
			float linearValue = (float)(x - P1->x) / dx; 	
			Pixel P = {x, y, 
				Linear(P1->r, P2->r, linearValue), 
				Linear(P1->g, P2->g, linearValue), 
				Linear(P1->b, P2->b, linearValue), 
				Linear(P1->a, P2->a, linearValue), };
			DrawPixel(Buffer, &P);
		}
	}	
	else
	{
		if(P1->y > P2->y)
		{
			Pixel* temp = P1;
			P1 = P2;
			P2 = temp;
		}
		int sign = P2->x - P1->x < 0 ? -1 : 1;
		for (int y = P1->y + 1, x = P1->x, e = dy - 2 * dx; y < P2->y; y++)
		{
			if(e <= 0)
			{
				e += 2 * dy - 2 * dx;
				x += sign;
			}
			else 
			{
				e -= 2 * dx;
			}
			float linearValue = (float)(y - P1->y) / dy; 	
			Pixel P = {x, y, 
				Linear(P1->r, P2->r, linearValue), 
				Linear(P1->g, P2->g, linearValue), 
				Linear(P1->b, P2->b, linearValue), 
				Linear(P1->a, P2->a, linearValue), };
			DrawPixel(Buffer, &P);			
		}
	}
}

void Draw2D::DrawTriangleByBarycentic(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2, Pixel* P3)
{
	int minX = Min(Min(P1->x, P2->x), P3->x);
	int minY = Min(Min(P1->y, P2->y), P3->y);
	int maxX = Max(Max(P1->x, P2->x), P3->x);
	int maxY = Max(Max(P1->y, P2->y), P3->y);
	
	//顺序P1,P2,P3
	int allArea = P1->x * P2->y + P2->x * P3->y + P3->x * P1->y - P1->y * P2->x - P2->y * P3->x - P3->y * P1->x;

	//三个点在同一条直线上
	if(allArea == 0)
	{
		return;
	}	

	for(int x = minX; x <= maxX; x++)
	{
		for(int y = minY; y <= maxY; y++)
		{
			//顺序P,P2,P3
			int area1 = x * P2->y + P2->x * P3->y + P3->x * y - y * P2->x - P2->y * P3->x - P3->y * x;
			float alpha = (float)area1 / allArea;
			//顺序P,P3,P1
			int area2 = x * P3->y + P3->x * P1->y + P1->x * y - y * P3->x - P3->y * P1->x - P1->y * x;
			float beta = (float)area2 / allArea;
			float gamma = 1 - alpha - beta;
			if(gamma >= 0 && beta >= 0 && alpha >= 0)
			{
				Pixel P = {x, y,
					(uint8_t)(alpha * P1->r + beta * P2->r + gamma * P3->r),
					(uint8_t)(alpha * P1->g + beta * P2->g + gamma * P3->g),
					(uint8_t)(alpha * P1->b + beta * P2->b + gamma * P3->b),
					(uint8_t)(alpha * P1->a + beta * P2->a + gamma * P3->a), };
				DrawPixel(Buffer, &P);
			}
		}
	}	
}

void Draw2D::DrawTriangleByScanningLine(RenderTargetBuffer* Buffer, Pixel* P1, Pixel* P2, Pixel* P3)
{
	if(P1->y > P2->y)
	{
		Pixel* temp = P1;
		P1 = P2;
		P2 = temp;
	}
	if(P2->y > P3->y)
	{
		Pixel* temp = P2;
		P2 = P3;
		P3 = temp;
	}
	if(P1->y > P2->y)
	{
		Pixel* temp = P1;
		P1 = P2;
		P2 = temp;
	}

	//此时P1为bottom，P2为middle，P3为top
	float kbt = P3->y - P1->y ? (float)(P3->x - P1->x) / (P3->y - P1->y): 0.0;
	float kbm = P2->y - P1->y ? (float)(P2->x - P1->x) / (P2->y - P1->y): 0.0;
	float kmt = P3->y - P2->y ? (float)(P3->x - P2->x) / (P3->y - P2->y): 0.0;
	
	//三个点在同一条直线上
	if(kbt == kbm && kbt == kmt && kmt == kbm)
		return;

	int y = P1->y;
	float x1 = P1->x, x2 = P1->x;	
	for(; y < P2->y; x1 += kbt, x2 += kbm, y++)
	{
		int drawX1 = Round(x1);
		int drawX2 = Round(x2);
		float linearValue1 = Abs(kbt) > 1 ? (float)(drawX1 - P1->x) / (P3->x - P1->x) : (float)(y - P1->y) / (P3->y - P1->y);
		float linearValue2 = Abs(kbm) > 1 ? (float)(drawX2 - P1->x) / (P2->x - P1->x) : (float)(y - P1->y) / (P2->y - P1->y);
		
		uint8_t r1 = Linear(P1->r, P3->r, linearValue1);
		uint8_t g1 = Linear(P1->g, P3->g, linearValue1);
		uint8_t b1 = Linear(P1->b, P3->b, linearValue1);
		uint8_t a1 = Linear(P1->a, P3->a, linearValue1);
			
		uint8_t r2 = Linear(P1->r, P2->r, linearValue2);
		uint8_t g2 = Linear(P1->g, P2->g, linearValue2);
		uint8_t b2 = Linear(P1->b, P2->b, linearValue2);
		uint8_t a2 = Linear(P1->a, P2->a, linearValue2);

		for(int x = x1 < x2 ? drawX1 : drawX2 ; x <= (x1 < x2 ? drawX2 : drawX1); x++)
		{
			float linearValue3 = float(x - drawX1) / (drawX2 - drawX1);
			Pixel P = {x, y,
				Linear(r1, r2, linearValue3),
				Linear(g1, g2, linearValue3),
				Linear(b1, b2, linearValue3),
				Linear(a1, a2, linearValue3), };
			DrawPixel(Buffer, &P);
		}
	}

	x2 = P2->x;
	for(; y <= P3->y; x1 += kbt, x2 += kmt, y++)
	{
		int drawX1 = Round(x1);
		int drawX2 = Round(x2);
		float linearValue1 = Abs(kbt) > 1 ? (float)(drawX1 - P1->x) / (P3->x - P1->x) : (float)(y - P1->y) / (P3->y - P1->y);
		float linearValue2 = Abs(kmt) > 1 ? (float)(drawX2 - P2->x) / (P3->x - P2->x) : (float)(y - P2->y) / (P3->y - P2->y);
		
		uint8_t r1 = Linear(P1->r, P3->r, linearValue1);
		uint8_t g1 = Linear(P1->g, P3->g, linearValue1);
		uint8_t b1 = Linear(P1->b, P3->b, linearValue1);
		uint8_t	a1 = Linear(P1->a, P3->a, linearValue1);
				
		uint8_t r2 = Linear(P2->r, P3->r, linearValue2);
		uint8_t g2 = Linear(P2->g, P3->g, linearValue2);
		uint8_t b2 = Linear(P2->b, P3->b, linearValue2);
		uint8_t a2 = Linear(P2->a, P3->a, linearValue2);

		for(int x = x1 < x2 ? drawX1 : drawX2 ; x <= (x1 < x2 ? drawX2 : drawX1); x++)
		{
			float linearValue3 = float(x - drawX1) / (drawX2 - drawX1);
			Pixel P = {x, y,
				Linear(r1, r2, linearValue3),
				Linear(g1, g2, linearValue3),
				Linear(b1, b2, linearValue3),
				Linear(a1, a2, linearValue3), };
			DrawPixel(Buffer, &P);
		}
	}
}

RenderTargetBuffer* Draw3D::Pipeline::RenderTarget = nullptr;
DepthStencilBuffer* Draw3D::Pipeline::DepthStencil = nullptr;
Matrix4X4* Draw3D::Pipeline::CameraMatrix = nullptr;
Vertex* Draw3D::Pipeline::VertexData = nullptr;
uint32_t Draw3D::Pipeline::VertexNum = 0;
uint32_t* Draw3D::Pipeline::IndexData = nullptr;
uint32_t Draw3D::Pipeline::IndexNum = 0;

void Draw3D::BindRenderTarget(RenderTargetBuffer* RenderTarget)
{
	Pipeline::RenderTarget = RenderTarget;
}

void Draw3D::BindDepthStencil(DepthStencilBuffer* DepthStencil)
{
	Pipeline::DepthStencil = DepthStencil;
}

void Draw3D::BindCameraMatrix(Matrix4X4* CameraMatrix)
{
	Pipeline::CameraMatrix = CameraMatrix;
}

void Draw3D::BindVertex(Vertex* VertexData, uint32_t VertexNum)
{
	Pipeline::VertexData = VertexData;
	Pipeline::VertexNum = VertexNum;
}

void Draw3D::BindIndex(uint32_t* IndexData, uint32_t IndexNum)
{
	Pipeline::IndexData = IndexData;
	Pipeline::IndexNum = IndexNum;
}

void Draw3D::DrawGraph(uint32_t IndexStart, uint32_t IndexNum)
{
	for(int i = IndexStart; i < IndexNum && i < Pipeline::IndexNum; i += 3)
	{
		bool CanTriangleRasterize = true;
		uint32_t IndexToDraw[3];

		for(int j = 0; j < 3 && i + j < Pipeline::IndexNum; j++)
		{
			uint32_t Index = Pipeline::IndexData[i + j];
			if(Index >= Pipeline::VertexNum)
			{
				CanTriangleRasterize = false;
				break;
			}
			IndexToDraw[j] = Index;
		}

		if(CanTriangleRasterize)
		{	
			Pipeline::VertexOut OutToRasterize[3];
			for(int k = 0; k < 3; k++)
			{
				Pipeline::VertexIn In;
				In = {(Pipeline::VertexData[IndexToDraw[k]]).Position, (Pipeline::VertexData[IndexToDraw[k]]).Color};
				OutToRasterize[k] = Pipeline::VertexShader(&In);
			}	
			
			Pipeline::TriangleRasterize(&OutToRasterize[0], &OutToRasterize[1], &OutToRasterize[2]);	

		}
	}	
}

Draw3D::Pipeline::VertexOut Draw3D::Pipeline::VertexShader(Draw3D::Pipeline::VertexIn* In)
{
	VertexOut Out;
	Out.ProjectPos = Vector4(In->Position, 1) * (*CameraMatrix);
	Out.ScreenColor = Vector4(In->Color, 1);

	return Out;
}	

Vector4 Draw3D::Pipeline::FragmentShader(VertexOut* Out)
{
	return Out->ScreenColor;
}

bool Draw3D::Pipeline::DepthTest(int x, int y, uint32_t Value)
{
	int w, h;
	DepthStencil->GetBufferSize(&w, &h);
	if(x < 0 || x >= w || y < 0 || y >= h)
		return false;

	if(Value <= 0)
		return false;

	uint32_t BufferValue = DepthStencil->GetBuffer(x, y);
	uint32_t DepthValue = BufferValue >> 8;
	if(Value < DepthValue || DepthValue == 0 )
	{
		DepthStencil->SetBuffer(x, y, (Value << 8) | (BufferValue) & 0x000000ff);
		return true;
	}
	return false;
}

void Draw3D::Pipeline::TriangleRasterize(VertexOut* Out1, VertexOut* Out2, VertexOut* Out3)
{
	// 经过透视除法映射到NDC空间
	Vector3 ScreenPos1(Out1->ProjectPos / Out1->ProjectPos.w); 
	Vector3 ScreenPos2(Out2->ProjectPos / Out2->ProjectPos.w);
	Vector3 ScreenPos3(Out3->ProjectPos / Out3->ProjectPos.w);

	// 计算屏幕坐标
	int w, h;
	RenderTarget->GetBufferSize(&w, &h);
	uint32_t DepthMaxValue = 0x00ffffff;
	ScreenPos1.x = (ScreenPos1.x + 1.f) * (float)w / 2;
	ScreenPos1.y = (ScreenPos1.y + 1.f) * (float)h / 2;
	ScreenPos1.z = (ScreenPos1.z) * (float)DepthMaxValue;

	ScreenPos2.x = (ScreenPos2.x + 1.f) * (float)w / 2;
	ScreenPos2.y = (ScreenPos2.y + 1.f) * (float)h / 2;
	ScreenPos2.z = (ScreenPos2.z) * (float)DepthMaxValue;

	ScreenPos3.x = (ScreenPos3.x + 1.f) * (float)w / 2;
	ScreenPos3.y = (ScreenPos3.y + 1.f) * (float)h / 2;
	ScreenPos3.z = (ScreenPos3.z) * (float)DepthMaxValue;

	Vector3 Out2_Out1(ScreenPos2.x - ScreenPos1.x, ScreenPos2.y - ScreenPos1.y, 0);		
	Vector3 Out3_Out1(ScreenPos3.x - ScreenPos1.x, ScreenPos3.y - ScreenPos1.y, 0);
	Vector3 Result = CrossProduct(Out2_Out1, Out3_Out1);
	if(Result.z >= 0)	
		return;
	
	int minX = Round(Min(Min(ScreenPos1.x, ScreenPos2.x), ScreenPos3.x));
	int minY = Round(Min(Min(ScreenPos1.y, ScreenPos2.y), ScreenPos3.y));
	int maxX = Round(Max(Max(ScreenPos1.x, ScreenPos2.x), ScreenPos3.x));
	int maxY = Round(Max(Max(ScreenPos1.y, ScreenPos2.y), ScreenPos3.y));
	
	//顺序P1,P2,P3
	float allArea = ScreenPos1.x * ScreenPos2.y + ScreenPos2.x * ScreenPos3.y + ScreenPos3.x * ScreenPos1.y - ScreenPos1.y * ScreenPos2.x - ScreenPos2.y * ScreenPos3.x - ScreenPos3.y * ScreenPos1.x;

	//三个点在同一条直线上
	if(allArea == 0)
	{
		return;
	}	

	for(int x = minX; x <= maxX; x++)
	{
		for(int y = minY; y <= maxY; y++)
		{
			//顺序P,P2,P3
			float area1 = x * ScreenPos2.y + ScreenPos2.x * ScreenPos3.y + ScreenPos3.x * y - y * ScreenPos2.x - ScreenPos2.y * ScreenPos3.x - ScreenPos3.y * x;
			float alpha = (float)area1 / allArea;
			//顺序P,P3,P1
			float area2 = x * ScreenPos3.y + ScreenPos3.x * ScreenPos1.y + ScreenPos1.x * y - y * ScreenPos3.x - ScreenPos3.y * ScreenPos1.x - ScreenPos1.y * x;
			float beta = (float)area2 / allArea;
			float gamma = 1 - alpha - beta;
			if(gamma >= 0 && beta >= 0 && alpha >= 0)
			{
				if(DepthTest(x, y, (uint32_t)(alpha * ScreenPos1.z + beta * ScreenPos2.z + gamma * ScreenPos3.z)))
				{
					VertexOut Out;
					Out.ProjectPos = alpha * Out1->ProjectPos + beta * Out2->ProjectPos + gamma * Out3->ProjectPos;
					Out.ScreenColor = alpha * Out1->ScreenColor + beta * Out2->ScreenColor + gamma * Out3->ScreenColor;
					
					Vector4 PixelColor = FragmentShader(&Out);

					Pixel P = {x, y, 
						(uint8_t)(PixelColor.x * 0xff), (uint8_t)(PixelColor.y * 0xff), 
						(uint8_t)(PixelColor.z * 0xff), (uint8_t)(PixelColor.w * 0xff)};

					Draw2D::DrawPixel(RenderTarget, &P);
				}
			}
		}
	}	
}
