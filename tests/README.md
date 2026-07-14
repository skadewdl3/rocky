# Testing in Rocky

Rocky uses a two-tier testing strategy: **Unity** for unit testing C code and **Lit** for end-to-end integration testing of the compiler pipeline.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Folder Structure](#folder-structure)
- [Unit Testing with Unity](#unit-testing-with-unity)
- [Integration Testing with Lit](#integration-testing-with-lit)
- [Running Tests](#running-tests)
- [Guidelines](#guidelines)

---

## Prerequisites

Ensure the following tools are installed:

- **CMake** >= 3.20
- **Pixi** — [installation guide](https://prefix.dev/docs/pixi/overview)
- **LLVM Tools** (lit, FileCheck) — provided automatically via Pixi.

---

## Folder Structure

```
tests/
├── arena.c             # Unity unit tests for the arena allocator
├── lexer.c             # Unity unit tests for the lexer
├── parser.c            # Unity unit tests for the parser/AST
└── lit/                # Lit integration tests (.rocky files)
    ├── ast/            # AST dump checks (uses %rocky --dump-ast)
    └── ir/             # IR generation tests
        └── add.rocky
```

---

## Unit Testing with Unity

Unity is used for fine-grained testing of C modules.

### Writing Unity Tests

1. Create a .c file in a subdirectory of tests/ (e.g., tests/my_feature/test_feature.c).
2. Include unity.h and your module header.
3. Implement setUp() and tearDown().
4. Use RUN_TEST in main().

**Example:**
```c
#include "unity.h"
#include <rocky/arena.h>

void setUp(void) {}
void tearDown(void) {}

void test_allocation(void) {
    Arena a;
    arena_init(&a, 1024);
    TEST_ASSERT_NOT_NULL(arena_alloc(&a, 16));
    arena_free(&a);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_allocation);
    return UNITY_END();
}
```

---

## Integration Testing with Lit
### Writing Lit Tests

1. Create a .rocky file under tests/lit/.
2. Add RUN: and CHECK: directives.

- RUN: the command to run. Use `%rocky` for the real rocky binary, or `%parser` for the fake IR helper.
- CHECK: what the output should look like.

**Example AST test (tests/lit/ast/ast_binary.rocky):**
```
// RUN: %rocky --dump-ast -c "1 + 2" | FileCheck %s --check-prefix=ADD

// ADD: +
// ADD-NEXT: ├── 1
// ADD-NEXT: └── 2
```

**Example IR test (tests/lit/ir/add.rocky):**
```llvm
// RUN: %parser %s | FileCheck %s

// CHECK: define i32 @main()
// CHECK: %{{.*}} = add i32 2, 3
// CHECK: ret i32 %{{.*}}

fn main() {
    return 2 + 3;
}
```

CMake fills in `%rocky` and `%parser` with the real paths from your build folder.

---

## Running Tests

Tests are executed using Pixi tasks, which wrap ctest.

### Run All Tests
```sh
pixi run test
```

### Run Unity Tests Only
You can filter by path-based labels:
```sh
pixi run test arena   # Runs arena tests
pixi run test lexer   # Runs lexer tests
```

### Run Lit Tests Only
```sh
pixi run lit
```
To run specific lit tests:
```sh
pixi run lit ir/add   # Runs tests matching lit/ir/add
```

### Test Labels
Labels are derived from the relative path under tests/:
- Unity: tests/lexer.c -> lexer
- Lit: tests/lit/ir/add.rocky -> lit/ir/add

---

## Guidelines

- **Automatic Discovery**: New .c files in tests/ and .rocky files in tests/lit/ are detected automatically when you run pixi run configure (or pixi run test which depends on it).
- **Independence**: Tests must not depend on each other.
- **Verification**: Always run pixi run test before submitting changes.
