#pragma once
#ifndef _PARSINGARGUMENT_H_
#define _PARSINGARGUMENT_H_
#include<string>
#include<map>
#include<vector>



#include"info.h"
#include"makerstring.h"

class ParsingArgument {
private:
	friend class BuilderParsingArgument;
	std::string countSpace(int count) {
		std::string ans1;
		for (int i = 0; i < count; i++)
			ans1 += " ";
		return ans1;
	}
	void* getVoid(void* block, size_t offset, size_t sizeOf)
	{
		char* p = (char*)block;
		char* val = (char*)malloc(sizeOf);

		memcpy(val, p + offset, sizeOf);
		return val;
	}

	//база данных множеств
	std::map<std::string, std::map<int, std::string>> baseOfEnum =
	{
		{
			"enumHeapParam",
			{
				{
					0x00000000,
					"NULL"
				},
				{
					HEAP_NO_SERIALIZE,
					"HEAP_NO_SERIALIZE"
				},
				{
					HEAP_GROWABLE,
					"HEAP_GROWABLE"
				},
				{
					HEAP_GENERATE_EXCEPTIONS,
					"HEAP_GENERATE_EXCEPTIONS"
				},
				{
					HEAP_ZERO_MEMORY,
					"HEAP_ZERO_MEMORY"
				},
				{
					HEAP_REALLOC_IN_PLACE_ONLY,
					"HEAP_REALLOC_IN_PLACE_ONLY"
				},
				{
					HEAP_TAIL_CHECKING_ENABLED,
					"HEAP_TAIL_CHECKING_ENABLED"
				},
				{
					HEAP_FREE_CHECKING_ENABLED,
					"HEAP_FREE_CHECKING_ENABLED"
				},
				{
					HEAP_DISABLE_COALESCE_ON_FREE,
					"HEAP_DISABLE_COALESCE_ON_FREE"
				},
				{
					HEAP_CREATE_ALIGN_16,
					"HEAP_CREATE_ALIGN_16"
				},
				{
					HEAP_CREATE_ENABLE_TRACING,
					"HEAP_CREATE_ENABLE_TRACING"
				},
				{
					HEAP_CREATE_ENABLE_EXECUTE,
					"HEAP_CREATE_ENABLE_EXECUTE"
				},
				{
					HEAP_MAXIMUM_TAG,
					"HEAP_MAXIMUM_TAG"
				},
				{
					HEAP_PSEUDO_TAG_FLAG,
					"HEAP_PSEUDO_TAG_FLAG"
				},
				{
					HEAP_TAG_SHIFT,
					"HEAP_TAG_SHIFT"
				},
				{
					HEAP_CREATE_SEGMENT_HEAP,
					"HEAP_CREATE_SEGMENT_HEAP"
				},
				{
					HEAP_CREATE_HARDENED,
					"HEAP_CREATE_HARDENED"
				}
			}
		},
		{
			"heapInformationClass",
			{
				{
					HeapCompatibilityInformation,
					"HeapCompatibilityInformation"
				},
				{
					HeapEnableTerminationOnCorruption,
					"HeapEnableTerminationOnCorruption"
				},
				{
					HeapOptimizeResources,
					"HeapOptimizeResources"
				}

			}

		}

	};

	//база количества аргументов
	std::map <std::string, int> baseOfCountArg = {
		{
			"HeapCreate",
			3
		},
		{
			"HeapDestroy",
			1
		},
		{
			"HeapAlloc",
			3
		},
		{
			"HeapFree",
			3
		},
		{
			"HeapSize",
			3
		},
		{
			"HeapReAlloc",
			4
		},
		{
			"HeapQueryInformation",
			5
		}
	};


	//база данных возвращаемых значений
	std::map<std::string, std::vector<Info>> baseOfAnsFunctions = {
		{
			"HeapCreate",
			{
				BuilderInfo().type(Type::heapHandler).info("Ans of function - HANDLE of maked heap").build()
			}
		},

		{
			"HeapDestroy",
			{
				BuilderInfo().type(Type::integer).info("Ans of function - If the function succeeds, the return VALUE is nonzero."
				"If the function fails, the return VALUE is zero").build()
			}
		},

		{
			"HeapAlloc",
			{
				BuilderInfo().type(Type::heapPvoid).info("Ans of function - PVOID of allocated heap. If an error has occurred, it will NULL").build()
			}
		},

		{
			"HeapFree",
			{
				BuilderInfo().type(Type::integer).info("Ans of function - If the function succeeds, the return VALUE is nonzero."
				"If the function fails, the return VALUE is zero").build()
			}
		},

		{
			"HeapSize",
			{
				BuilderInfo().type(Type::integer).info("Ans of function - If the function succeeds, the return VALUE is the requested size of the allocated memory block, in bytes."
				"If the function fails, the return VALUE is -1").build()
			}
		},

		{
			"HeapReAlloc",
			{
				BuilderInfo().type(Type::heapPvoid).info("Ans of function - PVOID of allocated heap. If has error returned of NULL").build()
			}
		},

		{
			"HeapQueryInformation",
			{
				BuilderInfo().type(Type::integer).info("Ans of function - If the function succeeds, the return VALUE is nonzero."
				"If the function fails, the return VALUE is zero").build()
			}
		}

	};
	//база данных аргументов
	std::map<std::string, std::vector<std::vector<Info>>> baseOfArgFunctions = {
		{
			"HeapCreate",
			{
				{
					BuilderInfo().type(Type::enumerate).info("flOptions - DWORD, The heap allocation options."
					"These options affect subsequent access to the new heap through calls to the heap functions."
					"This parameter can be 0 or one or more of values. ").enumName("enumHeapParam").build()
				},
				{
					BuilderInfo().type(Type::integer).info("dwInitialSize - SIZE_T, The initial size of the heap, in bytes."
					"This value determines the initial amount of memory that is committed for the heap."
					"The value is rounded up to a multiple of the system page size."
					"The value must be smaller than dwMaximumSize.").build()
				},
				{
					BuilderInfo().type(Type::integer).info("dwMaximumSize - SIZE_T, The maximum size of the heap, in bytes."
					"The HeapCreate function rounds dwMaximumSize up to a multiple of the system page size and then reserves a block of that size in the process's virtual address space for the heap."
					"If allocation requests made by the HeapAlloc or HeapReAlloc functions exceed the size specified by dwInitialSize, the system commits additional pages of memory for the heap, up to the heap's maximum size")
					.build()
				}
			}
		},

		{
		"HeapDestroy",
		{
			{
				BuilderInfo().type(Type::heapHandler).info("hHeap - HANDLE, A handle to the heap to be destroyed."
				"This handle is returned by the HeapCreate function").build()
			}
		}
		},

		{
			"HeapAlloc",
			{
				{
					BuilderInfo().type(Type::heapHandler).info("hHeap - HANDLE, A handle to the heap from which the memory will be allocated."
					"This handle is returned by the HeapCreate or GetProcessHeap function.").build()
				},
				{
					BuilderInfo().type(Type::enumerate).info("dwFlags - DWORD, The heap allocation options. Specifying any of these values will override the corresponding value specified when the heap was created with HeapCreate."
					"This parameter can be one or more of values.").enumName("enumHeapParam").build()
				},
				{
					BuilderInfo().type(Type::integer).info("dwBytes - SIZE_T, The number of bytes to be allocated.").build()
				}
			}
		},

		{
			"HeapFree",
			{
				{
					BuilderInfo().type(Type::heapHandler).info("hHeap - HANDLE, A handle to the heap whose memory block is to be freed."
					"This handle is returned by either the HeapCreate or GetProcessHeap function.").build()
				},
				{
					BuilderInfo().type(Type::enumerate).info("dwFlags - DWORD, The heap allocation options. Specifying any of these values will override the corresponding value specified when the heap was created with HeapCreate."
					"This parameter can be one or more of values.").enumName("enumHeapParam").build()
				},
				{
					BuilderInfo().type(Type::heapPvoid).info("lpMem - PVOID, A pointer to the memory block to be freed."
					"This pointer is returned by the HeapAlloc or HeapReAlloc function"
					"This pointer can be NULL.").build()
				}
			}
		},

		{
			"HeapSize",
			{
				{
					BuilderInfo().type(Type::heapHandler).info("hHeap - HANDLE, A handle to the heap whose memory block is to be freed."
					"This handle is returned by either the HeapCreate or GetProcessHeap function.").build()
				},
				{
					BuilderInfo().type(Type::enumerate).info("dwFlags - DWORD, The heap allocation options. Specifying any of these values will override the corresponding value specified when the heap was created with HeapCreate."
					"This parameter can be one or more of values.").enumName("enumHeapParam").build()
				},
				{
					BuilderInfo().type(Type::heapPvoid).info("lpMem - PVOID, A pointer to the memory block to be freed."
					"This pointer is returned by the HeapAlloc or HeapReAlloc function"
					"This pointer can be NULL.").build()
				}
			}
		},

		{
			"HeapReAlloc",
			{
				{
					BuilderInfo().type(Type::heapHandler).info("hHeap - HANDLE, A handle to the heap whose memory block is to be freed."
					"This handle is returned by either the HeapCreate or GetProcessHeap function.").build()
				},
				{
					BuilderInfo().type(Type::enumerate).info("dwFlags - DWORD, The heap allocation options. Specifying any of these values will override the corresponding value specified when the heap was created with HeapCreate."
					"This parameter can be one or more of values.").enumName("enumHeapParam").build()
				},
				{
					BuilderInfo().type(Type::heapPvoid).info("lpMem - PVOID, A pointer to the memory block to be freed."
					"This pointer is returned by the HeapAlloc or HeapReAlloc function").build()
				},
				{
					BuilderInfo().type(Type::integer).info("dwBytes - SIZE_T, The number of bytes to be allocated.").build()
				}
			}
		},

		{
			"HeapQueryInformation",
			{
				{
					BuilderInfo().type(Type::heapHandler).info("HeapHandle - HANDLE, A handle to the heap whose memory block is to be freed."
					"This handle is returned by either the HeapCreate or GetProcessHeap function.").build()
				},
				{
					BuilderInfo().type(Type::enumerate).info("HeapInformationClass - winnt.h ENUM ").enumName("heapInformationClass").build()
				},
				{
					BuilderInfo().nesting(0).type(Type::pvoid).info("A pointer to a buffer that receives the heap information."
					"The format of this data depends on the value of the HeapInformationClass parameter").sizeOf(sizeof(void*)).build(),
					BuilderInfo().nesting(1).type(Type::integer).info("Nope").build()
				},
				{
					BuilderInfo().type(Type::integer).info("The size of the heap information being queried, in bytes").build()
				},
				{
					BuilderInfo().nesting(0).type(Type::pvoid).info("A pointer to a variable that receives the length of data written to the HeapInformation buffer."
					"If the buffer is too small, the function fails and ReturnLength specifies the minimum size required for the buffer."
					"If you do not want to receive this information, specify NULL.").sizeOf(sizeof(void*)).build(),
					BuilderInfo().nesting(1).type(Type::integer).info("Nope").build()
				}
			}
		}
	};

	int maxLengthOfString = 200; //ограничение для vec[numInArray].info.substr(0,maxlengthOfString)
	int maxLengthOfArray = 20; //ограничеие для длины массива
	int maxNesting = 20; //ограничение для вложенности
public:
	std::string reqAllVoid(void* argument, int numInArray, const std::vector<Info>& vec) {

		Type t = vec[numInArray].type;

		std::string ans;

		if (vec[numInArray].nesting > maxNesting)
			return ans;

		switch (t) {
		case Type::structure:
			ans += countSpace(vec[numInArray].nesting) + vec[numInArray].info.substr(0, maxLengthOfString) + " \n";
			for (int i = numInArray + 1; i < vec.size(); i++) {//ограничение на то, чтобы одни стркутуры 
				//не залезали в аргументы других вложенных данных соответсвующего уровня вложенности
				if (vec[i].nesting == vec[numInArray].nesting)
					break;
				if (vec[i].nesting == vec[numInArray].nesting + 1) {//если вложен в структуру, запускаем рекурсию

					ans += reqAllVoid(getVoid(argument, vec[i].offset, vec[i].sizeOf), i, vec);
				}
			}
			break;
		case Type::enumerate:
			//получаем соответсвие числа значению по имени множества 
			ans += countSpace(vec[numInArray].nesting)
				+ vec[numInArray].info.substr(0, maxLengthOfString)
				+ " "
				+ std::to_string((DWORD64)argument)
				+ " - "
				+ baseOfEnum[vec[numInArray].enumName][(DWORD64)argument]
				+ "\n";
			break;
		case Type::heapHandler:
			ans += countSpace(vec[numInArray].nesting)
				+ vec[numInArray].info.substr(0, maxLengthOfString)
				+ ": "
				+ MakerString::MakeHexNum((DWORD64)argument)
				+ "\n";
			break;
		case Type::integer:
			ans += countSpace(vec[numInArray].nesting)
				+ vec[numInArray].info.substr(0, maxLengthOfString)
				+ ": "
				+ std::to_string((DWORD64)argument)
				+ "\n";
			break;
		case Type::massif:
			ans += countSpace(vec[numInArray].nesting) + vec[numInArray].info.substr(0, maxLengthOfString) + ":\n";
			for (int i = 0; i < vec[numInArray].sizeOfMassif && i < maxLengthOfArray; i++) {
				ans += countSpace(vec[numInArray].nesting) + reqAllVoid(
					getVoid(*(void**)argument, vec[numInArray].sizeOf * i, vec[numInArray].sizeOf), numInArray + 1, vec);
			}
			break;
		case Type::pvoid:
			ans += countSpace(vec[numInArray].nesting) + vec[numInArray].info.substr(0, maxLengthOfString) + ":\n";

			ans += countSpace(vec[numInArray].nesting) + reqAllVoid(
				getVoid(*(void**)argument, vec[numInArray].offset, vec[numInArray].sizeOf), numInArray + 1, vec);

			break;
		case Type::heapPvoid:
			ans += countSpace(vec[numInArray].nesting)
				+ vec[numInArray].info.substr(0, maxLengthOfString)
				+ ": "
				+ MakerString::MakeHexNum((DWORD64)argument)
				+ "\n";
			break;
		case Type::voider:
			ans += countSpace(vec[numInArray].nesting)
				+ vec[numInArray].info.substr(0, maxLengthOfString)
				+ "\n";
			break;
		default:
			break;
		}
		return ans;
	}

	std::string getInfoAboutArgument(const std::string& nameFunction, int numOfArg, void* argument) {
		auto vecInfo = baseOfArgFunctions[nameFunction][numOfArg];
		return reqAllVoid(argument, 0, vecInfo);
	}
	std::string getInfoAboutAns(const std::string& nameFunction, void* answer) {
		auto vecInfo = baseOfAnsFunctions[nameFunction];
		return reqAllVoid(answer, 0, vecInfo);
	}
	std::string getInfoAboutAllArgument(const std::string& nameFunction, const std::vector<void*>& arguments) {
		std::string ans = "\n" + nameFunction.substr(0, maxLengthOfString) + ":\n";
		for (int i = 0; i < arguments.size(); i++) {
			ans += "argument number " + std::to_string(i) + ":\n" + getInfoAboutArgument(nameFunction, i, arguments[i]) + "\n";
		}
		return ans;
	}

	std::vector<std::string> getNamesOfFunctions() {
		std::vector<std::string> ans;
		for (auto i = baseOfArgFunctions.begin(); i != baseOfArgFunctions.end(); i++) {
			ans.push_back((*i).first);
		}
		return ans;
	}

	std::map <std::string, int> getBaseOfCountArg() {
		return this->baseOfCountArg;
	}
};


class BuilderParsingArgument {
private:
	ParsingArgument ans;
public:
	BuilderParsingArgument maxLengthStr(int length) {
		ans.maxLengthOfString = length;
		return *this;
	}
	BuilderParsingArgument maxLengthArray(int length) {
		ans.maxLengthOfArray = length;
		return *this;
	}
	BuilderParsingArgument maxNesting(int length) {
		ans.maxNesting = length;
		return *this;
	}
	ParsingArgument build() {
		return ans;
	}
};

#endif;