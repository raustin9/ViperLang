#include "test_manager.h"
#include <chrono>
#include <cstdint>
#include <cstdio>


/// @brief Register a test to the manager to be run
/// @param func Function to register 
/// @param desc Description of the test
void TestManager::register_test(PFN_test&& func, const std::string& desc) {
    test_entry entry = {};
    entry.func = func;
    entry.description = desc;

    m_tests.push_back(entry);
}

/// @brief 
void TestManager::run_tests() {
    constexpr uint8_t BYPASS = 2;
   
    std::size_t passed = 0;
    std::size_t failed = 0;
    std::size_t skipped = 0;

    std::size_t count = m_tests.size();

    double total_time = 0;  
    for (std::size_t i = 0; i < count; i++) {
        auto test_start = std::chrono::high_resolution_clock::now();
        uint8_t result = m_tests[i].func();
        auto test_finish = std::chrono::high_resolution_clock::now();
        auto test_duration = std::chrono::duration<double, std::chrono::seconds::period>(test_finish - test_start).count();
        
        if (result) {
            passed++;
        } else if (result == BYPASS) {
            std::printf("[SKIPPED]: %s\n", m_tests[i].description.c_str());
            ++skipped;
        } else {
            std::printf("[FAILED]: %s\n", m_tests[i].description.c_str());
            ++failed;
        }

        char status[30];
        const char* format = 
            failed ? "\x1b[31m*** %d FAILED***\x1b[0m" : "\x1b[32m*** SUCCESS ***\x1b[0m";
        std::sprintf(status, format, failed);

        total_time += test_duration;

        std::printf("%s Executed %lu of %lu (skipped %lu) (%.6lf sec / %.6lf sec total)\n",
                status,
                i+1,
                count,
                skipped,
                test_duration,
                total_time
        );
    }

    std::printf("\x1b[36mResults: \x1b[32m%lu passed. \x1b[31m%lu failed. \x1b[36m%lu skipped. Took %.6lf seconds\n\x1b[0m",
        passed,
        failed,
        skipped,
        total_time
    );
}
