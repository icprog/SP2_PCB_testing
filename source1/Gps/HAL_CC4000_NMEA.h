/*****************************************************************************
*
*  HAL_CC4000_NMEA.h - CC4000 NMEA Functions Definitions.
*  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#ifndef HAL_CC4000_NMEA_H
#define HAL_CC4000_NMEA_H


//*****************************************************************************
//
// GLL Message Definitions.
//
//*****************************************************************************
#define GLL_LATITUDE		1
#define GLL_NS				2
#define GLL_LONGITUDE		3
#define GLL_EW				4
#define GLL_UTC_TIME		5
#define GLL_DATA_STATUS		6
#define GLL_MODE			7

#define GPS_DATA_INVALID   'V'
#define GPS_DATA_VALID     'A'

//*****************************************************************************
//
// RMC Message Definitions.
//
//*****************************************************************************
#define RMC_UTC_TIME		1
#define RMC_DATA_STATUS		2
#define RMC_LATITUDE		3
#define RMC_NS				4
#define RMC_LONGITUDE		5
#define RMC_EW				6
#define RMC_SPEED_GND_KNOTS	7
#define RMC_COURSE_GND_TRUE	8
#define RMC_DATE			9
#define RMC_MAGNETIC_VAR   10
#define RMC_MAGNETIC_DEG   11

//*****************************************************************************
//
// GGA Message Definitions.
//
//*****************************************************************************
#define GGA_UTC_TIME		1
#define GGA_LATITUDE		2
#define GGA_NS				3
#define GGA_LONGITUDE		4
#define GGA_EW				5
#define GGA_QUALITY_IND		6
#define GGA_NUM_USED_SVS	7
#define GGA_HOR_DIL_PREC	8
#define GGA_ALTITUDE		9
#define GGA_ALTITUDE_UNITS	10
#define GGA_GEOIDAL_SEP    11
#define GGA_GEOIDAL_SEP_UNITS 12
#define GGA_AGE_DIFF_DATA  13
#define GGA_DIFF_REF_ID	   14

//*****************************************************************************
//
// VTG Message Definitions.
//
//*****************************************************************************
#define VTG_COURSE_GND_TRUE	1
#define VTG_COURSE_GND_T	2
#define VTG_COURSE_GND_MAGN	3
#define VTG_COURSE_GND_M	4
#define VTG_SPEED_GND_KNOTS	5
#define VTG_SPEED_GND_N 	6
#define VTG_SPEED_GND_KMHRS	7
#define VTG_SPEED_GND_K 	8
#define VTG_MODE		9

//*****************************************************************************
//
// GSA Message Definitions.
//
//*****************************************************************************
#define GSA_FIX_MODE		1
#define GSA_FIX_STATUS		2
#define GSA_SV_IDS_FIRST	3
#define GSA_SV_IDS_LAST	   14
#define GSA_PDOP		   15
#define GSA_HDOP		   16
#define GSA_VDOP		   17
#define GSA_SV_CHAR_LEN         3

//*****************************************************************************
//
// GSV Message Definitions.
//
//*****************************************************************************
#define GSV_TOT_MSGS		1
#define GSV_MSG_NUM		2
#define GSV_NUM_VIEW_SVS	3
#define GSV_SV_ID_START		4
#define GSV_SV_ELEV_START	5
#define GSV_SV_AZI_START	6
#define GSV_SV_SNR_START	7
#define GSV_SV_NEXT_OFFSET	4
#define GSV_SV_MAX_PER_MSG	4
#define GSV_SV_AZI_MAX_LEN  3
#define GSV_SV_ID_MAX_LEN   2
#define GSV_SV_ELEV_MAX_LEN 2
#define GSV_SV_SNR_MAX_LEN  2

//*****************************************************************************
//
// Prototypes for CC4000 APIs.
//
//*****************************************************************************

char getGPSMessageError();


//*****************************************************************************
//
// Prototypes for GLL APIs.
//
//*****************************************************************************

void getGLLGPSLatitude(char *latitude, char latLength);
void getGLLGPSNorthSouth(char *northsouth, char nsLength);
void getGLLGPSLongitude(char *longitude, char longLength);
void getGLLGPSEastWest(char *eastwest, char ewLength);
void getGLLGPSUTCTime(char *utctime, char utcTimeLength);
void getGLLGPSDataStatus(char *dstatus, char dstatusLength);
void getGLLGPSMode(char *mode, char modeLength);


//*****************************************************************************
//
// Prototypes for RMC APIs.
//
//*****************************************************************************

void getRMCGPSUTCTime(char *utctime, char utcTimeLength);
void getRMCGPSDataStatus(char *dstatus, char dstatusLength);
void getRMCGPSLatitude(char *latitude, char latLength);
void getRMCGPSNorthSouth(char *northsouth, char nsLength);
void getRMCGPSLongitude(char *longitude, char longLength);
void getRMCGPSEastWest(char *eastwest, char ewLength);
void getRMCGPSSpeedGndKnots(char *speed, char speedLength);
void getRMCGPSCourseGndTrue(char *course, char courseLength);
void getRMCGPSDate(char *date, char dateLength);
void getRMCGPSMagneticVar(char *magvar, char magvarLength);
void getRMCGPSMagneticDeg(char *magdeg, char magdegLength);


//*****************************************************************************
//
// Prototypes for GGA APIs.
//
//*****************************************************************************

void getGGAGPSUTCTime(char *utctime, char utcTimeLength);
void getGGAGPSLatitude(char *latitude, char latLength);
void getGGAGPSNorthSouth(char *northsouth, char nsLength);
void getGGAGPSLongitude(char *longitude, char longLength);
void getGGAGPSEastWest(char *eastwest, char ewLength);
void getGGAGPSQualityInd(char *quality, char qualityLength);
void getGGAGPSNumUsedSVs(char *svs, char svsLength);
void getGGAGPSHorDilPrec(char *hordil, char hordilLength);
void getGGAGPSAltitude(char *altitude, char altitudeLength);
void getGGAGPSGeoidalSep(char *geoidal, char geoidalLength);
void getGGAGPSAgeDiffData(char *age, char ageLength);
void getGGAGPSDiffRefID(char *id, char idLength);


//*****************************************************************************
//
// Prototypes for VTG APIs.
//
//*****************************************************************************

void getVTGGPSCourseGndTrue(char *course, char courseLength);
void getVTGGPSCourseGndMagn(char *course, char courseLength);
void getVTGGPSSpeedGndKnots(char *speed, char speedLength);
void getVTGGPSSpeedGndKmHrs(char *speed, char speedLength);
void getVTGGPSMode(char *mode, char modeLength);


//*****************************************************************************
//
// Prototypes for GSA APIs.
//
//*****************************************************************************

void getGSAGPSFixMode(char *fixmode, char fixmodeLength);
void getGSAGPSFixStatus(char *fixstatus, char fixstatusLength);
void getGSAGPSSVsIDs(char ids[][GSA_SV_CHAR_LEN], char numIDs);
void getGSAGPSPDOP(char *pdop, char pdopLength);
void getGSAGPSHDOP(char *hdop, char hdopLength);
void getGSAGPSVDOP(char *vdop, char vdopLength);


//*****************************************************************************
//
// Prototypes for GSV APIs.
//
//*****************************************************************************

void getGSVGPSTotalMsgs(char *totmsgs, char totmsgsLength);
void getGSVGPSViewSVs(char *numsvs, char numsvsLength);
void getGSVGPSSVsIDs(char ids[][3], char numIds);
void getGSVGPSSVsElevations(char elevations[][3], char elevationsLength);
void getGSVGPSSVsAzimuths(char azimuths[][4], char azimuthsLength);
void getGSVGPSSVsSNRs(char snrs[][3], char numSVs);



#endif /* HAL_CC4000_NMEA_H */
