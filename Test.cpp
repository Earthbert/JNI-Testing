#include <jni.h>
#include <iostream>
#include <string>

JNIEnv *initialize_jvm() {
    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption *options = new JavaVMOption[1];

    options[0].optionString = const_cast<char *>(
        "-Djava.class.path=jars/nrssl.jar"
        ":jars/scala-library-2.13.8.jar");
    vm_args.version = JNI_VERSION_10;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;

    auto ret = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
    if (ret != JNI_OK)
        std::cout << "Failed to create Java VM" << std::endl;

    delete options;

    return env;
}

jclass initialize_jclass(JNIEnv *env, const char *class_name) {
    jclass clazz = env->FindClass(class_name);
    if (clazz == nullptr) {
        std::cout << "Failed to find " << class_name << std::endl;
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        JavaVM *jvm;
        env->GetJavaVM(&jvm);
        jvm->DestroyJavaVM();
        exit(1);
    }
    return clazz;
}

jmethodID get_method(JNIEnv *env, jclass clazz, const char *method_name, const char *signature, bool is_static = false) {
    jmethodID method = is_static
        ? env->GetStaticMethodID(clazz, method_name, signature)
        : env->GetMethodID(clazz, method_name, signature);
    if (method == nullptr) {
        std::cout << "Failed to find \"" << method_name << "\" sig: " << signature << std::endl;
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        JavaVM *jvm;
        env->GetJavaVM(&jvm);
        jvm->DestroyJavaVM();
        exit(1);
    }
    return method;
}

int main(int argc, char const *argv[]) {
    JavaVM *jvm;
    JNIEnv *env = initialize_jvm();
    env->GetJavaVM(&jvm);

    jclass class_posit = initialize_jclass(env, "ro/upb/nrs/sl/Posit");
    jclass class_posit_b = initialize_jclass(env, "ro/upb/nrs/sl/Posit_B");

    const char *binary_string;

    {
        jmethodID method_posit_apply_double = get_method(env, class_posit, "apply", "(D)Lro/upb/nrs/sl/Posit_B;", true);
        jmethodID method_posit_b_to_binary_string_clazz = get_method(env, class_posit_b, "toBinaryString", "()Ljava/lang/String;");

        jobject posit_1 = env->CallStaticObjectMethod(class_posit, method_posit_apply_double, 666.555);

        jobject posit_binary_string = env->CallObjectMethod(posit_1, method_posit_b_to_binary_string_clazz);

        binary_string = env->GetStringUTFChars((jstring)posit_binary_string, nullptr);
    }

    std::cout << "Binary string: " << binary_string << std::endl;

    {
        jmethodID method_posit_default_rounding = get_method(env, class_posit, "default_rounding", "()Lro/upb/nrs/sl/RoundingType;", true);
        jmethodID method_posit_default_size = get_method(env, class_posit, "default_size", "()I", true);
        jmethodID method_posit_default_exponent_size = get_method(env, class_posit, "default_exponent_size", "()I", true);

        jmethodID method_posit_apply = get_method(env, class_posit, "apply", "(Ljava/lang/String;IILro/upb/nrs/sl/RoundingType;)Lro/upb/nrs/sl/Posit_B;", true);
        jmethodID method_posit_b_to_double = get_method(env, class_posit_b, "toDouble", "()D");

        jobject rounding_type = env->CallStaticObjectMethod(class_posit, method_posit_default_rounding);
        jint size = env->CallStaticIntMethod(class_posit, method_posit_default_size);
        jint exponent_size = env->CallStaticIntMethod(class_posit, method_posit_default_exponent_size);

        jobject posit_1 = env->CallStaticObjectMethod(class_posit, method_posit_apply, env->NewStringUTF(binary_string), exponent_size, size, rounding_type);

        jdouble double_value = env->CallDoubleMethod(posit_1, method_posit_b_to_double);

        std::cout << "Double value: " << double_value << std::endl;
    }

    jvm->DestroyJavaVM();
    return 0;
}
