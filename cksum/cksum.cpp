#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <charconv>
#include <utility>

class CmdLineParser{
public:
	explicit CmdLineParser (int& argc, char *argv[]){
		CmdOptionsFromStdin(argc, argv);
	}
	~CmdLineParser();

	using Arg = std::variant<std::string, int>;
	void CmdOptionsFromStdin(int& argc, char *argv[]);
	Arg TryParseString(std::string_view&& arg);
	std::optional<Arg> getCmdOptions(const std::string& getStr);

	std::unordered_map<std::string, Arg> mParsedArgs;
};

CmdLineParser::~CmdLineParser() {
	mParsedArgs.~unordered_map();
}

void CmdLineParser::CmdOptionsFromStdin(int& argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		std::string_view cmd = std::string_view(argv[i]);
		size_t pos = cmd.find('-');
		if (pos != std::string_view::npos) {
			// arguments with `-` are flags
			cmd = cmd.substr(pos + 1);
			mParsedArgs[std::string(cmd.data(), cmd.size())] = TryParseString(std::string_view(argv[++i]));
		} else {
			// else argument is assumed to be file name
			mParsedArgs["filename"] = TryParseString(std::move(cmd));
		}
	}
}

CmdLineParser::Arg CmdLineParser::TryParseString(std::string_view&& arg) {
	int result{};
	const auto [ptr, ec] = std::from_chars(arg.data(), arg.data() + arg.size(), result);
	if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
		return std::string(arg.data(), arg.size());
	}

	return result;
}

std::optional<CmdLineParser::Arg> CmdLineParser::getCmdOptions(const std::string& getStr) {
	auto cmd = mParsedArgs.find(getStr);
	if (cmd == mParsedArgs.end()) {
		return {};
	}

	return cmd->second;
}

int main(int argc, char *argv[]){
	auto clp = CmdLineParser(argc, argv);
	auto fn = clp.getCmdOptions("filename");
	if (!fn) {
		std::cerr << "No file is given: aborting process!" << std::endl;
		return -1;
	}

	auto algo_ver = clp.getCmdOptions("o");
	if (!algo_ver) {
		// TODO: default algorithm is choosen
		return 0;
	}

	std::cout << "using historic algorithms instead of the superior default one." << std::endl;
	switch(std::get<int>(*algo_ver)) {
	case 1:
		puts("This is a 16-bit checksum, with a right rotation before each addition");
		break;
	case 2:
		puts("This is a 32-bit check‐sum.");
		break;
	case 3:
		puts("Algorithm 3 is what is commonly called the ‘32bit CRC’ algorithm.");
		break;
	default:
		std::cerr << "illegal argument to -o option" << std::endl;
	}
}
