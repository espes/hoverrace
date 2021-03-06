// ResourceLibBuilder.h
//
//
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
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
//

#ifndef MR_RESOURCE_LIB_BUILDER_H
#define MR_RESOURCE_LIB_BUILDER_H

#include "../../engine/ObjFacTools/ResourceLib.h"

namespace HoverRace {
	namespace Parcel {
		class ObjStream;
	}
}

class MR_ResourceLibBuilder : public HoverRace::ObjFacTools::ResourceLib
{
	// Each module can have its own MR_BitmapLib
	public:
		MR_ResourceLibBuilder();
		~MR_ResourceLibBuilder();

		void AddBitmap(MR_ResBitmap * pBitmap);
		void AddActor(MR_ResActor * pActor);
		void AddSprite(MR_ResSprite * pSprite);
		void AddSound(MR_ResShortSound * pSound);
		void AddSound(MR_ResContinuousSound * pSound);

		bool Export(const HoverRace::Util::OS::path_t &filename);

};

#undef MR_DllDeclare
#endif
