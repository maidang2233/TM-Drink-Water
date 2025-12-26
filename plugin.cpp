#include <windows.h>
#include <random>
#include <string>
#include "ITMPlugin.h" // 假设你已将 PluginInterface.h 重命名为 ITMPlugin.h

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
    // 对应报错中提到的 PI_Name 等未定义，这里使用整数索引或新版定义的常量
    virtual const wchar_t* GetInfo(int index) override {
        switch (index) {
            case 0: return L"喝水提醒";      // Name
            case 1: return L"随机20-30分钟提醒"; // Description
            case 2: return L"AI";            // Author
            case 3: return L"1.0";           // Version
            default: return L"";
        }
    }

    // 新版接口的 OnTimer 可能没有参数，或者参数类型已变
    virtual void OnTimer() override {
        if (++m_counter >= m_next_interval) {
            m_counter = 0;
            m_next_interval = GetRandomInterval();
            MessageBoxW(NULL, L"该喝水了！", L"健康提醒", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
        }
    }

    // 适配 IPluginItem 相关的纯虚函数
    virtual int GetItemCount() override { return 1; }
    virtual const wchar_t* GetItemName(int item_index) override { return L"喝水计时"; }
    virtual const wchar_t* GetItemValueText(int item_index) override { return L""; }
    virtual const wchar_t* GetItemValueSampleText(int item_index) override { return L""; }
    virtual int GetItemWidth(int item_index) override { return 0; }
    virtual void OnClick(int item_index) override {}
    virtual void OnContextMenu(int item_index, HWND hWnd, int x, int y) override {}
    virtual int GetStaticValue(int item_index) override { return 0; }
    
    // 对应报错末尾提到的 DataRequired 等缺失函数
    virtual void DataRequired() override {}
    virtual void* GetItem(int item_index) override { return nullptr; }

    virtual void Release() override { delete this; }
};

extern "C" __declspec(dllexport) ITMPlugin* TMPluginInstance() {
    return new CDrinkWaterPlugin();
}
