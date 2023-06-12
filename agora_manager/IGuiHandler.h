// igui_handler.h

#pragma once


#ifndef IGU_HANDLER_H
#define IGU_HANDLER_H

class IGuiHandler {
public:
    virtual ~IGuiHandler() = default;

    virtual void handleGuiAction(int commandId) = 0;
    virtual void handleGuiUserMsg(int msgid, WPARAM wparam, LPARAM lparam) = 0;

};

#endif // IGU_HANDLER_H