#ifndef PREFSDK_PROCESSORDEFINITION_H
#define PREFSDK_PROCESSORDEFINITION_H

#include "prefsdk/debug/debugobject.h"
#include "prefsdk/disassembler/disassemblerlisting.h"
#include "prefsdk/disassembler/blocks/instruction.h"
#include "prefsdk/disassembler/instruction/instructionset.h"
#include "prefsdk/disassembler/register/registerset.h"
#include "prefsdk/disassembler/processor/processoremulator.h"
#include <QtCore>

namespace PrefSDK
{
    class ProcessorDefinition : public DebugObject
    {
        Q_OBJECT

        Q_PROPERTY(PrefSDK::QtLua::LuaFunction analyze READ analyze WRITE setAnalyze)
        Q_PROPERTY(PrefSDK::QtLua::LuaFunction emulate READ emulate WRITE setEmulate)
        Q_PROPERTY(PrefSDK::QtLua::LuaFunction elaborate READ elaborate WRITE setElaborate)

        public:
            explicit ProcessorDefinition(InstructionSet* instructionset, RegisterSet* registerset, DataType::Type addresstype, QObject *parent = 0);
            DataType::Type addressType() const;
            const PrefSDK::QtLua::LuaFunction& analyze() const;
            const PrefSDK::QtLua::LuaFunction& emulate() const;
            const PrefSDK::QtLua::LuaFunction& elaborate() const;
            void setAnalyze(const PrefSDK::QtLua::LuaFunction& af);
            void setEmulate(const PrefSDK::QtLua::LuaFunction& ef);
            void setElaborate(const PrefSDK::QtLua::LuaFunction& ef);
            lua_Integer callAnalyze(Instruction* instruction, const DataValue &baseaddress, QHexEditData* hexeditdata);
            void callEmulate(ProcessorEmulator* emulator, Instruction* instruction, QHexEditData* hexeditdata);
            void callElaborate(DisassemblerListing* listing, QHexEditData *hexeditdata);

        public:
            const InstructionSet* instructionSet() const;
            const RegisterSet* registerSet() const;
            void decode(Instruction* instruction);

        private:
            PrefSDK::QtLua::LuaFunction _analyzefunc;
            PrefSDK::QtLua::LuaFunction _emulatefunc;
            PrefSDK::QtLua::LuaFunction _elaboratefunc;
            InstructionSet* _instructionset;
            RegisterSet* _registerset;
            DataType::Type _addresstype;
    };
}

#endif // PREFSDK_PROCESSORDEFINITION_H