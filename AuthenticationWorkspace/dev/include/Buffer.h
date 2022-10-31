#pragma once
#include <iostream>
#include <string>
#include <vector>

#ifdef CHATLIB_EXPORTS
#define CHATLIB_EXPORTS_API __declspec(dllexport)
#else
#define CHATLIB_EXPORTS_API __declspec(dllimport)
#endif

CHATLIB_EXPORTS_API class Buffer
{
public:
	CHATLIB_EXPORTS_API Buffer();
	CHATLIB_EXPORTS_API Buffer(size_t size);
	CHATLIB_EXPORTS_API ~Buffer();
	std::vector<uint8_t> Data;

	// Unsigned Int serialization/deserialization
	CHATLIB_EXPORTS_API void WriteUInt32(uint32_t value);
	CHATLIB_EXPORTS_API uint32_t ReadUInt32(size_t index);

	// Signed Int serialization/deserialization
	CHATLIB_EXPORTS_API void WriteSignedInt(int value);
	CHATLIB_EXPORTS_API int ReadSignedInt(size_t index);

	// Short serialization/deserialization
	CHATLIB_EXPORTS_API void WriteShort(short value);
	CHATLIB_EXPORTS_API short ReadShort(size_t index);

	// String serialization/deserialization
	CHATLIB_EXPORTS_API void WriteString(char* value);
	CHATLIB_EXPORTS_API char* ReadString(size_t index);

	// Resize vector
	CHATLIB_EXPORTS_API void ResizeBuffer(size_t size);
};
