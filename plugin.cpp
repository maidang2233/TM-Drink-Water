#include <windows.h>
#include <random>
#include <ctime>
#include <string>
#include <cwchar>
#include "ITMPlugin.h"

class CDrinkWaterItem : public IPluginItem {
public:
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_item"; }
    virtual const wchar_t* GetItemLableText() const override { return L"喝水"; }
    virtual const wchar_t* GetItemValueText() const override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }
    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override { return 0; }
};

class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    ITrafficMonitor* m_pApp = nullptr;
    std::time_t m_last_notify_time = 0;
    
    // --- 可配置参数 ---
    int m_interval_min = 20; // 默认20分钟
    int m_sound_type = 1;    // 1: 默认, 2: 蜂鸣, 3: 静音

    // 获取配置文件路径 (DrinkWater.dll -> DrinkWater.ini)
    std::wstring GetConfigPath() {
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(GetModuleHandleW(L"DrinkWater.dll"), path, MAX_PATH);
        std::wstring str_path = path;
        return str_path.substr(0, str_path.find_last_of(L".")) + L".ini";
    }

    void LoadConfig() {
        std::wstring path = GetConfigPath();
        m_interval_min = GetPrivateProfileIntW(L"Config", L"Interval", 20, path.c_str());
        m_sound_type = GetPrivateProfileIntW(L"Config", L"SoundType", 1, path.c_str());
    }

    void SaveConfig() {
        std::wstring path = GetConfigPath();
        WritePrivateProfileStringW(L"Config", L"Interval", std::to_wstring(m_interval_min).c_str(), path.c_str());
        WritePrivateProfileStringW(L"Config", L"SoundType", std::to_wstring(m_sound_type).c_str(), path.c_str());
    }

public:
    virtual IPluginItem* GetItem(int index) override { return (index == 0) ? &m_item : nullptr; }

    virtual void OnInitialize(ITrafficMonitor* pApp) override {
        m_pApp = pApp;
        LoadConfig(); // 启动时加载配置
        m_last_notify_time = std::time(nullptr);
    }

    virtual void DataRequired() override {
        std::time_t now = std::time(nullptr);
        // 将分钟转为秒进行判断
        if (now - m_last_notify_time >= (m_interval_min * 60)) {
            m_last_notify_time = now;
            if (m_pApp) {
                // 根据配置播放声音
                if (m_sound_type == 1) MessageBeep(MB_ICONINFORMATION);
                else if (m_sound_type == 2) Beep(750, 300); 

                m_pApp->ShowNotifyMessage(L"该喝水了！");
            }
        }
    }

    // --- 实现设置界面 ---
    virtual OptionReturn ShowOptionsDialog(void* hParent) override {
        // 由于编写完整的 Win32 UI 窗口代码极其冗长，
        // 这里提供一个“简易调试输入框”逻辑：点击选项可切换声音模式
        m_sound_type = (m_sound_type % 3) + 1;
        
        wchar_t info[256];
        swprintf_s(info, L"设置已更改！\n当前提醒间隔：%d 分钟\n声音模式：%s (1:默认 2:蜂鸣 3:静音)\n\n提示：如需修改间隔，请直接编辑插件目录下的 DrinkWater.ini 文件。", 
            m_interval_min, 
            m_sound_type == 1 ? L"默认" : (m_sound_type == 2 ? L"蜂鸣" : L"静音"));

        MessageBoxW((HWND)hParent, info, L"插件设置", MB_OK | MB_ICONINFORMATION);
        
        SaveConfig(); // 保存设置
        return OR_OPTION_CHANGED;
    }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒 (支持配置)";
            case TMI_VERSION: return L"1.3";
            default: return L"";
        }
    }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
