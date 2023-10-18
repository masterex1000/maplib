/* file minunit_example.c */

#include <stdio.h>
#include <iostream>
#include <sstream>
#include "minunit.h"
#include <mapflib/mapparser.h>

int tests_run = 0;
int tests_failed = 0;

char mu_last_message[1024];
int mu_status = 0;


int foo = 7;
int bar = 4;


static void test_mfl_expect() {
    std::istringstream stream("} ");
    mu_assert("close quote", MapFLib::expect(stream, "}"));

    stream = std::istringstream("    } ");
    mu_assert("close quote with whitespace", MapFLib::expect(stream, "}"));
    
    stream = std::istringstream("    } }   ");
    mu_assert("multiple times", 
        MapFLib::expect(stream, "}") && 
        MapFLib::expect(stream, "}"));
}

static void test_mfl_expectStartsWith() {
    std::istringstream stream( "    }    blah  blah   aa");

    mu_assert("handles whitespace", MapFLib::expectStartsWith(stream, "}"));
    mu_assert("starts with working", MapFLib::expectStartsWith(stream, "blah"));
    mu_assert("not match", !MapFLib::expectStartsWith(stream, "ab"));
    mu_assert("handles whitespace", 
        MapFLib::expectStartsWith(stream, "blah") &&
        MapFLib::expectStartsWith(stream, "aa"));
}

static void test_mfl_expectChar() {
    std::istringstream stream("aaa  b   c    d");

    long pos = stream.tellg();
    mu_assert("don't match char", !MapFLib::expectChar(stream, 'b'));
    mu_assert("doesn't move stream forward on fail", pos == stream.tellg());
    mu_assert("read correct char", MapFLib::expectChar(stream, 'a'));
    mu_assert("move stream forward on success", pos < stream.tellg());
    mu_assert("read correct char second", MapFLib::expectChar(stream, 'a'));
    mu_assert("handle incorrect char", !MapFLib::expectChar(stream, 'b'));
    mu_assert("read correct char third", MapFLib::expectChar(stream, 'a'));

    mu_assert("correctly handle whitespace", MapFLib::expectChar(stream, 'b'));

    pos = stream.tellg();
    mu_assert("correctly handle whitespace && wrong char", !MapFLib::expectChar(stream, 'x'));
    mu_assert("make sure stream is in same pos after fail", stream.tellg() == pos);
}

static void test_mfl_parseComment() {
    std::istringstream stream(
        "// blah blah\n"
        "1\n"
        "\n"
        "      // yeaaa\n"
        "1234\n\n"
        "//comment\n"
        "1231 //aaaa"
        );

    mu_assert("reads succesfully", MapFLib::parseComment(stream));
    long pos = stream.tellg();
    mu_assert("fails to read non-comment", !MapFLib::parseComment(stream));
    mu_assert("on-fail resumes to proper position on stream", pos == stream.tellg());
    
    int blank;
    stream >> blank;

    mu_assert("handle whitespace", MapFLib::parseComment(stream));
    mu_assert("should be another failure", !MapFLib::parseComment(stream));
    stream >> blank;
    mu_assert("another whitespace test", MapFLib::parseComment(stream));
    mu_assert("comment on same line, but after other text before (should fail)", !MapFLib::parseComment(stream));
    stream >> blank;
    mu_assert("consumed start of line, should work now", MapFLib::parseComment(stream));
}

static void test_mfl_parseTextureAxis() {
    std::istringstream stream ("a [ 1 2 3 4 ]   b");

    MapFLib::TextureAxis axis;

    long pos = stream.tellg();
    mu_assert("should fail texture axis parse", !MapFLib::parseTextureAxis(stream, axis));
    mu_assert("should leave stream at same pos called if fail", pos == stream.tellg());
    char blank;
    stream >> blank;
    pos = stream.tellg();
    mu_assert("successfully reads axis", MapFLib::parseTextureAxis(stream, axis));
    mu_assert("stream moves after successfull read", pos < stream.tellg());
    mu_assert("only read axis value", MapFLib::expectChar(stream, 'b'));
    mu_assert_float("correctly read axis data x", 1, axis.x);
    mu_assert_float("correctly read axis data y", 2, axis.y);
    mu_assert_float("correctly read axis data z", 3, axis.z);
    mu_assert_float("correctly read axis data offset", 4, axis.offset);
}

static void test_mfl_parseVector3() {
    std::istringstream stream("a (1 2 3)   b");
    
    MapFLib::Vector3 vector;
    
    long pos = stream.tellg();
    mu_assert("should fail to parse non-vector3", !MapFLib::parseVector3(stream, vector));
    mu_assert("should leave stream in same position after failure", pos == stream.tellg());

    char blank;
    stream >> blank;
    pos = stream.tellg();

    mu_assert("successfully reads axis", MapFLib::parseVector3(stream, vector));
    mu_assert("stream should move after succesful read", pos < stream.tellg());
    mu_assert("only read vector3", MapFLib::expectChar(stream, 'b'));
    mu_assert_float("correctly read vector x", 1, vector.x);
    mu_assert_float("correctly read vector y", 2, vector.y);
    mu_assert_float("correctly read vector z", 3, vector.z);
}

static void test_mfl_parseBrushFace() {
    std::istringstream stream("b (1 2 3) (3 4 5) (6 7 8) __TB_empty [ 1 2 3 4 ] [ 6 7 8 9 ] 0 1 2 c");

    long pos = stream.tellg();
    MapFLib::BrushFace face;
    mu_assert("should fail to parse non-brush face", !MapFLib::parseBrushFace(stream, face));
    mu_assert("shouldn't move after failed read", pos == stream.tellg());
    
    char blank;
    stream >>blank;
    pos = stream.tellg();

    mu_assert("should properly parse correct brush face", MapFLib::parseBrushFace(stream, face));
    mu_assert("stream should move forward after successful parse", pos < stream.tellg());
    mu_assert("stream should only consume brush face", MapFLib::expectChar(stream, 'c'));
}

static void test_mfl_parseEntityBrush() {
    MapFLib::Brush originalBrush;

    originalBrush.faces.push_back((MapFLib::BrushFace) {
        (MapFLib::Vector3) {1, 2, 3},
        (MapFLib::Vector3) {4, 5, 6},
        (MapFLib::Vector3) {7, 8, 9},
        "blank",
        (MapFLib::TextureAxis) {0, 1, 3, 4},
        (MapFLib::TextureAxis) {5, 6, 7, 8},
        0, 0, 0 
    });

    std::stringstream stream;
    stream << "a ";
    MapFLib::printEntityBrush(originalBrush, stream);
    stream << " b";

    long pos = stream.tellg();
    MapFLib::Brush b;

    mu_assert("should fail to parse non-brush", !MapFLib::parseEntityBrush(stream, b));
    mu_assert("stream shouldn't move forward after unsuccessful parse", pos == stream.tellg());
    
    char blank;
    stream >> blank;
    pos = stream.tellg();

    mu_assert("parses correct brush", MapFLib::parseEntityBrush(stream, b));
    mu_assert("stream moves forward after sucessful parse", pos < stream.tellg());
    mu_assert("stream only consumes brush", MapFLib::expectChar(stream, 'b'));
}

static void test_mfl_parseEntityProperty() {
    std::istringstream stream("a \"hello\" \"world\" b     \"a\" \"aaa\" \"b\" \"a\" g");

    long pos = stream.tellg();

    std::string key;
    std::string value;

    mu_assert("should fail to parse non-property", !MapFLib::parseEntityProperty(stream, key, value));
    mu_assert("stream shouldn't move forward after failure to parse", pos == stream.tellg());

    char blank;
    stream >> blank;
    pos = stream.tellg();

    mu_assert("succesfully parses valid property", MapFLib::parseEntityProperty(stream, key, value));
    mu_assert("stream moves forward after successful parse", pos < stream.tellg());
    mu_assert("correct key value", key == "hello");
    mu_assert("correct value value", value == "world");
    mu_assert("only consumed entity property", MapFLib::expectChar(stream, 'b'));
}

static void all_tests() {
    mu_run_test(test_mfl_expect);
    mu_run_test(test_mfl_expectStartsWith);
    mu_run_test(test_mfl_expectChar);
    mu_run_test(test_mfl_parseComment);
    mu_run_test(test_mfl_parseTextureAxis);
    mu_run_test(test_mfl_parseVector3);
    mu_run_test(test_mfl_parseBrushFace);
    mu_run_test(test_mfl_parseEntityBrush);
    mu_run_test(test_mfl_parseEntityProperty);
}

int main(int argc, char **argv) {
    // const char *result = all_tests();
    // if (result != 0) {
    //     printf("%s\n", result);
    // }
    // else {
    //     printf("ALL TESTS PASSED\n");
    // }

    all_tests();
    printf("Tests run: %d\n", tests_run);
    printf("Tests failed: %d\n", tests_failed);

    if(tests_failed > 0)
        printf("------- FAIL!!!! ------\n");

    return tests_failed > 0;
}