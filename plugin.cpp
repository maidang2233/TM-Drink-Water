#include <windows.h>
#include <random>
#include <ctime>
#include <string>
#include <cwchar>
#include <vector>
#include "ITMPlugin.h"

class CDrinkWaterItem : public IPluginItem {
public:
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_item"; }
    virtual const wchar_t* GetItemLableText() const override { return L"喝水"; }
    virtual const wchar_t* GetItemValueText() const override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }

    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override {
        if (type == MT_LCLICKED) {
            return 1;
        }
        return 0;
    }
};

class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    ITrafficMonitor* m_pApp = nullptr;
    
    // 配置变量
    std::time_t m_last_notify_time = 0;
    int m_config_interval_min = 20; // 默认20分钟
    UINT m_selected_sound = MB_ICONINFORMATION; // 默认声音类型
    int m_current_actual_interval = 1200; // 当前运行的秒数

    void ResetTimer() {
        m_last_notify_time = std::time(nullptr);
        // 根据用户设置的分钟数，生成一个固定的秒数（你也可以继续保持随机逻辑）
        m_current_actual_interval = m_config_interval_min * 60;
    }

public:
    virtual IPluginItem* GetItem(int index) override { return (index == 0) ? &m_item : nullptr; }

    virtual void OnInitialize(ITrafficMonitor* pApp) override {
        m_pApp = pApp;
        ResetTimer();
    }

    virtual void DataRequired() override {
        std::time_t now = std::time(nullptr);
        if (now - m_last_notify_time >= m_current_actual_interval) {
            if (m_pApp) {
                MessageBeep(m_selected_sound); // 播放用户选中的声音
                m_pApp->ShowNotifyMessage(L"该喝水了！");
            }
            m_last_notify_time = now;
        }
    }

    // --- 核心修改：实现带设置功能的选项对话框 ---
    virtual OptionReturn ShowOptionsDialog(void* hParent) override {
        // 选项 1：修改声音
        int sound_choice = MessageBoxW((HWND)hParent, 
            L"是否更改提醒声音？\n\n[是]：切换为 叮声 (Asterisk)\n[否]：切换为 警告声 (Exclamation)\n[取消]：保持不变", 
            L"设置 - 声音选择", MB_YESNOCANCEL | MB_ICONQUESTION);
        
        if (sound_choice == IDYES) m_selected_sound = MB_ICONASTERISK;
        if (sound_choice == IDNO) m_selected_sound = MB_ICONEXCLAMATION;

        // 选项 2：修改间隔（演示简易切换）
        int time_choice = MessageBoxW((HWND)hParent, 
            L"设置提醒间隔：\n\n[是]：设为 30分钟\n[否]：设为 60分钟\n[取消]：保持当前设置", 
            L"设置 - 间隔调整", MB_YESNOCANCEL | MB_ICONQUESTION);

        if (time_choice == IDYES) m_config_interval_min = 30;
        if (time_choice == IDNO) m_config_interval_min = 60;

        if (sound_choice != IDCANCEL || time_choice != IDCANCEL) {
            ResetTimer();
            MessageBoxW((HWND)hParent, L"设置已生效并重置计时器！", L"成功", MB_OK | MB_ICONINFORMATION);
            return OR_OPTION_CHANGED;
        }

        return OR_OPTION_UNCHANGED;
    }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒 Pro";
            case TMI_VERSION: return L"1.3";
            case TMI_DESCRIPTION: return L"支持声音与间隔自定义设置";
            default: return L"";
        }
    }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
