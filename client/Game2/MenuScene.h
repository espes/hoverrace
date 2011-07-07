#pragma once

//#include "../../engine/Util/Config.h"
#include "../../engine/ObjFacTools/SpriteHandle.h"
#include "../../engine/VideoServices/Viewport2D.h"

#include "ClientApp.h"

#include "Scene.h"

namespace HoverRace {
namespace Client {


class MenuScene : public Scene
{
	typedef Scene SUPER;
	public:
		MenuScene(ClientApp *client, VideoServices::VideoBuffer *videoBuf);
		virtual ~MenuScene();
	
		void Advance(Util::OS::timestamp_t tick);
		void Render();
		
		void IncrementSelection(int amount);
		void Select();
	
	private:
		enum menuDrawMode_t {
			BASICMENU
		};
		
		enum menuState_t {
			MODESELECT,
			PLAYERSSELECT,
			TRACKSELECT
		};
		
		ClientApp *client;
		
		VideoServices::VideoBuffer *videoBuf;
		VideoServices::Viewport2D viewPort;

		ObjFac1::SpriteHandle *baseFont;
		
		menuState_t menuState;
		menuDrawMode_t drawMode;
		int menuSelection;
		
		const char* basicMenuTitle;
		std::vector<std::string> basicMenuOptions;
		
		int selectedPlayers;
		
		void StartSelectPlayers();
		void StartSelectTrack();
};


}  // namespace HoverScript
}  // namespace Client