// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_JNI_ANDROID_H_
#define BASE_ANDROID_JNI_ANDROID_H_

#include <jni.h>
#include <sys/types.h>

#include <string>

#include "third_party/jni_zero/jni_zero.h"
#include "base/android/scoped_java_ref.h"
#include "base/base_export.h"
#include "base/compiler_specific.h"

namespace base {
namespace android {

// Used to mark symbols to be exported in a shared library's symbol table.
#define JNI_EXPORT __attribute__ ((visibility("default")))

// Contains the registration method information for initializing JNI bindings.
struct RegistrationMethod {
  const char* name;
  bool (*func)(JNIEnv* env);
};

// Attaches the current thread to the VM (if necessary) and return the JNIEnv*.
inline JNIEnv* AttachCurrentThread() {
  return jni_zero::AttachCurrentThread();
}

// Same to AttachCurrentThread except that thread name will be set to
// |thread_name| if it is the first call. Otherwise, thread_name won't be
// changed. AttachCurrentThread() doesn't regard underlying platform thread
// name, but just resets it to "Thread-???". This function should be called
// right after new thread is created if it is important to keep thread name.
inline JNIEnv* AttachCurrentThreadWithName(const std::string& thread_name) {
  return jni_zero::AttachCurrentThreadWithName(thread_name);
}

// Detaches the current thread from VM if it is attached.
inline void DetachFromVM() {
  jni_zero::DetachFromVM();
}

// Initializes the global JVM.
BASE_EXPORT void InitVM(JavaVM* vm);

// Returns true if the global JVM has been initialized.
inline bool IsVMInitialized() {
  return jni_zero::IsVMInitialized();
}

// Returns the global JVM, or nullptr if it has not been initialized.
inline JavaVM* GetVM() {
  return jni_zero::GetVM();
}

// Finds the class named |class_name| and returns it.
// Use this method instead of invoking directly the JNI FindClass method (to
// prevent leaking local references).
// This method triggers a fatal assertion if the class could not be found.
// Use HasClass if you need to check whether the class exists.
inline ScopedJavaLocalRef<jclass> GetClass(JNIEnv* env,
                                           const char* class_name) {
  return jni_zero::GetClass(env, class_name);
}

using MethodID = jni_zero::MethodID;

// Returns true if an exception is pending in the provided JNIEnv*.
inline bool HasException(JNIEnv* env) {
  return jni_zero::HasException(env);
}

// If an exception is pending in the provided JNIEnv*, this function clears it
// and returns true.
inline bool ClearException(JNIEnv* env) {
  return jni_zero::ClearException(env);
}

// This function will call CHECK() macro if there's any pending exception.
BASE_EXPORT void CheckException(JNIEnv* env);

// This returns a string representation of the java stack trace.
BASE_EXPORT std::string GetJavaExceptionInfo(JNIEnv* env,
                                             jthrowable java_throwable);

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_JNI_ANDROID_H_
