/*****************************************************************************
*Tested OK19/12/13
*  HAL_CC4000_NMEA.c - CC4000 NMEA Functions Implementation.
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

//*****************************************************************************
//
//!  @file       HAL_CC4000.c
//! \addtogroup  CC4000_GPS
//! @{
//
//*****************************************************************************

#include <stdlib.h> 
#include "string.h"
#include "HAL_CC4000.h"
#include "HAL_CC4000_NMEA.h"



//*****************************************************************************
//
//! \brief Provides the Latitude value from the GPS GLL Message.
//!
//! \param latitude[] is an array in which the Latitude will be stored
//! \param latLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example Usage:
//! @code
//! char lat[13];
//! getGLLGPSLatitude(lat, 13);
//! if(strlen(lat) > 0)
//!     printf("%s\n",lat);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! lat contains "3254.772537"
//! @endcode
//!
//
//*****************************************************************************
void getGLLGPSLatitude(char *latitude, char latLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_LATITUDE, latitude, latLength);   
}


//*****************************************************************************
//
//! \brief Provides the North/South value from the GPS GLL Message.
//!
//! \param northsouth[] is an array in which the North/South will be stored
//! \param nsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example Usage:
//! @code
//! char ns;
//! getGLLGPSNorthSouth(&ns, 1);
//! if(strlen(ns) > 0)
//!     printf("%s\n",lat);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ns contains "N"
//! @endcode
//
//*****************************************************************************
void getGLLGPSNorthSouth(char *northsouth, char nsLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_NS, northsouth, nsLength);   
}


//*****************************************************************************
//
//! \brief Provides the Longitude value from the GPS GLL Message.
//!
//! \param longitude[] is an array in which the longitude will be stored
//! \param longLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char longitude[14];
//! getGLLGPSLongitude(longitude,14);
//! if(strlen(longitude) > 0)
//!     printf("%s\n",longitude);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! longitude contains "09645.272482"
//! @endcode
//
//*****************************************************************************
void getGLLGPSLongitude(char *longitude, char longLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_LONGITUDE, longitude, 
                    longLength);   
}

//*****************************************************************************
//
//! \brief Provides the East/West value from the GPS GLL Message.
//!
//! \param eastwest[] is an array in which East/West will be stored
//! \param ewLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char ew[14];
//! getGLLGPSEastWest(ew,1);
//! if(strlen(ew) > 0)
//!     printf("%s\n",ew);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ew contains "W"
//! @endcode
//
//*****************************************************************************
void getGLLGPSEastWest(char *eastwest, char ewLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_EW, eastwest, ewLength);   
}


//*****************************************************************************
//
//! \brief Provides the UTC Time value from the GPS GLL Message.
//!
//! \param utctime[] is an array in which UTC will be stored as hh:mm:ss.ss
//! \param utcTimeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char utc[9];
//!	getGLLGPSUTCTime(utc, 9);
//! if(strlen(utc) > 0)
//!     printf("%s\n",utc);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! utc contains "215245.00" 
//! @endcode
//
//*****************************************************************************
void getGLLGPSUTCTime(char *utctime, char utcTimeLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_UTC_TIME, utctime, 
                    utcTimeLength);   
}

// TODO: Clarify getGLLGPSDataStatus and getGLLGPSMode
//*****************************************************************************
//
//! \brief Provides the Data Status character from the GPS GLL Message.
//!
//! \param dstatus[] is an array in which data status will be stored
//! \param dstatusLength is the number of characters to return
//!
//! dstatus is 'A' if data valid, 'V' if data not valid.
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char dataStatus;
//!	getGLLGPSDataStatus(&dataStatus, 1);
//! if(strlen(dataStatus) > 0)
//!     printf("%s\n",dataStatus);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! dataStatus contains "A" 
//! @endcode
//
//*****************************************************************************
void getGLLGPSDataStatus(char *dstatus, char dstatusLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_DATA_STATUS, dstatus, 
                    dstatusLength);   
}

// TODO: Verify difference
//*****************************************************************************
//
//! \brief Provides the Mode Indicator character from the GPS GLL Message.
//!
//! mode is 'A' if data valid, 'V' if data not valid.
//!
//! \param mode[] is an array in which mode will be stored
//! \param modeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char mode;
//!	getGLLGPSMode(&mode, 1);
//! if(strlen(mode) > 0)
//!     printf("%s\n",mode);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! mode contains "A" 
//! @endcode
//
//*****************************************************************************
void getGLLGPSMode(char *mode, char modeLength){
    
    parseGPSMessage(getGLLGPSMessage(), GLL_MODE, mode, modeLength);   
}






//*****************************************************************************
//
//! \brief Provides the UTC Time value from the GPS RMC Message.
//!
//! \param utctime[] is an array in which UTC will be stored
//! \param utcTimeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char utc[9];
//!	getRMCGPSUTCTime(utc, 9);
//! if(strlen(utc) > 0)
//!     printf("%s\n",utc);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! utc contains "215245.00" 
//! @endcode
//
//*****************************************************************************
void getRMCGPSUTCTime(char *utctime, char utcTimeLength){
    parseGPSMessage(getRMCGPSMessage(), RMC_UTC_TIME, utctime, 
                    utcTimeLength); 
}

//*****************************************************************************
//
//! \brief Provides the Data Status value from the GPS RMC Message.
//!
//! mode is 'A' if data valid, 'V' if data not valid.
//!
//! \param dstatus[] is an array in which data status will be stored
//! \param dstatusLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char dataStatus;
//!	getRMCGPSDataStatus(&dataStatus, 1);
//! if(strlen(dataStatus) > 0)
//!     printf("%s\n",dataStatus);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! dataStatus contains "A" 
//! @endcode
//
//*****************************************************************************
void getRMCGPSDataStatus(char *dstatus, char dstatusLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_DATA_STATUS, dstatus, 
                    dstatusLength);   
}

//*****************************************************************************
//
//! \brief Provides the Latitude value from the GPS RMC Message.
//!
//! \param latitude[] is an array in which the Latitude will be stored
//! \param latLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char lat[13];
//!	getRMCGPSLatitude(lat, 13);
//! if(strlen(lat) > 0)
//!     printf("%s\n",lat);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! lat contains "3254.772537"
//! @endcode
//!
//
//*****************************************************************************
void getRMCGPSLatitude(char *latitude, char latLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_LATITUDE, latitude, latLength);   
}

//*****************************************************************************
//
//! \brief Provides the North/South value from the GPS RMC Message.
//!
//! \param northsouth[] is an array in which the North/South will be stored
//! \param nsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//!  char ns;
//!	 getRMCGPSNorthSouth(&ns, 1);
//! if(strlen(utc) > 0)
//!     printf("%s\n",utc);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ns contains "N"
//! @endcode
//
//*****************************************************************************
void getRMCGPSNorthSouth(char *northsouth, char nsLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_NS, northsouth, nsLength);   
}

//*****************************************************************************
//
//! \brief Provides the Longitude value from the GPS RMC Message.
//!
//! \param longitude[] is an array in which the longitude will be stored
//! \param longLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//!  char longitude[14];
//!	 getRMCGPSLongitude(longitude, 14);
//! if(strlen(longitude) > 0)
//!     printf("%s\n",longitude);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! longitude contains "09645.272482"
//! @endcode
//
//*****************************************************************************
void getRMCGPSLongitude(char *longitude, char longLength){
    parseGPSMessage(getRMCGPSMessage(), RMC_LONGITUDE, longitude, longLength);
}


//*****************************************************************************
//
//! \brief Provides the East/West value from the GPS RMC Message.
//!
//! \param eastwest[] is an array in which East/West will be stored
//! \param ewLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char ew;
//!	getRMCGPSEastWest(&ew, 1);
//! if(strlen(ew) > 0)
//!     printf("%s\n",ew);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ew contains "W"
//! @endcode
//
//*****************************************************************************
void getRMCGPSEastWest(char *eastwest, char ewLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_EW, eastwest, ewLength);   
}

//*****************************************************************************
//
//! \brief Provides the Ground Speed in Knots from the GPS RMC Message.
//!
//! \param speed[] is an array in which the speed string will be stored
//! \param speedLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char speedknots[7];
//!	getRMCGPSSpeedGndKnots(speedknots, 7);
//! if(strlen(speedknots) > 0)
//!     printf("%s\n",speedknots);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! speedknots contains "10.5"
//! @endcode
//
//*****************************************************************************
void getRMCGPSSpeedGndKnots(char *speed, char speedLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_SPEED_GND_KNOTS, speed, 
                    speedLength);  
}

// TODO: Clarify how many characters this uses
//*****************************************************************************
//
//! \brief Provides the Course Ground True from the GPS RMC Message.
//!
//! \param course[] is an array in which the course string will be stored
//! \param courseLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char crs[7];
//!	getRMCGPSCourseGndTrue(crs, 7);
//! if(strlen(crs) > 0)
//!     printf("%s\n",crs);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! crs contains "0.0"
//! @endcode
//
//*****************************************************************************
void getRMCGPSCourseGndTrue(char *course, char courseLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_COURSE_GND_TRUE, course, 
                    courseLength);	
}

//*****************************************************************************
//
//! \brief Provides the date from the GPS RMC Message.
//!
//! Date provided in dd/mm/yy format
//!
//! \param date[] is an array in which the date string will be stored
//! \param dateLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char dt[7];
//!	getRMCGPSDate(dt, 7);
//! if(strlen(dt) > 0)
//!     printf("%s\n",dt);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! dt contains "230311"
//! @endcode
//
//*****************************************************************************
void getRMCGPSDate(char *date, char dateLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_DATE, date, dateLength);	
}

// TODO: Explain output of this Magnetic variation function, since next one is similar
//*****************************************************************************
//
//! \brief Provides the Magnetic Variation from the GPS RMC Message.
//!
//!
//! \param magvar[] is an array in which the magnetic variation string 
//!        will be stored.
//! \param dateLength is the number of characters to return.
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char mv[7];
//!	getRMCGPSMagneticVar(mv, 7);
//! if(strlen(mv) > 0)
//!     printf("%s\n",mv);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! mv contains "27.2"
//! @endcode
//
//*****************************************************************************
void getRMCGPSMagneticVar(char *magvar, char magvarLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_MAGNETIC_VAR, magvar, magvarLength);	
}

//*****************************************************************************
//
//! \brief Provides the Magnetic Variation Degrees from the GPS RMC Message.
//!
//! \param magdeg[] is an array in which the magnetic variation string 
//!        will be stored.
//! \param magdegLength is the number of characters to return.
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char mvd[7];
//!	getRMCGPSMagneticDeg(mvd, 7);
//! if(strlen(mvd) > 0)
//!     printf("%s\n",mvd);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! mvd contains "1.1"
//! @endcode
//
//*****************************************************************************
void getRMCGPSMagneticDeg(char *magdeg, char magdegLength){
    
    parseGPSMessage(getRMCGPSMessage(), RMC_MAGNETIC_DEG, magdeg, magdegLength);	
}

//*****************************************************************************
//
//! \brief Provides the Altitude value in meters from the GPS GGA Message.
//!
//! \param altitude[] is an array in which the altitude will be stored
//! \param altitudeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char alt[7];
//!	getGGAGPSAltitude(alt, 7);
//! if(strlen(alt) > 0)
//!     printf("%s\n",alt);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! alt contains "200.6"
//! @endcode
//
//*****************************************************************************
void getGGAGPSAltitude(char *altitude, char altitudeLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_ALTITUDE, altitude, altitudeLength);   
}

//*****************************************************************************
//
//! \brief Provides the UTC Time value from the GPS GGA Message.
//!
//! Given as 24-Hour Clock notation 
//!
//! \param utctime[] is an array in which UTC will be stored
//! \param utcTimeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char utc[9];
//!	getGGAGPSUTCTime(utc, 9);
//! if(strlen(utc) > 0)
//!     printf("%s\n",utc);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! utc contains "215245.00" 
//! @endcode
//
//*****************************************************************************
void getGGAGPSUTCTime(char *utctime, char utcTimeLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_UTC_TIME, utctime, utcTimeLength);   
}

//*****************************************************************************
//
//! \brief Provides the Latitude value from the GPS GGA Message.
//!
//! \param latitude[] is an array in which the Latitude will be stored
//! \param latLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example Usage:
//! @code
//! char lat[13];
//! getGGAGPSLatitude(lat, 13);
//! if(strlen(lat) > 0)
//!     printf("%s\n",lat);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! lat contains "3254.772537"
//! @endcode
//!
//
//*****************************************************************************
void getGGAGPSLatitude(char *latitude, char latLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_LATITUDE, latitude, latLength);   
}


//*****************************************************************************
//
//! \brief Provides the North/South value from the GPS GGA Message.
//!
//! \param northsouth[] is an array in which the North/South will be stored
//! \param nsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char ns;
//!	getGGAGPSNorthSouth(&ns, 1);
//! if(strlen(utc) > 0)
//!     printf("%s\n",utc);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ns contains "N"
//! @endcode
//
//*****************************************************************************
void getGGAGPSNorthSouth(char *northsouth, char nsLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_NS, northsouth, nsLength);   
}



//*****************************************************************************
//
//! \brief Provides the Longitude value from the GPS GGA Message.
//!
//! \param longitude[] is an array in which the longitude will be stored
//! \param longLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char longitude[14];
//! getGGAGPSLongitude(lat,13);
//! if(strlen(longitude) > 0)
//!     printf("%s\n",longitude);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! longitude contains "09645.272482"
//! @endcode
//
//*****************************************************************************
void getGGAGPSLongitude(char *longitude, char longLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_LONGITUDE, longitude, 
                    longLength);   
}


//*****************************************************************************
//
//! \brief Provides the East/West value from the GPS GGA Message.
//!
//! \param eastwest[] is an array in which East/West will be stored
//! \param ewLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char ew;
//!	getGGAGPSEastWest(&ew, 1);
//! if(strlen(ew) > 0)
//!     printf("%s\n",ew);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ew contains "W"
//! @endcode
//
//*****************************************************************************
void getGGAGPSEastWest(char *eastwest, char ewLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_EW, eastwest, ewLength);   
}

//*****************************************************************************
//
//! \brief Provides the Quality Indicator value from the GPS GGA Message.
//!
//! Quality can be:
//! 0 for invalid
//! 1 for GPS fix 
//! 2 for Differential GPS fix
//!
//! \param quality[] is an array in which the Quality Indicator string 
//!        will be stored
//! \param qualityLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char qind;
//!	getGGAGPSQualityInd(&qind, 1);
//! if(strlen(qind) > 0)
//!     printf("%s\n",qind);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! qind contains "1"
//! @endcode
//
//*****************************************************************************
void getGGAGPSQualityInd(char *quality, char qualityLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_QUALITY_IND, quality, 
                    qualityLength);   
}

//*****************************************************************************
//
//! \brief Provides the number of Used SVs from the GPS GGA Message.
//!
//! \param svs[] is an array in which the used SVs string 
//!        will be stored
//! \param svsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char numSVs[3];
//!	getGGAGPSNumUsedSVs(numSVs, 3);
//! if(strlen(numSVs) > 0)
//!     printf("%s\n",numSVs);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! numSVs contains "10"
//! @endcode
//
//*****************************************************************************
void getGGAGPSNumUsedSVs(char *svs, char svsLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_NUM_USED_SVS, svs, svsLength);   
}


//*****************************************************************************
//
//! \brief Provides the Horizontal Dilution Precision string from the GPS GGA Message.
//!
//! \param hordil[] is an array in which the returned string 
//!        will be stored
//! \param hordilLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char horDil[7];
//!	getGGAGPSHorDilPrec(horDil, 7);
//! if(strlen(horDil) > 0)
//!     printf("%s\n",horDil);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! horDil contains "1.0"
//! @endcode
//
//*****************************************************************************
void getGGAGPSHorDilPrec(char *hordil, char hordilLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_HOR_DIL_PREC, hordil, hordilLength);   
}

//*****************************************************************************
//
//! \brief Provides the Geoidal Step value from the GPS GGA Message.
//!
//! \param geoidal[] is an array in which the returned string 
//!        will be stored
//! \param geoidalLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char geoSep[7];
//!	getGGAGPSGeoidalSep(geoSep, 7);
//! if(strlen(geoSep) > 0)
//!     printf("%s\n",geoSep);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! geoSep contains "-24.8"
//! @endcode
//
//*****************************************************************************
void getGGAGPSGeoidalSep(char *geoidal, char geoidalLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_GEOIDAL_SEP, geoidal, geoidalLength);   
}

// TODO: What does this look like as far as output for age diff??
//*****************************************************************************
//
//! \brief Provides the Age difference value from the GPS GGA Message.
//!
//! Result in seconds
//!
//! \param age[] is an array in which the returned string 
//!        will be stored
//! \param ageLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char ageDiff[7];
//!	getGGAGPSGeoidalSep(ageDiff, 7);
//! if(strlen(ageDiff) > 0)
//!     printf("%s\n",ageDiff);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! ageDiff contains "1.1"
//! @endcode
//
//*****************************************************************************
void getGGAGPSAgeDiffData(char *age, char ageLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_AGE_DIFF_DATA, age, ageLength);   
}

// TODO: Diff ref ID here isn't provided
//*****************************************************************************
//
//! \brief Provides the Difference Reference ID value from the GPS GGA Message.
//!
//! \param id[] is an array in which the returned string 
//!        will be stored
//! \param idLength is the number of characters to return
//!
//! \return None.
//
//*****************************************************************************
void getGGAGPSDiffRefID(char *id, char idLength){
    
    parseGPSMessage(getGGAGPSMessage(), GGA_DIFF_REF_ID, id, idLength);   
}


//*****************************************************************************
//
//! \brief Provides the Speed over ground in km/hr from the GPS VTG 
//!	Message.
//!
//! \param speed[] is an array in which the returned string 
//!        will be stored
//! \param speedLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char speedKMH[7];
//!	getVTGGPSSpeedGndKmHrs(speedKMH, 7);
//! if(strlen(speedKMH) > 0)
//!     printf("%s\n",speedKMH);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! speedKMH contains "0.0"
//! @endcode
//
//*****************************************************************************
void getVTGGPSSpeedGndKmHrs(char *speed, char speedLength){
    
    parseGPSMessage(getVTGGPSMessage(), VTG_SPEED_GND_KMHRS, speed, 
                    speedLength);   
}			

// TODO: What is sample output
//*****************************************************************************
//
//! \brief Provides the Course Ground True value from the GPS VTG	Message.
//!
//! \param course[] is an array in which the returned string 
//!        will be stored
//! \param courseLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char crs[7];
//!	getVTGGPSCourseGndTrue(crs, 7);
//! if(strlen(crs) > 0)
//!     printf("%s\n",crs);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! crs contains ""
//! @endcode
//
//*****************************************************************************
void getVTGGPSCourseGndTrue(char *course, char courseLength){
    
    parseGPSMessage(getVTGGPSMessage(), VTG_COURSE_GND_TRUE, course, 
                    courseLength);   
}	

//TODO: What is a sample output here??
//*****************************************************************************
//
//! \brief Provides the Course Ground Magnitude value from the GPS VTG	Message.
//!
//! \param course[] is an array in which the returned string 
//!        will be stored
//! \param courseLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char cgm[7];
//!	getVTGGPSCourseGndMagn(cgm, 7);
//! if(strlen(cgm) > 0)
//!     printf("%s\n",cgm);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! cgm contains ""
//! @endcode
//
//*****************************************************************************
void getVTGGPSCourseGndMagn(char *course, char courseLength){
    
    parseGPSMessage(getVTGGPSMessage(), VTG_COURSE_GND_MAGN, course, 
                    courseLength);   
}	

//*****************************************************************************
//
//! \brief Provides the Speed over ground in Knots from the GPS VTG Message.
//!
//! \param speed[] is an array in which the returned string 
//!        will be stored
//! \param speedLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char speedGndknots[7];
//!	getVTGGPSSpeedGndKnots(speedGndknots, 7);
//! if(strlen(speedGndknots) > 0)
//!     printf("%s\n",speedGndknots);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! speedGndknots contains "10.5"
//! @endcode
//
//*****************************************************************************
void getVTGGPSSpeedGndKnots(char *speed, char speedLength){
    
    parseGPSMessage(getVTGGPSMessage(), VTG_SPEED_GND_KNOTS, speed, 
                    speedLength);   
}	

// TODO: What is sample mode value??
//*****************************************************************************
//
//! \brief Provides the GPS mode value from the GPS VTG Message.
//!
//! \param mode[] is an array in which the returned string 
//!        will be stored
//! \param modeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char mode;
//!	getVTGGPSMode(&mode, 1);
//! if(strlen(mode) > 0)
//!     printf("%s\n",mode);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! mode contains "A" 
//! @endcode
//
//*****************************************************************************
void getVTGGPSMode(char *mode, char modeLength){
    
    parseGPSMessage(getVTGGPSMessage(), VTG_MODE, mode, modeLength);   
}			

//
//*****************************************************************************
//
//! \brief Provides the fix mode string from the GPS GSA Message.
//!
//! Fix mode is either:
//! M  Manual, forced to operate in 2D or 3D 
//! A  Automatic, 3D/2D
//!
//! \param fixmode[] is an array in which the returned string 
//!        will be stored
//! \param fixmodeLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char fixMode;
//!	getGSAGPSFixMode(&fixMode, 1);
//! if(strlen(fixMode) > 0)
//!     printf("%s\n",fixMode);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! fixMode contains "M" 
//! @endcode
//
//*****************************************************************************
void getGSAGPSFixMode(char *fixmode, char fixmodeLength)
{
    
    parseGPSMessage(getGSAGPSMessage(), GSA_FIX_MODE, fixmode, fixmodeLength);   
}			

//*****************************************************************************
//
//! \brief Provides the fix status string from the GPS GSA Message.
//!
//! Fix status is either:
//! 1 - Fix not available
//! 2 - 2D
//! 3 - 3D
//!
//! \param fixstatus[] is an array in which the returned string 
//!        will be stored
//! \param fixstatusLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char fixStatus;
//!	getGSAGPSFixMode(&fixStatus, 1);
//! if(strlen(fixStatus) > 0)
//!     printf("%s\n",fixStatus);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! fixMode contains "3" 
//! @endcode
//
//*****************************************************************************
void getGSAGPSFixStatus(char *fixstatus, char fixstatusLength){
    
    parseGPSMessage(getGSAGPSMessage(), GSA_FIX_STATUS, fixstatus, 
                    fixstatusLength);   
}			


//*****************************************************************************
//
//! \brief Provides the 12 SVs PRNsfrom the GPS GSA Message.
//!        These are the SVs used in obtaining a position fix.
//!
//! \param ids[] is an array in which the returned string 
//!        will be stored
//! \param numIDs is the number of characters to return for each SV PRN.
//!        This is usually 3 (2 for 00-99 and 1 for null string terminator)
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char SVsIDs[12][3];
//! getGSAGPSFixMode(&SVsIDs, 3);
//! unsigned int i = 0;
//! for(i = 0; i < 3; i++)
//!     printf("SV: %s\n",SVsIDs[i]);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! SV: 12
//! SV: 29
//! SV: 30
//! @endcode
//
//*****************************************************************************
void getGSAGPSSVsIDs(char ids[][GSA_SV_CHAR_LEN], char numIDs){
    char i;
    char k = 0;   

    for(i = GSA_SV_IDS_FIRST; i < GSA_SV_IDS_LAST; i++)
    {
        if(k < numIDs) // Return up to what the caller specified
        {
            parseGPSMessage(getGSAGPSMessage(), i, &ids[k][0], GSA_SV_CHAR_LEN);
            k++;
        }        
    }
}

//*****************************************************************************
//
//! \brief Provides the PDOP string from the GPS GSA Message.
//!
//! \param pdop[] is an array in which the returned string 
//!        will be stored
//! \param pdopLength is the number of characters to return
//!
//! \return None.
//
//*****************************************************************************
void getGSAGPSPDOP(char *pdop, char pdopLength){
    
    parseGPSMessage(getGSAGPSMessage(), GSA_PDOP, pdop, pdopLength);   
}			

//*****************************************************************************
//
//! \brief Provides the HDOP string from the GPS GSA Message.
//!
//! \param hdop[] is an array in which the returned string 
//!        will be stored
//! \param hdopLength is the number of characters to return
//!
//! \return None.
//
//*****************************************************************************
void getGSAGPSHDOP(char *hdop, char hdopLength){
    
    parseGPSMessage(getGSAGPSMessage(), GSA_HDOP, hdop, hdopLength);   
}			

//*****************************************************************************
//
//! \brief Provides the VDOP string from the GPS GSA Message.
//!
//! \param vdop[] is an array in which the returned string 
//!        will be stored
//! \param vdopLength is the number of characters to return
//!
//! \return None.
//
//*****************************************************************************
void getGSAGPSVDOP(char *vdop, char vdopLength){
    
    parseGPSMessage(getGSAGPSMessage(), GSA_VDOP, vdop, vdopLength);   
}






//*****************************************************************************
//
//! \brief Provides the total number of GPS GSV Messages received.
//!
//! \param totmsgs is an array in which the returned string 
//!        will be stored
//! \param totmsgsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char gsvTotMsgs[2];
//! getGSVGPSTotalMsgs(&gsvTotMsgs, sizeof(gsvTotMsgs));
//! if(strlen(gsvTotMsgs) > 0)
//!     printf("%s\n",gsvTotMsgs);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! gsvTotMsgs contains "2" 
//! @endcode
//
//*****************************************************************************
void getGSVGPSTotalMsgs(char *totmsgs, char totmsgsLength) {
    
    parseGPSMessage(getGSVGPSMessage(0), GSV_TOT_MSGS, totmsgs, totmsgsLength);
}

//*****************************************************************************
//
//! \brief Provides the total number of satellites in view.
//!
//! \param numsvs is an array in which the returned string 
//!        will be stored
//! \param numsvsLength is the number of characters to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char viewSVs[2];
//! getGSVGPSViewSVs(viewSVs, 2);
//! if(strlen(viewSVs) > 0)
//!     printf("%s\n",viewSVs);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! viewSVs contains "08" 
//! @endcode
//
//*****************************************************************************
void getGSVGPSViewSVs(char *numsvs, char numsvsLength){
    
    parseGPSMessage(getGSVGPSMessage(0), GSV_NUM_VIEW_SVS, numsvs, 
                    numsvsLength);   
}	

//*****************************************************************************
//
//! \brief Provides an array of Elevations of the satellites in view.
//!
//! Elevations are in degrees, with a maximum of 90 degrees.
//!
//! Note: Unlike many of the other functions, this function returns an array of
//!       numbers, not strings.
//!
//! \param elevations is an array of \b chars in which the returned values 
//!        will be stored
//! \param elevationsLength is the number of values to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char viewSVs[2];
//! char svNum = 0;
//! getGSVGPSViewSVs(viewSVs, 2);
//! svNum = atoi(viewSVs);
//! char svElevs[20];
//! printf("# of SVs: %d\n",svNum);
//! getGSVGPSSVsElevations(svElevs, svNum);
//! int i;
//! for(i = 0; i < svNum; i++)
//!	    printf("%d,",svElevs[i]);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! # of SVs: 12
//! 14,14,67,13,69,10,6,9,6,33,43,12
//! @endcode
//
//*****************************************************************************
void getGSVGPSSVsElevations(char elevations[][3], char numElevation){
    
    char strTotMessages[2];
    char totMsg = 0;
    char numSats = 0;  
    char i;
    char j;    	
    
    getGSVGPSTotalMsgs(strTotMessages, sizeof(strTotMessages));
    totMsg = atoi(strTotMessages); 	
    
    for(i = 0; i < totMsg; i++) // Iterate through all GSV Messages
    {
       
        for(j = 0; j < GSV_SV_MAX_PER_MSG; j++) // Iterate through all SVs 
            // in a msg
        {   				
            if(numSats < numElevation) // Return up to what the caller specified
            {
                char tempElev[GSV_SV_ELEV_MAX_LEN+1];
                char pos = GSV_SV_ELEV_START + (GSV_SV_NEXT_OFFSET*j);  
                
                parseGPSMessage(getGSVGPSMessage(i), pos, tempElev, 
                                GSV_SV_ELEV_MAX_LEN+1);
                
                memcpy(&elevations[numSats][0],tempElev,GSV_SV_ELEV_MAX_LEN);
                elevations[numSats][2] = '\0';
                numSats++;   			   			
            }
        }
    }
}	

//*****************************************************************************
//
//! \brief Provides an array of Azimuths of the satellites in view.
//!
//! Azimuths are in degrees from true north, 0 to 359.
//!
//!
//! \param azimuths is an array of \b ints in which the returned values 
//!        will be stored
//! \param numAzim is the number of values to return
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char viewSVs[2];
//! int svAzims[20];
//! char svNum = 0;
//! getGSVGPSViewSVs(viewSVs, 2);
//! svNum = atoi(viewSVs);
//! printf("# of SVs: %d\n",svNum);
//! getGSVGPSSVsAzimuths(svAzims, svNum);
//! int i;
//! for(i = 0; i < svNum; i++)
//!	    printf("%d,",svAzims[i]);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! # of SVs: 12
//! 39,117,334,268,52,315,228,158,322,194,96,11
//! @endcode
//
//*****************************************************************************
void getGSVGPSSVsAzimuths(char azimuths[][4], char numAzim){
    
    char strTotMessages[2];
    char totMsg = 0;
    char numSats = 0;  
    char i;
    char j;    	
    
    getGSVGPSTotalMsgs(strTotMessages, sizeof(strTotMessages));
    totMsg = atoi(strTotMessages); 	
    
    //
    // Iterate through all GSV Messages
    //
    for(i = 0; i < totMsg; i++) 
    {        
        for(j = 0; j < GSV_SV_MAX_PER_MSG; j++) // Iterate through all 
                // SVs in a msg
        {   				
            if(numSats < numAzim) // Return up to what the caller specified
            {
                char azim[GSV_SV_AZI_MAX_LEN+1];
                char pos = GSV_SV_AZI_START + (GSV_SV_NEXT_OFFSET*j);  
                
                parseGPSMessage(getGSVGPSMessage(i), pos, azim, 
                                GSV_SV_AZI_MAX_LEN+1);
                
                memcpy(&azimuths[numSats][0],azim,GSV_SV_AZI_MAX_LEN+1);
                azimuths[numSats][3] = '\0';
                numSats++;   			   			
            }
        }
    }  
}	

//*****************************************************************************
//
//! \brief Provides an array of SV IDs of the satellites in view.
//!
//! ID is the PRN of the Satellite Vehicle
//!
//! \param ids is an array of strings in which the returned values 
//!        will be stored
//! \param numIds is the number of ids to return.
//!
//! \note The number of IDs returned can never be above the maximum received in
//!       the NMEA sentence.
//! 
//! \return None.
//!
//! \par Example usage:
//! @code
//! char viewSVs[2];
//! char svIDs[20];
//! char svNum = 0;
//! getGSVGPSViewSVs(viewSVs, 2);
//! svNum = atoi(viewSVs);
//! printf("# of SVs: %d\n",svNum);
//! getGSVGPSSVsIDs(svIDs, svNum);
//! int i;
//! for(i = 0; i < svNum; i++)
//!	    printf("%d,",svIDs[i]);
//! @endcode
//!
//! \par Example Output: 
//! @code
//! # of SVs: 12
//! 9,25,14,19,22,11,6,21,24,31,18,31
//! @endcode
//
//*****************************************************************************
void getGSVGPSSVsIDs(char ids[][3], char numIds){
    
    char strTotMessages[2];
    char totMsg = 0;
    char numSats = 0;  
    char i;
    char j;    	
    getGSVGPSTotalMsgs(strTotMessages, sizeof(strTotMessages));
    totMsg = atoi(strTotMessages); 	
    
    //
    // Iterate through all GSV Messages
    //
    for(i = 0; i < totMsg; i++) 
    {
         				
        for(j = 0; j < GSV_SV_MAX_PER_MSG; j++) // Iterate through all 
            // SVs in a msg
        {
             if(numSats < numIds) // Return up to what the caller specified
            { 
                char tempId[GSV_SV_ID_MAX_LEN+1];
                char pos = GSV_SV_ID_START + (GSV_SV_NEXT_OFFSET*j);  
                
                // +1 for GSV_S
                parseGPSMessage(getGSVGPSMessage(i), pos, tempId, 
                                GSV_SV_ID_MAX_LEN+1);
                
                memcpy(&ids[numSats][0],tempId,GSV_SV_ID_MAX_LEN);
                ids[numSats][2] = '\0';
                numSats++;   
            }
        }
        
    }  
}

//*****************************************************************************
//
//! \brief Provides the SNRs of the satellites in view.
//!
//! SNR is a string of two digits, indicating SNR in dB from 0-99.
//! Note that the order in which values are provided is the same as in the
//! message, which coincides with other functions.
//!
//! \param snrs is an array of strings in which the returned values 
//!        will be stored
//! \param numSVs is the number of SNRs to return. 
//!
//! \return None.
//!
//! \par Example usage:
//! @code
//! char viewSVs[3];
//! char svSNRs[20];
//! char svNum = 0;
//! getGSVGPSViewSVs(viewSVs, 3);
//! svNum = atoi(viewSVs);
//! printf("# of SVs: %d\n",svNum);
//! getGSVGPSSVsSNRs(svSNRs, 12);
//! int i;
//! for(i = 0; i < svNum; i++)
//!	    printf("%d,",atoi(svIDs[i]));
//! @endcode
//!
//! \par Example Output: 
//! @code
//! # of SVs: 12
//! 30,27,38,35,35,31,21,27,29,36,31,34
//! @endcode
//
//*****************************************************************************
void getGSVGPSSVsSNRs(char snrs[][3], char numSVs){
    
    char strTotMessages[2];
    char totMsg = 0;
    char numSats = 0;  
    char i;
    char j;    	
    
    getGSVGPSTotalMsgs(strTotMessages, sizeof(strTotMessages));
    totMsg = atoi(strTotMessages); 	
    
    //
    // Iterate through all GSV Messages
    //
    for(i = 0; i < totMsg; i++) 
    {           				
        for(j = 0; j < GSV_SV_MAX_PER_MSG; j++) // Iterate through all 
            // SVs in a msg
        {
            if(numSats < numSVs) // Only return SVs that numSVs specifies
            {
                char tempSNR[GSV_SV_SNR_MAX_LEN+1];
                char pos = GSV_SV_SNR_START + (GSV_SV_NEXT_OFFSET*j);  
                
                parseGPSMessage(getGSVGPSMessage(i), pos, tempSNR, 
                                GSV_SV_SNR_MAX_LEN+1);
                
                memcpy(&snrs[numSats][0],tempSNR,GSV_SV_ID_MAX_LEN);
                snrs[numSats][2] = '\0';
                numSats++;   			   			
            }
        }
    }
}	
