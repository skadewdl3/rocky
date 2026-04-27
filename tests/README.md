# Unit Testing with Unity

A scalable, zero-configuration unit testing setup for C projects using the [Unity](https://github.com/ThrowTheSwitch/Unity) framework, [CMake](https://cmake.org/) for automatic test discovery, and [Pixi](https://prefix.dev/) for task execution.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Folder Structure](#folder-structure)
- [Writing Tests](#writing-tests)
- [Running Tests](#running-tests)
- [How It Works](#how-it-works)
- [Guidelines](#guidelines)

---

## Prerequisites

Ensure the following tools are installed before getting started:

- **CMake** ≥ 3.15
- **Pixi** — [installation guide](https://prefix.dev/docs/pixi/overview)
- A C compiler (GCC, Clang, or MSVC)

---

## Getting Started

Clone the repository and initialize the Unity submodule:

```sh
git clone <your-repo-url>
cd <your-repo>
git submodule update --init --recursive
```

This fetches the Unity testing framework into `external/unity/`.

---

## Folder Structure

Tests live inside the `tests/` directory. Each component or feature area gets its own subdirectory:

```
tests/
├── core/
│   ├── test_math.c
│   └── test_string.c
├── parser/
│   └── ast/
│       └── test_ast.c
└── renderer/
    └── test_render.c
```

| Rule | Detail |
|---|---|
| One test file per module | Keep tests focused and isolated |
| Naming convention | `test_<feature>.c` |
| Location | `tests/<component>/test_<feature>.c` |

> **No manual CMake changes are needed.** The build system automatically discovers all `.c` files under `tests/`.

---

## Writing Tests

Each test file follows the same structure:

1. Include the Unity header
2. Include the module header under test
3. Implement `setUp()` and `tearDown()` (can be empty)
4. Write test functions using Unity assertions
5. Call `RUN_TEST()` for each test inside `main()`

### Example

```c
#include "unity.h"
#include "rocky/math.h"

void setUp(void) {}
void tearDown(void) {}

void test_add_positive_numbers(void) {
    TEST_ASSERT_EQUAL(5, add(2, 3));
}

void test_add_negative_numbers(void) {
    TEST_ASSERT_EQUAL(-1, add(2, -3));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_positive_numbers);
    RUN_TEST(test_add_negative_numbers);
    return UNITY_END();
}
```

### Common Unity Assertions

| Assertion | Description |
|---|---|
| `TEST_ASSERT_EQUAL(expected, actual)` | Integer equality |
| `TEST_ASSERT_EQUAL_FLOAT(expected, actual)` | Float equality |
| `TEST_ASSERT_EQUAL_STRING(expected, actual)` | String equality |
| `TEST_ASSERT_TRUE(condition)` | Condition is true |
| `TEST_ASSERT_FALSE(condition)` | Condition is false |
| `TEST_ASSERT_NULL(pointer)` | Pointer is NULL |
| `TEST_ASSERT_NOT_NULL(pointer)` | Pointer is not NULL |

For the full list, see the [Unity Assertion Reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md).

---

## Running Tests

Tests are executed using Pixi. Tests are labeled by their folder and file path, enabling flexible filtering.

### Run all tests

```sh
pixi run test
```

### Run tests for a specific component

```sh
pixi run test core
```

### Run a specific test file

```sh
pixi run test core/test_math
```

### Test Labels

Test labels are derived directly from file paths:

| File | Label |
|---|---|
| `tests/core/test_math.c` | `core/test_math` |
| `tests/parser/ast/test_ast.c` | `parser/ast/test_ast` |

---

## How It Works

### Automatic Test Discovery

CMake scans the entire `tests/` directory for `.c` files at build time. Each discovered file is compiled into its own standalone test executable — no manual registration required.

### Source Linking

Source files from `src/` are automatically linked into each test executable, so tested modules are always available without extra configuration.

### Conflict Prevention

`main.c` from the application source is excluded from test builds to prevent `multiple definition of 'main'` linker errors.

### Build Flow

```
tests/**/*.c  ──► CMake discovery ──► compile each file ──► link with src/ ──► test executables
                                
pixi run test ────► run & report
```

---

## Guidelines

- **Run tests before pushing** — never push code with failing tests
- **Keep tests independent** — tests must not depend on the execution order of other tests
- **One module per file** — each test file should test exactly one source module
- **No application logic in tests** — test files should only contain test code and assertions
- **Follow naming conventions** — use `test_<feature>.c` and place files under `tests/<component>/`
- **Write meaningful test cases** — test edge cases, not just the happy path

---

## Adding New Tests

Simply create a new `.c` file in the appropriate `tests/` subdirectory:

```sh
touch tests/core/test_new_feature.c
```

The build system will automatically detect and include it on the next build. **No CMake changes required.**

---
