//
//  ofxFontStash2.h
//  ofxFontStash2
//
//  Created by Oriol Ferrer Mesià on 12/7/15.
//
//

#pragma once
#include "ofMain.h"

#include "ofxFontStashParser.h"
#include "ofxFontStashStyle.h"

#include "ofxfs2_fontstash.h"
#include "ofxfs2_nanovg.h"

class ofxFontStash2{

public:


	ofxFontStash2();
	~ofxFontStash2();

	void setup(bool debug = false);

	/// load fonts
	bool addFont(const string& fontID, const string& fontFile); //returns fontID
	bool isFontLoaded(const string& fontID); //checks if a font was already loaded

	/// work with font styles
	void addStyle(const string& styleID, ofxFontStashStyle style);
	bool styleExists(const string& styleID);
	ofxFontStashStyle getStyle(const string& styleID, bool & exists);
	map<string, ofxFontStashStyle> getStyles(){ return styleIDs; }

	/// draw single line string
	/// returns text width
	/// multiline ("\n") not supported - to use for one-liners
	float draw(const string& text, const ofxFontStashStyle& style, float x, float y);

	/// draw xml formatted string
	/// return text width
	/// multiline ("\n") not supported - to use for one-liners with style
	float drawFormatted(const string& styledText, float x, float y);

	/// draw string with fixed maximum width, breaking lines to fit the width
	/// returns text bbox
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawColumn(const string& text, const ofxFontStashStyle& style, float x, float y, float width, ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT, bool debug=false);

	/// draw xml formatted string with fixed maximum width, breaking lines to fit the width
	/// returns text bbox
	/// multiline ("\n") supported - and it will break lines on its own given a column width
	ofRectangle drawFormattedColumn(const string& styledText, float x, float y, float width, ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT, bool debug=false);


	// DIY layout + draw - so you can inspect intermediate states and do as you wish //
	/// 1 - Parse your text to get the vector<StyledText> blocks.
	/// 2 - Layout the lines to a certain column width; you get a vector of styled words (vectos<StyledText>).
	///	3 - Draw the layout; its a vector of Lines so you can dig and edit before you draw.

	/*
		// Styled Text //
		vector<StyledText> parsed = fs.parseStyledText(myStyledText);
		vector<StyledLine> laidOutLines = fs.layoutLines(parsed, columnWidth);
		ofRectangle bbox = fs.drawLines(laidOutLines, x, y);
		
		// Plain Text //
		ofxFontStashStyle style;
		string text = "my text";
		float columnWidth;
		vector<StyledLine> laidOutLines = fonts.layoutLines({{text, style}}, columnWidth);
		ofRectangle bbox = fonts.drawLines(laidOutLines, xx, yy);

	 */

	///draw a paragraph relying on NanoVG for layout & linebreaking
	void drawColumnNVG(const string& text, const ofxFontStashStyle& style,
					   float x, float y, float width,
					   ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT);

	///get the bbox containing the text laid out in a drawColumnNVG() call
	ofRectangle getTextBoundsNVG(const string& text,
								 const ofxFontStashStyle& style,
								 float x, float y, float width,
								 ofAlignHorz horAlign);


	// Utils below this line ///////////////////////////////////////////////////////////////////////////////

	/// split up styledText into a vector of blocks with the same style
	vector<StyledText> parseStyledText(const string & styledText);

	/// layout StyledText blocks
	const vector<StyledLine> layoutLines(const vector<StyledText> &blocks,
										 float targetWidth,
										 ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT,
										 int limitToNLines = 0, //stop layout if lines are > limitToNLines
										 bool debug = false);

	/// draw already prepared StyledLine´s
	ofRectangle drawLines(const vector<StyledLine> &lines,
						  float x, float y,
						  ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT,
						  bool debug = false);
	
	/// draw and layout blocks
	ofRectangle drawAndLayout(vector<StyledText> &blocks,
							  float x, float y,
							  float width,
							  ofAlignHorz horAlign = OF_ALIGN_HORZ_LEFT,
							  bool debug=false);

	/// only applies to draw(); return the bbox of the text
	ofRectangle getTextBounds( const string &text, const ofxFontStashStyle &style, const float x, const float y );
	ofRectangle getTextBounds(const vector<StyledLine> &lines, float x, float y);

	///get font metrics for a particular style
	void getVerticalMetrics( const ofxFontStashStyle& style, float* ascender, float* descender, float* lineH);

	/// a global lineH multiplier that affects all loaded fonts.
	void setLineHeightMult(float l){lineHeightMultiplier = l;}

	///in case you want to issue nanoVG calls directly
	NVGcontext * getNanoVGContext(){return ctx;}

	/// sets a single font as fallback for all fonts
	/// this can only be set once - useful for emoji
	void setGlobalFallbackFont(const string& fallbackFontID);
	string getGlobalFallbackFont();
	
	/// add fallback variant to a font
	/// removing/listing the fallbacks is currently not possible.
	void addFallbackFont(const string& fontID, const string &fallbackFontID);
	
	/// allows for higher pixel densities.
	/// this will increase texture resolution during drawing,
	/// but will leave all sizes exactly the same
	/// this can be changed between calls, meaning it's read (and applied) before each draw call
	float pixelDensity;
	
	/// width of tabs (measured in spaces)
	int tabWidth = 4;
	
	/// with this option one can scale all fonts up/down by a factor
	float fontScale;
	
protected:

	float lineHeightMultiplier;

	string toString(TextBlockType t){
		switch(t){
			case WORD: return "WORD";
			case SEPARATOR: return "SEPARATOR";
			case SEPARATOR_INVISIBLE: return "SEPARATOR_INVISIBLE";
			default: return "UNKNOWN";
		}
	}

	int getFsID(const string& userFontID);

	float calcWidth(const StyledLine & line);
	float calcLineHeight(const StyledLine & line);
	
	bool applyStyle(const ofxFontStashStyle& style);

	NVGcolor toFScolor(const ofColor& c);
	NVGcontext* ctx = nullptr;

	map<string, int> fontIDs; //userFontID to fontStashFontID

	map<string, ofxFontStashStyle> styleIDs;
	
	string globalFallbackFontID; // id of the fallback font for all loaded fonts

	vector<TextBlock> splitWords( const vector<StyledText> & blocks);

	ofShader nullShader;
	bool drawingFS;

	void applyOFMatrix();
	void begin();
	void end();
};
