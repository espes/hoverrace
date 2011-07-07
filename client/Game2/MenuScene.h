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
		ClientApp *client;
		
		VideoServices::VideoBuffer *videoBuf;
		VideoServices::Viewport2D viewPort;

		ObjFac1::SpriteHandle *baseFont;
		
		enum menuDrawMode_t {
			BASICMENU,
			TRACKMENU
		};
		menuDrawMode_t drawMode;
		
		enum menuState_t {
			MODESELECT,
			PLAYERSSELECT,
			TRACKSELECT,
			LAPSSELECT
		};
		menuState_t menuState;

		int menuSelection;
		
		const char* menuHeading;
		std::vector<std::string> basicMenuOptions;
		
		int selectedPlayers;
		void StartSelectPlayers();
		
		std::string selectedTrack;
		std::vector<std::string> trackDescriptions;
		VideoServices::Sprite *curTrackMap;
		void StartSelectTrack();
		void LoadTrackMap(std::string trackName);
		
		int selectedLaps;
		void StartSelectLaps();
		
};


}  // namespace HoverScript
}  // namespace Client