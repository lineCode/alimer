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

#pragma once

#include "../Core/Object.h"
#include <memory>
#include <atomic>

namespace Alimer
{
	/// Asynchronous loading state of a resource.
	enum class AsyncLoadState : uint32_t
	{
		/// No async operation in progress.
		Done = 0,
		/// Queued for asynchronous loading.
		Queued = 1,
		/// In progress of calling BeginLoad() in a worker thread.
		Loading = 2,
		/// BeginLoad() succeeded. EndLoad() can be called in the main thread.
		Success = 3,
		/// BeginLoad() failed.
		Fail = 4
	};

    class Stream;

	/// Runtime resource class.
	class ALIMER_API Resource : public Object
	{
        ALIMER_OBJECT(Resource, Object);

	protected:
		/// Constructor.
		Resource();

	public:
		/// Destructor.
		virtual ~Resource() = default;

		/// Set name.
		void SetName(const String& name);

		/// Return name.
		const String& GetName() const { return _name; }

		/// Return the asynchronous loading state.
		AsyncLoadState GetAsyncLoadState() const { return _asyncLoadState; }

	protected:
        String _name;
		AsyncLoadState _asyncLoadState;
	};
}
