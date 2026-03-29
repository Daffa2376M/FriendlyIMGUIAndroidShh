


/*void *getTransform(void *player) {
    if (!player) return NULL;
    static const auto get_transform_injected = reinterpret_cast<uint64_t(__fastcall *)(void *)>(getAbsoluteAddress("libil2cpp.so", 0x1A89D1C)); //SKY •0x8C8A64
    return (void *) get_transform_injected(player);
}

Vector3 get_position(void *transform) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto get_position_injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3 &)>(getAbsoluteAddress("libil2cpp.so", 0x1A92AF4)); //SKY •0x67EB10
    get_position_injected(transform, position);
    return position;
}


Vector3 WorldToScreenPoint(void *transform, Vector3 test) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto WorldToScreenPoint_Injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3, int, Vector3 &)>(getAbsoluteAddress("libil2cpp.so", 0x1A84198)); // SKY •0x8C66A8
    WorldToScreenPoint_Injected(transform, test, 4, position);
    return position;
}

void *get_camera() {
    static const auto get_camera_injected = reinterpret_cast<uint64_t(__fastcall *)()>(getAbsoluteAddress("libil2cpp.so", 0x1A84C98)); //SKY •0x8C6E30
    return (void *) get_camera_injected();
}

Vector3 GetPlayerLocation(void *Player) {
    return get_position(getTransform(Player));
}*/


namespace getAddr {
    uintptr_t Transform;
    uintptr_t Position;
    uintptr_t SetPosition;
    uintptr_t WorldTScrn;
    uintptr_t Camera;
}


void *getTransform(void *player) {
    if (!player) return NULL;
    static const auto get_transform_injected = reinterpret_cast<uint64_t(__fastcall *)(void *)>(getAddr::Transform);
    return (void *) get_transform_injected(player);
}

Vector3 get_position(void *transform) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto get_position_injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3 &)>(getAddr::Position);
    get_position_injected(transform, position);
    return position;
}

void set_position(void* transform, Vector3 test) {
    if (transform) {
        static const auto set_position_injected = reinterpret_cast<uintptr_t(__fastcall*)(void*, Vector3)>(getAddr::SetPosition);
        set_position_injected(transform, test);
    }
}

Vector3 WorldToScreenPoint(void *transform, Vector3 test) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto WorldToScreenPoint_Injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3, int, Vector3 &)>(getAddr::WorldTScrn);
    WorldToScreenPoint_Injected(transform, test, 4, position);
    return position;
}

void *get_camera() {
    static const auto get_camera_injected = reinterpret_cast<uint64_t(__fastcall *)()>(getAddr::Camera);
    return (void *) get_camera_injected();
}


Vector3 GetPlayerLocation(void *player) {
    return get_position(getTransform(player));
}




//----------------

/*
float GetPlayerHealth(void *player) {
    return *(float *) ((uintptr_t) player + isHealth);
}

bool PlayerAlive(void *player) {
    return player != NULL && GetPlayerHealth(player) > 0;
}

bool IsPlayerDead(void *player) {
    return player == NULL && GetPlayerHealth(player) < 1;
}
*/

string float_to_string (int value)
{
    string str;
    str = std::to_string(value);
    str += "M";
    return str;
}


