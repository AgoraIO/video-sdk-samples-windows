#pragma once
#ifndef IEVENT_HANDLER_STRATEGY_H
#define IEVENT_HANDLER_STRATEGY_H

class IEventHandlerStrategy : public IRtcEngineEventHandler
{
public:
    virtual ~IEventHandlerStrategy() = default;
    
    void SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }
    HWND getMsgEventHandler() { return m_hMsgHandler; }

private:
   HWND m_hMsgHandler;
};

#endif // IEVENT_HANDLER_STRATEGY_H