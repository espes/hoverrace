
// GameScene.cpp
// Main game scene.
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

#include "StdAfx.h"

#include "../../engine/Model/Track.h"
#include "../../engine/Parcel/TrackBundle.h"
#include "../../engine/VideoServices/SoundServer.h"

#include "Control/Controller.h"
#include "HoverScript/GamePeer.h"
#include "HoverScript/SessionPeer.h"

#include "ClientSession.h"
#include "HighObserver.h"
#include "Rulebook.h"

#include "GameScene.h"

using namespace HoverRace::Client::HoverScript;
using namespace HoverRace::Util;
namespace SoundServer = HoverRace::VideoServices::SoundServer;

namespace HoverRace {
namespace Client {

GameScene::GameScene(GameDirector *director, VideoServices::VideoBuffer *videoBuf,
                     Script::Core *scripting, HoverScript::GamePeer *gamePeer,
                     RulebookPtr rules, int numPlayers) :
	SUPER(),
	frame(0), numPlayers(numPlayers), videoBuf(videoBuf),
	session(NULL), highObserver(NULL), highConsole(NULL)
{
	memset(observers, 0, sizeof(observers[0]) * MAX_OBSERVERS);

	// Create the new session
	session = new ClientSession();
	sessionPeer = boost::make_shared<SessionPeer>(scripting, session);

	// Load the selected track
	try {
		Model::TrackPtr track = Config::GetInstance()->
			GetTrackBundle()->OpenTrack(rules->GetTrackName());
		if (track.get() == NULL) throw Parcel::ObjStreamExn("Track does not exist.");
		if (!session->LoadNew(
			rules->GetTrackName().c_str(), track->GetRecordFile(),
			rules->GetLaps(), rules->GetGameOpts(), videoBuf))
		{
			throw Parcel::ObjStreamExn("Track load failed.");
		}
	}
	catch (Parcel::ObjStreamExn&) {
		Cleanup();
		throw;
	}

	session->SetSimulationTime(-6000);

	for (int i=0; i<numPlayers; i++) {
		if (!session->CreateMainCharacter(i)) {
			Cleanup();
			throw Exception("Main character creation failed");
		}
	}

	for (int i=0; i<numPlayers; i++) {
		observers[i] = Observer::New();
	}
	
	if(numPlayers == 2) {
		observers[0]->SetSplitMode(Observer::eUpperSplit);
		observers[1]->SetSplitMode(Observer::eLowerSplit);
	}
	if(numPlayers >= 3) {
		observers[0]->SetSplitMode(Observer::eUpperLeftSplit);
		observers[1]->SetSplitMode(Observer::eUpperRightSplit);
		observers[2]->SetSplitMode(Observer::eLowerLeftSplit);
	}
	if(numPlayers == 4) {
		observers[3]->SetSplitMode(Observer::eLowerRightSplit);
	}
	
	Control::InputEventController *controller = director->GetController();
	
	highObserver = new HighObserver();

	if (Config::GetInstance()->runtime.enableConsole) {
		highConsole = new HighConsole(scripting, director, gamePeer, sessionPeer);
		controller->SetConsole(highConsole);
	}
	
	//set up controls
	controller->ClearActionMap();
	MainCharacter::MainCharacter* mcs[Util::Config::MAX_PLAYERS];
	for (int i=0; i<numPlayers; i++) {
		mcs[i] = session->GetPlayer(i);
	}
	controller->AddPlayerMaps(numPlayers, mcs);
	controller->AddObserverMaps(observers, numPlayers);

}

GameScene::~GameScene()
{
	Cleanup();
}

void GameScene::Cleanup()
{
	delete highConsole;
	delete highObserver;
	delete session;
	for (int i = 0; i < numPlayers; i++) {
		if (observers[i] != NULL) {
			observers[i]->Delete();
		}
	}
}

void GameScene::Advance(Util::OS::timestamp_t tick)
{
	if (highConsole != NULL && highConsole->IsVisible()) {
		highConsole->Advance(tick);
	}

	session->Process();
}

void GameScene::Render()
{
	MR_SimulationTime simTime = session->GetSimulationTime();

	for (int i = 0; i < MAX_OBSERVERS; ++i) {
		Observer *obs = observers[i];
		if (obs != NULL) {
			obs->RenderNormalDisplay(videoBuf, session,
				session->GetPlayer(i),
				simTime, session->GetBackImage());
		}
	}
	
	if (highObserver != NULL) {
		highObserver->Render(videoBuf, session);
	}
	if (highConsole != NULL) {
		highConsole->Render(videoBuf);
	}
	
	// Sound refresh
	if(session != NULL) {
		for (int i = 0; i < MAX_OBSERVERS; ++i) {
			Observer *obs = observers[i];
			if (obs != NULL) {
				obs->PlaySounds(session->GetCurrentLevel(), session->GetPlayer(i));
			}
		}
		SoundServer::ApplyContinuousPlay();
	}
	
}

}  // namespace HoverScript
}  // namespace Client
