
#ifndef ImGuiAndroid_Drawing
#define ImGuiAndroid_Drawing

#include "ImGui/imgui_internal.h"


namespace ESP
   {
       
void DrawLine(ImVec2 start, ImVec2 end,ImVec4 color,float size)
   {
   auto background = ImGui::GetBackgroundDrawList();
   if(background)
   {
    background->AddLine(start, end, ImColor(color.x, color.y, color.z, color.w),size);
   }
}

 void DrawText(float fontSize, ImVec2 position, ImVec4 color, const char *text)
    {
        auto background = ImGui::GetBackgroundDrawList();

        if(background)
        {
            background->AddText(NULL, fontSize, position, ImColor(color.x, color.y, color.z, color.w), text);
        }
    }

     
void DrawBox(Rect rect, ImVec4 color,float size)
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
    c.x= a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
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

        DrawBox(hpbar, ImVec4(0, 0, 0, 255), 3);

        DrawBox(hp, clr, 3);
    }

/*
    void DrawHorizontalHealthBar(Vector2 screenPos, float width, float maxHealth, float currentHealth, ImVec4 clr) {
        screenPos -= Vector2(0.0f, 8.0f);
        DrawBox(Rect(screenPos.X, screenPos.Y, width + 2, 5.0f), ImVec4(0, 0, 0, 255));
        screenPos += Vector2(1.0f, 1.0f);
        float hpWidth = (currentHealth * width) / maxHealth;
        if (currentHealth <= (maxHealth * 0.6)) {
            clr = ImVec4(255, 255, 0, 255);
        }
        if (currentHealth < (maxHealth * 0.3)) {
            clr = ImVec4(255, 0, 0, 255);
        }
        if (currentHealth <= (maxHealth * 0.0)) {
            clr = ImVec4(0, 0, 0, 0);
        }
        DrawBox(Rect(screenPos.X, screenPos.Y, hpWidth, 3.0f), clr);
    }*/

/*
    void DrawCrosshair(ImVec4 color, Vector2 center, float size = 20) {
        float x = center.X - (size / 2.0f);
        float y = center.Y - (size / 2.0f);
        DrawLine(ImVec2(x, center.Y), ImVec2(x + size, center.Y), ImVec4(120, 120, 120, 120));
        DrawLine(ImVec2(center.X, y), ImVec2(center.X, y + size), ImVec4(120, 120, 120, 120));
    }*/




 
void DrawCircleFilled(ImVec2 end, float radius, ImVec4 color)
    {
        auto background = ImGui::GetBackgroundDrawList();

        if(background)
        {
         

                background->AddCircleFilled(end, radius, ImColor(color.x, color.y, color.z, color.w));
            

            
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






#endif
