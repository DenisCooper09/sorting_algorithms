#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <cstdio>
#include <random>
#include <thread>
#include <chrono>
#include <algorithm>

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "Sorting Algorithms Visualization"

bool is_sorting = false;

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

static void glfw_error_callback(int error, const char *DESCRIPTION) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, DESCRIPTION);
}

void draw_bar_chart(const uint32_t *ARRAY, uint32_t array_size, rgb *colors) {
    float bar_width = 1 / (float) array_size;

    uint32_t max = 0;
    for (uint32_t i = 0; i < array_size; ++i) if (ARRAY[i] > max) max = ARRAY[i];

    float height_coefficient = 1 / (float) max;

    float x = 0;
    for (uint32_t i = 0; i < array_size; ++i) {
        float actual_x = x * 6 - 1;

        glColor3f((float) colors[i].r / 255, (float) colors[i].g / 255, (float) colors[i].b / 255);
        glBegin(GL_QUADS);
        glVertex2f(actual_x, -1);
        glVertex2f(actual_x + bar_width, -1);
        glVertex2f(actual_x + bar_width, height_coefficient * (float) ARRAY[i] - 1);
        glVertex2f(actual_x, height_coefficient * (float) ARRAY[i] - 1);
        glEnd();

        x += bar_width / 3;
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

void bubble_sort_algorithm_delay(uint32_t *array, rgb *colors, uint32_t array_size, uint32_t delay_ms) {
    for (uint32_t i = 0; i < array_size - 1; ++i) {
        for (uint32_t j = 0; j < array_size - i - 1; ++j) {
            if (!is_sorting) return;

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

void merge_delay(uint32_t *arr, rgb *colors, uint32_t l, uint32_t m, uint32_t r, uint32_t delay_ms) {
    if (!is_sorting) return;

    uint32_t i, j, k;
    uint32_t n1 = m - l + 1;
    uint32_t n2 = r - m;

    uint32_t L[n1], R[n2];

    for (i = 0; i < n1; i++) {
        if (!is_sorting) return;
        L[i] = arr[l + i];
    }

    for (j = 0; j < n2; j++) {
        if (!is_sorting) return;
        R[j] = arr[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (!is_sorting) return;

        colors[l + i] = rgb(255, 0, 0);
        colors[m + 1 + j] = rgb(255, 0, 0);

        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

        for (uint32_t c = l; c <= r; ++c) {
            if (!is_sorting) return;
            colors[c] = rgb(255, 255, 255);
        }
    }

    for (uint32_t c = l; c <= r; ++c) {
        if (!is_sorting) return;
        colors[c] = rgb(255, 255, 255);
    }

    while (i < n1) {
        if (!is_sorting) return;
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        if (!is_sorting) return;
        arr[k] = R[j];
        j++;
        k++;
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

void merge_sort(uint32_t *arr, uint32_t l, uint32_t r) {
    if (l < r) {
        uint32_t m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge_sort_delay(uint32_t *arr, rgb *colors, uint32_t l, uint32_t r, uint32_t delay_ms) {
    if (l < r) {
        if (!is_sorting) return;

        uint32_t m = l + (r - l) / 2;

        merge_sort_delay(arr, colors, l, m, delay_ms);
        merge_sort_delay(arr, colors, m + 1, r, delay_ms);

        colors[l] = rgb(0, 255, 0);
        colors[m] = rgb(0, 0, 255);
        colors[r] = rgb(0, 255, 0);

        merge_delay(arr, colors, l, m, r, delay_ms);

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

        for (uint32_t c = l; c <= r; ++c) {
            colors[c] = rgb(255, 255, 255);
        }
    } else if (l == r) {
        colors[l] = rgb(0, 255, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        colors[l] = rgb(255, 255, 255);
    }
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

        static bool show_message = false;

        static SORTING_ALGORITHMS selected_sorting_algorithm = SORTING_ALGORITHMS::BUBBLE_SORT;

        static int number_of_elements = 100, last_number_of_elements = 100, max_value = 1000, delay = 5;
        static long long sort_time = 0;

        static uint32_t *array = nullptr;
        static rgb *colors = nullptr;

        static std::random_device rd;
        static std::mt19937 generator(rd());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {}
                if (ImGui::MenuItem("Save")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings")) {
                ImGui::Combo(
                        "Sorting Algorithm",
                        reinterpret_cast<int *>(&selected_sorting_algorithm),
                        SORTING_ALGORITHMS_NAMES, 2
                );

                ImGui::InputInt("Number of Elements", &number_of_elements, 1);
                ImGui::InputInt("Max Value", &max_value, 1);
                ImGui::InputInt("Delay (ms)", &delay, 1);

                if (ImGui::Button("Randomize")) {
                    is_sorting = false;

                    std::uniform_int_distribution<uint32_t> distribution(1, max_value);

                    free(array);
                    free(colors);

                    array = (uint32_t *) malloc(number_of_elements * sizeof(uint32_t));
                    colors = (rgb *) malloc(number_of_elements * sizeof(rgb));

                    for (int i = 0; i < number_of_elements; ++i) {
                        array[i] = distribution(generator);
                        colors[i] = rgb(255, 255, 255);
                    }

                    last_number_of_elements = number_of_elements;
                }

                ImGui::SameLine();

                if (ImGui::Button("Sort") && array) {
                    switch (selected_sorting_algorithm) {
                        case BUBBLE_SORT: {
                            auto start_time = std::chrono::high_resolution_clock::now();
                            bubble_sort_algorithm(array, number_of_elements);
                            auto end_time = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    end_time - start_time);
                            sort_time = duration.count();
                            show_message = true;
                            break;
                        }
                        case MERGE_SORT:
                            auto start_time = std::chrono::high_resolution_clock::now();
                            merge_sort(array, 0, number_of_elements - 1);
                            auto end_time = std::chrono::high_resolution_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                    end_time - start_time);
                            sort_time = duration.count();
                            show_message = true;
                            break;
                    }
                }

                ImGui::SameLine();

                if (ImGui::Button("Visualize") && array && colors) {
                    switch (selected_sorting_algorithm) {
                        case BUBBLE_SORT: {
                            is_sorting = true;
                            std::thread sorting_thread(
                                    bubble_sort_algorithm_delay,
                                    array,
                                    colors,
                                    number_of_elements,
                                    delay
                            );

                            sorting_thread.detach();
                            break;
                        }
                        case MERGE_SORT:
                            is_sorting = true;
                            std::thread sorting_thread(
                                    merge_sort_delay,
                                    array,
                                    colors,
                                    0,
                                    number_of_elements - 1,
                                    delay
                            );

                            sorting_thread.detach();
                            break;
                    }
                }

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

        if (array && colors && number_of_elements == last_number_of_elements) {
            draw_bar_chart(array, number_of_elements, colors);
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
