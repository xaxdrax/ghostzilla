#include "./CHANGELOG"

#include <windows.h>



/*********************************************************************************
 * TIMER related functions and values                      {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
#define MAINTIMERRATE GetMainTimerRate()			                     // hundredths of seconds
#define maxMouseTicksToReachTheOtherSide (MAINTIMERRATE * 2)             // two seconds
#define minMouseTicksSinceReactivationBeforeDeactivationDetection (MAINTIMERRATE /2) // 1/2 sec

// number of ticks from start during which hiding is disabled -- 8 seconds
#define TICKS_FROM_START_DURING_WHICH_HIDING_IS_DISABLED (8 * MAINTIMERRATE)

// MAINTIMERRATE implementation - 100 per sec (10ms) on NT, and 20 per sec (50ms, the minimum) on 98
static int realTimerRate = 0;
static inline int GetMainTimerRate() {
	if (realTimerRate == 0) {
		OSVERSIONINFO os;
		GetVersionEx( &os );
		if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			// NT, 2000, XP: 100 per second
			realTimerRate = 100;
		} else {
			// 95, 98, Me: 20 per second
			realTimerRate = 20;
		}
	}
	return realTimerRate;
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/


/*********************************************************************************
 * WIDGET stuff                                             {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
HWND hOneAndOnlyBrowserWindow = NULL;
// basic main window geometry (not couting user-caused window shrinking)
static LONG mainX, mainY, mainW, mainH;

enum { STARTFROMCENTER,
	   STARTFROMUPLEFT,
	   STARTFROMUPRIGHT,
	   STARTFROMBOTTOMLEFT,
	   STARTFROMBOTTOMRIGHT,
	   STARTFROMUPMIDDLE,
	   STARTFROMLEFTMIDDLE,
	   STARTFROMRIGHTMIDDLE,
	   STARTFROMBOTTOMMIDDLE,
	   LASTPOINTTOTRY = STARTFROMBOTTOMMIDDLE,
};

const MINIMAL_PIC_SURFACE = (200*200);

BOOL enlargeWindowToFitTheLargestWhiteArea( LONG &x, LONG &y, LONG &w, LONG &h ) {
	HWND hwndDesktopWindow = GetDesktopWindow();
	RECT rectDesktopWindow;
	struct MaxWhiteRect {
		LONG x, y, w, h;
	} maxWhiteRect = { 0, 0, 0, 0 };

	if (::GetWindowRect( hwndDesktopWindow, &rectDesktopWindow ) == 0) {
		return FALSE;
	}

	for (int initialPoint = 0; initialPoint <= LASTPOINTTOTRY; initialPoint++) {
		LONG x1, y1, x2, y2;
		switch (initialPoint) {
		case STARTFROMCENTER:
			x1 = x2 = rectDesktopWindow.right  / 2;
			y1 = y2 = rectDesktopWindow.bottom / 2;
			break;
		case STARTFROMUPLEFT:
			x1 = x2 = rectDesktopWindow.right  / 4;
			y1 = y2 = rectDesktopWindow.bottom / 4;
			break;
		case STARTFROMUPRIGHT:
			x1 = x2 = rectDesktopWindow.right  * 3 / 4;
			y1 = y2 = rectDesktopWindow.bottom / 4;
			break;
		case STARTFROMBOTTOMLEFT:
			x1 = x2 = rectDesktopWindow.right  / 4;
			y1 = y2 = rectDesktopWindow.bottom * 3 / 4;
			break;
		case STARTFROMBOTTOMRIGHT:
			x1 = x2 = rectDesktopWindow.right  * 3 / 4;
			y1 = y2 = rectDesktopWindow.bottom * 3 / 4;
			break;
		case STARTFROMUPMIDDLE:
			x1 = x2 = rectDesktopWindow.right  / 2;
			y1 = y2 = rectDesktopWindow.bottom / 4;
			break;
		case STARTFROMLEFTMIDDLE:
			x1 = x2 = rectDesktopWindow.right  / 4;
			y1 = y2 = rectDesktopWindow.bottom / 2;
			break;
		case STARTFROMRIGHTMIDDLE:
			x1 = x2 = rectDesktopWindow.right  * 3 / 4;
			y1 = y2 = rectDesktopWindow.bottom / 2;
			break;
		case STARTFROMBOTTOMMIDDLE:
			x1 = x2 = rectDesktopWindow.right  / 2;
			y1 = y2 = rectDesktopWindow.bottom * 3 / 4;
			break;
		}

		// this finds middle window's coordinates perfectly! the only problem is
		// that that window may overlap with something else. but it seems that for
		// all normal apps, especially if they use MDI, works great.
		POINT start = { x1, y1 };
		HWND winPoint = ::WindowFromPoint( start );

		// collect its geometry
		RECT rectWinPoint;
		if (winPoint != NULL) {
			if (::GetWindowRect( winPoint, &rectWinPoint ) > 0) {
				x1 = rectWinPoint.left;
				y1 = rectWinPoint.top;
				x2 = rectWinPoint.right;
				y2 = rectWinPoint.bottom;
			}
			else
				continue;
		}
		LONG wt = x2 - x1 + 1, ht = y2 - y1 + 1;

		// if the window is too small, try again
		if (wt * ht < MINIMAL_PIC_SURFACE) continue;

		// if the window is too big, it's probably the desktop -- try again
		if ( (x2 - x1) > rectDesktopWindow.right * 9 / 10 &&
		     (y2 - y1) > rectDesktopWindow.bottom * 9 / 10 ) {
			continue;
		}

		// check that the height isn't too big (a mouse move up or down must be able to remove the window)
		if (y1 - rectDesktopWindow.bottom/10 <= 0 &&
			y2 + rectDesktopWindow.bottom/10 >= rectDesktopWindow.bottom-1 ) {
			y1 = rectDesktopWindow.bottom/10 + 1;
			ht = y2 - y1 + 1;
		}
		
		// if the new window has a larger surface than the candidates so far, pick it.
		// fixme: is that a good criterium?
		if (wt * ht > maxWhiteRect.w * maxWhiteRect.h) {
			maxWhiteRect.x = x1;
			maxWhiteRect.y = y1;
			maxWhiteRect.w = wt;
			maxWhiteRect.h = ht;
		}
		// fixme: break if you found a good enough -- that ok?
		break;
	}
	if (maxWhiteRect.w * maxWhiteRect.h < MINIMAL_PIC_SURFACE) {
		// nothing found; pick a window in the middle of the screen
		maxWhiteRect.x = rectDesktopWindow.right  / 4;
		maxWhiteRect.y = rectDesktopWindow.bottom / 4;
		maxWhiteRect.w = rectDesktopWindow.right  / 2;
		maxWhiteRect.h = rectDesktopWindow.bottom / 2;
	}
	x = maxWhiteRect.x;
	y = maxWhiteRect.y;
	w = maxWhiteRect.w;
	h = maxWhiteRect.h;

	return TRUE;
}

BOOL mouseOverWindow = FALSE;
#define ID_CHECKTIMER 667

POINT point;
int rdState = 0;	// reactivation detection state
LONG nMouseTicks = 0;
LONG lastMouseTick;
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/




/*********************************************************************************
 * hidingLevel                                              {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
static int hidingLevel = 5;
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/



/*********************************************************************************
 * graying functions                                        {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
static inline unsigned int myGrayScalingFunction( unsigned char red, unsigned char green,
	unsigned char blue, int extraBright )
{
	int gray = (unsigned char)(.299 * red + .587 * green + .114 * blue);
	if (extraBright) {
		gray += 100;
		if (gray < 140)  {
			gray = 140;
		}
	}
	if (gray > 255) {
		gray = 255;
	}
	return (unsigned int) gray;
}

// determining if it's grayed
#define ITSAKINDOFMAGIC 77
#define isItGrayable() ((mAlphaBits == NULL) && (*mAlphaDepth != ITSAKINDOFMAGIC))
#define isItGrayed()   ((mAlphaBits == NULL) && (*mAlphaDepth == ITSAKINDOFMAGIC))
#define setGrayed()    (*mAlphaDepth = ITSAKINDOFMAGIC)

// this func determines if the pic should be color, b/w, or gray:
// level 		small pic		big pic
// 1			color			color
// 2			b/w				color
// 3			b/w				b/w
// 4			gray			color
// 5			gray			b/w
// 6			gray			gray
// color = return false
// b/w   = *extraBright = 0, return true
// gray  = *extraBright = 1, return true
static inline bool pickGrayOrBWforPic( int w, int h, int *extraBright )
{
	if ( w * h  < 2000 ) {
		// small pic
		if (hidingLevel < 2) {
			return false;	// color
		}else if (hidingLevel < 4) {
			*extraBright = 0;	// b/w
		} else {
			*extraBright = 1;	// gray
		}
		return true;
	} else {
		// large pic
		if (hidingLevel == 1 || hidingLevel == 2 || hidingLevel == 4) {
			return false;	// color
		} else if (hidingLevel == 6) {
			*extraBright = 1;	// gray
		} else {
			*extraBright = 0;	// bw
		}
		return true;
	}
}

#define ghzilTOOBIG(W,H) ((W) * (H) > 2000)	// in pixels
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/



/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/************************   D L L    F U N C T I O N S      *********************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/





/*********************************************************************************
 * nsWindow_ProcessMessage_6                                {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from WIDGET/nswidget.cpp:ProcessMessage
__declspec(dllexport) void nsWindow_ProcessMessage_6( void * ovaj,
		UINT * msg,
		WPARAM * wParam,
		HWND mWnd,
		unsigned char *mIsVisible )
{

	static int decementUnit = 0;

	// check if the user pressed ALT-up or ALT-down (resize window)
	if (*msg == WM_KEYDOWN && (::GetKeyState(VK_CONTROL) &0x80) && *mIsVisible && hOneAndOnlyBrowserWindow != NULL) {
		int oldDecrementUnit = decementUnit;
		if (*wParam == VK_UP) {
			if (decementUnit >= -4) {
				--decementUnit;
			}
		}
		if (*wParam == VK_DOWN) {
			if (decementUnit < 0) {
				++decementUnit;
			}
		}
		if (oldDecrementUnit != decementUnit) {
			::SetWindowPos( hOneAndOnlyBrowserWindow, HWND_TOP, 0, 0, mainW, mainH * (8 + decementUnit) / 8, SWP_NOMOVE );
		}
	}

	if (mWnd != hOneAndOnlyBrowserWindow)
		return;

	if (*msg != WM_TIMER || *wParam != ID_CHECKTIMER) return;


				int mustShow = 0;
				int mustPersist = 0;

				// this block checks if less than N (say, 8) secondws has elapsed from the start.
				// if so, sets mustPersist to 1, so in the first N seconds the window won't 
				// disappear when the mouse moves. 
				static int ticksBeforeHidingAllowed = -10;
				if (ticksBeforeHidingAllowed != 0) {
					if (ticksBeforeHidingAllowed == -10) {
						ticksBeforeHidingAllowed = TICKS_FROM_START_DURING_WHICH_HIDING_IS_DISABLED;
					}
					--ticksBeforeHidingAllowed;
					mustPersist = 1;
				}
				// end of persistence check block

				GetCursorPos (&point);
				RECT rectDW; // DesktopWindow;
				::GetWindowRect( ::GetDesktopWindow(), &rectDW );
				RECT rectCW; // CurrentWindow;

				::GetWindowRect( mWnd, &rectCW );

				++nMouseTicks;

				if (rdState == 0) {
					if (point.x < 10 /* left edge, same as i == 0 */ ) {
						rdState = 1;
						lastMouseTick = nMouseTicks;
					}
				} else if (rdState == 1) {
					if (point.x >= rectDW.right - 10 /* right edge */) {
						rdState = 2;
						lastMouseTick = nMouseTicks;
					} else if (nMouseTicks - lastMouseTick > maxMouseTicksToReachTheOtherSide) {
						rdState = 0;
					}
				} else if (rdState == 2) {
					if (point.x < 10 /* left edge */) {
						rdState = 3;
						lastMouseTick = nMouseTicks;
						mustShow = 1;
					} else if (nMouseTicks - lastMouseTick > maxMouseTicksToReachTheOtherSide) {
						rdState = 0;
					}
				} else if (rdState == 3) {
					mustPersist = 1;
					if (nMouseTicks - lastMouseTick > minMouseTicksSinceReactivationBeforeDeactivationDetection) {
						rdState = 0;
					}
				}

				if (mustShow) {
					LONG x, y, w, h;
					if (enlargeWindowToFitTheLargestWhiteArea(x, y, w, h) == FALSE) {
						ShowWindow( mWnd, SW_SHOW );
					} else {
						if (x == mainX && y == mainY && w == mainW && h == mainH) {
							h = mainH * (8 + decementUnit) / 8;
						} else {
							mainX = x; mainY = y; mainW = w; mainH = h;
							decementUnit = 0;
						}
						// make it topmost
						::SetWindowPos( mWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE );
						// then show it
						::ShowWindow( mWnd, SW_SHOW );
						// then make it not topmost
						::SetWindowPos( mWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
						//::SetFocus( mWnd );
						::SetForegroundWindow( mWnd );
						::SetActiveWindow( mWnd );
					}
					*mIsVisible = TRUE;
				} else if (*mIsVisible) {
					if (mouseOverWindow == FALSE) {
						if (point.x >= rectCW.left && point.x < rectCW.right &&
							point.y >= rectCW.top  && point.y < rectCW.bottom) {
							mouseOverWindow = TRUE;
						}
					}
					// check if the mouse coursor left the app area (meaning the user wants to hide the app)
					if (mouseOverWindow && !mustPersist) {
						// the zone to check is actually slightly larger then the app window, for comfort
						RECT rectCW; // CurrentWindow;
						::GetWindowRect( mWnd, &rectCW );
						int minStrayDistanceX = rectDW.right  / 10; 
						int minStrayDistanceY = rectDW.bottom / 10; 
						if ( point.x < max( rectCW.left   - minStrayDistanceX, rectDW.left   ) ||
							 point.x > min( rectCW.right  + minStrayDistanceX, rectDW.right  ) ||
							 point.y < max( rectCW.top    - minStrayDistanceY, rectDW.top    ) ||
							 point.y > min( rectCW.bottom + minStrayDistanceY, rectDW.bottom ) ) {
							mouseOverWindow = 0;
							rdState = 0;
						 	ShowWindow( mWnd, SW_HIDE );	// SW_HIDE hides it without trace!
							*mIsVisible = FALSE;
						}
					}
				}

}

/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/



/*********************************************************************************
 * nsWindow_*_8,9,10                                        {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from WIDGET/nswidget.cpp
// returns 1 (TRUE) if the passed handle belongs to the one and only window
__declspec(dllexport) int nsWindow_Move_8( HWND mWnd ) {
	return (mWnd == hOneAndOnlyBrowserWindow) ? 1 : 0;
}
__declspec(dllexport) int nsWindow_Resize_9( HWND mWnd ) {
	return (mWnd == hOneAndOnlyBrowserWindow) ? 1 : 0;
}
__declspec(dllexport) int nsWindow_Resize_10( HWND mWnd ) {
	return (mWnd == hOneAndOnlyBrowserWindow) ? 1 : 0;
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/



/*********************************************************************************
 * nsImageWin_CreateDDB_14                                  {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from GFX/nsImageWin.cpp:CreateDDB
__declspec(dllexport) void nsImageWin_CreateDDB_14( void * thisObj,
	void *mAlphaBits,
	signed char *mAlphaDepth,
	int mNaturalHeight,
	int mNaturalWidth,
	int mRowBytes,
	int mNumBytesPixel,
	unsigned char *mImageBitsPtr) {

		register unsigned char * mImageBits = mImageBitsPtr;

		int bright;

       	// gray it: {
       	if (isItGrayable() &&
       		pickGrayOrBWforPic(mNaturalWidth, mNaturalHeight, &bright) ) {
	   		setGrayed();
	   		for (register int i = 0; i < mNaturalHeight; i++) {
				for (register int j = 0; j < mNaturalWidth; j++) {
					register int o = i * mRowBytes + j * mNumBytesPixel;
					unsigned char gray = myGrayScalingFunction( mImageBits[o+0], mImageBits[o+1], mImageBits[o+2], bright );
					mImageBits[o+0] = mImageBits[o+1] = mImageBits[o+2] = (unsigned char)gray;
				}
			}
		 }
		 //
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/




/*********************************************************************************
 * nsWindow_StandardWindowCreate_17                         {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from WIDGET/nsWindow.cpp:StandardWindowCreate()
__declspec(dllexport) void nsWindow_StandardWindowCreate_17( void * ovaj,
			HWND *pmWnd,
			int mWindowType,
			int eWindowType_toplevel,
			int extendedStyle,
			LPCTSTR lpClassName,
			HWND parent,
			HINSTANCE hInstance )
{
		*pmWnd = NULL;
		if (mWindowType == eWindowType_toplevel) {
			static int numberOfTopLevelWindowsCreated = 0;
			numberOfTopLevelWindowsCreated++;
			// this is a terrible, terrible hack: if this is the second toplevel window ever,
			// we'll call it *the* browser (the oneandonly) window and affect that one 
			if (numberOfTopLevelWindowsCreated == 2) {
				LONG x, y, w, h;
				if (enlargeWindowToFitTheLargestWhiteArea(x, y, w, h)) {
			      	*pmWnd = ::CreateWindowEx(extendedStyle,
                              	lpClassName,
                              	"",
                              	WS_POPUP,
                              	x,
                              	y,
                              	w,
                              	h,
                              	parent,
                              	NULL,
	                            hInstance,
                              	NULL);
                    // remember this
					mainX = x, mainY = y, mainW = w, mainH =h;
            	}
				hOneAndOnlyBrowserWindow = *pmWnd;
				SetTimer (*pmWnd, ID_CHECKTIMER, 1000/MAINTIMERRATE, NULL); // check every 10 ms if mouse is still over the window
			}
		}

}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/


/*********************************************************************************
 * nsImageWin_Draw_19:                                      {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from GFX/nsImageWin.cpp::Draw
__declspec(dllexport) void nsImageWin_Draw_19( void * ovaj,
	HDC TheHDC,
	void *mAlphaBits,
	signed char *mAlphaDepth,
	int aDWidth,
	int aDHeight,
	int aDX,
	int aDY)
{
	int bright;

    // gray out the little pic in TheHDC as it couldn't have been grayed out directly
      if (!isItGrayed() &&
      	  pickGrayOrBWforPic(aDWidth, aDHeight, &bright) ) {
	      for (int x = 0; x < aDWidth; x++) {
				for (int y = 0; y < aDHeight; y++) {
		        	COLORREF pixel = ::GetPixel( TheHDC, aDX + x, aDY + y );
		        	int gray = myGrayScalingFunction( GetRValue( pixel ), GetGValue( pixel ), GetBValue( pixel ), bright );
		        	pixel = RGB( gray, gray, gray );
		        	::SetPixel( TheHDC, aDX + x, aDY + y, pixel );
				}
		  }
      }
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/




/*********************************************************************************
 * ghzil_GetHidingLevel_25                                  {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called mostly from nsImageFrame
__declspec(dllexport) int ghzil_GetHidingLevel_25() {
	return hidingLevel;
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/

/*********************************************************************************
 * ghzil_SetHidingLevel_26                                  {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from xpfe/components/winhooks/winhooks/nsWindowsHooks.cpp
__declspec(dllexport) void ghzil_SetHidingLevel_26( int preflevel ) {
	hidingLevel = preflevel;
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/





/*********************************************************************************
 * getProfileLocation_31           {{{{{{{{{{{{{{{{{{{{{{
 *********************************************************************************/
// called from xpcom/io/nsAppFileLocationProvider.cpp::GetProductDirectory() and others
__declspec(dllexport) char * getProfileLocation_31() {
	return "TmpD";			// temp directory
}
/*********************************************************************************
 * }}}}}}}}}}}}}}
 *********************************************************************************/

