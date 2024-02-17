#include <format>
#include <fstream>
#include <string>

#include <CLI/CLI.hpp>
#include <openssl/rand.h>
#include <range/v3/all.hpp>
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
    std::string file;

    CLI::App app("portfolio");

    app.add_option("-x,-c,--columns", x, "Columns")->group("Size");
    app.add_option("-y,-r,--rows", y, "Rows")->group("Size");
    app.add_option("-w,--word", word, "Word size")->group("Size");
    app.add_option("-o,--output-csv", file, "Output CSV file");

    spdlog::set_pattern("%v");

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

    std::string table{std::format("{:^{}}|", " ", 3)};
    std::string csv{" ,"};

    for (auto i = 0; i < x; ++i) {
        table += std::format("{:^{}}|", i, word + 2);
        csv += std::format("{}{}", i, (i < (x - 1)) ? ',' : '\n');
    }

    table += std::format("\n{:->{}}\n", "", (word + 3) * x + 4);

    for (auto &&[index, words] : views::enumerate(buffer | views::chunk(word) | views::chunk(x))) {
        auto v = words | to<std::vector<std::string_view>>();

        table += std::format("{:^{}}| {} |\n", index, 3, v | views::join(" | ") | to<std::string>());
        csv += std::format("{},{}\n", index, (v | views::join(',') | ::ranges::to<std::string>()));
    }

    table += std::format("{:->{}}\n", "", (word + 3) * x + 4);
    spdlog::info(table);

    if (!file.empty()) {
        std::ofstream output(file);
        output << csv;
    }

    return 0;
}
