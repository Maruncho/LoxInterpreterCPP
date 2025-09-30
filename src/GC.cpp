
module GC;
import GC;

import Environment;
import Stmt;
import Object;

size_t type_sizes[(int)(Type::Type_MAX)] = {
	sizeof Environment,
	sizeof NativeFn,
	sizeof LoxFn,
	sizeof LoxClass,
	sizeof LoxInstance,
	sizeof Function
};

//void*s do not call destructors.
void deletePtr(void* ptr, Type t) {
	switch (t) {
	case Type::ENV: return delete (Environment*)ptr;
	case Type::NATIVEFN: return delete (NativeFn*)ptr;
	case Type::LOXFN: return delete (LoxFn*)ptr;
	case Type::LOXCLASS: return delete (LoxClass*)ptr;
	case Type::INSTANCE: return delete (LoxInstance*)ptr;
	case Type::FUNCTION: return delete (Function*)ptr;
	}
}

GC::GC() : alloc_size{0} {}

Environment* GC::track(Environment* ptr) { 
	allocs[(void*)ptr] = Data(Type::ENV);
	alloc_size += sizeof Environment;
	return ptr;
}
NativeFn* GC::track(NativeFn* ptr) {
	allocs[(void*)ptr] = Data(Type::NATIVEFN);
	alloc_size += sizeof NativeFn;
	return ptr;
}
LoxFn* GC::track(LoxFn* ptr) {
	allocs[(void*)ptr] = Data(Type::LOXFN);
	alloc_size += sizeof LoxFn;
	return ptr;
}
LoxClass* GC::track(LoxClass* ptr) {
	allocs[(void*)ptr] = Data(Type::LOXCLASS);
	alloc_size += sizeof LoxClass;
	return ptr;
}
LoxInstance* GC::track(LoxInstance* ptr) {
	allocs[(void*)ptr] = Data(Type::INSTANCE);
	alloc_size += sizeof LoxInstance;
	return ptr;
}
Function* GC::track(Function* ptr) {
	allocs[(void*)ptr] = Data(Type::FUNCTION);
	alloc_size += sizeof Function;
	return ptr;
}

bool GC::reachedLimit() {
	if (alloc_size >= alloc_limit) {
		//alloc_limit *= 2; //Some factor
		return true;
	}
	return false;
}

void GC::mark(void* void_ptr, Data& data) {
	if (data.mark == Mark::BLACK)
		return;
	data.mark = Mark::BLACK;

	switch (data.type) {
		case Type::ENV: {
			Environment* ptr = (Environment*)void_ptr;


			if (ptr->enclosing && ptr->enclosing->enclosing != nullptr) markOne(ptr->enclosing);
			for (const auto& [_, value] : ptr->values) {
				if (value.isPointer())
					markOne(value.getPointer());
			}
		} break;
		case Type::NATIVEFN:
			break;
		case Type::LOXFN: {
			LoxFn* ptr = (LoxFn*)void_ptr;

			markOne(ptr->function);
			markOne(ptr->closure);

		} break;
		case Type::LOXCLASS: {
			LoxClass* ptr = (LoxClass*)void_ptr;

			if(ptr->superclass) markOne(ptr->superclass);

			for (const auto& [_, value] : ptr->methods) {
				markOne(value);
			}
		} break;
		case Type::INSTANCE: {
			LoxInstance* ptr = (LoxInstance*)void_ptr;

			markOne(ptr->klass);
			for (const auto& [_, value] : ptr->fields) {
				if (value.isPointer())
					markOne(value.getPointer());
			}
		} break;
		case Type::FUNCTION: {
			Function* ptr = (Function*)void_ptr;

			for (const auto fn : ptr->fns_in_body) {
				markOne(fn);
			}
		} break;
	}
}

void GC::markOne(void* entry) {
	if (auto found = allocs.find(entry); found != allocs.end()) {
		mark(found->first, found->second);
	}
	else {
		std::cerr << "UNTRACKED MEMORY: " << entry << "\n"; //print some stuff.
	}
}

void GC::markFromList(std::vector<void*> entryPoints) {
	for (void* entry : entryPoints) {
		markOne(entry);
	}
}

void GC::markFromEnv(Environment* env) {
	if (auto found = allocs.find((void*)env); found != allocs.end()) {
		mark(found->first, found->second);
	}
	else {
		std::cerr << "UNTRACKED ENVIRONMENT: " << env << "\n"; //print some stuff.
	}
}

void GC::sweep() {
	for (auto iter = allocs.begin(); iter != allocs.end(); ) {
		void* ptr = iter->first;
		Data& data = iter->second;

		if (data.mark == Mark::WHITE) {
			alloc_size -= type_sizes[(int)(data.type)];
			if (ptr != nullptr) //for some reason
				deletePtr(ptr, data.type);
			iter = allocs.erase(iter);
		}
		else {
			data.mark = Mark::WHITE;
			iter++;
		}
	}
}

void GC::runFromEnv(Environment* env) {
	if (!reachedLimit()) return;
	markFromEnv(env);
	sweep();
}
