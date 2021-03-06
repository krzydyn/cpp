#ifndef __AWT_WINDOW_HPP
#define __AWT_WINDOW_HPP

#include <awt/AWTEvent.hpp>
#include <awt/Color.hpp>
#include <awt/Graphics.hpp>
#include <awt/MouseInfo.hpp>
#include <awt/Toolkit.hpp>
#include <util/ArrayList.hpp>

namespace awt {

class IllegalComponentStateException : extends IllegalStateException {
	using IllegalStateException::IllegalStateException;
};

class Cursor : extends Object {
};

class AppContext;
class Container;
class PopupMenu;
class Region;
class ComponentListener;
class KeyListener;
class MouseWheelEvent;
class HierarchyListener;
class Component : extends Object {
private:
	String  name;
	boolean nameExplicitlySet = false;
	boolean isAddNotifyComplete = false;
	boolean valid = false;
	boolean focusable = true;
	boolean ignoreRepaint = false;

	GraphicsConfiguration* graphicsConfig = null;
	Region* compoundShape = null;
	HierarchyListener* hierarchyListener = null;

	boolean newEventsOnly = false;
	ComponentListener* componentListener;
	KeyListener* keyListener;

	void repaintParentIfNeeded(int oldX, int oldY, int oldWidth, int oldHeight);
	void notifyNewBounds(boolean resized, boolean moved);
	void reshapeNativePeer(int x, int y, int width, int height, int op);

	void applyCurrentShape();
	void applyCurrentShapeBelowMe();
	void subtractAndApplyShapeBelowMe();

	void mixOnShowing();
	void mixOnHiding(boolean isLightweight);
	void mixOnReshaping();
	void mixOnZOrderChanging(int oldZorder, int newZorder);
	void mixOnValidating() {}
	boolean isMixingNeeded() { return false; }

protected:
	static Object LOCK;

	ComponentPeer *peer = null;
	AppContext *appContext = null;
	Container *parent = null;
	int    x,y,width,height;
	Color  foreground;
	Color  background;
	Font   *font = null;
	Font   *peerFont = null;
	//Cursor     cursor;
	Locale     locale;
	long eventMask = AWTEvent::INPUT_METHODS_ENABLED_MASK;

	boolean visible = false;
	boolean enabled = false;
	boolean isPacked = false;
	int boundsOp = ComponentPeer::DEFAULT_OPERATION;

	Dimension minSize;
	boolean minSizeSet = false;
	Dimension prefSize;
	boolean prefSizeSet = false;
	Dimension maxSize;
	boolean maxSizeSet = false;

	ArrayList<PopupMenu*> popups;

	Component() : x(0),y(0),width(0),height(0) {
		//appContext = AppContext.getAppContext();
	}
	~Component();

	virtual String constructComponentName() const {return "";}

	virtual Point pointRelativeToComponent(Point absolute) const {
		Point p0 = getLocationOnScreen();
		return Point(absolute.x - p0.x, absolute.y - p0.y);
	}

	virtual void dispatchEventImpl(AWTEvent& e);
	virtual void processEvent(AWTEvent& e);
	virtual void invalidateParent();
	virtual void setGraphicsConfiguration(GraphicsConfiguration& gc);
	virtual void setBoundsOp(int op) {
		if (op == ComponentPeer::RESET_OPERATION) {
			boundsOp = ComponentPeer::DEFAULT_OPERATION;
		}
		else if (boundsOp == ComponentPeer::DEFAULT_OPERATION) {
			boundsOp = op;
		}
	}
	virtual int getBoundsOp() const {return boundsOp;}
	virtual boolean requestFocus(boolean temporary) {return false;}
	virtual boolean requestFocusInWindow(boolean temporary) {return false;}

	virtual boolean eventTypeEnabled(int type);
	virtual boolean areInputMethodsEnabled();
	virtual boolean dispatchMouseWheelToAncestor(MouseWheelEvent& e);

	virtual boolean transferFocus(boolean clearOnFailure);
	virtual boolean isRecursivelyVisible();

public:
	static constexpr float TOP_ALIGNMENT = 0.0f;
	static constexpr float CENTER_ALIGNMENT = 0.5f;
	static constexpr float BOTTOM_ALIGNMENT = 1.0f;
	static constexpr float LEFT_ALIGNMENT = 0.0f;
	static constexpr float RIGHT_ALIGNMENT = 1.0f;

	static Object& getLOCK() { return LOCK; }

	virtual boolean updateGraphicsData(GraphicsConfiguration& gc) {
		if (graphicsConfig == &gc) return false;
		graphicsConfig = &gc;
		if (peer != null) return peer->updateGraphicsData(gc);
		return false;
	}

	virtual void invalidateIfValid() final {
		if (isValid()) invalidate();
	}

	virtual Object& getTreeLock() { return LOCK; }
	virtual Toolkit& getToolkit();

	virtual String getName() const { return name; }
	virtual void setName(const String& name) {
		this->name = name;
		nameExplicitlySet = true;
	}
	virtual Container *getParent() const { return parent; }
	virtual ComponentPeer* getPeer() { return peer; }
	virtual GraphicsConfiguration& getGraphicsConfiguration() {
		if (graphicsConfig == null) throw NullPointerException("graphicsConfig");
		return *graphicsConfig;
	}
	virtual boolean isLightweight() { return instanceof<LightweightPeer>(peer); }

	virtual void setPreferredSize(const Dimension& preferredSize);
	virtual Dimension getPreferredSize();
	virtual boolean isPreferredSizeSet() { return prefSizeSet; }
	virtual void setMinimumSize(const Dimension& minimumSize);
	virtual Dimension getMinimumSize();
	virtual boolean isMinimumSizeSet() { return minSizeSet; }

	virtual boolean isValid() const { return (peer != null) && valid; }
	virtual boolean isDisplayable()  const{ return peer != null; }
	virtual boolean isVisible() const { return visible; }
	virtual Point getMousePosition() const {
		PointerInfo pi = MouseInfo::getPointerInfo();
		return pointRelativeToComponent(pi.getLocation());
	}
	virtual boolean isShowing() const;
	virtual boolean isEnabled() const { return enabled; }
	virtual void setEnabled(boolean b) { enabled = b; }
	virtual boolean isDoubleBuffered() const { return false; }
	virtual void enableInputMethods(boolean enable) {}
	virtual void setVisible(boolean b);
	virtual Color getForeground() const {
		return foreground;
	}

	virtual Container *getContainer() { return parent; }
	virtual Point getLocationOnScreen() const {
		if (peer != null && isShowing()) {
			Point pt;
			return pt;
		}
		throw IllegalComponentStateException("componentn not on screen");
	}

	virtual void validate() {
		synchronized (getTreeLock()) {
			ComponentPeer* peer = this->peer;
			boolean wasValid = isValid();
			if (!wasValid && peer != null) {
				peer->layout();
			}
			valid = true;
		}
	}
	virtual void invalidate() {
		synchronized (getTreeLock()) {
			valid = false;
			invalidateParent();
		}
	}
	virtual void revalidate() {
	}

	virtual Color getBackground() const;
	virtual void setBackground(const Color& c) {
        Color oldColor = background;
		ComponentPeer* peer = this->peer;
		background = c;
		if (peer != null) {
			Color cc = getBackground();
			if (cc != null) peer->setBackground(cc);
		}
		//firePropertyChange("background", oldColor, c);
	}
	virtual boolean isBackgroundSet() {
		return (background != null);
	}
	virtual Graphics& getGraphics();
	virtual Font& getFont();

	virtual void setLocation(int x, int y) {
		synchronized(getTreeLock()) {
			setBoundsOp(ComponentPeer::SET_LOCATION);
			setBounds(x, y, width, height);
		}
	}
	virtual void setIgnoreRepaint(boolean ignoreRepaint) { this->ignoreRepaint = ignoreRepaint; }
	virtual boolean getIgnoreRepaint() { return ignoreRepaint; }
	virtual boolean contains(int x, int y) {
		return (x >= 0) && (x < width) && (y >= 0) && (y < height);
	}
	virtual boolean contains(const Point& p) { return contains(p.x, p.y); }
	virtual Component* getComponentAt(int x, int y) { return contains(x, y) ? this : null; }
	virtual Component* getComponentAt(const Point& p) { return getComponentAt(p.x, p.y); }
	virtual boolean eventEnabled(AWTEvent& e) { return eventTypeEnabled(e.getID()); }
	virtual void dispatchEvent(AWTEvent& e) final { dispatchEventImpl(e); }
	virtual Dimension getSize() { return Dimension(width, height); }
	virtual void setSize(int width, int height) {
		synchronized(getTreeLock()) {
			setBoundsOp(ComponentPeer::SET_SIZE);
			setBounds(x, y, width, height);
		}
	}
	virtual Rectangle getBounds() { return Rectangle(x, y, width, height); }
	virtual void setBounds(int x, int y, int width, int height);
	virtual void setBounds(const Rectangle& r) { setBounds(r.x, r.y, r.width, r.height); }
	virtual int getX() { return x; }
	virtual int getY() { return y; }
	virtual int getWidth() { return width; }
	virtual int getHeight() { return height; }

	virtual void update(awt::Graphics& g) { paint(g); }

	virtual void paint(awt::Graphics& g) {}
	virtual void paintAll(Graphics& g) {}
	virtual void lightweightPaint(Graphics& g) {paint(g);}

	virtual void repaint() { repaint(0, 0, 0, width, height); }
	virtual void repaint(int x, int y, int width, int height) { repaint(0, x, y, width, height); }
	virtual void repaint(long tm, int x, int y, int width, int height);

	virtual void print(awt::Graphics& g) { paint(g); }
	virtual void printAll(Graphics& g) {}
	virtual void lightweightPrint(Graphics& g) {}

	virtual boolean requestFocusInWindow() {return false;}
	virtual void requestFocus() {}

	virtual void addNotify();
	virtual void removeNotify();
	virtual void updateZOrder() {
		//TODO
		//peer.setZOrder(getHWPeerAboveMe());
	}

	virtual boolean hasFocus();
	virtual boolean isFocusOwner() { return hasFocus(); }
	virtual void nextFocus() {transferFocus(false);}
	virtual void transferFocus() { nextFocus(); }
};

class Button : extends Component {
};


class Container : extends Component {
private:
	util::ArrayList<Component*> component;
public:
	boolean updateGraphicsData(GraphicsConfiguration& gc);
	virtual void validateUnconditionally() final {
	}
	Dimension getPreferredSize();
	void addNotify();

	void increaseComponentCount(Component *c) {
		synchronized (getTreeLock()) {
			if (!c->isDisplayable()) throw IllegalStateException("Peer does not exist while invoking the increaseComponentCount() method");
			//TODO
		}
	}

	virtual void paintComponents(Graphics& g) {}
	virtual void decreaseComponentCount(Component* c) final;
};

class WindowListener;
class Window : extends Container {
public:
	enum class Type {
		NORMAL,
		UTILITY,
		POPUP
	};
private:
	static const int OPENED = 0x01;
	static const boolean locationByPlatformProp = true;

	WindowListener* windowListener = null;
	Type type = Type::NORMAL;
	boolean beforeFirstShow = true;
	boolean isInShow = false;
	boolean locationByPlatform = locationByPlatformProp;
	int state = 0;

	Window(GraphicsConfiguration& gc) { init(gc); }
	GraphicsConfiguration& initGC(GraphicsConfiguration& gc);
	void init(GraphicsConfiguration& gc);
	void ownedInit(Window& owner);
	void setClientSize(int w, int h) {
		synchronized (getTreeLock()) {
			setBoundsOp(ComponentPeer::SET_CLIENT_SIZE);
			setBounds(x, y, w, h);
		}
	}
	void doDispose();

protected:
	Window() {init((GraphicsConfiguration&)null_obj);}
	void setGraphicsConfiguration(GraphicsConfiguration& gc);
	boolean isRecursivelyVisible() override { return visible; }
	void postWindowEvent(int id);

public:
	Window(const std::nullptr_t&) { init((GraphicsConfiguration&)null_obj); }
	~Window() { doDispose(); }
/*
	Window(Frame& owner) : Window(owner == null ? (GraphicsConfiguration&)null_obj : owner.getGraphicsConfiguration())  {
		ownedInit(owner);
	}
*/
	Window(Window& owner) : Window(owner == null ? (GraphicsConfiguration&)Object::null_obj : owner.getGraphicsConfiguration())  {
		ownedInit(owner);
	}
	Window(Window& owner, GraphicsConfiguration& gc) : Window(gc) {
		ownedInit(owner);
	}
	void setBounds(int x, int y, int width, int height) {
		synchronized (getTreeLock()) {
			if (getBoundsOp() == ComponentPeer::SET_LOCATION || getBoundsOp() == ComponentPeer::SET_BOUNDS) {
				locationByPlatform = false;
			}
			Container::setBounds(x, y, width, height);
		}
	}

	virtual void toFront();

	void setVisible(boolean b);
	void pack();
	void addNotify() override;
	void removeNotify() override;
	virtual void dispose() { doDispose(); }
	void updateZOrder() {}

	virtual Type getType() const { return type; }
	virtual void setType(const Type& type) { this->type=type; }

	virtual boolean isLocationByPlatform() { return locationByPlatform; }
};

class Frame : extends Window { //,implements MenuContainer {
private:
	String      title = "Untitled";
	boolean undecorated = false;

	void init(const String& title, GraphicsConfiguration& gc) {this->title=title;}

public:
	static const int NORMAL = 0;
	static const int ICONIFIED = 1;
	static const int MAXIMIZED_HORIZ = 2;
	static const int MAXIMIZED_VERT = 4;
	static const int MAXIMIZED_BOTH = MAXIMIZED_VERT | MAXIMIZED_HORIZ;

	Frame() : Frame("") {}
	Frame(GraphicsConfiguration& gc) : Frame("", gc) {}
	Frame(const String& title) { init(title, (GraphicsConfiguration&)null_obj); }
	Frame(const String& title, GraphicsConfiguration& gc) { init(title, gc); }

	void addNotify() override;
	void removeNotify() override;

	String getTitle() const { return title; }
	void setTitle(const String& title);

	void setUndecorated(boolean undecorated);
	boolean isUndecorated() { return undecorated; }
};

class LightweightFrame : extends Frame {
};

class Dialog : extends Window {
};

} //namespace awt

#endif
