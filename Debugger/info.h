#pragma once
#include<string>
#include<map>
#include<vector>


#ifndef _INFO_H_
#define _INFO_H_


enum class Type {
	structure,
	enumerate,
	massif,
	heapPvoid,
	pvoid,
	integer,
	voider,
	heapHandler
};


struct Info {
	Type type; //тип, смотрите сверху
	std::string info; //по умолчанию думал, что использовать typeid().name(), 
	//но сюда может записываться любая информация любого уровня подробности о аргументе и типе аргумента
	size_t offset = 0; //сдвиг от void* 
	size_t sizeOf = 0;//размер 
	size_t sizeOfMassif = 0;//размер массива, если он передан
	size_t nesting = 0; //вложенность
	std::string enumName;//имя набора битовых флагов
};


class BuilderInfo {
private:
	Info ans;
public:
	BuilderInfo() = default;

	BuilderInfo type(Type type) {
		ans.type = type;
		return *this;
	}
	BuilderInfo info(const std::string& info) {
		ans.info = info;
		return *this;
	}
	BuilderInfo offset(size_t offset) {
		ans.offset = offset;
		return *this;
	}
	BuilderInfo sizeOf(size_t sizeOf) {
		ans.sizeOf = sizeOf;
		return *this;
	}
	BuilderInfo size(size_t size) {
		ans.sizeOfMassif = size;
		return *this;
	}
	BuilderInfo nesting(size_t nesting) {
		ans.nesting = nesting;
		return *this;
	}
	BuilderInfo enumName(const std::string& enumName) {
		ans.enumName = enumName;
		return *this;
	}
	Info build() {
		return ans;
	}
};

#endif;