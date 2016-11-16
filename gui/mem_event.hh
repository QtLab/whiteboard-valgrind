#pragma once

#include <QDebug>

namespace Whiteboard {

struct MemEvent
{
	enum Type { LOAD, STORE };
	Type type;
	quint64 addr;
	int size;
	quint64 data;
};

struct HeapEvent
{
	enum Type { ALLOC, FREE };
	Type type;
	quint64 addr;
	int size;
	QString call;
};


inline QDebug operator<<(QDebug s, const MemEvent& e)
{
	s << "Mem " << (e.type == MemEvent::STORE ? "store" : "load");
	s << " " << e.size << "@" << e.addr;
	return s;
}

inline QDebug operator<<(QDebug s, const HeapEvent& h)
{
	s << "Heap " << (h.type == HeapEvent::ALLOC ? "allocation" : "free");
	s << h.size << "@" << h.addr << ", type: " << h.type;
	return s;
}


}
