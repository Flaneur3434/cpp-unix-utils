#include <algorithm>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <charconv>
#include <optional>

class CmdLineParser{
public:
	using Arg = std::variant<std::string, int>;

	explicit CmdLineParser (int& argc, char *argv[]){
		CmdOptionsFromStdin(argc, argv);
	}

	void CmdOptionsFromStdin(int& argc, char *argv[]);
	Arg TryParseString(std::string_view& arg);
	std::optional<Arg> getCmdOptions(std::string&& getStr);


private:
	std::unordered_map<std::string, Arg> mParsedArgs;
};

void CmdLineParser::CmdOptionsFromStdin(int& argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string_view cmd = std::string_view(argv[i]);
		size_t pos = cmd.find('-');
		
		if (pos == std::string_view::npos) {
			std::cout << "Wrong command line argument" << std::endl;
		}

		cmd = cmd.substr(pos + 1);
		mParsedArgs[std::string(cmd.data(), cmd.size())] = TryParseString(cmd);
    }
}

CmdLineParser::Arg CmdLineParser::TryParseString(std::string_view& arg) {
	int result{};
	const auto [ptr, ec] = std::from_chars(arg.data(), arg.data() + arg.size(), result);
	if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
		std::cerr << "Error parsing interger: " << arg << std::endl;
		return std::string(arg.data(), arg.size());
	}

	return result;
}

std::optional<CmdLineParser::Arg> CmdLineParser::getCmdOptions(std::string&& getStr) {
	auto cmd = mParsedArgs.find(getStr);
	if (cmd == mParsedArgs.end()) {
		return {};
	}

	return cmd->first;
}

int main(int argc, char *argv[]){
	auto clp = CmdLineParser(argc, argv);
	auto get = clp.getCmdOptions(std::string("1"));
	if (!get) {
		std::cout << "did not find matching argument" << std::endl;
		return -1;
	}

	std::visit([](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, std::string>) {
			std::cout << "Found argument: " << arg << std::endl;
		}
		else if constexpr (std::is_same_v<T, int>) {
			std::cout << "Found argument: " << arg << std::endl;
		}
	}, *get);
}
