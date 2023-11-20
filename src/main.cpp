#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <random>
#include <thread>
#include <chrono>
#include <atomic>
#include <algorithm>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "Sorting Algorithms Visualization"

struct rgb {
    uint8_t r, g, b;

    rgb(uint8_t r, uint8_t g, uint8_t b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

enum SORTING_ALGORITHMS {
    BUBBLE_SORT,
    MERGE_SORT [[maybe_unused]]
};

const char *SORTING_ALGORITHMS_NAMES[] = {"Bubble Sort", "Merge Sort"};

enum PROCESS {
    NONE,
    SHUFFLING,
    SORTING
};

PROCESS current_process = PROCESS::NONE;

std::atomic<bool> stop_flag(false);

static void glfw_error_callback(int error, const char *DESCRIPTION) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, DESCRIPTION);
}

void draw_bar_chart(
        const uint32_t *ARR,
        uint32_t arr_size,
        rgb *colors,
        GLFWwindow *window,
        double clearance,
        double height_coefficient_multiplier
) {
    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, 0.0, window_height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    uint64_t max = 0;
    for (uint64_t i = 0; i < arr_size; ++i) if (ARR[i] > max) max = ARR[i];

    const double COLUMN_WIDTH = (double) window_width / (double) arr_size;
    const double HEIGHT_COEFFICIENT = ((double) window_height / (double) max) * (double) height_coefficient_multiplier;

    if (clearance > 1.0) clearance = 1.0;
    clearance *= COLUMN_WIDTH;

    double x = 0;
    for (uint32_t i = 0; i < arr_size; ++i) {
        glColor3f((float) colors[i].r / 255, (float) colors[i].g / 255, (float) colors[i].b / 255);

        glBegin(GL_QUADS);
        glVertex2d(x + clearance, 0);
        glVertex2d(x + COLUMN_WIDTH, 0);
        glVertex2d(x + COLUMN_WIDTH, HEIGHT_COEFFICIENT * (double) ARR[i]);
        glVertex2d(x + clearance, HEIGHT_COEFFICIENT * (double) ARR[i]);
        glEnd();

        x += COLUMN_WIDTH;
    }
}

void bubble_sort_algorithm(uint32_t *array, uint32_t array_size) {
    for (uint32_t i = 0; i < array_size - 1; ++i) {
        for (uint32_t j = 0; j < array_size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                std::swap(array[j], array[j + 1]);
            }
        }
    }
}

void bubble_sort_algorithm_visual(uint32_t *array, rgb *colors, uint32_t array_size, uint32_t delay_ms) {
    current_process = PROCESS::SORTING;
    for (uint32_t i = 0; i < array_size - 1; ++i) {
        for (uint32_t j = 0; j < array_size - i - 1; ++j) {
            if (stop_flag.load()) {
                current_process = PROCESS::NONE;
                return;
            }

            colors[j] = rgb(0, 255, 0);
            colors[j + 1] = rgb(255, 0, 0);

            if (array[j] > array[j + 1]) {
                std::swap(array[j], array[j + 1]);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

            colors[j] = rgb(255, 255, 255);
            colors[j + 1] = rgb(255, 255, 255);
        }
    }
    current_process = PROCESS::NONE;
}

void merge(uint32_t *arr, uint32_t l, uint32_t m, uint32_t r) {
    uint32_t i, j, k;
    uint32_t l_size = m - l + 1;
    uint32_t r_size = r - m;

    uint32_t L[l_size], R[r_size];

    for (i = 0; i < l_size; i++) L[i] = arr[l + i];
    for (j = 0; j < r_size; j++) R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < l_size && j < r_size) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }

        k++;
    }

    while (i < l_size) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < r_size) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_visual(uint32_t *arr, rgb *colors, uint32_t l, uint32_t m, uint32_t r, uint32_t delay_ms) {
    uint32_t i, j, k;
    uint32_t n1 = m - l + 1;
    uint32_t n2 = r - m;

    uint32_t L[n1], R[n2];

    for (i = 0; i < n1; i++) {
        L[i] = arr[l + i];
    }

    for (j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (stop_flag.load()) {
            for (uint32_t c = l; c <= r; ++c) {
                colors[c] = rgb(255, 255, 255);
            }
            current_process = PROCESS::NONE;
            return;
        }

        if (colors[l + i].r == 255 && colors[l + i].g == 255 && colors[l + i].b == 255) colors[l + i] = rgb(255, 0, 0);
        if (colors[m + 1 + j].r == 255 && colors[m + 1 + j].g == 255 && colors[m + 1 + j].b == 255)
            colors[m + 1 + j] = rgb(255, 0, 0);

        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;

        if (!stop_flag.load()) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

        for (uint32_t c = l; c <= r; ++c) {
            colors[c] = rgb(255, 255, 255);
        }
    }

    for (uint32_t c = l; c <= r; ++c) {
        colors[c] = rgb(255, 255, 255);
    }

    while (i < n1) {
        if (stop_flag.load()) {
            for (uint32_t c = l; c <= r; ++c) {
                colors[c] = rgb(255, 255, 255);
            }
            current_process = PROCESS::NONE;
            return;
        }

        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        if (stop_flag.load()) {
            for (uint32_t c = l; c <= r; ++c) {
                colors[c] = rgb(255, 255, 255);
            }
            current_process = PROCESS::NONE;
            return;
        }

        arr[k] = R[j];
        j++;
        k++;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

void merge_sort_algorithm(uint32_t *arr, uint32_t l, uint32_t r) {
    if (l < r) {
        uint32_t m = l + (r - l) / 2;
        merge_sort_algorithm(arr, l, m);
        merge_sort_algorithm(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge_sort_algorithm_visual(uint32_t *arr, rgb *colors, uint32_t l, uint32_t r, uint32_t delay_ms) {
    if (l < r) {
        uint32_t m = l + (r - l) / 2;

        merge_sort_algorithm_visual(arr, colors, l, m, delay_ms);
        merge_sort_algorithm_visual(arr, colors, m + 1, r, delay_ms);

        colors[l] = rgb(0, 255, 0);
        colors[m] = rgb(0, 0, 255);
        colors[r] = rgb(0, 255, 0);

        merge_visual(arr, colors, l, m, r, delay_ms);

        if (!stop_flag.load()) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

        if (stop_flag.load()) {
            for (uint32_t c = l; c <= r; ++c) {
                colors[c] = rgb(255, 255, 255);
            }
            current_process = PROCESS::NONE;
            return;
        }
    } else if (l == r) {
        colors[l] = rgb(0, 255, 0);
        if (!stop_flag.load()) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        colors[l] = rgb(255, 255, 255);
    }

    current_process = PROCESS::NONE;
}

#pragma clang diagnostic pop

int main(int, char **) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return -1;
    }

    const char *GLSL_VERSION = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window = glfwCreateWindow(
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            WINDOW_TITLE,
            nullptr,
            nullptr
    );

    if (!window) {
        return -2;
    }

    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool render = true, show_message = false, auto_update = false, unique_nums = false, unique_nums_ui = false;
        static float clearance = 0.3, height_coefficient_multiplier = 0.9;

        static SORTING_ALGORITHMS selected_sorting_algorithm = SORTING_ALGORITHMS::BUBBLE_SORT;

        static int arr_size = 100, arr_size_ui = 100, max_num = 1000, max_num_ui = 1000, delay = 5;

        static uint32_t *arr = nullptr;
        static rgb *colors = nullptr;
        static uint64_t sort_time = 0;

        static std::random_device rd;
        static std::mt19937 rng(rd());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Array")) {
                static int shuffle_delay_ms = 10;

                ImGui::InputInt("Number Of Elements", &arr_size_ui, 1);
                ImGui::InputInt("Max Value", &max_num_ui, 1);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Affects only non-unique arrays!");
                ImGui::InputInt("Shuffle Visual Delay (milliseconds)", &shuffle_delay_ms, 1);
                ImGui::Checkbox("Only Unique Values", &unique_nums_ui);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(
                            "If toggled, array will have only unique numbers. When array is sorted it will form a triangle.");
                ImGui::SameLine();
                ImGui::Checkbox("Auto Update", &auto_update);
                ImGui::Separator();

                if (ImGui::Button("Randomize") && current_process == PROCESS::NONE) {
                    arr_size = arr_size_ui;
                    max_num = max_num_ui;
                    unique_nums = unique_nums_ui;

                    free(arr);
                    free(colors);

                    arr = (uint32_t *) malloc(arr_size_ui * sizeof(uint32_t));
                    colors = (rgb *) malloc(arr_size_ui * sizeof(rgb));

                    if (unique_nums) {
                        for (int i = 0; i < arr_size_ui; ++i) {
                            arr[i] = i;
                            colors[i] = rgb(255, 255, 255);
                        }

                        for (int i = arr_size_ui - 1; i > 0; --i) {
                            std::uniform_int_distribution<int> distribution(0, i);
                            std::swap(arr[i], arr[distribution(rng)]);
                        }
                    } else {
                        std::uniform_int_distribution<uint32_t> distribution(1, arr_size_ui);
                        for (int i = 0; i < arr_size_ui; ++i) {
                            arr[i] = distribution(rng);
                            colors[i] = rgb(255, 255, 255);
                        }
                    }
                }
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Randomizes array with new values.");

                ImGui::SameLine();

                if (ImGui::Button("Shuffle") && current_process == PROCESS::NONE) {
                    current_process = PROCESS::SHUFFLING;

                    auto shuffle_thread_func = []() {
                        for (int i = arr_size - 1; i > 0; --i) {
                            std::uniform_int_distribution<int> distribution(0, i);
                            int j = distribution(rng);

                            colors[i] = rgb(255, 0, 0);
                            colors[j] = rgb(0, 255, 0);

                            std::swap(arr[i], arr[j]);

                            std::this_thread::sleep_for(std::chrono::milliseconds(shuffle_delay_ms));

                            colors[i] = rgb(255, 255, 255);
                            colors[j] = rgb(255, 255, 255);
                        }

                        current_process = PROCESS::NONE;
                    };

                    std::thread shuffle_thread(shuffle_thread_func);
                    shuffle_thread.detach();
                }
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Shuffles array. Keeps old values.");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sort")) {
                ImGui::Combo(
                        "Sorting Algorithm",
                        reinterpret_cast<int *>(&selected_sorting_algorithm),
                        SORTING_ALGORITHMS_NAMES, 2
                );

                ImGui::InputInt("Visual Delay (microseconds)", &delay, 1);

                ImGui::Separator();

                if (ImGui::Button("Sort") && current_process == PROCESS::NONE && arr) {
                    switch (selected_sorting_algorithm) {
                        case BUBBLE_SORT: {
                            current_process = PROCESS::SORTING;
                            auto start_time = std::chrono::high_resolution_clock::now();
                            bubble_sort_algorithm(arr, arr_size);
                            auto end_time = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    end_time - start_time);
                            sort_time = duration.count();
                            show_message = true;
                            current_process = PROCESS::NONE;
                            break;
                        }
                        case MERGE_SORT:
                            current_process = PROCESS::SORTING;
                            auto start_time = std::chrono::high_resolution_clock::now();
                            merge_sort_algorithm(arr, 0, arr_size - 1);
                            auto end_time = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    end_time - start_time);
                            sort_time = duration.count();
                            show_message = true;
                            current_process = PROCESS::NONE;
                            break;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Visualize") && current_process == PROCESS::NONE && arr && colors) {
                    switch (selected_sorting_algorithm) {
                        case BUBBLE_SORT: {
                            current_process = PROCESS::SORTING;
                            stop_flag.store(false);
                            std::thread sorting_thread(
                                    bubble_sort_algorithm_visual,
                                    arr,
                                    colors,
                                    arr_size,
                                    delay
                            );

                            sorting_thread.detach();
                            break;
                        }
                        case MERGE_SORT:
                            current_process = PROCESS::SORTING;
                            stop_flag.store(false);
                            std::thread sorting_thread(
                                    merge_sort_algorithm_visual,
                                    arr,
                                    colors,
                                    0,
                                    arr_size,
                                    delay
                            );

                            sorting_thread.detach();
                            break;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Stop")) {
                    stop_flag.store(true);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Renderer")) {
                ImGui::Checkbox("Render Enable", &render);
                ImGui::SliderFloat("Clearance", &clearance, 0, 1);
                ImGui::SliderFloat("Height Coefficient", &height_coefficient_multiplier, 0, 1);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About")) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Sorting Algorithms V0.2");
                ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Made by Denys Bondar in 2023.");
                ImGui::Text("");
                ImGui::Text("This program is still in development, so there might be bugs.");
                ImGui::Text("");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "---------- IMPORTANT ----------");
                ImGui::Text("Please don't use 'stop' button for large arrays while sorting with merge sort.");
                ImGui::Text("Merge sort is implemented as a recursive function that runs on independent thread, so it's not that easy to stop it.");
                ImGui::Text("Hopefully, I will fix it in next updates.");
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (show_message) {
            ImGui::OpenPopup("Message");
            show_message = false;
        }

        if (ImGui::BeginPopupModal(
                "Message",
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
        )) {
            float popup_width = 200, popup_height = 100;

            int window_width, window_height;
            glfwGetFramebufferSize(window, &window_width, &window_height);

            ImGui::SetWindowPos(
                    ImVec2(
                            ((float) window_width - popup_width) * 0.5f,
                            ((float) window_height - popup_height) * 0.5f
                    )
            );

            ImGui::SetWindowSize(ImVec2(popup_width, popup_height));

            ImGui::Text("Sorted in %i milliseconds", sort_time);
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Render();

        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClearColor(
                clear_color.x * clear_color.w,
                clear_color.y * clear_color.w,
                clear_color.z * clear_color.w,
                clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT);

        if (auto_update && ((arr_size != arr_size_ui) || (max_num != max_num_ui) || (unique_nums != unique_nums_ui))) {
            arr_size = arr_size_ui;
            max_num = max_num_ui;
            unique_nums = unique_nums_ui;

            free(arr);
            free(colors);

            arr = (uint32_t *) malloc(arr_size_ui * sizeof(uint32_t));
            colors = (rgb *) malloc(arr_size_ui * sizeof(rgb));

            if (unique_nums) {
                for (int i = 0; i < arr_size_ui; ++i) {
                    arr[i] = i;
                    colors[i] = rgb(255, 255, 255);
                }

                for (int i = arr_size_ui - 1; i > 0; --i) {
                    std::uniform_int_distribution<int> distribution(0, i);
                    std::swap(arr[i], arr[distribution(rng)]);
                }
            } else {
                std::uniform_int_distribution<uint32_t> distribution(1, arr_size_ui);
                for (int i = 0; i < arr_size_ui; ++i) {
                    arr[i] = distribution(rng);
                    colors[i] = rgb(255, 255, 255);
                }
            }

            arr_size = arr_size_ui;
        }

        if (render && arr && colors) {
            draw_bar_chart(arr, arr_size, colors, window, clearance, height_coefficient_multiplier);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
