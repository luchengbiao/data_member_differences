// data_member_ptr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

template<typename StructT, typename DataMemberT>
struct DataMemberTrait
{
	typedef DataMemberT StructT::*DataMemberPtr;

	const std::string	key_;
	const DataMemberPtr	mem_ptr_{ nullptr };

	DataMemberTrait(const std::string& key, const DataMemberPtr mem_ptr)
		: key_(key), mem_ptr_(mem_ptr)
	{}
};

#define TEMPLATE_DATA_MEMBER_TRAIT(TemplateName, MemberT) \
template<typename StructT> \
struct TemplateName : DataMemberTrait<StructT, MemberT> \
{ \
	typedef DataMemberTrait<StructT, MemberT> MyBase; \
	TemplateName(const std::string& key, const MyBase::DataMemberPtr mem_ptr) \
	: MyBase(key, mem_ptr) {}\
    \
	static std::vector<std::string> DifferencesBetween(const StructT& lhs, const StructT& rhs) \
    { \
		std::vector<std::string> vec_key;\
		\
		for (const auto& trait : StructT::Get##TemplateName##s()) \
		{ \
			if (trait.mem_ptr_ && !((lhs.*trait.mem_ptr_) == (rhs.*trait.mem_ptr_))) \
			{ \
				vec_key.emplace_back(trait.key_); \
			} \
		}\
		return vec_key; \
    } \
}

TEMPLATE_DATA_MEMBER_TRAIT(StringMemberTrait, std::string);
TEMPLATE_DATA_MEMBER_TRAIT(IntMemberTrait, int);

template<typename StructT, template<typename> class... MemberTraits>
struct DifferencesBetweenImpl;

// partial specialization to recur
template<typename StructT, template<typename> class HeadMemberTrait, template<typename> class... RemainMemberTraits>
struct DifferencesBetweenImpl<StructT, HeadMemberTrait, RemainMemberTraits...>
{
	static std::vector<std::string> Apply(const StructT& lhs, const StructT& rhs)
	{
		std::vector<std::string> vec_key;

		auto vec_key_head = HeadMemberTrait<StructT>::DifferencesBetween(lhs, rhs);
		vec_key.insert(vec_key.cend(), vec_key_head.cbegin(), vec_key_head.cend());

		// recursive
		auto vec_key_remain = DifferencesBetweenImpl<StructT, RemainMemberTraits...>::Apply(lhs, rhs);
		vec_key.insert(vec_key.cend(), vec_key_remain.cbegin(), vec_key_remain.cend());

		return vec_key;
	}
};

// partial specialization to terminate recursion
template<typename StructT>
struct DifferencesBetweenImpl<StructT>
{
	static std::vector<std::string> Apply(const StructT& lhs, const StructT& rhs)
	{
		return std::vector<std::string>();
	}
};

struct Test
{
	std::string str_0_;
	std::string str_1_;

	int			i_0_;
	int			i_1_;

	static const int StringMembers = 2;
	typedef StringMemberTrait<Test> StringMemberTraits[StringMembers];
	static const StringMemberTraits& GetStringMemberTraits()
	{
		static const StringMemberTraits traits =
		{
			{ "str_0", &Test::str_0_ },
			{ "str_1", &Test::str_1_ },
		};

		return traits;
	}

	static const int IntMembers = 2;
	typedef IntMemberTrait<Test> IntMemberTraits[IntMembers];
	static const IntMemberTraits& GetIntMemberTraits()
	{
		static const IntMemberTraits traits =
		{
			{ "i_0", &Test::i_0_ },
			{ "i_1", &Test::i_1_ },
		};

		return traits;
	}

	std::vector<std::string> DifferencesWith(const Test& rhs)
	{
		return Test::DifferencesBetween(*this, rhs);
	}

	static std::vector<std::string> DifferencesBetween(const Test& lhs, const Test& rhs)
	{
		return DifferencesBetweenImpl<
			Test,
			StringMemberTrait,
			IntMemberTrait
		>::Apply(lhs, rhs);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	Test lhs = { "str0", "str1", 0, 1 };
	Test rhs = { "STR0", "STR1", 10, 11 };

	auto vec_key = lhs.DifferencesWith(rhs);
	std::cout << "Differences Between lhs and rhs:" << std::endl;
	for (const auto& key : vec_key)
	{
		std::cout << key << ": ";
		if (key == "str_0")
		{
			std::cout << lhs.str_0_ << " " << rhs.str_0_;
		}
		else if (key == "str_1")
		{
			std::cout << lhs.str_1_ << " " << rhs.str_1_;
		}
		else if (key == "i_0")
		{
			std::cout << lhs.i_0_ << " " << rhs.i_0_;
		}
		else if (key == "i_1")
		{
			std::cout << lhs.i_1_ << " " << rhs.i_1_;
		}

		std::cout << std::endl;
	}

	system("pause");

	return 0;
}

