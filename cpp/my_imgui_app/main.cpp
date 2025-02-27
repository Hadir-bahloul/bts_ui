#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For rand()

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <shobjidl.h> // For IFileDialog
#endif

#ifdef __linux__
#include <gtk/gtk.h>
#endif

// Function to simulate device discovery
int DiscoverBTSRevoDevices() {
    // Simulate discovery by returning a random number between 1 and 4
    return rand() % 4 + 6;
}

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

    BTSRevoDevice(int index) : deviceIndex(index) {}
};

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

// Function to render the control interface for a BTS_Revo device
void RenderBTSRevoControl(BTSRevoDevice& device) {
    // Constants for better readability
    const int NUM_DAGGERS = 6;
    const int NUM_BLITZES = 3;
    const int CHANNELS_PER_BLITZ = 8;
    const int CHANNELS_PER_DAGGER = 4;
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
        // Blitz buttons
        for (int blitzIndex = 0; blitzIndex < NUM_BLITZES; blitzIndex++) {
            if (ImGui::Button(("BLITZ" + std::to_string(blitzIndex + 1)).c_str(), ImVec2(100, 30))) {
                device.isBlitzPressed[blitzIndex] = !device.isBlitzPressed[blitzIndex]; // Toggle state
            }

            // Change color if Blitz is pressed
            if (device.isBlitzPressed[blitzIndex]) {
                for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                    device.buttonColors[i] = true;
                }
            }

            ImGui::SameLine();
        }

        ImGui::NewLine();

        // Dagger buttons
        for (int daggerIndex = 0; daggerIndex < NUM_DAGGERS; daggerIndex++) {
            if (ImGui::Button(("DAGGER" + std::to_string(daggerIndex + 1)).c_str(), ImVec2(100, 30))) {
                device.isDaggerPressed[daggerIndex] = !device.isDaggerPressed[daggerIndex]; // Toggle state
            }

            // Change color if Dagger is pressed
            if (device.isDaggerPressed[daggerIndex]) {
                for (int i = daggerIndex * CHANNELS_PER_DAGGER; i < (daggerIndex + 1) * CHANNELS_PER_DAGGER; i++) {
                    device.buttonColors[i] = true;
                }
            }

            ImGui::SameLine();
        }

        ImGui::NewLine();

        // Display the 24 buttons at the bottom
        for (int i = 0; i < TOTAL_CHANNELS; i++) {
            if (device.buttonColors[i]) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green color
            }

            if (ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(50, 30))) {
                device.channelMeasurements[i].isWindowOpen = true; // Open the measurement window for this device's channel
            }

            if (device.buttonColors[i]) {
                ImGui::PopStyleColor();
                device.buttonColors[i] = false; // Reset after displaying
            }

            if (i % 8 < 7) {
                ImGui::SameLine();
            }
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
                    ImGui::Text("- DAGGER %d controls channels %d to %d", daggerIndex + 1, daggerIndex * CHANNELS_PER_DAGGER + 1, (daggerIndex + 1) * CHANNELS_PER_DAGGER);
                }
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

// Function to open a directory dialog (platform-specific)
std::string OpenDirectoryDialog() {
    std::string selectedDir;

#ifdef _WIN32
    // Windows: Use IFileDialog
    IFileDialog* pFileDialog = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog)))) {
        DWORD options;
        pFileDialog->GetOptions(&options);
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS);

        if (SUCCEEDED(pFileDialog->Show(nullptr))) {
            IShellItem* pItem;
            if (SUCCEEDED(pFileDialog->GetResult(&pItem))) {
                PWSTR pszFilePath;
                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
                    char buffer[MAX_PATH];
                    WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, buffer, MAX_PATH, nullptr, nullptr);
                    selectedDir = buffer;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }
#endif

#ifdef __linux__
    // Linux: Use GTK
    gtk_init(nullptr, nullptr);
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Select Directory",
        nullptr,
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Select", GTK_RESPONSE_ACCEPT,
        nullptr);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        selectedDir = folder;
        g_free(folder);
    }
    gtk_widget_destroy(dialog);
#endif

    return selectedDir;
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

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Discovery phase
    bool discoveryPhase = true;
    int numDevices = 0;

    // List to keep track of BTS_Revo devices
    std::vector<BTSRevoDevice> devices;

    // Log file path
    char logFilePathBuffer[256] = ""; // Buffer for the log file path
    std::string logFilePath; // To store the final log file path

    // Script execution
    char scriptBuffer[1024] = ""; // Buffer for the script
    bool showScriptWindow = false;

    // Selected device index
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

        // Left panel: Device list
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
        }
        ImGui::EndChild();

        // Middle panel: Control interface
        ImGui::SameLine();
        ImGui::BeginChild("Middle Panel", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
        {
            if (!discoveryPhase) {
                if (selectedDeviceIndex >= 0 && selectedDeviceIndex < devices.size()) {
                    RenderBTSRevoControl(devices[selectedDeviceIndex]);
                }
            }
        }
        ImGui::EndChild();

        // Bottom panel: Script editor and execution
        ImGui::BeginChild("Bottom Panel", ImVec2(0, 0), true);
        {
            ImGui::InputTextMultiline("Script", scriptBuffer, IM_ARRAYSIZE(scriptBuffer), ImVec2(-1, 100));
            if (ImGui::Button("Run Script on All Devices")) {
                // Implement script execution logic here
                std::cout << "Executing script on all devices:\n" << scriptBuffer << std::endl;
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