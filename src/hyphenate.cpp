#include <stdio.h>
#include <sys/poll.h>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <hyphen.h>
#include "config.h"

const char* regex_error_description(int id) {
    switch (id) {
    case 0: return "error_collate (the expression contains an invalid collating element name)";
    case 1: return "error_ctype (the expression contains an invalid character class name)";
    case 2: return "error_escape (the expression contains an invalid escaped character or a trailing escape)";
    case 3: return "error_backref (the expression contains an invalid back reference)";
    case 4: return "error_brack (the expression contains mismatched square brackets)";
    case 5: return "error_paren (the expression contains mismatched parentheses)";
    case 6: return "error_brace (the expression contains mismatched curly braces)";
    case 7: return "error_badbrace (the expression contains an invalid range in a {} expression)";
    case 8: return "error_range (the expression contains an invalid character range, e.g. [b-a])";
    case 9: return "error_space (there was not enough memory to convert the expression into a finite state machine)";
    case 10: return "error_badrepeat (one of *?+{ was not preceded by a valid regular expression)";
    case 11: return "error_complexity (the complexity of an attempted match exceeded a predefined level)";
    case 12: return "error_stack (there was not enough memory to perform a match)";
    default: return "unknown error";
    }
}

// const auto SOFTHYPHEN = "&shy;";
const auto SOFTHYPHEN = "\u00AD";

void usage() {
    fprintf(stderr,
            "\nhyphenate v%d.%d.%d\nInserts Unicode soft hyphens in text from stdin, to stdout.\nThe language argument is optional, if omitted the current user language is tried.\n\nUsage: cat text.txt | hyphenate [lang_LANG] > hyphenated.txt \n\n",
            hyphenate_VERSION_MAJOR,
            hyphenate_VERSION_MINOR,
            hyphenate_VERSION_PATCH);
}

int main (int argc, char *argv[]) {
    std::string dictFile (HYPHEN_DICT_LOC);

    std::string lang ("en_US");

    // See if we have stdin waiting: https://stackoverflow.com/a/6839581
    // struct pollfd fds;
    // int ret;
    // fds.fd = 0; /* this is STDIN */
    // fds.events = POLLIN;
    // ret = poll(&fds, 1, 0);
    // if(ret == 0) {
    //     usage();
    //     return 0;
    // }

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage();
            return 1;
        } else {
            lang = std::string(argv[1]);
        }
    } else {
        if (const auto env_lang = std::getenv("LANG")) {
            std::string s (env_lang);
            try {
                const std::regex    lang_regex("^[a-zA-Z_-]+");
                std::smatch         lang_match;
                if (std::regex_search(s, lang_match, lang_regex) && lang_match.size() > 0) {
                    lang = lang_match.str(0);
                }
            } catch (const std::regex_error& e) {
                std::cerr   << "Error "
                            << e.code()
                            << " parsing LANG variable content '"
                            << env_lang
                            << "': "
                            << regex_error_description(e.code())
                            << '\n';
            }
        }

        std::cerr << "No language specified, using system language: " << lang << "\n";
    }

    dictFile.append("hyph_");
    dictFile.append(lang);
    // dictFile.append("en_US");
    dictFile.append(".dic");

    auto dict = hnj_hyphen_load(dictFile.c_str());

    if (dict == NULL) {
        std::cerr << "Error loading dictionary file: " << dictFile << "\n";
        return 2;
    } else {
        std::cerr << "Successfully loaded dictionary file: " << dictFile << "\n";
    }

    std::string line;
    char hword[2000];
    char ** rep;
    int *   pos;
    int *   cut;
    uint    lineCount = 0;

    while (std::getline(std::cin, line)) {
        ++lineCount;
        try {
            //TODO: Move outside of while loop, but in try block:
            const std::regex    words_regex(R"(([\S\s\r\n]*?)([a-zA-ZÀ-ÿ]{4,}|$))",
                                            std::regex_constants::icase);

            auto words_begin    = std::sregex_iterator(line.begin(), line.end(), words_regex);
            auto words_end      = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match   = *i;
                auto nonword        = match[1].str();
                auto word           = match[2].str();
                auto size           = word.size();

                if (size >= 4) {
                    char hyphens[size + 5];
                    rep = NULL;
                    pos = NULL;
                    cut = NULL;
                    hword[0] = '\0';

                    auto ret = hnj_hyphen_hyphenate3(
                                                        dict,
                                                        word.c_str(),
                                                        size,
                                                        hyphens,
                                                        hword,
                                                        &rep,
                                                        &pos,
                                                        &cut,
                                                        2,
                                                        2,
                                                        2,
                                                        2);
                    if (ret > 0) {
                        std::cerr   << "Error hyphenating: "
                                    << ret
                                    << '\n';
                        return 3;
                    }

                    std::string hyphed (hword);

                    std::size_t found = hyphed.find("=");
                    while (found != std::string::npos) {
                        hyphed.replace(found, 1, SOFTHYPHEN);
                        found = hyphed.find("=", found);
                    }

                    std::cout   << nonword
                                << hyphed;
                } else {
                    std::cout   << nonword
                                << match[2].str();
                }

            }

            std::cout << "\n";

        } catch (const std::regex_error& e) {
            std::cerr   << "Error "
                        << e.code()
                        << " parsing input line '"
                        << line
                        << "': "
                        << regex_error_description(e.code())
                        << '\n';
            return 4;
        }
    }

    return 0;
}
