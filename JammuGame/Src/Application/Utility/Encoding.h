#pragma once
#include <string>

// UTF-8文字列をShift-JIS(CP932)に変換する
// ・utf8Str … 変換元のUTF-8文字列
// 戻り値：Shift-JISに変換された文字列
std::string Utf8ToSjis(const std::string& utf8Str);