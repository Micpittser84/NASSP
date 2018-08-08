/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission G

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

**************************************************************************/

#include "Orbitersdk.h"
#include "soundlib.h"
#include "saturn.h"
#include "sivb.h"
#include "mcc.h"
#include "rtcc.h"
#include "MCC_Mission_G.h"
#include "iu.h"

void MCC::MissionSequence_G()
{
	switch (MissionState)
	{
	case MST_G_INSERTION: //Ground liftoff time update to TLI Simulation
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(1.0*3600.0 + 30.0*60.0), 10, MST_G_EPO1);
		break;
	case MST_G_EPO1: //TLI Simulation to TLI+90 PAD
		UpdateMacro(UTP_NONE, PT_NONE, true, 11, MST_G_EPO2);
		break;
	case MST_G_EPO2: //TLI+90 Maneuver PAD to TLI+5h P37 PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 12, MST_G_EPO3);
		break;
	case MST_G_EPO3: //TLI+5h P37 PAD to TLI PAD
		UpdateMacro(UTP_PADONLY, PT_P37PAD, SubStateTime > 3.0*60.0, 13, MST_G_EPO4);
		break;
	case MST_G_EPO4: //TLI PAD to TLI Evaluation
		UpdateMacro(UTP_PADONLY, PT_TLIPAD, rtcc->GETEval(rtcc->calcParams.TLI), 14, MST_G_TRANSLUNAR1);
		break;
	case MST_G_TRANSLUNAR1: //TLI Evaluation to Evasive Maneuver Update
		UpdateMacro(UTP_NONE, PT_NONE, true, 15, MST_G_TRANSLUNAR2, scrubbed, rtcc->GETEval(3.0*3600.0), MST_G_EPO1);
		break;
	case MST_G_TRANSLUNAR2:
		switch (SubState) {
		case 0:
		{
			addMessage("TLI");
			MissionPhase = MMST_TL_COAST;
			setSubState(1);
		}
		break;
		case 1:
		{
			if (rtcc->GETEval(rtcc->calcParams.TLI + 3600.0 + 10.0*60.0))
			{
				setState(MST_G_TRANSLUNAR3);
			}
		}
		break;
		}
	case MST_G_TRANSLUNAR3: //Evasive Maneuver Update to TB8 Enable
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 3600.0 + 30.0*60.0), 16, MST_G_TRANSLUNAR4);
		break;
	case MST_G_TRANSLUNAR4:  //TB8 enable to Block Data 1
		switch (SubState) {
		case 0:
		{
			if (cm->GetStage() >= CSM_LEM_STAGE)
			{
				setSubState(1);
			}
		}
		break;
		case 1:
		{
			if (SubStateTime > 2.0*60.0)
			{
				setSubState(2);
			}
		}
		break;
		case 2:
		{
			if (sivb == NULL)
			{
				VESSEL *v;
				OBJHANDLE hLV;
				hLV = oapiGetObjectByName(LVName);
				if (hLV != NULL)
				{
					v = oapiGetVesselInterface(hLV);

					if (!stricmp(v->GetClassName(), "ProjectApollo\\sat5stg3") ||
						!stricmp(v->GetClassName(), "ProjectApollo/sat5stg3")) {
						sivb = (SIVB *)v;
					}
				}
			}

			sivb->GetIU()->dcs.Uplink(DCSUPLINK_TIMEBASE_8_ENABLE, NULL);
			setSubState(3);
		}
		break;
		case 3:
			if (rtcc->GETEval(rtcc->calcParams.TLI + 3.0*3600.0))
			{
				setState(MST_G_TRANSLUNAR5);
			}
			break;
		}
		break;
	case MST_G_TRANSLUNAR5: //Block Data 1 to MCC-1 Calculation
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval(rtcc->calcParams.TLI + 3.0*3600.0 + 20.0*60.0), 17, MST_G_TRANSLUNAR6);
		break;
	case MST_G_TRANSLUNAR6: //MCC-1 Calculation to MCC-1 update (or PTC REFSMMAT)
		UpdateMacro(UTP_NONE, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 7.0*3600.0 + 10.0*60.0), 20, MST_G_TRANSLUNAR7, scrubbed, rtcc->GETEval(rtcc->calcParams.TLI + 4.0*3600.0), MST_G_TRANSLUNAR_NO_MCC1_1);
		break;
	case MST_G_TRANSLUNAR7: //MCC-1 update to PTC REFSMMAT update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 9.0*3600.0 + 5.0*60.0), 21, MST_G_TRANSLUNAR8);
		break;
	case MST_G_TRANSLUNAR_NO_MCC1_1: //PTC REFSMMAT update to SV update (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 7.0*3600.0 + 10.0*60.0), 19, MST_G_TRANSLUNAR_NO_MCC1_2);
		break;
	case MST_G_TRANSLUNAR_NO_MCC1_2: //SV update to Block Data 2 (MCC-1 was scrubbed)
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, rtcc->GETEval(rtcc->calcParams.TLI + 9.0*3600.0 + 5.0*60.0), 1, MST_G_TRANSLUNAR9);
		break;
	case MST_G_TRANSLUNAR8: //PTC REFSMMAT update to Block Data 2
		UpdateMacro(UTP_CMCUPLINKONLY, PT_NONE, true, 19, MST_G_TRANSLUNAR9);
		break;
	case MST_G_TRANSLUNAR9: //Block Data 2 to MCC-2 update
		UpdateMacro(UTP_PADONLY, PT_P37PAD, rtcc->GETEval(rtcc->calcParams.TLI + 22.0*3600.0 + 50.0*60.0), 18, MST_G_TRANSLUNAR10);
		break;
	case MST_G_TRANSLUNAR10: //MCC-2 update to Lunar Flyby PAD
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.TLI + 32.0*3600.0 + 10.0*60.0), 22, MST_G_TRANSLUNAR11);
		break;
	case MST_G_TRANSLUNAR11: //Lunar Flyby PAD to MCC-3
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 23.0*3600.0 - 30.0*60.0), 23, MST_G_TRANSLUNAR12);
		break;
	case MST_G_TRANSLUNAR12: //MCC-3 update to MCC-4 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 6.0*3600.0 - 30.0*60.0), 24, MST_G_TRANSLUNAR13);
		break;
	case MST_G_TRANSLUNAR13: //MCC-4 update to PC+2 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 25, MST_G_TRANSLUNAR14);
		break;
	case MST_G_TRANSLUNAR14: //PC+2 update to LOI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 2.0*3600.0 - 50.0*60.0), 26, MST_G_TRANSLUNAR15);
		break;
	case MST_G_TRANSLUNAR15: //LOI-1 update to TEI-1 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 2.0*3600.0 - 50.0*60.0), 30, MST_G_TRANSLUNAR16);
		break;
	case MST_G_TRANSLUNAR16: //TEI-1 update to TEI-4 update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, SubStateTime > 5.0*60.0, 40, MST_G_TRANSLUNAR17);
		break;
	case MST_G_TRANSLUNAR17: //TEI-4 update to Rev 1 Map Update
		UpdateMacro(UTP_PADONLY, PT_AP11MNV, rtcc->GETEval(rtcc->calcParams.LOI - 40.0*60.0), 41, MST_G_TRANSLUNAR18);
		break;
	case MST_G_TRANSLUNAR18: //Rev 1 Map Update to lunar orbit phase begin
		UpdateMacro(UTP_PADONLY, PT_AP10MAPUPDATE, rtcc->GETEval(rtcc->calcParams.LOI), 31, MST_G_LUNAR_ORBIT_LOI_DAY_1);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_1: //Lunar orbit phase begin
		MissionPhase = MMST_LUNAR_ORBIT;
		setState(MST_G_LUNAR_ORBIT_LOI_DAY_2);
		break;
	case MST_G_LUNAR_ORBIT_LOI_DAY_2: //LOI-2 update to TEI-5 update
		UpdateMacro(UTP_PADWITHCMCUPLINK, PT_AP11MNV, SubStateTime > 5.0*60.0, 32, MST_G_LUNAR_ORBIT_LOI_DAY_3);
		break;
	}
}