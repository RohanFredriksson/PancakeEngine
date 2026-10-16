#pragma once

// A minimal test harness: each TEST registers a function, and RUN_TESTS runs them all.

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <functional>

namespace PancakeTest {

    struct Case {
        const char* name;
        std::function<void()> body;
    };

    inline std::vector<Case>& cases() {
        static std::vector<Case> instance;
        return instance;
    }

    inline int& failures() {
        static int count = 0;
        return count;
    }

    struct Registrar {
        Registrar(const char* name, std::function<void()> body) {
            cases().push_back({name, body});
        }
    };

    inline int run() {
        for (const Case& c : cases()) {
            int before = failures();
            c.body();
            std::printf("[%s] %s\n", failures() == before ? " OK " : "FAIL", c.name);
        }
        std::printf("%zu tests, %d failed checks\n", cases().size(), failures());
        std::fflush(stdout);
        return failures() == 0 ? 0 : 1;
    }

}

#define PANCAKE_TEST_CONCAT_IMPL(a, b) a##b
#define PANCAKE_TEST_CONCAT(a, b) PANCAKE_TEST_CONCAT_IMPL(a, b)

#define TEST(name) \
    static void PANCAKE_TEST_CONCAT(test_, __LINE__)(); \
    static PancakeTest::Registrar PANCAKE_TEST_CONCAT(registrar_, __LINE__)(name, PANCAKE_TEST_CONCAT(test_, __LINE__)); \
    static void PANCAKE_TEST_CONCAT(test_, __LINE__)()

#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            std::printf("    %s:%d: CHECK(%s) failed\n", __FILE__, __LINE__, #condition); \
            PancakeTest::failures()++; \
        } \
    } while (0)

#define CHECK_NEAR(a, b, eps) \
    do { \
        double pancake_a = (a), pancake_b = (b); \
        if (!(std::fabs(pancake_a - pancake_b) <= (eps))) { \
            std::printf("    %s:%d: CHECK_NEAR(%s, %s) failed: %f vs %f\n", __FILE__, __LINE__, #a, #b, pancake_a, pancake_b); \
            PancakeTest::failures()++; \
        } \
    } while (0)

#define RUN_TESTS() int main() { return PancakeTest::run(); }
