#ifndef DEPTH_STENCIL_BUFFER_H
#define DEPTH_STENCIL_BUFFER_H

#include <cstdint>

class DepthStencilBuffer
{
public:
	DepthStencilBuffer() = delete;
	DepthStencilBuffer(int w, int h);
	DepthStencilBuffer(const DepthStencilBuffer& b);
	DepthStencilBuffer(DepthStencilBuffer&& b);
	~DepthStencilBuffer();
	
	void GetBufferSize(int* w, int* h);	
	void SetBuffer(int x, int y, uint32_t value);
	uint32_t GetBuffer(int x, int y) const;
	void Clear(uint32_t Value);
private:
	int width = 0;
	int height = 0;
	
	// 前24位Depth,后8位Stencil
	uint32_t* buffer;
};

#endif
