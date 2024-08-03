#pragma once

#include "Engine/MiscMacros.h"
#include "Meta/MetaAttributes.h"

#define UM_CLASS(...)
#define UM_ENUM(...)
#define UM_FUNCTION(...)
#define UM_PROPERTY(...)
#define UM_STRUCT(...)

#define UM_GENERATED_BODY_IMPL_2(FileId, Line) FileId##_##Line##_GENERATED_BODY()
#define UM_GENERATED_BODY_IMPL_1(FileId, Line) UM_GENERATED_BODY_IMPL_2(FileId, Line)
#define UM_GENERATED_BODY() UM_GENERATED_BODY_IMPL_1(UMBRAL_CURRENT_FILE_ID, UMBRAL_LINE)

#define Abstract UM_META_ATTRIBUTE_TYPE(Abstract)
#define ChildOf UM_META_ATTRIBUTE_TYPE(ChildOf)