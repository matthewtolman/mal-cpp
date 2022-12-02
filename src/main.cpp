#include "mal.h"
#include <string>
#include "linenoise.h"
#include <iostream>
#include <utility>

auto simplePrompt(const char* prompt, std::string& line) -> decltype(auto) {
    std::cout << prompt;
    return std::getline(std::cin, line);
}


//auto completion(const char *buf, linenoiseCompletions *lc) -> void {
//    if (buf[0] == 'h') {
//        linenoiseAddCompletion(lc,"hello");
//        linenoiseAddCompletion(lc,"hello there");
//    }
//}
//
//auto freeHint(void * hint) -> void {
//    delete [](static_cast<char*>(hint));
//}
//
//auto hints(const char *buf, int *color, int *bold) -> char * {
//    if (!strcasecmp(buf,"hello")) {
//        *color = 35;
//        *bold = 0;
//        return new char[] {" there"};
//    }
//    return nullptr;
//}

auto main(int argc, char** argv) -> int {
    bool reducedView = argc > 1 && strcmp(argv[1], "--simple") == 0;
    auto env = std::make_shared<mal::MalEnv>();

    if (reducedView) {
        std::string line;
        while (simplePrompt("user> ", line)) {
            try {
                std::cout << mal::rep(env, line) << "\n";
            } catch (const std::runtime_error& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
    }
    else {
        linenoiseSetMultiLine(1);

//        linenoiseSetCompletionCallback(completion);
//        linenoiseSetHintsCallback(hints);
//        linenoiseSetFreeHintsCallback(freeHint);

        linenoiseHistorySetMaxLen(20);

        char *line;

        while ((line = linenoise("user> ")) != nullptr) {
            linenoiseHistoryAdd(line);

            if (line[0] != '\0') {
                try {
                printf("%s\n", mal::rep(env, line).c_str());
                } catch (const std::runtime_error& e) {
                    printf("Error: %s, \n", e.what());
                }
            }

            linenoiseFree(line);
        }
    }
}
