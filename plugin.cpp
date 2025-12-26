#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h"

// 1. 实现具体的显示项目 (Item)
class CDrinkWaterItem : public IPluginItem {
public:
    // 注意：请核对 ITMPlugin.h 中这些函数是否带有 const
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemValueText() override { return L""; }
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }
    virtual bool IsShownByDefault() override { return true; }
    virtual int GetItemWidth() override { return 0; }
    
    // 以下函数必须存在于 Item 类中
    virtual void OnClick() override {}
    virtual void OnContextMenu(HWND hWnd, int x, int y) override {}
    
    // 如果 SDK 要求 GetStaticValue，请添加
    virtual const wchar_t* GetStaticValue() override { return L""; }
};

// 2. 实现插件主类 (Plugin)
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
    // 这里的 index 通常用于返回不同的显示项
    virtual IPluginItem* GetItem(int index) override { return &m_item; }
    
    // 注意：核对基类 GetItemCount 是否有 const
    virtual int GetItemCount() const override { return 1; }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case PluginInfoIndex::Name: return L"喝水提醒";
            case PluginInfoIndex::Description: return L"随机20-30分钟提醒";
            case PluginInfoIndex::Author: return L"AI Assistant";
            case PluginInfoIndex::Version: return L"1.0";
            default: return L"";
        }
    }

    // 核心：处理定时逻辑
    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            // 注意：MessageBox 会阻塞线程，建议在实际使用中改用异步或通知
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    virtual void Release() override { delete this; }
    
    // 根据具体 SDK 版本，可能还需要实现以下函数：
    virtual void PrepareData() override {} 
};

// 导出函数
extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
