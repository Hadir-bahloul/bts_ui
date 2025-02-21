#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "UI", nullptr, nullptr);
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

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Your UI code starts here
        ImGui::Begin("BTS_Revo");

        // Constants for better readability
        const int NUM_DAGGERS = 6;
        const int NUM_BLITZES = 3;
        const int CHANNELS_PER_BLITZ = 8;
        const int CHANNELS_PER_DAGGER = 4;
        const int TOTAL_CHANNELS = 24;

        static bool isDaggerPressed[NUM_DAGGERS] = { false };
        static bool isBlitzPressed[NUM_BLITZES] = { false };
        static bool buttonColors[TOTAL_CHANNELS] = { false };

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
                    isBlitzPressed[blitzIndex] = !isBlitzPressed[blitzIndex]; // Toggle state
                }

                // Change color if Blitz is pressed
                if (isBlitzPressed[blitzIndex]) {
                    for (int i = blitzIndex * CHANNELS_PER_BLITZ; i < (blitzIndex + 1) * CHANNELS_PER_BLITZ; i++) {
                        buttonColors[i] = true;
                    }
                }

                ImGui::SameLine();
            }

            ImGui::NewLine();

            // Dagger buttons
            for (int daggerIndex = 0; daggerIndex < NUM_DAGGERS; daggerIndex++) {
                if (ImGui::Button(("DAGGER" + std::to_string(daggerIndex + 1)).c_str(), ImVec2(100, 30))) {
                    isDaggerPressed[daggerIndex] = !isDaggerPressed[daggerIndex]; // Toggle state
                }

                // Change color if Dagger is pressed
                if (isDaggerPressed[daggerIndex]) {
                    for (int i = daggerIndex * CHANNELS_PER_DAGGER; i < (daggerIndex + 1) * CHANNELS_PER_DAGGER; i++) {
                        buttonColors[i] = true;
                    }
                }

                ImGui::SameLine();
            }

            ImGui::NewLine();

            // Display the 24 buttons at the bottom
            for (int i = 0; i < TOTAL_CHANNELS; i++) {
                if (buttonColors[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green color
                }

                ImGui::Button(std::to_string(i + 1).c_str(), ImVec2(50, 30));

                if (buttonColors[i]) {
                    ImGui::PopStyleColor();
                    buttonColors[i] = false; // Reset after displaying
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
                if (isBlitzPressed[i]) {
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
            }

            // Load Simulation (Resistor) function controlled by active daggers
            bool anyDaggerActive = false;
            for (int i = 0; i < NUM_DAGGERS; i++) {
                if (isDaggerPressed[i]) {
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
                    if (isDaggerPressed[daggerIndex]) {
                        ImGui::Text("- DAGGER %d controls channels %d to %d", daggerIndex + 1, daggerIndex * CHANNELS_PER_DAGGER + 1, (daggerIndex + 1) * CHANNELS_PER_DAGGER);
                    }
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