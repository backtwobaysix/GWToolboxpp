#pragma once

#include <ToolboxUIPlugin.h>

#include <imgui.h>
#include <SimpleIni.h>

class AdvancedHacks : public ToolboxUIPlugin {

public:
    AdvancedHacks() = default;
    ~AdvancedHacks() override = default;

    void Initialize(ImGuiContext*, ImGuiAllocFns, HMODULE) override;
    static void targetsame(const wchar_t*, int, LPWSTR*);
    static void rawdialog(const wchar_t*, int argc, LPWSTR*);
    static bool ParseUInt(const wchar_t* str, unsigned int* val, int base = 0);
    const char* Name() const override { return "Advanced Hacks"; }
    // Draw user interface. Will be called every frame if the element is visible 
     
};
