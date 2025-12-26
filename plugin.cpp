#include <windows.h>
#include <random>
#include "ITMPlugin.h"

class CDrinkWaterPlugin : public ITMPlugin {
private:
    int m_counter = 0;
    int m_next_interval = 1200;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800); // 20-30分钟
        return dis(gen);
    }

public:
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case PI_Name: return L"喝水提醒器";
            case PI_Description: return L"随机20-30分钟提醒喝水";
            case PI_Author: return L"AI Assistant";
            default: return L"";
        }
    }

    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            // 使用异步弹窗，防止卡住主程序
            std::wstring msg = L"喝口水吧！下次提醒在 " + std::to_wstring(m_next_interval / 60) + L" 分钟后。";
            MessageBoxW(NULL, msg.c_str(), L"提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    virtual const wchar_t* GetItemName() override { return L"下一次提醒(秒)"; }
    virtual const wchar_t* GetItemValueText() override { 
        static wchar_t buf[16];
        swprintf_s(buf, L"%ds", m_next_interval - m_counter);
        return buf; 
    }
    virtual const wchar_t* GetItemValueSampleText() override { return L"1800s"; }
    virtual void Release() override { delete this; }

    // 必须实现的空接口
    virtual int GetItemWidth() override { return 0; }
    virtual void OnClick() override {}
    virtual void OnContextMenu(HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue() override { return 0; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}