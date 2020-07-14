//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HUD_BASECHAT_H
#define HUD_BASECHAT_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include <vgui_controls/panel.h>
#include "vgui_basepanel.h"
#include "vgui_controls/frame.h"
#include <vgui_controls/textentry.h>
#include <vgui_controls/richtext.h>
#include <vgui_controls/button.h>
#include <vgui_controls/checkbutton.h>

class CBaseHudChatInputLine;
class CBaseHudChatEntry;
class CHudChatFilterPanel;

namespace vgui
{
	class IScheme;
};

#define CHATLINE_NUM_FLASHES 8.0f
#define CHATLINE_FLASH_TIME 5.0f
#define CHATLINE_FADE_TIME 1.0f

#define CHAT_HISTORY_FADE_TIME 0.25f
#define CHAT_HISTORY_IDLE_TIME 15.0f
#define CHAT_HISTORY_IDLE_FADE_TIME 2.5f
#define CHAT_HISTORY_ALPHA 127

extern Color g_ColorBlue;
extern Color g_ColorRed;
extern Color g_ColorGreen;
extern Color g_ColorDarkGreen;
extern Color g_ColorYellow;
extern Color g_ColorGrey;

extern ConVar cl_showtextmsg;

enum ChatFilters
{
	CHAT_FILTER_NONE = 0,
	CHAT_FILTER_JOINLEAVE = 1,
	CHAT_FILTER_NAMECHANGE = 2,
	CHAT_FILTER_PUBLICCHAT = 4,
	CHAT_FILTER_SERVERMSG = 8,
	CHAT_FILTER_TEAMCHANGE = 16
};


//-----------------------------------------------------------------------------
enum TextColor
{
	COLOR_NORMAL = 1,
	COLOR_USEOLDCOLORS = 2,
	COLOR_PLAYERNAME = 3,
	COLOR_LOCATION = 4,
	COLOR_ACHIEVEMENT = 5,
	COLOR_MAX
};

//--------------------------------------------------------------------------------------------------------------
struct TextRange
{
	int start;
	int end;
	Color color;
};

void StripEndNewlineFromString( char *str );
void StripEndNewlineFromString( wchar_t *str );

char* ConvertCRtoNL( char *str );
wchar_t* ConvertCRtoNL( wchar_t *str );
wchar_t* ReadLocalizedString( bf_read &msg, wchar_t *pOut, int outSize, bool bStripNewline, char *originalString = NULL, int originalSize = 0 );
wchar_t* ReadChatTextString( bf_read &msg, wchar_t *pOut, int outSize );
char* RemoveColorMarkup( char *str );

//-----------------------------------------------------------------------------
// Purpose: An output/display line of the chat interface
//-----------------------------------------------------------------------------
class CBaseHudChatLine : public vgui::RichText
{
	typedef vgui::RichText BaseClass;

public:
	CBaseHudChatLine( vgui::Panel *parent, const char *panelName );
	~CBaseHudChatLine();

	void			SetExpireTime( void );

	bool			IsReadyToExpire( void );

	void			Expire( void );

	float			GetStartTime( void );

	int				GetCount( void );

	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);

	vgui::HFont		GetFont() { return m_hFont; }

	Color			GetTextColor( void ) { return m_clrText; }
	void			SetNameLength( int iLength ) { m_iNameLength = iLength;	}
	void			SetNameColor( Color cColor ){ m_clrNameColor = cColor; 	}
		
	virtual void	PerformFadeout( void );
	virtual void	InsertAndColorizeText( wchar_t *buf, int clientIndex );
	virtual			void Colorize( int alpha = 255 );								///< Re-inserts the text in the appropriate colors at the given alpha


	void			SetNameStart( int iStart ) { m_iNameStart = iStart;	}

protected:
	int				m_iNameLength;
	vgui::HFont		m_hFont;

	Color			m_clrText;
	Color			m_clrNameColor;

	float			m_flExpireTime;

	CUtlVector< TextRange > m_textRanges;
	wchar_t					*m_text;

	int				m_iNameStart;
	
private:
	float			m_flStartTime;
	int				m_nCount;

	vgui::HFont		m_hFontMarlett;


private:
	CBaseHudChatLine( const CBaseHudChatLine & ); // not defined, not accessible
};


class CHudChatHistory : public vgui::RichText
{
	DECLARE_CLASS_SIMPLE( CHudChatHistory, vgui::RichText );
public:

	CHudChatHistory( vgui::Panel *pParent, const char *panelName );
	virtual ~CHudChatHistory() {}

	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);
};

class CHudChatFilterButton : public vgui::Button
{
	DECLARE_CLASS_SIMPLE( CHudChatFilterButton, vgui::Button );

public:

	CHudChatFilterButton(  vgui::Panel *pParent, const char *pName, const char *pText );

	virtual void DoClick( void );
};

class CHudChatFilterCheckButton : public vgui::CheckButton
{
	DECLARE_CLASS_SIMPLE( CHudChatFilterCheckButton, vgui::CheckButton );

public:

	CHudChatFilterCheckButton( vgui::Panel *pParent, const char *pName, const char *pText, int iFlag );

	int		GetFilterFlag( void ) { return m_iFlag; }

private:

	int m_iFlag;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CBaseHudChat : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CBaseHudChat, vgui::EditablePanel );
public:
	DECLARE_MULTIPLY_INHERITED();

	enum
	{
		CHAT_INTERFACE_LINES = 6,
		MAX_CHARS_PER_LINE = 128
	};

	CBaseHudChat( const char *pElementName );

	virtual void	CreateChatInputLine( void );
	virtual void	CreateChatLines( void );
	
	virtual void	Init( void );

	void			LevelInit( const char *newmap );
	void			LevelShutdown( void );

	void			MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
	
	virtual void	Printf( int iFilter, const char *fmt, ... );
	virtual void	ChatPrintf( int iPlayerIndex, int iFilter, const char *fmt, ... );
	
	void			StartMessageMode( int iMessageModeType );
	void			StopMessageMode( void );
	void			Send( void );

	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void	Paint( void );
	virtual void	OnTick( void );
	virtual void	Reset();
#ifdef _XBOX
	virtual bool	ShouldDraw();
#endif
	vgui::Panel		*GetInputPanel( void );

	static int		m_nLineCounter;

	virtual int		GetChatInputOffset( void );

	// IGameEventListener interface:
	virtual void FireGameEvent( IGameEvent *event);

	CHudChatHistory			*GetChatHistory();

	void					FadeChatHistory();
	float					m_flHistoryFadeTime;
	float					m_flHistoryIdleTime;

	virtual void			MsgFunc_SayText( bf_read &msg );
	virtual void			MsgFunc_SayText2( bf_read &msg );
	virtual void			MsgFunc_TextMsg( bf_read &msg );
	virtual void			MsgFunc_VoiceSubtitle( bf_read &msg );

	
	CBaseHudChatInputLine	*GetChatInput( void ) { return m_pChatInput; }
	CHudChatFilterPanel		*GetChatFilterPanel( void );

	int						GetFilterFlags( void ) { return m_iFilterFlags; }
	void					SetFilterFlag( int iFilter );

	virtual Color	GetTextColorForClient( TextColor colorNum, int clientIndex );
	virtual Color	GetClientColor( int clientIndex );

	virtual int		GetFilterForString( const char *pString );

	virtual const char *GetDisplayedSubtitlePlayerName( int clientIndex );

	bool			IsVoiceSubtitle( void ) { return m_bEnteringVoice; }
	void			SetVoiceSubtitleState( bool bState ) { m_bEnteringVoice = bState; }

protected:
	CBaseHudChatLine		*FindUnusedChatLine( void );

	CBaseHudChatInputLine	*m_pChatInput;
	CBaseHudChatLine		*m_ChatLine;
	int					m_iFontHeight;

	CHudChatHistory			*m_pChatHistory;

	CHudChatFilterButton	*m_pFiltersButton;
	CHudChatFilterPanel		*m_pFilterPanel;

private:	
	void			Clear( void );

	int				ComputeBreakChar( int width, const char *text, int textlen );

	int				m_nMessageMode;

	int				m_nVisibleHeight;

	vgui::HFont		m_hChatFont;

	int				m_iFilterFlags;
	bool			m_bEnteringVoice;
};

class CBaseHudChatEntry : public vgui::TextEntry
{
	typedef vgui::TextEntry BaseClass;
public:
	CBaseHudChatEntry( vgui::Panel *parent, char const *panelName, CBaseHudChat *pChat )
		: BaseClass( parent, panelName )
	{
		SetCatchEnterKey( true );
		SetAllowNonAsciiCharacters( true );
		SetDrawLanguageIDAtLeft( true );
		m_pHudChat = pChat;
	}

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme )
	{
		BaseClass::ApplySchemeSettings(pScheme);

		SetPaintBorderEnabled( false );
	}

	virtual void OnKeyCodeTyped(vgui::KeyCode code)
	{
		if ( code == KEY_ENTER || code == KEY_PAD_ENTER || code == KEY_ESCAPE )
		{
			if ( code != KEY_ESCAPE )
			{
				if ( m_pHudChat )
				{
					m_pHudChat->Send();
				}
			}
		
			// End message mode.
			if ( m_pHudChat )
			{
				m_pHudChat->StopMessageMode();
			}
		}
		else if ( code == KEY_TAB )
		{
			// Ignore tab, otherwise vgui will screw up the focus.
			return;
		}
		else
		{
			BaseClass::OnKeyCodeTyped( code );
		}
	}

private:
	CBaseHudChat *m_pHudChat;
};

//-----------------------------------------------------------------------------
// Purpose: The prompt and text entry area for chat messages
//-----------------------------------------------------------------------------
class CBaseHudChatInputLine : public vgui::Panel
{
	typedef vgui::Panel BaseClass;
	
public:
	CBaseHudChatInputLine( CBaseHudChat *parent, char const *panelName );
	virtual ~CBaseHudChatInputLine() {}

	void			SetPrompt( const wchar_t *prompt );
	void			ClearEntry( void );
	void			SetEntry( const wchar_t *entry );
	void			GetMessageText( wchar_t *buffer, int buffersizebytes );

	virtual void	PerformLayout();
	virtual void	ApplySchemeSettings(vgui::IScheme *pScheme);

	vgui::Panel		*GetInputPanel( void );
	virtual vgui::VPANEL GetCurrentKeyFocus() { return m_pInput->GetVPanel(); } 

	virtual void Paint()
	{
		BaseClass::Paint();
	}

	vgui::Label		*GetPrompt( void ) { return m_pPrompt; }

protected:
	vgui::Label		*m_pPrompt;
	CBaseHudChatEntry	*m_pInput;
};


class CHudChatFilterPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudChatFilterPanel, vgui::EditablePanel );

public:

	CHudChatFilterPanel(  vgui::Panel *pParent, const char *pName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	MESSAGE_FUNC_PTR( OnFilterButtonChecked, "CheckButtonChecked", panel );

	CBaseHudChat *GetChatParent( void ) { return dynamic_cast < CBaseHudChat * > ( GetParent() ); }

	virtual void SetVisible(bool state);

private:

};

#endif // HUD_BASECHAT_H
