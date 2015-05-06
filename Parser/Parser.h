#pragma once
#include "Tokenizer.h"
#include "Core/Expression.h"
#include "Commands/CommandSequence.h"
#include "DirectivesParser.h"

struct AssemblyTemplateArgument
{
	const wchar_t* variableName;
	std::wstring value;
};

class Parser
{
public:
	bool atEnd() { return entries.back().tokenizer->atEnd(); }

	Expression parseExpression();
	bool parseExpressionList(std::vector<Expression>& list);
	bool parseIdentifier(std::wstring& dest);
	CAssemblerCommand* parseCommand();
	CAssemblerCommand* parseCommandSequence(std::initializer_list<wchar_t*> terminators = {});
	CAssemblerCommand* parseFile(TextFile& file);
	CAssemblerCommand* parseString(const std::wstring& text);
	CAssemblerCommand* parseTemplate(const std::wstring& text, std::initializer_list<AssemblyTemplateArgument> variables = {});
	CAssemblerCommand* parseDirective(const DirectiveEntry* directiveSet);
	bool matchToken(TokenType type, bool optional = false);

	Tokenizer* getTokenizer() { return entries.back().tokenizer; };
	Token& peekToken(int ahead = 0) { return getTokenizer()->peekToken(ahead); };
	Token& nextToken() { return getTokenizer()->nextToken(); };
	void eatToken() { getTokenizer()->eatToken(); };
	void eatTokens(int num) { getTokenizer()->eatTokens(num); };
protected:
	CAssemblerCommand* Parser::parse(Tokenizer* tokenizer, bool allowEqu, bool allowMacro);
	CAssemblerCommand* parseLabel();
	bool parseMacro();
	bool checkEquLabel();
	bool isEquAllowed() { return entries.back().allowEqu; }
	bool isMacroAllowed() { return entries.back().allowMacro; }

	struct Entry
	{
		Tokenizer* tokenizer;
		bool allowEqu;
		bool allowMacro;
	};

	std::vector<Entry> entries;
};

struct TokenSequenceValue
{
	TokenSequenceValue(const wchar_t* text)
	{
		type = TokenType::Identifier;
		textValue = text;
	}
	
	TokenSequenceValue(u64 num)
	{
		type = TokenType::Integer;
		intValue = num;
	}
	
	TokenSequenceValue(double num)
	{
		type = TokenType::Float;
		floatValue = num;
	}
	

	TokenType type;
	union
	{
		const wchar_t* textValue;
		u64 intValue;
		double floatValue;
	};
};

typedef std::initializer_list<TokenType> TokenSequence;
typedef std::initializer_list<TokenSequenceValue> TokenValueSequence;

class TokenSequenceParser
{
public:
	void addEntry(int result, TokenSequence tokens, TokenValueSequence values);
	bool parse(Parser& parser, int& result);
	size_t getEntryCount() { return entries.size(); }
private:
	struct Entry
	{
		std::vector<TokenType> tokens;
		std::vector<TokenSequenceValue> values;
		int result;
	};

	std::vector<Entry> entries;
};

bool checkExpressionListSize(std::vector<Expression>& list, int min, int max);
