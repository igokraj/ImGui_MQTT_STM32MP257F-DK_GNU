#include "calculator.h"
#include "imgui.h"
#include <stdio.h>

void Calculator::draw() {

ImGui::SetNextWindowSize(ImVec2(600, 600));

        // Set the position of the window at the start of the app
        ImGui::SetNextWindowPos(ImVec2(50, 200), ImGuiCond_Once);
        ImGui::Begin("Calculator app", NULL, ImGuiWindowFlags_NoResize);

        ImGui::Text("First number is: %s", input1.c_str());
        ImGui::Text("Second number is: %s", input2.c_str());
        ImGui::Text("Result is: %g", res);

        const char *tab[] = {"+", "-", "/", "*"};

        for (int j = 0; j < 3; j++)
        {
            for (int i = 1; i <= 3; i++)
            {
                int num = j * 3 + i;
                if (ImGui::Button(std::to_string(num).c_str(), ImVec2(100, 100)))
                {
                    if (op_active)
                    {
                        input2 += std::to_string(num);
                    }
                    else
                    {
                        if (result_shown)
                        {
                            input1 = "";
                            result_shown = false;
                        }
                        input1 += std::to_string(num);
                    }
                }
                ImGui::SameLine();
            }
            if (ImGui::Button(tab[j], ImVec2(100, 100)))
            {
                op_active = true;
                oper = tab[j];
                result_shown = false;
            }
        }

        // 4. rząd: 0, C, =, *
        if (ImGui::Button("0", ImVec2(100, 100)))
        {
            if (op_active)
            {
                input2 += "0";
            }
            else
            {
                if (result_shown)
                {
                    input1 = "";
                    result_shown = false;
                }
                input1 += "0";
            }
        }
        ImGui::SameLine();

        if (ImGui::Button(".", ImVec2(100, 100)))
        {
            if (!op_active && result_shown)
            {
                input1 = "";
                result_shown = false;
            }
            std::string &in = op_active ? input2 : input1;
            // only one dot is allowed
            if (in.find('.') == std::string::npos)
            {
                if (in.empty())
                    in = "0"; // "." -> "0."
                in += ".";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("C", ImVec2(100, 100)))
        {
            input1 = "";
            input2 = "";
            op_active = false;
            oper = "";
            res = 0;
            result_shown = false;
        }
        ImGui::SameLine();

        if (ImGui::Button(tab[3], ImVec2(100, 100)))
        {
            op_active = true;
            oper = tab[3];
            result_shown = false;
        }
        ImGui::SameLine();

        if (ImGui::Button("=", ImVec2(100, 100)))
        {
            if (!input1.empty() && !input2.empty() && oper[0] != '\0')
            {
                double number1 = std::stod(input1);
                double number2 = std::stod(input2);
                switch (oper[0])
                {
                case '+':
                    res = number1 + number2;
                    break;
                case '-':
                    res = number1 - number2;
                    break;
                case '*':
                    res = number1 * number2;
                    break;
                case '/':
                    if (number2 != 0)
                        res = number1 / number2;
                    break;
                }
                op_active = false;
                oper = "";
                char buf[32];
                snprintf(buf, sizeof(buf), "%g", res);
                input1 = buf;
                input2 = "";
                result_shown = true;
            }
        }
        ImGui::SameLine();

        ImGui::End();



}