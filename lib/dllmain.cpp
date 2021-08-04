// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "jni.h"
#include "classes.h"
#include "loader.h"
#include <iostream>
#include <stdio.h>


DWORD WINAPI main_thread(LPVOID) {

	std::cout << "thread start success!" << std::endl;
	const auto jvm_dll = GetModuleHandleA("jvm.dll");
	if (!jvm_dll) {
		MessageBox(nullptr, "Can't find jvm.dll module handle", "ELoader", MB_OK | MB_ICONERROR);
		return 1;
	}
	std::cout << "dll finded" << std::endl;
	const auto jni_get_created_vms_pointer = GetProcAddress(jvm_dll, "JNI_GetCreatedJavaVMs");
	if (!jni_get_created_vms_pointer) {
		MessageBox(nullptr, "Can't find JNI_GetCreatedJavaVMs proc handle", "ELoader", MB_OK | MB_ICONERROR);
		return 1;
	}
	std::cout << "address finded" << std::endl;
	typedef jint(JNICALL* jni_get_created_java_vms_type)(JavaVM**, jsize, jsize*);
	const auto jni_get_created_java_vms_proc = jni_get_created_java_vms_type(jni_get_created_vms_pointer);
	auto n_vms = 1;
	JavaVM* jvm = nullptr;
	jni_get_created_java_vms_proc(&jvm, n_vms, &n_vms);
	if (n_vms == 0) {
		MessageBox(nullptr, "JVM not found!", "ELoader", MB_OK | MB_ICONERROR);
		return 1;
	}
	std::cout << "jvm finded" << std::endl;
	JNIEnv* jni_env = nullptr;
	jvm->AttachCurrentThread(reinterpret_cast<void**>(&jni_env), nullptr);
	jvm->GetEnv(reinterpret_cast<void**>(&jni_env), JNI_VERSION_1_8);
	if (!jni_env) {
		MessageBox(nullptr, "Can't attach to JNIEnv", "ELoader", MB_OK | MB_ICONERROR);
		jvm->DetachCurrentThread();
		return 1;
	}
	std::cout << "jnienv attached" << std::endl;
	const auto loader_clazz = jni_env->DefineClass(nullptr, nullptr, reinterpret_cast<jbyte*>(class_loader_class), class_loader_class_size);
	if (!loader_clazz) {
		MessageBox(nullptr, "Error on class defining", "ELoader", MB_OK | MB_ICONERROR);
		jvm->DetachCurrentThread();
		return 1;
	}
	std::cout << "loader class loaded" << std::endl;
	const jobjectArray classes_data = jobjectArray(jni_env->CallStaticObjectMethod(loader_clazz,
		jni_env->GetStaticMethodID(loader_clazz, "getByteArray", "(I)[[B"),
		jint(classes_count)));

	std::cout << "inject classes..." << std::endl;
	if (classes_count < 1)
	{
		std::cout << "no class need inject" << std::endl;
	}
	else {
		std::cout << "will inject " << classes_count << " classes" << std::endl;
		auto class_ptr = 0;
		for (auto j = 0; j < classes_count; j++) {
			jbyteArray class_byte_array = jni_env->NewByteArray(jsize(classes_sizes[j]));
			jni_env->SetByteArrayRegion(class_byte_array, 0, jsize(classes_sizes[j]), reinterpret_cast<jbyte*>(classes + class_ptr));
			class_ptr += classes_sizes[j];
			jni_env->SetObjectArrayElement(classes_data, j, class_byte_array);
		}

		const auto inject_result = jni_env->CallStaticIntMethod(loader_clazz, jni_env->GetStaticMethodID(loader_clazz, "injectCP", "([[B)I"), classes_data);
		if (inject_result) {
			MessageBox(nullptr, "Error on injecting: injectResult != 0", "ELoader", MB_OK | MB_ICONERROR);
			jvm->DetachCurrentThread();
			return 1;
		}
		std::cout << "class inject success!" << std::endl;
	}
	
	jvm->DetachCurrentThread();
	std::cout << "exit thread" << std::endl;
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	std::cout << "InjectSuccess!" << std::endl;
	//&& ul_reason_for_call != DLL_THREAD_ATTACH
	if (ul_reason_for_call != DLL_PROCESS_ATTACH)
		return TRUE;
	CreateThread(nullptr, 0, main_thread, nullptr, 0, nullptr);
	return TRUE;
}

