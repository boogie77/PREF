#ifndef PREFSDK_UNICODESTRING_H
#define PREFSDK_UNICODESTRING_H

#include "stringfield.h"

namespace PrefSDK
{
    class UnicodeString : public StringField
    {
        Q_OBJECT

        public:
            explicit UnicodeString(lua_State* l, lua_Integer offset, QString name, quint64 itemcount, ByteBuffer* bytebuffer, LuaCTable* model, FormatElement* formatobject, QObject *parent = 0);

        public: /* Overriden methods */
            virtual QString displayType();
            virtual lua_Integer size();
    };
}

#endif // PREFSDK_UNICODESTRING_H
