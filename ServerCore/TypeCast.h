#pragma once
#include "Types.h"

// Compile 타임에 모든 것이 결정됨.
// -> 빌드 후 실행부터는 부담이 없다.
// 나머지 기능들도 전부 Compile 타임에 지정이 되는 것들만 사용 가능
#pragma region TypeList
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion

#pragma region Length
template <typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};
#pragma endregion

#pragma region TypeAt
template <typename TL, int32 index>
struct TypeAt;

template <typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template <typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};
#pragma endregion

#pragma region IndexOf
template <typename TL, typename T>
struct IndexOf;

template <typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0 };
};

template <typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template <typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T >
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };

public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};
#pragma endregion

#pragma region Conversion
template <typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }	// true
	static Big Test(...) { return 0; }			// false
	
	static From MakeFrom() { return 0; }

public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};
#pragma endregion

#pragma region TypeCast
template <int32 v>
struct IntToType
{
	enum { value = v };
};

template<typename TL>
class TypeConversion
{
public:
	enum 
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		// Compile 타임에 i와 j가 정해지지 않아 실행 불가능
		/*for (int32 i = 0; i < length; i++)
		{
			for (int32 j = 0; j < length; j++)
			{
				using FromType = typename TypeAt<TL, i>::Result;
				using ToTYpe = typename TypeAt<TL, j>::Result;

				if (Conversion<const FromType* const ToType*>::exists)
					s_convert[i][j] = true;
				else
					s_convert[i][j] = false;
			}
		}*/

		// 이러면 Compile 타임에 만들어져 실행 가능
		MakeTable(IntToType<0>(), IntToType<0>());
	}

	template <int32 i, int32 j>
	static void MakeTable(IntToType<i>, IntToType<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(IntToType<i>(), IntToType<j + 1>());
	}

	template <int32 i>
	static void MakeTable(IntToType<i>, IntToType <length>)
	{
		MakeTable(IntToType<i + 1>(), IntToType<0>());
	}

	template <int32 j>
	static void MakeTable(IntToType<length>, IntToType <j>)
	{
		
	}

	static inline bool CanConvert(int32 from, int32 to)
	{
		static TypeConversion conversion;
		return s_convert[from][to];
	}

public:
	static bool s_convert[length][length];
};

template <typename TL>
bool TypeConversion<TL>::s_convert[length][length];

template <typename To, typename From>
To TypeCast(From* _ptr)
{
	if (_ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(
		_ptr->m_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(_ptr);

	return nullptr;
}

template <typename To, typename From>
bool CanCast(From* _ptr)
{
	if (_ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(
		_ptr->m_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	return false;
}

template <typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> _ptr)
{
	if (_ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::CanConvert(
		_ptr->m_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(_ptr);

	return nullptr;
}

template <typename To, typename From>
bool CanCast(shared_ptr<From> _ptr)
{
	if (_ptr == nullptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::CanConvert(
		_ptr->m_typeId, IndexOf<TL, remove_pointer_t<To>>::value);

	return false;
}
#pragma endregion

#define DECLARE_TL		using TL = TL; int32 m_typeId;
#define INIT_TL(type)	m_typeId = IndexOf<TL, type>::value;