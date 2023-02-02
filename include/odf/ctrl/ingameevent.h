#ifndef ODF_CTRL_IN_GAME_EVENT_H
#define ODF_CTRL_IN_GAME_EVENT_H

#include <stdint.h>
#include "odf/ctrl/types/ingameevent.h"
#include "odf/sys/list.h"

void ctrlInGameEventRegister(CtrlActionType type, CtrlAction action, float value);

/**
 * Transfers ownership of registered events.
 * Once used, the list, and all it's content should be free'd
 * with ctrlInGameEventReleaseAll()
 **/
ListOf(CtrlEvent*)* ctrlInGameEventFetchAll();

void ctrlInGameEventReleaseAll(ListOf(CtrlEvent*)* events);

const char* ctrlGetCtrlActionTypeName(CtrlActionType type);
const char* ctrlGetCtrlActionName(CtrlAction action);

#endif
