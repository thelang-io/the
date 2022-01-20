/*!
 * Copyright (c) Aaron Delasy
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <map>
#include "AST.hpp"
#include "AnonMap.hpp"

//struct CodegenFunctionsBool {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool str = false;
//};
//
//struct CodegenFunctionsByte {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool str = false;
//};
//
//struct CodegenFunctionsChar {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool str = false;
//};
//
//struct CodegenFunctionsFloat {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool str = false;
//};
//
//struct CodegenFunctionsFn {
//  bool init = false;
//  bool deinit = false;
//  bool call = false;
//};
//
//struct CodegenFunctionsInt {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool str = false;
//};
//
//struct CodegenFunctionsStr {
//  bool init = false;
//  bool copy = false;
//  bool deinit = false;
//  bool to_cstr = false;
//
//  bool concat = false;
//};
//
//struct CodegenFunctions {
//  CodegenFunctionsByte byte;
//  CodegenFunctionsChar ch;
//  CodegenFunctionsFloat f;
//  CodegenFunctionsFloat f32;
//  CodegenFunctionsFloat f64;
//  CodegenFunctionsFn fn;
//  CodegenFunctionsInt i;
//  CodegenFunctionsInt i8;
//  CodegenFunctionsInt i16;
//  CodegenFunctionsInt i32;
//  CodegenFunctionsInt i64;
//  CodegenFunctionsStr str;
//  CodegenFunctionsBool t;
//  CodegenFunctionsInt u8;
//  CodegenFunctionsInt u16;
//  CodegenFunctionsInt u32;
//  CodegenFunctionsInt u64;
//};

struct Codegen {
  AnonMap anonMap;
  std::size_t indent;
  std::string output;
  std::map<std::string, std::tuple<std::string, std::string>> functionDeclarations;
  std::map<std::string, std::tuple<std::string, std::string, std::string>> functionDefinitions;
  std::vector<std::string> structDeclarations;
  std::map<std::string, std::vector<std::tuple<std::string, std::string>>> structDefinitions;
};

Codegen codegen (AST *);

#endif
