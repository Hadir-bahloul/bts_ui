#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For rand()

// Struct to hold measurement data for each channel
struct ChannelMeasurement {
    float voltage = 0.0f;
    float current = 0.0f;
    float resistance = 0.0f;
    bool isWindowOpen = false;
};

// Struct to hold the state of each BTS_Revo device
struct BTSRevoDevice {
    int deviceIndex;
    bool isDaggerPressed[6] = { false };
    bool isBlitzPressed[3] = { false };
    bool buttonColors[24] = { false };
    ChannelMeasurement channelMeasurements[24]; // Each device has its own channel measurements
    std::string script; // Script for this device
    char scriptBuffer[1024] = {0}; // Buffer for script input

    BTSRevoDevice(int index) : deviceIndex(index) {}
};

// Function to simulate device discovery
int DiscoverBTSRevoDevices() {
    // Simulate discovery by returning a random number between 1 and 4
    return rand() % 4 + 3;
}

// Function to render a measurement window for a channel
void RenderChannelMeasurementWindow(ChannelMeasurement& measurement, int channelNumber, int deviceIndex) {
    // Set a default size for the window
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    // Begin the measurement window
    if (ImGui::Begin(("Channel " + std::to_string(channelNumber) + " Measurements (Device " + std::to_string(deviceIndex) + ")").c_str(), &measurement.isWindowOpen)) {
        // Add some padding around the content
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));

        // Group voltage-related controls
        if (ImGui::CollapsingHeader("Voltage", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Current Voltage: %.2f V", measurement.voltage);
            ImGui::InputFloat("Set Voltage (V)", &measurement.voltage, 0.1f, 1.0f, "%.2f");
        }

        // Separator
        ImGui::Separator();

        // Group current-related controls
        if (ImGui::CollapsingHeader("Current", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Current Current: %.2f A", measurement.current);
            ImGui::InputFloat("Set Current (A)", &measurement.current, 0.1f, 1.0f, "%.2f");
        }

        // Separator
        ImGui::Separator();

        // Group resistance-related controls
        if (ImGui::CollapsingHeader("Resistance", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Current Resistance: %.2f Ohm", measurement.resistance);
            ImGui::InputFloat("Set Resistance (Ohm)", &measurement.resistance, 0.1f, 1.0f, "%.2f");
        }

        // Separator
        ImGui::Separator();

        // Add a button to reset all values
        if (ImGui::Button("Reset Values", ImVec2(-1, 30))) {
            measurement.voltage = 0.0f;
            measurement.current = 0.0f;
            measurement.resistance = 0.0f;
        }

        // Pop style variables
        ImGui::PopStyleVar(2);
    }
    ImGui::End();
}

// Function to set light theme colors
void SetLightTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // Dark gray text
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f); // Light gray background
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f); // Slightly lighter gray for child windows
    style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.80f, 0.50f); // Light gray border
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // White for input fields
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // Light gray for hover
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f); // Slightly darker gray for active
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f); // Light gray for title bar
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f); // Slightly darker gray for active title
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // Blue buttons
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.49f, 0.86f, 1.00f); // Darker blue for hover
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.39f, 0.78f, 1.00f); // Even darker blue for active
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // Blue headers
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.49f, 0.86f, 1.00f); // Darker blue for hover
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.39f, 0.78f, 1.00f); // Even darker blue for active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f); // Medium gray for separators
}

// Function to set dark theme colors
void SetDarkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f); // White text
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f); // Dark gray background
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Slightly lighter gray for child windows
    style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f); // Medium gray border
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Dark gray for input fields
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Lighter gray for hover
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Slightly lighter gray for active
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // Dark gray for title bar
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // Slightly lighter gray for active title
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // Blue buttons
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.49f, 0.86f, 1.00f); // Darker blue for hover
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.39f, 0.78f, 1.00f); // Even darker blue for active
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f); // Blue headers
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.49f, 0.86f, 1.00f); // Darker blue for hover
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16f, 0.39f, 0.78f, 1.00f); // Even darker blue for active
    style.Colors[ImGuiCol_Separator] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Medium gray for separators
}

// Function to render the control interface for a BTS_Revo device
void RenderBTSRevoControl(BTSRevoDevice& device, std::vector<BTSRevoDevice>& devices, bool& isDarkTheme) {
    // Constants for better readability
    const int NUM_DAGGERS = 6;
    const int NUM_BLITZES = 3;
    const int CHANNELS_PER_BLITZ = 8;
    const int TOTAL_CHANNELS = 24;

    // Blitz functions
    const char* blitz_functions[] = {
        "Current Measurement (High Current)",
        "Quiescent Current Measurement",
        "Short to GND (High Current)",
        "Load Simulation (High Current)"
    };

    // Display base functions
    const char* base_functions[] = {
        "Open Load", "Short Vbat", "Short GND", "PWM Generation", "Low Current Generation",
        "Voltage Measurement", "Current Measurement (Low Current)", "PWM Measurement"
    };

    // Split the window into two columns
    ImGui::Columns(2, "MainColumns", true);

    // Left column: Buttons and controls
    ImGui::BeginChild("LeftColumn", ImVec2(0, 0), true);
    {
        // Render Dagger buttons in pairs, with Blitz centered between them
        ImGui::Text("Daggers:");
        for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
            // Calculate the horizontal offset to center the Dagger pair above the Blitz
            float offsetX = static_cast<float>(blitzIndex * CHANNELS_PER_BLITZ * 45) + (CHANNELS_PER_BLITZ * 40 / 3) - 90; // Adjusted for Dagger pair width
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

            // First Dagger in the pair
            int daggerIndex = blitzIndex * 2; // Daggers start at 0
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.44f, 0.38f, 1.0f)); // Professional pink (#FF6F61)
            if (ImGui::Button(("DAGGER " + std::to_string(daggerIndex + 1)).c_str(), ImVec2(80, 30))) {
                device.isDaggerPressed[daggerIndex] = !device.isDaggerPressed[daggerIndex]; // Toggle state
            }
            ImGui::PopStyleColor();

            // Change color if Dagger is pressed
            if (device.isDaggerPressed[daggerIndex]) {
                for (int i = daggerIndex * 4; i < (daggerIndex + 1) * 4; i++) {
                    device.buttonColors[i] = true;
                }
            }

            ImGui::SameLine(0, 5); // Reduce spacing between Dagger buttons to 5 pixels

            // Second Dagger in the pair
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.44f, 0.38f, 1.0f)); // Professional pink (#FF6F61)
            if (ImGui::Button(("DAGGER " + std::to_string(daggerIndex + 2)).c_str(), ImVec2(80, 30))) {
                device.isDaggerPressed[daggerIndex + 1] = !device.isDaggerPressed[daggerIndex + 1]; // Toggle state
            }
            ImGui::PopStyleColor();

            // Change color if Dagger is pressed
            if (device.isDaggerPressed[daggerIndex + 1]) {
                for (int i = (daggerIndex + 1) * 4; i < (daggerIndex + 2) * 4; i++) {
                    device.buttonColors[i] = true;
                }
            }

            // Add a vertical bar separator between Dagger pairs
            if (blitzIndex < NUM_BLITZES - 1) {
                ImGui::NewLine();
            }
        }

        ImGui::NewLine();
        ImGui::Separator(); // Add a separator between Daggers and Blitzes

        // Add a rectangle with "BTS_Révo" text
        ImVec2 rectSize = ImVec2(ImGui::GetContentRegionAvail().x, 50); // Rectangle size
        ImVec2 rectPos = ImGui::GetCursorScreenPos(); // Position of the rectangle
        ImDrawList* drawList = ImGui::GetWindowDrawList(); // Get the draw list

        // Draw the rectangle
        drawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), IM_COL32(46, 46, 46, 255)); // Dark gray background (#2E2E2E)
        drawList->AddRect(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), IM_COL32(74, 74, 74, 255)); // Light gray border (#4A4A4A)

        // Add text "BTS_Révo" in the center of the rectangle
        ImVec2 textSize = ImGui::CalcTextSize("BTS_Révo");
        ImVec2 textPos = ImVec2(rectPos.x + (rectSize.x - textSize.x) / 2, rectPos.y + (rectSize.y - textSize.y) / 2);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), "BTS_Révo"); // White text (#FFFFFF)

        // Move cursor down after the rectangle
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + rectSize.y + 10);

        ImGui::Separator(); // Add a separator between the rectangle and Blitzes

        // Render Blitz buttons, each centered between its corresponding Dagger pair
        ImGui::Text("Blitzes:");
        for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
            // Calculate the horizontal offset to center the Blitz button between the Dagger pair
            float offsetX = static_cast<float>(blitzIndex * CHANNELS_PER_BLITZ * 40) + (CHANNELS_PER_BLITZ * 40 / 2) - 50; // Adjust based on button width (100)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.42f, 0.36f, 0.58f, 1.0f)); // Professional blue (#6B5B95)
            if (ImGui::Button(("BLITZ " + std::to_string(blitzIndex + 1)).c_str(), ImVec2(100, 30))) {
                device.isBlitzPressed[blitzIndex] = !device.isBlitzPressed[blitzIndex]; // Toggle state
            }
            ImGui::PopStyleColor();

            // Change color if Blitz is pressed
            if (device.isBlitzPressed[blitzIndex]) {
                for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                    device.buttonColors[i] = true;
                }
            }
        }

        ImGui::NewLine();
        ImGui::Separator(); // Add a separator between Blitzes and Channels

        // Render channels, each group of 8 under its corresponding Blitz
        ImGui::Text("Channels:");
        for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
            // Position the channels under their corresponding Blitz button
            for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                if (device.buttonColors[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green color for active channels
                }

                if (ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(30, 30))) {
                    device.channelMeasurements[i].isWindowOpen = true; // Open the measurement window for this device's channel
                }

                if (device.buttonColors[i]) {
                    ImGui::PopStyleColor();
                    device.buttonColors[i] = false; // Reset after displaying
                }

                if (i < (blitzIndex + 1) * CHANNELS_PER_BLITZ - 1) {
                    ImGui::SameLine();
                }
            }

            // Add a vertical bar separator between channel groups
            if (blitzIndex < NUM_BLITZES - 1) {
                ImGui::SameLine();
                ImGui::Text("|");
                ImGui::SameLine();
            }
        }

        // Device script section (moved under the channels)
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("Device Script:");
        if (ImGui::InputTextMultiline(("##DeviceScript" + std::to_string(device.deviceIndex)).c_str(), device.scriptBuffer, IM_ARRAYSIZE(device.scriptBuffer), ImVec2(-1, 100))) {
            device.script = std::string(device.scriptBuffer); // Update the script string
        }
        if (ImGui::Button("Run Device Script")) {
            // Implement script execution for this device
            std::cout << "Running script for Device " << device.deviceIndex << ":\n" << device.script << "\n";
        }
    }
    ImGui::EndChild();

    // Right column: Functions
    ImGui::NextColumn();
    ImGui::BeginChild("RightColumn", ImVec2(0, 0), true);
    {
        ImGui::Text("Base Functions:");
        for (const auto& func : base_functions) {
            if (ImGui::Button(func)) {
                // Implement function behavior here
            }
        }

        // Blitz functions appear when at least one Blitz is pressed
        bool anyBlitzActive = false;
        for (int i = 0; i < NUM_BLITZES; i++) {
            if (device.isBlitzPressed[i]) {
                anyBlitzActive = true;
                break;
            }
        }

        if (anyBlitzActive) {
            ImGui::Text("Blitz Functions:");
            for (const auto& func : blitz_functions) {
                if (ImGui::Button(func)) {
                    // Implement function behavior here
                }
            }

            // Display controlled channels for each active Blitz
            ImGui::Text("Controlled Channels:");
            for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
                if (device.isBlitzPressed[blitzIndex]) {
                    ImGui::Text("- BLITZ %d controls channels %d to %d", blitzIndex + 1, blitzIndex * CHANNELS_PER_BLITZ + 1, (blitzIndex + 1) * CHANNELS_PER_BLITZ);
                }
            }
        }

        // Load Simulation (Resistor) function controlled by active daggers
        bool anyDaggerActive = false;
        for (int i = 0; i < NUM_DAGGERS; i++) {
            if (device.isDaggerPressed[i]) {
                anyDaggerActive = true;
                break;
            }
        }

        if (anyDaggerActive) {
            ImGui::NewLine();
            if (ImGui::Button("Load Simulation (Resistor)")) {
                // Implement the behavior for Load Simulation (Resistor) here
            }

            // Display channels controlled by active daggers
            ImGui::Text("Channels Controlled by Active Daggers:");
            for (int daggerIndex = 0; daggerIndex < NUM_DAGGERS; daggerIndex++) {
                if (device.isDaggerPressed[daggerIndex]) {
                    ImGui::Text("- DAGGER %d controls channels %d to %d", daggerIndex + 1, daggerIndex * 4 + 1, (daggerIndex + 1) * 4);
                }
            }
        }

        // New script zone for applying to all devices
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("Apply to All Devices:");
        static char applyToAllScriptBuffer[1024] = {0};
        if (ImGui::InputTextMultiline("##ApplyToAllScript", applyToAllScriptBuffer, IM_ARRAYSIZE(applyToAllScriptBuffer), ImVec2(-1, 100))) {
            // Update the apply-to-all script string
        }
        if (ImGui::Button("Apply to All Devices")) {
            // Implement script execution for all devices
            std::string applyToAllScript = std::string(applyToAllScriptBuffer);
            std::cout << "Applying script to all devices:\n" << applyToAllScript << "\n";
            for (auto& device : devices) {
                device.script += "\n" + applyToAllScript; // Append the script to each device's script
            }
        }
    }
    ImGui::EndChild();

    // Render measurement windows for each channel
    for (int i = 0; i < TOTAL_CHANNELS; i++) {
        if (device.channelMeasurements[i].isWindowOpen) {
            RenderChannelMeasurementWindow(device.channelMeasurements[i], i + 1, device.deviceIndex);
        }
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "BTS_Revo UI", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style (default to dark theme)
    bool isDarkTheme = true;
    SetDarkTheme();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Discovery phase
    bool discoveryPhase = true;
    int numDevices = 0;

    // List to keep track of BTS_Revo devices
    std::vector<BTSRevoDevice> devices;

    // Track the currently selected device
    static int selectedDeviceIndex = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Full-screen main window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
        ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // Left panel: Device list and theme toggle
        ImGui::BeginChild("Left Panel", ImVec2(200, 0), true);
        {
            if (discoveryPhase) {
                if (ImGui::Button("Discover")) {
                    numDevices = DiscoverBTSRevoDevices();
                    discoveryPhase = false;

                    // Create BTS_Revo devices
                    for (int i = 0; i < numDevices; i++) {
                        devices.emplace_back(i + 1); // Each device has its own index and state
                    }
                }
            } else {
                for (int i = 0; i < devices.size(); i++) {
                    if (ImGui::Selectable(("BTS_Revo " + std::to_string(i + 1)).c_str(), selectedDeviceIndex == i)) {
                        selectedDeviceIndex = i;
                    }
                }
            }

            // Theme toggle button
            ImGui::NewLine();
            ImGui::Separator();
            if (ImGui::Button(isDarkTheme ? "Switch to Light Theme" : "Switch to Dark Theme")) {
                isDarkTheme = !isDarkTheme; // Toggle theme
                if (isDarkTheme) {
                    SetDarkTheme();
                } else {
                    SetLightTheme();
                }
            }
        }
        ImGui::EndChild();

        // Middle panel: Control interface
        ImGui::SameLine();
        ImGui::BeginChild("Middle Panel", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
        {
            if (!discoveryPhase) {
                if (selectedDeviceIndex >= 0 && selectedDeviceIndex < devices.size()) {
                    RenderBTSRevoControl(devices[selectedDeviceIndex], devices, isDarkTheme);
                }
            }
        }
        ImGui::EndChild();

        // Right panel: Apply-to-all script
        ImGui::SameLine();
        ImGui::BeginChild("Right Panel", ImVec2(200, 0), true);
        {
            // New script zone for applying to all devices
            ImGui::Text("Apply to All Devices:");
            static char applyToAllScriptBuffer[1024] = {0};
            if (ImGui::InputTextMultiline("##ApplyToAllScript", applyToAllScriptBuffer, IM_ARRAYSIZE(applyToAllScriptBuffer), ImVec2(-1, 100))) {
                // Update the apply-to-all script string
            }
            if (ImGui::Button("Apply to All Devices")) {
                // Implement script execution for all devices
                std::string applyToAllScript = std::string(applyToAllScriptBuffer);
                std::cout << "Applying script to all devices:\n" << applyToAllScript << "\n";
                for (auto& device : devices) {
                    device.script += "\n" + applyToAllScript; // Append the script to each device's script
                }
            }
        }
        ImGui::EndChild();

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}