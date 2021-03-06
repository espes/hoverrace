/*
The zlib/libpng License

Copyright (c) 2005-2007 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

/*
This file has been altered from the original OIS 1.2.0 distribution in order
to integrate directly with the Linux build of HoverRace.

The original distribution may be found at:
  http://sourceforge.net/projects/wgois/
*/

#pragma once

#include "OIS/OISKeyboard.h"
#include "SDLPrereqs.h"

namespace HoverRace {
namespace Client {
namespace Control {
namespace SDL {

	class SDLKeyboard : public OIS::Keyboard
	{
	public:
		/**
		@remarks
			Constructor
		@param buffered
			True for buffered input mode
		*/
		SDLKeyboard( OIS::InputManager*, bool buffered );
		virtual ~SDLKeyboard();

		/** @copydoc Keyboard::isKeyDown */
		virtual bool isKeyDown( OIS::KeyCode key );

		/** @copydoc Keyboard::getAsString */
		virtual const std::string& getAsString( OIS::KeyCode kc );

		/** @copydoc Keyboard::copyKeyStates */
		virtual void copyKeyStates( char keys[256] );

		/** @copydoc Object::setBuffered */
		virtual void setBuffered(bool buffered);

		/** @copydoc Object::capture */
		virtual void capture();

		/** @copydoc Object::queryInterface */
		virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}

		/** @copydoc Object::_initialize */
		virtual void _initialize();

		/** @copydoc Object::setTextTranslation */
		virtual void setTextTranslation( TextTranslationMode mode );

	protected:
		typedef std::map<SDLKey, OIS::KeyCode> KeyMap;
        KeyMap mKeyMap;

		unsigned char KeyBuffer[256];
		Uint8* mSDLBuff;

		std::string mGetString;
	};

}  // namespace SDL
}  // namespace Control
}  // namespace Client
}  // namespace HoverRace
