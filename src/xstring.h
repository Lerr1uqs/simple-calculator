#include <string>
#include <cstring>
#include <vector>
#include <optional>
#include <climits>


namespace squ {
#define self (*this)
#define fn auto

class String {
    friend struct hash<squ::String>;
    std::string inner_string;
  public:
    // 构造函数
    String(const char *s = ""): inner_string(s) {}
    String(std::string &s): inner_string(s) {}
    String(std::string &&s): inner_string(std::move(s)) {}
    // String(std::string s): inner_string(s) {}
    // 拷贝构造函数
    String(const String &other): inner_string(other.inner_string) {}
    // 移动构造函数
    String(String &&other): inner_string(std::move(other.inner_string)) {}
    // 拷贝赋值运算符
    String &operator =(const String &other) {
        self.inner_string = other.inner_string;
    }
    // 移动赋值运算符
    String &operator =(String &&other) {
        self.inner_string = std::move(other.inner_string);
    }
    // =
    bool operator =(const String &other) const {
        return self.inner_string == other.inner_string;
    }
    // 重载下标运算
    char operator [](ssize_t idx) const {
        if(idx >= self.len()) {
            throw std::runtime_error("over bountry");
        }
        return self.inner_string[idx];
    }
    String operator +(const char chr) {
        self.inner_string = self.inner_string + chr;
        return self;
    }
    bool operator ==(const String& other) const {
        return self.inner_string == other.inner_string;
    }

    /* ----------------- 特种函数 --------------------- */
    inline String substr(std::size_t __pos, std::size_t __n = std::numeric_limits<size_t>::max()) const {
        return String{self.inner_string.substr(__pos, __n)};
    }
    fn c_str() -> const char * {
        return self.inner_string.c_str();
    }
    /* ----------------- string函数 --------------------- */
    fn len() const -> ssize_t {
        return self.inner_string.length();
    }
    fn inner() const -> const std::string {
        return self.inner_string;
    }
    fn split(String &&delimiter) -> std::vector<String> {
        size_t pos_start = 0, pos_end, delim_len = delimiter.len();
        std::string token;
        std::vector<String> res;

        while ((pos_end = self.inner_string.find(delimiter.inner_string, pos_start)) != std::string::npos) {
            token = self.inner_string.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(String{token});
        }

        res.push_back(String{self.inner_string.substr(pos_start)});
        return res;
    }

    // fn split(const String &del) -> std::vector<String> {
    //     std::regex re{ del.inner_string };
    //     return std::vector<String> {
    //         std::sregex_token_iterator(self.inner_string.begin(), self.inner_string.end(), re, -1),
    //         std::sregex_token_iterator()
    //     };
    // }
    template<typename T>
    fn parse() -> std::optional<T> {
        try {
            T v = (T)std::atoi(self.inner_string.c_str());
            return std::optional{v};
        }catch (std::exception &err) {
            return std::nullopt;
        }
    }

    fn startwith(const String &sub) const {
        if(self.len() < sub.len()) {
            return false;
        }

        for(auto i = 0; i < sub.len(); i++) {
            if(self.inner_string[i] != sub[i]) 
                return false;
        }
        return true;
    }

};
    
} // namespace squ

// 在 std 命名空间内声明哈希特化 声明为友元让其可以访问
namespace std {
template <>
struct hash<squ::String> {
    std::size_t operator()(const squ::String& obj) const {
        return std::hash<std::string>()(obj.inner_string);
    }
};
}  // namespace std