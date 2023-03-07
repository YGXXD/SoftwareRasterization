#pragma once
#include <cstdint>

class RenderTargetBuffer
{
public:
	RenderTargetBuffer() = delete;
	RenderTargetBuffer(int w, int h);
	RenderTargetBuffer(const RenderTargetBuffer& b);
	RenderTargetBuffer(RenderTargetBuffer&& b);
	~RenderTargetBuffer();
	
	void GetBufferSize(int* w, int* h);	
	void SetBuffer(int x, int y, uint32_t value);
	uint32_t GetBuffer(int x, int y) const;
	void Clear(uint32_t Value);
private:
	int width = 0;
	int height = 0;

	uint32_t* buffer;
};
