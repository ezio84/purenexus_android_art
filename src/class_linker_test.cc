// Copyright 2011 Google Inc. All Rights Reserved.

#include "src/common_test.h"
#include "src/class_linker.h"
#include "src/dex_file.h"
#include "src/heap.h"
#include "gtest/gtest.h"

namespace art {

TEST(ClassLinker, LoadNonexistent) {
  scoped_ptr<DexFile> dex(DexFile::OpenBase64(kMyClassDex));
  ASSERT_TRUE(dex != NULL);

  scoped_ptr<ClassLinker> linker(ClassLinker::Create());
  linker->AppendToClassPath(dex.get());

  scoped_ptr<Class> klass(linker.get()->AllocClass(dex.get()));
  bool result1 = linker->LoadClass("NoSuchClass", klass.get());
  EXPECT_FALSE(result1);
  bool result2 = linker->LoadClass("LNoSuchClass;", klass.get());
  EXPECT_FALSE(result2);
}

TEST(ClassLinker, Load) {
  scoped_ptr<DexFile> dex(DexFile::OpenBase64(kNestedDex));
  ASSERT_TRUE(dex != NULL);

  scoped_ptr<ClassLinker> linker(ClassLinker::Create());
  linker->AppendToClassPath(dex.get());

  scoped_ptr<Class> klass(linker.get()->AllocClass(dex.get()));
  bool result = linker->LoadClass("LNested;", klass.get());
  ASSERT_TRUE(result);

  uint32_t vmeth = klass->NumVirtualMethods();
  EXPECT_EQ(vmeth, 0U);

  uint32_t dmeth = klass->NumDirectMethods();
  EXPECT_EQ(dmeth, 1U);
}

TEST(ClassLinker, FindClass) {
  ASSERT_TRUE(Thread::Init());
  ASSERT_TRUE(Thread::Attach() != NULL);

  scoped_ptr<DexFile> dex(DexFile::OpenBase64(kMyClassDex));
  ASSERT_TRUE(dex != NULL);

  scoped_ptr<ClassLinker> linker(ClassLinker::Create());
  linker->AppendToClassPath(dex.get());

  Class* JavaLangObject = linker->FindClass("Ljava/lang/Object;", NULL, dex.get());
  ASSERT_TRUE(JavaLangObject != NULL);
  ASSERT_TRUE(JavaLangObject->GetClass() != NULL);
  ASSERT_EQ(JavaLangObject->GetClass(), JavaLangObject->GetClass()->GetClass());
  EXPECT_EQ(JavaLangObject->GetClass()->GetSuperClass(), JavaLangObject);
  ASSERT_TRUE(JavaLangObject->GetDescriptor() == "Ljava/lang/Object;");
  EXPECT_TRUE(JavaLangObject->GetSuperClass() == NULL);
  EXPECT_FALSE(JavaLangObject->HasSuperClass());
  EXPECT_TRUE(JavaLangObject->GetComponentType() == NULL);
  EXPECT_FALSE(JavaLangObject->IsErroneous());
  EXPECT_FALSE(JavaLangObject->IsVerified());
  EXPECT_TRUE(JavaLangObject->IsLinked());
  EXPECT_FALSE(JavaLangObject->IsArray());
  EXPECT_FALSE(JavaLangObject->IsInterface());
  EXPECT_TRUE(JavaLangObject->IsPublic());
  EXPECT_FALSE(JavaLangObject->IsFinal());
  EXPECT_FALSE(JavaLangObject->IsPrimitive());
  EXPECT_EQ(1U, JavaLangObject->NumDirectMethods());
  EXPECT_EQ(0U, JavaLangObject->NumVirtualMethods());
  EXPECT_EQ(0U, JavaLangObject->NumInstanceFields());
  EXPECT_EQ(0U, JavaLangObject->NumStaticFields());

  Class* MyClass = linker->FindClass("LMyClass;", NULL, dex.get());
  ASSERT_TRUE(MyClass != NULL);
  ASSERT_TRUE(MyClass->GetClass() != NULL);
  ASSERT_EQ(MyClass->GetClass(), MyClass->GetClass()->GetClass());
  EXPECT_EQ(MyClass->GetClass()->GetSuperClass(), JavaLangObject);
  ASSERT_TRUE(MyClass->GetDescriptor() == "LMyClass;");
  EXPECT_TRUE(MyClass->GetSuperClass() == JavaLangObject);
  EXPECT_TRUE(MyClass->HasSuperClass());
  EXPECT_TRUE(MyClass->GetComponentType() == NULL);
  EXPECT_TRUE(MyClass->GetStatus() == Class::kStatusResolved);
  EXPECT_FALSE(MyClass->IsErroneous());
  EXPECT_FALSE(MyClass->IsVerified());
  EXPECT_TRUE(MyClass->IsLinked());
  EXPECT_FALSE(MyClass->IsArray());
  EXPECT_FALSE(MyClass->IsInterface());
  EXPECT_FALSE(MyClass->IsPublic());
  EXPECT_FALSE(MyClass->IsFinal());
  EXPECT_FALSE(MyClass->IsPrimitive());
  EXPECT_EQ(1U, MyClass->NumDirectMethods());
  EXPECT_EQ(0U, MyClass->NumVirtualMethods());
  EXPECT_EQ(0U, MyClass->NumInstanceFields());
  EXPECT_EQ(0U, MyClass->NumStaticFields());

  EXPECT_EQ(JavaLangObject->GetClass()->GetClass(), MyClass->GetClass()->GetClass());

}

}  // namespace art
