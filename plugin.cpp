#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h"

// 实现具体的显示项目
class CDrinkWaterItem : public IPluginItem {
public:
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemValueText() override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }
    virtual bool IsShownByDefault() override { return true; }
    virtual int GetItemWidth() override { return 0; }
    virtual void OnClick() override {}
    virtual void OnContextMenu(HWND hWnd, int x, int y) override {}
};

// 实现插件主类
class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    int m_counter = 0;
    int m_next_interval = 1200;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800);
        return dis(gen);
    }

public:
    virtual IPluginItem* GetItem(int index) override { return &m_item; }
    virtual int GetItemCount() const override { return 1; }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        // 使用新版枚举命名空间
        switch (index) {
            case PluginInfoIndex::Name: return L"喝水提醒";
            case PluginInfoIndex::Description: return L"随机20-30分钟提醒";
            case PluginInfoIndex::Author: return L"AI Assistant";
            case PluginInfoIndex::Version: return L"1.0";
            default: return L"";
        }
    }

    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    virtual void DataRequired() override {}
    virtual void Release() override { delete this; }
};

// 导出函数
extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
