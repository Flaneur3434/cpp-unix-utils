#include <algorithm>
#include <ios>
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
#include <fstream>

#include "extern.h"

class CmdLineParser{
public:
	explicit CmdLineParser (int& argc, char *argv[]) noexcept {
		CmdOptionsFromStdin(argc, argv);
	}

	~CmdLineParser();

	void CmdOptionsFromStdin(int argc, char *argv[]) const noexcept;

	using Arg = std::variant<std::string, int>;
	Arg TryParseString(const std::string& arg) const noexcept;
	std::optional<Arg> getCmdOptions(const std::string& getStr) const noexcept;

private:
	mutable std::unordered_map<std::string, Arg> mParsedArgs;
};

CmdLineParser::~CmdLineParser() {
	mParsedArgs.~unordered_map();
}

void CmdLineParser::CmdOptionsFromStdin(int argc, char **argv) const noexcept {
	for (int i = 1; i < argc; i++) {
		const std::string cmd = std::string(argv[i]);
		if (cmd == "-?") {
			mParsedArgs["help"] = TryParseString(cmd);
			return;
		}

		const size_t pos = cmd.find('-');
		if (pos != std::string_view::npos) {
			// arguments with `-` are flags
			const std::string flag = cmd.substr(pos + 1);
			mParsedArgs[flag] = TryParseString(std::string(argv[++i]));
		} else {
			// else argument is assumed to be file name
			mParsedArgs["filename"] = TryParseString(cmd);
		}
	}
}

CmdLineParser::Arg CmdLineParser::TryParseString(const std::string& arg) const noexcept {
	int result{};
	const auto [ptr, ec] = std::from_chars(arg.data(), arg.data() + arg.size(), result);
	if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
		return std::string(arg.data(), arg.size());
	}

	return result;
}

std::optional<CmdLineParser::Arg> CmdLineParser::getCmdOptions(const std::string& getStr) const noexcept {
	const auto cmd = mParsedArgs.find(getStr);
	if (cmd == mParsedArgs.end()) {
		return {};
	}

	return cmd->second;
}

static void usage() noexcept {
	std::cerr << "usage: cksum [-o 1 | 2 | 3] [file ...]" << std::endl;
	std::cerr << "       sum [file ...]" << std::endl;
	exit(1);
}

int main(int argc, char *argv[]) {
	// https://www.cprogramming.com/tutorial/const_correctness.html
	const auto clp = CmdLineParser(argc, argv);

	const auto help_flag = clp.getCmdOptions("help");
	if (help_flag) {
		usage();
	}

	const auto file_mabye = clp.getCmdOptions("filename");
	if (not file_mabye) {
		std::cerr << "No file is given: aborting process!" << std::endl;
		return -1;
	}

	std::ifstream fd;
	const auto filename = std::get<std::string>(*file_mabye);
	fd.open(filename, std::ios_base::in);
	if (not fd) {
		std::cerr << "Could not open file: " << filename << std::endl;
	}

	const auto algo_ver = clp.getCmdOptions("o");
	if (not algo_ver) {
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
