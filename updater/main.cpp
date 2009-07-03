// HoverRace auto-updater
// main.cpp
//
// This file defines the command-line automatic updater for HoverRace
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define new DEBUG_NEW

using namespace std;

bool dirExists(string dir); /// check if a directory exists; auxiliary function
bool fileExists(string dir); /// check if a file exists; auxiliary function

int main(int argc, const char **argv) {

	bool createUpdate = false; // if createUpdate is false we are updating
	string targetDir = "";
	string sourceDir = "";
	string patchFile = "";

	// Windows has no getopt() therefore we must do it by hand and as a result my implementation is not the best or the
	// cleanest but it does work
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(argv[i][1] == 'c') {
				createUpdate = true;

				if(i == argc - 1) {
					fprintf(stderr, "No updated version path given!\n");
					return -1;
				} else {
					sourceDir = argv[++i];
				}
			} else if(argv[i][1] == 'h') {
				fprintf(stderr, "Usage: %s [-c /path/to/updated/version/] /path/to/hoverrace/ patchfile.bsdiff\n", argv[0]);
				fprintf(stderr, "  The -c option specifies that an update should be created\n");
				return -1;
			} else {
				// no gettext here... this should not ever be used by users
				fprintf(stderr, "Unknown option %s\n", argc[i]);
				return -1;
			}
		} else {
			// must be our working directory
			targetDir = argc[i];

			if(i == argc - 1) { // missing an argument
				fprintf(stderr, "No bsdiff file supplied!\n");
				return -1;
			} else {
				patchFile = argc[i + 1];
			}

			break;
		}
	}

	if(targetDir == "") {
		fprintf(stderr, "No directory supplied!\n");
		return -1;
	}

	// check that updated directory is valid
	if(createUpdate && !dirExists(sourceDir)) {
		fprintf(stderr, "Source directory %s does not exist!\n", sourceDir.c_str());
		return -1;
	}

	// check that directory is valid
	if(!dirExists(targetDir)) {
		fprintf(stderr, "Target directory %s does not exist!\n", targetDir.c_str());
		return -1;
	}

	// now that all the setup is done, begin the actual work
	if(createUpdate) {
		// diff source and target

	} else {
		// check that patch exists
		

		// check that HoverRace is not running
		HWND hrWin = FindWindowW("HoverRaceClass", NULL);

		if(hrWin != NULL) {
			// kill it, with user confirmation
			SendMessage(hrWin, WM_CLOSE, 0, 0);
			// if we wanted to kill instantly we would use WM_DESTROY
		}

		// now, HoverRace is closed; do the patching
		PatchHoverRace(targetDir, patchFile);
	}
}

bool dirExists(string dir) {
	if(access(dir.c_str(), 0) == 0) {
		struct stat dirStatus;
		stat(dir.c_str(), &dirStatus);
		return (dirStatus.st_mode & S_IFDIR);
	}
	
	return false;
}

bool fileExists(string file) {
	if(access(file.c_str(), 0) == 0) {
		struct stat fileStatus;
		stat(file.c_str(), &fileStatus);
		return (fileStatus.st_mode & S_IFREG);
	}

	return false;
}