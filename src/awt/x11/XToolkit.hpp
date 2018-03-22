#ifndef __AWT_X11_XTOOLKIT_HPP
#define __AWT_X11_XTOOLKIT_HPP

#include <awt/Window.hpp>
#include <lang/Thread.hpp>
#include <nio/ByteBuffer.hpp>

namespace awt { namespace x11 {

class XGraphicsEnvironment : extends GraphicsEnvironment {
public:
	Array<GraphicsDevice*> getScreenDevices();
	GraphicsDevice& getDefaultScreenDevice();
};

class XEvent : extends Object {
private:
	Shared<nio::ByteBuffer> pData;
public:
	static int getSize() { return 192; }

	XEvent() {
		pData = nio::ByteBuffer::allocate(getSize());
	}
	XEvent(Shared<nio::ByteBuffer> buf) {
		pData = buf;
	}
	void *getPData() { return &(pData->array()[0]); }
	void dispose() { pData.reset(); }
	int get_type() { return pData->getInt(0); }
};

class XBaseWindow;
class XToolkit : extends Toolkit, implements Runnable {
private:
	void init();
	long getNextTaskTime();
public:
	static void awtLock();
	static void awtUnlock();
	static long getDisplay();
	static long getDefaultRootWindow();
	static boolean getSunAwtDisableGrab();
	static void addToWinMap(long window, XBaseWindow* xwin);
	static void removeFromWinMap(long window, XBaseWindow* xwin);
	static XBaseWindow* windowToXWindow(long window);
	//static void addEventDispatcher(long window, XEventDispatcher dispatcher);

	XToolkit();
	awt::FramePeer* createFrame(awt::Frame* target);
	awt::LightweightPeer* createComponent(awt::Component* target);
	awt::WindowPeer* createWindow(awt::Window* target);
	awt::DialogPeer* createDialog(awt::Dialog* target);

	awt::MouseInfoPeer& getMouseInfoPeer();

	void run();
	void run(boolean loop);
};

}}

#endif
