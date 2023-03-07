#include "DepthStencilBuffer.h"
#include <cstring>

DepthStencilBuffer::DepthStencilBuffer(int w, int h):width(w),height(h)
{
	buffer = new uint32_t[width * height];
}

DepthStencilBuffer::DepthStencilBuffer(const DepthStencilBuffer& b):width(b.width),height(b.height)
{
	buffer = new uint32_t[width * height];
}

DepthStencilBuffer::DepthStencilBuffer(DepthStencilBuffer&& b)
{
	width = b.width;
	height = b.height;
	buffer = b.buffer;
	b.buffer = nullptr;
}

DepthStencilBuffer::~DepthStencilBuffer()
{
	if(buffer)
	{
		delete buffer;
	}
}

void DepthStencilBuffer::GetBufferSize(int* w, int* h)
{
	*w = width;
	*h = height;
}

void DepthStencilBuffer::SetBuffer(int x, int y, uint32_t value)
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		buffer[width * y + x] = value;
}

uint32_t DepthStencilBuffer::GetBuffer(int x, int y) const
{
	if(x >= 0 && x < width && y >= 0 && y < height)
		return buffer[width * y + x];
	return 0;
}

void DepthStencilBuffer::Clear(uint32_t Value)
{
	memset(buffer, Value, sizeof(uint32_t) * width * height);
}
