#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h"

// 如果头文件里没定义，我们手动定义这些索引常量
#ifndef PI_Name
#define PI_Name (ITMPlugin::PluginInfoIndex)0
#define PI_Description (ITMPlugin::PluginInfoIndex)1
#define PI_Author (ITMPlugin::PluginInfoIndex)2
#define PI_Version (ITMPlugin::PluginInfoIndex)3
#endif

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
    // 修正：添加 const
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        if (index == PI_Name) return L"喝水提醒";
        if (index == PI_Description) return L"随机20-30分钟提醒";
        if (index == PI_Author) return L"AI Assistant";
        if (index == PI_Version) return L"1.0";
        return L"";
    }

    // 修正：某些版本 OnTimer 需要接收参数，我们改为最基础的尝试
    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    // 修正：根据最新接口要求，补齐 const
    virtual int GetItemCount() const override { return 1; }
    virtual const wchar_t* GetItemName(int item_index) const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemValueText(int item_index) override { return L""; }
    virtual const wchar_t* GetItemValueSampleText(int item_index) const override { return L""; }
    virtual int GetItemWidth(int item_index) override { return 0; }
    virtual void OnClick(int item_index) override {}
    virtual void OnContextMenu(int item_index, HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue(int item_index) override { return 0; }
    
    virtual void DataRequired() override {}
    virtual IPluginItem* GetItem(int item_index) override { return nullptr; }
    virtual void Release() override { delete this; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
