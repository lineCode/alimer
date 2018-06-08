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

#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Matrix4x4.h"
#include "../Scene/SceneComponent.h"

namespace Alimer
{
    /// Defines a scene object class.
    class SceneObject final : public Serializable
    {
        friend class Scene;

    public:
        /// Constructor.
        SceneObject();

        /// Destructor.
        ~SceneObject();

        /// Gets the name of this object.
        std::string GetName() const { return _name; }

        /// Sets the name of this object.
        void SetName(const std::string& name);

    private:
        std::string _name;
        bool _isRoot{ false };

    private:
        DISALLOW_COPY_MOVE_AND_ASSIGN(SceneObject);
    };
}
