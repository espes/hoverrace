#pragma once

#include "../../engine/ObjFacTools/SpriteHandle.h"
#include "../../engine/VideoServices/Viewport2D.h"
#include "../../engine/VideoServices/SoundServer.h"

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
		VideoServices::ShortSound *bumpSound;
		VideoServices::ShortSound *lapSound;
		
		enum menuDrawMode_t {
			BASICMENU,
			TRACKMENU
		};
		menuDrawMode_t drawMode;
		
		enum menuState_t {
			MAINMENU,
			PLAYERSSELECT,
			TRACKSELECT,
			LAPSSELECT
		};
		menuState_t menuState;

		int menuSelection;
		const char* menuHeading;
		std::vector<std::string> menuOptions;
		
		void StartMainMenu();
		
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