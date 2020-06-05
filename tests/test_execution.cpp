#include <gtest/gtest.h>

#include "../compressor/include/seam.hpp"
#include "../ui/include/execution.hpp"

TEST(ExecutionParams, Default) {
    Execution exc;
    EXPECT_EQ(exc.getExecutionParams().advanced, execution::Params().advanced);
}

TEST(ExecutionParams, On) {
    Execution exc;
    exc.setPrettyView(false);
    EXPECT_EQ(exc.getExecutionParams().advanced, false);
}

TEST(ExecutionParams, Off) {
    Execution exc;
    exc.setPrettyView(true);
    EXPECT_EQ(exc.getExecutionParams().advanced, true);
}
