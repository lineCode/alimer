//
// Copyright (c) 2018 Amer Koleci and contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../IO/Stream.h"
#include "../Core/Log.h"

namespace Alimer
{
	Stream::Stream()
		: _position(0)
        , _size(0)
	{
	}

    Stream::Stream(size_t sizeInBytes)
        : _position(0)
        , _size(sizeInBytes)
    {
    }

    void Stream::SetName(const String& name)
    {
        _name = name;
    }

	String Stream::ReadAllText()
	{
		String content;
		uint64_t length = _size;
		if (length)
		{
			content.Resize(length);
			Read(&content[0], length);
		}

		return content;
	}

	std::vector<uint8_t> Stream::ReadBytes(size_t count)
	{
		if (!count)
			count = _size;

		std::vector<uint8_t> result(count);

		size_t read = Read(result.data(), count);
		if (read != count)
		{
			ALIMER_LOGERRORF("Failed to read complete contents of stream (amount read vs. file size: %u < %u).",
                static_cast<uint32_t>(read),
                static_cast<uint32_t>(count)
            );
			return {};
		}

		return result;
	}

    void Stream::WriteUByte(uint8_t value)
    {
        Write(&value, sizeof value);
    }

    void Stream::WriteLine(const String& value)
    {
        Write(value.CString(), value.Length());
        WriteUByte('\r');
        WriteUByte('\n');
    }
}
