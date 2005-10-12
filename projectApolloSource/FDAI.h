/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Rob Conley, Markus Joachim

  This class is used to display a FDAI on the panel.

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************** Revision History ****************************
  *	$Log$
  *	Revision 1.2  2005/08/20 17:50:41  movieman523
  *	Added FDAI state save and load.
  *	
  *	Revision 1.1  2005/08/19 13:34:18  tschachim
  *	Initial version
  *	
  **************************************************************************/

#include < GL\gl.h >                                
#include < GL\glu.h >

class FDAI {

public:
	FDAI();
	virtual ~FDAI();
	void RegisterMe(int index, int x, int y);
	void PaintMe(VECTOR3 attitude, SURFHANDLE surf, SURFHANDLE hFDAI, SURFHANDLE hFDAIRoll, HBITMAP hBmpRoll);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

protected:
	int ScrX;
	int ScrY;			//coords on screen
	int idx;			//index on the panel list 
	int init;
	int list_name; //we store the rendering into a display list
	VECTOR3 now, target;
	double lastPaintTime, over_rate;

	//some stuff for OpenGL
	HDC hDC2;
	HGLRC hRC;
	HBITMAP hBMP;
	HBITMAP hBMP_old;
	GLUquadricObj *quadObj; 

	void InitGL();
	void MoveBall();
	void SetAttitude(VECTOR3 attitude);
	int LoadOGLBitmap(char *filename);
};

//
// Strings for state saving.
//

#define FDAI_START_STRING	"FDAI_BEGIN"
#define FDAI_END_STRING		"FDAI_END"

#define FDAI2_START_STRING	"FDAI2_BEGIN"
#define FDAI2_END_STRING	"FDAI2_END"
