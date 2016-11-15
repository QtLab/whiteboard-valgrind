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


inline QDebug operator<<(QDebug s, const MemEvent& e)
{
	s << "Mem " << (e.type == MemEvent::STORE ? "store" : "load");
	s << " " << e.size << "@" << e.addr;
	return s;
}


}
