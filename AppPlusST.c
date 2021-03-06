/* $Id: AppPlusST.c,v 1.7 1995/08/04 17:46:08 cwikla Exp $ */
/*
 * Copyright 1994,1995 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/ShellP.h>
#include <stdio.h>

#if MOTIF
#include <Xm/Label.h>
#else
#include <X11/Xaw/Command.h>
#endif /* MOTIF */

#include "AppPlusS.h"
#include "SubPlusS.h"
#if HAS_XCC
#include "XCC.h"
#endif /* HAS_XCC */

#include <stdio.h>

#define APPNAME "AppPlusSTest"
#define APPCLASS "AppPlusSTest"

static void QuitIt(_w, _nil, _event)
Widget _w;
XtPointer _nil;
XEvent *_event;
{
	Display *dpy = XtDisplay(_w);
	if (_event->type == ButtonPress)
		if (((XButtonEvent *)_event)->button == 3)
		{
			printf("Have a nice day. --JLC\n");
			exit(1);
		}
		else
		{
			Widget shell, button;

			XtDestroyWidget(XtParent(_w));
			shell = XtAppCreateShell (APPNAME, APPCLASS,
					appPlusShellWidgetClass, dpy, NULL, 0);

#if MOTIF
			button = XtCreateManagedWidget("Quit", xmLabelWidgetClass, shell, NULL, 0);
#else
			button = XtCreateManagedWidget("Quit", commandWidgetClass, shell, NULL, 0);
#endif /* MOTIF */
			XtAddEventHandler(button, ButtonPressMask, FALSE, QuitIt, NULL);

			XtRealizeWidget(shell);
		}
		
}

static GC TheGC;

#if HAS_XCC
static XFontStruct *TheFont;
static XCC TheXCC;
static int TheTextHeight;

static char *ColorList[] =
{
	"White",
	"Red", 
	"Green",
	"Blue",
	"Yellow",
	"Violet",
	"Cyan",
	"Orange",
	"Brown",
	"LightSkyBlue",
	"DarkSeaGreen",
	"gold",
	"salmon",
	"coral",
	"orchid",
	"seashell",
	"YellowGreen",
	"RosyBrown",
	"#RC63F9",
	"#339A36",
	"#5F8D09",
	"Black",
};

#define NUMBER(a) (sizeof(a)/sizeof(a[0]))

static void DrawIt(_w, _nil, _event)
Widget _w;
XtPointer _nil;
XEvent *_event;
{
	Pixel pix, backPix;
	XColor color;
	int x, y;
	int i;
	double ginc, gr;

	ginc = 65535.0/(NUMBER(ColorList) * TheTextHeight);

	for(i=0,gr=0;i<NUMBER(ColorList)*TheTextHeight;i++,gr+=ginc)
	{
		pix = XCCGetPixel(TheXCC, (int)gr, (int)gr, (int)gr);
		XSetForeground(XtDisplay(_w), TheGC, pix);
		XDrawLine(XtDisplay(_w), XtWindow(_w), TheGC,
			0, i, 256, i);
	}

	y = TheFont->ascent;
	x = 0;
	for(i=0,gr=0;i<NUMBER(ColorList);i++,gr+=ginc)
	{
		color.red = color.blue = color.green = 0;
		XParseColor(XtDisplay(_w), XCCGetColormap(TheXCC),
			ColorList[i], &color);

		pix = XCCGetPixel(TheXCC, color.red, color.green, color.blue);
		XSetForeground(XtDisplay(_w), TheGC, pix);
		XDrawString(XtDisplay(_w), XtWindow(_w), TheGC,
			x, y, ColorList[i], strlen(ColorList[i]));
		y += TheTextHeight;
	}

}
#endif /* HAS_XCC */



static XtCallbackProc saveYourselfCB(_w, _client, _data)
Widget _w;
XtPointer _client;
XtPointer _data;
{
	printf("Save yourself...\n");
}

main(argc, argv)
int argc;
char *argv[];
{
	XColor color;
	Arg warg[4];
	int n;
	Widget button, toplevel;
	Widget button2, popup;
	XtAppContext app;
	Display *theDisplay;
	int theScreenNumber;
	Visual *theVisual;
	Colormap theColormap;
	XGCValues gcValues;

	XtToolkitInitialize();
	app = XtCreateApplicationContext();

	theDisplay = XtOpenDisplay (app, NULL, APPNAME, APPCLASS, 
		NULL, 0, &argc, argv);

	if (!theDisplay)
	{
	printf("%s: can't open display, exiting...", APPNAME);
	exit (0);
	}

	theScreenNumber = DefaultScreen(theDisplay);

	toplevel = XtAppCreateShell (APPNAME, APPCLASS,
		appPlusShellWidgetClass, theDisplay, NULL, 0);

#if HAS_XCC
	TheXCC = AppPlusShellGetXCC(toplevel);

	printf("XCC using %d colors.\n", XCCGetNumColors(TheXCC));
#endif /* HAS_XCC */

	XtAddCallback(toplevel, XtNsaveYourselfCallback, (XtCallbackProc)saveYourselfCB, NULL);

#if MOTIF
	button = XtCreateManagedWidget("Quit", xmLabelWidgetClass, toplevel, NULL, 0);
#else
	button = XtCreateManagedWidget("Quit", commandWidgetClass, toplevel, NULL, 0);
#endif /* MOTIF */

	popup = XtPlusCreatePopupShell("popup", transientShellWidgetClass, button, NULL, 0);

#if HAS_XCC
	TheFont = XLoadQueryFont(theDisplay, "fixed");
	TheTextHeight = TheFont->ascent + TheFont->descent;
	
	n = 0;
	XtSetArg(warg[n], XtNwidth, 256); n++;
	XtSetArg(warg[n], XtNheight, TheTextHeight * NUMBER(ColorList)); n++;
	button2 = XtCreateManagedWidget("XCCTest", coreWidgetClass, popup, warg, n);
#else
#if MOTIF
	button2 = XtCreateManagedWidget("Hello", xmLabelWidgetClass, popup, NULL, 0) ;
#else
	button2 = XtCreateManagedWidget("Hello", commandWidgetClass, popup, NULL, 0) ;
#endif /* MOTIF */
#endif /* HAS_XCC */

#if HAS_XCC
	XtAddEventHandler(button2, ExposureMask, FALSE, DrawIt, NULL);
#endif /* HAS_XCC */

	XtRealizeWidget(toplevel);

	XtPopup(popup, XtGrabNone);


#if HAS_XCC
	TheFont = XLoadQueryFont(theDisplay, "fixed");
	gcValues.font = TheFont->fid;
	TheGC = XCreateGC(theDisplay, XtWindow(popup), GCFont, &gcValues);
	TheTextHeight = TheFont->ascent + TheFont->descent;
#endif /* HAS_XCC */

	XtAddEventHandler(button, ButtonPressMask, FALSE, QuitIt, NULL);

	printf("Press mouse button 3 to exit.\n");

	XtAppMainLoop(app);
}
