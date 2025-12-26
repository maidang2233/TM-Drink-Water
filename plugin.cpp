#include <windows.h>
#include <random>
#include <ctime>
#include "ITMPlugin.h"

class CDrinkWaterItem : public IPluginItem {
public:
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_id"; }
    virtual const wchar_t* GetItemLableText() const override { return L"喝水"; }
    virtual const wchar_t* GetItemValueText() const override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }

    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override {
        if (type == MT_LCLICKED) {
            MessageBoxW((HWND)hWnd, L"多喝水有益健康！", L"提醒", MB_OK | MB_ICONINFORMATION);
            return 1;
        }
        return 0;
    }
};

class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    ITrafficMonitor* m_pApp = nullptr;
    std::time_t m_last_notify_time = std::time(nullptr);
    int m_next_interval = 1200;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800);
        return dis(gen);
    }

public:
    virtual IPluginItem* GetItem(int index) override { return (index == 0) ? &m_item : nullptr; }
    virtual void DataRequired() override {
        std::time_t now = std::time(nullptr);
        if (now - m_last_notify_time >= m_next_interval) {
            m_last_notify_time = now;
            m_next_interval = GetRandomInterval();
            if (m_pApp) m_pApp->ShowNotifyMessage(L"该喝水了！");
        }
    }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒";
            case TMI_AUTHOR: return L"AI Assistant";
            case TMI_VERSION: return L"1.0";
            default: return L"";
        }
    }

    virtual void OnInitialize(ITrafficMonitor* pApp) override { m_pApp = pApp; }
    virtual void Release() override { /* 静态对象无需手动 delete */ }
};

// 关键修改：函数名必须是 TMPluginGetInstance
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
