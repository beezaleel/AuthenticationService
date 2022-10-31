#include "pch.h"
#include "Buffer.h"
#include <iostream>
#include <string>
#include <vector>


const unsigned int BYTE_SIZE = 8;

Buffer::Buffer()
	: Buffer(0) {}

Buffer::Buffer(size_t size) {
	Data.resize(size);
}

Buffer::~Buffer() {}

void Buffer::WriteUInt32(uint32_t value) {
	//ResizeBuffer(sizeof(value));
	for (int i = 0; i < sizeof(value); i++) {
		//m_buffer[i] = value >> (i * BYTE_SIZE);
		Data.push_back(value >> (i * BYTE_SIZE));
	}
}

uint32_t Buffer::ReadUInt32(size_t index) {
	uint32_t value = 0; 
	for (int i = 0; i < sizeof(value); i++) {
		value += Data[index] << (i * BYTE_SIZE);
		index++;
	}
	return value;
}

void Buffer::WriteSignedInt(int value) {
	//ResizeBuffer(sizeof(value));
	for (int i = 0; i < sizeof(value); i++) {
		//m_buffer[i] = value >> (i * BYTE_SIZE);
		Data.push_back(value >> (i * BYTE_SIZE));
	}
}

int Buffer::ReadSignedInt(size_t index) {
	int value = 0;
	for (int i = 0; i < sizeof(value); i++) {
		value += Data[index] << (i * BYTE_SIZE);
		index++;
	}
	return value;
}

void Buffer::WriteShort(short value) {
	//ResizeBuffer(sizeof(value));
	for (int i = 0; i < sizeof(value); i++) {
		//m_buffer[i] = value >> (i * BYTE_SIZE);
		Data.push_back(value >> (i * BYTE_SIZE));
	}
}
short Buffer::ReadShort(size_t index) {
	short value = 0;
	for (int i = 0; i < sizeof(value); i++) {
		value += Data[index] << (i * BYTE_SIZE);
		index++;
	}
	return value;
}

void Buffer::WriteString(char* value) {
	// Plus one to because of `\0`
	size_t charLen = strlen(value);
	for (int i = 0; i < charLen; i++) {
		Data.push_back(value[i]);
	}
}

char* Buffer::ReadString(size_t index) {
	char* value = (char*)malloc(sizeof(char)* Data.size());
	for (int i = index; i < Data.size(); i++) {
		value[i] = Data[i];
	}
	return value;
}

void Buffer::ResizeBuffer(size_t size) {
	Data.resize(size);
}
