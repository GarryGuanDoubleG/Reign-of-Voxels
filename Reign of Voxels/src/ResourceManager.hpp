#pragma once

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void * GetMemory(size_t amount1);

private:

	void *m_memory;
	size_t m_totalMemory;
};