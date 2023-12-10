#pragma once
#pragma once

#include "AsstPort.h"
#include <stdint.h>

struct AsstExtAPI;
typedef struct AsstExtAPI* AsstHandle;

typedef uint8_t AsstBool;
typedef uint64_t AsstSize;

typedef int32_t AsstId;
typedef AsstId AsstMsgId;
typedef AsstId AsstTaskId;
typedef AsstId AsstAsyncCallId;

typedef int32_t AsstOptionKey;
typedef AsstOptionKey AsstStaticOptionKey;
typedef AsstOptionKey AsstInstanceOptionKey;

typedef void(ASST_CALL* AsstApiCallback)(AsstMsgId msg, const char* details_json, void* custom_arg);

#ifdef __cplusplus
extern "C"
{
#endif

    AsstHandle ASSTAPI AsstCreate();
    AsstBool ASSTAPI AsstStart(AsstHandle handle);
    AsstBool ASSTAPI AsstStop(AsstHandle handle);
    AsstBool ASSTAPI AsstRunning(AsstHandle handle);


#ifdef __cplusplus
}
#endif
