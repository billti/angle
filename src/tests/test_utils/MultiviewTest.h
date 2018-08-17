//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// MultiviewTest:
//   Implementation of helpers for multiview testing.
//

#ifndef ANGLE_TESTS_TESTUTILS_MULTIVIEWTEST_H_
#define ANGLE_TESTS_TESTUTILS_MULTIVIEWTEST_H_

#include "test_utils/ANGLETest.h"

namespace angle
{

// Creates a simple program that passes through two-dimensional vertices and renders green
// fragments.
GLuint CreateSimplePassthroughProgram(int numViews);

struct MultiviewImplementationParams : public PlatformParameters
{
    MultiviewImplementationParams(GLint majorVersion,
                                  GLint minorVersion,
                                  bool forceUseGeometryShaderOnD3D,
                                  const EGLPlatformParameters &eglPlatformParameters)
        : PlatformParameters(majorVersion, minorVersion, eglPlatformParameters),
          mForceUseGeometryShaderOnD3D(forceUseGeometryShaderOnD3D)
    {
    }
    bool mForceUseGeometryShaderOnD3D;
};
std::ostream &operator<<(std::ostream &os, const MultiviewImplementationParams &params);

MultiviewImplementationParams VertexShaderOpenGL(GLint majorVersion, GLint minorVersion);
MultiviewImplementationParams VertexShaderD3D11(GLint majorVersion, GLint minorVersion);
MultiviewImplementationParams GeomShaderD3D11(GLint majorVersion, GLint minorVersion);

class MultiviewTestBase : public ANGLETestBase
{
  protected:
    MultiviewTestBase(const PlatformParameters &params) : ANGLETestBase(params)
    {
        setWindowWidth(128);
        setWindowHeight(128);
        setWebGLCompatibilityEnabled(true);
    }
    virtual ~MultiviewTestBase() {}

    void MultiviewTestBaseSetUp()
    {
        ANGLETestBase::ANGLETestSetUp();

        glRequestExtensionANGLE = reinterpret_cast<PFNGLREQUESTEXTENSIONANGLEPROC>(
            eglGetProcAddress("glRequestExtensionANGLE"));
    }

    void MultiviewTestBaseTearDown() { ANGLETestBase::ANGLETestTearDown(); }

    // Requests the ANGLE_multiview extension and returns true if the operation succeeds.
    bool requestMultiviewExtension()
    {
        if (extensionRequestable("GL_ANGLE_multiview"))
        {
            glRequestExtensionANGLE("GL_ANGLE_multiview");
        }

        if (!extensionEnabled("GL_ANGLE_multiview"))
        {
            std::cout << "Test skipped due to missing GL_ANGLE_multiview." << std::endl;
            return false;
        }
        return true;
    }

    PFNGLREQUESTEXTENSIONANGLEPROC glRequestExtensionANGLE = nullptr;
};

// Base class for multiview tests that don't need specific helper functions.
class MultiviewTest : public MultiviewTestBase,
                      public ::testing::TestWithParam<MultiviewImplementationParams>
{
  protected:
    MultiviewTest() : MultiviewTestBase(GetParam()) {}
    void SetUp() override { MultiviewTestBase::MultiviewTestBaseSetUp(); }
    void TearDown() override { MultiviewTestBase::MultiviewTestBaseTearDown(); }

    void overrideWorkaroundsD3D(WorkaroundsD3D *workarounds) final;
};

}  // namespace angle

#endif  // ANGLE_TESTS_TESTUTILS_MULTIVIEWTEST_H_
