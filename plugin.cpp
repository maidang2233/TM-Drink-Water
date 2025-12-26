#include <windows.h>
#include <random>
#include <ctime>
#include <string>
#include "ITMPlugin.h"

// 1. 实现显示项目类
class CDrinkWaterItem : public IPluginItem {
public:
    // 获取项目名称（插件列表中显示）
    virtual const wchar_t* GetItemName() const override { return L"喝水提醒"; }

    // 获取项目的唯一ID
    virtual const wchar_t* GetItemId() const override { return L"drink_water_reminder_item"; }

    // 获取项目在任务栏显示的标签（L前缀确保宽字符）
    virtual const wchar_t* GetItemLableText() const override { return L"喝水"; }

    // 获取显示的数值（提醒类插件通常数值为空）
    virtual const wchar_t* GetItemValueText() const override { return L""; }

    // 示例文本（用于计算显示宽度）
    virtual const wchar_t* GetItemValueSampleText() const override { return L""; }

    // 鼠标点击事件：点击任务栏图标弹出强制提醒
    virtual int OnMouseEvent(MouseEventType type, int x, int y, void* hWnd, int flag) override {
        if (type == MT_LCLICKED) {
            MessageBoxW((HWND)hWnd, L"多喝水有益健康！身体是革命的本钱。", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            return 1; // 返回1表示已处理该事件
        }
        return 0;
    }
};

// 2. 实现插件主类
class CDrinkWaterPlugin : public ITMPlugin {
private:
    CDrinkWaterItem m_item;
    ITrafficMonitor* m_pApp = nullptr;
    std::time_t m_last_notify_time = 0; // 上次提醒的时间戳
    int m_next_interval = 1200;         // 下次提醒间隔（秒）

    // 生成20-30分钟之间的随机秒数
    int GetRandomInterval() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1200, 1800);
        return dis(gen);
    }

public:
    // 返回插件包含的项目数量
    virtual IPluginItem* GetItem(int index) override {
        if (index == 0) return &m_item;
        return nullptr;
    }

    // 主程序初始化时调用，保存接口指针
    virtual void OnInitialize(ITrafficMonitor* pApp) override {
        m_pApp = pApp;
        m_last_notify_time = std::time(nullptr); // 启动时初始化时间
        m_next_interval = GetRandomInterval();
    }

    // 核心逻辑：主程序定期调用此函数
    virtual void DataRequired() override {
        std::time_t now = std::time(nullptr);
        
        // 检查是否到达提醒时间
        if (now - m_last_notify_time >= m_next_interval) {
            m_last_notify_time = now;
            m_next_interval = GetRandomInterval();

            // 优先使用主程序通知栏，避免阻塞界面
            if (m_pApp) {
                m_pApp->ShowNotifyMessage(L"该喝水了！喝口水休息一下吧。");
            }
        }
    }

    // 获取插件元信息
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override {
        switch (index) {
            case TMI_NAME: return L"喝水提醒插件";
            case TMI_DESCRIPTION: return L"每隔20-30分钟提醒您补充水分。";
            case TMI_AUTHOR: return L"AI Assistant";
            case TMI_VERSION: return L"1.1";
            case TMI_COPYRIGHT: return L"Copyright (C) 2025";
            default: return L"";
        }
    }

    // 提示信息
    virtual const wchar_t* GetTooltipInfo() override { return L"点击可手动弹出提醒"; }
};

// 3. 导出接口函数（必须是这个名字，且返回静态对象指针）
extern "C" __declspec(dllexport) ITMPlugin* TMPluginGetInstance() {
    static CDrinkWaterPlugin instance;
    return &instance;
}
