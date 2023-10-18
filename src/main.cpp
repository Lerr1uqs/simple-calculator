#include <iostream>
#include <future>
#include <thread>
#include <vector>
#include <optional>
#include <cassert>
#include <stack>
#include <map>
#include <exception>
using namespace std;

const map<char, function<int(int, int)>> opts {  // 将Lambda存入容器形成”命令集“
    {'+', [] (int lv, int rv) { return lv + rv; }},
    {'-', [] (int lv, int rv) { return lv - rv; }},
    {'*', [] (int lv, int rv) { return lv * rv; }},
    {'/', [] (int lv, int rv) { return lv / rv; }}
};


#include "xstring.h"

using namespace squ;
class Token {
  public:
    enum Type {
        ID = 0,
        NUM,
        OP,
    };
    String id;
    int num;
    char op;

    Type type;
    Token(const String &__id): id("") {
        self.type = ID;
        self.id = __id;
    }
    Token(int __num): id("") {
        self.type = NUM;
        self.num = __num;
    }
    Token(char __op): id("") {
        self.type = OP;
        self.op = __op;
    }
    ~Token() {};

  private:

    /*
        op_prior[-][*] = '<'
        代表 -的优先级 小于 <
        ) 不参与优先级的比较 
        ( 比任何优先级都要高
    */
    static constexpr char OPS[] = {'+', '-', '*', '/', '(', ')', '$'};
    static constexpr int OP_NUMS = sizeof(OPS) / sizeof(char);

    char op_prior[OP_NUMS][OP_NUMS] = { // 优先级表 prior[stack][new op]
        /*       -    +    *    /    (    )    $ */
        /* - */ '>', '>', '<', '<', '<', '>', '>',
        /* + */ '>', '>', '<', '<', '<', '>', '>',
        /* * */ '>', '>', '>', '>', '<', '>', '>',
        /* / */ '>', '>', '>', '>', '<', '>', '>',
        /* ( */ '<', '<', '<', '<', '<', '=', '>',
        /* ) */ ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        /* $ */ '<', '<', '<', '<', '<', ' ', '=',
    };
  public:
    char order_compare(char chr2) const {
        
        assert(self.type == OP);
        char chr1 = self.op;

        int c1 = -1, c2 = -1;
        for(int i = 0; i < OP_NUMS; ++i){
            if(chr1 == OPS[i]) c1 = i;
            if(chr2 == OPS[i]) c2 = i;
        }

        return op_prior[c2][c1];
    }

};


fn real_start(std::vector<Token> &tokens) -> void {
    // a + b * c - (d / e)
    stack<int> num_stack{};
    stack<char> op_stack{};
    op_stack.push('$');

    for(auto token_iter = tokens.begin(); token_iter != tokens.end();) {
        switch (token_iter->type)
        {
            case Token::NUM : {
                num_stack.push(token_iter->num);
                token_iter++;
                break;
            }
            case Token::OP : {
                switch (token_iter->order_compare(op_stack.top())) {
                    case '>': /*  cur op > stack */ {
                        // 栈中优先级更高
                        auto n2 = num_stack.top();num_stack.pop();
                        auto n1 = num_stack.top();num_stack.pop();

                        char op = op_stack.top();op_stack.pop();

                        std::cout << n1 << " " << op << " " << n2 << " = " << opts.at(op)(n1, n2) << endl;
                        num_stack.push(opts.at(op)(n1, n2));// 计算后压入栈中 

                        break;
                    }

                    case '=':// 括号遇到括号 delimiter遇到delimiter 本次token舍去 同时从栈中舍去对应的op
                        op_stack.pop();
                        token_iter++;
                        break;
                    case '<':
                        // 当前优先级更高
                        op_stack.push(token_iter->op);
                        token_iter++;
                        break;
                    default:
                        throw runtime_error("unreachable #1");
                }
                break;
            default:
                throw runtime_error("unreachable #2");
            }
        }
        // std::cout << token_iter->num << endl;
    }

    while(not num_stack.empty()) {
        std::cout << num_stack.top() << endl;
        num_stack.pop();
    }

}
#include <unordered_map>
fn replace_id_with_num(std::vector<Token> &tokens) -> void {

    unordered_map<String, int> identifiers = {// 自己后续修改或者运行时获取
        {"a", 1},
        {"b", 2},
        {"c", 3},
        {"d", 4},
        {"e", 2},
    };

    for(auto &token: tokens) {
        if(token.type == Token::ID) {
            token = Token{identifiers.at(token.id)};
        }
    }
}

fn split_to_token(String &s) -> std::vector<Token> {

    std::vector<Token> vec{};
    int idx = 0;
    
    while(idx < s.len()) {

        char c = s[idx];
        if(c == ' ') {
            idx++; 
            continue;
        };

        auto subs = s.substr(idx);

        if('0' <= subs[0] and subs[0] <= '9') {
            
            int i = 0;
            while('0' <= subs[i] and subs[i] <= '9') i++;
            // 12 34 12
            auto num_s = subs.substr(0, i);
            auto value = num_s.parse<int>();

            vec.push_back(Token{value.value()});
            idx += i;

        }else if ('a' <= subs[0] and subs[0] <= 'z'){
            
            int i = 0;
            while('a' <= subs[i] and subs[i] <= 'z') i++;

            String id = subs.substr(0, i);

            idx += i;
            
            vec.push_back(Token{id});
        }else {// operator
            vec.push_back(Token{subs[0]});
            idx += 1;
        }
    }

    vec.push_back('$');// 结尾终止符
    replace_id_with_num(vec);

    return vec;
}

int main(int argc, char const *argv[])
{
    String s = "a + b * c - (d / e)";
    // std::vector<String> ss = s.split(" ");

    // auto v = ss[0].parse<int>();
    // cout << v.value() << endl;
    std::vector<Token> tokens = split_to_token(s);
    // return start();
    real_start(tokens);
    return 0;
}
