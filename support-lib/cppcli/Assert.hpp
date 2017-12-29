#pragma once

#ifdef _DEBUG
#define ASSERT(condition, ...) ::System::Diagnostics::Debug::Assert(condition, ##__VA_ARGS__)
#else
#define ASSERT(condition, ...) // This macro will completely evaporate in Release.
#endif
