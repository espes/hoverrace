// Controller.cpp
//
// This file contains the definition for the HoverRace::Client::Controller
// class.  It contains all the code to control input devices; joysticks, 
// keyboards, and the like.

#include "StdAfx.h"

#include "Controller.h"
#include "../../engine/Util/Config.h"

using namespace HoverRace::Client::Control;
using namespace HoverRace::Util;
using namespace OIS;

/***
 * Initialize the controller for player 1, loading configuration values.
 *
 * @param mainWindow Handle to the main HR window
 */
Controller::Controller(HWND mainWindow) {
	kbd = NULL;
	mouse = NULL;
	joys = NULL;

	captureNext = false;
	captureControl = 0;
	capturePlayerId = 0;

	updated = false;

	clearControlState(); // initialize to all false

	InitInputManager(mainWindow);
	LoadControllerConfig();
}

/***
 * Clean up the Controller and everything in it.
 */
Controller::~Controller() {
	// simple cleanup... I like OIS
	InputManager::destroyInputSystem(mgr);
	delete[] joys;
}

/***
 * Save any control bindings that may have changed.
 */
void Controller::saveControls() {
	Config *cfg = Config::GetInstance();

	for(int i = 0; i < 4; i++) {
		cfg->controls[i].brake = toCfgControl(this->brake[i]);
		cfg->controls[i].fire = toCfgControl(this->fire[i]);
		cfg->controls[i].jump = toCfgControl(this->jump[i]);
		cfg->controls[i].left = toCfgControl(this->left[i]);
		cfg->controls[i].lookBack = toCfgControl(this->lookBack[i]);
		cfg->controls[i].motorOn = toCfgControl(this->motorOn[i]);
		cfg->controls[i].right = toCfgControl(this->right[i]);
		cfg->controls[i].weapon = toCfgControl(this->weapon[i]);
	}
}

/***
 * Poll the inputs for new input.  If there is input, it will call the correct handler.
 */
void Controller::poll() {
	try {
		if(kbd)
			kbd->capture();
		if(mouse)
			mouse->capture();
		if(joys) {
			for(int i = 0; i < numJoys; i++)
				joys[i]->capture();
		}
	} catch(Exception&) { // this should not happen, hence our error message
		ASSERT(FALSE);
	}
}

/***
 * Return the current control state.
 */
ControlState Controller::getState(int player) const {
	return (const ControlState) curState[player];
}

/***
 * Tell the Controller to take the next input and assign it to a control.
 *
 * @param control ID of the control (CTL_MOTOR_ON, ...)
 * @param player ID of the player; 0-3
 * @param hwnd Handle to the window we should contact when we are done
 */
void Controller::captureNextInput(int control, int player, HWND hwnd) {
	captureNext = true;
	captureControl = control;
	capturePlayerId = player;
	captureHwnd = hwnd;
}

/***
 * Stop waiting to capture the next input.
 */
void Controller::stopCapture() {
	captureNext = false;
	captureControl = 0;
	capturePlayerId = 0;
	captureHwnd = NULL;
}

/***
 * Tell the Controller to disable the given control for the given player.
 *
 * @param control ID of the control (CTL_MOTOR_ON, ...)
 * @param player ID of the player; 0-3
 */
void Controller::disableInput(int control, int player) {
	Config *cfg = Config::GetInstance();

	switch(control) {
		case CTL_MOTOR_ON:
			motorOn[player].inputType = OISUnknown;
			cfg->controls[player].motorOn.inputType = OISUnknown;
			break;
		case CTL_LEFT:
			left[player].inputType = OISUnknown;
			cfg->controls[player].left.inputType = OISUnknown;
			break;
		case CTL_RIGHT:
			right[player].inputType = OISUnknown;
			cfg->controls[player].right.inputType = OISUnknown;
			break;
		case CTL_JUMP:
			jump[player].inputType = OISUnknown;
			cfg->controls[player].jump.inputType = OISUnknown;
			break;
		case CTL_BRAKE:
			brake[player].inputType = OISUnknown;
			cfg->controls[player].brake.inputType = OISUnknown;
			break;
		case CTL_FIRE:
			fire[player].inputType = OISUnknown;
			cfg->controls[player].fire.inputType = OISUnknown;
			break;
		case CTL_WEAPON:
			weapon[player].inputType = OISUnknown;
			cfg->controls[player].weapon.inputType = OISUnknown;
			break;
		case CTL_LOOKBACK:
			lookBack[player].inputType = OISUnknown;
			cfg->controls[player].lookBack.inputType = OISUnknown;
			break;
	}
}

/***
 * Return a string representation of the input control.
 */
std::string Controller::toString(HoverRace::Util::Config::cfg_controls_t::cfg_control_t control) {
	if(control.inputType == OISKeyboard)
		return kbd->getAsString((KeyCode) control.kbdBinding);
	else if(control.inputType == OISUnknown) // cfg_control_t is set entirely to 0: disabled
		return "Disabled";
	else
		return "Something crazy"; // a mouse or joystick: TODO
}

/***
 * Initialize the InputManager object and set up all input devices.
 *
 * @param mainWindow Handle to the main HR window
 */
void Controller::InitInputManager(HWND mainWindow) {
	// collect parameters to give to init InputManager
	// just following the example here... I'm not 100% on this
	std::ostringstream wnd;
	wnd << (size_t) mainWindow;

	ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));

	// mice
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

	// this throws an exception on errors
	mgr = InputManager::createInputSystem(pl);

	// enable addons... I assume they will be useful
	mgr->enableAddOnFactory(InputManager::AddOn_All);

	// set up our input devices
	kbd = (Keyboard *) mgr->createInputObject(OISKeyboard, true);
	mouse = (Mouse *) mgr->createInputObject(OISMouse, true);

	// we can have more than one joystick
	numJoys = mgr->getNumberOfDevices(OISJoyStick);
	joys = new JoyStick *[numJoys];

	for(int i = 0; i < numJoys; i++) {
		joys[i] = NULL;

		try {
			joys[i] = (JoyStick *) mgr->createInputObject(OISJoyStick, true);
		} catch(OIS::Exception&) {
			ASSERT(false); // maybe some logging would be good here... #105
		}
	}

	// it is very nice that OIS does this for us!
	if(kbd)
		kbd->setEventCallback(this);
	else
		ASSERT(false); // this should be logged... wait until #105 is done

	if(mouse)
		kbd->setEventCallback(this);
	else
		ASSERT(false); // this should be logged... wait until #105 is done
	
	for(int i = 0; i < numJoys; i++) {
		if(joys[i])
			joys[i]->setEventCallback(this);
		else
			ASSERT(false); // this should be logged... wait until #105 is done
	}

	// now everything is set up and hopefully it will working forever!
}

/***
 * Load configuration values from HoverRace::Util::Config for a given
 * player.
 *
 * @param player Which player to load values for (0 through 3)
 */
void Controller::LoadControllerConfig() {
	Config *cfg = Config::GetInstance();

	/* now we need to load the values */
	for(int i = 0; i < 4; i++) {
		this->brake[i] = toInputControl(cfg->controls[i].brake);
		this->fire[i] = toInputControl(cfg->controls[i].fire);
		this->jump[i] = toInputControl(cfg->controls[i].jump);
		this->left[i] = toInputControl(cfg->controls[i].left);
		this->lookBack[i] = toInputControl(cfg->controls[i].lookBack);
		this->motorOn[i] = toInputControl(cfg->controls[i].motorOn);
		this->right[i] = toInputControl(cfg->controls[i].right);
		this->weapon[i] = toInputControl(cfg->controls[i].weapon);
	}
}

/***
 * Check whether or not the controls have been updated (for setting new controls)
 *
 * @return whether the controls have been updated
 */
bool Controller::controlsUpdated() {
	return updated;
	updated = false;	
}

bool Controller::keyPressed(const KeyEvent &arg) {
	if(captureNext) {
		// capture this input as a keycode
		Config *cfg = Config::GetInstance();

		InputControl *input = NULL;
		Util::Config::cfg_controls_t::cfg_control_t *cfg_input = NULL;

		switch(captureControl) {
			case CTL_MOTOR_ON:
				input = &motorOn[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].motorOn;
				break;
			case CTL_LEFT:
				input = &left[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].left;
				break;
			case CTL_RIGHT:
				input = &right[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].right;
				break;
			case CTL_JUMP:
				input = &jump[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].jump;
				break;
			case CTL_BRAKE:
				input = &brake[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].brake;
				break;
			case CTL_FIRE:
				input = &fire[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].fire;
				break;
			case CTL_WEAPON:
				input = &weapon[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].weapon;
				break;
			case CTL_LOOKBACK:
				input = &lookBack[capturePlayerId];
				cfg_input = &cfg->controls[capturePlayerId].lookBack;
				break;
		}

		// if it is a new key, set the binding
		if((*input).kbdBinding != arg.key) {
			(*input).kbdBinding = arg.key;
			(*input).inputType = OISKeyboard;
			(*cfg_input).kbdBinding = arg.key;
			(*cfg_input).inputType = OISKeyboard;
			updated = true; // so we know if we need to say
		}

		// disable capture hook
		captureNext = false;
		captureControl = 0;
		capturePlayerId = 0;
		captureHwnd = NULL;
	} else {
		for(int i = 0; i < 4; i++) {
			if(arg.key == brake[i].kbdBinding) {
				curState[i].brake = true;
			} else if(arg.key == fire[i].kbdBinding) {
				curState[i].fire = true;
			} else if(arg.key == jump[i].kbdBinding) {
				curState[i].jump = true;
			} else if(arg.key == left[i].kbdBinding) {
				curState[i].left = true;
			} else if(arg.key == lookBack[i].kbdBinding) {
				curState[i].lookBack = true;
			} else if(arg.key == motorOn[i].kbdBinding) {
				curState[i].motorOn = true;
			} else if(arg.key == right[i].kbdBinding) {
				curState[i].right = true;
			} else if(arg.key == weapon[i].kbdBinding) {
				curState[i].weapon = true;
			}
		}
	}

	return true;
}

bool Controller::keyReleased(const KeyEvent &arg) {
	if(!captureNext) {
		for(int i = 0; i < 4; i++) {
			if(arg.key == brake[i].kbdBinding) {
				curState[i].brake = false;
			} else if(arg.key == fire[i].kbdBinding) {
				curState[i].fire = false;
			} else if(arg.key == jump[i].kbdBinding) {
				curState[i].jump = false;
			} else if(arg.key == left[i].kbdBinding) {
				curState[i].left = false;
			} else if(arg.key == lookBack[i].kbdBinding) {
				curState[i].lookBack = false;
			} else if(arg.key == motorOn[i].kbdBinding) {
				curState[i].motorOn = false;
			} else if(arg.key == right[i].kbdBinding) {
				curState[i].right = false;
			} else if(arg.key == weapon[i].kbdBinding) {
				curState[i].weapon = false;
			}
		}
	}

	return true;	
}

bool Controller::mouseMoved(const MouseEvent &arg) {
	return true;
}

bool Controller::mousePressed(const MouseEvent &arg, MouseButtonID id) {
	return true;
}

bool Controller::mouseReleased(const MouseEvent &arg, MouseButtonID id) {
	return true;
}

bool Controller::buttonPressed(const JoyStickEvent &arg, int button) {
	return true;
}

bool Controller::buttonReleased(const JoyStickEvent &arg, int button) {
	return true;
}

bool Controller::axisMoved(const JoyStickEvent &arg, int axis) {
	return true;
}

bool Controller::povMoved(const JoyStickEvent &arg, int pov) {
	return true;
}

bool Controller::vector3Moved(const JoyStickEvent &arg, int index) {
	return true;
}

/***
 * Clear the control state of each player.  This is done each time Controller::poll() is
 * called.
 */
void Controller::clearControlState() {
	memset(curState, 0, sizeof(ControlState));
}

InputControl Controller::toInputControl(HoverRace::Util::Config::cfg_controls_t::cfg_control_t control) {
	InputControl ret;

	ret.axis = control.axis;
	ret.button = control.button;
	ret.inputType = control.inputType;
	ret.kbdBinding = control.kbdBinding;
	ret.pov = control.pov;
	ret.sensitivity = control.sensitivity;
	ret.slider = control.slider;

	return ret;
}

HoverRace::Util::Config::cfg_controls_t::cfg_control_t Controller::toCfgControl(InputControl control) {
	HoverRace::Util::Config::cfg_controls_t::cfg_control_t ret;

	ret.axis = control.axis;
	ret.button = control.button;
	ret.inputType = control.inputType;
	ret.kbdBinding = control.kbdBinding;
	ret.pov = control.pov;
	ret.sensitivity = control.sensitivity;
	ret.slider = control.slider;

	return ret;
}