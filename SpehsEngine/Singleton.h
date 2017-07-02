#pragma once
#define SPEHS_SINGLETON_DECLARATION(MyClass) static MyClass* instance;
#define SPEHS_SINGLETON_IMPLEMENTATION(MyClass) MyClass* MyClass::instance = nullptr;
#define SPEHS_SINGLETON_CONSTRUCTOR(MyClass) \
	if (instance) \
		spehs::exceptions::unexpectedError(#MyClass " singleton instance already exists!"); \
	else \
		instance = this;
#define SPEHS_SINGLETON_DESTRUCTOR(MyClass) \
	if (instance == this) \
		instance = nullptr;