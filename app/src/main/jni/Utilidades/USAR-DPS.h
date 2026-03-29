


// precisarei dps


void (*SetResolution)(int width, int height, bool fullscreen);

//--------

          if (ESPDistance) {
                    float distance = Vector3::Distance(position, get_position(getTransform(myPlayer)));
                    std::string distText = std::to_string(static_cast<int>(distance));
                    getDrawList()->AddText(
                        ImGui::GetFont(), 
                        250 / NewPosPlayer.z, 
                        ImVec2(PosPlayer.x - CalcTextSize(distText.c_str()).x / 2, 
                               screenHeight - PosPlayer.y + (50 / NewPosPlayer.z) + (CalcTextSize(distText.c_str()).y / 2)),
                        GetColorU32(*(ImVec4 *)EspDistanceColor), 
                        distText.c_str()
                    );
                }
				
//----------
				
float distance;

                distance = Distancebyirc(GetPlayerLocation(myPlayer), PosPlayer);
			   
			   if (ESPDistance) {
                    char buffer[30];
                    sprintf(buffer, "%.2fm", distance);
                    ESP::DrawText(0.6f, ImVec2(rect.x + (rect.w / 2), rect.y + rect.h + 21.5f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), buffer);
                }
				
				
//----------ESSE AO ATIVAR UM CHECKBOX ELE MOSTRA UMA SUBCATEGORIA-----------------
				
		            ImGui::Checkbox("Enable Laprocada", &Laproco);
					if (Laproco) {
						
						ImGui::Checkbox("Achei tu kk", &Teste);
						ImGui::Checkbox("Achei tu kk", &Teste);
						ImGui::Checkbox("Achei tu kk", &Teste);
					}		
