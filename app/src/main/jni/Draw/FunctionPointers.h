//
// Created by letha on 9/4/2021.
//
#ifndef IMGUIANDROID_FUNCTIONPOINTERS_H
#define IMGUIANDROID_FUNCTIONPOINTERS_H
using namespace BNM::UNITY_STRUCTS;
//using namespace Hooks;
namespace Pointers
{

    void * (*Component_get_transform)(void *instance);

    Vector3 (*Transform_get_position)(void *instance);

    Vector3 (*Camera_WorldToScreenPoint)(void *instance, Vector3 worldPosition);

    void *(*Camera_get_main)();


    void LoadPointers()
    {
        Component_get_transform =
                *(void *(*)(void *)) OBFUSCATE_BYNAME_METHOD("UnityEngine", "Component",
                                                             "get_transform", 0);
        Transform_get_position =
                *(Vector3 (*)(void *)) OBFUSCATE_BYNAME_METHOD("UnityEngine", "Transform",
                                                               "get_position", 0);
        Camera_WorldToScreenPoint =
                *(Vector3 (*)(void *, Vector3)) OBFUSCATE_BYNAME_METHOD("UnityEngine", "Camera",
                                                                        "WorldToScreenPoint", 1);
        Camera_get_main =
                *(void *(*)()) OBFUSCATE_BYNAME_METHOD("UnityEngine", "Camera",
                                                                        "get_main", 0);
  
        DWORD OnUpdate = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("CharacterMotor")).GetMethodOffsetByName(OBFUSCATE_BNM("Update"), 0);      
        DWORD OnDestroy = LoadClass(OBFUSCATE_BNM(""), OBFUSCATE_BNM("CharacterMotor")).GetMethodOffsetByName(OBFUSCATE_BNM("OnDestroy"), 0);        
        hook((void *) OnUpdate, (void *) &_Player_Update, (void **) &Player_Update);
        hook((void *) OnDestroy, (void *) &_Player_Ondestroy, (void **) &Player_Ondestroy);
 
                                                          
   }
}
#endif IMGUIANDROID_FUNCTIONPOINTERS_H
