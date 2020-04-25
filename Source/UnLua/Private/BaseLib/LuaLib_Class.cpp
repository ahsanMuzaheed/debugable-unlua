// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#include "UnLuaEx.h"
#include "LuaCore.h"
#include "ReflectionUtils/ClassDesc.h"

/**
 * Load a class. for example: UClass.Load("/Game/Core/Blueprints/AICharacter.AICharacter_C")
 */
int32 UClass_Load(lua_State *L)
{
    int32 NumParams = lua_gettop(L);
    if (NumParams != 1)
    {
        UE_LOG(LogUnLua, Log, TEXT("%s: Invalid parameters!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    const char *ClassName = lua_tostring(L, 1);
    if (!ClassName)
    {
        UE_LOG(LogUnLua, Log, TEXT("%s: Invalid class name!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    
    FString ClassPath(ClassName);

	bool IsCppClass = ClassPath.StartsWith(TEXT("/Script"));

	if (!IsCppClass)
	{
		const TCHAR *Suffix = TEXT("_C");
		int32 Index = INDEX_NONE;
		ClassPath.FindChar(TCHAR('.'), Index);
		if (Index == INDEX_NONE)
		{
			ClassPath.FindLastChar(TCHAR('/'), Index);
			if (Index != INDEX_NONE)
			{
				const FString Name = ClassPath.Mid(Index + 1);
				ClassPath += TCHAR('.');
				ClassPath += Name;
				ClassPath.AppendChars(Suffix, 2);
			}
		}
		else
		{
			if (ClassPath.Right(2) != TEXT("_C"))
			{
				ClassPath.AppendChars(TEXT("_C"), 2);
			}
		}
	}

    FClassDesc *ClassDesc = RegisterClass(L, TCHAR_TO_ANSI(*ClassPath));
    if (ClassDesc && ClassDesc->AsClass())
    {
        UnLua::PushUObject(L, ClassDesc->AsClass());
    }
    else
    {
        lua_pushnil(L);
    }

    return 1;
}

/**
 * Test whether this class is a child of another class
 */
static int32 UClass_IsChildOf(lua_State *L)
{
    int32 NumParams = lua_gettop(L);
    if (NumParams != 2)
    {
        UE_LOG(LogUnLua, Log, TEXT("%s: Invalid parameters!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    UClass *SrcClass = Cast<UClass>(UnLua::GetUObject(L, 1));
    if (!SrcClass)
    {
        UE_LOG(LogUnLua, Log, TEXT("%s: Invalid source class!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    UClass *TargetClass = Cast<UClass>(UnLua::GetUObject(L, 2));
    if (!TargetClass)
    {
        UE_LOG(LogUnLua, Log, TEXT("%s: Invalid target class!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }

    bool bValid = SrcClass->IsChildOf(TargetClass);
    lua_pushboolean(L, bValid);
    return 1;
}

static const luaL_Reg UClassLib[] =
{
    { "Load", UClass_Load },
    { "IsChildOf", UClass_IsChildOf },
    { nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(UClass)
    ADD_LIB(UClassLib)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(UClass)
