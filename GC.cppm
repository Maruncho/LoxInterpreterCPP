
export module GC;

import <unordered_map>;
import <iostream>;

export class Environment;
export class NativeFn;
export class LoxFn;
export class LoxClass;
export class LoxInstance;
export struct Function;

enum class Type : unsigned char {
	ENV,
	NATIVEFN,
	LOXFN,
	LOXCLASS,
	INSTANCE,
	FUNCTION,

	Type_MAX
};

enum class Mark : unsigned char {
	WHITE,
	GRAY,
	BLACK
};

struct Data {
	Type type;
	Mark mark;
	
	inline Data() {};
	inline Data(Type type) : type{ type }, mark{ Mark::WHITE } {}
};

export class GC {
	size_t alloc_limit = 256*256;

	std::unordered_map<void*, Data> allocs;
	size_t alloc_size;

	bool reachedLimit();

	void markOne(void* entry);
	void mark(void* void_ptr, Data& data);
	void markFromList(std::vector<void*> entryPoints);
	void markFromEnv(Environment* env);

	void sweep();
public:
	GC();

	Environment* track(Environment* ptr);
	NativeFn* track(NativeFn* ptr);
	LoxFn* track(LoxFn* ptr);
	LoxClass* track(LoxClass* ptr);
	LoxInstance* track(LoxInstance* ptr);
	Function*	 track(Function*    ptr);

	inline void deleteAll() {
		for (auto x : allocs) {
			delete x.first;
		}
	}

	void runFromEnv(Environment* env);
};

//export GC global_gc;
