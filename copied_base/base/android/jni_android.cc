// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/android/jni_android.h"

#include <stddef.h>

#include <iostream>
#include <regex>
#include <string>

#include "base/android/java_exception_reporter.h"
#include "base/android/jni_string.h"
#include "base/android/jni_utils.h"
#include "base/base_jni/PiiElider_jni.h"
#include "base/logging.h"
#include "build/build_config.h"
#include "third_party/jni_zero/jni_zero.h"

namespace base {
namespace android {
namespace {

jmethodID g_class_loader_load_class_method_id = nullptr;

bool g_fatal_exception_occurred = false;

// ClassLoader.loadClass() accepts either slashes or dots on Android, but JVM
// requires dots. We could translate, but there is no need to go through
// ClassLoaders in Robolectric anyways.
jclass GetClassFromSplit(JNIEnv* env,
                         const char* class_name,
                         const char* split_name) {
  ScopedJavaLocalRef<jstring> j_class_name(env, env->NewStringUTF(class_name));
  return static_cast<jclass>(env->CallObjectMethod(
      GetSplitClassLoader(env, split_name), g_class_loader_load_class_method_id,
      j_class_name.obj()));
}

// Must be called before using GetClassFromSplit - we need to set the global,
// and we need to call GetClassLoader at least once to allow the default
// resolver (env->FindClass()) to get our main ClassLoader class instance, which
// we then cache use for all future calls to GetSplitClassLoader.
void PrepareClassLoaders(JNIEnv* env) {
  if (g_class_loader_load_class_method_id == nullptr) {
    GetClassLoader(env);
    ScopedJavaLocalRef<jclass> class_loader_clazz = ScopedJavaLocalRef<jclass>(
        env, env->FindClass("java/lang/ClassLoader"));
    CHECK(!ClearException(env));
    g_class_loader_load_class_method_id =
        env->GetMethodID(class_loader_clazz.obj(),
                         "loadClass",
                         "(Ljava/lang/String;)Ljava/lang/Class;");
    CHECK(!ClearException(env));
  }
}

// Java exception stack trace example:
//
// java.lang.RuntimeException: Hello
//     at dev.cobalt.media.VideoFrameReleaseTimeHelper.MethodC(VideoFrameReleaseTimeHelper.java:111)
//     at dev.cobalt.media.VideoFrameReleaseTimeHelper.MethodB(VideoFrameReleaseTimeHelper.java:115)
//     at dev.cobalt.media.VideoFrameReleaseTimeHelper.MethodA(VideoFrameReleaseTimeHelper.java:119)
//     at dev.cobalt.media.VideoFrameReleaseTimeHelper.adjustReleaseTime(VideoFrameReleaseTimeHelper.java:135)
std::string GetFirstLine(const std::string& stack_trace) {
  return stack_trace.substr(0, stack_trace.find('\n'));
}

std::string FindTopJavaMethodsAndFiles(const std::string& stack_trace, const size_t max_matches) {
    std::regex pattern("\\.([^.(]+)\\(([^)]+\\.java:\\d+)\\)");

    std::vector<std::string> all_matches;
    std::sregex_iterator it(stack_trace.begin(), stack_trace.end(), pattern);
    std::sregex_iterator end;

    while (it != end && all_matches.size() < max_matches) {
        std::smatch match = *it;

        // match[0] contains the method, file, and line (e.g., ".onCreate(CobaltActivity.java:219)")
        all_matches.push_back(match[0].str());

        ++it; // Move to the next match
    }

    std::ostringstream oss;
    for (size_t i = 0; i < all_matches.size(); ++i) {
        oss << all_matches[i];
        if (i < all_matches.size() - 1) {
            oss << "&";
        }
    }

    return oss.str();
}

}  // namespace

void InitVM(JavaVM* vm) {
  jni_zero::InitVM(vm);
  JNIEnv* env = jni_zero::AttachCurrentThread();
  PrepareClassLoaders(env);
  jni_zero::SetClassResolver(GetClassFromSplit);
}

void CheckException(JNIEnv* env) {
  if (!HasException(env))
    return;

  std::string exception_token;
  jthrowable java_throwable = env->ExceptionOccurred();
  if (java_throwable) {
    // Clear the pending exception, since a local reference is now held.
    env->ExceptionDescribe();
    env->ExceptionClear();

    if (g_fatal_exception_occurred) {
      // Another exception (probably OOM) occurred during GetJavaExceptionInfo.
      base::android::SetJavaException(
          "Java OOM'ed in exception handling, check logcat");
      exception_token = "Java OOM'ed";
    } else {
      g_fatal_exception_occurred = true;
      std::string exception_info = GetJavaExceptionInfo(env, java_throwable);
      base::android::SetJavaException(exception_info.c_str());
      exception_token =
          GetFirstLine(exception_info) + " at " +
          FindTopJavaMethodsAndFiles(exception_info, /*max_matches=*/4);
    }
  }

  // Now, feel good about it and die.
  LOG(FATAL) << "JNI exception: " << exception_token;
}

std::string GetJavaExceptionInfo(JNIEnv* env, jthrowable java_throwable) {
  ScopedJavaLocalRef<jstring> sanitized_exception_string =
      Java_PiiElider_getSanitizedStacktrace(
          env, ScopedJavaLocalRef<jthrowable>(env, java_throwable));

  return ConvertJavaStringToUTF8(sanitized_exception_string);
}

}  // namespace android
}  // namespace base
