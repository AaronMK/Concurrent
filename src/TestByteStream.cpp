#include <Resource/TestByteStream.h>

#include <algorithm>

namespace Resource
{
	TestByteStream::TestByteStream()
		: mSeekPosition(0), mMaxSeek(0)
	{
		setFlags(WRITE_ONLY | CAN_SEEK);
	}

	TestByteStream::~TestByteStream()
	{

	}

	void* TestByteStream::dataPtr(Resource::seek_t) const
	{
		return nullptr;
	}

	bool TestByteStream::readRaw(void* destination, bytesize_t byteLength)
	{
		return false;
	}

	bool TestByteStream::writeRaw(const void* data, bytesize_t byteLength)
	{
		mSeekPosition += byteLength;
		mMaxSeek = std::max(mMaxSeek, mSeekPosition);
		return true;
	}

	bool TestByteStream::seek(seek_t position)
	{
		if (position < mMaxSeek)
		{
			mSeekPosition = position;
			return true;
		}

		return false;
	}

	seek_t TestByteStream::getSeekPosition() const
	{
		return mSeekPosition;
	}

	bytesize_t TestByteStream::bytesAvailable() const
	{
		return 0;
	}

	bool TestByteStream::canRead(bytesize_t numBytes)
	{
		return false;
	}

	bool TestByteStream::canWrite(bytesize_t numBytes, bool autoExpand)
	{
		return true;
	}

	bool TestByteStream::clear()
	{
		mSeekPosition = 0;
		mMaxSeek = 0;
		return true;
	}
}