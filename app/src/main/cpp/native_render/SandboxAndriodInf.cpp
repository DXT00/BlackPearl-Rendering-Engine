#include<stdio.h>
#include "jni.h"
#include "SandBoxAndriod.h"

/*
 * Class:     JT
 * Method:    call
 * Signature: ()V
 */
JNIEXPORT void JNICALL app_init(JNIEnv * env, jobject obj) {
  printf("hello, demo!\n");
  SandBoxAndriod::GetInstance();
}

JNIEXPORT void JNICALL app_run(JNIEnv * env, jobject obj) {
    printf("run, demo!\n");
    SandBoxAndriod::GetInstance()->Run();
}
