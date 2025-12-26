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
    // 修正 1: 参数类型必须是 PluginInfoIndex
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case PI_Name: return L"喝水提醒";
            case PI_Description: return L"随机20-30分钟提醒";
            case PI_Author: return L"AI";
            case PI_Version: return L"1.0";
            default: return L"";
        }
    }

    // 修正 2: 确保 OnTimer 签名匹配
    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    // 修正 3: IPluginItem 相关接口，确保参数类型正确
    virtual int GetItemCount() const override { return 1; }
    virtual const wchar_t* GetItemName(int item_index) const override { return L"喝水计时"; }
    virtual const wchar_t* GetItemValueText(int item_index) override { return L""; }
    virtual const wchar_t* GetItemValueSampleText(int item_index) const override { return L""; }
    virtual int GetItemWidth(int item_index) override { return 0; }
    virtual void OnClick(int item_index) override {}
    virtual void OnContextMenu(int item_index, HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue(int item_index) override { return 0; }
    
    virtual void DataRequired() override {}
    
    // 修正 4: GetItem 的返回类型必须是 IPluginItem*
    virtual IPluginItem* GetItem(int item_index) override { return nullptr; }

    virtual void Release() override { delete this; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
