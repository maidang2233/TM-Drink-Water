#include <windows.h>
#include <random>
#include <ctime>
#include <string>
#include <cwchar>
#include "ITMPlugin.h"

// 1. 实现项目类
class CDrinkWaterItem : public IPluginItem {
public:
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_item"; }
    virtual const wchar_t* GetItemLableText() const override { return L"喝水"; }
    virtual const wchar_t* GetItemValueText() const override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }

    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override {
        if (type == MT_LCLICKED) {
            MessageBeep(MB_ICONASTERISK); // 点击时发出声音
            MessageBoxW((HWND)hWnd, L"多喝水有益健康！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            return 1;
        }
        return 0;
    }
};

// 2. 实现插件类
class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    ITrafficMonitor* m_pApp = nullptr;
    std::time_t m_last_notify_time = 0;
    int m_next_interval = 1200;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800);
        return dis(gen);
    }

public:
    virtual IPluginItem* GetItem(int index) override { return (index == 0) ? &m_item : nullptr; }
    virtual int GetItemCount() const { return 1; } // 确保主程序知道有一个显示项

    virtual void OnInitialize(ITrafficMonitor* pApp) override {
        m_pApp = pApp;
        m_last_notify_time = std::time(nullptr);
        m_next_interval = GetRandomInterval();
    }

    virtual void DataRequired() override {
        std::time_t now = std::time(nullptr);
        if (now - m_last_notify_time >= m_next_interval) {
            m_last_notify_time = now;
            m_next_interval = GetRandomInterval();

            if (m_pApp) {
                MessageBeep(MB_ICONINFORMATION); // 核心功能：发出系统提示音
                m_pApp->ShowNotifyMessage(L"该喝水了！喝口水休息一下吧。");
            }
        }
    }

    // --- 核心修改：实现选项功能 ---
    virtual OptionReturn ShowOptionsDialog(void* hParent) override {
        std::time_t now = std::time(nullptr);
        int elapsed = (int)(now - m_last_notify_time);
        int remaining = m_next_interval - elapsed;

        wchar_t debug_info[512];
        swprintf_s(debug_info, 
            L"【插件调试控制台】\n\n"
            L"运行状态：正在监控\n"
            L"本次提醒间隔：%d 秒\n"
            L"已等待时间：%d 秒\n"
            L"距离下次提醒还剩：%d 秒\n\n"
            L"提示：点击确定将重置提醒计时器（用于调试）。", 
            m_next_interval, elapsed, (remaining > 0 ? remaining : 0));

        // 弹出一个 Win32 消息框作为简单的“选项”界面
        int result = MessageBoxW((HWND)hParent, debug_info, L"喝水提醒 选项", MB_OKCANCEL | MB_ICONINFORMATION);
        
        if (result == IDOK) {
            // 调试功能：点击确定立即重置计时
            m_last_notify_time = std::time(nullptr);
            return OR_OPTION_CHANGED; // 告知主程序配置已更改
        }
        
        return OR_OPTION_UNCHANGED;
    }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒 (带声音调试版)";
            case TMI_AUTHOR: return L"AI Assistant";
            case TMI_VERSION: return L"1.2";
            case TMI_DESCRIPTION: return L"支持声音提醒与实时调试查看";
            default: return L"";
        }
    }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
