// main.cpp              Fireball game entry point
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
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
//
#include "StdAfx.h"

#ifdef _WIN32
#include "GameApp.h"
#endif

#ifdef _WIN32
#	include <direct.h>
#endif

#include <iostream>

#include <curl/curl.h>

#include "../../engine/Util/Config.h"
#include "../../engine/Util/OS.h"

#ifndef _WIN32
#	include "version.h"
#endif

using boost::format;
using boost::str;

using HoverRace::Util::OS;

static bool safeMode = false;
static bool allowMultipleInstances = false;
static bool showVersion = false;

/**
 * Display a message to the user.
 * On Windows, this will pop up a 
 */
static void ShowMessage(const std::string &s)
{
#ifdef _WIN32
	MessageBox(NULL, s.c_str(), PACKAGE_NAME, MB_OK);
#else
	std::cout << s << std::endl;
#endif
}

/**
 * Process command-line options.
 * @param argc The arg count.
 * @param argv The original argument list.
 * @return @c true if successful.
 */
static bool ProcessCmdLine(int argc, char **argv)
{
	for (int i = 1; i < argc; ) {
		const char *arg = argv[i++];

		if (strcmp("-L", arg) == 0) {
			if (i < argc) {
				arg = argv[i++];
				// Set the language by updating the environment.
				// This may allow for more languages than setlocale() allows.
				OS::SetEnv("LC_ALL", arg);
			}
			else {
				ShowMessage("Expected: -L (language)");
				return false;
			}
		}
		else if (strcmp("-m", arg) == 0) {
			allowMultipleInstances = true;
		}
		else if (strcmp("-s", arg) == 0) {
			safeMode = true;
		}
		else if (strcmp("-V", arg) == 0 || strcmp("--version", arg) == 0) {
			showVersion = true;
		}
	}
	return true;
}

// Initialize (but not load) the config system.
static MR_Config *InitConfig(
#ifdef _WIN32
	const char *exePath
#endif
	)
{
#ifdef _WIN32
	// Load our own version info so we can pass it along to the config.
	long verMajor = 0;
	long verMinor = 0;
	long verPatch = 0;
	long verBuild = 0;
	bool prerelease = true;
	DWORD dummyHandle;
	DWORD verInfoSize = GetFileVersionInfoSize(exePath, &dummyHandle);
	void *verInfo = malloc(verInfoSize);
	if (GetFileVersionInfo(exePath, 0, verInfoSize, verInfo)) {
		UINT outSize;
		void *outPtr;
		if (VerQueryValue(verInfo, "\\", (LPVOID*)&outPtr, &outSize)) {
			VS_FIXEDFILEINFO *fixedInfo = (VS_FIXEDFILEINFO*)outPtr;
			verMajor = (fixedInfo->dwProductVersionMS >> 16) & 0xffff;
			verMinor = fixedInfo->dwProductVersionMS & 0xffff;
			verPatch = (fixedInfo->dwProductVersionLS >> 16) & 0xffff;
			verBuild = fixedInfo->dwProductVersionLS & 0xffff;
			prerelease = (fixedInfo->dwFileFlagsMask & VS_FF_PRERELEASE & fixedInfo->dwFileFlags) > 0;
		}
	}
	free(verInfo);

	// Format the version.
	if (verMajor == 0 && verMinor == 0 && verPatch == 0 && verBuild == 0) {
		//FIXME: Oh bother, this means the .exe was compiled without
		//       version resources.  What do we do now?
	}

	// Load the configuration, using the default OS-specific path.
	return MR_Config::Init(verMajor, verMinor, verPatch, verBuild, prerelease);

#else
	
	//FIXME: Extract prerelease flag from ver resource at build time.
	return MR_Config::Init(HR_APP_VERSION, HR_APP_VERSION_PRERELEASE);

#endif
}

// Entry point
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE pInstance, HINSTANCE pPrevInstance, LPSTR /* pCmdLine */ , int pCmdShow)
#else
int main(int argc, char** argv)
#endif
{
	// initialize return variables
	BOOL lReturnValue = TRUE;
	int lErrorCode = EXIT_SUCCESS;

#ifdef _WIN32
	char exePath[MAX_PATH];
	GetModuleFileName(NULL, exePath, MAX_PATH - 1);

	// Change the working directory to the app's directory.
	char *appPath = strdup(exePath);
	char *appDiv = strrchr(appPath, '\\');
	*appDiv = '\0';
	chdir(appPath);
	free(appPath);
#endif

	// Process command-line options.
#ifdef _WIN32
	int argc = __argc;
	char** argv = __argv;
#endif
	if (!ProcessCmdLine(argc, argv))
		return EXIT_FAILURE;

	MR_Config *cfg = InitConfig(
#ifdef _WIN32
		exePath
#endif
		);

#ifdef ENABLE_NLS
	// Gettext initialization.
	OS::SetLocale();
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

	// If --version was passed, output the version and exit.
	if (showVersion) {
		// We intentionally do not localize this string since scripts
		// may want to parse it (for whatever reason).
		ShowMessage(PACKAGE_NAME " version " + cfg->GetVersion());
		return EXIT_SUCCESS;
	}

	// Library initialization.
	curl_global_init(CURL_GLOBAL_ALL);

#ifdef _WIN32
	MR_GameApp lGame(pInstance, safeMode);

	// Allow only one instance of HoverRace; press CAPS_LOCK to bypass or
	// use the "-m" command-line option.
	if (!allowMultipleInstances) {
		GetAsyncKeyState(VK_CAPITAL);				  // Reset the function
		if(!GetAsyncKeyState(VK_CAPITAL))
			lReturnValue = lGame.IsFirstInstance();
	}

	if(lReturnValue && (pPrevInstance == NULL))
		lReturnValue = lGame.InitApplication();

	if(lReturnValue)
		lReturnValue = lGame.InitGame();

	// this is where the game actually takes control
	if(lReturnValue)
		lErrorCode = lGame.MainLoop();
#else
	std::cout << boost::format(_("HoverRace for Linux is under development!\n"
		"Please visit %s to learn how to\n"
		"contribute to this project.")) % "http://svn.igglybob.com/hoverrace/" <<
		std::endl;
#endif

	// Library cleanup.
	curl_global_cleanup();

	MR_Config::Shutdown();

	return lErrorCode;
}
