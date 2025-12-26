#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h"

// 1. 实现具体的显示项目
class CDrinkWaterItem : public IPluginItem {
public:
    // 必须匹配基类的 const 属性
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_id"; } // 必须实现
    virtual const wchar_t* GetItemLableText() const override { return L"提醒"; } // 必须实现
    virtual const wchar_t* GetItemValueText() const override { return L""; } // 注意：基类这里有 const
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }

    // 鼠标事件处理 (替代了你之前的 OnClick 和 OnContextMenu)
    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override {
        if (type == MT_LCLICKED) {
            MessageBoxW((HWND)hWnd, L"多喝水有益健康！", L"提醒", MB_OK | MB_ICONINFORMATION);
            return 1; // 表示已处理
        }
        return 0;
    }
};

// 2. 实现插件主类
class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    int m_counter = 0;
    int m_next_interval = 1200; // 20分钟 (假设DataRequired每秒调用一次)
    ITrafficMonitor* m_pApp = nullptr;

    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800); // 20-30分钟
        return dis(gen);
    }

public:
    // 注意：基类 GetItem 没有 const
    virtual IPluginItem* GetItem(int index) override {
        if (index == 0) return &m_item;
        return nullptr;
    }

    // 核心逻辑：DataRequired 会被主程序定期调用
    virtual void DataRequired() override {
        m_counter++;
        if (m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            
            // 使用主程序提供的 ShowNotifyMessage 接口，不会卡死界面
            if (m_pApp != nullptr && m_pApp->GetAPIVersion() >= 1) {
                m_pApp->ShowNotifyMessage(L"该喝水了！");
            } else {
                // 退而求其次使用系统弹窗（注意：这会阻塞当前线程）
                MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            }
        }
    }

    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒";
            case TMI_DESCRIPTION: return L"随机20-30分钟提醒喝水";
            case TMI_AUTHOR: return L"AI Assistant";
            case TMI_VERSION: return L"1.0";
            default: return L"";
        }
    }

    // 保存主程序接口指针
    virtual void OnInitialize(ITrafficMonitor* pApp) override {
        m_pApp = pApp;
    }

    // 根据 SDK 建议，不需要 delete this，直接使用静态对象
};

// 导出函数
// 根据 SDK 说明，应返回全局/静态对象的指针
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
