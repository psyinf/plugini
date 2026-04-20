#include <catch2/catch_session.hpp>

// artificial main to put all tests into one binary
int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}
