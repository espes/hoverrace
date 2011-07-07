
#include "StdAfx.h"

#include "../../engine/Util/Config.h"
#include "../../engine/VideoServices/VideoBuffer.h"
#include "../../engine/VideoServices/Sprite.h"
#include "../../engine/Model/TrackEntry.h"
#include "../../engine/Model/TrackList.h"
#include "Rulebook.h"
#include "Control/Controller.h"

#include <string>

#include "MenuScene.h"

using HoverRace::Util::Config;
using HoverRace::VideoServices::Sprite;
using HoverRace::VideoServices::Ascii2Simple;
using HoverRace::Model::TrackEntry;
using HoverRace::Model::TrackList;

namespace HoverRace {
namespace Client {

MenuScene::MenuScene(ClientApp *client, VideoServices::VideoBuffer *videoBuf) :
	SUPER(),
	client(client), videoBuf(videoBuf), menuSelection(0), selectedPlayers(1), basicMenuTitle("")
{
	Util::ObjectFromFactoryId baseFontId = { 1, 1000 };
	baseFont = (ObjFac1::SpriteHandle *) Util::DllObjectFactory::CreateObject(baseFontId);
	
	Control::InputEventController *controller = client->GetController();
	controller->ClearActionMap();
	controller->AddMenuMap(this);
	
	StartSelectPlayers();
}

MenuScene::~MenuScene()
{
	delete baseFont;
}

void MenuScene::StartSelectPlayers()
{
	basicMenuTitle = "Number of Players";
	
	basicMenuOptions.clear();
	basicMenuOptions.push_back("1");
	basicMenuOptions.push_back("2");
	basicMenuOptions.push_back("3");
	basicMenuOptions.push_back("4");
	
	menuState = PLAYERSSELECT;
	drawMode = BASICMENU;
	
	menuSelection = 0;
}

void MenuScene::StartSelectTrack()
{
	basicMenuTitle = "Track";
	
	Model::TrackList trackList;
	trackList.Reload(Config::GetInstance()->GetTrackBundle());
	
	basicMenuOptions.clear();
	BOOST_FOREACH(TrackEntry *ent, trackList) {
		basicMenuOptions.push_back(ent->name);
	}
	
	menuState = TRACKSELECT;
	drawMode = BASICMENU;
	
	menuSelection = 0;
}

void MenuScene::IncrementSelection(int amount) {
	menuSelection += amount;
	menuSelection %= basicMenuOptions.size();
	if (menuSelection < 0) menuSelection += basicMenuOptions.size();
}

void MenuScene::Select() {
	if (menuState == PLAYERSSELECT) {
		selectedPlayers = menuSelection+1;
		StartSelectTrack();
	} else if (menuState == TRACKSELECT) {
		std::string trackFile = basicMenuOptions[menuSelection]+Config::TRACK_EXT;
		
		RulebookPtr rules = boost::make_shared<Rulebook>(trackFile, 2, 0x7f);
		
		if (selectedPlayers == 1) {
			client->NewLocalSession(rules);
		} else {
			client->NewSplitSession(selectedPlayers, rules);
		}
	}
}

void MenuScene::Advance(Util::OS::timestamp_t tick)
{
}

void MenuScene::Render()
{
	
	viewPort.Setup(videoBuf, 0, 0, videoBuf->GetXRes(), videoBuf->GetYRes());
	viewPort.Clear(0);
	
	if (drawMode == BASICMENU) {
		const Sprite *font = baseFont->GetSprite();
		
		int titleScaling = 1;
		int XMargin = 10;
		int YMargin = 10;
		
		font->StrBlt(XMargin, YMargin, Ascii2Simple(basicMenuTitle), &viewPort, Sprite::eLeft, Sprite::eTop, titleScaling);
		
		int entryYMargin = YMargin+font->GetItemHeight()/titleScaling+20;
		int scaling = 2;
		int lineSpace = font->GetItemHeight() / scaling + 10;
	
		for (int i=0; i<basicMenuOptions.size(); i++) {
			if (i == menuSelection)  {
				font->StrBlt(XMargin, entryYMargin+lineSpace*i, Ascii2Simple("-"), &viewPort, Sprite::eLeft, Sprite::eTop, scaling);
			}
			font->StrBlt(XMargin+font->GetItemWidth()*2/scaling, entryYMargin+lineSpace*i,
			             Ascii2Simple(basicMenuOptions[i].c_str()), &viewPort, Sprite::eLeft, Sprite::eTop, scaling);
		}
	}
	
}


}  // namespace HoverScript
}  // namespace Client