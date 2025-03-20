#include <jni.h>
#include <iostream>
#include <string>

int main(int argc, char const *argv[]) {
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

    jclass clazz = env->FindClass("java/lang/String");
    if (clazz == nullptr)
        std::cout << "Failed to find class" << std::endl;

    jclass posit_cls = env->FindClass("ro/upb/nrs/sl/Posit");
    if (posit_cls == nullptr) {
        std::cout << "Failed to find class" << std::endl;
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
        jvm->DestroyJavaVM();
        exit(1);
    }

    jmethodID method = env->GetStaticMethodID(posit_cls, "fromFloatToPosit", "(F)Lro/upb/nrs/sl/Posit_B");
    // env->CallStaticObjectMethod(posit_cls, method, 1.0f);

    // jmethodID mid = env->GetMethodID(cls, "toBinaryString", "");
    delete options;
    /* invoke the Main.test method using the JNI */
    /* We are done. */
    jvm->DestroyJavaVM();
    return 0;
}
