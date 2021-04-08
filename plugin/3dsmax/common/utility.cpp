// Copyright (C) 2021 Hamed Sabri
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "utility.h"

namespace M3mf {

// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
std::string wstring_to_utf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}

Matrix3 convert(const sLib3MFTransform& xform)
{
    // row major order
    Matrix3 mat;

    MRow* m = mat.GetAddr();

    m[0][0] = xform.m_Fields[0][0];
    m[0][1] = xform.m_Fields[0][1];
    m[0][2] = xform.m_Fields[0][2];

    m[1][0] = xform.m_Fields[1][0];
    m[1][1] = xform.m_Fields[1][1];
    m[1][2] = xform.m_Fields[1][2];

    m[2][0] = xform.m_Fields[2][0];
    m[2][1] = xform.m_Fields[2][1];
    m[2][2] = xform.m_Fields[2][2];

    m[3][0] = xform.m_Fields[3][0];
    m[3][1] = xform.m_Fields[3][1];
    m[3][2] = xform.m_Fields[3][2];

    return mat;
}

// converts Matrix3 to Lib3MFTransform
sLib3MFTransform convert(const Matrix3& xform)
{
    sLib3MFTransform m;

    const MRow* mat = xform.GetAddr();

    m.m_Fields[0][0] = mat[0][0];
    m.m_Fields[0][1] = mat[0][1];
    m.m_Fields[0][2] = mat[0][2];

    m.m_Fields[1][0] = mat[1][0];
    m.m_Fields[1][1] = mat[1][1];
    m.m_Fields[1][2] = mat[1][2];

    m.m_Fields[2][0] = mat[2][0];
    m.m_Fields[2][1] = mat[2][1];
    m.m_Fields[2][2] = mat[2][2];

    m.m_Fields[3][0] = mat[3][0];
    m.m_Fields[3][1] = mat[3][1];
    m.m_Fields[3][2] = mat[3][2];

    return m;
}

} // namespace M3mf
