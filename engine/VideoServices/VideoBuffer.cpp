// VideoBuffer.cpp
//
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

#include "VideoBuffer.h"

#include "../Exception.h"
#include "../Util/Profiler.h"
#include "../Util/Config.h"
#include "../Util/OS.h"

using HoverRace::Util::Config;
using HoverRace::Util::OS;

namespace HoverRace {
namespace VideoServices {

// Debug flag
#ifdef _DEBUG
static const BOOL gDebugMode = TRUE;
#else
static const BOOL gDebugMode = FALSE;
#endif

// Video card debuging traces

// #define _CARD_DEBUG

#ifdef _CARD_DEBUG

#include <Mmsystem.h>

static FILE *gOutputFile = NULL;

#define OPEN_LOG() \
if( gOutputFile == NULL ) \
{ \
	gOutputFile = fopen( "Video.log", "a" ); \
}

#define CLOSE_LOG() \
if( gOutputFile != NULL ) \
{ \
	fclose( gOutputFile ); \
	gOutputFile = NULL; \
}

static void PrintTimeAndLine(int pLine)
{
	if(gOutputFile != NULL) {
		fprintf(gOutputFile, "%6d %4d : ", (int) timeGetTime(), pLine);
	}
}

static int Assert(int pCondition, int pLine)
{
	if(!pCondition && (gOutputFile != NULL)) {
		fprintf(gOutputFile, "%6d %4d : ASSERT FAILED", (int) timeGetTime(), pLine);
	}

	return pCondition;
}

static void PrintLog(const char *pFormat, ...)
{
	va_list lParamList;

	va_start(lParamList, pFormat);

	if(gOutputFile != NULL) {
		vfprintf(gOutputFile, pFormat, lParamList);
		fprintf(gOutputFile, "\n");
		fflush(gOutputFile);
	}
}

int DDrawCall(int pFuncResult, int pLine)
{
	const char *lErrStr = "DDERR_<other>";

	switch (pFuncResult) {
		case DD_OK:
			lErrStr = "DD_OK";
			break;

		case DDERR_INVALIDPARAMS:
			lErrStr = "DDERR_INVALIDPARAMS";
			break;

		case DDERR_INVALIDOBJECT:
			lErrStr = "DDERR_INVALIDOBJECT";
			break;

		case DDERR_SURFACELOST:
			lErrStr = "DDERR_SURFACELOST";
			break;

		case DDERR_SURFACEBUSY:
			lErrStr = "DDERR_SURFACEBUSY";
			break;

		case DDERR_GENERIC:
			lErrStr = "DDERR_GENERIC";
			break;

		case DDERR_WASSTILLDRAWING:
			lErrStr = "DDERR_WASSTILLDRAWING";
			break;

		case DDERR_UNSUPPORTED:
			lErrStr = "DDERR_UNSUPPORTED";
			break;

		case DDERR_NOTFLIPPABLE:
			lErrStr = "DDERR_NOTFLIPPABLE";
			break;

		case DDERR_NOFLIPHW:
			lErrStr = "DDERR_NOFLIPHW";
			break;

		case DDERR_INVALIDMODE:
			lErrStr = "DDERR_INVALIDMODE";
			break;

		case DDERR_LOCKEDSURFACES:
			lErrStr = "DDERR_LOCKEDSURFACES";
			break;

			/*
			   case DDERR_WASSTILLDRAWING:
			   lErrStr = "DDERR_WASSTILLDRAWING";
			   break;
			 */

		case DDERR_NOEXCLUSIVEMODE:
			lErrStr = "DDERR_NOEXCLUSIVEMODE";
			break;

			/*
			   case DDERR_INVALIDPARAMS:
			   lErrStr = "DDERR_INVALIDPARAMS";
			   break;
			 */

		case DDERR_OUTOFVIDEOMEMORY:
			lErrStr = "DDERR_OUTOFVIDEOMEMORY";
			break;

		case DDERR_NODIRECTDRAWHW:
			lErrStr = "DDERR_NODIRECTDRAWHW";
			break;

		case DDERR_NOCOOPERATIVELEVELSET:
			lErrStr = "DDERR_NOCOOPERATIVELEVELSET";
			break;

		case DDERR_INVALIDCAPS:
			lErrStr = "DDERR_INVALIDCAPS";
			break;

		case DDERR_INVALIDPIXELFORMAT:
			lErrStr = "DDERR_INVALIDPIXELFORMAT";
			break;

		case DDERR_NOALPHAHW:
			lErrStr = "DDERR_NOALPHAHW";
			break;

			/*
			   case DDERR_NOFLIPHW:
			   lErrStr = "DDERR_NOFLIPHW";
			   break;
			 */

		case DDERR_NOZBUFFERHW:
			lErrStr = "DDERR_NOZBUFFERHW";
			break;

			/*
			   case DDERR_NOEXCLUSIVEMODE:
			   lErrStr = "DDERR_NOEXCLUSIVEMODE";
			   break;
			 */

		case DDERR_OUTOFMEMORY:
			lErrStr = "DDERR_OUTOFMEMORY";
			break;

		case DDERR_PRIMARYSURFACEALREADYEXISTS:
			lErrStr = "DDERR_PRIMARYSURFACEALREADYEXISTS";
			break;

		case DDERR_NOEMULATION:
			lErrStr = "DDERR_NOEMULATION";
			break;

		case DDERR_INCOMPATIBLEPRIMARY:
			lErrStr = "DDERR_INCOMPATIBLEPRIMARY";
			break;
	}

	if(gOutputFile != NULL) {
		fprintf(gOutputFile, "%6d %4d : %d %s\n", (int) timeGetTime(), pLine, pFuncResult, lErrStr);
		fflush(gOutputFile);
	}

	return pFuncResult;
}

#define PRINT_LOG   PrintTimeAndLine( __LINE__ );PrintLog

#define DD_CALL( pFunc )   DDrawCall( pFunc, __LINE__ )

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT( pCondition ) Assert( pCondition, __LINE__ )

#else

void PrintLog(const char *pFormat, ...);

#define OPEN_LOG()
#define CLOSE_LOG()

#ifdef _DEBUG
#define PRINT_LOG          1?NULL:
#else
#define PRINT_LOG          if( FALSE )PrintLog
#endif
#define DD_CALL( pFunc )   pFunc
#endif

static GUID zeroGuid = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

VideoBuffer::VideoBuffer(Util::OS::wnd_t pWindow, double pGamma, double pContrast, double pBrightness) :
	desktopWidth(0), desktopHeight(0),
	mBackPalette(NULL)
{
	OPEN_LOG();
	PRINT_LOG("VIDEO_BUFFER_CREATION");

#ifndef WITH_SDL
	mWindow = pWindow;
	memset(&curMonitor, 0, sizeof(curMonitor));
	mDirectDraw = NULL;
	mFrontBuffer = NULL;
	mBackBuffer = NULL;
	mPalette = NULL;
	mClipper = NULL;
	mPackedPalette = NULL;
#endif

	mZBuffer = NULL;
	mBuffer = NULL;

	mModeSettingInProgress = FALSE;
	mFullScreen = false;

	mBpp = 0;
	mNativeBpp = 0;

#ifdef WITH_SDL
	mIconMode = false;
#else
	mIconMode = IsIconic(pWindow);
#endif

	mGamma = pGamma;
	mContrast = pContrast;
	mBrightness = pBrightness;

	mSpecialWindowMode = FALSE;

#ifdef WITH_SDL
	//TODO
#else
	// Load DirectDraw.
	// As of the June 2010 update of the DirectX SDK, ddraw.lib is no longer
	// included; however, it is possible to still load DirectDraw manually.
	directDrawInst = LoadLibrary("ddraw.dll");
	if (directDrawInst == NULL) {
		throw HoverRace::Exception("Could not load DirectDraw: ddraw.dll");
	}
#endif

}

VideoBuffer::~VideoBuffer()
{
#ifdef WITH_SDL
	delete[] mZBuffer;
#else
	//   mFullScreen        = TRUE;
	//   mSpecialWindowMode = FALSE;   // force real windows resolution
	ReturnToWindowsResolution();

	if(mPalette != NULL) {
		mPalette->Release();
		mPalette = NULL;
	}

	if(mDirectDraw != NULL) {
		mDirectDraw->Release();
	}

	delete[]mBackPalette;
	delete[]mPackedPalette;

	PRINT_LOG("VIDEO_BUFFER_DESTRUCTION\n\n");
	CLOSE_LOG();

	FreeLibrary(directDrawInst);
#endif
}

/**
 * Notify the video buffer that the desktop resolution has changed.
 * This should be called immediately after the video buffer instance is created
 * and every time the desktop resolution changes.
 * @param width The desktop resolution width.
 * @param height The desktop resolution height.
 */
void VideoBuffer::NotifyDesktopModeChange(int width, int height)
{
	desktopWidth = width;
	desktopHeight = height;
}

#ifdef WITH_SDL
/**
 * Notify the video buffer that the window size has changed.
 * @param width The window client area width.
 * @param height The window client area height.
 * @param lineLen The length of each line in bytes.
 */
void VideoBuffer::NotifyWindowResChange(int width, int height, int lineLen)
{
	mXRes = width;
	mYRes = height;
	mLineLen = lineLen;

	if (mZBuffer != NULL) {
		delete[] mZBuffer;
	}
	mZBuffer = new MR_UInt16[mXRes * mYRes];
}
#endif

DWORD VideoBuffer::PackRGB(ColorPalette::paletteEntry_t &pal)
{
	return
		mRChan.Pack(ColorPalette::PalR(pal)) |
		mGChan.Pack(ColorPalette::PalG(pal)) |
		mBChan.Pack(ColorPalette::PalB(pal));
}

static inline bool guidEqual(const GUID &a, const GUID &b)
{
	if (a.Data1 != b.Data1 ||
		a.Data2 != b.Data2 ||
		a.Data3 != b.Data3) return false;
	for (int i = 0; i < 8; ++i)
		if (a.Data4[i] != b.Data4[i]) return false;
	return true;
}

#ifndef WITH_SDL
/**
 * Initialize DirectDraw.
 * This should be called whenever the monitor or fullscreen state changes.
 * @param monitor The GUID of the monitor to output to.
 *                If in windowed mode, this should be NULL so that DirectDraw
 *                can automatically decide if emulation mode is necessary.
 * @param newFullscreen @c true if switching to fullscreen mode,
 *                      @c false if switching to windowed mode.
 * @return @c true if successful.
 */
bool VideoBuffer::InitDirectDraw(GUID *monitor, bool newFullscreen)
{
	//OutputDebugString("InitDirectDraw\n");

	bool lReturnValue = true;

	if (mDirectDraw != NULL) {
		const GUID &newGuid = (monitor == NULL) ? zeroGuid : *monitor;

		// Check if the monitor or fullscreen setting has changed.
		// If so, then we need to re-initialize DirectDraw.
		if (mFullScreen != newFullscreen || !guidEqual(curMonitor, newGuid)) {
			DeleteInternalSurfaces();
			if (mPalette != NULL) {
				mPalette->Release();
				mPalette = NULL;
			}
			mDirectDraw->Release();
			mDirectDraw = NULL;
			memcpy(&curMonitor, &newGuid, sizeof(GUID));
			// mFullScreen will be updated by the caller.
		}
	}

	if(mDirectDraw == NULL) {
		typedef HRESULT (WINAPI* LPDIRECTDRAWCREATE)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
		LPDIRECTDRAWCREATE directDrawCreate = (LPDIRECTDRAWCREATE)GetProcAddress(directDrawInst, "DirectDrawCreate");
		if (directDrawCreate == NULL) {
			return false;
		}

		if(DD_CALL(directDrawCreate(monitor, &mDirectDraw, NULL)) != DD_OK) {
			ASSERT(FALSE);
			lReturnValue = false;
		}
		else {
			if(DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_NORMAL)) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = false;
			}
		}
	}

	if(lReturnValue) {
		// Keep track of the native pixel format so we can blit later.
		DDSURFACEDESC lSurfaceDesc;
		memset(&lSurfaceDesc, 0, sizeof(lSurfaceDesc));
		lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);

		if(DD_CALL(mDirectDraw->GetDisplayMode(&lSurfaceDesc)) != DD_OK) {
			lReturnValue = false;
		}
		else {
			lReturnValue = (ProcessCurrentBpp(lSurfaceDesc.ddpfPixelFormat) != FALSE);
			if(lReturnValue) {
				// We make the assumption that the desktop color depth
				// won't change while we're running.
				if(mNativeBpp == 0)
					mNativeBpp = mBpp;
			}
		}
	}

	if(mPalette == NULL) {
		// Create a palette
		CreatePalette(mGamma, mContrast, mBrightness);
	}

	return lReturnValue;
}
#endif

#ifndef WITH_SDL
BOOL VideoBuffer::ProcessCurrentBpp(const DDPIXELFORMAT & lFormat)
{
	if(lFormat.dwFlags & DDPF_PALETTEINDEXED8) {
		mBpp = 8;
		PRINT_LOG("BPP: Indexed");
		// Don't need to process the pixel format since we
		// can just copy the bits.
	}
	else {
		mBpp = lFormat.dwRGBBitCount;
		PRINT_LOG("BPP: %d", mBpp);
		if(mBpp < 8 || mBpp > 32) {
			// Interesting!  Not quite sure how to deal with this.
			PRINT_LOG("Unhandled RGB bpp: %d", mBpp);
			return FALSE;
		}
		else {
			mRChan.SetMask(lFormat.dwRBitMask);
			mGChan.SetMask(lFormat.dwGBitMask);
			mBChan.SetMask(lFormat.dwBBitMask);
		}
	}
	return TRUE;
}
#endif

#ifndef WITH_SDL
BOOL VideoBuffer::TryToSetColorMode(int colorBits)
{

	// do it only if it is safe
	if(!mSpecialWindowMode && !mFullScreen && !mModeSettingInProgress && (mDirectDraw != NULL)) {
		// Retrieve current mode info
		mSpecialModeXRes = GetSystemMetrics(SM_CXSCREEN);
		mSpecialModeYRes = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(mWindow, &mOriginalPos);

		if(DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT | DDSCL_NOWINDOWCHANGES)) == DD_OK) {
			if(DD_CALL(mDirectDraw->SetDisplayMode(mSpecialModeXRes, mSpecialModeYRes, colorBits)) == DD_OK) {
				mSpecialWindowMode = TRUE;

				// Resize window to it's original position
												  // This parameter have no effect when used here(I dont know why?? ask Bill)
				SetWindowPos(mWindow, HWND_NOTOPMOST,
					mOriginalPos.left, mOriginalPos.top, mOriginalPos.right - mOriginalPos.left, mOriginalPos.bottom - mOriginalPos.top, SWP_SHOWWINDOW /*SWP_NOACTIVATE */ );
			}
			else {
				DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_NORMAL));

				// It failled BOF
				ASSERT(FALSE);
			}
		}

	}

	return mSpecialWindowMode;
}
#endif

#ifndef WITH_SDL
void VideoBuffer::DeleteInternalSurfaces()
{
	PRINT_LOG("DeleteInternalSurfaces");

	ASSERT(mBuffer == NULL);					  // should be unlock

	if(mDirectDraw != NULL) {
		if(mBackBuffer != NULL) {
			DD_CALL(mBackBuffer->Release());
			mBackBuffer = NULL;
		}

		if(mFrontBuffer != NULL) {
			DD_CALL(mFrontBuffer->Release());
			mFrontBuffer = NULL;
		}

		if(mClipper != NULL) {
			DD_CALL(mClipper->Release());
			mClipper = NULL;
		}
	}
	delete[]mZBuffer;
	mZBuffer = NULL;
	mBuffer = NULL;
}
#endif

void VideoBuffer::CreatePalette(double pGamma, double pContrast, double pBrightness)
{
	PRINT_LOG("CreatePalette");

	ColorPalette::paletteEntry_t lPalette[256] = { 0 };

	int lCounter;

	mGamma = pGamma;
	mContrast = pContrast;
	mBrightness = pBrightness;

	if(mGamma < 0.2) {
		mGamma = 0.2;
	}

	if(mGamma > 4.0) {
		mGamma = 4.0;
	}

	if(mContrast > 1.0) {
		mContrast = 1.0;
	}

	if(mContrast < 0.3) {
		mContrast = 0.3;
	}

	if(mBrightness > 1) {
		mBrightness = 1.0;
	}

	if(mBrightness < 0.3) {
		mBrightness = 0.3;
	}

#ifndef WITH_SDL
	// Clean existing palette
	if(mPalette != NULL) {
		mPalette->Release();
		mPalette = NULL;
	}

	if (mDirectDraw == NULL) return;

	// Initialize with system colors (Ignore errors)
	HDC hdc = GetDC(NULL);
	if(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE) {
		// get the current windows colors.
		GetSystemPaletteEntries(hdc, 0, 256, lPalette);
	}
	else {
		// ASSERT( FALSE );
	}
	ReleaseDC(NULL, hdc);
#endif

	// Add our own entries
	ColorPalette::paletteEntry_t *lOurEntries = ColorPalette::GetColors(1.0 / mGamma, mContrast * mBrightness, mBrightness - (mContrast * mBrightness));

	for(lCounter = 0; lCounter < MR_BASIC_COLORS; lCounter++) {
		lPalette[MR_RESERVED_COLORS_BEGINNING + lCounter] = lOurEntries[lCounter];
#ifndef WITH_SDL
												//|*/PC_EXPLICIT; //lPalette[ 0 ].peFlags;
		lPalette[MR_RESERVED_COLORS_BEGINNING + lCounter].peFlags = PC_NOCOLLAPSE;
#endif
	}
	delete[] lOurEntries;

	if(mBackPalette != NULL) {
		for(lCounter = 0; lCounter < MR_BACK_COLORS; lCounter++) {
			lPalette[MR_RESERVED_COLORS_BEGINNING + MR_BASIC_COLORS + lCounter] =
				ColorPalette::ConvertColor(mBackPalette[lCounter * 3], mBackPalette[lCounter * 3 + 1], mBackPalette[lCounter * 3 + 2], 1.0 / mGamma, mContrast * mBrightness, mBrightness - (mContrast * mBrightness));
#ifndef WITH_SDL
												//|*/PC_EXPLICIT; //lPalette[ 0 ].peFlags;
			lPalette[MR_RESERVED_COLORS_BEGINNING + MR_BASIC_COLORS + lCounter].peFlags = PC_NOCOLLAPSE;
#endif
		}
	}

#ifndef WITH_SDL
	for(lCounter = 0; lCounter < MR_RESERVED_COLORS_BEGINNING; lCounter++) {
		lPalette[lCounter].peFlags = 0;		  //PC_NOCOLLAPSE; //lPalette[ 0 ].peFlags;
	}
#endif

	// Generate the packed palette.
	if(mBpp > 8) {
		if(mPackedPalette == NULL) {
			mPackedPalette = new DWORD[256];
		}
		for(int i = 0; i < 256; i++) {
			mPackedPalette[i] = PackRGB(lPalette[i]);
			PRINT_LOG("Palette entry %d is %08xd", i, mPackedPalette[i]);
		}
	}

	// Create the palette
#ifdef WITH_SDL
	memcpy(mPalette, lPalette, 256 * sizeof(ColorPalette::paletteEntry_t));
#else
	if(DD_CALL(mDirectDraw->CreatePalette(DDPCAPS_8BIT /*|DDPCAPS_ALLOW256 */ , lPalette, &mPalette, NULL)) != DD_OK) {
		ASSERT(FALSE);
		mPalette = NULL;
	}
#endif

	// Assign the palette to the existing buffers
	// AssignPalette();
}

void VideoBuffer::GetPaletteAttrib(double &pGamma, double &pContrast, double &pBrightness)
{
	pGamma = mGamma;
	pContrast = mContrast;
	pBrightness = mBrightness;
}

void VideoBuffer::SetBackPalette(MR_UInt8 * pPalette)
{
	delete[]mBackPalette;
	mBackPalette = pPalette;

	CreatePalette(mGamma, mContrast, mBrightness);
}

void VideoBuffer::AssignPalette()
{
	PRINT_LOG("AssignPalette");

#ifdef WITH_SDL
	SDL_Surface *surface = SDL_GetVideoSurface();
	if (surface != NULL) {
		SDL_SetPalette(surface, SDL_PHYSPAL, mPalette, 0, 256);
	}
#else
	// Currently only work in 8bit mode
	if((mFrontBuffer != NULL) && (mPalette != NULL)) {
		DD_CALL(mFrontBuffer->SetPalette(mPalette));

	}
#endif
}

#ifndef WITH_SDL
void VideoBuffer::ReturnToWindowsResolution()
{
	PRINT_LOG("ReturnToWindowsResolution");

	DeleteInternalSurfaces();

	if(mDirectDraw && mFullScreen) {
		mFullScreen = false;

		if(!mSpecialWindowMode) {
			DD_CALL(mDirectDraw->RestoreDisplayMode());

			if(DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_NORMAL)) != DD_OK) {
				ASSERT(FALSE);
			}
		}
		else {
			DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT | DDSCL_NOWINDOWCHANGES));

			if(DD_CALL(mDirectDraw->SetDisplayMode(mSpecialModeXRes, mSpecialModeYRes, 8)) == DD_OK) {
				/*
				   if( DD_CALL( mDirectDraw->SetCooperativeLevel( mWindow, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX|DDSCL_ALLOWREBOOT|DDSCL_NOWINDOWCHANGES )) != DD_OK )
				   {
				   ASSERT( FALSE );
				   }
				 */
			}
			else {
				// ASSERT( FALSE );

				mSpecialWindowMode = FALSE;

				DD_CALL(mDirectDraw->RestoreDisplayMode());

				if(DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_NORMAL)) != DD_OK) {
					ASSERT(FALSE);
				}
			}

		}

		// if( mFullScreen )
		{
			// Adjust the window position
			SetWindowLong(mWindow, GWL_EXSTYLE, mOriginalExStyle);
			SetWindowLong(mWindow, GWL_STYLE, mOriginalStyle);

			// SetForegroundWindow( mWindow );

			SetWindowPos(mWindow, HWND_NOTOPMOST,  // This parameter have no effect when used here(I dont know why?? ask Bill)
				mOriginalPos.left, mOriginalPos.top, mOriginalPos.right - mOriginalPos.left, mOriginalPos.bottom - mOriginalPos.top, SWP_SHOWWINDOW /*SWP_NOACTIVATE */ );

			// We're back in windowed mode, so use native color depth.
			mBpp = mNativeBpp;

			CreatePalette(mGamma, mContrast, mBrightness);
		}
	}
}
#endif

/**
 * Switch to windowed mode.
 * @return @c true if successful.
 */
bool VideoBuffer::SetVideoMode()
{
#ifdef WITH_SDL
	throw UnimplementedExn("VideoBuffer::SetVideoMode()");
#else
	PRINT_LOG("SetVideoMode(Window)");

	bool lReturnValue;
	DDSURFACEDESC lSurfaceDesc;

	ASSERT(!mModeSettingInProgress);

	mModeSettingInProgress = TRUE;

	lReturnValue = InitDirectDraw(NULL, false);

	if(lReturnValue) {
		ReturnToWindowsResolution();
	}
	// Retrieve the window size
	if(lReturnValue) {
		RECT lRect;

		lReturnValue = (GetClientRect(mWindow, &lRect) != FALSE);

		ASSERT(lReturnValue);

		mXRes = lRect.right;
		mYRes = lRect.bottom;
		mLineLen = mXRes;
	}

	if(lReturnValue) {
		POINT lPoint = { 0, 0 };

		lReturnValue = (ClientToScreen(mWindow, &lPoint) != FALSE);

		mX0 = lPoint.x;
		mY0 = lPoint.y;

		ASSERT(lReturnValue);
	}

	// Create a front buffer
	if(lReturnValue) {
		// Ask specificcly for a 8 bit per pixel mode
		memset(&lSurfaceDesc, 0, sizeof(lSurfaceDesc));
		lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);
		lSurfaceDesc.dwFlags = DDSD_CAPS /*|DDSD_PIXELFORMAT */ ;

		lSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if(DD_CALL(mDirectDraw->CreateSurface(&lSurfaceDesc, &mFrontBuffer, NULL)) != DD_OK) {
			// ASSERT( FALSE );
			lReturnValue = false;
		}
		else {
			// We're running windowed now, so we need to use the
			// native color depth.
			mBpp = mNativeBpp;
		}
	}

	if(lReturnValue) {
		// Create the working surface
		memset(&lSurfaceDesc, 0, sizeof(lSurfaceDesc));
		lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);
		lSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH /*|DDSD_PIXELFORMAT */ ;

		lSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		lSurfaceDesc.dwHeight = mYRes;
		lSurfaceDesc.dwWidth = mXRes;

		if(DD_CALL(mDirectDraw->CreateSurface(&lSurfaceDesc, &mBackBuffer, NULL)) != DD_OK) {
			// ASSERT( FALSE ); // Probably a bad video mode (not 8bit/pixel)
			// Retry but not is system memory this time

			lSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN /*| DDSCAPS_SYSTEMMEMORY */ ;

			if(DD_CALL(mDirectDraw->CreateSurface(&lSurfaceDesc, &mBackBuffer, NULL)) != DD_OK) {
				// ASSERT( FALSE ); // Probably a bad video mode (not 8bit/pixel)
				lReturnValue = false;
			}
		}
	}

	if(lReturnValue) {
		// Create a clipper
		if(DD_CALL(mDirectDraw->CreateClipper(0, &mClipper, NULL)) != DD_OK) {
			ASSERT(FALSE);
			lReturnValue = false;
		}
	}

	if(lReturnValue) {
		// Attatch it to the current window
		if(DD_CALL(mClipper->SetHWnd(0, mWindow)) != DD_OK) {
			ASSERT(FALSE);
			lReturnValue = false;
		}
	}

	if(lReturnValue) {
		// Attatch it to the current window
		if(DD_CALL(mFrontBuffer->SetClipper(mClipper)) != DD_OK) {
			ASSERT(FALSE);
			lReturnValue = false;
		}
	}

	if(lReturnValue) {
		// Create a local memory ZBuffer
		// We do not use DirectDrawZBuffer for now

		mZBuffer = new MR_UInt16[mXRes * mYRes];

	}

	if(!lReturnValue) {
		DeleteInternalSurfaces();
	}
	// AssignPalette();

	mModeSettingInProgress = FALSE;

	return lReturnValue;
#endif
}

/**
 * Switch to fullscreen mode.
 * @param pXRes The resolution width.
 * @param pYRes The resolution height.
 * @param monitor The GUID of the monitor to use for fullscreen.
 *                May be @c NULL to use the primary monitor.
 * @return @c true if successful.
 */
bool VideoBuffer::SetVideoMode(int pXRes, int pYRes, GUID *monitor)
{
#ifdef WITH_SDL
	throw UnimplementedExn("VideoBuffer::SetVideoMode(int,int,GUID*)");
#else
	PRINT_LOG("SetVideoMode %dx%d", pXRes, pYRes);

	// Set afull screen video mode
	HRESULT lErrorCode;
	bool lReturnValue;
	DDSURFACEDESC lSurfaceDesc;
	// DDCAPS          lDDCaps;
	Config *cfg = Config::GetInstance();

	DWORD lReqBpp = cfg->video.nativeBppFullscreen ? mNativeBpp : 8;

	ASSERT(!mModeSettingInProgress);

	mModeSettingInProgress = TRUE;

	lReturnValue = InitDirectDraw(monitor, true);

	if(lReturnValue) {
		DeleteInternalSurfaces();

		if(!mFullScreen) {
			// Save current position and style to be able to restore the current mode
			mOriginalExStyle = GetWindowLong(mWindow, GWL_EXSTYLE);
			mOriginalStyle = GetWindowLong(mWindow, GWL_STYLE);
			GetWindowRect(mWindow, &mOriginalPos);

		}
		// Make the window a non-borderwindow
		// SetWindowLong( mWindow, GWL_STYLE, mOriginalStyle & ~(WS_THICKFRAME ) );

		if(DD_CALL(mDirectDraw->SetCooperativeLevel(mWindow, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT /*|DDSCL_NOWINDOWCHANGES */ )) != DD_OK) {
			//ASSERT(FALSE);
			lReturnValue = false;
		}
	}

	// Retrieve the window size
	if(lReturnValue) {
		mXRes = pXRes;
		mYRes = pYRes;
		mLineLen = mXRes;

		mFullScreen = true;
	}

	if(lReturnValue) {
		// ASSERT( FALSE );

		if(DD_CALL(mDirectDraw->SetDisplayMode(pXRes, pYRes, lReqBpp)) != DD_OK) {
			lReturnValue = false;
		//	ASSERT(FALSE);
		}
	}

	if(lReturnValue) {
		// Resize to full screen
		ShowWindow(mWindow, SW_MAXIMIZE);

	}

	// Create a front buffer
	if(lReturnValue) {
		memset(&lSurfaceDesc, 0, sizeof(lSurfaceDesc));
		lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);

		lSurfaceDesc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		lSurfaceDesc.dwBackBufferCount = 1;
		lSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;

		if((lErrorCode = DD_CALL(mDirectDraw->CreateSurface(&lSurfaceDesc, &mFrontBuffer, NULL))) != DD_OK) {
			ASSERT(FALSE);

			lSurfaceDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX /*|DDSCAPS_SYSTEMMEMORY */ ;

			if((lErrorCode = DD_CALL(mDirectDraw->CreateSurface(&lSurfaceDesc, &mFrontBuffer, NULL))) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = false;
			}
		}

		if(lReturnValue) {
			// Create (retrieve already created) the working surface
			DDSCAPS lDDSCaps;

			lDDSCaps.dwCaps = DDSCAPS_BACKBUFFER;

			if(DD_CALL(mFrontBuffer->GetAttachedSurface(&lDDSCaps, &mBackBuffer)) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = false;
			}
		}

		if(lReturnValue) {
			mBpp = lReqBpp;
		}
	}

	if(lReturnValue) {
		// Create a local memory ZBuffer
		// We do not use DirectDrawZBuffer for now
		mZBuffer = new MR_UInt16[mXRes * mYRes];
	}

	if(!lReturnValue) {
		ReturnToWindowsResolution();
	}

	// AssignPalette();

	mModeSettingInProgress = FALSE;

	return lReturnValue;
#endif
}

BOOL VideoBuffer::IsWindowMode() const
{
	return !mFullScreen;
}

BOOL VideoBuffer::IsIconMode() const
{
	return mIconMode;
}

BOOL VideoBuffer::IsModeSettingInProgress() const
{
	return mModeSettingInProgress;
}

int VideoBuffer::GetXRes() const
{
	return mXRes;
}

int VideoBuffer::GetYRes() const
{
	return mYRes;
}

int VideoBuffer::GetLineLen() const
{
	return mLineLen;
}

int VideoBuffer::GetZLineLen() const
{
	return mXRes;
}

MR_UInt8 *VideoBuffer::GetBuffer()
{
	return mBuffer;
}

MR_UInt16 *VideoBuffer::GetZBuffer()
{
	return mZBuffer;
}

VideoBuffer::pixelMeter_t VideoBuffer::GetPixelMeter() const
{
	if (mFullScreen) {
		return pixelMeter_t(mXRes * 3, mYRes * 4);
	}
	else {
		return pixelMeter_t(
			3 * desktopWidth,
			4 * desktopHeight);
	}
}

BOOL VideoBuffer::Lock()
{
#ifdef WITH_SDL
	SDL_Surface *surface = SDL_GetVideoSurface();

	if (SDL_MUSTLOCK(surface)) {
		if (SDL_LockSurface(surface) < 0) {
			throw Exception("Unable to lock surface");
		}
	}

	mBuffer = static_cast<MR_UInt8*>(surface->pixels);

	return TRUE;
#else
	PRINT_LOG("Lock");

	MR_SAMPLE_CONTEXT("LockVideoBuffer");

	BOOL lReturnValue = TRUE;

	ASSERT(mBuffer == NULL);
	ASSERT(mDirectDraw != NULL);

	if(mIconMode) {
		lReturnValue = FALSE;
	}

	if(mBackBuffer == NULL) {
		// ASSERT( FALSE ); // It is possible but I want to know when it append
		// No surface
		lReturnValue = FALSE;
	}
	// Restore lost buffers (I have to do that but I don't know why
	if(lReturnValue) {
		if(DD_CALL(mFrontBuffer->IsLost()) == DDERR_SURFACELOST) {
			if(DD_CALL(mFrontBuffer->Restore()) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = FALSE;
			}
		}
		if(DD_CALL(mBackBuffer->IsLost()) == DDERR_SURFACELOST) {
			if(DD_CALL(mBackBuffer->Restore()) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = FALSE;
			}
		}

	}
	// Do the lock
	if(lReturnValue) {
		if(!gDebugMode && (mBpp == 8)) {
			DDSURFACEDESC lSurfaceDesc;

			lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);

			if(DD_CALL(mBackBuffer->Lock(NULL, &lSurfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) != DD_OK) {
				ASSERT(FALSE);
				lReturnValue = FALSE;
			}
			else {
				mLineLen = lSurfaceDesc.lPitch;
				mBuffer = (MR_UInt8 *) lSurfaceDesc.lpSurface;
			}
		}
		else {
			// Render indirectly if debugging or not in 256-color mode.
			mBuffer = new MR_UInt8[mXRes * mYRes];
			mLineLen = mXRes;
		}
	}

	return lReturnValue;
#endif
}

void VideoBuffer::Unlock()
{
#ifdef WITH_SDL
	SDL_Surface *surface = SDL_GetVideoSurface();

	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
#else
	PRINT_LOG("Unlock");

	MR_SAMPLE_CONTEXT("UnlockVideoBuffer");

	ASSERT(mBuffer != NULL);
	ASSERT(mDirectDraw != NULL);
	ASSERT(mBackBuffer != NULL);

	if(!gDebugMode && (mBpp == 8)) {
		if(DD_CALL(mBackBuffer->Unlock(NULL)) != DD_OK) {
			ASSERT(FALSE);
		}
		mBuffer = NULL;
	}
	else {
		// Lock the back buffer and copy mBuffer
		if(DD_CALL(mFrontBuffer->IsLost()) == DDERR_SURFACELOST) {
			if(DD_CALL(mFrontBuffer->Restore()) != DD_OK) {
				// ASSERT( FALSE );
			}
		}
		if(DD_CALL(mBackBuffer->IsLost()) == DDERR_SURFACELOST) {
			if(DD_CALL(mBackBuffer->Restore()) != DD_OK) {
				// ASSERT( FALSE );
			}
		}

		DDSURFACEDESC lSurfaceDesc;

		lSurfaceDesc.dwSize = sizeof(lSurfaceDesc);

		if(DD_CALL(mBackBuffer->Lock(NULL, &lSurfaceDesc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) != DD_OK) {
			// ASSERT( FALSE );
		}
		else {
			int lLineLen = lSurfaceDesc.lPitch;
			MR_UInt8 *lDest = (MR_UInt8 *) lSurfaceDesc.lpSurface;
			MR_UInt8 *lSrc = mBuffer;

			MR_SAMPLE_START(CopyVideoBuffer, "CopyVideoBuffer");

			if(mBpp <= 8) {
				for(int lCounter = 0; lCounter < mYRes; lCounter++) {
					memcpy(lDest, lSrc, mXRes);
					lDest += lLineLen;
					lSrc += mLineLen;
				}
			}
			else if(mBpp <= 16) {
				for(int lCounter = 0; lCounter < mYRes; lCounter++) {
					MR_UInt16 *lRDest = reinterpret_cast < MR_UInt16 * >(lDest);
					for(int lx = 0; lx < mXRes; ++lx) {
						*lRDest++ = static_cast < MR_UInt16 > (mPackedPalette[lSrc[lx]]);
					}
					lDest += lLineLen;
					lSrc += mLineLen;
				}
			}
			else if(mBpp <= 24) {
				// Note: Untested!
				for(int lCounter = 0; lCounter < mYRes; lCounter++) {
					MR_UInt8 *lRDest = lDest;
					for(int lx = 0; lx < mXRes; ++lx) {
						DWORD lColor = mPackedPalette[lSrc[lx]];
						*lRDest++ = static_cast < MR_UInt8 > ((lColor >> 16) & 0xff);
						*lRDest++ = static_cast < MR_UInt8 > ((lColor >> 8) & 0xff);
						*lRDest++ = static_cast < MR_UInt8 > (lColor & 0xff);
					}
					lDest += lLineLen;
					lSrc += mLineLen;
				}
			}
			else if(mBpp <= 32) {
				for(int lCounter = 0; lCounter < mYRes; lCounter++) {
					MR_UInt32 *lRDest = reinterpret_cast < MR_UInt32 * >(lDest);
					for(int lx = 0; lx < mXRes; ++lx) {
						*lRDest++ = mPackedPalette[lSrc[lx]];
					}
					lDest += lLineLen;
					lSrc += mLineLen;
				}
			}

			MR_SAMPLE_END(CopyVideoBuffer);

		}

		// Unlock
		if(DD_CALL(mBackBuffer->Unlock(NULL)) != DD_OK) {
			// ASSERT( FALSE );
		}

		delete[]mBuffer;
		mBuffer = NULL;
	}
#endif

	Flip();
}

void VideoBuffer::Flip()
{
#ifdef WITH_SDL
	SDL_Flip(SDL_GetVideoSurface());
#else
	PRINT_LOG("Flip");

	HRESULT lErrorCode;

	ASSERT(mBuffer == NULL);
	ASSERT(mDirectDraw != NULL);
	ASSERT(mFrontBuffer != NULL);

	if(mFullScreen) {
		if(DD_CALL(mFrontBuffer->Flip(NULL, DDFLIP_WAIT)) != DD_OK) {
			// ASSERT( FALSE );
		}

	}
	else {
		// We are in a window, use normal blitting
		int lX0 = mFullScreen ? 0 : mX0;
		int lY0 = mFullScreen ? 0 : mY0;

		RECT lDestRectangle = { lX0, lY0, lX0 + mXRes, lY0 + mYRes };
		RECT lSrcRectangle = { 0, 0, mXRes, mYRes };

		lErrorCode = DD_CALL(mFrontBuffer->Blt(&lDestRectangle, mBackBuffer, &lSrcRectangle, DDBLT_WAIT, NULL));

		if(lErrorCode != DD_OK) {
			// ASSERT( FALSE );
		}
	}
#endif
}

void VideoBuffer::Clear(MR_UInt8 pColor)
{
	ASSERT(mBuffer != NULL);
#ifndef WITH_SDL
	ASSERT(mDirectDraw != NULL);
	ASSERT(mBackBuffer != NULL);
#endif

	memset(mBuffer, pColor, mLineLen * mYRes);
}

void VideoBuffer::EnterIconMode()
{
	PRINT_LOG("EnterIconMode");

	if(!mIconMode) {
		mIconMode = TRUE;
	}
}

void VideoBuffer::ExitIconMode()
{
	PRINT_LOG("ExitIconMode");

	if(mIconMode) {
		mIconMode = FALSE;
	}
}

}  // namespace VideoServices
}  // namespace HoverRace
