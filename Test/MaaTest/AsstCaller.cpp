#include "AsstCaller.h"

#include <cstring>
#include <filesystem>
#include <iostream>

#include <meojson/json.hpp>

#include "Assistant.h"
#include "Common/AsstTypes.h"
#include "Common/AsstVersion.h"
//#include "Config/ResourceLoader.h"
#include "Utils/Logger.hpp"
#include "Utils/WorkingDir.hpp"

static constexpr AsstSize NullSize = static_cast<AsstSize>(-1);
static constexpr AsstId InvalidId = 0;
static constexpr AsstBool AsstTrue = 1;
static constexpr AsstBool AsstFalse = 0;


AsstHandle AsstCreate()
{
    /*if (!inited()) {
        return nullptr;
    }*/
    return new asst::Assistant();
}


AsstBool AsstStart(AsstHandle handle)
{
    /*if (!inited() || handle == nullptr) {
        return AsstFalse;
    }*/

    return handle->start() ? AsstTrue : AsstFalse;
}

AsstBool AsstStop(AsstHandle handle)
{
   /* if (!inited() || handle == nullptr) {
        return AsstFalse;
    }*/

    return handle->stop() ? AsstTrue : AsstFalse;
}

AsstBool AsstRunning(AsstHandle handle)
{
    /*if (!inited() || handle == nullptr) {
        return AsstFalse;
    }*/

    return handle->running() ? AsstTrue : AsstFalse;
}