#pragma once


#if defined(_MSC_VER)
	#define SE_INTERFACE __declspec(novtable)
#else
	#define SE_INTERFACE
#endif


#define SE_NO_COPY(CLASS) \
	public: \
		CLASS(const CLASS& _other) = delete; \
		CLASS& operator=(const CLASS& _other) = delete;

#define SE_NO_MOVE(CLASS) \
	public: \
		CLASS(CLASS&& _other) = delete; \
		CLASS& operator=(CLASS&& _other) = delete;

#define SE_NO_COPY_OR_MOVE(CLASS) \
	SE_NO_COPY(CLASS) \
	SE_NO_MOVE(CLASS)
