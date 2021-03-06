// HoverRender.h
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

#pragma once

#include "../ObjFacTools/FreeElementBase.h"
#include "../MainCharacter/MainCharacterRenderer.h"

namespace HoverRace {
namespace ObjFac1 {

class HoverRender : public MainCharacter::MainCharacterRenderer
{
	typedef MainCharacter::MainCharacterRenderer SUPER;
	protected:
		const ObjFacTools::ResActor *mActor0;
		const ObjFacTools::ResActor *mActor1;
		const ObjFacTools::ResActor *mActor2;
		const ObjFacTools::ResActor *mActor3;

		int mFrame;

		VideoServices::ShortSound *mLineCrossingSound;
		VideoServices::ShortSound *mStartSound;
		VideoServices::ShortSound *mFinishSound;
		VideoServices::ShortSound *mBumpSound;
		VideoServices::ShortSound *mJumpSound;
		VideoServices::ShortSound *mFireSound;
		VideoServices::ShortSound *mMisJumpSound;
		VideoServices::ShortSound *mMisFireSound;
		VideoServices::ShortSound *mOutOfCtrlSound;
		VideoServices::ContinuousSound *mMotorSound;
		VideoServices::ContinuousSound *mFrictionSound;

		const ObjFacTools::ResBitmap *mCockpitBitmap[10];
		const ObjFacTools::ResBitmap *mCockpitBitmap2[10];
		const ObjFacTools::ResBitmap *mEonCockpitBitmap[10];

	public:
		HoverRender(const Util::ObjectFromFactoryId &pId, ObjFacTools::ResourceLib *resourceLib);
		~HoverRender();

		void Render(VideoServices::Viewport3D *pDest, const MR_3DCoordinate &pPosition, MR_Angle pOrientation, BOOL pMotorOn, int pHoverId, int pModel);

		virtual VideoServices::ShortSound *GetLineCrossingSound();
		virtual VideoServices::ShortSound *GetStartSound();
		virtual VideoServices::ShortSound *GetFinishSound();
		virtual VideoServices::ShortSound *GetBumpSound();
		virtual VideoServices::ShortSound *GetJumpSound();
		virtual VideoServices::ShortSound *GetFireSound();
		virtual VideoServices::ShortSound *GetMisJumpSound();
		virtual VideoServices::ShortSound *GetMisFireSound();
		virtual VideoServices::ShortSound *GetOutOfCtrlSound();
		virtual VideoServices::ContinuousSound *GetMotorSound();
		virtual VideoServices::ContinuousSound *GetFrictionSound();

};

}  // namespace ObjFac1
}  // namespace HoverRace
