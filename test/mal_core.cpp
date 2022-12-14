#include "doctest.h"
#include <mal.h>

TEST_SUITE("[mal_env][impl]") {
    TEST_CASE("def!") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(def! a 6)"), "6");
        REQUIRE_EQ(mal::rep(env, "a"), "6");
        REQUIRE_EQ(mal::rep(env, "(def! b (+ a 2))"), "8");
        REQUIRE_EQ(mal::rep(env, "(+ a b)"), "14");
    }

    TEST_CASE("let*") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(let* (c 2) c)"), "2");
        REQUIRE_EQ(mal::rep(env, "(let* (c 2) (let* [d 3] (let* {a 5} (+ c d a))))"), "10");
    }

    TEST_CASE("do") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(do (def! b 3) (let* (c 2) (+ c b)))"), "5");
    }

    TEST_CASE("fn*") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(fn* (a) a)"), "(fn* [a] a)");
        REQUIRE_EQ(mal::rep(env, "(fn* [a] a b)"), "(fn* [a] a b)");
        REQUIRE_EQ(mal::rep(env, "((fn* (a) (+ a a)) 5)"), "10");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a) a) 7)"), "7");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a) (+ a 1)) 10)"), "11");
        REQUIRE_EQ(mal::rep(env, "( (fn* (a b) (+ a b)) 2 3)"), "5");
    }

    TEST_CASE("if") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(if true 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if 12 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if 0 5 3)"), "5");
        REQUIRE_EQ(mal::rep(env, "(if false 5 3)"), "3");
        REQUIRE_EQ(mal::rep(env, "(if nil 5 3)"), "3");
    }

    TEST_CASE("prn-str") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((pr-str true 5 3 "hello"))"), R"("true 5 3 \"hello\"")");
    }

    TEST_CASE("str") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((str true 5 3 "hello"))"), R"("true53hello")");
    }

    TEST_CASE("prn") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((prn true 5 3 "hello"))"), "nil");
    }

    TEST_CASE("println") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((println true 5 3 "hello"))"), "nil");
    }

    TEST_CASE("read-string") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((read-string "5"))"), "5");
        REQUIRE_EQ(mal::rep(env, R"((read-string "\"hello\""))"), R"("hello")");
    }

    TEST_CASE("slurp") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((slurp "test.txt"))"), R"("Hello World!\nHow are you?")");
    }

    TEST_CASE("list") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((list 1 2 3 "hello"))"), R"((1 2 3 "hello"))");
    }

    TEST_CASE("list?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((list? 1))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((list? (list 1 2 3)))"), R"(true)");
    }

    TEST_CASE("empty?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((empty? (list)))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? (list 1 2 3)))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((empty? []))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? [1 2 3]))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((empty? {}))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((empty? {1 2 3 4}))"), R"(false)");
    }

    TEST_CASE("count") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((count (list)))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count (list 1 2 3)))"), R"(3)");
        REQUIRE_EQ(mal::rep(env, R"((count []))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count [1 2 3]))"), R"(3)");
        REQUIRE_EQ(mal::rep(env, R"((count {}))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((count {1 2 3 4}))"), R"(2)");
    }

    TEST_CASE("=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((= 5 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((= 5 5 5 5 5 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((= 5 5 15 5 5 5))"), R"(false)");
    }

    TEST_CASE("<") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((< 5 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 6 7 8 9 10))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 6 15 8 9 10))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((< 5 5))"), R"(false)");
    }

    TEST_CASE("<=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6 7 8 9 9))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((<= 5 6 15 8 9 10))"), R"(false)");
    }

    TEST_CASE(">") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((> 6 5))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((> 6 5 4 3 2 1))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((> 6 5 15 8 9 10))"), R"(false)");
        REQUIRE_EQ(mal::rep(env, R"((> 5 5))"), R"(false)");
    }

    TEST_CASE(">=") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((>= 8 6))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((>= 5 5 4 3 2 1))"), R"(true)");
        REQUIRE_EQ(mal::rep(env, R"((>= 5 6 15))"), R"(false)");
    }

    TEST_CASE("<=>") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((<=> 8 6))"), R"(1)");
        REQUIRE_EQ(mal::rep(env, R"((<=> 5 5))"), R"(0)");
        REQUIRE_EQ(mal::rep(env, R"((<=> 5 6))"), R"(-1)");
    }

    TEST_CASE("eval") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((let* [mal-prog (list + 1 2)] (eval mal-prog)))"), R"(3)");
    }

    TEST_CASE("load-file") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((do (load-file "test.lsp") (inc4 3)))"), R"(7)");
    }

    TEST_CASE("atom") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((atom 5))"), R"((atom 5))");
    }

    TEST_CASE("atom?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((atom? (atom 5)))"), R"(true)");
    }

    TEST_CASE("deref") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((deref (atom 5)))"), R"(5)");
        REQUIRE_EQ(mal::rep(env, R"(@(atom 5))"), R"(5)");
    }

    TEST_CASE("reset!") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((reset! (atom 5) 10))"), R"(10)");
    }

    TEST_CASE("swap!") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((swap! (atom 5) (fn* [x] (+ x 1))))"), R"(6)");
        REQUIRE_EQ(mal::rep(env, R"((swap! (atom 5) str))"), R"("5")");
        REQUIRE_EQ(mal::rep(env, R"((swap! (atom 5) str "a"))"), R"("5a")");
    }

    TEST_CASE("cons") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((cons 0 (list 1 2 5)))"), R"((0 1 2 5))");
        REQUIRE_EQ(mal::rep(env, R"((cons 0 [1 2 5]))"), R"((0 1 2 5))");
    }

    TEST_CASE("concat") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((concat (list 1 2 4) (list 1 2 5)))"), R"((1 2 4 1 2 5))");
        REQUIRE_EQ(mal::rep(env, R"((concat (list 1 2 4) [1 2 5]))"), R"((1 2 4 1 2 5))");
        REQUIRE_EQ(mal::rep(env, R"((concat [1 2 4] [1 2 5]))"), R"((1 2 4 1 2 5))");
        REQUIRE_EQ(mal::rep(env, R"((concat [] (list 1 4) (list 3 4)))"), R"((1 4 3 4))");
        REQUIRE_EQ(mal::rep(env, R"((concat (list) (list 1 4) (list 3 4)))"), R"((1 4 3 4))");
        REQUIRE_EQ(mal::rep(env, R"((concat))"), R"(())");
    }

    TEST_CASE("quote") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, R"((quote abc))"), R"(abc)");
        REQUIRE_EQ(mal::rep(env, R"((quote (1 4 2 3)))"), R"((1 4 2 3))");
        REQUIRE_EQ(mal::rep(env, R"('abc)"), R"(abc)");
        REQUIRE_EQ(mal::rep(env, R"('(1 4 2 3))"), R"((1 4 2 3))");
    }

    TEST_CASE("quasiquoteexpand") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(
                mal::rep(env, R"((quasiquoteexpand (quasiquote (4 (splice-unquote (list 3 2 4)) 9))))"),
                R"((cons (quasiquote 4) (concat (list 3 2 4) (cons (quasiquote 9) ()))))"
        );
        REQUIRE_EQ(
                mal::rep(env, R"((quasiquoteexpand `(4 ~@(list 3 2 4) ~(list 2) 9)))"),
                R"((cons (quasiquote 4) (concat (list 3 2 4) (cons (quasiquote (unquote (list 2))) (cons (quasiquote 9) ())))))"
        );
    }

    TEST_CASE("quasiquote") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(
                mal::rep(env, R"((quasiquote (4 (splice-unquote (list 3 2 4)) (unquote (list 2)) 9)))"),
                R"((4 3 2 4 (2) 9))"
        );
        REQUIRE_EQ(
                mal::rep(env, R"(`(4 ~@(list 3 2 4) ~(list 2) 9))"),
                R"((4 3 2 4 (2) 9))"
        );
    }

    TEST_CASE("macro?") {
        auto env = std::make_shared<mal::MalEnv>();
        mal::rep(env, R"((defmacro! add2 (fn* [a] (list + a 2))))");
        mal::rep(env, R"((def! add3 (fn* [a] (+ a 3))))");
        REQUIRE_EQ(mal::rep(env, "(macro? add2)"), "true");
        REQUIRE_EQ(mal::rep(env, "(macro? add3)"), "false");
    }

    TEST_CASE("macroexpand") {
        auto env = std::make_shared<mal::MalEnv>();
        mal::rep(env, R"((defmacro! add2 (fn* [a] (list + a 2))))");
        REQUIRE_EQ(mal::rep(env, "(macroexpand (add2 4))"), "(#<native_function> 4 2)");
    }

    TEST_CASE("macros") {
        auto env = std::make_shared<mal::MalEnv>();
        mal::rep(env, R"((defmacro! add2 (fn* [a] (list + a 2))))");
        REQUIRE_EQ(mal::rep(env, "(add2 4)"), "6");
    }

    TEST_CASE("nth") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(nth (list 2 4 5) 1)"), "4");
        REQUIRE_EQ(mal::rep(env, "(nth [2 4 5] 1)"), "4");
    }

    TEST_CASE("rest") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(rest (list 2 4 5))"), "(4 5)");
        REQUIRE_EQ(mal::rep(env, "(rest [2 4 5])"), "(4 5)");
    }

    TEST_CASE("first") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(first (list 2 4 5))"), "2");
        REQUIRE_EQ(mal::rep(env, "(first [2 4 5])"), "2");
        REQUIRE_EQ(mal::rep(env, "(first [])"), "nil");
        REQUIRE_EQ(mal::rep(env, "(first nil)"), "nil");
    }

    TEST_CASE("vec") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(vec 2 4 5)"), "[2 4 5]");
    }

    TEST_CASE("vec?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(vec? [2 4 5])"), "true");
    }

    TEST_CASE("throw") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_THROWS_AS(mal::rep(env, "(throw 93)"), std::runtime_error);
    }

    TEST_CASE("try/catch") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(try* (+ 2 3) (catch* E 3)))"), "5");
        REQUIRE_EQ(mal::rep(env, "(try* (throw 23) (catch* E (str E 3))))"), R"("233")");
    }

    TEST_CASE("apply") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(apply + 1 2 [3 4])"), "10");
    }

    TEST_CASE("variadic fn") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "((fn* [&x] (apply + x)) 1 4 2 8)"), "15");
    }

    TEST_CASE("variadic fn") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "((fn* [&x] (apply + x)) 1 4 2 8)"), "15");
    }

    TEST_CASE("map") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(map (fn* [x] (* x x)) (list 1 2 3 4))"), "(1 4 9 16)");
        REQUIRE_EQ(mal::rep(env, "(map (fn* [x] (* x x)) (vec 1 2 3 4))"), "(1 4 9 16)");
        REQUIRE_EQ(mal::rep(env, R"((map (fn* [x] (str x x)) "hello"))"), R"(("hh" "ee" "ll" "ll" "oo"))");
        REQUIRE_EQ(mal::rep(env, "(map (fn* [x] (* (first x) (nth x 1))) {1 2 3 4})"), "(2 12)");
        REQUIRE_EQ(mal::rep(env, R"((map str [:a :b :c] (list 9 5 4 2) {1 2 3 4} "abcde"))"), R"((":a9[1 2]a" ":b5[3 4]b" ":c4nilc" "nil2nild" "nilnilnile"))");
    }

    TEST_CASE("nil?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(nil? nil)"), "true");
        REQUIRE_EQ(mal::rep(env, "(nil? 5)"), "false");
    }

    TEST_CASE("keyword?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(keyword? :abc)"), "true");
        REQUIRE_EQ(mal::rep(env, "(keyword? 5)"), "false");
    }

    TEST_CASE("keyword?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(keyword? :abc)"), "true");
        REQUIRE_EQ(mal::rep(env, "(keyword? 5)"), "false");
    }

    TEST_CASE("symbol?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(symbol? 'abc)"), "true");
        REQUIRE_EQ(mal::rep(env, "(symbol? 5)"), "false");
    }

    TEST_CASE("hash-map?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(hash-map? {'abc 5})"), "true");
        REQUIRE_EQ(mal::rep(env, "(hash-map? 5)"), "false");
    }

    TEST_CASE("hash-map") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(hash-map 'abc 5)"), "{abc 5}");
        REQUIRE_EQ(mal::rep(env, "(hash-map 4 5 6)"), "{4 5 6 nil}");
    }

    TEST_CASE("true?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(true? true)"), "true");
        REQUIRE_EQ(mal::rep(env, "(true? 5)"), "false");
    }

    TEST_CASE("false?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(false? false)"), "true");
        REQUIRE_EQ(mal::rep(env, "(false? nil)"), "false");
    }

    TEST_CASE("truthy?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(truthy? true)"), "true");
        REQUIRE_EQ(mal::rep(env, "(truthy? 5)"), "true");
        REQUIRE_EQ(mal::rep(env, "(truthy? false)"), "false");
    }

    TEST_CASE("falsey?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(falsey? false)"), "true");
        REQUIRE_EQ(mal::rep(env, "(falsey? nil)"), "true");
        REQUIRE_EQ(mal::rep(env, "(falsey? true)"), "false");
    }

    TEST_CASE("not") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(not false)"), "true");
        REQUIRE_EQ(mal::rep(env, "(not nil)"), "true");
        REQUIRE_EQ(mal::rep(env, "(not true)"), "false");
    }

    TEST_CASE("or") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(or false false true)"), "true");
        REQUIRE_EQ(mal::rep(env, "(or true false false)"), "true");
        REQUIRE_EQ(mal::rep(env, "(or false false false)"), "false");
    }

    TEST_CASE("and") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(and false false true)"), "false");
        REQUIRE_EQ(mal::rep(env, "(and true false false)"), "false");
        REQUIRE_EQ(mal::rep(env, "(and false false false)"), "false");
        REQUIRE_EQ(mal::rep(env, "(and true true true)"), "true");
    }

    TEST_CASE("seq?") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(seq? [1 12])"), "true");
        REQUIRE_EQ(mal::rep(env, "(seq? (vec 12))"), "true");
        REQUIRE_EQ(mal::rep(env, "(seq? (hash-map))"), "false");
        REQUIRE_EQ(mal::rep(env, "(seq? \"abc\")"), "false");
    }

    TEST_CASE("assoc") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(assoc {1 12} 'a 34 :e 4)"), "{1 12 :e 4 a 34}");
    }

    TEST_CASE("dissoc") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(dissoc {1 12 'a 34 :e 4} 1 :e)"), "{a 34}");
    }

    TEST_CASE("get") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(get {1 12 'a 34 :e 4} 1)"), "12");
    }

    TEST_CASE("entries") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(entries {1 12 'a 34 :e 4})"), "([1 12] [:e 4] [a 34])");
    }

    TEST_CASE("keys") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(keys {1 12 'a 34 :e 4})"), "(1 :e a)");
    }

    TEST_CASE("values") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(values {1 12 'a 34 :e 4})"), "(12 4 34)");
    }

    TEST_CASE("second") {
        auto env = std::make_shared<mal::MalEnv>();
        REQUIRE_EQ(mal::rep(env, "(second [1 4])"), "4");
        REQUIRE_EQ(mal::rep(env, "(second [1])"), "nil");
    }
}
