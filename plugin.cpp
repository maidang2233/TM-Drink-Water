#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h"

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
    // 修正：不再强求枚举类型，直接根据日志提示使用 index
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        if (index == PI_Name) return L"喝水提醒";
        if (index == PI_Description) return L"随机20-30分钟提醒";
        if (index == PI_Author) return L"AI";
        if (index == PI_Version) return L"1.0";
        return L"";
    }

    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    // 移除所有 const 修饰符和 ITMPlugin:: 前缀，以匹配最基础的接口定义
    virtual int GetItemCount() override { return 1; }
    virtual const wchar_t* GetItemName(int item_index) override { return L"喝水计时"; }
    virtual const wchar_t* GetItemValueText(int item_index) override { return L""; }
    virtual const wchar_t* GetItemValueSampleText(int item_index) override { return L""; }
    virtual int GetItemWidth(int item_index) override { return 0; }
    virtual void OnClick(int item_index) override {}
    virtual void OnContextMenu(int item_index, HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue(int item_index) override { return 0; }
    
    virtual void DataRequired() override {}
    
    // 修正：根据日志，IPluginItem 是独立定义的，不再使用 ITMPlugin:: 前缀
    virtual IPluginItem* GetItem(int item_index) override { return nullptr; }

    virtual void Release() override { delete this; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
