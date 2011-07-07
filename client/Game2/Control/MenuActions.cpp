#include "StdAfx.h"

#include "MenuActions.h"

using namespace HoverRace::Client::Control;
using HoverRace::Client::MenuScene;

void MenuAction::SetMenu(MenuScene *scene) {
	this->scene = scene;
}

void MenuChangeSelectionAction::operator()(int value)
{
	if (value > 0) {
		scene->IncrementSelection(menuIncrement);
	}
}

void MenuSelectAction::operator()(int value)
{
	if (value > 0) {
		scene->Select();
	}
}