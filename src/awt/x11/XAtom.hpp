#ifndef __AWT_X11_XATOM_HPP
#define __AWT_X11_XATOM_HPP

#include <lang/String.hpp>

namespace awt { namespace x11 {

class XAtom final : extends Object {
private:
	long atom = 0;
	String name;
	long display;
	void registerAtom(const XAtom& at);

public:
	static const long XA_CARDINAL=6;
	static XAtom get(long atom);
	static XAtom get(const String& name);

	XAtom(const XAtom& o) : atom(o.atom), name(o.name), display(o.display) {}
 	XAtom() {}
	XAtom(long atom);
	XAtom(const String& name, boolean autoIntern);

	boolean isSet() {return atom!=0;}

	String getName() const;

	void setProperty(long window, const String& str);
	String getProperty(long window);

	void setCard32Property(long window, long value);
	long getCard32Property(long window);

	void deleteProperty(long window);
};

}}

#endif
