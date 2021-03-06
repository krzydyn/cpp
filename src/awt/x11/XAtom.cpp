#include "XAtom.hpp"
#include "XBaseWindow.hpp"
#include "XConstants.hpp"
#include "XToolkit.hpp"
#include "XlibWrapper.hpp"

namespace {
static util::HashMap<long, awt::x11::XAtom> atomToAtom;
static util::HashMap<String, awt::x11::XAtom> nameToAtom;

const awt::x11::XAtom& lookup(long atom) {
	return atomToAtom.get(atom);
}
const awt::x11::XAtom& lookup(const String& name) {
	if (!nameToAtom.containsKey(name)) return (const awt::x11::XAtom&)Object::null_obj;
	return nameToAtom.get(name);
}
}

namespace awt { namespace x11 {

void XAtom::registerAtom(const XAtom& at) {
	if (at.atom != 0) atomToAtom.put(at.atom, at);
	if (!at.name.isEmpty()) nameToAtom.put(at.name, at);
	LOGD("registered atom %s %ld", at.name.cstr(), at.atom);
}
long XAtom::getAtom(long ptr) {
	return *(long*)ptr;
}
XAtom XAtom::get(long atom) {
	const XAtom& xatom = lookup(atom);
	if (xatom == null) return XAtom(atom);
	return xatom;
}
XAtom XAtom::get(const String& name) {
	const XAtom& xatom = lookup(name);
	if (xatom == null) return XAtom(name, true);
	return xatom;
}

XAtom::XAtom(const String& name, boolean autoIntern) : name(name) {
	this->display = XToolkit::getDisplay();
	if (autoIntern) {
		XToolkit::awtLock();
		this->atom = XlibWrapper::InternAtom(display,name,0);
		XToolkit::awtUnlock();
	}
	registerAtom(*this);
}
XAtom::XAtom(long atom) : atom(atom) {
	this->display = XToolkit::getDisplay();
	registerAtom(*this);
}

String XAtom::getName() const {
	if (name.isEmpty()) {
		XToolkit::awtLock();
		const_cast<XAtom*>(this)->name = XlibWrapper::XGetAtomName(display, atom);
		XToolkit::awtUnlock();
	}
	return name;
}
void XAtom::setProperty(long window, const String& str) {
	if (atom == 0) throw IllegalStateException("Atom should be initialized");
	XToolkit::awtLock();
	XlibWrapper::SetProperty(display,window,atom,str);
	XToolkit::awtUnlock();
}
String XAtom::getProperty(long window) {
	if (atom == 0) throw IllegalStateException("Atom should be initialized");
	XToolkit::awtLock();
	String prop = XlibWrapper::GetProperty(display,window,atom);
	XToolkit::awtUnlock();
	return prop;
}

void XAtom::setCard32Property(long window, long value) {
	if (atom == 0) throw IllegalStateException("Atom should be initialized");
	XToolkit::awtLock();
	XlibWrapper::XChangeProperty(display,window,atom,XA_CARDINAL,32,XConstants::PropModeReplace,&value,1);
	XToolkit::awtUnlock();
}
long XAtom::getCard32Property(long window) {
	if (atom == 0) throw IllegalStateException("Atom should be initialized");
	XToolkit::awtLock();
	long prop = 0;
	XToolkit::awtUnlock();
	return prop;
}

void XAtom::deleteProperty(long window) {
	if (atom == 0) throw IllegalStateException("Atom should be initialized");
	XToolkit::awtLock();
	XlibWrapper::XDeleteProperty(display,window,atom);
	XToolkit::awtUnlock();
}

void XAtom::setAtomListProperty(long window, const Array<XAtom>& atoms) {
}
void XAtom::setAtomListProperty(long window, const XAtomList& atoms) {
}
void XAtom::setAtomListProperty(XBaseWindow* window, const Array<XAtom>& atoms) {
	setAtomListProperty(window->getWindow(), atoms);
}
void XAtom::setAtomListProperty(XBaseWindow* window, const XAtomList& atoms) {
	setAtomListProperty(window->getWindow(), atoms);
}


void XAtomList::init(long data, int count) {
	for (int i = 0; i < count; i++) {
		add(XAtom(XAtom::getAtom(data+count*XAtom::getAtomSize())));
	}
}
void XAtomList::init(const Array<XAtom>& atoms) {
	for (int i=0; i < atoms.length; ++i)
		add(atoms[i]);
}

}}
