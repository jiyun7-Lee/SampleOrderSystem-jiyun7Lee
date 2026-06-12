add_executable(SampleOrderSystemTests
    domain/DomainModelTest.cpp
    domain/TimeProviderTest.cpp
)

target_include_directories(SampleOrderSystemTests PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(SampleOrderSystemTests
    gtest_main
    gmock
)

include(GoogleTest)
gtest_discover_tests(SampleOrderSystemTests)
