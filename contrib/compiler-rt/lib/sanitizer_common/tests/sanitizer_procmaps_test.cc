//===-- sanitizer_procmaps_test.cc ----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of ThreadSanitizer/AddressSanitizer runtime.
//
//===----------------------------------------------------------------------===//
#if !defined(_WIN32)  // There are no /proc/maps on Windows.

#include "sanitizer_common/sanitizer_procmaps.h"
#include "gtest/gtest.h"

#include <stdlib.h>

static void noop() {}
extern const char *argv0;

namespace __sanitizer {

#if SANITIZER_LINUX && !SANITIZER_ANDROID
TEST(MemoryMappingLayout, CodeRange) {
  uptr start, end;
  bool res = GetCodeRangeForFile("[vdso]", &start, &end);
  EXPECT_EQ(res, true);
  EXPECT_GT(start, 0U);
  EXPECT_LT(start, end);
}
#endif

TEST(MemoryMappingLayout, DumpListOfModules) {
  const char *last_slash = strrchr(argv0, '/');
  const char *binary_name = last_slash ? last_slash + 1 : argv0;
  MemoryMappingLayout memory_mapping(false);
  const uptr kMaxModules = 100;
  LoadedModule *modules =
      (LoadedModule *)malloc(kMaxModules * sizeof(LoadedModule));
  uptr n_modules = memory_mapping.DumpListOfModules(modules, kMaxModules, 0);
  EXPECT_GT(n_modules, 0U);
  bool found = false;
  for (uptr i = 0; i < n_modules; ++i) {
    if (modules[i].containsAddress((uptr)&noop)) {
      // Verify that the module name is sane.
      if (strstr(modules[i].full_name(), binary_name) != 0)
        found = true;
    }
    modules[i].clear();
  }
  EXPECT_TRUE(found);
  free(modules);
}

}  // namespace __sanitizer
#endif  // !defined(_WIN32)
