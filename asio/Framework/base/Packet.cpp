#include "Packet.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "base/endian/Endian.hpp"

using namespace bsio::base;



bool BasePacketWriter::writeBool(bool value) {
	static_assert(sizeof(bool) == sizeof(int8_t));
	return write(value);
}

bool BasePacketWriter::writeInt8(int8_t value) {
	return write(value);
}

bool BasePacketWriter::writeUInt8(uint8_t value) {
	return write(value);
}

bool BasePacketWriter::writeInt16(int16_t value) {
	value = endian::hostToNetwork16(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeUInt16(uint16_t value) {
	value = endian::hostToNetwork16(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeInt32(int32_t value) {
	value = endian::hostToNetwork32(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeUInt32(uint32_t value) {
	value = endian::hostToNetwork32(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeInt64(int64_t value) {
	value = endian::hostToNetwork64(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeUInt64(uint64_t value) {
	value = endian::hostToNetwork64(value, bigEndian_);
	return write(value);
}

bool BasePacketWriter::writeBinary(std::string const& binary) {
	return writeBuffer(binary.data(), binary.size());
}

bool BasePacketWriter::writeBinary(char const* binary, size_t len) {
	return writeBuffer(binary, len);
}

void BasePacketWriter::growBuffer(size_t len)
{
	if (pos_ + len > max_size_) {
		if (autoMalloc_) {
			auto const newSize = max_size_ + len;
			auto const newBuffer = new char[newSize];
			memcpy(newBuffer, buffer_, pos_);
			if (malloc_buffer_) {
				delete[] malloc_buffer_;
				malloc_buffer_ = newBuffer;
			}
			buffer_ = malloc_buffer_ = newBuffer;
			max_size_ = newSize;
		}
		else {
			return;
		}
	}
}

BasePacketWriter& BasePacketWriter::operator<<(int16_t const& value)
{
	writeInt16(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(uint16_t const& value)
{
	writeUInt16(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(int32_t const& value)
{
	writeInt32(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(uint32_t const& value)
{
	writeUInt32(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(int64_t const& value)
{
	writeInt64(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(uint64_t const& value)
{
	writeUInt64(value);
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(const char* const& v)
{
	writeBinary(v, strlen(v));
	return *this;
}

BasePacketWriter& BasePacketWriter::operator<<(const std::string& v)
{
	writeBinary(v.data(), v.size());
	return *this;
}


bool BasePacketWriter::writeBuffer(void const* data, size_t len) {
	if (len == 0) {
		return true;
	}

	growBuffer(len);

	if (pos_ + len > max_size_)
	{
		return false;
	}

	memcpy(buffer_ + pos_, data, len);
	pos_ += len;

	return true;
}

size_t BasePacketReader::getLeft()const
{
	if (pos_ > size_)
	{
		throw std::out_of_range("current pos is greater than max length");
	}
	return size_ - pos_;
}

bool BasePacketReader::enough(size_t len) const
{
	if (pos_ > size_) {
		return false;
	}
	return (size_ - pos_) >= len;
}

void BasePacketReader::addPos(size_t diff)
{
	if (pos_ > size_) {
		throw std::out_of_range("current pos is greater than max length");
	}
	auto const tmpPos = pos_ + diff;
	if (tmpPos > size_) {
		throw std::out_of_range("current diff is greater than max length");
	}
	pos_ = tmpPos;
}

bool BasePacketReader::readBool()
{
	static_assert(sizeof(bool) == sizeof(int8_t));
	bool value;
	read(value);
	return value;
}

int8_t BasePacketReader::readInt8()
{
	int8_t value;
	read(value);
	return value;
}

uint8_t BasePacketReader::readUInt8()
{
	uint8_t value;
	read(value);
	return value;
}

int16_t BasePacketReader::readInt16()
{
	int16_t value;
	read(value);
	return endian::networkToHost16(value, bigEndian_);
}

uint16_t BasePacketReader::readUInt16()
{
	uint16_t value;
	read(value);
	return endian::networkToHost16(value, bigEndian_);
}

int32_t BasePacketReader::readInt32()
{
	int32_t value;
	read(value);
	return endian::networkToHost32(value, bigEndian_);
}

uint32_t BasePacketReader::readUInt32()
{
	uint32_t value;
	read(value);
	return endian::networkToHost32(value, bigEndian_);
}

int64_t BasePacketReader::readInt64()
{
	int64_t value;
	read(value);
	return endian::networkToHost64(value, bigEndian_);
}

uint64_t BasePacketReader::readUInt64()
{
	uint64_t value;
	read(value);
	return endian::networkToHost64(value, bigEndian_);
}
