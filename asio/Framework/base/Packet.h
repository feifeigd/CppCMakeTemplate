#pragma once

#include "../noncopyable.hpp"

#include <stdexcept>	// out_of_range

class BasePacketWriter : private noncopyable
{
public:
	BasePacketWriter(char* buffer, size_t len, bool useBigEndian = false, bool autoMalloc = false)
		: buffer_(buffer), bigEndian_(useBigEndian), max_size_(len), autoMalloc_(autoMalloc)
	{
	}

	~BasePacketWriter() {
		if (malloc_buffer_)
		{
			delete[](malloc_buffer_);
			malloc_buffer_ = nullptr;
		}
	}

	void init() {
		pos_ = 0;
	}

	bool writeBuffer(void const* buffer, size_t len);	// 会字段增长内存

	void useBigEndian() {
		bigEndian_ = true;
	}
	void useLittleEndian() {
		bigEndian_ = false;
	}

	void savePos() {
		saved_pos_ = pos_;
	}

	[[nodiscard]]
	size_t savedPos()const {
		return saved_pos_;
	}

	// 未写的长度
	[[nodiscard]]
	size_t getLeft()const;

	bool enough(size_t len)const;

	char* begin()const {
		return buffer_;
	}
	char* currentBuffer()const {
		return buffer_ + pos_;
	}

	size_t getPos()const {
		return pos_;
	}

	size_t currentPos()const {
		return pos_;
	}
	char const* getData()const {
		return buffer_;
	}

	bool isAutoGrow()const {
		return autoMalloc_;
	}

	size_t getMaxLen()const {
		return max_size_;
	}

	void addPos(size_t diff);

	bool writeBool(bool value);

	bool writeInt8(int8_t value);
	bool writeUInt8(uint8_t value);

	bool writeInt16(int16_t value);
	bool writeUInt16(uint16_t value);

	bool writeInt32(int32_t value);
	bool writeUInt32(uint32_t value);

	bool writeInt64(int64_t value);
	bool writeUInt64(uint64_t value);

	bool writeBinary(std::string const& binary);
	bool writeBinary(char const* binary, size_t len);

	BasePacketWriter& operator<<(int16_t const& value);
	BasePacketWriter& operator<<(uint16_t const& value);
	BasePacketWriter& operator<<(int32_t const& value);
	BasePacketWriter& operator<<(uint32_t const& value);
	BasePacketWriter& operator<<(int64_t const& value);
	BasePacketWriter& operator<<(uint64_t const& value);

	BasePacketWriter& operator<<(const char* const& v);
	BasePacketWriter& operator<<(const std::string& v);
private:
	template<typename T>
	BasePacketWriter& operator<<(T const& value) {
		static_assert(not std::is_pointer_v<T>, "T must is't a pointer");
		static_assert(std::is_standard_layout_v<T> || std::is_trivial_v<T>, "T must be a pod type");

		writeBuffer(&value, sizeof(value));

		return *this;
	}

	template<typename T>
	bool write(T const& value) {
		static_assert(not std::is_pointer_v<T>, "T must is't a pointer");
		static_assert(std::is_standard_layout_v<T> || std::is_trivial_v<T>, "T must be a pod type");

		return writeBuffer(&value, sizeof(value));
	}
	void growBuffer(size_t len);
private:
	char* buffer_;
	bool bigEndian_;
	size_t pos_{};	// 当前写到的位置
	size_t saved_pos_{};	// 保存的位置
	size_t max_size_;
	const bool autoMalloc_;
	char* malloc_buffer_{};	// 如果是自动分配的内存，这里保存的是分配的内存
};


class BasePacketReader: private noncopyable
{
public:
	BasePacketReader(char const* buffer, size_t len, bool useBigEndian = false)
		: buffer_(buffer), bigEndian_(useBigEndian), size_(len)
	{
	}

	void useBigEndian() {
		bigEndian_ = true;
	}
	void useLittleEndian() {
		bigEndian_ = false;
	}

	void savePos() {
		saved_pos_ = pos_;
	}

	[[nodiscard]]
	size_t savedPos()const {
		return saved_pos_;
	}

	// 未读的长度
	[[nodiscard]]
	size_t getLeft()const;

	bool enough(size_t len)const;

	char const* begin()const {
		return buffer_;
	}
	char const* currentBuffer()const {
		return buffer_ + pos_;
	}

	void consumeAll() {
		pos_ = size_;
		savePos();
	}

	size_t currentPos()const {
		return pos_;
	}

	size_t size()const {
		return size_;
	}

	void addPos(size_t diff);

	bool readBool();

	int8_t readInt8();
	uint8_t readUInt8();

	int16_t readInt16();
	uint16_t readUInt16();

	int32_t readInt32();
	uint32_t readUInt32();

	int64_t readInt64();
	uint64_t readUInt64();
private:
	template<typename T>
	void read(T& value) {
		static_assert(std::is_same<T, typename std::remove_pointer_t<T>>::value, "T must be a normal type");
		static_assert(std::is_standard_layout_v<T> || std::is_trivial_v<T>, "T must be a pod type");

		if (pos_ + sizeof(value) > size_) {
			throw std::out_of_range("current pos is greater than max length");
		}
		value = *reinterpret_cast<T const*>(buffer_ + pos_);
		pos_ += sizeof(value);
	}
private:
	char const* buffer_;
	bool bigEndian_;
	size_t pos_{};
	size_t saved_pos_{};
	const size_t size_;
};

template<size_t SIZE>
class AutoMallocPacket : public BasePacketWriter
{
	char data_[SIZE];
public:
	AutoMallocPacket( bool useBigEndian = false)
		: BasePacketWriter(data_, SIZE, useBigEndian, false)
	{
	}
};
