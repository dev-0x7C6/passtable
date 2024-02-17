#include <iostream>
#include <string>
#include <format>

#include <range/v3/algorithm/shuffle.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/sample.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/take.hpp>

#include <CLI/CLI.hpp>
#include <openssl/rand.h>
#include <spdlog/spdlog.h>

namespace openssl {
class rng {
public:
    using result_type = unsigned int;

    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    result_type operator()() {
        result_type result;
        RAND_bytes(reinterpret_cast<unsigned char *>(&result), sizeof(result));
        return result;
    }
};
} // namespace openssl

namespace query::us {
constexpr std::string_view letters_lowercase = "qwertyuiopasdfghjklzxcvbnm";
constexpr std::string_view letters_uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
constexpr std::string_view miscellaneous = "{}|:\"<>?";
constexpr std::string_view numbers = "1234567890";
constexpr std::string_view operators = "~!@#$%^&*()_+";
constexpr std::string_view punctuation = "`-=[]\\;',./";

auto keys() {
    return ::ranges::views::concat(letters_lowercase, letters_uppercase, numbers);
}
} // namespace query::us

int main(int argc, const char **argv) {
    RAND_poll();

    int x{10};
    int y{10};
    int word{5};

    CLI::App app("portfolio");

    app.add_option("-x,-c,--columns", x, "Columns")->group("Size");
    app.add_option("-y,-r,--rows", y, "Rows")->group("Size");
    app.add_option("-w,--word", y, "Word size")->group("Size");

    try {
        app.parse(argc, argv);
    } catch (...) {
        spdlog::info(app.help());
        return 1;
    }

    using namespace ::ranges;
    const auto size = x * y * word;

    auto buffer = query::us::keys() | views::cycle | views::take(size) | to<std::string>();
    shuffle(buffer, openssl::rng{});

    std::cout << std::format("{:^{}}|", " ", 3);
    for (auto i = 0; i < x; ++i)
        std::cout << std::format("{:^{}}|", i, word + 2);

    std::cout << std::endl;
    std::cout << std::format("{:->{}}", "", word * x + static_cast<int>(x * 3.49)) << std::endl;

    for (auto &&[index, words] : views::enumerate(buffer | views::chunk(word) | views::chunk(x))) {
        std::cout << std::format("{:^{}}| ", index, 3);

        for (auto word : words)
            std::cout << (word | ::ranges::to<std::string>()) << " | ";

        std::cout << std::endl;
    }

    std::cout << std::format("{:->{}}", "", word * x + static_cast<int>(x * 3.49)) << std::endl;
    return 0;
}
