#include "RenderTargetBuffer.h"
#include <cstring>

RenderTargetBuffer::RenderTargetBuffer(int w, int h):width(w),height(h)
{
	buffer = new uint32_t[width * height];
}

RenderTargetBuffer::RenderTargetBuffer(const RenderTargetBuffer& b):width(b.width),height(b.height)
{
	buffer = new uint32_t[width * height];
}

RenderTargetBuffer::RenderTargetBuffer(RenderTargetBuffer&& b)
{
	width = b.width;
	height = b.height;
	buffer = b.buffer;
	b.buffer = nullptr;
}

RenderTargetBuffer::~RenderTargetBuffer()
{
	if(buffer)
	{
		delete buffer;
	}
}

void RenderTargetBuffer::GetBufferSize(int* w, int* h)
{
	*w = width;
	*h = height;
}

void RenderTargetBuffer::SetBuffer(int x, int y, uint32_t value)
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		buffer[width * y + x] = value;
}

uint32_t RenderTargetBuffer::GetBuffer(int x, int y) const
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		return buffer[width * y + x];
	return 0;
}

void RenderTargetBuffer::Clear(uint32_t Value)
{
	memset(buffer, Value, sizeof(uint32_t) * width * height);
}
