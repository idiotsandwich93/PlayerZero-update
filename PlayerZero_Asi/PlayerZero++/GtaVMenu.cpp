/*
	   GTAV Menu System
	--By Adopocalipt 2024--
*/

#include "..\inc\types.h"
#include "..\inc\natives.h"

#include <sstream>
#include <string>
#include <iomanip> // For std::fixed and std::setprecision
#include "keyboard.h"
#include "GtaVMenu.h"

namespace GVM
{
	std::string ConvertWideToUtf8(const std::wstring& wstr)
	{
		int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
	}
	std::string GetExeFileName()
	{
		WCHAR buffer[MAX_PATH]{};
		GetModuleFileNameW(NULL, buffer, ARRAYSIZE(buffer));

		return ConvertWideToUtf8(std::wstring(buffer));
	}
	std::string GetDir()
	{
		std::string FileName = GetExeFileName();
		std::string Dir = FileName.substr(0, FileName.find_last_of("\\/"));

		return Dir;
	}
	
	Hash MyHashKey(const std::string& name)
	{
		return GAMEPLAY::GET_HASH_KEY((LPSTR)name.c_str());
	}
	int AddMeunBanner(const std::string& file)
	{
		return createTexture(file.c_str());
	}
	std::string FloatToString(float number)
	{
		std::ostringstream Oss;
		Oss << std::fixed << std::setprecision(2) << number;
		return Oss.str();
	}
	int AddScreenTextCenter(int font, const std::string& caption, float textPositionScaled, float boxTopScaled, float lineHeightScaled, float text_scale, RGBA text_col)
	{
		// text upper part
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(0.0, text_scale);
		UI::SET_TEXT_COLOUR(text_col.R, text_col.G, text_col.B, text_col.A);
		UI::SET_TEXT_CENTRE(0);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		UI::_SET_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
		UI::_DRAW_TEXT(textPositionScaled, (((boxTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

		// text lower part
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(0.0, text_scale);
		UI::SET_TEXT_COLOUR(text_col.R, text_col.G, text_col.B, text_col.A);
		UI::SET_TEXT_CENTRE(0);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		UI::_SET_TEXT_GXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
		return UI::_0x9040DFB09BE75706(textPositionScaled, (((boxTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));
	}
	int AddScreenText(int font, const std::string& caption, float wapStart, float wapEnd, float boxPositionScaled, float textPositionScaled, float boxTopScaled, float lineHeightScaled, float text_scale, RGBA text_col, int rightIndent)
	{
		// text upper part
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(0.0, text_scale);
		UI::SET_TEXT_COLOUR(text_col.R, text_col.G, text_col.B, text_col.A);
		UI::SET_TEXT_WRAP(wapStart, wapEnd);
		UI::SET_TEXT_JUSTIFICATION(rightIndent);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		UI::_SET_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
		UI::_DRAW_TEXT(textPositionScaled, (((boxTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

		// text lower part
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(0.0, text_scale);
		UI::SET_TEXT_COLOUR(text_col.R, text_col.G, text_col.B, text_col.A);
		UI::SET_TEXT_WRAP(wapStart, wapEnd);
		UI::SET_TEXT_JUSTIFICATION(rightIndent);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		UI::_SET_TEXT_GXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
		return UI::_0x9040DFB09BE75706(textPositionScaled, (((boxTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));
	}
	void AddBanner(const std::string& title, const std::string& spriteLocation, const std::string& spriteName, const GVM::GVMRatioSet& scRatio)
	{
		float text_scale = 0.75f;

		int font = 1;

		float posX, posY, sizeX, sizeY, heading = 0.0f;
		float textPosX, textPosY;

		posX = scRatio.Banner_posX;
		posY = scRatio.Banner_posY;
		sizeX = scRatio.Banner_sizeX;
		sizeY = scRatio.Banner_sizeY;
		textPosX = scRatio.BanTextOffset_X;
		textPosY = scRatio.BanTextOffset_Y;

		if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((LPSTR)spriteLocation.c_str()))
			GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((LPSTR)spriteLocation.c_str(), true);

		int iWidth, iHeight;
		GRAPHICS::GET_SCREEN_RESOLUTION(&iWidth, &iHeight);

		float num = (float)iWidth / (float)iHeight;
		float num2 = (float)1080.0 * num;
		float width = posX / num2;
		float height = posY / 1080;
		float screenX = sizeX / num2 + width * 0.5f;
		float screenY = sizeY / 1080 + height * 0.5f;


		float textPositionScaled = textPosX / (float)iWidth; // text left offset
		float boxTopScaled = textPosY / (float)iHeight; // line top offset
		float lineHeightScaled = textPosY / (float)iHeight; // line height

		GRAPHICS::DRAW_SPRITE((LPSTR)spriteLocation.c_str(), (LPSTR)spriteName.c_str(), screenX, screenY, width, height, heading, scRatio.Banner_RGBA.R, scRatio.Banner_RGBA.G, scRatio.Banner_RGBA.B, scRatio.Banner_RGBA.A);
		int num25 = AddScreenTextCenter(font, title, textPositionScaled, boxTopScaled, lineHeightScaled, text_scale, scRatio.text_RGBA);
	}
	void AddSprite(const std::string& spriteLocation, const std::string& spriteName, const GVM::GVMRatioSet& scRatio, bool arrowLeft, bool arrowRight, int place, int low, bool active, float leftIndent)
	{
		float posX, posY, sizeX, sizeY, heading = 0.0f;

		if (arrowLeft)
		{
			posX = scRatio.arrowTickSizeX;
			posY = scRatio.arrowTickSizeY;
			sizeX = scRatio.arrowNumberStart - leftIndent + scRatio.arrowLeft;
			sizeY = (scRatio.arrowHeight + place - low)* scRatio.tickMulti;
		}
		else if (arrowRight)
		{
			posX = scRatio.arrowTickSizeX;
			posY = scRatio.arrowTickSizeY;
			sizeX = scRatio.arrowNumberStart + scRatio.arrowRight;
			sizeY = (scRatio.arrowHeight + place - low)* scRatio.tickMulti;
		}
		else
		{
			posX = scRatio.tickSizeX;
			posY = scRatio.tickSizeY;
			sizeX = scRatio.tickX;
			sizeY = (scRatio.tickY + place - low)* scRatio.tickMulti;
		}

		if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((LPSTR)spriteLocation.c_str()))
			GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((LPSTR)spriteLocation.c_str(), true);
	
		int red = scRatio.text_RGBA.R;
		int blue = scRatio.text_RGBA.G;
		int green = scRatio.text_RGBA.B;
		int alpha = scRatio.text_RGBA.A;
		if (arrowLeft || arrowRight) 
		{
			if (active)
			{
				red = scRatio.textActive_RGBA.R;
				blue = scRatio.textActive_RGBA.G;
				green = scRatio.textActive_RGBA.B;
				alpha = scRatio.textActive_RGBA.A;
			}
		}

		int iWidth, iHeight;
		GRAPHICS::GET_SCREEN_RESOLUTION(&iWidth, &iHeight);

		float num = (float)iWidth / (float)iHeight;
		float num2 = (float)1080.0 * num;
		float width = posX / num2;
		float height = posY / 1080;
		float screenX = sizeX / num2 + width * 0.5f;
		float screenY = sizeY / 1080 + height * 0.5f;
		GRAPHICS::DRAW_SPRITE((LPSTR)spriteLocation.c_str(), (LPSTR)spriteName.c_str(), screenX, screenY, width, height, heading, red, green, blue, alpha);
	}
	void Draw_rect(float posX, float posY, float width, float height, int colourR, int colourG, int colourB, int colourA)
	{
		GRAPHICS::DRAW_RECT((posX + (width * 0.5f)), (posY + (height * 0.5f)), width, height, colourR, colourG, colourB, colourA);
	}
	void Draw_menu_line(const std::string& title, const GVM::GVMRatioSet& scRatio, int pos, int low, bool active, bool discription, bool* checkbox)
	{
		float Text_scale = 0.35f;
		if (discription)
			Text_scale -= 0.001f;
		int Font = 0;
		int Screen_w, Screen_h;
		GRAPHICS::GET_SCREEN_RESOLUTION(&Screen_w, &Screen_h);

		std::string Caption = title;
		int CapLeg = (int)Caption.length();
		if (CapLeg > 25 && !discription)
			Caption = title.substr(CapLeg - 25, CapLeg);

		int AlfaRedux = (pos - low);
		if (AlfaRedux > 6)
			AlfaRedux -= 6;
		AlfaRedux *= 10;

		float BoxTop = scRatio.boxTop + (pos - low) * scRatio.lineSpace;
		if (discription)
			BoxTop += 3.0f;//

		float TextPosition = scRatio.textPosition + scRatio.boxPosition;

		float BoxWidthScaled = scRatio.boxWidth / (float)Screen_w; // line width
		float BoxTopScaled = BoxTop / (float)Screen_h; // line top offset
		float LineHeightScaled = scRatio.lineHeight / (float)Screen_h; // line height
		float BoxPositionScaled = scRatio.boxPosition / (float)Screen_w;
		float TextPositionScaled = TextPosition / (float)Screen_w; // text left offset

		float WapStart = scRatio.wapStart / (float)Screen_w; // wap offset
		float WapEnd = scRatio.wapEnd / (float)Screen_w; // wap offset

		if (active)
		{
			int Num25 = AddScreenText(Font, Caption, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.textActive_RGBA, 1);
			Draw_rect(BoxPositionScaled, BoxTopScaled + (0.00278f), BoxWidthScaled, ((((float)(Num25)*UI::_0xDB88A37483346780(Text_scale, 0)) + (LineHeightScaled * 2.0f)) + 0.005f), scRatio.boxActive_RGBA.R, scRatio.boxActive_RGBA.G, scRatio.boxActive_RGBA.B, scRatio.boxActive_RGBA.A - AlfaRedux);
		}
		else
		{
			int Num25 = AddScreenText(Font, Caption, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.text_RGBA, 1);
			Draw_rect(BoxPositionScaled, BoxTopScaled + (0.00278f), BoxWidthScaled, ((((float)(Num25)*UI::_0xDB88A37483346780(Text_scale, 0)) + (LineHeightScaled * 2.0f)) + 0.005f), scRatio.box_RGBA.R, scRatio.box_RGBA.G, scRatio.box_RGBA.B, scRatio.box_RGBA.A - AlfaRedux);
		}

		if (checkbox != nullptr)
		{
			std::string YourBox = "shop_box_tickb";
			if (*checkbox)
			{
				if (!active)
					YourBox = "shop_box_tick";
			}
			else
			{
				if (!active)
					YourBox = "shop_box_blank";
				else
					YourBox = "shop_box_blankb";
			}
			AddSprite("commonmenu", YourBox, scRatio, false, false, pos, low, active, 0.0f);
		}

	}
	void Draw_menu_line(const std::string& title, const GVM::GVMRatioSet& scRatio, int pos, int low, bool active, bool discription)
	{
		Draw_menu_line(title, scRatio, pos, low, active, discription, nullptr);
	}
	void Draw_menu_line_Select(const std::string& title, const std::string& description, const GVM::GVMRatioSet& scRatio, int pos, int low, bool active)
	{
		float Text_scale = 0.35;
		int Font = 0;
		int Screen_w, Screen_h;
		GRAPHICS::GET_SCREEN_RESOLUTION(&Screen_w, &Screen_h);

		std::string Caption = title;
		int CapLeg = (int)Caption.length();
		if (CapLeg > 25)
			Caption = title.substr(CapLeg - 25, CapLeg);

		std::string Capture = description;
		int Gap = (int)Capture.length();
		if (Gap > 17)
		{
			Capture = description.substr(Gap - 17, Gap);
			Gap = 17;
		}
		else
			Gap++;

		int AlfaRedux = (pos - low);
		if (AlfaRedux > 6)
			AlfaRedux -= 6;
		AlfaRedux *= 10;

		float BoxTop = scRatio.boxTop + (pos - low) * scRatio.lineSpace;
		float TextPosition = scRatio.textPosition + scRatio.boxPosition;

		float BoxWidthScaled = scRatio.boxWidth / (float)Screen_w; // line width
		float BoxTopScaled = BoxTop / (float)Screen_h; // line top offset
		float LineHeightScaled = scRatio.lineHeight / (float)Screen_h; // line height
		float BoxPositionScaled = scRatio.boxPosition / (float)Screen_w;
		float TextPositionScaled = TextPosition / (float)Screen_w; // text left offset
		float TextPositionScaled2 = scRatio.arrowNumberStart / (float)Screen_w; // text left offset

		float WapStart = scRatio.wapStart / (float)Screen_w; // wap offset
		float WapEnd = scRatio.wapEnd / (float)Screen_w; // wap offset
		
		if (active)
		{
			int Num25 = AddScreenText(Font, Caption, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.textActive_RGBA, 1);
			int Num26 = AddScreenText(Font, Capture, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled2, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.textActive_RGBA, 2);
			Draw_rect(BoxPositionScaled, BoxTopScaled + (0.00278f), BoxWidthScaled, ((((float)(Num25)*UI::_0xDB88A37483346780(Text_scale, 0)) + (LineHeightScaled * 2.0f)) + 0.005f), scRatio.boxActive_RGBA.R, scRatio.boxActive_RGBA.G, scRatio.boxActive_RGBA.B, scRatio.boxActive_RGBA.A - AlfaRedux);

			float Texrtendings = ((float)Gap * scRatio.arrowNumberGap) * 3.3f / (float)Screen_w;

			AddSprite("commonmenu", "arrowleft", scRatio, true, false, pos, low, active, Texrtendings);
			AddSprite("commonmenu", "arrowright", scRatio, false, true, pos, low, active, Texrtendings);
		}
		else
		{
			int Num25 = AddScreenText(Font, Caption, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.text_RGBA, 1);
			int Num26 = AddScreenText(Font, Capture, WapStart, WapEnd, BoxPositionScaled, TextPositionScaled2, BoxTopScaled, LineHeightScaled, Text_scale, scRatio.text_RGBA, 2);
			Draw_rect(BoxPositionScaled, BoxTopScaled + (0.00278f), BoxWidthScaled, ((((float)(Num25)*UI::_0xDB88A37483346780(Text_scale, 0)) + (LineHeightScaled * 2.0f)) + 0.005f), scRatio.box_RGBA.R, scRatio.box_RGBA.G, scRatio.box_RGBA.B, scRatio.box_RGBA.A - AlfaRedux);
		}

	}
	void Draw_menu_line_Numbers(const std::string& caption, int number, const GVM::GVMRatioSet& scRatio, int pos, int low, bool active)
	{
		Draw_menu_line_Select(caption, std::to_string(number), scRatio, pos, low, active);
	}
	void Draw_menu_line_Numbers(const std::string& caption, float number, const GVM::GVMRatioSet& scRatio, int pos, int low, bool active)
	{
		Draw_menu_line_Select(caption, FloatToString(number), scRatio, pos, low, active);
	}
	int BottomRight(const std::vector<int>& buttonList, const std::vector<std::string>& instuctionList)
	{
		std::string CharCon = "instructional_buttons";
		int iScale = GRAPHICS::REQUEST_SCALEFORM_MOVIE((LPSTR)CharCon.c_str());

		while (!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(iScale))
			WAIT(1);

		CharCon = "CLEAR_ALL";
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(iScale, (LPSTR)CharCon.c_str());
		CharCon = "TOGGLE_MOUSE_BUTTONS";
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(iScale, (LPSTR)CharCon.c_str());
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		CharCon = "CREATE_CONTAINER";
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(iScale, (LPSTR)CharCon.c_str());//Was---_CALL_SCALEFORM_MOVIE_FUNCTION_VOID

		int iAddOns = 0;

		for (int i = 0; i < (int)buttonList.size(); i++)
		{
			CharCon = "SET_DATA_SLOT";
			GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(iScale, (LPSTR)CharCon.c_str());
			GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(iAddOns);
			GRAPHICS::_0xE83A3E3557A56640(CONTROLS::_GET_CONTROL_ACTION_NAME(2, buttonList[i], 1));
			GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING((LPSTR)instuctionList[i].c_str());
			GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();
			iAddOns++;
		}
		CharCon = "DRAW_INSTRUCTIONAL_BUTTONS";
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(iScale, (LPSTR)CharCon.c_str());
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		return iScale;
	}
	void CloseBaseHelpBar(int closeSF)
	{
		int SF = closeSF;
		GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&SF);
	}

	bool CloseAll = false;
	void ShowText(float posX, float posY, float scale, const std::string& text, int font, RGBA rgba, bool outline)
	{
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(scale, scale);
		UI::SET_TEXT_COLOUR(rgba.R, rgba.G, rgba.B, rgba.A);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(0);
		if (outline)
			UI::SET_TEXT_OUTLINE();
		UI::_SET_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)text.c_str());
		UI::_DRAW_TEXT(posX, posY);
	}
	void RightSideChat(const std::string& caption, int pos)
	{
		RGBA Text_col = RGBA(255, 255, 255, 255);

		float Text_scale = 0.5f;
		int Font = 4;

		float LineTop = (float)251.0f + (float)pos * 22.5f;
		float LineHeight = 49.5f;
		float LineLeft = 1000.0f;
		float TextLeft = -10.0f;

		// correcting values for active line
		int Screen_w, Screen_h;
		GRAPHICS::GET_SCREEN_RESOLUTION(&Screen_w, &Screen_h);

		TextLeft += LineLeft;

		float LineTopScaled = LineTop / (float)Screen_h; // line top offset
		float TextLeftScaled = TextLeft / (float)Screen_w; // text left offset
		float LineHeightScaled = LineHeight / (float)Screen_h; // line height

		float LineLeftScaled = LineLeft / (float)Screen_w;

		// this is how it's done in original scripts

		// text upper part
		int num25 = AddScreenTextCenter(Font, caption, TextLeftScaled, LineTopScaled, LineHeightScaled, Text_scale, Text_col);
	}
	void BigMessage(const std::string& message, const std::string& message2, int colour)
	{
		int ScaleF = GRAPHICS::REQUEST_SCALEFORM_MOVIE("MIDSIZED_MESSAGE");

		WAIT(1500);
		while (!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(ScaleF))
			WAIT(1);

		GRAPHICS::_START_SCREEN_EFFECT("SuccessNeutral", 8500, false);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(ScaleF, "SHOW_SHARD_MIDSIZED_MESSAGE");
		GRAPHICS::_BEGIN_TEXT_COMPONENT("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)message.c_str());
		GRAPHICS::_END_TEXT_COMPONENT();
		GRAPHICS::_BEGIN_TEXT_COMPONENT("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)message2.c_str());
		GRAPHICS::_END_TEXT_COMPONENT();
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(colour);// color 0,1=white 2=black 3=grey 6,7,8=red 9,10,11=blue 12,13,14=yellow 15,16,17=orange 18,19,20=green 21,22,23=purple 
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		DWORD waitTime = 8000;
		DWORD maxTickCount = GetTickCount() + waitTime;

		while (GetTickCount() < maxTickCount)
		{
			GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(ScaleF, 255, 255, 255, 255, 2);
			WAIT(1);
		}

		GRAPHICS::SET_SCALEFORM_MOVIE_AS_NO_LONGER_NEEDED(&ScaleF);
	}
	void TopLeft(const std::string& text)
	{
		UI::_SET_TEXT_COMPONENT_FORMAT("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)text.c_str());
		UI::_DISPLAY_HELP_TEXT_FROM_STRING_LABEL(0, 0, 1, 5000);
	}
	int BottomLeft(const std::string& text)
	{
		UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)text.c_str());
		return UI::_DRAW_NOTIFICATION(1, 1);
	}
	int BottomLeftIcon(const std::string& textMain, const std::string& textSub, const std::string& subject, const std::string& badge, const std::string& clanTag)
	{
		/*
			char* picName1, char* picName2, BOOL flash, int iconType1, char* sender, char* subject, float duration, char* clanTag, int iconType2, int p9
			badge = "CHAR_SOCIAL_CLUB";
			clanTag = "__EXAMPLE"
		*/
		UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)textMain.c_str());
		UI::_SET_NOTIFICATION_MESSAGE_CLAN_TAG_2((LPSTR)badge.c_str(), (LPSTR)badge.c_str(), 0, 7, (LPSTR)textSub.c_str(), (LPSTR)subject.c_str(), 3.0, (LPSTR)clanTag.c_str(), 7, 0);
		return UI::_DRAW_NOTIFICATION(1, 1);
	}

	std::string CaptureScreenText(const std::string& text)
	{
		std::string Output = "";
		GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(0, "FMMC_KEY_TIP8", "", "", "", "", "", 64);
		while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0)
		{
			TopLeft(text);
			WAIT(1);
		}
		if (GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())
		{
			const char* text = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
			Output = text;
		}
		return Output;
	}
	
	int Convert_to_int(const std::string& line)
	{
		int Num = 0;
		int Times = 0;
		bool Negative = false;
		for (int i = (int)line.size(); i > -1; i--)
		{
			bool Skip = false;
			int Add = 0;
			char Comp = line[i];

			if (Comp == char(45))
				Negative = true;
			else if (Comp == char(49))
			{
				Negative = false;
				Add = 1;
			}
			else if (Comp == char(50))
			{
				Negative = false;
				Add = 2;
			}
			else if (Comp == char(51))
			{
				Negative = false;
				Add = 3;
			}
			else if (Comp == char(52))
			{
				Negative = false;
				Add = 4;
			}
			else if (Comp == char(53))
			{
				Negative = false;
				Add = 5;
			}
			else if (Comp == char(54))
			{
				Negative = false;
				Add = 6;
			}
			else if (Comp == char(55))
			{
				Negative = false;
				Add = 7;
			}
			else if (Comp == char(56))
			{
				Negative = false;
				Add = 8;
			}
			else if (Comp == char(57))
			{
				Negative = false;
				Add = 9;
			}
			else if (Comp == char(48))
			{

			}
			else
				Skip = true;

			if (!Skip)
			{
				if (Times == 0)
				{
					Num = Add;
					Times = 1;
				}
				else
					Num += Add * Times;
				Times *= 10;
			}
		}

		if (Negative)
			Num = Num * (-1);

		return Num;
	}
	float Convert_to_float(const std::string& line)
	{
		float Num = 0;
		std::string DecOp = ".";
		int EndIndex = -1; // Index to print until (exclusive)
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == DecOp[0])
			{
				EndIndex = i;
				break;
			}
		}

		if (EndIndex >= 0 && EndIndex <= line.length())
		{
			std::string AfterDec = line.substr(EndIndex + 1);
			std::string BigNum = line.substr(0, EndIndex) + AfterDec;
			Num = (float)Convert_to_int(BigNum);

			for (int i = 0; i < AfterDec.length(); i++)
				Num = Num / 10;
		}
		else
			Num = (float)Convert_to_int(line);

		return Num;
	}
	int Screen_Test_To_Int(int min, int max)
	{
		std::string MyNum = CaptureScreenText("Enter a whole number");
		int iOut = Convert_to_int(MyNum);
		if (iOut < min || iOut > max)
			iOut = min;
		return iOut;
	}
	float Screen_Test_To_Float(float min, float max)
	{
		std::string MyNum = CaptureScreenText("Enter a decimal number");
		float fOut = Convert_to_float(MyNum);
		if (fOut < min || fOut > max)
			fOut = min;
		return fOut;
	}
	void Menu_beep()
	{
		AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	}
	void ButtonDisabler()
	{
		UI::HIDE_HELP_TEXT_THIS_FRAME();
		CAM::SET_CINEMATIC_BUTTON_ACTIVE(1);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_NEXT_CAMERA, true);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_X);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_ACCEPT);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_CANCEL);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_HUD_SPECIAL, true);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_DOWN);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_UP);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_ACCEPT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_CANCEL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_LEFT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RIGHT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_DOWN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_UP, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_RDOWN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_FRONTEND_ACCEPT, true);
		UI::HIDE_HUD_COMPONENT_THIS_FRAME(10);
		UI::HIDE_HUD_COMPONENT_THIS_FRAME(6);
		UI::HIDE_HUD_COMPONENT_THIS_FRAME(7);
		UI::HIDE_HUD_COMPONENT_THIS_FRAME(9);
		UI::HIDE_HUD_COMPONENT_THIS_FRAME(8);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_LEFT);
		CONTROLS::SET_INPUT_EXCLUSIVE(2, INPUT_FRONTEND_RIGHT);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_UNARMED, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_MELEE, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_HANDGUN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_SHOTGUN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_SMG, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_AUTO_RIFLE, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_SNIPER, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_HEAVY, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_WEAPON_SPECIAL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_WHEEL_NEXT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_WHEEL_PREV, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_SPECIAL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_WEAPON_SPECIAL_TWO, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_DIVE, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_LIGHT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_ATTACK_HEAVY, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_MELEE_BLOCK, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_ARREST, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_HEADLIGHT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_RADIO_WHEEL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_CONTEXT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_RELOAD, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_CIN_CAM, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_JUMP, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_SELECT_NEXT_WEAPON, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_FLY_SELECT_NEXT_WEAPON, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_FRANKLIN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MICHAEL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_TREVOR, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SELECT_CHARACTER_MULTIPLAYER, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_CHARACTER_WHEEL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_CANCEL, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_SELECT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_UP, true);
		CONTROLS::DISABLE_CONTROL_ACTION(2, INPUT_CELLPHONE_DOWN, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_DETONATE, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_SPRINT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_DUCK, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_HEADLIGHT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_PUSHBIKE_SPRINT, true);
		CONTROLS::DISABLE_CONTROL_ACTION(0, INPUT_VEH_PUSHBIKE_PEDAL, true);
	}

	bool WhileButtonDown(int button)
	{
		bool ButtonDown = (bool)CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, button);

		if (ButtonDown)
		{
			while (!(bool)CONTROLS::IS_DISABLED_CONTROL_JUST_RELEASED(0, button))
				WAIT(1);
		}
		return ButtonDown;
	}
	bool ButtonDown(int button)
	{
		return (bool)CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, button);
	}
	void get_button_state(bool* ent, bool* del, bool* up, bool* down, bool* left, bool* right, bool* shutDown)
	{
		if (ent) *ent = IsKeyDown(VK_RETURN);
		if (del) *del = IsKeyDown(VK_DELETE) || IsKeyDown(VK_BACK);
		if (up) *up = IsKeyDown(VK_UP);
		if (down) *down = IsKeyDown(VK_DOWN);
		if (left) *left = IsKeyDown(VK_LEFT);
		if (right) *right = IsKeyDown(VK_RIGHT);
		if (shutDown) *shutDown = IsKeyDown(VK_TAB);
	}

	void MeunDescrition(GVMSystem* myMenu, const std::string& descrip)
	{
		if (myMenu != nullptr)
		{
			int Start = 7;
			if (!myMenu->Scroling)
				Start = myMenu->High;
			Start *= -1;
				Draw_menu_line(descrip, DefaultRatio, 0, Start, false, true);
		}
	}
	void MenuDisplay(GVMSystem* myMenu, GVMRatioSet& scRatio)
	{
		if (myMenu != nullptr)
		{
			if ((int)myMenu->Menu_Form.size() > 0)
			{
				DWORD waitTime = 350;
				DWORD holdButton = 0;
				std::vector<std::string> InstText = { "Enter", "Back", "Left", "Right" };
				std::vector<int> InstBut = { 21, 45, 189, 190 };
				myMenu->InstructSF = BottomRight(InstBut, InstText);

				if ((int)myMenu->Menu_Form.size() < 7)
					myMenu->High = (int)myMenu->Menu_Form.size();
				else
					myMenu->Scroling = true;

				bool Up, Down, Left, Right, Ratio;

				while (true)
				{
					DWORD maxTickCount = GetTickCount() + waitTime;
					do
					{
						ButtonDisabler();
						GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(myMenu->InstructSF, 255, 255, 255, 255, 0);
						if (myMenu->BannerListId != -1)
						{
							float ScreenHeightScaleFactor = GRAPHICS::_GET_SCREEN_ASPECT_RATIO(true);
							drawTexture(myMenu->BannerListId, 0, 10, 100, scRatio.PngBanner_sizeX, scRatio.PngBanner_sizeY, scRatio.PngBanner_centerX, scRatio.PngBanner_centerY, scRatio.PngBanner_posX, scRatio.PngBanner_posY, scRatio.PngBanner_rotation, ScreenHeightScaleFactor, scRatio.PngBanner_RGBA.R, scRatio.PngBanner_RGBA.G, scRatio.PngBanner_RGBA.B, scRatio.PngBanner_RGBA.A);
						}
						else
							AddBanner(myMenu->Banner_Title, myMenu->DefaultBgLib, myMenu->DefaultBg, scRatio);

						for (int i = myMenu->Low; i < myMenu->High; i++)
						{
							bool bIndex = false;
							if (i == myMenu->Index)
								bIndex = true;

							if (myMenu->Menu_Form[i].TickBox != nullptr)
								Draw_menu_line(myMenu->Menu_Form[i].Title, scRatio, i, myMenu->Low, bIndex, false, myMenu->Menu_Form[i].TickBox);
							else if (myMenu->Menu_Form[i].iTemsList.size() > 0)
								Draw_menu_line_Select(myMenu->Menu_Form[i].Title, myMenu->Menu_Form[i].iTemsList[*myMenu->Menu_Form[i].IntNum], scRatio, i, myMenu->Low, bIndex);
							else if (myMenu->Menu_Form[i].IntNum != nullptr)
							{
								if (myMenu->Menu_Form[i].IntSwitch)
								{
									bool intBool = (bool)*myMenu->Menu_Form[i].IntNum;
									bool* BoolPoint = &intBool;
									Draw_menu_line(myMenu->Menu_Form[i].Title, scRatio, i, myMenu->Low, bIndex, false, BoolPoint);
								}
								else
									Draw_menu_line_Numbers(myMenu->Menu_Form[i].Title, *myMenu->Menu_Form[i].IntNum, scRatio, i, myMenu->Low, bIndex);
							}
							else if (myMenu->Menu_Form[i].FloatNum != nullptr)
								Draw_menu_line_Numbers(myMenu->Menu_Form[i].Title, *myMenu->Menu_Form[i].FloatNum, scRatio, i, myMenu->Low, bIndex);
							else
								Draw_menu_line(myMenu->Menu_Form[i].Title, scRatio, i, myMenu->Low, bIndex, false);
						}
						
						if (myMenu->Menu_Form[myMenu->Index].Description != "")
							MeunDescrition(myMenu, myMenu->Menu_Form[myMenu->Index].Description);
						WAIT(0);
					} while (GetTickCount() < maxTickCount);
					waitTime = 0;
					get_button_state(&myMenu->_Activate, &myMenu->_Back, &Up, &Down, &Left, &Right, &Ratio);

					if (myMenu->_Back || ButtonDown(199) || ButtonDown(45))
					{
						myMenu->_Back = true;
						Menu_beep();
						break;
					}
					else if (myMenu->_Activate || ButtonDown(21))
					{
						myMenu->_Activate = true;
						Menu_beep();
						if (myMenu->Menu_Form[myMenu->Index].ActiveFunction && !myMenu->Menu_Form[myMenu->Index].ActivateOnChange)
							break;
						else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr && myMenu->Menu_Form[myMenu->Index].EditNumber)
							*myMenu->Menu_Form[myMenu->Index].IntNum = Screen_Test_To_Int(myMenu->Menu_Form[myMenu->Index].Min, myMenu->Menu_Form[myMenu->Index].Max);
						else if (myMenu->Menu_Form[myMenu->Index].FloatNum != nullptr && myMenu->Menu_Form[myMenu->Index].EditNumber)
							*myMenu->Menu_Form[myMenu->Index].FloatNum = Screen_Test_To_Float((float)myMenu->Menu_Form[myMenu->Index].Min, (float)myMenu->Menu_Form[myMenu->Index].Max);

						waitTime = 150;
					}
					else
					{
						if (Up || ButtonDown(27))
						{
							if (myMenu->Scroling)
							{
								if (myMenu->Low > 0)
								{
									myMenu->Low--;
									myMenu->High--;
								}

								if (myMenu->Index == 0)
								{
									myMenu->Index = (int)myMenu->Menu_Form.size();
									if (myMenu->Index > 6)
									{
										myMenu->High = myMenu->Index;
										myMenu->Low = myMenu->High - 7;
									}
								}
								myMenu->Index--;
							}
							else
							{
								if (myMenu->Index == 0)
									myMenu->Index = (int)myMenu->Menu_Form.size();

								myMenu->Index--;
							}
							waitTime = 150;
						}
						else if (Down || ButtonDown(19))
						{
							myMenu->Index++;
							if (myMenu->Scroling)
							{
								if (myMenu->Index == (int)myMenu->Menu_Form.size())
								{
									myMenu->Index = 0;
									myMenu->High = 7;
									myMenu->Low = 0;
								}
								else if (myMenu->Index == myMenu->High && myMenu->High < (int)myMenu->Menu_Form.size())
								{
									myMenu->Low++;
									myMenu->High++;
								}
							}
							else
							{
								if (myMenu->Index == (int)myMenu->Menu_Form.size())
									myMenu->Index = 0;
							}
							waitTime = 150;
						}
						else if (Left || ButtonDown(47))
						{
							if (holdButton < 145)
								holdButton++;

							waitTime = 150 - holdButton;
							bool ItemActive = true;

							if (myMenu->Menu_Form[myMenu->Index].TickBox != nullptr)
							{
								*myMenu->Menu_Form[myMenu->Index].TickBox = !*myMenu->Menu_Form[myMenu->Index].TickBox;
								ItemActive = *myMenu->Menu_Form[myMenu->Index].TickBox;
								waitTime = 150;
							}
							else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr)
							{
								if (*myMenu->Menu_Form[myMenu->Index].IntNum == myMenu->Menu_Form[myMenu->Index].Min)
									*myMenu->Menu_Form[myMenu->Index].IntNum = myMenu->Menu_Form[myMenu->Index].Max;
								else
									*myMenu->Menu_Form[myMenu->Index].IntNum -= 1;

								if (myMenu->Menu_Form[myMenu->Index].IntSwitch)
								{
									if (*myMenu->Menu_Form[myMenu->Index].IntNum == 0)
										ItemActive = false;
								}
							}
							else if (myMenu->Menu_Form[myMenu->Index].FloatNum != nullptr)
							{
								if (*myMenu->Menu_Form[myMenu->Index].FloatNum <= (float)myMenu->Menu_Form[myMenu->Index].Min)
									*myMenu->Menu_Form[myMenu->Index].FloatNum = (float)myMenu->Menu_Form[myMenu->Index].Max;
								else
									*myMenu->Menu_Form[myMenu->Index].FloatNum -= myMenu->Menu_Form[myMenu->Index].F_increment;
							}

							if (myMenu->Menu_Form[myMenu->Index].ActivateOnChange)
							{
								if (myMenu->Menu_Form[myMenu->Index].MenuFun != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].MenuFunAny != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].MenuFunAnyAny != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].UseIndex)
									myMenu->Menu_Form[myMenu->Index].CallFunction(myMenu->Index);
								else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction(*myMenu->Menu_Form[myMenu->Index].IntNum);
							}
						}
						else if (Right || ButtonDown(46))
						{
							if (holdButton < 145)
								holdButton++;

							waitTime = 150 - holdButton;
							bool ItemActive = true;

							if (myMenu->Menu_Form[myMenu->Index].TickBox != nullptr)
							{
								*myMenu->Menu_Form[myMenu->Index].TickBox = !*myMenu->Menu_Form[myMenu->Index].TickBox;
								ItemActive = *myMenu->Menu_Form[myMenu->Index].TickBox;
								waitTime = 150;
							}
							else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr)
							{
								if (*myMenu->Menu_Form[myMenu->Index].IntNum == myMenu->Menu_Form[myMenu->Index].Max)
									*myMenu->Menu_Form[myMenu->Index].IntNum = myMenu->Menu_Form[myMenu->Index].Min;
								else
									*myMenu->Menu_Form[myMenu->Index].IntNum += 1;

								if (myMenu->Menu_Form[myMenu->Index].IntSwitch)
								{
									if (*myMenu->Menu_Form[myMenu->Index].IntNum == 0)
										ItemActive = false;
								}
							}
							else if (myMenu->Menu_Form[myMenu->Index].FloatNum != nullptr)
							{
								if (*myMenu->Menu_Form[myMenu->Index].FloatNum >= (float)myMenu->Menu_Form[myMenu->Index].Max)
									*myMenu->Menu_Form[myMenu->Index].FloatNum = (float)myMenu->Menu_Form[myMenu->Index].Min;
								else
									*myMenu->Menu_Form[myMenu->Index].FloatNum += myMenu->Menu_Form[myMenu->Index].F_increment;
							}

							if (myMenu->Menu_Form[myMenu->Index].ActivateOnChange)
							{
								if (myMenu->Menu_Form[myMenu->Index].MenuFun != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].MenuFunAny != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].MenuFunAnyAny != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction();
								else if (myMenu->Menu_Form[myMenu->Index].UseIndex)
									myMenu->Menu_Form[myMenu->Index].CallFunction(myMenu->Index);
								else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr)
									myMenu->Menu_Form[myMenu->Index].CallFunction(*myMenu->Menu_Form[myMenu->Index].IntNum);
							}
						}
						else
							holdButton = 0;
					}
				}
				myMenu->MenuActive = false;
				CloseBaseHelpBar(myMenu->InstructSF);
				if (!myMenu->Menu_Form[myMenu->Index].ActivateOnChange)
				{
					if (myMenu->_Activate)
					{
						if (myMenu->Menu_Form[myMenu->Index].MenuFun != nullptr)
							myMenu->Menu_Form[myMenu->Index].CallFunction();
						else if (myMenu->Menu_Form[myMenu->Index].MenuFunAny != nullptr)
							myMenu->Menu_Form[myMenu->Index].CallFunction();
						else if (myMenu->Menu_Form[myMenu->Index].MenuFunAnyAny != nullptr)
							myMenu->Menu_Form[myMenu->Index].CallFunction();
						else if (myMenu->Menu_Form[myMenu->Index].UseIndex)
							myMenu->Menu_Form[myMenu->Index].CallFunction(myMenu->Index);
						else if (myMenu->Menu_Form[myMenu->Index].IntNum != nullptr)
							myMenu->Menu_Form[myMenu->Index].CallFunction(*myMenu->Menu_Form[myMenu->Index].IntNum);
					}
				}
				if (myMenu->ExitFunction != nullptr || myMenu->ExitFunctionAny != nullptr || myMenu->ExitFunctionAnyAny != nullptr)
				{
					if (myMenu->ActivateOnBack && myMenu->_Back)
						myMenu->CallFunction();
					else if (!myMenu->ActivateOnBack)
						myMenu->CallFunction();
				}
			}
			else
				myMenu->_Back = true;
		}
	}
}