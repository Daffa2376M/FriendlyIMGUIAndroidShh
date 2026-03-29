#ifndef ImGuiAndroid_ESP
#define ImGuiAndroid_ESP

#include "../ImGui/imgui_internal.h"

namespace ESP {
    
  void DrawLine(ImVec2 start, ImVec2 end,ImVec4 color,float size) {
   auto background = ImGui::GetBackgroundDrawList();
     if(background) {
    background->AddLine(start, end, ImColor(color.x, color.y, color.z, color.w),size);
   }
}

    /*void DrawBox(ImVec4 rect, ImVec4 color, float w) {
        ImVec2 v1(rect.x, rect.y);
        ImVec2 v2(rect.x + rect.z, rect.y);
        ImVec2 v3(rect.x + rect.z, rect.y + rect.w);
        ImVec2 v4(rect.x, rect.y + rect.w);

        DrawLine(v1, v2, color, w);
        DrawLine(v2, v3, color, w);
        DrawLine(v3, v4, color, w);
        DrawLine(v4, v1, color, w);
    }*/
	
	void DrawBox(const ImVec4& rect, const ImVec4& color, float rounding, int flags, float thickness) {
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        draw->AddRect(
            ImVec2(rect.x, rect.y), 
            ImVec2(rect.z, rect.w), 
            ImGui::ColorConvertFloat4ToU32(color), 
            rounding, 
            flags, 
            thickness
        );
    }
	
	
	void DrawBox1(Rect rect, ImVec4 color,float size)
    {
        ImVec2 v1(rect.x, rect.y);
        ImVec2 v2(rect.x + rect.w, rect.y);
        ImVec2 v3(rect.x + rect.w, rect.y + rect.h);
        ImVec2 v4(rect.x, rect.y + rect.h);

        DrawLine(v1, v2, color,size);
        DrawLine(v2, v3, color,size);
        DrawLine(v3, v4, color,size);
        DrawLine(v4, v1, color,size);
    }
	
	Vector3 add(Vector3 a, Vector3 b) {
    Vector3 c;
    c.X= a.X + b.X;
    c.Y = a.Y + b.Y;
    c.Z = a.Z + b.Z;
    return c;
}
	
	
	
	void DrawVerticalHealth(Vector2 end, float h, float health) {
        float x = end.X;
        float y = end.Y;
        h = -h;
        ImVec4 clr = ImVec4(0, 255, 0, 255);

        float hpwidth = h - h * health / 100;

        if (health <= (100 * 0.6)) {
            clr = ImVec4(255, 255, 0, 255);
        }
        if (health < (100 * 0.3)) {
            clr = ImVec4(255, 0, 0, 255);
        }

        Rect hpbar((x + h / 4) - 8, y, 4.0f ,-h);
        Rect hp((x + h / 4) - 8, y - hpwidth, 2.0f, -h + hpwidth);

        DrawBox1(hpbar, ImVec4(0, 0, 0, 255), 3);

        DrawBox1(hp, clr, 3);
    }
	
	
	void DrawCircleHealth(ImVec2 position, int health, int max_health, float raddi) {
    float a_max = ((3.14159265359f * 2.0f));
    ImU32 healthColor = IM_COL32(0, 160, 0, 230); // GREEN
    if (health <= (max_health * 0.6)) {
        healthColor = IM_COL32(255, 255, 0, 110); // YELLOW
    }
    if (health < (max_health * 0.3)) {
        healthColor = IM_COL32(255, 0, 0, 255); // RED
    }
    ImGui::GetBackgroundDrawList()->PathArcTo(position, raddi, (-(a_max / 4.0f)) + (a_max / max_health) * (max_health - health), a_max - (a_max / 4.0f));
    ImGui::GetBackgroundDrawList()->PathStroke(healthColor, ImDrawFlags_None, 4);
}

	
	
	void DrawCircleFilled(ImVec2 end, float radius, ImVec4 color)
    {
        auto background = ImGui::GetBackgroundDrawList();

        if(background)
        {
         background->AddCircleFilled(end, radius, ImColor(color.x, color.y, color.z, color.w)); 
        }
    }
	
	
    void DrawCircle(float X, float Y, float radius, bool filled, ImVec4 color) {
        auto background = ImGui::GetBackgroundDrawList();
        if(background) {
            if(filled) {
                background->AddCircleFilled(ImVec2(X, Y), radius, ImColor(color.x,color.y,color.z,color.w));
            } else {
                background->AddCircle(ImVec2(X, Y), radius, ImColor(color.x,color.y,color.z,color.w));
            }
        }
    }
    void DrawText(float size, ImVec2 position, ImVec4 color, const char *text) {
        auto background = ImGui::GetBackgroundDrawList();
        if(background) {
            background->AddText(NULL, size, position, ImColor(color.x,color.y,color.z,color.w), text);
        }
    }
}

//copy pested hehehe
string float_to_string_health (int value) 
{
    string str; 
    str = "HEALTH : ";
    str += std::to_string(value);
    return str;
}

#endif ImGuiAndroid_ESP
