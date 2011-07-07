#pragma once

#include "Controller.h"
#include "ControlAction.h"
#include "../MenuScene.h"

namespace HoverRace {
namespace Client {
namespace Control {


class MenuAction : public ControlAction {
	public:
		MenuAction(std::string name, int listOrder, MenuScene *scene) : ControlAction(name, listOrder), scene(scene) { }
		virtual ~MenuAction() { }
		
		void SetMenu(MenuScene *scene);
		virtual void operator()(int value) = 0;
	protected:
		MenuScene *scene;
};

class MenuChangeSelectionAction : public MenuAction {
	public:
		MenuChangeSelectionAction(std::string name, int listOrder, MenuScene *scene, int menuIncrement) : MenuAction(name, listOrder, scene), menuIncrement(menuIncrement) { }
		
		virtual void operator()(int value);
	protected:
		int menuIncrement;
};

class MenuSelectAction : public MenuAction {
	public:
		MenuSelectAction(std::string name, int listOrder, MenuScene *scene) : MenuAction(name, listOrder, scene) { }
		
		virtual void operator()(int value);
};

}
}
}