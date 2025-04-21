#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <cstdlib>

class PeakyBlinders {
private:
    std::string sourcecode;
    std::vector<std::string> codelines;
    std::vector<std::string> recommendations;

    bool pollygray(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "ошибка: не удалось открыть файл " << filepath << std::endl;
            return false;
        }

        sourcecode.clear();
        codelines.clear();

        std::string line;
        while (std::getline(file, line)) {
            codelines.push_back(line);
            sourcecode += line + "\n";
        }
        file.close();
        return true;
    }

    void arthurdayn() {
        std::regex functionparampattern(R"((\w+\s+)+(\w+)\s*\(\s*(\w+(?:\s*\<[^>]+\>)?)\s+(\w+)\s*[\),])");

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];
            std::smatch matches;
            std::string::const_iterator searchstart(line.cbegin());

            while (std::regex_search(searchstart, line.cend(), matches, functionparampattern)) {
                std::string type = matches[3];
                std::string paramname = matches[4];

                if (type != "int" && type != "float" && type != "double" &&
                    type != "char" && type != "bool" &&
                    type.find("&") == std::string::npos &&
                    type.find("*") == std::string::npos) {

                    if (type.find("std::") != std::string::npos ||
                        type == "string" ||
                        type.find("vector") != std::string::npos ||
                        type.find("map") != std::string::npos) {
                        recommendations.push_back("строка " + std::to_string(i + 1) +
                            ": послушай внимательно. параметр '" + paramname +
                            "' передаётся по значению. в нашем деле такая расточительность непростительна. передавай по константной ссылке (const " + type + "&). ясно?");
                    }
                }

                searchstart = matches.suffix().first;
            }
        }
    }

    void johnshelby() {
        std::regex forloopsizepattern(R"(for\s*\(\s*\w+\s+\w+\s*=\s*[^;]+;\s*\w+\s*<\s*(\w+)\.size\(\)\s*;)");

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];
            std::smatch matches;

            if (std::regex_search(line, matches, forloopsizepattern)) {
                std::string container = matches[1];
                recommendations.push_back("строка " + std::to_string(i + 1) +
                    ": в этом городе время — деньги. кэшируй " + container + ".size() перед циклом. умный человек делает работу один раз. запомни это.");
            }
        }

        std::regex forlooppattern(R"(for\s*\(\s*\w+)");

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];

            if (std::regex_search(line, forlooppattern)) {
                for (size_t j = i + 1; j < i + 6 && j < codelines.size(); ++j) {
                    if (std::regex_search(codelines[j], forlooppattern)) {
                        recommendations.push_back("строки " + std::to_string(i + 1) +
                            "-" + std::to_string(j + 1) + ": вложенные циклы. опасная игра. " +
                            "у меня нет времени на медленный код. найди способ оптимизировать, или этот алгоритм станет твоей могилой.");
                        break;
                    }
                }
            }
        }
    }

    void finndickie() {
        std::regex iterwithoutautopattern(R"(std::\w+(?:\s*<[^>]*>)?::\w+(?:_iterator|::iterator)\s+(\w+)\s*=)");

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];
            std::smatch matches;

            if (std::regex_search(line, matches, iterwithoutautopattern)) {
                std::string itername = matches[1];
                recommendations.push_back("строка " + std::to_string(i + 1) +
                    ": умные люди не тратят время на лишние слова. используй 'auto' для итератора '" + itername +
                    "'. адаптируйся или погибнешь — таков закон бизнеса и кода.");
            }
        }
    }

    void michaelgray() {
        std::regex vectordeclpattern(R"(std::vector\s*<[^>]+>\s+(\w+)\s*;)");
        std::unordered_set<std::string> vectornames;
        std::unordered_map<std::string, bool> vectorwithreserve;

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];
            std::smatch matches;
            std::string::const_iterator searchstart(line.cbegin());

            while (std::regex_search(searchstart, line.cend(), matches, vectordeclpattern)) {
                vectornames.insert(matches[1]);
                vectorwithreserve[matches[1]] = false;
                searchstart = matches.suffix().first;
            }
        }

        std::regex reservepattern(R"((\w+)\.reserve\()");

        for (const auto& line : codelines) {
            std::smatch matches;
            std::string::const_iterator searchstart(line.cbegin());

            while (std::regex_search(searchstart, line.cend(), matches, reservepattern)) {
                std::string vectorname = matches[1];
                if (vectornames.find(vectorname) != vectornames.end()) {
                    vectorwithreserve[vectorname] = true;
                }
                searchstart = matches.suffix().first;
            }
        }

        std::regex forpushbackpattern(R"(for\s*\([^{]*\{[^}]*(\w+)\.push_back\()");
        std::regex pushbackpattern(R"((\w+)\.push_back\()");
        std::unordered_set<std::string> reportedvectors;

        for (size_t i = 0; i < codelines.size(); ++i) {
            if (codelines[i].find("for") != std::string::npos) {
                for (size_t j = i; j < i + 5 && j < codelines.size(); ++j) {
                    std::smatch matches;
                    if (std::regex_search(codelines[j], matches, pushbackpattern)) {
                        std::string vectorname = matches[1];
                        if (vectornames.find(vectorname) != vectornames.end() &&
                            !vectorwithreserve[vectorname] &&
                            reportedvectors.find(vectorname) == reportedvectors.end()) {
                            recommendations.push_back("строка " + std::to_string(j + 1) +
                                ": знаешь, что я делаю с теми, кто тратит мои ресурсы? вектор '" + vectorname +
                                "' нуждается в reserve перед push_back в цикле. планируй наперёд — так делают серьёзные люди.");
                            reportedvectors.insert(vectorname);
                        }
                    }
                }
            }
        }

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::smatch matches;
            std::string::const_iterator searchstart(codelines[i].cbegin());

            while (std::regex_search(searchstart, codelines[i].cend(), matches, pushbackpattern)) {
                std::string vectorname = matches[1];

                if (vectornames.find(vectorname) != vectornames.end() &&
                    !vectorwithreserve[vectorname] &&
                    reportedvectors.find(vectorname) == reportedvectors.end()) {

                    int pushbackcount = 0;
                    for (const auto& line : codelines) {
                        size_t pos = 0;
                        std::string searchstr = vectorname + ".push_back";
                        while ((pos = line.find(searchstr, pos)) != std::string::npos) {
                            pushbackcount++;
                            pos += searchstr.length();
                        }
                    }

                    if (pushbackcount > 2) {
                        recommendations.push_back("строка " + std::to_string(i + 1) +
                            ": по моим подсчётам, вектор '" + vectorname + "' используется " + std::to_string(pushbackcount) +
                            " раз. без reserve. непростительная слабость. исправь немедленно или понеси последствия.");
                        reportedvectors.insert(vectorname);
                    }
                }

                searchstart = matches.suffix().first;
            }
        }
    }

    void lukachangretta() {
        std::unordered_map<std::string, std::vector<size_t>> variabledeclarations;
        std::regex variabledeclarationpattern(R"((?:const\s+)?(\w+(?:\s*<[^>]+>)?)\s+([a-zA-Z_]\w*)\s*(?:=|;|\(.*?\)))");
        std::regex variableusagepattern(R"([a-zA-Z_]\w*)");

        for (size_t i = 0; i < codelines.size(); ++i) {
            std::string line = codelines[i];
            std::smatch matches;
            std::string::const_iterator searchstart(line.cbegin());

            while (std::regex_search(searchstart, line.cend(), matches, variabledeclarationpattern)) {
                std::string variablename = matches[2];
                variabledeclarations[variablename].push_back(i + 1);
                searchstart = matches.suffix().first;
            }
        }

        for (const auto& pair : variabledeclarations) {
            if (pair.second.size() > 1) {
                recommendations.push_back("строки " + tomhardy(pair.second) +
                    ": слишком много внимания одной переменной '" + pair.first + "'. "
                    "в этом коде должна быть только одна такая. разберись!");
            }
        }

        std::unordered_set<std::string> usedvariables;
        for (const auto& line : codelines) {
            std::smatch matches;
            std::string::const_iterator searchstart(line.cbegin());
            while (std::regex_search(searchstart, line.cend(), matches, variableusagepattern)) {
                std::string usedvar = matches[0];
                if (usedvar != "int" && usedvar != "float" && usedvar != "double" &&
                    usedvar != "char" && usedvar != "bool" && usedvar != "void" &&
                    usedvar != "class" && usedvar != "struct" && usedvar != "enum" &&
                    usedvar != "return" && usedvar != "if" && usedvar != "else" &&
                    usedvar != "for" && usedvar != "while" && usedvar != "do" &&
                    usedvar != "switch" && usedvar != "case" && usedvar != "break" &&
                    usedvar != "continue" && usedvar != "const" && usedvar != "auto" &&
                    usedvar.find("std::") == std::string::npos &&
                    usedvar.find("::") == std::string::npos &&
                    usedvar.find("(") == std::string::npos &&
                    usedvar.find(")") == std::string::npos &&
                    usedvar.find("{") == std::string::npos &&
                    usedvar.find("}") == std::string::npos &&
                    usedvar.find(";") == std::string::npos &&
                    usedvar.find("=") == std::string::npos &&
                    usedvar.find("<") == std::string::npos &&
                    usedvar.find(">") == std::string::npos &&
                    usedvar.find(".") == std::string::npos &&
                    usedvar.find(",") == std::string::npos) {
                    usedvariables.insert(usedvar);
                }
                searchstart = matches.suffix().first;
            }
        }

        for (const auto& pair : variabledeclarations) {
            const std::string& varname = pair.first;
            bool isused = usedvariables.count(varname);
            if (!isused) {
                recommendations.push_back("строки " + tomhardy(pair.second) +
                    ": эта переменная '" + varname + "' здесь просто пылится. "
                    "если она не нужна, избавься от неё. мёртвый код — плохой код.");
            }
        }
    }

    std::string tomhardy(const std::vector<size_t>& linenumbers) const {
        std::stringstream ss;
        for (size_t i = 0; i < linenumbers.size(); ++i) {
            ss << linenumbers[i];
            if (i < linenumbers.size() - 1) {
                ss << ", ";
            }
        }
        return ss.str();
    }

public:
    bool thomasshelby(const std::string& filepath) {
        if (!pollygray(filepath)) {
            return false;
        }

        recommendations.clear();

        arthurdayn();
        johnshelby();
        finndickie();
        michaelgray();
        lukachangretta();

        return true;
    }

    const std::vector<std::string>& mayonez() const {
        return recommendations;
    }
};

int main(int argc, char* argv[]) {
    std::system("chcp 65001");

    if (argc < 2) {
        std::cerr << "использование: " << argv[0] << " <путь_к_cpp_файлу>" << std::endl;
        return 1;
    }

    PeakyBlinders analyzer;
    std::string filepath = argv[1];

    if (!analyzer.thomasshelby(filepath)) {
        std::cerr << "не удалось проанализировать файл: " << filepath << std::endl;
        return 1;
    }

    const auto& recommendations = analyzer.mayonez();

    if (recommendations.empty()) {
        std::cout << "код чистый, как виски в моём стакане. продолжай в том же духе." << std::endl;
    } else {
        std::cout << "я нашёл " << recommendations.size() << " вещей, которые требуют нашего внимания:" << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;

        for (size_t i = 0; i < recommendations.size(); ++i) {
            std::cout << i + 1 << ". " << recommendations[i] << std::endl;
        }

        std::cout << "--------------------------------------------------------" << std::endl;
        std::cout << "исправь это к следующему разу. по рукопожатию можно понять человека. по коду — программиста." << std::endl;
    }

    return 0;
}
