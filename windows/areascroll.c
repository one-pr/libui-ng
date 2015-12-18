// 8 september 2015
#include "uipriv_windows.h"
#include "area.h"

// TODO
// - move from pixels to points somehow
// 	- add a function to offset points and rects by scrolling amounts; call it from doPaint() in areadraw.c
// - recalculate scrolling after:
// 	- creation?
// 	- resize?
// 	- recreating the render target? (after moving to points)
// - error if these are called without scrollbars?

struct scrollParams {
	intmax_t *pos;
	intmax_t pagesize;
	intmax_t length;
	int *wheelCarry;
	UINT wheelSPIAction;
};

static void scrollto(uiArea *a, int which, struct scrollParams *p, intmax_t pos)
{
	SCROLLINFO si;
	intmax_t xamount, yamount;

	// note that the pos < 0 check is /after/ the p->length - p->pagesize check
	// it used to be /before/; this was actually a bug in Raymond Chen's original algorithm: if there are fewer than a page's worth of items, p->length - p->pagesize will be negative and our content draw at the bottom of the window
	// this SHOULD have the same effect with that bug fixed and no others introduced... (thanks to devin on irc.badnik.net for confirming this logic)
	if (pos > p->length - p->pagesize)
		pos = p->length - p->pagesize;
	if (pos < 0)
		pos = 0;

	// negative because ScrollWindowEx() is "backwards"
	xamount = -(pos - *(p->pos));
	yamount = 0;
	if (which == SB_VERT) {
		yamount = xamount;
		xamount = 0;
	}

	// TODO this isn't safe with Direct2D
//	if (ScrollWindowEx(a->hwnd, xamount, yamount,
//		NULL, NULL, NULL, NULL,
//		SW_ERASE | SW_INVALIDATE) == ERROR)
//		logLastError("error scrolling area in scrollto()");

	*(p->pos) = pos;

	// now commit our new scrollbar setup...
	ZeroMemory(&si, sizeof (SCROLLINFO));
	si.cbSize = sizeof (SCROLLINFO);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nPage = p->pagesize;
	si.nMin = 0;
	si.nMax = p->length - 1;		// endpoint inclusive
	si.nPos = *(p->pos);
	SetScrollInfo(a->hwnd, which, &si, TRUE);
}

static void scrollby(uiArea *a, int which, struct scrollParams *p, intmax_t delta)
{
	scrollto(a, which, p, *(p->pos) + delta);
}

static void scroll(uiArea *a, int which, struct scrollParams *p, WPARAM wParam, LPARAM lParam)
{
	intmax_t pos;
	SCROLLINFO si;

	pos = *(p->pos);
	switch (LOWORD(wParam)) {
	case SB_LEFT:			// also SB_TOP
		pos = 0;
		break;
	case SB_RIGHT:		// also SB_BOTTOM
		pos = p->length - p->pagesize;
		break;
	case SB_LINELEFT:		// also SB_LINEUP
		pos--;
		break;
	case SB_LINERIGHT:		// also SB_LINEDOWN
		pos++;
		break;
	case SB_PAGELEFT:		// also SB_PAGEUP
		pos -= p->pagesize;
		break;
	case SB_PAGERIGHT:	// also SB_PAGEDOWN
		pos += p->pagesize;
		break;
	case SB_THUMBPOSITION:
		ZeroMemory(&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		si.fMask = SIF_POS;
		if (GetScrollInfo(a->hwnd, which, &si) == 0)
			logLastError("error getting thumb position for area in scroll()");
		pos = si.nPos;
		break;
	case SB_THUMBTRACK:
		ZeroMemory(&si, sizeof (SCROLLINFO));
		si.cbSize = sizeof (SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(a->hwnd, which, &si) == 0)
			logLastError("error getting thumb track position for area in scroll()");
		pos = si.nTrackPos;
		break;
	}
	scrollto(a, which, p, pos);
}

static void wheelscroll(uiArea *a, int which, struct scrollParams *p, WPARAM wParam, LPARAM lParam)
{
	int delta;
	int lines;
	UINT scrollAmount;

	delta = GET_WHEEL_DELTA_WPARAM(wParam);
	if (SystemParametersInfoW(p->wheelSPIAction, 0, &scrollAmount, 0) == 0)
		// TODO use scrollAmount == 3 (for both v and h) instead?
		logLastError("error getting area wheel scroll amount in wheelscroll()");
	if (scrollAmount == WHEEL_PAGESCROLL)
		scrollAmount = p->pagesize;
	if (scrollAmount == 0)		// no mouse wheel scrolling (or t->pagesize == 0)
		return;
	// the rest of this is basically http://blogs.msdn.com/b/oldnewthing/archive/2003/08/07/54615.aspx and http://blogs.msdn.com/b/oldnewthing/archive/2003/08/11/54624.aspx
	// see those pages for information on subtleties
	delta += *(p->wheelCarry);
	lines = delta * ((int) scrollAmount) / WHEEL_DELTA;
	*(p->wheelCarry) = delta - lines * WHEEL_DELTA / ((int) scrollAmount);
	scrollby(a, which, p, -lines);
}

static void hscrollParams(uiArea *a, struct scrollParams *p)
{
	RECT r;

	ZeroMemory(p, sizeof (struct scrollParams));
	p->pos = &(a->hscrollpos);
	// TODO get rid of these and replace with points
	if (GetClientRect(a->hwnd, &r) == 0)
		logLastError("error getting area client rect in hscrollParams()");
	p->pagesize = r.right - r.left;
	p->length = a->scrollWidth;
	p->wheelCarry = &(a->hwheelCarry);
	p->wheelSPIAction = SPI_GETWHEELSCROLLCHARS;
}

static void hscrollto(uiArea *a, intmax_t pos)
{
	struct scrollParams p;

	hscrollParams(a, &p);
	scrollto(a, SB_HORZ, &p, pos);
}

static void hscrollby(uiArea *a, intmax_t delta)
{
	struct scrollParams p;

	hscrollParams(a, &p);
	scrollby(a, SB_HORZ, &p, delta);
}

static void hscroll(uiArea *a, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	hscrollParams(a, &p);
	scroll(a, SB_HORZ, &p, wParam, lParam);
}

static void hwheelscroll(uiArea *a, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	hscrollParams(a, &p);
	wheelscroll(a, SB_HORZ, &p, wParam, lParam);
}

static void vscrollParams(uiArea *a, struct scrollParams *p)
{
	RECT r;

	ZeroMemory(p, sizeof (struct scrollParams));
	p->pos = &(a->vscrollpos);
	if (GetClientRect(a->hwnd, &r) == 0)
		logLastError("error getting area client rect in vscrollParams()");
	p->pagesize = r.bottom - r.top;
	p->length = a->scrollHeight;
	p->wheelCarry = &(a->vwheelCarry);
	p->wheelSPIAction = SPI_GETWHEELSCROLLLINES;
}

static void vscrollto(uiArea *a, intmax_t pos)
{
	struct scrollParams p;

	vscrollParams(a, &p);
	scrollto(a, SB_VERT, &p, pos);
}

static void vscrollby(uiArea *a, intmax_t delta)
{
	struct scrollParams p;

	vscrollParams(a, &p);
	scrollby(a, SB_VERT, &p, delta);
}

static void vscroll(uiArea *a, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	vscrollParams(a, &p);
	scroll(a, SB_VERT, &p, wParam, lParam);
}

static void vwheelscroll(uiArea *a, WPARAM wParam, LPARAM lParam)
{
	struct scrollParams p;

	vscrollParams(a, &p);
	wheelscroll(a, SB_VERT, &p, wParam, lParam);
}

BOOL areaDoScroll(uiArea *a, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *lResult)
{
	switch (uMsg) {
	case WM_HSCROLL:
		hscroll(a, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case WM_MOUSEHWHEEL:
		hwheelscroll(a, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case WM_VSCROLL:
		vscroll(a, wParam, lParam);
		*lResult = 0;
		return TRUE;
	case WM_MOUSEWHEEL:
		vwheelscroll(a, wParam, lParam);
		*lResult = 0;
		return TRUE;
	}
	return FALSE;
}

// TODO do we need an areaScrollOnResize()?

void areaUpdateScroll(uiArea *a)
{
	// use a no-op scroll to simulate scrolling
	hscrollby(a, 0);
	vscrollby(a, 0);
}