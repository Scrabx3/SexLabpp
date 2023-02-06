#pragma once

namespace Script
{
	using VM = RE::BSScript::Internal::VirtualMachine;
	using ObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
	using ArrayPtr = RE::BSTSmartPointer<RE::BSScript::Array>;
	using CallbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;
	using Args = RE::BSScript::IFunctionArguments;

	inline RE::VMHandle GetHandle(const RE::TESForm* a_form)
	{
		auto vm = VM::GetSingleton();
		auto policy = vm->GetObjectHandlePolicy();
		return policy->GetHandleForObject(a_form->GetFormType(), a_form);
	}

	inline ObjectPtr GetScriptObject(const RE::TESForm* a_form, const char* a_class, bool a_create = false)
	{
		auto vm = VM::GetSingleton();
		auto handle = GetHandle(a_form);

		ObjectPtr object = nullptr;
		bool found = vm->FindBoundObject(handle, a_class, object);
		if (!found && a_create) {
			vm->CreateObject2(a_class, object);
			vm->BindObject(object, handle, false);
		}

		return object;
	}

	template <class T>
	inline T GetProperty(ObjectPtr a_obj, const RE::BSFixedString& a_prop)
	{
		auto var = a_obj->GetProperty(a_prop);
		assert(var);
		return RE::BSScript::UnpackValue<T>(var);
	}

	template <class T>
	inline void SetProperty(ObjectPtr a_obj, const RE::BSFixedString& a_prop, T a_val)
	{
		auto var = a_obj->GetProperty(a_prop);
		assert(var);
		RE::BSScript::PackValue(var, a_val);
	}

	inline bool DispatchMethodCall(ObjectPtr a_obj, const RE::BSFixedString& a_fnName, CallbackPtr a_callback, Args* a_args)
	{
		auto vm = VM::GetSingleton();
		return vm->DispatchMethodCall(a_obj, a_fnName, a_args, a_callback);
	}

	template <class... Args>
	inline bool DispatchMethodCall(ObjectPtr a_obj, const RE::BSFixedString& a_fnName, CallbackPtr a_callback, Args&&... a_args)
	{
		auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
		return DispatchMethodCall(a_obj, a_fnName, a_callback, args);
	}

	inline bool DispatchStaticCall(const RE::BSFixedString& a_class, const RE::BSFixedString& a_fnName, CallbackPtr a_callback, Args* a_args)
	{
		auto vm = VM::GetSingleton();
		return vm->DispatchStaticCall(a_class, a_fnName, a_args, a_callback);
	}

	template <class... Args>
	inline bool DispatchStaticCall(const RE::BSFixedString& a_class, const RE::BSFixedString& a_fnName, CallbackPtr a_callback, Args&&... a_args)
	{
		auto args = RE::MakeFunctionArguments(std::forward<Args>(a_args)...);
		return DispatchStaticCall(a_class, a_fnName, a_callback, args);
	}
}
	