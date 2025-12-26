#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h" // 确保你的仓库里有一个叫 ITMPlugin.h 的文件

class CDrinkWaterPlugin : public ITMPlugin {
private:
    int m_counter = 0;
    int m_next_interval = 1200;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800);
        return dis(gen);
    }

public:
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case PI_Name: return L"喝水提醒";
            case PI_Description: return L"随机20-30分钟提醒";
            case PI_Author: return L"AI";
            case PI_Version: return L"1.0";
            default: return L"";
        }
    }

    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            // 弹出置顶提醒
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    virtual const wchar_t* GetItemName() override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemValueText() override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() override { return L""; }
    virtual void Release() override { delete this; }
    
    // 补全可能需要的虚函数
    virtual int GetItemWidth() override { return 0; }
    virtual void OnClick() override {}
    virtual void OnContextMenu(HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue() override { return 0; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
