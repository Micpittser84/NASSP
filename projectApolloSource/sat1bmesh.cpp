/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn 1b mesh code

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
  *	Revision 1.75  2008/01/18 05:57:23  movieman523
  *	Moved SIVB creation code into generic Saturn function, and made ASTP sort of start to work.
  *	
  *	Revision 1.74  2008/01/17 01:46:27  movieman523
  *	Renamed LEMName to PayloadName and replaced LEMN with PAYN in the scenario file; reading LEMN is still supported for backward compatibility.
  *	
  *	Revision 1.73  2008/01/16 05:52:06  movieman523
  *	Removed all dockstate code.
  *	
  *	Revision 1.72  2008/01/14 04:31:09  movieman523
  *	Initial tidyup: ASTP should now work too.
  *	
  *	Revision 1.71  2008/01/14 01:17:06  movieman523
  *	Numerous changes to move payload creation from the CSM to SIVB.
  *	
  *	Revision 1.70  2008/01/13 08:15:51  jasonims
  *	New Saturn IB Exhaust Texture - BETA 1.0
  *	Feedback Desired.
  *	
  *	Revision 1.69  2008/01/12 04:14:10  movieman523
  *	Pass payload information to SIVB and have LEM use the fuel masses passed to it.
  *	
  *	Revision 1.68  2008/01/11 03:20:10  movieman523
  *	Probably fixed nosecap position.
  *	
  *	Revision 1.67  2008/01/10 05:42:20  movieman523
  *	Hopefully fix Saturn 1b mass.
  *	
  *	Revision 1.66  2007/12/11 12:10:37  tschachim
  *	Bugfix meshes.
  *	
  *	Revision 1.65  2007/12/11 07:55:06  movieman523
  *	Revised stage mesh handling for Saturn 1b, to help generalise the code.
  *	
  *	Revision 1.64  2007/12/05 23:07:45  movieman523
  *	Revised to allow SLA panel rotaton to be specified up to 150 degrees. Also start of new connector-equipped vessel code which was mixed up with the rest!
  *	
  *	Revision 1.63  2007/12/04 20:26:33  tschachim
  *	IMFD5 communication including a new TLI for the S-IVB IU.
  *	Additional CSM panels.
  *	
  *	Revision 1.62  2007/12/04 06:51:07  movieman523
  *	Shifted origin of Saturn 1b SIVB meshes to match Saturn V SIVB.
  *	
  *	Revision 1.61  2007/12/03 07:14:22  movieman523
  *	Moved Saturn 1b SIVB mesh back so that payload offsets match.
  *	
  *	Revision 1.60  2007/12/02 07:13:39  movieman523
  *	Updates for Apollo 5 and unmanned Saturn 1b missions.
  *	
  *	Revision 1.59  2007/10/18 00:23:20  movieman523
  *	Primarily doxygen changes; minimal functional change.
  *	
  *	Revision 1.58  2007/08/13 16:06:14  tschachim
  *	Moved bitmaps to subdirectory.
  *	New VAGC mission time pad load handling.
  *	New telescope and sextant panels.
  *	Fixed CSM/LV separation speed.
  *	
  *	Revision 1.57  2007/06/06 15:02:16  tschachim
  *	OrbiterSound 3.5 support, various fixes and improvements.
  *	
  *	Revision 1.56  2007/02/18 01:35:29  dseagrav
  *	MCC / LVDC++ CHECKPOINT COMMIT. No user-visible functionality added. lvimu.cpp/h and mcc.cpp/h added.
  *	
  *	Revision 1.55  2007/02/06 18:30:17  tschachim
  *	Bugfixes docking probe, CSM/LM separation. The ASTP stuff still needs fixing though.
  *	
  *	Revision 1.54  2007/01/23 14:41:57  tschachim
  *	Bugfix docking probe jettison.
  *	
  *	Revision 1.53  2007/01/21 18:34:12  jasonims
  *	shifted j2 exhast
  *	
  *	Revision 1.52  2006/12/07 18:52:43  tschachim
  *	New LC34, Bugfixes.
  *	
  *	Revision 1.51  2006/11/13 14:47:30  tschachim
  *	New SPS engine.
  *	New ProjectApolloConfigurator.
  *	Fixed and changed camera and FOV handling.
  *	
  *	Revision 1.50  2006/09/23 22:34:40  jasonims
  *	New J-2 Engine textures...
  *	
  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "OrbiterSoundSDK35.h"
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "dsky.h"
#include "IMU.h"
#include "lvimu.h"

#include "saturn.h"

#include "saturn1b.h"

#include "sivb.h"
#include "s1b.h"
#include "sm.h"

//
// Meshes are loaded globally, once, so we use these global
// variables for them. Nothing vessel-specific should use a
// global variable.
//

static MESHHANDLE hSat1stg1;
static MESHHANDLE hSat1stg1low;
static MESHHANDLE hSat1intstg;
static MESHHANDLE hSat1intstglow;
static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hNosecap;
static MESHHANDLE hastp;
static MESHHANDLE hastp2;
static MESHHANDLE hCOAStarget;

static SURFHANDLE exhaust_tex;

//
// Same for particle streams.
//

PARTICLESTREAMSPEC srb_contrail = {
	0, 12.0, 1, 50.0, 0.3, 4.0, 4, 3.0, PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};

PARTICLESTREAMSPEC contrail_condensation = {
	0,		// flag
	12.0,	// size
	1,		// rate
	50.0,	// velocity
	0.3,	// velocity distribution
	4.0,	// lifetime
	4,		// growthrate
	3.0,	// atmslowdown
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};

PARTICLESTREAMSPEC s1b_exhaust = {
	0,		// flag
	3.2,	// size
	7000,	// rate
	180.0,	// velocity
	0.15,	// velocity distribution
	0.33,	// lifetime
	4.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-1140, 1.0
};


/*
PARTICLESTREAMSPEC srb_exhaust = {
	0, 8.0, 2, 50.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,//	0, 4.0, 20, 150.0, 0.1, 0.3, 12, 2.0, PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};
*/

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.75,	// size
	2000,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	0.4,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-1140, 1.0
};

PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_PLOG, 1e-6, 0.1
};


void Saturn1b::SetFirstStage ()
{
	SetSize (45);
	SetEmptyMass (Stage1Mass);
	SetPMI (_V(140,145,28));
	SetCrossSections (_V(395,380,115));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();

	SetFirstStageMeshes(-14.0);
	SetSecondStageMeshes(13.95);

	SetView(34.95, false);

	Offset1st = -28.5;
	SetCameraOffset (_V(-1,1.0,35.15));

	EnableTransponder (true);

	// **************************** NAV radios *************************************

	InitNavRadios (4);
}

void Saturn1b::SetFirstStageMeshes(double offset)

{
	double TCP=-54.485-TCPO;//STG0O;
	SetTouchdownPoints (_V(0,-1.0,TCP), _V(-.7,.7,TCP), _V(.7,.7,TCP));

	VECTOR3 mesh_dir=_V(0,0,offset);

	AddMesh (hStage1Mesh, &mesh_dir);
	mesh_dir=_V(0,0,16.2 + offset);
	AddMesh (hInterstageMesh, &mesh_dir);
}

void Saturn1b::SetFirstStageEngines()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// ************************* propellant specs **********************************
	if (!ph_1st)
		ph_1st  = CreatePropellantResource(SI_FuelMass); //1st stage Propellant
	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	// *********************** thruster definitions ********************************

	int i;

	Offset1st = -80.1;//+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};

	VECTOR3 m_exhaust_pos5= {0,1.414,Offset1st+55};
    VECTOR3 m_exhaust_pos6= {1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {0,-1.414,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {-1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos1= {2.12,2.12,Offset1st+55};
    VECTOR3 m_exhaust_pos2= {2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos3= {-2.12,-2.12,Offset1st+55};
	VECTOR3 m_exhaust_pos4= {-2.12,2.12,Offset1st+55};

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);

	SURFHANDLE tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);
	for (i = 0; i < 8; i++)
		AddExhaust(th_main[i], 30.0, 0.80, tex);

	srb_exhaust.tex = oapiRegisterParticleTexture ("ProjectApollo/Contrail_Saturn2");
	s1b_exhaust.tex = oapiRegisterParticleTexture ("ProjectApollo/Contrail_Saturn");

	double exhpos = -4;
	double exhpos2 = -5;
	//AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,-15), &srb_contrail);
	//AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,-15), &srb_contrail);

	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,exhpos2), &s1b_exhaust);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,exhpos2), &s1b_exhaust);
	
	AddExhaustStream (th_main[0], m_exhaust_pos1+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[1], m_exhaust_pos2+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[2], m_exhaust_pos3+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[3], m_exhaust_pos4+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[4], m_exhaust_pos5+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[5], m_exhaust_pos6+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[6], m_exhaust_pos7+_V(0,0,exhpos), &srb_exhaust);
	AddExhaustStream (th_main[7], m_exhaust_pos8+_V(0,0,exhpos), &srb_exhaust);
/**/

	// Contrail
	/*for (i = 0; i < 8; i++) {
		if (contrail[i]) {
			DelExhaustStream(contrail[i]);
			contrail[i] = NULL;
		}
	}

	double conpos = -10;
	contrail[0] = AddParticleStream(&srb_contrail, m_exhaust_pos1+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[1] = AddParticleStream(&srb_contrail, m_exhaust_pos2+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[2] = AddParticleStream(&srb_contrail, m_exhaust_pos3+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[3] = AddParticleStream(&srb_contrail, m_exhaust_pos4+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[4] = AddParticleStream(&srb_contrail, m_exhaust_pos5+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[5] = AddParticleStream(&srb_contrail, m_exhaust_pos6+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[6] = AddParticleStream(&srb_contrail, m_exhaust_pos7+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	contrail[7] = AddParticleStream(&srb_contrail, m_exhaust_pos8+_V(0,0,conpos), _V( 0,0,-1), &contrailLevel);
	*/

}

void Saturn1b::SetSecondStage ()
{
	SetSize (22);
	SetCOG_elev (15.225);

	double EmptyMass = Stage2Mass + (LESAttached ? Abort_Mass : 0.0);

	SetEmptyMass (EmptyMass);
	SetPMI (_V(94,94,20));
	SetCrossSections (_V(267,267,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetBankMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearMeshes();
	SetSecondStageMeshes(13.95-12.25);
}

void Saturn1b::SetSecondStageMeshes(double offset)
{
	UINT meshidx;

	VECTOR3 mesh_dir=_V(0,0,offset);
    AddMesh (hStage2Mesh, &mesh_dir);

	mesh_dir=_V(1.85,1.85,10.55 + offset);
    AddMesh (hStageSLA1Mesh, &mesh_dir);
	mesh_dir=_V(-1.85,1.85,10.55 + offset);
    AddMesh (hStageSLA2Mesh, &mesh_dir);
	mesh_dir=_V(1.85,-1.85,10.55 + offset);
    AddMesh (hStageSLA3Mesh, &mesh_dir);
	mesh_dir=_V(-1.85,-1.85,10.55 + offset);
    AddMesh (hStageSLA4Mesh, &mesh_dir);

	probeidx = -1;
	probeextidx = -1;
	crewidx = -1;
	cmpidx = -1;

	if (SaturnHasCSM()) {

		//
		// Add CSM.
		//

		AddSM(17.05 + offset, false);

		WORD CMMode = MESHVIS_VCEXTERNAL;

		if (LESAttached)
		{
			TowerOffset = 26.15 + offset;
			mesh_dir=_V(0, 0, TowerOffset);
			meshidx = AddMesh (hsat5tower, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

			//
			// If the LES is attached, no point drawing things in the external view which can't
			// actually be seen...
			//
			CMMode = MESHVIS_VC;
		}
		else if (HasProbe)
		{
			mesh_dir=_V(0,0,21.2 + offset);
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		}

		//
		// ... but at least draw the CM in the external view, 
		// otherwise the BPC is floating above the SM.
		//
		mesh_dir=_V(0,0,21.2 + offset);
		meshidx = AddMesh (hCM, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

		//
		// And the Crew.
		//
		if (Crewed) {
			cmpidx = AddMesh (hCMP, &mesh_dir);
			crewidx = AddMesh (hCREW, &mesh_dir);
			SetCrewMesh();
		}

		meshidx = AddMesh (hCMInt, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

		//
		// Don't Forget the Hatch
		//
		meshidx = AddMesh (hFHC, &mesh_dir);
		SetMeshVisibilityMode (meshidx, CMMode);

	}
	else if (NosecapAttached)
	{
		//
		// Add nosecap.
		//
		mesh_dir=_V(0,0,15.8 + offset);
		AddMesh (hNosecap, &mesh_dir);
	}

	// Dummy docking port so the auto burn feature of IMFD 4.2 is working
	// Remove it when a newer release of IMFD don't need that anymore
	VECTOR3 dockpos = {0,0,24.8 + offset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	SetCameraOffset (_V(-1,1.0,31.15-STG1O));
    SetView(22.7, false);
}

void Saturn1b::SetSecondStageEngines ()

{
	ClearThrusters();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	//
	// ************************* propellant specs **********************************
	//

	if(ph_1st)
	{
		//
		// Delete remaining S1B stage propellant.
		//

		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	if (!ph_3rd)
	{
		//
		// Create SIVB stage Propellant
		//

		ph_3rd  = CreatePropellantResource(SII_FuelMass);
	}

	if (!ph_ullage3)
	{
		//
		// Create SIVB stage ullage rocket propellant
		//

		ph_ullage3  = CreatePropellantResource(80.0);
	}

	//
	// display SIVB stage propellant level in generic HUD
	//

	SetDefaultPropellantResource (ph_3rd);

	//
	// *********************** thruster definitions ********************************
	//

	VECTOR3 m_exhaust_pos1= {0,0,-9.-STG1O+10};

	//
	// orbiter main thrusters
	//

	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC, ph_3rd, ISP_SECOND_VAC, ISP_SECOND_SL);
	thg_main = CreateThrusterGroup (th_main, 1, THGROUP_MAIN);
	AddExhaust (th_main[0], 30.0, 2.9 ,J2Tex);

	//
	//  Ullage rockets (3)
	//

	VECTOR3	m_exhaust_pos6= _V(3.27,0.46,-2-STG1O+9);
	VECTOR3 m_exhaust_pos7= _V(-1.65,2.86,-2-STG1O+9);
	VECTOR3	m_exhaust_pos8= _V(-1.65,-2.86,-2-STG1O+9);

	int i;

	//
	// Ullage rocket thrust and ISP is a guess for now.
	//

	th_ver[0] = CreateThruster (m_exhaust_pos6, _V( -0.45,0.0,1), 10000, ph_ullage3, 3000);
	th_ver[1] = CreateThruster (m_exhaust_pos7, _V( 0.23,-0.39,1), 10000, ph_ullage3, 3000);
	th_ver[2] = CreateThruster (m_exhaust_pos8, _V( 0.23,0.39,1), 10000, ph_ullage3, 3000);

	for (i = 0; i < 3; i++) {
		AddExhaust(th_ver[i], 11.0, 0.25, exhaust_tex);
		AddExhaustStream(th_ver[i], &solid_exhaust);
	}
	thg_ver = CreateThrusterGroup (th_ver, 3,THGROUP_USER);

	//
	// Give the AGC our new stats.
	//

	agc.SetVesselStats(ISP_SECOND_VAC, THRUST_SECOND_VAC, false);
	iu.SetVesselStats(ISP_SECOND_VAC, THRUST_SECOND_VAC);
}

void Saturn1b::SeparateStage (int new_stage)

{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;

	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);

	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);

	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && !bAbort)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0,0,-4.0);
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && bAbort )
	{
		ofs1= _V(0, 0, 4.7);
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_SIVB && new_stage == CM_STAGE)
	{
		ofs1= _V(0, 0, 4.7);
		vel1 = _V(0,0,-2);
	}
	
	if ((stage == LAUNCH_STAGE_SIVB && new_stage != CM_STAGE) || stage == STAGE_ORBIT_SIVB)
	{
	 	ofs1 = _V(0, 0, 1.7);
		vel1 = _V(0, 0, 0);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0, 0, -0.1);
	}

	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};

	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);

	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;

	if (stage == CM_STAGE)
	{
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && !bAbort )
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;
		StageS.play();

		//
		// Create S1b stage and set it up.
		//

		if (hstg1) {
			S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs1);

			S1BSettings S1Config;

			S1Config.SettingsType.S1B_SETTINGS_ENGINES = 1;
			S1Config.SettingsType.S1B_SETTINGS_FUEL = 1;
			S1Config.SettingsType.S1B_SETTINGS_GENERAL = 1;
			S1Config.SettingsType.S1B_SETTINGS_MASS = 1;

			S1Config.EngineNum = 8;
			S1Config.RetroNum = 4;
			S1Config.EmptyMass = SI_EmptyMass;
			S1Config.MainFuelKg = GetPropellantMass(ph_1st);
			S1Config.MissionTime = MissionTime;
			S1Config.Realism = Realism;
			S1Config.VehicleNo = VehicleNo;
			S1Config.ISP_FIRST_SL = ISP_FIRST_SL;
			S1Config.ISP_FIRST_VAC = ISP_FIRST_VAC;
			S1Config.THRUST_FIRST_VAC = THRUST_FIRST_VAC;
			S1Config.CurrentThrust = GetThrusterLevel(th_main[0]);
			S1Config.LowRes = LowRes;

			stage1->SetState(S1Config);
		}

		SetSecondStage ();
		SetSecondStageEngines ();
		ShiftCentreOfMass (_V(0,0,12.25));
		SetThrusterGroupLevel(thg_ver,1.0);
	}

	if ((stage == LAUNCH_STAGE_SIVB && new_stage != CM_STAGE) || stage == STAGE_ORBIT_SIVB)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		CreateSIVBStage("ProjectApollo/nsat1stg2", vs1, false);

		SeparationS.play();
		SetCSMStage();

		ShiftCentreOfMass(_V(0, 0, 20.8));
		SeparationSpeed = 0.15;
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		SMJetS.play();

		GetApolloName(VName); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "ProjectApollo/SM", vs1);

		SMSettings SMConfig;

		SMConfig.SettingsType.SM_SETTINGS_ENGINES = 1;
		SMConfig.SettingsType.SM_SETTINGS_FUEL = 1;
		SMConfig.SettingsType.SM_SETTINGS_GENERAL = 1;
		SMConfig.SettingsType.SM_SETTINGS_MASS = 1;

		SMConfig.EmptyMass = SM_EmptyMass;
		SMConfig.MainFuelKg = GetPropellantMass(ph_sps);
		SMConfig.RCSAFuelKg = GetPropellantMass(ph_rcs0);
		SMConfig.RCSBFuelKg = GetPropellantMass(ph_rcs1);
		SMConfig.RCSCFuelKg = GetPropellantMass(ph_rcs2);
		SMConfig.RCSDFuelKg = GetPropellantMass(ph_rcs3);
		SMConfig.MissionTime = MissionTime;
		SMConfig.Realism = Realism;
		SMConfig.VehicleNo = VehicleNo;
		SMConfig.LowRes = LowRes;
		SMConfig.showHGA = !NoHGA;
		SMConfig.A13Exploded = ApolloExploded;

		SM *SMVessel = (SM *) oapiGetVesselInterface(hSMJet);
		SMVessel->SetState(SMConfig);

		// Store CM Propellant 
		double cmprop1 = -1;
		double cmprop2 = -1;
		if (ph_rcs_cm_1) cmprop1 = GetPropellantMass(ph_rcs_cm_1);
		if (ph_rcs_cm_2) cmprop2 = GetPropellantMass(ph_rcs_cm_2);

		SetReentryStage();

		// Restore CM Propellant
		if (cmprop1 != -1) SetPropellantMass(ph_rcs_cm_1, cmprop1); 
		if (cmprop2 != -1) SetPropellantMass(ph_rcs_cm_2, cmprop2); 

		ShiftCentreOfMass(_V(0, 0, 2.1));
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}

	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}

	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && bAbort )
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel("Saturn_Abort", "ProjectApollo/Saturn1bAbort1", vs1);
		SetReentryStage();
		ShiftCentreOfMass(_V(0, 0, 35.15));
	}

	if (stage == LAUNCH_STAGE_SIVB && new_stage == CM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		StageS.play();
		habort = oapiCreateVessel("Saturn_Abort", "ProjectApollo/Saturn1bAbort2", vs1);
		SetReentryStage();
		ShiftCentreOfMass(_V(0, 0, 22.9));
	}
 }

//
// Load the meshes once per DLL.
//

void Saturn1bLoadMeshes()

{
	hSat1stg1 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg1");
	hSat1stg1low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg1");
	hSat1intstg = oapiLoadMeshGlobal ("ProjectApollo/nsat1intstg");
	hSat1intstglow = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1intstg");
	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg2");

	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hastp2 = oapiLoadMeshGlobal ("ProjectApollo/nASTP2");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");
	hNosecap = oapiLoadMeshGlobal ("ProjectApollo/nsat1aerocap");

	exhaust_tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");
}

//
// Update per-vessel handles to the appropriate low-res or high-res meshes.
//

void Saturn1b::SetupMeshes()

{
	if (LowRes) {
		hStage1Mesh = hSat1stg1low;
		hStage2Mesh = hSat1stg2low;
		hInterstageMesh = hSat1intstglow;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
	else {
		hStage1Mesh = hSat1stg1;
		hStage2Mesh = hSat1stg2;
		hInterstageMesh = hSat1intstg;
		hStageSLA1Mesh = hSat1stg21;
		hStageSLA2Mesh = hSat1stg22;
		hStageSLA3Mesh = hSat1stg23;
		hStageSLA4Mesh = hSat1stg24;
	}
}

void Saturn1b::CreateStageOne() {

	// Create hidden SIB vessel
	char VName[256];
	VESSELSTATUS vs;

	GetStatus(vs);
	strcpy (VName, GetName()); 
	strcat (VName, "-STG1");
	hstg1 = oapiCreateVessel(VName,"ProjectApollo/nsat1stg1", vs);

	// Load only the necessary meshes
	S1B *stage1 = (S1B *) oapiGetVesselInterface(hstg1);
	stage1->LoadMeshes(LowRes);
}
