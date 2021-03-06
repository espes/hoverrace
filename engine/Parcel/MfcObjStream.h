
// MfcObjStream.h
// CArchive implementation of ObjStream.
//
// Copyright (c) 2010 Michael Imamura.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which
// you have taken this file. If you can not find the license you can not use
// this file.
//
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions
// and limitations under the License.

#pragma once

#include "ObjStream.h"

#ifdef _WIN32
#	ifdef MR_ENGINE
#		define MR_DllDeclare   __declspec( dllexport )
#	else
#		define MR_DllDeclare   __declspec( dllimport )
#	endif
#else
#	define MR_DllDeclare
#endif

class CFile;

namespace HoverRace {
namespace Parcel {

class MR_DllDeclare MfcObjStream : public ObjStream {
	typedef ObjStream SUPER;
	public:
		MfcObjStream(CFile *file, const Util::OS::path_t &name, bool writing);
		virtual ~MfcObjStream() { }

		virtual void Write(const void *buf, size_t ct) { archive.Write(buf, ct); }

		virtual void WriteUInt8(MR_UInt8 i) { archive << i; }
		virtual void WriteInt16(MR_Int16 i) { archive << i; }
		virtual void WriteUInt16(MR_UInt16 i) { archive << i; }
		virtual void WriteInt32(MR_Int32 i) { archive << i; }
		virtual void WriteUInt32(MR_UInt32 i) { archive << i; }
		virtual void WriteString(const std::string &s) { CString cs(s.c_str()); archive << cs; }
		virtual void WriteCString(const CString &s) { archive << s; }

		virtual void Read(void *buf, size_t ct) { archive.Read(buf, ct); }

		virtual void ReadUInt8(MR_UInt8 &i) { archive >> i; }
		virtual void ReadInt16(MR_Int16 &i) { archive >> i; }
		virtual void ReadUInt16(MR_UInt16 &i) { archive >> i; }
		virtual void ReadInt32(MR_Int32 &i) { archive >> i; }
		virtual void ReadUInt32(MR_UInt32 &i) { archive >> i; }
		virtual void ReadString(std::string &s) { CString cs; archive >> cs; s = (const char*)cs; }
		virtual void ReadCString(CString &s) { archive >> s; }

	private:
		CArchive archive;
};

}  // namespace Parcel
}  // namespace HoverRace

#undef MR_DllDeclare
