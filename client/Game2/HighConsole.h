
// HighConsole.h
// Header for the on-screen debug console.
//
// Copyright (c) 2009 Michael Imamura.
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

#include <boost/thread/mutex.hpp>

#include "Console.h"

class MR_VideoBuffer;

namespace HoverRace {

namespace VideoServices {
	struct Font;
	class StaticText;
}

/**
 * An on-screen debug console.
 * @author Michael Imamura
 */
class HighConsole : public Console
{
	typedef Console SUPER;
	public:
		HighConsole();
		virtual ~HighConsole();

		virtual void Advance(Util::OS::timestamp_t tick);

	protected:
		virtual void LogInfo(const std::string &s);
		virtual void LogError(const std::string &s);

	public:
		void toggleVisible() { visible = !visible; }
		bool isVisible() const { return visible; }

	public:
		void OnChar(char c);
		void Render(MR_VideoBuffer *dest);

	private:
		bool visible;
		MR_2DViewPort *vp;

		std::string submitBuffer;
		std::string historyBuffer;
		boost::mutex submitBufferMutex;
		std::string commandLine;

		VideoServices::Font *logFont;
		VideoServices::StaticText *commandPrompt, *continuePrompt;
		VideoServices::StaticText *commandLineDisplay;

		bool cursorOn;
		Util::OS::timestamp_t cursorTick;
		VideoServices::StaticText *cursor;

		class LogLines;
		LogLines *logLines;

		static const int PADDING_TOP = 2;
		static const int PADDING_BOTTOM = 5;
		static const int PADDING_LEFT = 5;
};

}  // namespace HoverRace
