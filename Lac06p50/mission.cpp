/*
    LAC (Linux Air combat)
    Copyright 2015 by Robert J. Bosen. Major portions of
    this code were derived from "gl-117", by Thomas A. Drexl and
    other contributors, who are mentioned in the "Credits" menu.

    This file is part of LAC.

    LAC is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    LAC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with LAC; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>
#include "mission.h"
#include "common.h"
#include "main.h"
#include "mathtab.h"
#include "conf.h"
#include "glland.h"
#include "aiobject.h"
#include "NetworkApi.h"

#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


extern bool AirfieldRepairRateNormalForBlueTeam;
extern bool AirfieldRepairRateNormalForRedTeam;
extern bool AirfieldRepairsAcceleratedForBlueTeam;
extern bool AirfieldRepairsAcceleratedForRedTeam ;
extern bool AirfieldRepairsStoppedForBlueTeam;
extern bool AirfieldRepairsStoppedForRedTeam;
extern bool AutoPilotActive;
extern bool ClearSpeedHistoryArrayFlag;
extern bool LandedAtSafeSpeed;
extern bool MissionNetworkBattle01BlueTeamVictory;
extern bool MissionNetworkBattle01RedTeamVictory;
extern bool MissionNetworkBattle02BlueTeamVictory;
extern bool MissionNetworkBattle02RedTeamVictory;
extern bool MissionNetworkBattle03BlueTeamVictory;
extern bool MissionNetworkBattle03RedTeamVictory;
extern bool MissionRunning;
extern bool NetworkPlayerKilled;
extern bool NewSystemMessageNeedsScrolling;
extern bool NoMissionHasYetCommenced;
extern bool PeerPacketReceivedInThisMissionState;
extern bool RadarReflectorBlueHasBeenDestroyedAtLeastOnce;
extern bool RadarReflectorRedHasBeenDestroyedAtLeastOnce;
extern bool WindNoiseOn;
extern char DebugBuf[];
extern char MissionEntryLatch[];
extern char SystemMessageBufferA[];
extern unsigned char AirfieldChosenForLanding;
extern unsigned char AirfieldRequested;
extern unsigned char AirfieldState;
extern unsigned char DefaultAircraft;
extern unsigned char MaxPlayersInCurrentMission;
extern unsigned char MissionHeadToHead00State;
extern unsigned char MissionIdNegotiationCount;
extern unsigned char MissionOutPacketCount;
extern unsigned char MyNetworkId;
extern unsigned char NetworkApiVersion;
extern unsigned char NetworkOpponent;
extern unsigned char MissionStateNetworkBattle01;
extern unsigned char MissionStateNetworkBattle02;
extern unsigned char MissionStateNetworkBattle03;

extern int AirfieldXCenter;
extern int AirfieldXMax;
extern int AirfieldXMin;
extern int AirfieldYCenter;
extern int AirfieldYMax;
extern int AirfieldYMin;
extern int BattleDamageRiskTimer;
extern int HudLadderBarsOnOff;
extern int HudOnOff;
extern int IffOnOff;
extern int lastDurability;
extern int MissionAircraftDamaged;
extern int MapViewOnOff;
extern int MissionNetworkBattle01PriorAircraft[];
extern int MissionNetworkBattle02PriorAircraft[];
extern int MissionNetworkBattle03PriorAircraft[];
extern int NetworkMode; // Mode 0 = PeerToPeer. Mode 1 = ClientServer.
extern int NetworkReceiveTimer;
extern int NetworkReceiveTimerInterval;
extern int NetworkTransmitTimer;
extern int NetworkTransmitTimerInterval;
extern int PlayerAircraftType;
extern int PlayersOriginalDurability;
extern int PriorStateEndTimer;
extern int RadarOnOff;
extern int RadarWasOn;
extern int RadarZoom;
extern int StateTransitionTimer;
extern int StaticObjectUpdateTimer;
extern int TrimElevatorSetting;
extern int TrimRudderSetting;
extern unsigned int key_DROPFLARE;
extern unsigned int key_DROPCHAFF;
extern unsigned int key_PRIMARY;
extern unsigned int key_SECONDARY;
extern unsigned int key_TARGETNEXT;
extern unsigned int key_TARGETPREVIOUS;
extern unsigned int key_WEAPONSELECT;
extern unsigned int MissedPacketCount;

extern Uint32 DeltaTime;
extern Uint32 Me163LandingTimer;
extern Uint32 MissionEndingTimer;
extern Uint32 MissionEndingTimer2;
extern Uint32 MissionNetworkBattle01Timer;
extern Uint32 MissionNetworkBattle02Timer;
extern Uint32 MissionNetworkBattle03Timer;
extern Uint32 MissionNetworkBattleRadarTimer;
extern Uint32 NetworkApiPriorTimer[32];

extern float AutoPilotAltitude;
extern float blackout;
extern float CalculatedDamageDueToCurrentRisk;
extern float DamageToClaim;
extern float InPacketDeltaXPosition[];
extern float InPacketDeltaYPosition[];
extern float InPacketDeltaZPosition[];
extern float MaxGammaPenaltyPerWeapon;
extern float NetDeltaX;
extern float NetDeltaY;
extern float NetDeltaZ;

extern float NetworkApiPriorXPosition[];
extern float NetworkApiPriorYPosition[];
extern float NetworkApiPriorZPosition[];
extern float PlayersOriginalRollRate;
extern float PlayersOriginalMaxGamma;
extern float PlayersOriginalMaxThrust;
extern float redout;
extern float ScreenFOVx;
extern float ScreenFOVy;
extern float SeaLevel;

extern Uint32 NetworkApiPriorTimer[32];

extern LacUdpApiPacket InPacket;

extern int  GetNetworkApiPacket();
extern int  OpenClientUdpSocket();
extern int  OpenUdpSocketForReceiving();
extern int  OpenUdpSocketForSending();

extern void ConfigureClientUdpSocket();
extern void ConfigureIncomingUdpSocket();
extern void ConfigureOutgoingUdpSocket();
extern int DiscardAnyInPacketsInQueue();
extern void event_HudLadderBarsOnOff();
extern void event_IffOnOff();
extern void event_MapViewOnOff();
extern void event_MapZoomIn();
extern void event_RadarOnOff();
extern void event_RadarZoomIn();
extern void event_RadarZoomOut();
extern void event_targetNext ();
extern void event_thrustUp ();
extern void event_ToggleUndercarriage();
extern void event_TrimElevatorDn();
extern void event_TrimElevatorUp();
extern void event_ZoomFovOut();
extern void game_quit ();
extern void LoadServerIpAddress();
extern void LoadVariablesFromNetworkApiPacket();
extern void SendNetworkApiPacket();
extern void UpdateOnlineScoreLogFileWithCalculatedRisks();
extern void UpdateOnlineScoreLogFileWithNewSorties();

bool MissionHeadToHead00RetrieveFirstDamageDescription();
bool MissionNetworkBattle01RetrieveFirstDamageDescription();
bool MissionNetworkBattle02RetrieveFirstDamageDescription();
bool MissionNetworkBattle03RetrieveFirstDamageDescription();
void event_FlapsDN();
void MissionHeadToHead00LoadVariablesFromNetworkApiPacket(int);
void MissionNetworkBattle01LoadVariablesFromNetworkApiPacket(int);
void MissionNetworkBattle02LoadVariablesFromNetworkApiPacket(int);
void MissionNetworkBattle03LoadVariablesFromNetworkApiPacket(int);

void ArmPlayerAtRequestedField()
{
display ("Function Entry: ArmPlayerAtRequestedField()", LOG_MOST);
sprintf (DebugBuf, "ArmPlayerAtRequestedField() AirfieldRequested = %d\n", AirfieldRequested);
display (DebugBuf, LOG_MOST);
sprintf (DebugBuf, "fplayer->missiles[0] = %d\n", fplayer->missiles[0]);
display (DebugBuf, LOG_MOST);
if (MyNetworkId %2)
   {
   display ("ArmPlayerAtRequestedField() RedTeam", LOG_MOST);
   fplayer->party = 1;
   if (AirfieldRequested == 1)
      {
      display ("ArmPlayerAtRequestedField() RedTeam Field 1", LOG_MOST);
      display ("SPAWNING AT RED HQ (29)", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         {
         fplayer->missiles [0] *= 0.50; // Cut bombload to 50% from this field.
         }
      fplayer->tl->x = 300;
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested == 2)
      {
      display ("ArmPlayerAtRequestedField() RedTeam Field 2", LOG_MOST);
      display ("SPAWNING AT RED FIELD2", LOG_MOST);
      fplayer->tl->x = 800;
      fplayer->tl->z = 5.00;
      if (fplayer->missiles [0] >= 6)
         {
         fplayer->missiles [0] *= 0.75; // Cut bombload to 75% from this field.
         }
      }
   if (AirfieldRequested > 2)
      {
      display ("ArmPlayerAtRequestedField() RedTeam Field > 2", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         {
         AirfieldRequested = 4;
         display ("SPAWNING AT RED FIELD4", LOG_MOST);
         fplayer->tl->x = 1800;
         fplayer->tl->z = 5.00;
         }
      else
         {
         AirfieldRequested = 3;
         display ("SPAWNING AT RED FIELD3", LOG_MOST);
         fplayer->tl->x = 1300;
         fplayer->tl->z = 5.00;
         }
      }
   }
else
   {
   display ("ArmPlayerAtRequestedField() BlueTeam", LOG_MOST);
   fplayer->party = 0;
   if (AirfieldRequested == 1)
      {
      display ("ArmPlayerAtRequestedField() BlueTeam Field 1", LOG_MOST);
      display ("SPAWNING AT BLUE HQ (28)", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         {
         fplayer->missiles [0] *= 0.50; // Cut bombload to 50% from this field.
         }
      fplayer->tl->x = -400;
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested == 2)
      {
      display ("ArmPlayerAtRequestedField() BlueTeam Field 2", LOG_MOST);
      display ("SPAWNING AT BLUE FIELD2", LOG_MOST);
      if (fplayer->missiles [0] >= 6)
         {
         fplayer->missiles [0] *= 0.75; // Cut bombload to 75% from this field.
         }
      fplayer->tl->x = -900;
      fplayer->tl->z = 5.00;
      }
   if (AirfieldRequested > 2)
      {
      display ("ArmPlayerAtRequestedField() BlueTeam Field > 2", LOG_MOST);
      if (fplayer->missiles[0] >= 6)
         {
         AirfieldRequested = 4;
         display ("SPAWNING AT BLUE FIELD4", LOG_MOST);
         fplayer->tl->x = -1900;
         fplayer->tl->z = 5.00;
         }
      else
         {
         AirfieldRequested = 3;
         display ("SPAWNING AT BLUE FIELD3", LOG_MOST);
         fplayer->tl->x = -1400;
         fplayer->tl->z = 5.00;
         }
      }
   }
}

void AutoPilot()
{
if (AutoPilotActive)
   {
   float AltitudeDifferential = ThreeDObjects[0]->tl->y - AutoPilotAltitude;
   if (fabs(AltitudeDifferential) < 10)
      {
      TrimElevatorSetting /=2;
      }
   float GammaDifferential = ThreeDObjects[0]->gamma - 180;
   if (fabs(GammaDifferential) < 1)
      {
      TrimElevatorSetting /=2;
      }
   if (AltitudeDifferential < 4)
      {
      TrimElevatorSetting += (int)(AltitudeDifferential * -20);
      }
   if (AltitudeDifferential > 4)
      {
      TrimElevatorSetting -= (int)(AltitudeDifferential * 20);
      }

   if (TrimElevatorSetting > 2000)
      {
      TrimElevatorSetting = 2000;
      }
   if (TrimElevatorSetting < -2000)
      {
      TrimElevatorSetting = -2000;
      }

   if (ThreeDObjects[0]->gamma > 190)
      {
      ThreeDObjects[0]->gamma = 190;
      TrimElevatorSetting /=2;
      }
   if (ThreeDObjects[0]->gamma < 170)
      {
      ThreeDObjects[0]->gamma = 170;
      TrimElevatorSetting /=2;
      }

   if (ThreeDObjects[0]->theta > 5 )
      {
      ThreeDObjects[0]->theta -=2;
      }
   else if (ThreeDObjects[0]->theta < -5)
      {
      ThreeDObjects[0]->theta +=2;
      }
   if (ThreeDObjects[0]->theta > 2)
      {
      ThreeDObjects[0]->theta -= 0.5;
      }
   else if (ThreeDObjects[0]->theta < -2)
      {
      ThreeDObjects[0]->theta += 0.5;
      }
   if (TrimRudderSetting != 0)
      {
      ThreeDObjects[0]->theta = TrimRudderSetting * -0.0005;
      }
   }
else
   {
   TrimElevatorSetting = 0;
   }
}

void CalcDamageRiskFromNearbyOpposition()
{
display ("CalcDamageRiskFromNearbyOpposition()", LOG_ALL);
static float CalculatedDamageFromAirfields = 0;
static float CalculatedDamageFromAircraft[11] = {0};
unsigned char Mission3dObject;
float XDisplacementTemp;
float YDisplacementTemp;
float ZDisplacementTemp;
float TotalXYZDisplacement;
float HostileDamageState;
float JinkingStrength;
CalculatedDamageDueToCurrentRisk = 0;
for (Mission3dObject = 1; Mission3dObject<=29; Mission3dObject++)
   {
   if (ThreeDObjects[Mission3dObject]->active == true && Mission3dObject%2 != MyNetworkId%2)
      {
      XDisplacementTemp = fabs (fplayer->tl->x - ThreeDObjects[Mission3dObject]->tl->x);
      if (XDisplacementTemp < 90.0)
         {
         ZDisplacementTemp = fabs (fplayer->tl->z - ThreeDObjects[Mission3dObject]->tl->z);
         if (ZDisplacementTemp < 90.0)
            {
            YDisplacementTemp = fabs (fplayer->tl->y - ThreeDObjects[Mission3dObject]->tl->y);
            if (YDisplacementTemp < 200)
               {
               TotalXYZDisplacement = XDisplacementTemp + YDisplacementTemp + ZDisplacementTemp;
               if (TotalXYZDisplacement < 5.0)
                  {
                  TotalXYZDisplacement = 5.0;
                  }
               if (Mission3dObject == 28 or Mission3dObject == 29)
                  {

                  CalculatedDamageFromAirfields = 600/TotalXYZDisplacement;
                  if (fplayer->id == BOMBER_B17 || fplayer->id == BOMBER_B24 || fplayer->id == BOMBER_G5M || fplayer->id == BOMBER_LANCASTER || fplayer->id == BOMBER_B29)
                     {
                     CalculatedDamageFromAirfields *= 5; // Big Bombers get hit more often.
                     display ("CalcDamageRiskFromNearbyOpposition() 5x Heavy bomber penalty.", LOG_MOST);
                     }
                  else if (fplayer->missiles [0] > 3 && fplayer->missiles [0] < 7)
                     {
                     CalculatedDamageFromAirfields *=3; // Medium bombers are also easier to hit.
                     display ("CalcDamageRiskFromNearbyOpposition() 3x Medium bomber penalty.", LOG_MOST);
                     }
                  int volume = (int)(CalculatedDamageFromAirfields * 36) -110;
                  if (volume > 127)
                     {
                     volume = 127;
                     }

                  HostileDamageState = ThreeDObjects[Mission3dObject]->Durability / ThreeDObjects[Mission3dObject]->maxDurability;
                  CalculatedDamageFromAirfields *= HostileDamageState;
                  sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Damage due to airfield proximity and damage state = %f", CalculatedDamageFromAirfields);
                  display (DebugBuf, LOG_MOST);
                  if (HostileDamageState > 0.4)
                     {
                     sound->setVolume (SOUND_BEEP1, volume);
                     sound->play (SOUND_BEEP1, false);
                     }
                  else
                     {
                     CalculatedDamageFromAirfields *= .30;
                     }

                  if (blackout > 0)
                     {
                     JinkingStrength = blackout *2;
                     if (JinkingStrength > 10.0)
                        {
                        JinkingStrength = 10.0;
                        CalculatedDamageFromAirfields = 0.0;
                        display ("CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                        }
                     if (JinkingStrength > 1.0)
                        {
                        CalculatedDamageFromAirfields /= JinkingStrength;
                        display ("CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                        }
                     }
                  else if (redout > 0)
                     {
                     JinkingStrength = redout * 4;
                     if (JinkingStrength > 10.0)
                        {
                        JinkingStrength = 10.0;
                        CalculatedDamageFromAirfields = 0.0;
                        display ("CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                        }
                     if (JinkingStrength > 1.0)
                        {
                        CalculatedDamageFromAirfields /= JinkingStrength;
                        display ("CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                        }
                     }

                  if (myrandom(100) > 50)
                     {
                     CalculatedDamageFromAirfields = 0.0;
                     display ("CalcDamageRiskFromNearbyOpposition() myrandom() helped.", LOG_MOST);
                     }

                  if (JinkingStrength >= 10.0)
                     {
                     CalculatedDamageFromAirfields = 0.0;
                     }

                  if (fplayer->realspeed > .32)
                     {
                     if (myrandom(100) > 20)
                        {
                        CalculatedDamageFromAirfields = 0;
                        display ("CalcDamageRiskFromNearbyOpposition() Airfield guns missed due to our high speed.", LOG_MOST);
                        }
                     }
                  }

               if (
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B17)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B24)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_JU87)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_G5M)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B25)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B26)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_LANCASTER) ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B29)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_B5N)       ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_DAUNTLESS) ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_DORNIER)   ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_HE111)     ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_JU88)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_SB2C)      ||
                   (ThreeDObjects[Mission3dObject]->id == BOMBER_TBF)       ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_D3A)      ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_ME110)    ||
                   (ThreeDObjects[Mission3dObject]->id == FIGHTER_IL2)
                  )
                  {
                  sound->setVolume (SOUND_BEEP1, 40);
                  sound->play (SOUND_BEEP1, false);

                  if (XDisplacementTemp < 30 && YDisplacementTemp < 30 && ZDisplacementTemp < 30)
                     {
                     CalculatedDamageFromAircraft[Mission3dObject] = 100/TotalXYZDisplacement; // In range
                     int volume = (int)(CalculatedDamageFromAircraft[Mission3dObject] * 36)-110;
                     if (volume > 127)
                        {
                        volume = 127;
                        }
                     sound->setVolume (SOUND_BEEP1, volume);
                     sound->play (SOUND_BEEP1, false);

                     float PhiDifferential = fabs(fplayer->phi - ThreeDObjects[Mission3dObject]->phi);
                     float GammaDifferential = fabs(fplayer->gamma - ThreeDObjects[Mission3dObject]->gamma);
                     sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Damage due to bomber proximity = %f from bomber %d", CalculatedDamageFromAircraft[Mission3dObject], Mission3dObject);
                     display (DebugBuf, LOG_MOST);
                     if (PhiDifferential < 14.0 && GammaDifferential < 6.0)
                        {
                        CalculatedDamageFromAircraft[Mission3dObject] *= 2.0; // Serious hits are far more likely in this circumstance!
                        display ("CalcDamageRiskFromNearbyOpposition() 2x Damage Penalty!", LOG_MOST);
                        }
                     else if (PhiDifferential > 80 || GammaDifferential > 20)
                        {
                        CalculatedDamageFromAircraft[Mission3dObject] /= 3.0;
                        display ("CalcDamageRiskFromNearbyOpposition() Radical intercept helped.", LOG_MOST);
                        }

                     HostileDamageState = ThreeDObjects[Mission3dObject]->Durability / ThreeDObjects[Mission3dObject]->maxDurability;
                     CalculatedDamageFromAircraft[Mission3dObject] *= HostileDamageState;

                     if (blackout > 0)
                        {
                        JinkingStrength = blackout * 3;
                        if (JinkingStrength > 10.0)
                           {
                           JinkingStrength = 10.0;
                           CalculatedDamageFromAircraft[Mission3dObject] = 0.0;
                           }
                        if (JinkingStrength > 1.0)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] /= JinkingStrength;
                           display ("CalcDamageRiskFromNearbyOpposition() +Jinking helped.", LOG_MOST);
                           }
                        }
                     else if (redout > 0)
                        {
                        JinkingStrength = redout * 5;
                        if (JinkingStrength > 10.0)
                           {
                           JinkingStrength = 10.0;
                           CalculatedDamageFromAircraft[Mission3dObject] = 0.0;
                           }
                        if (JinkingStrength > 1.0)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] /= JinkingStrength;
                           display ("CalcDamageRiskFromNearbyOpposition() -Jinking helped.", LOG_MOST);
                           }
                        }

                     if (myrandom(100) > 50)
                        {
                        CalculatedDamageFromAircraft[Mission3dObject] = 0.0;
                        display ("CalcDamageRiskFromNearbyOpposition() myrandom() helped.", LOG_MOST);
                        }

                     if (fplayer->realspeed > (ThreeDObjects[Mission3dObject]->realspeed * 1.3) )
                        {
                        if (myrandom(100) > 20)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] = 0;
                           display ("CalcDamageRiskFromNearbyOpposition() Bomber guns missed due to our high speed.", LOG_MOST);
                           }
                        }

                     if (fplayer->realspeed > (ThreeDObjects[Mission3dObject]->realspeed * 1.5) )
                        {
                        if (myrandom(100) > 10)
                           {
                           CalculatedDamageFromAircraft[Mission3dObject] = 0;
                           display ("CalcDamageRiskFromNearbyOpposition() Bomber guns missed due to our high speed.", LOG_MOST);
                           }
                        }
                     if (ThreeDObjects[Mission3dObject]->tl->y < 25)
                        {
                        CalculatedDamageFromAircraft[Mission3dObject] = 0;
                        display ("CalcDamageRiskFromNearbyOpposition() Bomber altitude too low to fire guns.", LOG_MOST);
                        }
                     }
                  else
                     {
                     CalculatedDamageFromAircraft[Mission3dObject] = 0.0; // Out of range
                     }

                  float LethalityFactor = ((float)(ThreeDObjects[Mission3dObject]->DefensiveLethality) / 39.0);
                  CalculatedDamageFromAircraft[Mission3dObject] *= LethalityFactor;
                  } // end of section handling bombers in the general vicinity
               } // end of logic checking x, y, and z displacements
            }
         }
      }
   }
CalculatedDamageDueToCurrentRisk = CalculatedDamageFromAirfields;
if (CalculatedDamageFromAirfields > 0.0)
   {
   sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Airfield damaged us by %f", CalculatedDamageFromAirfields);
   display (DebugBuf, LOG_MOST);
   CalculatedDamageFromAirfields = 0;
   }
for (Mission3dObject = 1; Mission3dObject <=10; Mission3dObject++)
   {
   if (CalculatedDamageFromAircraft[Mission3dObject] > 0.0)
      {
      CalculatedDamageDueToCurrentRisk += CalculatedDamageFromAircraft[Mission3dObject];

      sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Bomber %d damaged us by %f", Mission3dObject, CalculatedDamageFromAircraft[Mission3dObject]);
      display (DebugBuf, LOG_MOST);
      CalculatedDamageFromAircraft[Mission3dObject] = 0;
      }
   }
if (CalculatedDamageDueToCurrentRisk > 0 && CalculatedDamageDueToCurrentRisk < 0.22)
   {
   display ("CalcDamageRiskFromNearbyOpposition() Discarding negligible value of CalculatedDamageDueToCurrentRisk.", LOG_MOST);
   CalculatedDamageDueToCurrentRisk = 0;
   }
if (CalculatedDamageDueToCurrentRisk > 0.0)
   {

   for (Mission3dObject = 1; Mission3dObject<=10; Mission3dObject++)
      {
      if (ThreeDObjects[Mission3dObject]->active == true)
         {
         if (Mission3dObject%2 == MyNetworkId%2)
            {
            if ((ThreeDObjects[Mission3dObject]->id >FIGHTER1) && (ThreeDObjects[Mission3dObject]->id <FIGHTER2))
               {
               float XDisplacementTemp1 = fabs (fplayer->tl->x - ThreeDObjects[Mission3dObject]->tl->x);
               float YDisplacementTemp1 = fabs (fplayer->tl->y - ThreeDObjects[Mission3dObject]->tl->y);
               float ZDisplacementTemp1 = fabs (fplayer->tl->z - ThreeDObjects[Mission3dObject]->tl->z);
               if (XDisplacementTemp1 < 40 && YDisplacementTemp1 < 40 && ZDisplacementTemp1 < 40)
                  {
                  CalculatedDamageDueToCurrentRisk *= 0.5; // A nearby ally shares our damage risk.
                  display ("CalcDamageRiskFromNearbyOpposition() Ally sharing helped.", LOG_MOST);
                  }
               else
                  {
                  ; // Make no adjustment if this aircraft is too far away to share damage risk.
                  }
               }
            }
         }
      }
   }
if (CalculatedDamageDueToCurrentRisk > 0.0)
   {
   sprintf (DebugBuf, "CalcDamageRiskFromNearbyOpposition() Total CalculatedDamageDuetoCurrentRisk = %f", CalculatedDamageDueToCurrentRisk);
   display (DebugBuf, LOG_MOST);
   }
}

void ConfigureOrdnanceForOnlineMissions()
{
int i2;
for (i2 = 0; i2 < missiletypes; i2 ++)
    {
    fplayer->missiles [i2] = 4;
    }
if (
    fplayer->id == FIGHTER_P38L      ||
    fplayer->id == FIGHTER_A6M2      ||
    fplayer->id == FIGHTER_F4U       ||
    fplayer->id == FIGHTER_F4F       ||
    fplayer->id == FIGHTER_F6F       ||
    fplayer->id == FIGHTER_P47D      ||
    fplayer->id == FIGHTER_P51D      ||
    fplayer->id == FIGHTER_FW190     ||
    fplayer->id == FIGHTER_IL16      ||
    fplayer->id == FIGHTER_FIATG55   ||
    fplayer->id == FIGHTER_ME109G    ||
    fplayer->id == FIGHTER_P39       ||
    fplayer->id == FIGHTER_P40       ||
    fplayer->id == FIGHTER_HURRICANE ||
    fplayer->id == FIGHTER_SPIT9     ||
    fplayer->id == FIGHTER_KI43      ||
    fplayer->id == FIGHTER_LA5       ||
    fplayer->id == FIGHTER_LA7       ||
    fplayer->id == FIGHTER_IL2       ||
    fplayer->id == FIGHTER_MACCIC202 ||
    fplayer->id == FIGHTER_TYPHOON   ||
    fplayer->id == FIGHTER_YAK1      ||
    fplayer->id == FIGHTER_N1K1      ||
    fplayer->id == FIGHTER_YAK9      ||
    fplayer->id == BOMBER_B29        ||
    fplayer->id == FIGHTER_DW520     ||
    fplayer->id == BOMBER_SB2C       ||
    fplayer->id == BOMBER_TBF        ||
    fplayer->id == FIGHTER_ME163     ||
    fplayer->id == FIGHTER_TEMPEST   ||
    fplayer->id == FIGHTER_D3A       ||
    fplayer->id == BOMBER_B5N        ||
    fplayer->id == BOMBER_DAUNTLESS  ||
    fplayer->id == FIGHTER_ME110     ||
    fplayer->id == BOMBER_DORNIER    ||
    fplayer->id == BOMBER_HE111      ||
    fplayer->id == BOMBER_JU88       ||
    fplayer->id == FIGHTER_KI84      ||
    fplayer->id == FIGHTER_KI61      ||
    fplayer->id == BOMBER_JU87       ||
    fplayer->id == BOMBER_B17        ||
    fplayer->id == BOMBER_B24        ||
    fplayer->id == BOMBER_G5M        ||
    fplayer->id == BOMBER_B25        ||
    fplayer->id == BOMBER_B26        ||
    fplayer->id == BOMBER_LANCASTER  ||
    fplayer->id == BOMBER_MOSQUITOB  ||
    fplayer->id == FIGHTER_GENERIC01 ||
    fplayer->id == FIGHTER_A6M5      ||
    fplayer->id == FIGHTER_SPIT5     ||
    fplayer->id == FIGHTER_P51B      ||
    fplayer->id == FIGHTER_P47B      ||
    fplayer->id == FIGHTER_ME109F    ||
    fplayer->id == FIGHTER_P38F

    )
   {
   for (i2 = 0; i2 < missiletypes; i2 ++)
       {
       fplayer->missiles [i2] = 0;
       }

   for (i2 = 0; i2 < missiletypes; i2 ++)
       {
       if (i2 == 0)
          {
          if (fplayer->id == FIGHTER_P38L)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_A6M2)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_F4U)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_F4F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F6F)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P47D)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_P51D)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_FW190)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL16)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_FIATG55)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME109G)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P39)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P40)
             {
             fplayer->missiles [i2] = 3;
             }
          if (fplayer->id == FIGHTER_HURRICANE)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_SPIT9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI43)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_N1K1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_YAK9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA5)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_LA7)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_MACCIC202)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_TYPHOON)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_YAK1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 40;
             }
          if (fplayer->id == FIGHTER_DW520)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_SB2C)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == BOMBER_TBF)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_ME163)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TEMPEST)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_D3A)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == BOMBER_B5N)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == BOMBER_DAUNTLESS)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_ME110)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DORNIER)
             {
             fplayer->missiles [i2] = 5;
             }
          if (fplayer->id == BOMBER_HE111)
             {
             fplayer->missiles [i2] = 12;
             }
          if (fplayer->id == BOMBER_JU88)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_KI84)
             {
             fplayer->missiles [i2] = 3;
             }
          if (fplayer->id == FIGHTER_KI61)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_GENERIC01)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_A6M5)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_SPIT5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P51B)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P47B)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_ME109F)
             {
             fplayer->missiles [i2] = 1;
             }
          if (fplayer->id == FIGHTER_P38F)
             {
             fplayer->missiles [i2] = 1;
             }

          if (fplayer->id == FIGHTER_IL2)
             {
             fplayer->missiles [i2] = 2;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_JU87)
             {
             fplayer->missiles [i2] = 4;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_G5M)
             {
             fplayer->missiles [i2] = 4;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_B25)
             {
             fplayer->missiles [i2] = 6;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_B26)
             {
             fplayer->missiles [i2] = 8;
             if (AirfieldRequested == 1)
                 {
                 fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                 }
             if (AirfieldRequested == 2)
                 {
                 fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                 }
             }
          if (fplayer->id == BOMBER_B17)
             {
             fplayer->missiles [i2] = 15;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_MOSQUITOB)
             {
             fplayer->ammo = 0; // Mosquito Bomber version carried no guns at all.
             fplayer->missiles [i2] = 8;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_LANCASTER)
             {
             fplayer->missiles [i2] = 28;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 40;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
           if (fplayer->id == BOMBER_B24)
             {
             fplayer->missiles [i2] = 16;
             if (AirfieldRequested == 1)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.5);
                }
             if (AirfieldRequested == 2)
                {
                fplayer->missiles [i2] = (int)(fplayer->missiles [i2] * 0.75);
                }
             }
          }
       if (i2 == 5)
          {
          if (fplayer->id == FIGHTER_P38L)
             {
             fplayer->missiles [i2] = 10;
             }
          if (fplayer->id == FIGHTER_A6M2)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F4U)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_F4F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_F6F)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_P47D)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_P51D)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_FW190)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL16)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_FIATG55)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME109G)
             {
             fplayer->missiles [i2] = 2;
             }
          if (fplayer->id == FIGHTER_P39)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P40)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_HURRICANE)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_SPIT9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI43)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_N1K1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_YAK9)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_LA7)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_IL2)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_JU87)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_G5M)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B25)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_B26)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B17)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B24)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_LANCASTER)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_MOSQUITOB)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_MACCIC202)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TYPHOON)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_YAK1)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B29)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_DW520)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_SB2C)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == BOMBER_TBF)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_ME163)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_TEMPEST)
             {
             fplayer->missiles [i2] = 8;
             }
          if (fplayer->id == FIGHTER_D3A)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_B5N)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DAUNTLESS)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_ME110)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_DORNIER)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_HE111)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == BOMBER_JU88)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI84)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_KI61)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_GENERIC01)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_A6M5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_SPIT5)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P51B)
             {
             fplayer->missiles [i2] = 4;
             }
          if (fplayer->id == FIGHTER_P47B)
             {
             fplayer->missiles [i2] = 6;
             }
          if (fplayer->id == FIGHTER_ME109F)
             {
             fplayer->missiles [i2] = 0;
             }
          if (fplayer->id == FIGHTER_P38F)
             {
             fplayer->missiles [i2] = 6;
             }
          }
       }
   }
}

void DetermineCurrentAirfield()
{
if (fplayer->tl->y > (SeaLevel + 1000))
   {
   AirfieldChosenForLanding = 0;
   return;
   }
if (fplayer->tl->z >10.0)
   {
   display ("Z Position is too high for any defined airfield.", LOG_MOST);
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->z < -10.0)
   {
   display ("Z position is too low for any defined airfield.", LOG_MOST);
   AirfieldChosenForLanding = 7;
   return;
   }
display ("Z position is inline with strategic airfields.", LOG_MOST);
if (fplayer->tl->x < -1500.0)
   {
   display ("X Position is too low for any defined airfield.", LOG_MOST);
   AirfieldChosenForLanding = 7;
   return;
   }
if (fplayer->tl->x > 1500.0)
   {
   display ("X Position is too high for any defined airfield.", LOG_MOST);
   AirfieldChosenForLanding = 7;
   return;
   }
display ("X Position is within range of Strategic airfields", LOG_MOST);
if (fplayer->tl->x > 1100.0)
   {
   AirfieldChosenForLanding = 6;
   return;
   }
if (fplayer->tl->x > 600.0)
   {
   AirfieldChosenForLanding = 5;
   return;
   }
if (fplayer->tl->x > 100.0)
   {
   AirfieldChosenForLanding = 4;
   return;
   }
if (fplayer->tl->x > -500.0)
   {
   AirfieldChosenForLanding = 1;
   return;
   }
if (fplayer->tl->x > -1000.0)
   {
   AirfieldChosenForLanding = 2;
   return;
   }
else
   {
   AirfieldChosenForLanding = 3;
   return;
   }
}

void ProcessUdpObjFlightDetails()
{
if (InPacket.UdpObjFlightDetails & 4)
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->SpeedBrake = 1;
   }
else
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->SpeedBrake = 0;
   }
if (InPacket.UdpObjFlightDetails & 3)
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->FlapsLevel = (InPacket.UdpObjFlightDetails & 3);
   }
else
   {
   ThreeDObjects[InPacket.UdpObjPlayerNumber]->FlapsLevel = 0;
   }
if (InPacket.UdpObjFlightDetails & 8)
   {
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A FLARE.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20);
   sound->play (SOUND_BEEP1, false);
   }
if (InPacket.UdpObjFlightDetails & 16)
   {
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A CHAFF PACKET.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20);
   sound->play (SOUND_BEEP1, false);
   }
if (InPacket.UdpObjFlightDetails & 32)
   {
   float XDistance = fabs(InPacket.UdpObjXPosition - fplayer->tl->x);
   float YDistance = fabs (InPacket.UdpObjYPosition - fplayer->tl->y);
   float ZDistance = fabs (InPacket.UdpObjZPosition - fplayer->tl->z);
   float TotalDistance = XDistance + YDistance + ZDistance;

   int volume;
   if (TotalDistance > 200)
      {
      volume = 0;
      }
   else
      {
      volume = (int) 127 - (int)(TotalDistance/1.8);
      }
   sprintf (SystemMessageBufferA, "PLAYER %d FIRED A MISSILE.", InPacket.UdpObjPlayerNumber);
   NewSystemMessageNeedsScrolling = true;
   sound->setVolume (SOUND_BEEP1, 20);
   sound->play (SOUND_BEEP1, false);
   if (MyNetworkId%2 != InPacket.UdpObjPlayerNumber%2)
      {

      sound->setVolume (SOUND_FIVEBEEPS00, volume);
      sound->play (SOUND_FIVEBEEPS00, false);
      }
   }
}

void RepairDamagedAirfields()
{
static bool Siren28 = false;
static bool Siren29 = false;
static float PriorDurability28 = 0.0;
static float PriorDurability29 = 0.0;

float XDistance28 = fabs(fplayer->tl->x - ThreeDObjects[28]->tl->x);
float YDistance28 = fabs(fplayer->tl->y - ThreeDObjects[28]->tl->y);
float ZDistance28 = fabs(fplayer->tl->z - ThreeDObjects[28]->tl->z);
float Distance28 = XDistance28 + YDistance28 + ZDistance28;
if (Siren28)
   {
   int Volume28 = 16384 / ((int)Distance28 * Distance28);
   if (Volume28 > 127)
      {
      Volume28 = 127;
      }
   sound->setVolume (SOUND_AIRRAIDSIREN, Volume28);
   if (Distance28 > 250 || ((ThreeDObjects[28]->Durability >= ThreeDObjects[28]->maxDurability)))
      {
      sound->stop (SOUND_AIRRAIDSIREN);
      Siren28 = false;
      }
   }

float XDistance29 = fabs(fplayer->tl->x - ThreeDObjects[29]->tl->x);
float YDistance29 = fabs(fplayer->tl->y - ThreeDObjects[20]->tl->y);
float ZDistance29 = fabs(fplayer->tl->z - ThreeDObjects[29]->tl->z);
float Distance29 = XDistance29 + YDistance29 + ZDistance29;
if (Siren29)
   {
   int Volume29 = 16384 / ((int)Distance29 * Distance29);
   if (Volume29 > 127)
      {
      Volume29 = 127;
      }
   sound->setVolume (SOUND_AIRRAIDSIREN, Volume29);
   if (Distance29 > 250 || ((ThreeDObjects[29]->Durability >= ThreeDObjects[29]->maxDurability)))
      {
      sound->stop (SOUND_AIRRAIDSIREN);
      Siren29 = false;
      }
   }
float DurabilityChange28 = PriorDurability28 - ThreeDObjects[28]->Durability;
float DurabilityChange29 = PriorDurability29 - ThreeDObjects[29]->Durability;
if (DurabilityChange28 > 0.1)
   {
   sprintf (DebugBuf, "DebugRjb180530 Airfield 28 sustained %f units of new damage.", DurabilityChange28);
   display (DebugBuf, LOG_MOST);
   sound->play (SOUND_AIRRAIDSIREN, true);
   Siren28 = true;
   if (DurabilityChange28 > 7000)
      {
      float XDistance28 = fabs(fplayer->tl->x - ThreeDObjects[28]->tl->x);
      float YDistance28 = fabs(fplayer->tl->y - ThreeDObjects[28]->tl->y);
      float ZDistance28 = fabs(fplayer->tl->z - ThreeDObjects[28]->tl->z);
      float Distance28 = XDistance28 + YDistance28 + ZDistance28;
      int Volume28 = 16384 / ((int)Distance28 * Distance28);
      if (Volume28 > 127)
         {
         Volume28 = 127;
         }
      sound->setVolume (SOUND_EXPLOSION1, Volume28);
      sound->play (SOUND_EXPLOSION1, false);
      }
   }
if (DurabilityChange29 > 0.1)
   {
   sprintf (DebugBuf, "DebugRjb180530 Airfield 29 sustained %f units of new damage.", DurabilityChange29);
   display (DebugBuf, LOG_MOST);
   sound->play (SOUND_AIRRAIDSIREN, true);
   Siren29 = true;
   if (DurabilityChange29 > 7000)
      {
      float XDistance29 = fabs(fplayer->tl->x - ThreeDObjects[29]->tl->x);
      float YDistance29 = fabs(fplayer->tl->y - ThreeDObjects[29]->tl->y);
      float ZDistance29 = fabs(fplayer->tl->z - ThreeDObjects[29]->tl->z);
      float Distance29 = XDistance29 + YDistance29 + ZDistance29;
      int Volume29 = 16384 / ((int)Distance29 * Distance29);
      if (Volume29 > 127)
         {
         Volume29 = 127;
         }
      sound->setVolume (SOUND_EXPLOSION1, Volume29);
      sound->play (SOUND_EXPLOSION1, false);
      }
   }

int i;
// Make incremental repairs to any airfield damage. (Airfields are object numbers 28 and 29.)
for (i=28; i<=29; i++)
   {
   if (ThreeDObjects[i]->Durability < ThreeDObjects[i]->maxDurability)
      {

      //  83 units of repair will completely repair a nearly destroyed airfield in about an hour.

      static float MissionAircraftDistanceX [11];
      static float MissionAircraftDistanceY [11];
      static float MissionAircraftDistanceZ [11];
      static float MissionAircraftDistanceTotal [11];

      static float NearestAircraftDistance [2];
      NearestAircraftDistance[0] = 10000.0;
      NearestAircraftDistance[1] = 10000.0;

      static unsigned int NearestAircraftNumber[2] = {10};

      int j;
      for (j=0; j<=10; j++)
         {
         MissionAircraftDistanceX [j] = fabs(ThreeDObjects[j]->tl->x - ThreeDObjects[i]->tl->x);
         MissionAircraftDistanceY [j] = fabs(ThreeDObjects[j]->tl->y - ThreeDObjects[i]->tl->y);
         MissionAircraftDistanceZ [j] = fabs(ThreeDObjects[j]->tl->z - ThreeDObjects[i]->tl->z);
         MissionAircraftDistanceTotal[j] = (MissionAircraftDistanceX[j] + MissionAircraftDistanceY[j] + MissionAircraftDistanceZ[j]);

         if (MissionAircraftDistanceTotal[j] < NearestAircraftDistance[29-i])
            {
            NearestAircraftDistance[29-i] = MissionAircraftDistanceTotal[j];
            NearestAircraftNumber[29-i] = j;
            }
         }

      if (NearestAircraftNumber[29-i] == 0)
         {
         NearestAircraftNumber[29-i] = (unsigned int)MyNetworkId;
         }

      ThreeDObjects[i]->Durability += AIRFIELDREPAIRVALUE;

      if (ThreeDObjects[25]->Durability <= (ThreeDObjects[25]->maxDurability * 0.4))
         {
         if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.4))
            {
            ThreeDObjects[29]->Durability = ThreeDObjects[29]->maxDurability * 0.39;

            ThreeDObjects[25]->Durability = (ThreeDObjects[25]->maxDurability * 0.5);
            }
         }
      if (ThreeDObjects[24]->Durability <= (ThreeDObjects[24]->maxDurability * 0.4))
         {
         if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.4))
            {
            ThreeDObjects[28]->Durability = ThreeDObjects[28]->maxDurability * 0.39;

            ThreeDObjects[24]->Durability = (ThreeDObjects[24]->maxDurability * 0.5);
            }
         }

      if (i%2)
         {
         if (ThreeDObjects[29]->Durability < (ThreeDObjects[29]->maxDurability * 0.4 ))
            {
            RadarReflectorRedHasBeenDestroyedAtLeastOnce = true;
            }
         if (NearestAircraftDistance[29-i] < 250)
            {
            if (ThreeDObjects[29-i]->realspeed > ThreeDObjects[29-i]->StallSpeed)
               {
               if (NearestAircraftNumber[29-i]%2)
                  {
                  sprintf (SystemMessageBufferA, "REDTEAM HQ AIRFIELD REPAIRS ACCELERATED TO 5X.");
                  NewSystemMessageNeedsScrolling = true;
                  ThreeDObjects[i]->Durability += (4*AIRFIELDREPAIRVALUE);
                  AirfieldRepairsAcceleratedForRedTeam = true;
                  AirfieldRepairsStoppedForRedTeam = false;
                  AirfieldRepairRateNormalForRedTeam = false;
                  }
               else
                  {
                  sprintf (SystemMessageBufferA, "REDTEAM HQ AIRFIELD REPAIRS STOPPED.");
                  NewSystemMessageNeedsScrolling = true;
                  ThreeDObjects[i]->Durability -= AIRFIELDREPAIRVALUE;
                  AirfieldRepairsAcceleratedForRedTeam = false;
                  AirfieldRepairsStoppedForRedTeam = true;
                  AirfieldRepairRateNormalForRedTeam = false;
                  }
               }
            }
         else
            {
            AirfieldRepairsAcceleratedForRedTeam = false;
            AirfieldRepairsStoppedForRedTeam = false;
            AirfieldRepairRateNormalForRedTeam = true;
            }
         }
      else
         {
         if (ThreeDObjects[28]->Durability < (ThreeDObjects[28]->maxDurability * 0.4 ))
            {
            RadarReflectorBlueHasBeenDestroyedAtLeastOnce = true;
            }
         if (NearestAircraftDistance[29-i] < 250)
            {
            if (ThreeDObjects[29-i]->realspeed > ThreeDObjects[29-i]->StallSpeed)
               {
               if (NearestAircraftNumber[29-i]%2)
                  {
                  sprintf (SystemMessageBufferA, "BLUETEAM HQ AIRFIELD REPAIRS STOPPED.");
                  NewSystemMessageNeedsScrolling = true;
                  ThreeDObjects[i]->Durability -= AIRFIELDREPAIRVALUE;
                  AirfieldRepairsAcceleratedForBlueTeam = false;
                  AirfieldRepairsStoppedForBlueTeam = true;
                  AirfieldRepairRateNormalForBlueTeam = false;
                  }
               else
                  {
                  sprintf (SystemMessageBufferA, "BLUETEAM HQ AIRFIELD REPAIRS ACCELERATED TO 5X.");
                  NewSystemMessageNeedsScrolling = true;
                  ThreeDObjects[i]->Durability += (4*AIRFIELDREPAIRVALUE);
                  AirfieldRepairsAcceleratedForBlueTeam = true;
                  AirfieldRepairsStoppedForBlueTeam = false;
                  AirfieldRepairRateNormalForBlueTeam = true;
                  }
               }
            }
         else
            {
            AirfieldRepairsAcceleratedForBlueTeam = false;
            AirfieldRepairsStoppedForBlueTeam = false;
            AirfieldRepairRateNormalForBlueTeam = true;
            }
         }
      }
   }
PriorDurability28 = ThreeDObjects[28]->Durability;
PriorDurability29 = ThreeDObjects[29]->Durability;
}

void DegradeFlightModelDueToOrdnanceLoad()
{
PlayersOriginalMaxGamma = fplayer->maxgamma;
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() 001 PlayersOriginalMaxGamma = %f", PlayersOriginalMaxGamma);
display (DebugBuf, LOG_MOST);
unsigned int TotalMissileLoad = 0;
int HardPoint;
for (HardPoint =0; HardPoint < missiletypes; HardPoint++)
   {
   TotalMissileLoad += fplayer->missiles [HardPoint];
   }
display ("DegradeFlightModelDueToOrdnanceLoad() TotalMissileLoad =", LOG_MOST);
sprintf (DebugBuf, "%d", TotalMissileLoad);
display (DebugBuf, LOG_MOST);
if (TotalMissileLoad > 12)
   {
   TotalMissileLoad = 12;
   }
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() (float)TotalMissileLoad = %f", (float)TotalMissileLoad);
display (DebugBuf, LOG_MOST);
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() PlayersOriginalMaxGamma = %f", PlayersOriginalMaxGamma);
display (DebugBuf, LOG_MOST);
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() (float)TotalMissileLoad / 6.0 = %f", (TotalMissileLoad / 6.0));
display (DebugBuf, LOG_MOST);
fplayer->maxgamma -= (float)(TotalMissileLoad / 6.0);
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() adjusted maxgamma = %f", fplayer->maxgamma);
display (DebugBuf, LOG_MOST);
MaxGammaPenaltyPerWeapon = (PlayersOriginalMaxGamma - fplayer->maxgamma);
if (TotalMissileLoad != 0)
   {
   MaxGammaPenaltyPerWeapon /= TotalMissileLoad;
   }
if (MaxGammaPenaltyPerWeapon < 0)
   {
   MaxGammaPenaltyPerWeapon *= -1.0;
   }
sprintf (DebugBuf, "DegradeFlightModelDueToOrdnanceLoad() MaxGammaPenaltyPerWeapon = %f", MaxGammaPenaltyPerWeapon);
display (DebugBuf, LOG_MOST);
unsigned int i;
for (i=0; i<TotalMissileLoad; i++)
   {
   fplayer->maxthrust -= 0.004;
   }
}

Mission::Mission ()
    {
    int i;
    timer = 0;

    for (i = 0; i < 3; i ++)
        {
        selfighter [i] = 0;
        }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_HAWK;
    selfighter [2] = FIGHTER_SPIT9;
    selfighters = 2;
    wantfighter = 0;
    selweapons = 3;
    selweapon [0] = BOMB01;
    selweapon [1] = MISSILE_DF1;
    selweapon [2] = MISSILE_AIR2;
    wantweapon = 0;
    textcolor.setColor (255, 255, 0, 180);
    clouds = 0;
    heading = 180;
    state = 0;
    LandedAtSafeSpeed = false;
    }

void Mission::playerInit ()
    {
    int i;
    fplayer = ThreeDObjects [0];
    fplayer->target = NULL;
    fplayer->newinit (selfighter [wantfighter], 1, 0);
    if (selfighter [wantfighter] == FIGHTER_P38L)
        {
        fplayer->o = &model_fig;
        }
    else if (selfighter [wantfighter] == FIGHTER_P51D)
        {
        fplayer->o = &model_figc;
        }
    else if (selfighter [wantfighter] == FIGHTER_HAWK)
        {
        fplayer->o = &model_figb;
        }
    else if (selfighter [wantfighter] == FIGHTER_SPIT9)
        {
        fplayer->o = &model_figi;
        }
    else if (selfighter [wantfighter] == FIGHTER_A6M2)
        {
        fplayer->o = &model_figv;
        }
    else if (selfighter [wantfighter] == BOMBER_B17)
        {
        fplayer->o = &model_figu;
        }
    fplayer->ai = false;
    for (i = 0; i < missiletypes; i ++)
        {
        fplayer->missiles [i] = 0;
        }
    for (i = 0; i < missileracks; i ++)
        {
        fplayer->missilerack [i] = -1;
        }
    if (selweapon [wantweapon] == BOMB01)
        {
        if (selfighter [wantfighter] == FIGHTER_P38L)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_P51D)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_HAWK)
            {
            fplayer->missilerackn [0] = 2;
            fplayer->missilerackn [1] = 3;
            fplayer->missilerackn [2] = 3;
            fplayer->missilerackn [3] = 2;
            fplayer->missilerack [0] = 6;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 6;
            }
        if (selfighter [wantfighter] == FIGHTER_SPIT9)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_A6M2)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        }
    if (selweapon [wantweapon] == MISSILE_GROUND1)
        {
        if (selfighter [wantfighter] == FIGHTER_P38L)
            {
            fplayer->missilerackn [0] = 4;
            fplayer->missilerackn [1] = 4;
            fplayer->missilerackn [2] = 4;
            fplayer->missilerackn [3] = 4;
            fplayer->missilerack [0] = 4;
            fplayer->missilerack [1] = 4;
            fplayer->missilerack [2] = 4;
            fplayer->missilerack [3] = 4;
            }
        if (selfighter [wantfighter] == FIGHTER_P51D)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_A6M2)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_HAWK)
            {
            fplayer->missilerackn [0] = 1;
            fplayer->missilerackn [1] = 3;
            fplayer->missilerackn [2] = 3;
            fplayer->missilerackn [3] = 1;
            fplayer->missilerack [0] = 6;
            fplayer->missilerack [1] = 3;
            fplayer->missilerack [2] = 3;
            fplayer->missilerack [3] = 6;
            }
        if (selfighter [wantfighter] == FIGHTER_SPIT9)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        }
    if (selweapon [wantweapon] == MISSILE_DF1)
        {
        if (selfighter [wantfighter] == FIGHTER_P38L)
            {
            fplayer->missilerackn [0] = 2;
            fplayer->missilerackn [1] = 3;
            fplayer->missilerackn [2] = 3;
            fplayer->missilerackn [3] = 2;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_P51D)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_A6M2)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_HAWK)
            {
            fplayer->missilerackn [0] = 1;
            fplayer->missilerackn [1] = 2;
            fplayer->missilerackn [2] = 2;
            fplayer->missilerackn [3] = 1;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 5;
            fplayer->missilerack [2] = 5;
            fplayer->missilerack [3] = 0;
            }
        if (selfighter [wantfighter] == FIGHTER_SPIT9)
            {
            fplayer->missilerackn [0] = 0;
            fplayer->missilerackn [1] = 0;
            fplayer->missilerackn [2] = 0;
            fplayer->missilerackn [3] = 0;
            fplayer->missilerack [0] = 0;
            fplayer->missilerack [1] = 0;
            fplayer->missilerack [2] = 0;
            fplayer->missilerack [3] = 0;
            }
        }
    fplayer->missileCount ();
    }

void Mission::init ()
    {
    }

void Mission::start ()
    {
    RadarReflectorBlueHasBeenDestroyedAtLeastOnce = false;
    RadarReflectorRedHasBeenDestroyedAtLeastOnce = false;
    AutoPilotActive = false;
    }

int Mission::processtimer (Uint32 dt)
    {
    return 0;
    }

void Mission::draw ()
    {
    }

MissionDemo1::MissionDemo1 ()
    {
    id = MISSION_DEMO;
    strncpy (name, "DEMO", 1024);
    }

void MissionDemo1::start ()
    {
    int i;
    day = 1;
    clouds = 0;
    weather = WEATHER_THUNDERSTORM;
    sungamma = 50;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_SEA, NULL);
    fplayer = ThreeDObjects [0];
    ThreeDObjects [0]->tl->x = 1620;
    ThreeDObjects [0]->tl->z = 300;
    ThreeDObjects [0]->o = &model_fig;
    ThreeDObjects [0]->newinit (FIGHTER_FW190, 1, 0);
    }

int MissionDemo1::processtimer (Uint32 dt)
    {
    timer += dt;
    ThreeDObjects [0]->tl->y = 150;
    camera = 8;
    }

void MissionDemo1::draw ()
    {
    sunlight = 1;
    }

MissionTutorial1::MissionTutorial1 ()
    {
    id = MISSION_TUTORIAL;
    strncpy (name, "TUTORIAL1: FLIGHT BASICS", 1024);
    strncpy (briefing, "LEARN TO HANDLE YOUR FIGHTER AND THE BASIC CONTROLS.", 1024);
    font1->drawTextScaled(0, 0, -4, briefing, 10);
    heading = 210;
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTutorial1::start ()
    {
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=1;
    RadarZoom=1;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -13.0;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 220;
    fplayer->tl->z = -30;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    int i;
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionTutorial1::processtimer (Uint32 dt)
    {
    timer += dt;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        sound->stop (SOUND_PLANE1);
        sound->stop (SOUND_PLANE2);
        sound->stop (SOUND_WINDNOISE);
        WindNoiseOn = false;
        }
    if (!ThreeDObjects [1]->active && !ThreeDObjects [2]->active && state == 1)
        {
        return 1;
        }
    return 0;
    }

void MissionTutorial1::draw ()
    {
    char buf [250], buf2 [10];
    int timeroff = 100 * timestep;
    int timerdelay = 300 * timestep;
    int timerlag = 20 * timestep;
    static bool BeepLatch = false;

    if (timer >= 0 && timer <= 30 * timestep)
        {
        font1->drawTextCentered (0, 6, -1, name, &textcolor);
        }
    else if (timer > 50 * timestep && timer <= 180 * timestep)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            if (!BeepLatch)
                {
                BeepLatch = true;
                sound->play (SOUND_BEEP1, false);
                }
            font1->drawTextCentered (0, 11, -2.25, "JIGGLE YOUR THROTTLE AT THE START OF", &textcolor);
            font1->drawTextCentered (0, 10, -2.25, "EVERY MISSION TO START FUEL FLOW.", &textcolor);
            }
        }
    else if (timer > 190 * timestep && timer <= 400 * timestep)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0,  9, -2.25, "LEAVE THE JOYSTICK CENTERED.", &textcolor);
            font1->drawTextCentered (0,  8, -2.25, "THE FIGHTER SHOULD FLY STRAIGHT ON.", &textcolor);
            font1->drawTextCentered (0,  7, -2.25, "IF NOT, RECALIBRATE YOUR JOYSTICK", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, "THE MENU OPTIONS/CONTROLS LETS YOU", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "DETERMINE THE INPUT DEVICE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "CURRENTLY, THIS IS THE MOUSE", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, "THE MENU OPTIONS/CONTROLS LETS YOU", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "DETERMINE THE INPUT DEVICE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "CURRENTLY, THIS IS THE KEYBOARD", &textcolor);
            }
        }
    else if (timer > timeroff + timerdelay && timer <= timeroff + 2 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "MOVING THE JOYSTICK LEFT OR RIGHT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "WILL AFFECT THE AILERON.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THUS YOU WILL ONLY ROLL", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            if (!mouse_reverse && !mouse_relative)
                {
                font1->drawTextCentered (0, 9, -2.25, "THE MOUSE INTERFACE IS VERY EASY.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, "ALWAYS POINT TO WHERE YOU WANT TO FLY.", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, "YOU'LL HAVE TO MOVE THE MOUSE PERMANENTLY", &textcolor);
                }
            else if (mouse_reverse && !mouse_relative)
                {
                font1->drawTextCentered (0, 9, -2.25, "THE REVERSE MOUSE IS FOR EXPERIENCED GAMERS.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, "THE STANDARD MOUSE INTERFACE MAY BE", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, "EASIER TO LEARN", &textcolor);
                }
            else
                {
                font1->drawTextCentered (0, 9, -2.25, "MOUSE RELATIVE IS A LOT OF WORK.", &textcolor);
                font1->drawTextCentered (0, 8, -2.25, "ONLY THE RELATIVE MOUSE COORDINATES (MOVING)", &textcolor);
                font1->drawTextCentered (0, 7, -2.25, "WILL HAVE AN EFFECT", &textcolor);
                }
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, "DON'T TRY TO FLY WITH JUST THE KEYBOARD. USING A", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "JOYSTICK IS STRONLY RECOMMENDED, BUT IF NECESSARY", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "YOU CAN GET BY WITH MOUSE AND KEYBOARD.", &textcolor);
            }
        }
    else if (timer > timeroff + 2 * timerdelay && timer <= timeroff + 3 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "MOVING UP OR DOWN WILL AFFECT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "THE ELEVATOR.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "YOU'LL START TO FLY A LOOP", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE || controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, "MOVING RIGHT OR LEFT WILL AFFECT THE AILERON,", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "MOVING UP OR DOWN WILL AFFECT THE ELEVATOR.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THUS YOU CAN FLY ROLLS AND LOOPS", &textcolor);
            }
        }
    else if (timer > timeroff + 3 * timerdelay && timer <= timeroff + 4 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "IF YOUR JOYSTICK HAS A SO-CALLED \"RUDDER\",", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "YOU MAY ALTER THE FIGHTER'S RUDDER.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "YOU'LL SLIGHTLY FLY TO THE LEFT OR RIGHT", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, "MOVING THE MOUSE SLIGHTLY LEFT OR RIGHT", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "WILL AFFECT THE RUDDER.", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, "TO ALTER THE RUDDER", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "USE THE KEYS PGUP AND PGDN", &textcolor);
            }
        }
    else if (timer > timeroff + 4 * timerdelay && timer <= timeroff + 5 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "YOUR JOYSTICK HAS ALSO A THROTTLE", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "TO SPEEDUP OR DECELERATE.", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THERE IS NO WAY TO ENTIRELY STOP", &textcolor);
            }
        else
            {
            font1->drawTextCentered (0, 9, -2.25, "USE THE KEYS '1' THROUGH '9' TO ALTER", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "YOUR THROTTLE (SPEED).", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THERE IS NO WAY TO ENTIRELY STOP", &textcolor);
            }
        }
    else if (timer > timeroff + 5 * timerdelay && timer <= timeroff + 7 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "THE BUTTONS ON YOUR JOYSTICK CAN CONTROL ALL", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "OF YOUR WEAPONS AND FLIGHT SURFACES. YOU CAN MAKE", &textcolor);
            font1->drawTextCentered (0, 7,  -2.25, "ANY BUTTON ACTIVATE ANY CONTROL BY EDITING THE", &textcolor);
            font1->drawTextCentered (0, 6,  -2.25, "LACCONTROLS.TXT FILE IN THE .LAC FOLDER WITHIN", &textcolor);
            font1->drawTextCentered (0, 5,  -2.25, "YOUR HOME FOLDER.", &textcolor);
            }
        else if (controls == CONTROLS_MOUSE)
            {
            font1->drawTextCentered (0, 9, -2.25, "LEFT MOUSE BUTTON: FIRE CANNON", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "RIGHT MOUSE BUTTON: FIRE MISSILE", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "BUT YOU SHOULD REALLY PREFER THE KEYBOARD", &textcolor);
            }
        else if (controls == CONTROLS_KEYBOARD)
            {
            font1->drawTextCentered (0, 9, -2.25, "NOW, LETS HAVE A LOOK AT SOME IMPORTANT KEYS", &textcolor);
            }
        }
    else if (timer > timeroff + 6 * timerdelay && timer <= timeroff + 8 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "A SUMMARY OF COMMANDS CAN BE FOUND IN THE MENU.", &textcolor);
            font1->drawTextCentered (0, 8, -2.25, "\"ESC\" WILL SHOW THE MENU", &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "AND RETURN BACK TO THE ACTION", &textcolor);
            }
        else
            {
            key2string (key_PRIMARY, buf2);
            sprintf (buf, "%s: FIRE CANNON", buf2);
            font1->drawTextCentered (0, 6, -2.25, buf, &textcolor);
            key2string (key_WEAPONSELECT, buf2);
            sprintf (buf, "%s: CHOOSE MISSILE", buf2);
            font1->drawTextCentered (0, 5, -2.25, buf, &textcolor);
            key2string (key_SECONDARY, buf2);
            sprintf (buf, "%s: FIRE MISSILE", buf2);
            font1->drawTextCentered (0, 4, -2.25, buf, &textcolor);
            }
        }
    else if (timer > timeroff + 7 * timerdelay && timer <= timeroff + 9 * timerdelay - timerlag)
        {
        RadarOnOff=1;
        RadarZoom=1;
        IffOnOff=1;
        RadarZoom=3;
        if (!RadarWasOn)
            {
            sound->setVolume (SOUND_BEEP2, 20);
            sound->play (SOUND_BEEP2, false);
            RadarWasOn=1;
            }
        font1->drawTextCentered (0, 9, -2.25, "LOOK AT THE RADAR ON THE BOTTOM OF YOUR SCREEN.", &textcolor);
        font1->drawTextCentered (0, 8, -2.25, "THERE ARE ENEMIES REPRESENTED BY A WHITE DOT (TARGETED)", &textcolor);
        font1->drawTextCentered (0, 7, -2.25, "AND A YELLOW POINT (NOT TARGETED)", &textcolor);
        }
    else if (timer > timeroff + 8 * timerdelay && timer <= timeroff + 10 * timerdelay - timerlag)
        {
        if (controls == CONTROLS_JOYSTICK)
            {
            font1->drawTextCentered (0, 9, -2.25, "TARGET AN ADVERSARY WITH A CONFIGURED JOYSTICK BUTTON,", &textcolor);
            key2string (key_TARGETNEXT, buf2);
            sprintf (buf, "HATSWITCH, OR BY PRESSING '%s' ON THE KEYBOARD.", buf2);
            font1->drawTextCentered (0, 8, -2.25, buf, &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THEN APPROACH!", &textcolor);
            }
        else
            {
            font1->drawTextCentered (0, 9, -2.25, "YOU MAY TARGET AN ENEMY PRESSING", &textcolor);
            key2string (key_TARGETNEXT, buf2);
            sprintf (buf, "OR PRESSING '%s' ON THE KEYBOARD.", buf2);
            font1->drawTextCentered (0, 8, -2.25, buf, &textcolor);
            font1->drawTextCentered (0, 7, -2.25, "THEN APPROACH!", &textcolor);
            }
        }
    else if (timer > timeroff + 9 * timerdelay && timer <= timeroff + 11 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 9, -2.25, "SHOOT THE TARGETS TO COMPLETE", &textcolor);
        font1->drawTextCentered (0, 8, -2.25, "THIS TUTORIAL SUCCESSFULLY", &textcolor);
        }
    if (timer >= timeroff + 7 * timerdelay && state == 0)
        {
        state ++;
        ThreeDObjects [1]->activate ();
        ThreeDObjects [1]->target = ThreeDObjects [0];
        ThreeDObjects [1]->o = &model_figu;
        ThreeDObjects [1]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [1]->tl->x = fplayer->tl->x - 30;
        ThreeDObjects [1]->tl->z = fplayer->tl->z - 30;
        ThreeDObjects [2]->activate ();
        ThreeDObjects [2]->target = ThreeDObjects [0];
        ThreeDObjects [2]->o = &model_figu;
        ThreeDObjects [2]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [2]->tl->x = fplayer->tl->x + 30;
        ThreeDObjects [2]->tl->z = fplayer->tl->z + 30;
        }

    if (timer > timeroff + 10 * timerdelay)
        {
        if (!ThreeDObjects [1]->active && !ThreeDObjects [2]->active)
            {
            font1->drawTextCentered (0, 11, -2.25, "MISSION COMPLETED SUCCESSFULLY.", &textcolor);
            font1->drawTextCentered (0,  9, -2.25, "PRESS <ESC> TO RETURN TO MAIN MENU.", &textcolor);

            }
        }
    }

MissionTutorial2::MissionTutorial2 ()
    {
    id = MISSION_TUTORIAL2;
    strncpy (name, "TUTORIAL2: AIR-TO-GROUND", 1024);
    strncpy (briefing, "HOW TO USE AIR-TO-GROUND MISSILES VS ARMORED TARGETS.", 1024);
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_GROUND1;
    wantweapon = 0;
    }

void MissionTutorial2::start ()
    {
    NoMissionHasYetCommenced = false;
    int i;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=0;
    IffOnOff=0;
    event_RadarOnOff();
    RadarZoom=5;
    event_IffOnOff();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_FLAT_MOON, NULL);
    SeaLevel = 165.0;
    int px, py;

    l->searchPlain (-1, -1, &px, &py);
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();

    fplayer->tl->x = px;
    fplayer->tl->z = py + 150;
    fplayer->phi = 250;
    fplayer->missiles [1] = 0;
    fplayer->missiles [2] = 0;
    fplayer->missiles [3] = 8;  // Hardpoint 3 = AGM
    fplayer->missiles [4] = 0;
    fplayer->missiles [5] = 8; // Hardpoint 5 = DFM

    for (i = 1; i <= 2; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->o = &model_tank1;
        ThreeDObjects [i]->tl->x = px + 300 - i * 4;
        ThreeDObjects [i]->tl->z = py + 300 - i * 4;
        ThreeDObjects [i]->newinit (TANK_GROUND1, 0, 400);
        ThreeDObjects [i]->maxthrust = 0;
        ThreeDObjects [i]->activate();
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionTutorial2::processtimer (Uint32 dt)
    {
    bool b = false;
    int i;
    timer += dt;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        }
    for (i = 1; i <= 2; i ++)
        {
        if (ThreeDObjects [i]->active)
           {
           ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 0.1;
           if (ThreeDObjects [i]->party == 0)
                {
                b = true;
                }
            }
        }
    if (b)
        {
        return 0;
        }

    return 1;
    }

void MissionTutorial2::draw ()
    {
    char buf [250], buf2 [10];
    int timeroff = 100 * timestep, timerdelay = 300 * timestep, timerlag = 20 * timestep;

    if (timer >= 0 && timer <= timeroff - 20)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    else if (timer > timeroff && timer <= timeroff + timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "FIRSTLY, HAVE A LOOK AT YOUR MISSILES:", &textcolor);
        key2string (key_WEAPONSELECT, buf2);
        sprintf (buf, "PRESS '%s' TO SWITCH BETWEEN", buf2);
        font1->drawTextCentered (0, 6, -2.5, buf, &textcolor);
        font1->drawTextCentered (0, 5, -2.5, "AGM (AIR-GROUND-MISSILE) AND DF (DUMB FIRE)", &textcolor);
        }
    else if (timer > timerdelay + timeroff && timer <= timeroff + 2 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "LET'S USE THE AGM: AS SOON AS THE TARGET", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, "IS LOCKED (YELLOW), SHOOT THE AGM AT ANY", &textcolor);
        font1->drawTextCentered (0, 5, -2.5, "DIRECTION AND WATCH WHERE IT GOES.", &textcolor);
        }
    else if (timer > 2 * timerdelay + timeroff && timer <= timeroff + 3 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "THEN TRY A DF MISSILE BUT BE CAREFUL:", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, "IT IS QUITE 'DUMB' AND WILL FLY STRAIGHT AHEAD.", &textcolor);
        }
    else if (timer > 3 * timerdelay + timeroff && timer <= timeroff + 4 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 6, -2.5, "HOWEVER, DF MISSILES WILL CAUSE MUCH MORE DAMAGE", &textcolor);
        }
    else if (timer > 4 * timerdelay + timeroff && timer <= timeroff + 5 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 6, -2.5, "NOW, BLOW UP THE TWO TANKS", &textcolor);
        }
    }

MissionTutorial3::MissionTutorial3 ()
    {
    id = MISSION_TUTORIAL3;
    strncpy (name, "TUTORIAL3: FIGHTERS", 1024);
    strncpy (briefing, "HIGH SPEED DOGFIGHT PRACTICE WITH MISSILES AND GUNS.", 1024);
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTutorial3::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    RadarOnOff=0;
    RadarZoom=1;
    event_RadarOnOff();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_IffOnOff();
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_SEA, NULL);
    SeaLevel = 15.15;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    srand((unsigned)time(NULL));
    fplayer->tl->x = rand() % (1800) - 900 ;
    fplayer->tl->y = 10;
    fplayer->tl->z = rand() % (1800) - 900;
    fplayer->phi = 0;
    fplayer->gamma = 180;
    fplayer->theta = 0;
    cout << "fplayerX: " << fplayer->tl->x << endl;
//    fplayer->tl->x = 20;
//    fplayer->tl->z = 70;
//    fplayer->phi = 60;
//    for (i = 1; i <= 6; i ++)
    for (i = 1; i <= 1; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->o = &model_figb;
        ThreeDObjects [i]->tl->x = fplayer->tl->x;
        ThreeDObjects [i]->tl->y = fplayer->tl->y;
        ThreeDObjects [i]->tl->z = fplayer->tl->z - 2;
        ThreeDObjects [i]->phi = fplayer->phi;
        ThreeDObjects [i]->gamma = fplayer->gamma;
        ThreeDObjects [i]->theta = fplayer->theta;
        ThreeDObjects [i]->realspeed = 0;
        ThreeDObjects [i]->newinit(FIGHTER_SPIT9,0,400,100,800); // id, party, stupidity, precision, passivity
        ThreeDObjects [i]->deactivate ();
        }
    ThreeDObjects [1]->activate ();
    state = 0;
    laststate = 0;
    texttimer = 0;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    }

int MissionTutorial3::processtimer (Uint32 dt)
    {
    bool b = false;
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        }
    for (i = 0; i <= 1; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                {
                b = true;
                }
        }
    if (b)
        {
        return 0;
        }
    state ++;
    if (state == 1)
        {
//        for (i = 2; i <= 3; i ++)
//            {
//            ThreeDObjects [i]->activate ();
//            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
//            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
//            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
//            }
//        return 0;
//        }
//    else if (state == 2)
//        {
//        for (i = 4; i <= 6; i ++)
//            {
//            ThreeDObjects [i]->activate ();
//            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
//            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
//            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
//            }
//        return 0;
            cout << "finished mission" << endl;
        }
    return 1;
    }

void MissionTutorial3::draw ()
    {
    char buf [250], buf2 [10], buf3 [10];
    int timeroff = 100 * timestep, timerdelay = 300 * timestep, timerlag = 20 * timestep;

    if (laststate != state)
        {
        texttimer = 1;
        laststate = state;
        }
    if (texttimer > 0)
        {
        if (state == 1)
            {
            font1->drawTextCentered (0, 7, -2.5, "THAT WAS EASY", &textcolor);
            font1->drawTextCentered (0, 6, -2.5, "BUT CAN YOU HANDLE TWO ENEMIES?", &textcolor);
            return;
            }
        if (state == 2)
            {
            font1->drawTextCentered (0, 6, -2.5, "OK, LET'S TRY THREE ENEMIES", &textcolor);
            return;
            }
        }
    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    else if (timer >= timeroff && timer <= timeroff + timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "THERE IS AN ENEMY ATTACKING", &textcolor);
        key2string (key_WEAPONSELECT, buf2);
        sprintf (buf, "PRESS '%s' ON THE KEYBOARD TO", buf2);
        font1->drawTextCentered (0, 6, -2.5, buf, &textcolor);
        font1->drawTextCentered (0, 5, -2.5, "SELECT A MISSILE", &textcolor);
        }
    else if (timer >= timeroff + timerdelay && timer <= timeroff + 2 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "CHOOSE AN AIR-TO-AIR MISSILE", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, "WHICH DETECTS ENEMIES BY RADAR.", &textcolor);
        font1->drawTextCentered (0, 5, -2.5, "SHOOT AFTER YELLOW LOCK BOX STABILIZES.", &textcolor);
        }
    else if (timer >= timeroff + 2 * timerdelay && timer <= timeroff + 3 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "YOU CAN ONLY FIRE MISSILES WHEN", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, "THE RECTANGULAR LOCK AROUND THE ENEMY", &textcolor);
        font1->drawTextCentered (0, 5, -2.5, "APPEARS YELLOW", &textcolor);
        }
    else if (timer >= timeroff + 4 * timerdelay && timer <= timeroff + 5 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "CHAFF IS A DECOY FOR RADAR SEEKING MISSILES,", &textcolor);
        font1->drawTextCentered (0, 6, -2.5, "FLARES PROTECT FROM INFRARED MISSILES", &textcolor);
        key2string (key_DROPCHAFF, buf2);
        key2string (key_DROPFLARE, buf3);
        sprintf (buf, "KEYS: DROP CHAFF = '%s', DROP FLARE = '%s'", buf2, buf3);
        font1->drawTextCentered (0, 5, -2.5, buf, &textcolor);
        }
    else if (timer >= timeroff + 6 * timerdelay && timer <= timeroff + 7 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 7, -2.5, "A LITTLE TARGETING HINT:", &textcolor);
        key2string (key_TARGETNEXT, buf2);
        sprintf (buf, "'%s' WILL TARGET THE NEXT RADAR BLIP,", buf2);
        font1->drawTextCentered (0, 6, -2.5, buf, &textcolor);
        key2string (key_TARGETPREVIOUS, buf2);
        sprintf (buf, "'%s' WILL TARGET THE PREVIOUS RADAR BLIP", buf2);
        font1->drawTextCentered (0, 5, -2.5, buf, &textcolor);
        }
    }

MissionTutorial4::MissionTutorial4 ()
    {
    id = MISSION_TUTORIAL4;
    strncpy (name, "TUTORIAL4: AERODYNAMICS", 1024);
    strncpy (briefing, "BASIC AERODYNAMICS USING THE SIMULATION MODEL FOR MORE REALISM.", 1024);
    heading = 210;
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTutorial4::start ()
    {
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_SEA, NULL);
    SeaLevel= 12.915;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 220;
    fplayer->tl->z = -30;
    }

int MissionTutorial4::processtimer (Uint32 dt)
    {
    timer += dt;
    fplayer->realism = physics;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        }
    if (!ThreeDObjects [1]->active && !ThreeDObjects [2]->active && state == 1)
        {
        return 1;
        }
    return 0;
    }

void MissionTutorial4::draw ()
    {
    int timeroff = 100 * timestep, timerdelay = 350 * timestep, timerlag = 120 * timestep;

    if (timer >= 0 && timer <= timeroff - 20)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    else if (timer > timeroff && timer <= timeroff + 1.2 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "THE MENU OPTIONS/CONTROLS LETS YOU DEFINE DIFFICULTY", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "AND REALISM. CHANGING THESE SETTINGS WILL ONLY HAVE", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "EFFECT ON THE NEXT MISSION YOU START", &textcolor);
        }
    else if (timer > timeroff + timerdelay && timer <= timeroff + 2.8 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "APART FROM YOUR CURRENT PHYSICS SETTINGS", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "LET'S USE THE SIMULATION MODEL AND DISCUSS", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "THE FOUR FORCES THRUST, DRAG, LIFT, AND WEIGHT", &textcolor);
        }
    else if (timer > timeroff + 3 * timerdelay && timer <= timeroff + 4.5 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "THRUST IS FORWARD FORCE CREATED BY THE ENGINES.", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "CHANGE THE THRUST TO GAIN OR LOSE SPEED", &textcolor);
        }
    else if (timer > timeroff + 5 * timerdelay && timer <= timeroff + 7 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "DRAG IS THE RETARDING FORCE THAT LIMITS THE AIRCRAFT'S", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "SPEED. IT IS CAUSED BY THE AIRCRAFT'S STRUCTURE AND", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "SURFACE CONTROLS (SPEEDBRAKE, FLAPS, AND UNDERCARRIAGE)", &textcolor);
        font1->drawTextCentered (0, 10, -2.5, "FIGHTING AGAINST ATMOSPHERIC RESISTANCE.", &textcolor);
        }
    else if (timer > timeroff + 7 * timerdelay && timer <= timeroff + 8.8 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "LIFT IS THE UPWARD FORCE CREATED BY THE AIRFLOW THAT PASSES", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "OVER THE WINGS. IN STRAIGHT UNACCELERATED FLIGHT, IT", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "COMPENSATES THE WEIGHT AND YOUR FIGHTER WON'T CLIMB OR DIVE", &textcolor);
        }
    else if (timer > timeroff + 9 * timerdelay && timer <= timeroff + 10 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "THE LIFT FORCE DEPENDS ON THE SPEED:", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "LOW SPEED WILL CAUSE THE AIRPLANE TO DIVE,", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "AT HIGH SPEED IT WILL EVEN CLIMB", &textcolor);
        }
    else if (timer > timeroff + 11 * timerdelay && timer <= timeroff + 12 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "NOW TRY THIS EFFECT BY FLYING STRAIGHT AHEAD AT 0 DEGREE", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "ELEVATION WHILE USING THE LOWEST/HIGHEST THRUST SETTINGS", &textcolor);
        }
    else if (timer > timeroff + 13 * timerdelay && timer <= timeroff + 14 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "IF YOU FLY A ROLL, THE LIFT WILL NOT OPPOSE THE THRUST", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "AND YOU WILL LOSE HEIGHT. TRY!", &textcolor);
        }
    else if (timer > timeroff + 15 * timerdelay && timer <= timeroff + 16 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "IF YOU ROLL TO THE SIDE, THE LIFT WILL TEAR THE AIRCRAFT", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "FROM STRAIGHT FLIGHT TO A DIAGONAL DIRECTION WHILE", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "LOSING HEIGHT. TRY!", &textcolor);
        }
    else if (timer > timeroff + 17 * timerdelay && timer <= timeroff + 18 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "THE OVERALL FORCE IS GAINED BY ADDING THE FOUR FORCES.", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "IT IS MEASURED IN 'G' WITH 1G MEANING THE EARTH'S GRAVITY", &textcolor);
        }
    else if (timer > timeroff + 19 * timerdelay && timer <= timeroff + 20 * timerdelay - timerlag)
        {
        font1->drawTextCentered (0, 13, -2.5, "YOU ARE MOSTLY OPPOSED TO MORE THAN ONLY 1G. BE CAREFUL:", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "6G OR MORE MAY LEAD TO A BLACKOUT,", &textcolor);
        font1->drawTextCentered (0, 11, -2.5, "AT LESS THAN -3G THE BLOOD WILL SHOOT INTO YOUR HEAD", &textcolor);
        }
    else if (timer > timeroff + 21 * timerdelay && timer <= timeroff + 22 * timerdelay - timerlag)
        {
        RadarOnOff=1;
        RadarZoom=1;
        IffOnOff=1;
        RadarZoom=3;

        if (!RadarWasOn)
            {
            sound->setVolume (SOUND_BEEP2, 20);
            sound->play (SOUND_BEEP2, false);
            RadarWasOn=1;
            }

        font1->drawTextCentered (0, 13, -2.5, "NOW ADJUST SPEED TO ABOUT 500MPH AND GET RID OF", &textcolor);
        font1->drawTextCentered (0, 12, -2.5, "THE TWO TRANSPORT AIRCRAFT INDICATED ON RADAR.", &textcolor);
        }
    if (timer >= timeroff + 23 * timerdelay - timerlag / 2 && state == 0)
        {
        RadarWasOn=0;
        state ++;
        ThreeDObjects [1]->activate ();
        ThreeDObjects [1]->target = ThreeDObjects [0];
        ThreeDObjects [1]->o = &model_figu;
        ThreeDObjects [1]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [1]->tl->x = fplayer->tl->x - 30;
        ThreeDObjects [1]->tl->z = fplayer->tl->z - 30;
        ThreeDObjects [2]->activate ();
        ThreeDObjects [2]->target = ThreeDObjects [0];
        ThreeDObjects [2]->o = &model_figu;
        ThreeDObjects [2]->newinit (BOMBER_B17, 0, 200);
        ThreeDObjects [2]->tl->x = fplayer->tl->x + 30;
        ThreeDObjects [2]->tl->z = fplayer->tl->z + 30;
        }
    }

MissionTurkeyShoot::MissionTurkeyShoot ()
    {
    id = MISSION_TURKEYSHOOT;
    strncpy (name, "TURKEY SHOOT", 1024);
    strncpy (briefing, "SHOOT SLOW-MOVING, UNARMED B17 BOMBERS FLOWN BY TRAINEES.", 1024);
    selfighter [0] = DefaultAircraft;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTurkeyShoot::start ()
    {
    int i;
    int i2;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=0;
    IffOnOff=0;
    HudLadderBarsOnOff = 0;
    event_RadarOnOff();
    event_IffOnOff();
    event_HudLadderBarsOnOff();
    RadarZoom = 6;
    heading = 100;
    if (l != NULL)
        {
        delete l;
        }

    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 20;
    fplayer->tl->z = 70;

    for (i = 1; i <= 9; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->tl->x = -i * 10;
        ThreeDObjects [i]->tl->z = -i * 10;
        ThreeDObjects [i]->o = &model_figu;
        ThreeDObjects [i]->newinit (BOMBER_B17, 0, 395);

        if (i > 3)
            {
            ThreeDObjects [i]->deactivate ();
            }
        }
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionTurkeyShoot::processtimer (Uint32 dt)
    {
    bool AtLeastOneActiveB17isStillAlive = false;
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;

    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        sound->stop (SOUND_PLANE1);
        sound->stop (SOUND_PLANE2);
        sound->stop (SOUND_WINDNOISE);
        WindNoiseOn = false;
        }

    for (i = 0; i <= 9; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                {
                AtLeastOneActiveB17isStillAlive = true;
                }
        }
    if (AtLeastOneActiveB17isStillAlive)
        {
        return 0;
        }

    state ++;
    if (state == 1)
        {
        for (i = 4; i <= 5; i ++)
            {
            ThreeDObjects [i]->activate ();
            int phi = 120 * i;
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 40 * COS(phi);
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 40 * SIN(phi);
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 25;
            }
        return 0;
        }
    else if (state == 2)
        {
        for (i = 6; i <= 9; i ++)
            {
            ThreeDObjects [i]->activate ();
            int phi = 90 * i;
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 40 * COS(phi);
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 40 * SIN(phi);
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 25;
            }
        return 0;
        }
    return 1;
    }

void MissionTurkeyShoot::draw ()
    {
    int timeroff = 100 * timestep, timerlag = 20 * timestep;

    if (laststate != state)
        {
        texttimer = 1;
        laststate = state;
        }
    if (texttimer > 0)
        {
        if (state == 1)
            {
            font1->drawTextCentered (0, 6, -2.5, "TRANSPORTERS RESPAWNING", &textcolor);
            return;
            }

        if (state == 2)
            {
            font1->drawTextCentered (0, 6, -2.5, "TRANSPORTERS RESPAWNING", &textcolor);
            return;
            }
        }
    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionHeadToHead00::MissionHeadToHead00()
    {

    id = MISSION_HEADTOHEAD00;
    strncpy (name, "HEAD TO HEAD 00", 1024);
    strncpy (briefing, "SHOOT MINDLESS BOTS OR A SENTIENT NETWORK PEER!\nTHIS MISSION REQUIRES INTERNET ACCESS. EDIT YOUR LACCONFIG.TXT\nFILE TO SPECIFY THE IP ADDRESS OF YOUR SINGLE OPPONENT.\nNO LAC SERVER IS REQUIRED.", 1024);
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_HAWK;
    selfighters = 2;
    selweapons = 1;

    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 2;
    LoadServerIpAddress();

    NetworkOpponent = 0;
    if(NetworkMode == 0)
      {
      if (OpenUdpSocketForReceiving() == 0)
        {
        ConfigureIncomingUdpSocket();
        }
      else
        {
        }
      if (OpenUdpSocketForSending() == 0)
        {
        ConfigureOutgoingUdpSocket();
        }
      else
        {
        }
      }
    else if (NetworkMode == 1)
      {
      if (OpenClientUdpSocket() == 0)
        {
        ConfigureClientUdpSocket();
        }
      else
        {
        }
      }
    else
      {
      }
    }

void MissionHeadToHead00::start ()
    {
    NoMissionHasYetCommenced = false;

    NetworkPlayerKilled = false;
    state = 0;
    laststate = 0;
    texttimer = 0;
    int i;
    int i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915000;

    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    PlayerAircraftType = fplayer->id;
    fplayer->party = 1;

    int r = myrandom (200);
    r -=100;
    float RandomFloat = (float)r;
    fplayer->tl->x = AirfieldXMin;
    fplayer->tl->x += RandomFloat;
    fplayer->tl->z = AirfieldYMin;
    fplayer->tl->z += RandomFloat;
    MissionRunning = false;
    fplayer->phi = 270;
    fplayer->maxthrust *=1.10;

    HudOnOff = 1;
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    IffOnOff=0;
    MapViewOnOff = 0;
    RadarOnOff=0;
    RadarZoom = 1;
    ScreenFOVx = 1.0;
    ScreenFOVy = 1.0;

    event_IffOnOff();
    event_RadarOnOff();
    event_MapViewOnOff();
    event_MapZoomIn();
    event_MapZoomIn();
    event_MapViewOnOff();
    MaxPlayersInCurrentMission = 2;

    for (i = 1; i <= 9; i ++)
        {
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->tl->x = -i * 10;
        ThreeDObjects [i]->tl->z = -i * 10;
        ThreeDObjects [i]->o = &model_figv;
        ThreeDObjects [i]->party = 1;
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 1200);
        for (i2 = 0; i2 < missiletypes; i2 ++)
           {
           ThreeDObjects[i]->missiles [i2] = 0;
           }
        NetworkApiPriorXPosition[i] = -15;
        NetworkApiPriorZPosition[i] = 45;
        NetworkApiPriorYPosition[i] = 5;

        if (i > 1)
            {
            ThreeDObjects [i]->deactivate ();
            }
        }
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    ThreeDObjects[1]->thrustDown();
    NetworkApiPriorTimer[MissionHeadToHead00State] = timer;

    state = 1;
    MissionHeadToHead00State = (unsigned char)state;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    ConfigureOrdnanceForOnlineMissions();
    }

int MissionHeadToHead00::processtimer (Uint32 dt)
    {

    extern int PriorPeerId;
    bool AtLeastOneActiveAiIsStillAlive = false;
    int i;
    int BytesReceived = 0;
    if (!MissionRunning)
       {

       DegradeFlightModelDueToOrdnanceLoad();
       fplayer->tl->y = l->getHeight(AirfieldXMin+1, AirfieldYMin+2);
       if (fplayer->missiles [0] >= 6)
          {
          fplayer->tl->y += 300;
          fplayer->tl->x = AirfieldXMin-120;
          }
       else
          {
          fplayer->tl->y += 21;
          }

       fplayer->UndercarriageLevel = 0;
       event_ToggleUndercarriage;
       fplayer->FlapsLevel = 4;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       }

    StateTransitionTimer += dt;
    texttimer += dt;
    timer += dt;
    if (StateTransitionTimer <0)
       {
       DiscardAnyInPacketsInQueue();
       }

    PriorPeerId = ThreeDObjects[state]->id;

    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval & StateTransitionTimer >=0)
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          {
          display ("MissionHeadtoHead00 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          MissedPacketCount = 0;
          MissionHeadToHead00LoadVariablesFromNetworkApiPacket(timer);
          if (PriorPeerId != ThreeDObjects[state]->id)
             {

              ThreeDObjects[state]->newinit (ThreeDObjects[state]->id, 0, 395);
             }
          if (!PeerPacketReceivedInThisMissionState)
             {
             PeerPacketReceivedInThisMissionState=true;
             sound->setVolume (SOUND_NEWBANDITONRADARO1, 99);
             sound->play (SOUND_NEWBANDITONRADARO1, false);
             }
          }
       else
          {
          MissedPacketCount++;
          if (MissedPacketCount > 75 * (200/NetworkReceiveTimerInterval))
             {
             MissedPacketCount = 0;
             if (PeerPacketReceivedInThisMissionState)
                {
                PeerPacketReceivedInThisMissionState = false;
                sound->setVolume (SOUND_RADARBANDITDISAPPEARED01, 99);
                sound->play (SOUND_RADARBANDITDISAPPEARED01, false);
                }
             }
          }
       }

    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       {
       NetworkTransmitTimer=0;
       MissionHeadToHead00RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }

    if (!fplayer->active && fplayer->explode >= 2 * timestep)
        {
        NetworkPlayerKilled = true;
        sound->haltMusic();
        sound->stop (SOUND_PLANE1);
        sound->stop (SOUND_PLANE2);
        sound->stop (SOUND_WINDNOISE);
        WindNoiseOn = false;
        }
    if (!fplayer->active && fplayer->explode >= 100 * timestep)
        {

        display ("MissionHeadToHead00::processtimer() NetworkPlayerKilled.", LOG_MOST);
        game_quit ();
        }

    for (i = 0; i <= 9; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                {
                AtLeastOneActiveAiIsStillAlive = true;
                }
        }
    if (AtLeastOneActiveAiIsStillAlive)
        {
        return 0;
        }

    DiscardAnyInPacketsInQueue();

    NetworkReceiveTimer= -2500; // Wait awhile before getting next InPacket
    StateTransitionTimer = -2000;
    state ++;
    NetworkOpponent = false;
    PeerPacketReceivedInThisMissionState = false;
    if (state>10)
        {
        state = 10;
        }
    ThreeDObjects [1]->fighterkills = state -1;
    MissionHeadToHead00State = (unsigned char)state; // Advertise our state globally

    if (state==2)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 40;
       return 0;
       }
    if (state==3)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 50;
       return 0;
       }
    if (state==4)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 60;
       return 0;
       }
    if (state==5)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 70;
       return 0;
       }
    if (state==6)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 80;
       return 0;
       }
    if (state==7)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 90;
       return 0;
       }
    if (state==8)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 100;
       return 0;
       }
    if (state==9)
       {

       ThreeDObjects [state]->activate ();
       int phi = 120 * i;
       ThreeDObjects [state]->tl->x = -10;
       ThreeDObjects [state]->tl->z = fplayer->tl->z + 40 * SIN(phi);
       ThreeDObjects [state]->tl->y = l->getHeight (ThreeDObjects [state]->tl->x, ThreeDObjects [state]->tl->z) + 110;
       return 0;
       }
    return 1;
    }

void MissionHeadToHead00::draw ()
    {
    int timeroff = 100 * timestep, timerlag = 20 * timestep;

    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

void MissionHeadToHead00LoadVariablesFromNetworkApiPacket(int timer)
{

   unsigned char PlayerNumber;

   extern LacUdpApiPacket InPacket;
   display ((char *) "MissionHeadToHead00LoadVariablesFromNetworkApiPacket()", LOG_NET);

   float DeltaX;
   float DeltaY;
   float DeltaZ;
   float XSpeed;
   float YSpeed;
   float ZSpeed;
   int DeltaTimer;

   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {

      sound->setVolume (SOUND_BEEP1, 80);
      sound->play (SOUND_BEEP1, false);
      display ("MissionHeadToHead00LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ("Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         {
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ("This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         {
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d", LOG_ERROR);
         display (DebugBuf, LOG_ERROR);
         display ("Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1;

   PlayerNumber = MissionHeadToHead00State;
   display ("MissionHeadToHead00LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);

   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;

   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;

   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;

   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   DeltaX = InPacket.UdpObjXPosition - NetworkApiPriorXPosition[MissionHeadToHead00State];
   DeltaY = InPacket.UdpObjYPosition - NetworkApiPriorYPosition[MissionHeadToHead00State];
   DeltaZ = InPacket.UdpObjZPosition - NetworkApiPriorZPosition[MissionHeadToHead00State];
   DeltaTimer = timer - NetworkApiPriorTimer[MissionHeadToHead00State];
   XSpeed = DeltaX/(float)DeltaTimer;
   YSpeed = DeltaY/(float)DeltaTimer;
   ZSpeed = DeltaZ/(float)DeltaTimer;
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;

   NetworkApiPriorXPosition[MissionHeadToHead00State] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[MissionHeadToHead00State] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[MissionHeadToHead00State] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[MissionHeadToHead00State]     = timer;
   fplayer->Durability -= InPacket.UdpObjDamageAmount; // Accept damage from peer
}

bool MissionHeadToHead00RetrieveFirstDamageDescription()
{
display ("MissionHeadToHead00RetrieveFirstDamageDescription()", LOG_NET);
MissionAircraftDamaged = 1;
DamageToClaim = ThreeDObjects[1]->DamageInNetQueue;
ThreeDObjects[1]->DamageInNetQueue=0;
return (false);
}

MissionEveryManForHimself::MissionEveryManForHimself ()
    {
    id = MISSION_EVERYMANFORHIMSELF;
    strncpy (name, "EVERY MAN FOR HIMSELF. WW2.", 1024);
    strncpy (briefing, "5 WW2 OPPONENTS OVER BLUE OCEAN. LAST PLANE FLYING WINS.", 1024);
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionEveryManForHimself::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    RadarZoom = 6;
    RadarOnOff=1;
    IffOnOff=0;
    heading = 220;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 0;
    fplayer->tl->y = 200;
    fplayer->tl->z = 50;
    for (i = 1; i <= 6; i ++)
        {
        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 10);
        ThreeDObjects [i]->party = i + 1;
        ThreeDObjects [i]->target = ThreeDObjects [i - 1];
        ThreeDObjects [i]->o = &model_figv;
        ThreeDObjects [i]->tl->x = 80 * SIN(i * 360 / 8);
        ThreeDObjects [i]->tl->y = 20 * i;
        ThreeDObjects [i]->tl->z = 220 * COS(i * 360 / 8);
        }
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       event_IffOnOff();
       }
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionEveryManForHimself::processtimer (Uint32 dt)
    {
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }

    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    return 0;
    }

void MissionEveryManForHimself::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionFreeFlightWW2::MissionFreeFlightWW2 ()
    {
    id = MISSION_FREEFLIGHTWW2;
    strncpy (name, "FREE FLIGHT WW2", 1024);
    strncpy (briefing, "FREE FLIGHT OVER THE BLUE OCEAN IN A WW2 PLANE.", 1024);
    selweapons = 1;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionFreeFlightWW2::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    RadarOnOff=1;
    RadarZoom=1;
    IffOnOff=1;
    event_RadarOnOff();
    event_IffOnOff();
    heading = 220;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 0;
    fplayer->tl->y = 200;
    fplayer->tl->z = 50;
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       event_IffOnOff();
       }
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    }

int MissionFreeFlightWW2::processtimer (Uint32 dt)
    {
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    return 0;
    }

void MissionFreeFlightWW2::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionJetbait1::MissionJetbait1 ()
    {
    id = MISSION_JETBAIT1;
    strncpy (name, "ERROR. MISSION NOT AVAILABLE.", 1024);
    strncpy (briefing, "PRESS <ESC> TO RETURN TO MENU.", 1024);
    selweapons = 1;
    selfighter [0] = DefaultAircraft;
    selfighter [1] = NULL;
    selfighter [2] = NULL;
    selfighters = 0;
    }

void MissionJetbait1::start ()
    {
    NoMissionHasYetCommenced = false;
    }

int MissionJetbait1::processtimer (Uint32 dt)
    {
    return 0;
    }

void MissionJetbait1::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionTeamsMissilesAndGuns::MissionTeamsMissilesAndGuns ()
    {
    id = MISSION_TEAMSMISSILESANDGUNS;
    strncpy (name, "TEAMS, MISSILES, AND GUNS", 1024);
    strncpy (briefing, "8 OPPONENTS IN 2 TEAMS. 12 VICTORIES TO WIN.", 1024);
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTeamsMissilesAndGuns::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    RadarOnOff=0;
    RadarZoom=1;
    event_RadarOnOff();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_IffOnOff();
    sungamma = 45;
    heading = 220;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE, NULL);
    SeaLevel = 30.0;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 0;
    fplayer->tl->z = 50;
    for (i = 1; i <= 7; i ++)
        {
        if (i <= 1)
            {
            ThreeDObjects [i]->newinit (FIGHTER_HAWK, 0, 400);
            ThreeDObjects [i]->o = &model_figb;
            }
        else if (i <= 3)
            {
            ThreeDObjects [i]->newinit (FIGHTER_HAWK, 0, 400);
            ThreeDObjects [i]->o = &model_figb;
            }
        else if (i <= 5)
            {
            ThreeDObjects [i]->newinit (FIGHTER_HAWK, 0, 400);
            ThreeDObjects [i]->o = &model_figb;
            }
        else
            {
            ThreeDObjects [i]->newinit (FIGHTER_HAWK, 0, 400);
            ThreeDObjects [i]->o = &model_figb;
            }
        ThreeDObjects [i]->party = i%2;
        ThreeDObjects [i]->target = ThreeDObjects [(i + 4) % 8];
        ThreeDObjects [i]->tl->x = 50 * SIN(i * 360 / 8);
        ThreeDObjects [i]->tl->z = 50 * COS(i * 360 / 8);
        }
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    RadarZoom = 6;
    RadarOnOff=1;
    IffOnOff=1;
    HudLadderBarsOnOff = 0;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       }
    }

int MissionTeamsMissilesAndGuns::processtimer (Uint32 dt)
    {
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    for (i = 0; i <= 3; i ++)
        {
        if (ThreeDObjects [i * 2]->fighterkills + ThreeDObjects [i * 2 + 1]->fighterkills >= 12)
            {
            if (i == 0)
                {
                return 1;
                }
            else
                {
                return 2;
                }
            }
        }
    for (i = 0; i <= 7; i ++)
        {
        if (!ThreeDObjects [i]->active && ThreeDObjects [i]->explode >= 35 * timestep)
            {
            ThreeDObjects [i]->explode = 0;
            int temp = ThreeDObjects [i]->fighterkills;
            ThreeDObjects [i]->aiinit ();
            if (i == 0)
                {
                playerInit ();
                }
            else
                {
                ThreeDObjects [i]->newinit (FIGHTER_P38L, i + 1, 400);
                }
            ThreeDObjects [i]->party = i / 2 + 1;
            ThreeDObjects [i]->Durability = ThreeDObjects [i]->maxDurability;
            ThreeDObjects [i]->immunity = 50 * timestep;
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->fighterkills = temp;
            ThreeDObjects [i]->killed = false;
            }
        }
    return 0;
    }

void MissionTeamsMissilesAndGuns::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionNetworkBattle01::MissionNetworkBattle01 ()
    {

    id = MISSION_NETWORKBATTLE01;

    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE01", 1024);
       strncpy (briefing, "REQUIRES INTERNET AND CONFIGURED ACCESS TO A LAC SERVER.\n10 NETWORK PLAYERS IN 2 TEAMS. FIRST TEAM TO DESTROY ENEMY\nBASE WINS. ISLAND TERRAIN. YOU WILL NEED TO LAND AND FUEL\nUP BEFORE YOU CAN LAUNCH AN ATTACK.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }

    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 11;

    LoadServerIpAddress();

    NetworkOpponent = 0;

    if (NetworkMode == 1)
      {
      if (OpenClientUdpSocket() == 0)
        {
        ConfigureClientUdpSocket();
        }
      else
        {
        }
      }
    else
      {
      display ("Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    }

void MissionNetworkBattle01::start ()
    {
    NoMissionHasYetCommenced = false;

    MissionStateNetworkBattle01 = 0;
    NetworkPlayerKilled = false;
    MissionNetworkBattle01BlueTeamVictory = false;
    MissionNetworkBattle01BlueTeamVictory = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       {
       MyNetworkId = myrandom(MaxPlayersInCurrentMission);
       MyNetworkId++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915;

    l->flatten (AirfieldXMin+30, AirfieldYMin+5, 30, 6);

    int n = 25;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 27;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 24;
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;
    n = 28;
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 26;
    ThreeDObjects [n]->tl->x = AirfieldXMin -467;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.50;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false;
    fplayer->phi = 270;
    display ("MissionNetworkBattle01::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);

    HudOnOff = 1;
    IffOnOff=0;
    MapViewOnOff = 0;
    RadarOnOff=0;
    RadarZoom = 1;
    ScreenFOVx = 1.0;
    ScreenFOVy = 1.0;

    event_IffOnOff();
    event_RadarOnOff();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();
    event_ZoomFovOut();
    event_MapViewOnOff();
    event_MapZoomIn();
    event_MapZoomIn();
    event_MapViewOnOff();

    MaxPlayersInCurrentMission = 10;

    for (i = 1; i <= 10; i ++)
        {

        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 400);
        // Preserve aircraft type for later comparison to detect changes.
        MissionNetworkBattle01PriorAircraft[i] = FIGHTER_A6M2;
        if (i%2 == 0)
          {
          ThreeDObjects [i]->party = 0;
          }
        else
          {
          ThreeDObjects [i]->party = 1;
          }
        ThreeDObjects [i]->target = NULL;
        ThreeDObjects [i]->o = &model_figv;

        ThreeDObjects [i]->tl->x = 900 + (50 * SIN(i * 360 / 11));
        ThreeDObjects [i]->tl->z = 900 + (50 * COS(i * 360 / 11));
        ThreeDObjects [i]->ammo = 1600;
        ThreeDObjects [i]->Sentient = 0;
        for (i2 = 0; i2 < missiletypes; i2 ++)
            {
            ThreeDObjects [i]->missiles [i2] = 0;
            }
        }
    NetworkApiPriorXPosition[i] = -15;
    NetworkApiPriorZPosition[i] = 45;
    NetworkApiPriorYPosition[i] = 1;
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000;
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    }

int MissionNetworkBattle01::processtimer (Uint32 dt)
    {

    int i;
    int MissionAircraftNumber;
    int MissionAirfieldNumber;
    int BytesReceived = 0;

    MissionNetworkBattleRadarTimer += DeltaTime;
    if (MissionNetworkBattleRadarTimer > 100)
       {
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          {
          ThreeDObjects[24]->phi += 10;
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          {
          ThreeDObjects[25]->phi += 10;
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot();
       }

    MissionNetworkBattle01Timer += DeltaTime;
    if (MissionNetworkBattle01Timer > 1000)
       {
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --;
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             {
             ThreeDObjects[AircraftCount]->Sentient = 0;

             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          {
          sound->setVolume (SOUND_BEEP1, 20);
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle01Timer = 0;
       RepairDamagedAirfields();
       }

    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       {
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk;
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle01::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             {
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0;
       }

    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       {
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;

       if (TeamSwitcher == false)
          {
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          {
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       {

       DegradeFlightModelDueToOrdnanceLoad();
       if (AirfieldRequested != 4)
          {
          fplayer->FuelLevel = 4.0;
          }
       if (fplayer->party == 1)
          {
          fplayer->phi -= 180;
          }
       fplayer->FlapsLevel = 4;
       fplayer->UndercarriageLevel = 1;
       event_ToggleUndercarriage;

       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[24]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[24]->tl->y += 5.4;
       ThreeDObjects[26]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[26]->tl->y += 5.6;
       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y +=0.90;

       ThreeDObjects[25]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[25]->tl->y += 5.4;
       ThreeDObjects[27]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[27]->tl->y += 5.4;
       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.90;

       if (fplayer->missiles [0] >= 6)
          {
          fplayer->tl->y += 20;
          if (AirfieldRequested >= 3)
             {
             fplayer->tl->y += 200; // This is a heavy bomber airfield. Give them extra altitude at start.
             }
          }
       fplayer->UndercarriageLevel = 0;
       event_ToggleUndercarriage;
       fplayer->FlapsLevel = 0;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 100;
          }
       }
    else
       {
       if (LandedAtSafeSpeed)
          {
          if (fplayer->id != FIGHTER_ME163)
             {
             fplayer->FuelLevel = 100;
             fplayer->maxthrust = PlayersOriginalMaxThrust;
             fplayer->RollRate = PlayersOriginalRollRate;
             lastDurability = PlayersOriginalDurability;
             if (fplayer->ammo < 1000)
                {
                fplayer->ammo = 1000;
                }
             fplayer->Durability = fplayer->maxDurability;
             ConfigureOrdnanceForOnlineMissions();
             }
          else
             {
             if (Me163LandingTimer >= 100)
                {
                Me163LandingTimer -= DeltaTime;
                sprintf (SystemMessageBufferA, "CYCLING ME163 REFUEL CREW....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 40000)
                {
                sprintf (SystemMessageBufferA, "MOUNTING TAKEOFF WHEELS....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 20000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #1....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 10000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #2....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 1100)
                {
                sound->setVolume (SOUND_BEEP2, 20);
                sound->play (SOUND_BEEP2, false);
                fplayer->FuelLevel = 100;
                fplayer->maxthrust = PlayersOriginalMaxThrust;
                fplayer->RollRate = PlayersOriginalRollRate;
                lastDurability = PlayersOriginalDurability;
                fplayer->Durability = fplayer->maxDurability;
                fplayer->ammo = 120;
                Me163LandingTimer = 60000;
                sprintf (SystemMessageBufferA, "REARMED, REFUELED, REPAIRED TO %d.\n", lastDurability);
                NewSystemMessageNeedsScrolling = true;
                display (SystemMessageBufferA, LOG_MOST);
                }
             }

            if (MyNetworkId%2)
               {
               if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 4)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
            else
               {
               if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 1)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
          }
       }
    timer += dt;

    if (timer > 15000 && MissionStateNetworkBattle01 == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       {
       if (MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle01 = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle01 = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle01 == 2 && (timer > PriorStateEndTimer +1100))
       {
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle01 = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle01 = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle01 = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle01 = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle01 = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle01 = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle01 = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle01 = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE01, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE01, false);
       MissionStateNetworkBattle01 = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle01 = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle01 == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle01 = 13;
       PriorStateEndTimer = timer;
       }

    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {

        if (ThreeDObjects [0]->explode > 500 * timestep)
           {
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           {
           sound->setVolume (SOUND_BEEP1, 20);
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(1000)>995))
            {
            GetNetworkApiPacket();
            DiscardAnyInPacketsInQueue();
            InPacket.UdpObjXPosition -=300;
            InPacket.UdpObjZPosition -=300;

            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               NetworkReceiveTimer= -1000; // Wait awhile before getting next InPacket
               }
            ThreeDObjects [MissionAircraftNumber]->aiinit ();
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 1200);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            // Preserve this aircraft type as for later comparison to detect future changes:
            MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability;
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep;
            ThreeDObjects [MissionAircraftNumber]->activate ();
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;

            ThreeDObjects [MissionAircraftNumber]->tl->x = 900 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 900 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 20 + (myrandom(40) - 20);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0;
            }
        }

    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       {
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }

    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          {
          display ("MissionNetworkBattle01 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          MissedPacketCount = 0;
          MissionNetworkBattle01LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             {

              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 395);
             }

          MissionNetworkBattle01PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          }
       }

    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       {
       NetworkTransmitTimer=0;
       MissionNetworkBattle01RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       {
       MissionEndingTimer -= dt;
       if (MissionEndingTimer < 1000)
          {
          display ("MissionNetworkBattle01::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt;
       if (MissionEndingTimer2 < 1000)
          {
          display ("MissionNetworkBattle01::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    }

void MissionNetworkBattle01::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

void MissionNetworkBattle01LoadVariablesFromNetworkApiPacket(int timer)
{

   unsigned char PlayerNumber;

   extern LacUdpApiPacket InPacket;
   display ((char *) "MissionNetworkBattle01LoadVariablesFromNetworkApiPacket()", LOG_NET);

   float XSpeed;
   float YSpeed;
   float ZSpeed;
   int DeltaTimer;

   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {

      display ("MissionNetworkBattle01LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ("Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      sprintf (SystemMessageBufferA, "NETWORK PROTOCOL CONFLICT WITH PLAYER %d.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      sound->setVolume (SOUND_BEEP1, 80);
      sound->play (SOUND_BEEP1, false);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         {
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ("This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         {
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d from player %d", InPacket.NetworkApiVersion, InPacket.UdpObjPlayerNumber);
         display (DebugBuf, LOG_ERROR);
         display ("Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1;

   PlayerNumber = InPacket.UdpObjPlayerNumber;
   display ("MissionNetworkBattle01LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);
   display ("Sentient Count =", LOG_NET);
   sprintf (DebugBuf, "%d", ThreeDObjects[PlayerNumber]->Sentient);
   display (DebugBuf, LOG_NET);
   if (PlayerNumber > 10)
      {
      display ("MissionNetworkBattle01LoadVariablesFromNetworkApiPacket(): network PlayerNumber ID > 10 error.", LOG_MOST);
      }
   if (ThreeDObjects[PlayerNumber]->Sentient <= 10)
      {
      ThreeDObjects[PlayerNumber]->Sentient++;
      if (ThreeDObjects[PlayerNumber]->Sentient == 4)
         {
         if (MissionEntryLatch[PlayerNumber] == 0)
            {
            if (ThreeDObjects[PlayerNumber]->Durability > 600)
               {
               sprintf (SystemMessageBufferA, "STRATEGIC BOMBER # %d CROSSED INTO RADAR RANGE.", PlayerNumber);
               NewSystemMessageNeedsScrolling = true;
               MissionEntryLatch[PlayerNumber] = 1;
               sound->setVolume (SOUND_BEEP1, 20);
               sound->play (SOUND_BEEP1, false);
               }
            }
         }
      }
   if (ThreeDObjects[PlayerNumber]->Sentient < 2)
      {
      ThreeDObjects[PlayerNumber]->Sentient = 2;
      return; // Discard the first packet.
      }

   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;

   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;

   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;

   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   NetDeltaX = InPacket.UdpObjXPosition - NetworkApiPriorXPosition[PlayerNumber];
   NetDeltaY = InPacket.UdpObjYPosition - NetworkApiPriorYPosition[PlayerNumber];
   NetDeltaZ = InPacket.UdpObjZPosition - NetworkApiPriorZPosition[PlayerNumber];
   DeltaTimer = timer - NetworkApiPriorTimer[PlayerNumber];
   XSpeed = NetDeltaX/(float)DeltaTimer;
   YSpeed = NetDeltaY/(float)DeltaTimer;
   ZSpeed = NetDeltaZ/(float)DeltaTimer;
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;
   ProcessUdpObjFlightDetails();

   NetworkApiPriorXPosition[PlayerNumber] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[PlayerNumber] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[PlayerNumber] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[PlayerNumber]     = timer;
   if (InPacket.UdpObjDamageId == MyNetworkId)
      {
      ThreeDObjects[0]->Durability -= InPacket.UdpObjDamageAmount; // Damage this player
      sprintf (SystemMessageBufferA, "PLAYER %d DAMAGED YOUR AIRCRAFT.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      }
   else if (InPacket.UdpObjDamageAmount > 0.0)
      { // Get here if received damage claim addresses some other bot or player
      ThreeDObjects[InPacket.UdpObjDamageId]->Durability -= InPacket.UdpObjDamageAmount; // Damage addressed mission aircraft
      if (InPacket.UdpObjDamageId > 0 && InPacket.UdpObjDamageId <=10)
         {
         if (InPacket.UdpObjDamageAmount < 1000)
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DAMAGED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         else
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DESTROYED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         NewSystemMessageNeedsScrolling = true;
         }
       else if (InPacket.UdpObjDamageId >= 11)
         {
         if (InPacket.UdpObjDamageAmount < 32000)
            {
            sprintf  (
                     SystemMessageBufferA,
                     "AIRCRAFT %d DAMAGED AIRFIELD %d BY %5.0f KILOJOULES.",
                     InPacket.UdpObjPlayerNumber,
                     InPacket.UdpObjDamageId,
                     InPacket.UdpObjDamageAmount
                     );
            NewSystemMessageNeedsScrolling = true;
            }
         else
            {
            sound->setVolume (SOUND_BEEP1, 20);
            sound->play (SOUND_BEEP1, false);
            ThreeDObjects[InPacket.UdpObjDamageId]->Durability = -4000;
            if (InPacket.UdpObjDamageId%2)
               {
               if (InPacket.UdpObjDamageId == 29)
                  {
                  sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01BlueTeamVictory = true;
                  if (fplayer->party == 1)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            else
               {
               if (InPacket.UdpObjDamageId == 28)
                  {
                  sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01RedTeamVictory = true;
                  if (fplayer->party == 0)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            }
         }
      }
   if (InPacket.UdpObjDamageAmount < 0.0)
      { // Get here if received damage represents a static update
      InPacket.UdpObjDamageAmount *= -1.0;
      if (InPacket.UdpObjDamageAmount < ThreeDObjects[InPacket.UdpObjDamageId]->Durability)
         {
         ThreeDObjects[InPacket.UdpObjDamageId]->Durability = InPacket.UdpObjDamageAmount; // Update addressed 3d Object with lower static value.
         }
      }
}

bool MissionNetworkBattle01RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ("MissionNetworkBattle01RetrieveFirstDamageDescription()", LOG_NET);

for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   {
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
}

MissionNetworkBattle02::MissionNetworkBattle02 ()
    {

    id = MISSION_NETWORKBATTLE02;

    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE02", 1024);
       strncpy (briefing, "REQUIRES INTERNET AND CONFIGURED ACCESS TO A LAC SERVER.\n10 NETWORK PLAYERS IN 2 TEAMS. FIRST TEAM TO DESTROY ENEMY BASE\nWINS. DESERT TERRAIN. YOU WILL NEED TO LAND AND FUEL UP BEFORE\nYOU CAN LAUNCH AN ATTACK.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }

    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 11;

    LoadServerIpAddress();

    NetworkOpponent = 0;

    if (NetworkMode == 1)
      {
      if (OpenClientUdpSocket() == 0)
        {
        ConfigureClientUdpSocket();
        }
      else
        {
        }
      }
    else
      {
      display ("Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    }

void MissionNetworkBattle02::start ()
    {
    NoMissionHasYetCommenced = false;

    MissionStateNetworkBattle02 = 0;
    NetworkPlayerKilled = false;
    MissionNetworkBattle02BlueTeamVictory = false;
    MissionNetworkBattle02BlueTeamVictory = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       {
       MyNetworkId = myrandom(MaxPlayersInCurrentMission);
       MyNetworkId++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_DESERT, NULL);
    SeaLevel = -12.915;

    l->flatten (AirfieldXMin+30, AirfieldYMin+5, 30, 6);

    int n = 25;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 24;
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 28;
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false;
    fplayer->phi = 270;
    display ("MissionNetworkBattle02::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);

    HudOnOff = 1;
    IffOnOff=0;
    MapViewOnOff = 0;
    RadarOnOff=0;
    RadarZoom = 1;
    ScreenFOVx = 1.0;
    ScreenFOVy = 1.0;

    event_IffOnOff();
    event_RadarOnOff();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();
    event_ZoomFovOut();
    event_MapViewOnOff();
    event_MapZoomIn();
    event_MapZoomIn();
    event_MapViewOnOff();
    MaxPlayersInCurrentMission = 10;

    for (i = 1; i <= 10; i ++)
        {

        ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 400);
        // Preserve aircraft type for later comparison to detect changes.
        MissionNetworkBattle02PriorAircraft[i] = FIGHTER_A6M2;
        if (i%2 == 0)
          {
          ThreeDObjects [i]->party = 0;
          }
        else
          {
          ThreeDObjects [i]->party = 1;
          }
        ThreeDObjects [i]->target = NULL;
        ThreeDObjects [i]->o = &model_figv;

        ThreeDObjects [i]->tl->x = 900 + (50 * SIN(i * 360 / 11));
        ThreeDObjects [i]->tl->z = 900 + (50 * COS(i * 360 / 11));
        ThreeDObjects [i]->ammo = 1600;
        ThreeDObjects [i]->Sentient = 0;
        for (i2 = 0; i2 < missiletypes; i2 ++)
            {
            ThreeDObjects [i]->missiles [i2] = 0;
            }
        }
    NetworkApiPriorXPosition[i] = -15;
    NetworkApiPriorZPosition[i] = 45;
    NetworkApiPriorYPosition[i] = 1;
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000;
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    }

int MissionNetworkBattle02::processtimer (Uint32 dt)
    {

    int i;
    int MissionAircraftNumber;
    int MissionAirfieldNumber;
    int BytesReceived = 0;

    MissionNetworkBattleRadarTimer += DeltaTime;
    if (MissionNetworkBattleRadarTimer > 100)
       {
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          {
          ThreeDObjects[24]->phi += 10;
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          {
          ThreeDObjects[25]->phi += 10;
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot();
       }

    MissionNetworkBattle02Timer += DeltaTime;
    if (MissionNetworkBattle02Timer > 1000)
       {
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --;
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             {
             ThreeDObjects[AircraftCount]->Sentient = 0;

             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          {
          sound->setVolume (SOUND_BEEP1, 20);
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle02Timer = 0;
       RepairDamagedAirfields();
       }

    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       {
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk;
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle02::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             {
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0;
       }

    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       {
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;

       if (TeamSwitcher == false)
          {
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          {
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       {

       DegradeFlightModelDueToOrdnanceLoad();
       if (AirfieldRequested != 4)
          {
          fplayer->FuelLevel = 4.0;
          }
       if (fplayer->party == 1)
          {
          fplayer->phi -= 180;
          }
       fplayer->FlapsLevel = 4;
       fplayer->UndercarriageLevel = 1;
       event_ToggleUndercarriage;

       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y += 0.9;
       ThreeDObjects[24]->tl->y = ThreeDObjects[28]->tl->y + 2.0;

       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.9;
       ThreeDObjects[25]->tl->y = ThreeDObjects[29]->tl->y + 2.0;

       if (fplayer->missiles [0] >= 6)
          {
          fplayer->tl->y += 20;
          if (AirfieldRequested >= 3)
             {
             fplayer->tl->y += 200; // This is a heavy bomber airfield. Give them extra altitude at start.
             }
          }
       fplayer->UndercarriageLevel = 0;
       event_ToggleUndercarriage;
       fplayer->FlapsLevel = 0;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 100;
          }
       }
    else
       {
       if (LandedAtSafeSpeed)
          {
          if (fplayer->id != FIGHTER_ME163)
             {
             fplayer->FuelLevel = 100;
             fplayer->maxthrust = PlayersOriginalMaxThrust;
             fplayer->RollRate = PlayersOriginalRollRate;
             lastDurability = PlayersOriginalDurability;
             if (fplayer->ammo < 1000)
                {
                fplayer->ammo = 1000;
                }
             fplayer->Durability = fplayer->maxDurability;
             ConfigureOrdnanceForOnlineMissions();
             }
          else
             {
             if (Me163LandingTimer >= 100)
                {
                Me163LandingTimer -= DeltaTime;
                sprintf (SystemMessageBufferA, "CYCLING ME163 REFUEL CREW....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 40000)
                {
                sprintf (SystemMessageBufferA, "MOUNTING TAKEOFF WHEELS....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 20000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #1....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 10000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #2....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 1100)
                {
                sound->setVolume (SOUND_BEEP2, 20);
                sound->play (SOUND_BEEP2, false);
                fplayer->FuelLevel = 100;
                fplayer->maxthrust = PlayersOriginalMaxThrust;
                fplayer->RollRate = PlayersOriginalRollRate;
                lastDurability = PlayersOriginalDurability;
                fplayer->Durability = fplayer->maxDurability;
                fplayer->ammo = 120;
                Me163LandingTimer = 60000;
                sprintf (SystemMessageBufferA, "REARMED, REFUELED, REPAIRED TO %d.\n", lastDurability);
                NewSystemMessageNeedsScrolling = true;
                display (SystemMessageBufferA, LOG_MOST);
                }
             }

            if (MyNetworkId%2)
               {
               if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 4)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
            else
               {
               if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 1)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
          }
       }
    timer += dt;

    if (timer > 15000 && MissionStateNetworkBattle02 == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       {
       if (MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle02 = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle02 = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle02 == 2 && (timer > PriorStateEndTimer +1100))
       {
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle02 = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle02 = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle02 = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle02 = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle02 = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle02 = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle02 = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle02 = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE02, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE02, false);
       MissionStateNetworkBattle02 = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle02 = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle02 == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle02 = 13;
       PriorStateEndTimer = timer;
       }

    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {

        if (ThreeDObjects [0]->explode > 500 * timestep)
           {
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           {
           sound->setVolume (SOUND_BEEP1, 20);
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(1000)>995))
            {
            GetNetworkApiPacket();
            DiscardAnyInPacketsInQueue();
            InPacket.UdpObjXPosition -=300;
            InPacket.UdpObjZPosition -=300;

            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               NetworkReceiveTimer= -2000; // Wait awhile before getting next InPacket
               }
            ThreeDObjects [MissionAircraftNumber]->aiinit ();
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 1200);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            // Preserve this aircraft type as for later comparison to detect future changes:
            MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability;
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep;
            ThreeDObjects [MissionAircraftNumber]->activate ();
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;

            ThreeDObjects [MissionAircraftNumber]->tl->x = 900 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 900 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 20 + (myrandom(40) - 20);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0;
            }
        }

    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       {
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }

    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          {
          display ("MissionNetworkBattle02 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          MissedPacketCount = 0;
          MissionNetworkBattle02LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             {

              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 395);
             }

          MissionNetworkBattle02PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          }
       }

    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       {
       NetworkTransmitTimer=0;
       MissionNetworkBattle02RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       {
       MissionEndingTimer -= dt;
       if (MissionEndingTimer < 1000)
          {
          display ("MissionNetworkBattle02::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt;
       if (MissionEndingTimer2 < 1000)
          {
          display ("MissionNetworkBattle02::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    }

void MissionNetworkBattle02::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

void MissionNetworkBattle02LoadVariablesFromNetworkApiPacket(int timer)
{

   unsigned char PlayerNumber;

   extern LacUdpApiPacket InPacket;
   display ((char *) "MissionNetworkBattle02LoadVariablesFromNetworkApiPacket()", LOG_NET);

   float XSpeed;
   float YSpeed;
   float ZSpeed;
   int DeltaTimer;

   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {

      display ("MissionNetworkBattle02LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ("Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      sprintf (SystemMessageBufferA, "NETWORK PROTOCOL CONFLICT WITH PLAYER %d.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      sound->setVolume (SOUND_BEEP1, 80);
      sound->play (SOUND_BEEP1, false);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         {
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ("This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         {
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d from player %d", InPacket.NetworkApiVersion, InPacket.UdpObjPlayerNumber);
         display (DebugBuf, LOG_ERROR);
         display ("Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1;

   PlayerNumber = InPacket.UdpObjPlayerNumber;
   display ("MissionNetworkBattle02LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);
   display ("Sentient Count =", LOG_NET);
   sprintf (DebugBuf, "%d", ThreeDObjects[PlayerNumber]->Sentient);
   display (DebugBuf, LOG_NET);
   if (PlayerNumber > 10)
      {
      display ("MissionNetworkBattle02LoadVariablesFromNetworkApiPacket(): network PlayerNumber ID > 10 error.", LOG_MOST);
      }
   if (ThreeDObjects[PlayerNumber]->Sentient <= 10)
      {
      ThreeDObjects[PlayerNumber]->Sentient++;
      if (ThreeDObjects[PlayerNumber]->Sentient == 4)
         {
         if (MissionEntryLatch[PlayerNumber] == 0)
            {
            if (ThreeDObjects[PlayerNumber]->Durability > 600)
               {
               sprintf (SystemMessageBufferA, "STRATEGIC BOMBER # %d CROSSED INTO RADAR RANGE.", PlayerNumber);
               NewSystemMessageNeedsScrolling = true;
               MissionEntryLatch[PlayerNumber] = 1;
               sound->setVolume (SOUND_BEEP1, 20);
               sound->play (SOUND_BEEP1, false);
               }
            }
         }
      }
   if (ThreeDObjects[PlayerNumber]->Sentient < 2)
      {
      ThreeDObjects[PlayerNumber]->Sentient = 2;
      return; // Discard the first packet.
      }

   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;

   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;

   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;

   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   NetDeltaX = InPacket.UdpObjXPosition - NetworkApiPriorXPosition[PlayerNumber];
   NetDeltaY = InPacket.UdpObjYPosition - NetworkApiPriorYPosition[PlayerNumber];
   NetDeltaZ = InPacket.UdpObjZPosition - NetworkApiPriorZPosition[PlayerNumber];
   DeltaTimer = timer - NetworkApiPriorTimer[PlayerNumber];
   XSpeed = NetDeltaX/(float)DeltaTimer;
   YSpeed = NetDeltaY/(float)DeltaTimer;
   ZSpeed = NetDeltaZ/(float)DeltaTimer;
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;
   ProcessUdpObjFlightDetails();

   NetworkApiPriorXPosition[PlayerNumber] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[PlayerNumber] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[PlayerNumber] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[PlayerNumber]     = timer;
   if (InPacket.UdpObjDamageId == MyNetworkId)
      {
      ThreeDObjects[0]->Durability -= InPacket.UdpObjDamageAmount; // Damage this player
      sprintf (SystemMessageBufferA, "PLAYER %d DAMAGED YOUR AIRCRAFT.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      }
   else if (InPacket.UdpObjDamageAmount > 0.0)
      { // Get here if received damage claim addresses some other bot or player
      ThreeDObjects[InPacket.UdpObjDamageId]->Durability -= InPacket.UdpObjDamageAmount; // Damage addressed mission aircraft
      if (InPacket.UdpObjDamageId > 0 && InPacket.UdpObjDamageId <=10)
         {
         if (InPacket.UdpObjDamageAmount < 1000)
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DAMAGED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         else
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DESTROYED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         NewSystemMessageNeedsScrolling = true;
         }
       else if (InPacket.UdpObjDamageId >= 11)
         {
         if (InPacket.UdpObjDamageAmount < 32000)
            {
            sprintf  (
                     SystemMessageBufferA,
                     "AIRCRAFT %d DAMAGED AIRFIELD %d BY %5.0f KILOJOULES.",
                     InPacket.UdpObjPlayerNumber,
                     InPacket.UdpObjDamageId,
                     InPacket.UdpObjDamageAmount
                     );
            display (SystemMessageBufferA, LOG_MOST);
            NewSystemMessageNeedsScrolling = true;
            }
         else
            {
            sound->setVolume (SOUND_BEEP1, 20);
            sound->play (SOUND_BEEP1, false);
            ThreeDObjects[InPacket.UdpObjDamageId]->Durability = -4000;
            if (InPacket.UdpObjDamageId%2)
               {
               if (InPacket.UdpObjDamageId == 29)
                  {
                  sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01BlueTeamVictory = true;
                  if (fplayer->party == 1)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            else
               {
               if (InPacket.UdpObjDamageId == 28)
                  {
                  sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01RedTeamVictory = true;
                  if (fplayer->party == 0)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            }
         }
      }
   if (InPacket.UdpObjDamageAmount < 0.0)
      { // Get here if received damage represents a static update
      InPacket.UdpObjDamageAmount *= -1.0;
      if (InPacket.UdpObjDamageAmount < ThreeDObjects[InPacket.UdpObjDamageId]->Durability)
         {
         ThreeDObjects[InPacket.UdpObjDamageId]->Durability = InPacket.UdpObjDamageAmount; // Update addressed 3d Object with lower static value.
         }
      }
}

bool MissionNetworkBattle02RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ("MissionNetworkBattle02RetrieveFirstDamageDescription()", LOG_NET);

for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   {
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
}

MissionNetworkBattle03::MissionNetworkBattle03 ()
    {

    id = MISSION_NETWORKBATTLE03;

    if (NetworkMode == 0)
       {
       strncpy (name, "WRONG NETWORK MODE!", 1024);
       strncpy (briefing, "LACCONFIG.TXT FILE MUST SET MODE 1 AND A SERVER ADDRESS.", 1024);
       }
    else
       {
       strncpy (name, "NETWORKBATTLE03", 1024);
       strncpy (briefing, "REQUIRES INTERNET AND CONFIGURED ACCESS TO A LAC SERVER.\n10 NETWORK PLAYERS IN 2 TEAMS. FIRST TEAM TO DESTROY ENEMY BASE\nWINS. ISLAND TERRAIN. YOU ARE ALREADY FUELED UP AND READY TO\nLAUNCH AN ATTACK.", 1024);
       }
    selfighter [0] = DefaultAircraft;
    selfighter [1] = FIGHTER_A6M2;
    selfighter [2] = FIGHTER_P51D;
    selweapons = 0;
    if (NetworkMode == 0)
       {
       selfighters = 0;
       }
    else
       {
       selfighters = 2;
       }

    NetworkReceiveTimerInterval =  NetworkTransmitTimerInterval/ 11;

    LoadServerIpAddress();

    NetworkOpponent = 0;

    if (NetworkMode == 1)
      {
      if (OpenClientUdpSocket() == 0)
        {
        ConfigureClientUdpSocket();
        }
      else
        {
        }
      }
    else
      {
      display ("Incorrect network mode configured in LacConfig.txt file.", LOG_ERROR);
      sound->setVolume (SOUND_BEEP1, 128);
      sound->play (SOUND_BEEP1, false);
      }
    }

void MissionNetworkBattle03::start ()
    {
    NoMissionHasYetCommenced = false;

    MissionStateNetworkBattle03 = 0;
    NetworkPlayerKilled = false;
    MissionNetworkBattle03BlueTeamVictory = false;
    MissionNetworkBattle03BlueTeamVictory = false;
    if ((MyNetworkId == 0) || (MyNetworkId > MaxPlayersInCurrentMission))
       {
       MyNetworkId = myrandom(MaxPlayersInCurrentMission);
       MyNetworkId ++;
       }
    int i, i2;
    day = 1;
    clouds = 2;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 25;
    heading = 220;
    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_SEA, NULL);
    SeaLevel = -12.915;

    l->flatten (AirfieldXMin+30, AirfieldYMin+5, 30, 6);

    int n = 25;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 41.0;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.2;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 27;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 29;
    ThreeDObjects [n]->tl->x = AirfieldXMin + 46.5;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 8;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 1;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 24;
    ThreeDObjects [n]->tl->x = AirfieldXMin -470.58;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 6.25;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_RadarReflector;
    ThreeDObjects [n]->newinit (STATIC_RADARREFLECTOR, 0, 400);
    ThreeDObjects [n]->impact = 0.0;
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.66;

    n = 28;
    ThreeDObjects [n]->tl->x = -304;
    ThreeDObjects [n]->tl->z = 11.0;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_Airfield00;
    ThreeDObjects [n]->newinit (STATIC_AIRFIELD00, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 6.0;

    n = 26;
    ThreeDObjects [n]->tl->x = AirfieldXMin -467;
    ThreeDObjects [n]->tl->z = AirfieldYMin + 28;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_depot1;
    ThreeDObjects [n]->newinit (STATIC_DEPOT1, 0, 400);
    ThreeDObjects [n]->party = 0;
    ThreeDObjects [n]->Durability = ThreeDObjects [n]->maxDurability;
    ThreeDObjects [n]->zoom = 0.50;

    playerInit ();
    PlayerAircraftType = fplayer->id;
    MissionRunning = false;
    fplayer->phi = 270;
    display ("MissionNetworkBattle03::start setting PlayerAircraftType to: ", LOG_MOST);
    sprintf (DebugBuf, "%d", PlayerAircraftType);
    display (DebugBuf, LOG_MOST);

    HudOnOff = 1;
    IffOnOff=0;
    MapViewOnOff = 0;
    RadarOnOff=0;
    RadarZoom = 1;
    ScreenFOVx = 1.0;
    ScreenFOVy = 1.0;

    event_IffOnOff();
    event_RadarOnOff();
    if (!HudLadderBarsOnOff)
        {
        event_HudLadderBarsOnOff();
        }
    event_ZoomFovOut();
    event_ZoomFovOut();
    event_MapViewOnOff();
    event_MapZoomIn();
    event_MapZoomIn();
    event_MapViewOnOff();
    NetworkApiPriorXPosition[i] = -15;
    NetworkApiPriorZPosition[i] = 45;
    NetworkApiPriorYPosition[i] = 1;
    texttimer = 0;
    MissionOutPacketCount = 0;
    MissionIdNegotiationCount = 0;
    NetworkTransmitTimer = -1000;
    ConfigureOrdnanceForOnlineMissions();
    UpdateOnlineScoreLogFileWithNewSorties();
    ArmPlayerAtRequestedField();
    }

int MissionNetworkBattle03::processtimer (Uint32 dt)
    {

    int i;
    int MissionAircraftNumber;
    int MissionAirfieldNumber;
    int BytesReceived = 0;
    MissionNetworkBattle03Timer += DeltaTime;
    MissionNetworkBattleRadarTimer += DeltaTime;
    if (MissionNetworkBattleRadarTimer > 100)
       {
       MissionNetworkBattleRadarTimer = 0;
       if (ThreeDObjects[24]->Durability > 200)
          {
          ThreeDObjects[24]->phi += 10;
          }
       if (ThreeDObjects[24]->phi >= 360)
          {
          ThreeDObjects[24]->phi = 0;
          }
       if (ThreeDObjects[25]->Durability > 200)
          {
          ThreeDObjects[25]->phi += 10;
          }
       if (ThreeDObjects[25]->phi >= 360)
          {
          ThreeDObjects[25]->phi = 0;
          }
       AutoPilot();
       }

    if (MissionNetworkBattle03Timer > 1000)
       {
       int AircraftCount;
       for (AircraftCount =0; AircraftCount<=10; AircraftCount++)
          {
          if (ThreeDObjects[AircraftCount]->Sentient >1)
             {
             ThreeDObjects [AircraftCount]->Sentient --;
             }
          else if ((ThreeDObjects[AircraftCount]->Sentient == 1) && (MissionEntryLatch[AircraftCount] == 1))
             {
             ThreeDObjects[AircraftCount]->Sentient = 0;

             MissionEntryLatch [AircraftCount] = 0;
             }
          }
       if (MissionIdNegotiationCount > 32)
          {
          sound->setVolume (SOUND_BEEP1, 20);
          sound->play (SOUND_BEEP1, false);
          sprintf (SystemMessageBufferA, "THIS MISSION IS FULL. TRY ANOTHER.");
          NewSystemMessageNeedsScrolling = true;
          }
       MissionNetworkBattle03Timer = 0;
       RepairDamagedAirfields();
       }

    BattleDamageRiskTimer += DeltaTime;
    if ((BattleDamageRiskTimer > 5000) || (BattleDamageRiskTimer > (abs)(myrandom(131072))))
       {
       BattleDamageRiskTimer = 0;
       CalcDamageRiskFromNearbyOpposition();
       ThreeDObjects[0]->Durability -= CalculatedDamageDueToCurrentRisk;
       if (CalculatedDamageDueToCurrentRisk > 0)
          {
          sprintf (DebugBuf, "MissionNetworkBattle03::processTimer() fplayer->Durability is now %f.", fplayer->Durability);
          display (DebugBuf, LOG_MOST);
          if (fplayer->Durability < 0)
             {
             UpdateOnlineScoreLogFileWithCalculatedRisks();
             }
          }
       CalculatedDamageDueToCurrentRisk = 0;
       }

    StaticObjectUpdateTimer += DeltaTime;
    if (StaticObjectUpdateTimer > 4000)
       {
       static bool TeamSwitcher = false;
       float TempFloat1;
       StaticObjectUpdateTimer = 0;

       if (TeamSwitcher == false)
          {
          TeamSwitcher = true;
          if ((ThreeDObjects[28]->Durability > 0) && ((ThreeDObjects[28]->Durability) < (ThreeDObjects[28]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[28]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[28]->DamageInNetQueue = TempFloat1;
             }
          }
       else
          {
          TeamSwitcher = false;
          if ((ThreeDObjects[29]->Durability > 0) && ((ThreeDObjects[29]->Durability) < (ThreeDObjects[29]->maxDurability)))
             {
             TempFloat1 = ThreeDObjects[29]->Durability;
             TempFloat1 *= -1.0;
             ThreeDObjects[29]->DamageInNetQueue = TempFloat1;
             }
          }
       }
    if (!MissionRunning)
       {

       DegradeFlightModelDueToOrdnanceLoad();
       if (fplayer->id != FIGHTER_ME163)
          {
          fplayer->FuelLevel = 94.0;
          }
       else
          {
          fplayer->FuelLevel = 15.0;
          }
       if (fplayer->party == 1)
          {
          fplayer->phi -= 180;
          }
       fplayer->FlapsLevel = 4;
       fplayer->UndercarriageLevel = 1;
       event_ToggleUndercarriage;

       fplayer->tl->y = l->getHeight(AirfieldXMin, AirfieldYMin);
       fplayer->tl->y += 8.0;
       /*
       *
       * The following block of code attempts to cause the player to
       * spawn with zero airspeed. It works, but it isn't sufficient
       * becuase something else causes airspeed to build back to
       * about 160 MPH within about one second after spawning.
       *
       * However, it may be useful in the future, so it ought to
       * be preserved.
       */
       fplayer->realspeed = 0.0;
       fplayer->InertiallyDampenedPlayerSpeed = 0.0;
       ClearSpeedHistoryArrayFlag = true;
       fplayer->accx = 0.0;
       fplayer->accy = 0.0;
       fplayer->accz = 0.0;

       ThreeDObjects[24]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[24]->tl->y += 5.4;
       ThreeDObjects[26]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[26]->tl->y += 5.6;
       ThreeDObjects[28]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[28]->tl->y +=0.90;

       ThreeDObjects[25]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[25]->tl->y += 5.4;
       ThreeDObjects[27]->tl->y = l->getExactHeight(-467, 28);
       ThreeDObjects[27]->tl->y += 5.4;
       ThreeDObjects[29]->tl->y = l->getHeight(AirfieldXMin+5, AirfieldYMin+5);
       ThreeDObjects[29]->tl->y +=0.90;

       if (fplayer->missiles [0] >= 6)
          {
          fplayer->tl->y += 20;
          sprintf (DebugBuf, "MissionNetworkBattle03::processtimer() Heavy bomber launching. AirfieldRequested = %d", AirfieldRequested);
          display (DebugBuf, LOG_MOST);
          if (AirfieldRequested >= 3)
             {
             display ("MissionNetworkBattle03::processtimer() Increasing heavy bomber spawn altitude from Field 4", LOG_MOST);
             fplayer->tl->y += 200; // This is a bomber airfield. Give them extra altitude at start.
             }
          }
       fplayer->UndercarriageLevel = 0;
       event_ToggleUndercarriage;
       fplayer->FlapsLevel = 0;
       //fplayer->FlapsLevel = 1;
       MissionRunning = true;
       for (i=1; i<=10; i++)
          { // Start bots at medium altitude
          ThreeDObjects[i]->tl->y += 100;
          }
       }
    else
       {
       if (LandedAtSafeSpeed)
          {
          if (fplayer->id != FIGHTER_ME163)
             {
             fplayer->FuelLevel = 100;
             fplayer->maxthrust = PlayersOriginalMaxThrust;
             fplayer->RollRate = PlayersOriginalRollRate;
             lastDurability = PlayersOriginalDurability;
             if (fplayer->ammo < 1000)
                {
                fplayer->ammo = 1000;
                }
             fplayer->Durability = fplayer->maxDurability;
             ConfigureOrdnanceForOnlineMissions();
             }
          else
             {
             if (Me163LandingTimer >= 100)
                {
                Me163LandingTimer -= DeltaTime;
                sprintf (SystemMessageBufferA, "CYCLING ME163 REFUEL CREW....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 40000)
                {
                sprintf (SystemMessageBufferA, "MOUNTING TAKEOFF WHEELS....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 20000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #1....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 10000)
                {
                sprintf (SystemMessageBufferA, "PUMPING FUEL FROM TRUCK #2....");
                NewSystemMessageNeedsScrolling = true;
                }
             if (Me163LandingTimer < 1100)
                {
                sound->setVolume (SOUND_BEEP2, 20);
                sound->play (SOUND_BEEP2, false);
                fplayer->FuelLevel = 100;
                fplayer->maxthrust = PlayersOriginalMaxThrust;
                fplayer->RollRate = PlayersOriginalRollRate;
                lastDurability = PlayersOriginalDurability;
                fplayer->Durability = fplayer->maxDurability;
                fplayer->ammo = 120;
                Me163LandingTimer = 60000;
                sprintf (SystemMessageBufferA, "REARMED, REFUELED, REPAIRED TO %d.\n", lastDurability);
                NewSystemMessageNeedsScrolling = true;
                display (SystemMessageBufferA, LOG_MOST);
                }
             }

            if (MyNetworkId%2)
               {
               if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 4)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
            else
               {
               if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
                  {
                  DetermineCurrentAirfield();
                  if (fplayer->HistoricPeriod > 1 && AirfieldChosenForLanding == 1)
                     {
                     sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                     NewSystemMessageNeedsScrolling = true;
                     fplayer->FuelLevel *= 0.2;
                     fplayer->ammo /= 5;

                     int i2;
                     for (i2 = 0; i2 < missiletypes; i2 ++)
                         {
                         if (i2 == 0)
                            {
                            fplayer->missiles [i2] /= 3;
                            }
                         if (i2 == 5)
                            {
                            fplayer->missiles [i2] /=3;
                            }
                         }
                     }
                  else
                     {

                     }
                  }
               else
                  {
                  }
               }
          }
       }
    timer += dt;

    if (timer > 15000 && MissionStateNetworkBattle03 == 0 && NetworkTransmitTimer >= NetworkTransmitTimerInterval)
       {
       if (MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 240);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 240);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle03 = 1;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 1 && (timer > PriorStateEndTimer +600))
       {
       switch (MyNetworkId)
          {
          case 1:
              {
              sound->setVolume (SOUND_DIGIT001, 180);
              sound->play (SOUND_DIGIT001, false);
              break;
              }
          case 2:
              {
              sound->setVolume (SOUND_DIGIT002, 180);
              sound->play (SOUND_DIGIT002, false);
              break;
              }
          case 3:
              {
              sound->setVolume (SOUND_DIGIT003, 180);
              sound->play (SOUND_DIGIT003, false);
              break;
              }
          case 4:
              {
              sound->setVolume (SOUND_DIGIT004, 180);
              sound->play (SOUND_DIGIT004, false);
              break;
              }
          case 5:
              {
              sound->setVolume (SOUND_DIGIT005, 180);
              sound->play (SOUND_DIGIT005, false);
              break;
              }
          case 6:
              {
              sound->setVolume (SOUND_DIGIT006, 180);
              sound->play (SOUND_DIGIT006, false);
              break;
              }
          case 7:
              {
              sound->setVolume (SOUND_DIGIT007, 180);
              sound->play (SOUND_DIGIT007, false);
              break;
              }
          case 8:
              {
              sound->setVolume (SOUND_DIGIT008, 180);
              sound->play (SOUND_DIGIT008, false);
              break;
              }
          case 9:
              {
              sound->setVolume (SOUND_DIGIT009, 180);
              sound->play (SOUND_DIGIT009, false);
              break;
              }
          case 10:
              {
              sound->setVolume (SOUND_DIGIT010, 180);
              sound->play (SOUND_DIGIT010, false);
              break;
              }
          default:
              {
              break;
              }
          }
        MissionStateNetworkBattle03 = 2;
        PriorStateEndTimer = timer;
        }
    if (MissionStateNetworkBattle03 == 2 && (timer > PriorStateEndTimer +1100))
       {

       if (MyNetworkId%2)
          {
          if (ThreeDObjects[29]->Durability < ThreeDObjects[29]->maxDurability * 0.4)
             {
             display ("MissionNetworkBattle03 state 2: Player's RedTeam HQ is heavily damaged.", LOG_MOST);
             if (fplayer->HistoricPeriod > 1)
                {
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;

                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       {
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       {
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                {
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             {
             display ("MissionNetworkBattle03 state 2: Player's RedTeam HQ is NOT heavily damaged.", LOG_MOST);
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       else
          {
          if (ThreeDObjects[28]->Durability < ThreeDObjects[28]->maxDurability * 0.4)
             {
             if (fplayer->HistoricPeriod > 1)
                {
                sprintf (SystemMessageBufferA, "LOW AMMO & FUEL FOR LATE-WAR PLANES DUE TO HQ DAMAGE.");
                NewSystemMessageNeedsScrolling = true;
                fplayer->FuelLevel *= 0.2;
                fplayer->ammo /= 5;

                int i2;
                for (i2 = 0; i2 < missiletypes; i2 ++)
                    {
                    if (i2 == 0)
                       {
                       fplayer->missiles [i2] /= 3;
                       }
                    if (i2 == 5)
                       {
                       fplayer->missiles [i2] /=3;
                       }
                    }
                }
             else
                {
                sprintf (SystemMessageBufferA, "WE HAVE FUEL/ORDNANCE FOR THAT AIRCRAFT.");
                NewSystemMessageNeedsScrolling = true;
                }
             }
          else
             {
             sprintf (SystemMessageBufferA, "OUR HQ FACILITIES PERMIT FULL FUEL/ORDNANCE.");
             NewSystemMessageNeedsScrolling = true;
             }
          }
       sound->setVolume (SOUND_YOUHAVEAUTHORITYTOPROCEED, 180);
       sound->play (SOUND_YOUHAVEAUTHORITYTOPROCEED, false);
       MissionStateNetworkBattle03 = 3;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 3 && (timer > PriorStateEndTimer +2700))
       {
       sound->setVolume (SOUND_WEAPONSFREE, 180);
       sound->play (SOUND_WEAPONSFREE, false);
       MissionStateNetworkBattle03 = 4;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 4 && (timer > PriorStateEndTimer +3000))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "LAC SERVER IS AT LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, 180);
       sound->play (SOUND_MISSIONTELEMETRYAVAILABLEFROMLACSERVERAT, false);
       MissionStateNetworkBattle03 = 5;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 5 && (timer > PriorStateEndTimer +3300))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle03 = 6;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 6 && (timer > PriorStateEndTimer +3700))
       {
       sound->setVolume (SOUND_BEEP1, 20);
       sound->play (SOUND_BEEP1, false);
       sprintf (SystemMessageBufferA, "TUNE MUMBLE RADIO: LACSERVER2.LINUXAIRCOMBAT.COM");
       NewSystemMessageNeedsScrolling = true;
       sound->setVolume (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, 180);
       sound->play (SOUND_GLOBALASSIGNMENTAUDIOATMUMBLE, false);
       MissionStateNetworkBattle03 = 7;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 7 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_BOSENETDOTNODASHIPDOTBIZ, 180);
       sound->play (SOUND_BOSENETDOTNODASHIPDOTBIZ, false);
       MissionStateNetworkBattle03 = 8;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 8 && (timer > PriorStateEndTimer +3500))
       {
       sound->setVolume (SOUND_MISSIONCOMMSONCHANNEL, 180);
       sound->play (SOUND_MISSIONCOMMSONCHANNEL, false);
       MissionStateNetworkBattle03 = 9;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 9 && (timer > PriorStateEndTimer +2000))
       {
       sound->setVolume (SOUND_LINUXAIRCOMBAT, 180);
       sound->play (SOUND_LINUXAIRCOMBAT, false);
       MissionStateNetworkBattle03 = 10;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 10 && (timer > PriorStateEndTimer +1500))
       {
       sound->setVolume (SOUND_MISSIONNETWORKBATTLE03, 180);
       sound->play (SOUND_MISSIONNETWORKBATTLE03, false);
       MissionStateNetworkBattle03 = 11;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 11 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_SQUADCOMMSONSUBCHANNEL, 180);
       sound->play (SOUND_SQUADCOMMSONSUBCHANNEL, false);
       MissionStateNetworkBattle03 = 12;
       PriorStateEndTimer = timer;
       }
    if (MissionStateNetworkBattle03 == 12 && (timer > PriorStateEndTimer +2500))
       {
       sound->setVolume (SOUND_RED, 180);
       if(MyNetworkId % 2)
          {
          sound->setVolume (SOUND_RED, 180);
          sound->play (SOUND_RED, false);
          }
       else
          {
          sound->setVolume (SOUND_BLUE, 180);
          sound->play (SOUND_BLUE, false);
          }
       MissionStateNetworkBattle03 = 13;
       PriorStateEndTimer = timer;
       }

    for (MissionAircraftNumber=1; MissionAircraftNumber <= 10; MissionAircraftNumber ++)
        {

        if (ThreeDObjects [0]->explode > 500 * timestep)
           {
           game_quit (); // Player plane is dead and player has waited for program to exit automagically.
           }
        if (ThreeDObjects [0]->explode > 450 * timestep)
           {
           sound->setVolume (SOUND_BEEP1, 20);
           sound->play (SOUND_BEEP1, false);
           }
        if (MissionAircraftNumber!=0 && !ThreeDObjects[MissionAircraftNumber]->active && (myrandom(10000)>9990))
            {
            GetNetworkApiPacket();
            DiscardAnyInPacketsInQueue();
            InPacket.UdpObjXPosition -=300;
            InPacket.UdpObjZPosition -=300;

            if (ThreeDObjects[MissionAircraftNumber]->Sentient > 3)
               {
               NetworkReceiveTimer= -2000; // Wait awhile before getting next InPacket
               }
            ThreeDObjects [MissionAircraftNumber]->aiinit ();
            ThreeDObjects [MissionAircraftNumber]->newinit (FIGHTER_A6M2, i + 1, 1200);
            ThreeDObjects [MissionAircraftNumber]->id = FIGHTER_A6M2;
            // Preserve this aircraft type for later comparison to detect future changes:
            MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] = ThreeDObjects[MissionAircraftNumber]->id;
            ThreeDObjects [MissionAircraftNumber]->Durability = ThreeDObjects [MissionAircraftNumber]->maxDurability;
            ThreeDObjects [MissionAircraftNumber]->immunity = 50 * timestep;
            ThreeDObjects [MissionAircraftNumber]->activate ();
            ThreeDObjects [MissionAircraftNumber]->killed = false;
            ThreeDObjects [MissionAircraftNumber]->ammo = 1600;

            ThreeDObjects [MissionAircraftNumber]->tl->x = 900 + (myrandom(20) - 10);
            ThreeDObjects [MissionAircraftNumber]->tl->z = 900 + (myrandom(20) - 10);
            ThreeDObjects[MissionAircraftNumber]->tl->y = l->getHeight(ThreeDObjects[MissionAircraftNumber]->tl->x, ThreeDObjects[MissionAircraftNumber]->tl->z);
            ThreeDObjects[MissionAircraftNumber]->tl->y += 20 + (myrandom(40) - 20);
            ThreeDObjects [MissionAircraftNumber]->Sentient = 0;
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            ThreeDObjects [MissionAircraftNumber]->thrustUp();
            for (i = 0; i < missiletypes; i ++)
                {
                ThreeDObjects [MissionAircraftNumber]->missiles [i] = 0;
                }
            ThreeDObjects [MissionAircraftNumber]->explode = 0;
            }
        }

    // Check to see if the player's aircraft has been destroyed:
    if (!ThreeDObjects[0]->active)
       {
       static bool PlayerKilledEventSkipFlag = false;
       if (PlayerKilledEventSkipFlag == false)
          {
          PlayerKilledEventSkipFlag = true;
          NetworkPlayerKilled = true;
          }
       sound->stop (SOUND_PLANE1);
       sound->stop (SOUND_PLANE2);
       sound->stop (SOUND_WINDNOISE);
       WindNoiseOn = false;
       }

    NetworkReceiveTimer += dt;
    if (NetworkReceiveTimer > NetworkReceiveTimerInterval )
       {
       NetworkReceiveTimer = 0;
       BytesReceived = GetNetworkApiPacket();
       if (BytesReceived == sizeof (LacUdpApiPacket))
          {
          display ("MissionNetworkBattle03 BytesReceived=", LOG_NET);
          sprintf (DebugBuf, "%i", BytesReceived);
          display (DebugBuf, LOG_NET);
          MissedPacketCount = 0;
          MissionNetworkBattle03LoadVariablesFromNetworkApiPacket(timer);
          if (MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] != InPacket.UdpObjVehicle)
             {

              ThreeDObjects[InPacket.UdpObjPlayerNumber]->id = (int)InPacket.UdpObjVehicle;
              ThreeDObjects[InPacket.UdpObjPlayerNumber]->newinit (ThreeDObjects[InPacket.UdpObjPlayerNumber]->id, 0, 395);
             }

          MissionNetworkBattle03PriorAircraft[InPacket.UdpObjPlayerNumber] = InPacket.UdpObjVehicle;
          }
       }

    NetworkTransmitTimer += dt;
    if (NetworkTransmitTimer > NetworkTransmitTimerInterval && !NetworkPlayerKilled)
       {
       NetworkTransmitTimer=0;
       MissionNetworkBattle03RetrieveFirstDamageDescription();
       SendNetworkApiPacket();
       }
    if (MissionEndingTimer)
       {
       MissionEndingTimer -= dt;
       if (MissionEndingTimer < 1000)
          {
          display ("MissionNetworkBattle03::processtimer() Playing audio file MissionEndingIn15SecCountdown.wav", LOG_MOST);
          sound->play (SOUND_MISSIONENDINGIN15SECCOUNTDOWN, false);
          MissionEndingTimer = 0;
          MissionEndingTimer2 = 17000;
          }
       }
    if (MissionEndingTimer2)
       {
       MissionEndingTimer2 -= dt;
       if (MissionEndingTimer2 < 1000)
          {
          display ("MissionNetworkBattle03::processtimer(): Mission ending now.", LOG_MOST);
          fplayer->Durability = -1.0;
          }
       }
    return 0;
    }

void MissionNetworkBattle03::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

void MissionNetworkBattle03LoadVariablesFromNetworkApiPacket(int timer)
{

   unsigned char PlayerNumber;

   extern LacUdpApiPacket InPacket;
   display ((char *) "MissionNetworkBattle03LoadVariablesFromNetworkApiPacket()", LOG_NET);

   float XSpeed;
   float YSpeed;
   float ZSpeed;
   int DeltaTimer;

   if (InPacket.NetworkApiVersion != NetworkApiVersion)
      {

      display ("MissionNetworkBattle03LoadVariablesFromNetworkApiPacket() discarded a UDP packet.", LOG_ERROR);
      display ("Reason: Unexpected NetworkApiVersionNumber:", LOG_ERROR);
      sprintf (DebugBuf, "We are using NetworkApiVersion %d", NetworkApiVersion);
      display (DebugBuf, LOG_ERROR);
      sprintf (SystemMessageBufferA, "NETWORK PROTOCOL CONFLICT WITH PLAYER %d.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      sound->setVolume (SOUND_BEEP1, 80);
      sound->play (SOUND_BEEP1, false);
      if (NetworkApiVersion < InPacket.NetworkApiVersion)
         {
         sprintf (DebugBuf, "We received an InPacket using newer NetworkApiVersion %d", InPacket.NetworkApiVersion);
         display (DebugBuf, LOG_ERROR);
         display ("This means that you probably need to download a newer version of LAC for compatibility.", LOG_ERROR);
         }
      else
         {
         sprintf (DebugBuf, "We received an InPacket using older NetworkApiVersion %d from player %d", InPacket.NetworkApiVersion, InPacket.UdpObjPlayerNumber);
         display (DebugBuf, LOG_ERROR);
         display ("Please inform all players that you have upgraded to a newer version of LAC for compatibility.", LOG_ERROR);
         }
      return;
      }
   NetworkOpponent = 1;

   PlayerNumber = InPacket.UdpObjPlayerNumber;
   display ("MissionNetworkBattle03LoadVariablesFromNetworkApiPacket() PlayerNumber=", LOG_NET);
   sprintf (DebugBuf, "%d", PlayerNumber);
   display (DebugBuf, LOG_NET);
   display ("Sentient Count =", LOG_NET);
   sprintf (DebugBuf, "%d", ThreeDObjects[PlayerNumber]->Sentient);
   display (DebugBuf, LOG_NET);
   if (PlayerNumber > 10)
      {
      display ("MissionNetworkBattle03LoadVariablesFromNetworkApiPacket(): network PlayerNumber ID > 10 error.", LOG_MOST);
      }
   if (ThreeDObjects[PlayerNumber]->Sentient <= 10)
      {
      ThreeDObjects[PlayerNumber]->Sentient++;
      if (ThreeDObjects[PlayerNumber]->Sentient == 4)
         {
         if (MissionEntryLatch[PlayerNumber] == 0)
            {
            if (ThreeDObjects[PlayerNumber]->Durability > 600)
               {
               sprintf (SystemMessageBufferA, "STRATEGIC BOMBER # %d CROSSED INTO RADAR RANGE.", PlayerNumber);
               NewSystemMessageNeedsScrolling = true;
               MissionEntryLatch[PlayerNumber] = 1;
               sound->setVolume (SOUND_BEEP1, 20);
               sound->play (SOUND_BEEP1, false);
               }
            }
         }
      }
   if (ThreeDObjects[PlayerNumber]->Sentient < 2)
      {
      ThreeDObjects[PlayerNumber]->Sentient = 2;
      return; // Discard the first packet.
      }

   ThreeDObjects[PlayerNumber]->id = (int)InPacket.UdpObjVehicle;

   ThreeDObjects[PlayerNumber]->tl->x = InPacket.UdpObjXPosition;
   ThreeDObjects[PlayerNumber]->tl->y = InPacket.UdpObjYPosition;
   ThreeDObjects[PlayerNumber]->tl->z = InPacket.UdpObjZPosition;

   ThreeDObjects[PlayerNumber]->gamma = InPacket.UdpObjGamma;
   ThreeDObjects[PlayerNumber]->phi   = InPacket.UdpObjPhi;
   ThreeDObjects[PlayerNumber]->theta = InPacket.UdpObjTheta;

   ThreeDObjects[PlayerNumber]->realspeed = InPacket.UdpObjSpeed;
   NetDeltaX = InPacket.UdpObjXPosition - NetworkApiPriorXPosition[PlayerNumber];
   NetDeltaY = InPacket.UdpObjYPosition - NetworkApiPriorYPosition[PlayerNumber];
   NetDeltaZ = InPacket.UdpObjZPosition - NetworkApiPriorZPosition[PlayerNumber];
   DeltaTimer = timer - NetworkApiPriorTimer[PlayerNumber];
   XSpeed = NetDeltaX/(float)DeltaTimer;
   YSpeed = NetDeltaY/(float)DeltaTimer;
   ZSpeed = NetDeltaZ/(float)DeltaTimer;
   ThreeDObjects[PlayerNumber]->thrust         = InPacket.UdpObjThrust;
   ThreeDObjects[PlayerNumber]->elevatoreffect = InPacket.UdpObjElevator;
   ThreeDObjects[PlayerNumber]->ruddereffect   = InPacket.UdpObjRudder;
   ThreeDObjects[PlayerNumber]->rolleffect     = InPacket.UdpObjAileron;
   ProcessUdpObjFlightDetails();

   NetworkApiPriorXPosition[PlayerNumber] = InPacket.UdpObjXPosition;
   NetworkApiPriorYPosition[PlayerNumber] = InPacket.UdpObjYPosition;
   NetworkApiPriorZPosition[PlayerNumber] = InPacket.UdpObjZPosition;
   NetworkApiPriorTimer[PlayerNumber]     = timer;
   if (InPacket.UdpObjDamageId == MyNetworkId)
      {
      ThreeDObjects[0]->Durability -= InPacket.UdpObjDamageAmount; // Damage this player
      sprintf (SystemMessageBufferA, "PLAYER %d DAMAGED YOUR AIRCRAFT.", InPacket.UdpObjPlayerNumber);
      NewSystemMessageNeedsScrolling = true;
      }
   else if (InPacket.UdpObjDamageAmount > 0.0)
      { // Get here if received damage claim addresses some other bot or player
      ThreeDObjects[InPacket.UdpObjDamageId]->Durability -= InPacket.UdpObjDamageAmount; // Damage addressed mission aircraft
      if (InPacket.UdpObjDamageId > 0 && InPacket.UdpObjDamageId <=10)
         {
         if (InPacket.UdpObjDamageAmount < 1000)
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DAMAGED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         else
            {
            sprintf (SystemMessageBufferA, "AIRCRAFT %d DESTROYED AIRCRAFT %d.", InPacket.UdpObjPlayerNumber, InPacket.UdpObjDamageId);
            }
         NewSystemMessageNeedsScrolling = true;
         }
       else if (InPacket.UdpObjDamageId >= 11)
         {
         if (InPacket.UdpObjDamageAmount < 32000)
            {
            sprintf  (
                     SystemMessageBufferA,
                     "AIRCRAFT %d DAMAGED AIRFIELD %d BY %5.0f KILOJOULES.",
                     InPacket.UdpObjPlayerNumber,
                     InPacket.UdpObjDamageId,
                     InPacket.UdpObjDamageAmount
                     );
            display (SystemMessageBufferA, LOG_MOST);
            NewSystemMessageNeedsScrolling = true;
            }
         else
            {
            sound->setVolume (SOUND_BEEP1, 20);
            sound->play (SOUND_BEEP1, false);
            ThreeDObjects[InPacket.UdpObjDamageId]->Durability = -4000;
            if (InPacket.UdpObjDamageId%2)
               {
               if (InPacket.UdpObjDamageId == 29)
                  {
                  sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01BlueTeamVictory = true;
                  if (fplayer->party == 1)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            else
               {
               if (InPacket.UdpObjDamageId == 28)
                  {
                  sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                  NewSystemMessageNeedsScrolling = true;
                  MissionNetworkBattle01RedTeamVictory = true;
                  if (fplayer->party == 0)
                     {
                     IffOnOff = 0;
                     RadarOnOff = 0;
                     }
                  }
               }
            }
         }
      }
   if (InPacket.UdpObjDamageAmount < 0.0)
      { // Get here if received damage represents a static update

      InPacket.UdpObjDamageAmount *= -1.0;
      if (InPacket.UdpObjDamageAmount < ThreeDObjects[InPacket.UdpObjDamageId]->Durability)
         {
         ThreeDObjects[InPacket.UdpObjDamageId]->Durability = InPacket.UdpObjDamageAmount; // Update addressed 3d Object with lower static value.
         }
      }
}

bool MissionNetworkBattle03RetrieveFirstDamageDescription()
{
int Mission3dObject;
display ("MissionNetworkBattle03RetrieveFirstDamageDescription()", LOG_NET);

for (Mission3dObject=1; Mission3dObject<=29; Mission3dObject++)
   {
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue > 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   if (ThreeDObjects[Mission3dObject]->DamageInNetQueue < 0.0)
      {
      DamageToClaim =  ThreeDObjects[Mission3dObject]->DamageInNetQueue;
      ThreeDObjects[Mission3dObject]->DamageInNetQueue = 0.0;
      MissionAircraftDamaged = Mission3dObject;
      return true;
      }
   }
MissionAircraftDamaged = 0;
DamageToClaim = 0.0;
return (false);
}

MissionTeamBase1::MissionTeamBase1 ()
    {
    id = MISSION_TEAMBASE1;
    strncpy (name, "BASE VERSUS BASE", 1024);
    strncpy (briefing, "2 SIDES. DEFEND YOUR BASE. DESTROY THEIRS.", 1024);
    selweapons = 1;
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionTeamBase1::start ()
    {
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 0;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=0;
    IffOnOff = 0;
    RadarZoom=1;
    event_RadarOnOff();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_IffOnOff();
    heading = 180;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE_EROSION, NULL);
    SeaLevel = 45.0;
    int px, py;
    l->searchPlain (1, 1, &px, &py);
    l->flatten (px, py, 8, 8);
    team1x = px;
    team1y = py + 50;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = px;
    fplayer->tl->z = py;
    fplayer->phi = 180;
    if (fplayer->id == FIGHTER_P38L)
        {
        ThreeDObjects [1]->newinit (FIGHTER_HAWK, 0, 400);
        ThreeDObjects [1]->o = &model_figb;
        ThreeDObjects [1]->target = NULL;
        }
    else
        {
        ThreeDObjects [1]->newinit (FIGHTER_P38L, 0, 400);
        ThreeDObjects [1]->o = &model_fig;
        ThreeDObjects [1]->target = ThreeDObjects [3];
        }

    ThreeDObjects [1]->party = 1;
    ThreeDObjects [1]->tl->x = px + 5;
    ThreeDObjects [1]->tl->z = py + 5;
    int n = 4;
    ThreeDObjects [n]->tl->x = px + 2;
    ThreeDObjects [n]->tl->z = py + 5;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall2;
    ThreeDObjects [n]->newinit (STATIC_HALL2, 0, 400);
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px;
    ThreeDObjects [n]->tl->z = py - 1;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall1;
    ThreeDObjects [n]->newinit (STATIC_HALL1, 0, 400);
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px + 2;
    ThreeDObjects [n]->tl->z = py - 1;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall1;
    ThreeDObjects [n]->newinit (STATIC_HALL1, 0, 400);
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px - 6;
    ThreeDObjects [n]->tl->z = py + 6;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_flak1;
    ThreeDObjects [n]->newinit (FLAK1, 0, 400);
    ThreeDObjects [n]->phi = 90;
    ThreeDObjects [n]->maxtheta = 0;
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px - 6;
    ThreeDObjects [n]->tl->z = py - 6;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_flak1;
    ThreeDObjects [n]->newinit (FLAK1, 0, 400);
    ThreeDObjects [n]->phi = 0;
    ThreeDObjects [n]->maxtheta = 0;
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px - 15;
    ThreeDObjects [n]->tl->z = py - 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 400);
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px + 0;
    ThreeDObjects [n]->tl->z = py - 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 400);
    ThreeDObjects [n]->party = 1;
    n ++;
    ThreeDObjects [n]->tl->x = px + 15;
    ThreeDObjects [n]->tl->z = py - 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 400);
    ThreeDObjects [n]->party = 1;
    l->searchPlain (1, 2, &px, &py);
    l->flatten (px, py, 8, 8);
    team2x = px;
    team2y = py - 50;
    ThreeDObjects [2]->newinit (FIGHTER_HAWK, 0, 400);
    ThreeDObjects [2]->o = &model_figb;
    ThreeDObjects [2]->tl->x = px;
    ThreeDObjects [2]->tl->z = py;

    if (fplayer->id == FIGHTER_P38L)
        {
        ThreeDObjects [2]->target = ThreeDObjects [1];
        }
    else
        {
        ThreeDObjects [2]->target = ThreeDObjects [0];
        }

    ThreeDObjects [2]->party = 2;
    ThreeDObjects [3]->newinit (FIGHTER_HAWK, 0, 400);
    ThreeDObjects [3]->o = &model_figb;
    ThreeDObjects [3]->tl->x = px + 5;
    ThreeDObjects [3]->tl->z = py + 5;
    ThreeDObjects [3]->target = NULL;
    ThreeDObjects [3]->party = 2;
    ThreeDObjects [n]->tl->x = px + 2;
    ThreeDObjects [n]->tl->z = py + 5;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall2;
    ThreeDObjects [n]->newinit (STATIC_HALL2, 0, 400);
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px;
    ThreeDObjects [n]->tl->z = py - 1;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall1;
    ThreeDObjects [n]->newinit (STATIC_HALL1, 0, 400);
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px + 2;
    ThreeDObjects [n]->tl->z = py - 1;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_hall1;
    ThreeDObjects [n]->newinit (STATIC_HALL1, 0, 400);
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px - 6;
    ThreeDObjects [n]->tl->z = py + 6;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_flak1;
    ThreeDObjects [n]->newinit (FLAK1, 0, 200);
    ThreeDObjects [n]->phi = 90;
    ThreeDObjects [n]->maxtheta = 0;
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px - 6;
    ThreeDObjects [n]->tl->z = py - 6;
    ThreeDObjects [n]->target = ThreeDObjects [0];
    ThreeDObjects [n]->o = &model_flak1;
    ThreeDObjects [n]->newinit (FLAK1, 0, 200);
    ThreeDObjects [n]->phi = 0;
    ThreeDObjects [n]->maxtheta = 0;
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px - 15;
    ThreeDObjects [n]->tl->z = py + 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 300);
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px + 0;
    ThreeDObjects [n]->tl->z = py + 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 300);
    ThreeDObjects [n]->party = 2;
    n ++;
    ThreeDObjects [n]->tl->x = px + 15;
    ThreeDObjects [n]->tl->z = py + 20;
    ThreeDObjects [n]->target = NULL;
    ThreeDObjects [n]->o = &model_flarak1;
    ThreeDObjects [n]->newinit (FLARAK_AIR1, 0, 300);
    ThreeDObjects [n]->party = 2;
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    int i;
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    RadarZoom = 6;
    RadarOnOff=1;
    IffOnOff=1;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       }
    }

int MissionTeamBase1::processtimer (Uint32 dt)
    {
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    bool testb1 = false, testb2 = false;
    for (i = 0; i <= 3; i ++)
        {
        if (!ThreeDObjects [i]->active && ThreeDObjects [i]->explode >= 35 * timestep)
            {
            ThreeDObjects [i]->explode = 0;
            int temp = ThreeDObjects [i]->fighterkills;
            int tempid = ThreeDObjects [i]->id;
            ThreeDObjects [i]->aiinit ();
            if (i == 0)
                {
                playerInit ();
                }
            else
                {
                ThreeDObjects [i]->newinit (tempid, 0, 200);
                }
            ThreeDObjects [i]->party = i / 2 + 1;
            ThreeDObjects [i]->Durability = ThreeDObjects [i]->maxDurability;
            ThreeDObjects [i]->immunity = 50 * timestep;
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->fighterkills = temp;
            ThreeDObjects [i]->killed = false;
            if (i <= 1)
                {
                ThreeDObjects [i]->tl->x = team1x;
                ThreeDObjects [i]->tl->z = team1y;
                }
            else
                {
                ThreeDObjects [i]->tl->x = team2x;
                ThreeDObjects [i]->tl->z = team2y;
                }
            display ("DebugRjb170825ak Camera Reset.", LOG_MOST);
            camera = 0;
            }
        }

    return 0;
    }

void MissionTeamBase1::draw ()
    {
    if (timer >= 0 && timer <= 50 * timestep)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

MissionWaves1::MissionWaves1 ()
    {
    id = MISSION_WAVES1;
    strncpy (name, "ASSAULT WAVES", 1024);
    strncpy (briefing, "TWO HOSTILES IN FIRST WAVE. THREE IN THE NEXT, AND SO ON...", 1024);
    wantfighter = 0;
    selweapons = 1;
    selweapon [0] = MISSILE_AIR2;
    wantweapon = 0;
    }

void MissionWaves1::start ()
    {
    int i;
    NoMissionHasYetCommenced = false;
    day = 1;
    clouds = 0;
    weather = WEATHER_SUNNY;
    camera = 0;
    sungamma = 50;
    RadarOnOff=0;
    IffOnOff = 0;
    RadarZoom=1;
    event_RadarOnOff();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_RadarZoomIn();
    event_IffOnOff();
    heading = 220;

    if (l != NULL)
        {
        delete l;
        }
    l = new GLLandscape (space, LANDSCAPE_ALPINE, NULL);
    SeaLevel = 40.0;
    l->flatten (AirfieldXMin+30, AirfieldYMin+4, 30, 5);
    playerInit ();
    fplayer->tl->x = 20;
    fplayer->tl->z = 70;
    for (i = 1; i <= 9; i ++)
        {
        ThreeDObjects [i]->party = 0;
        ThreeDObjects [i]->target = ThreeDObjects [0];
        ThreeDObjects [i]->tl->x = -i * 10;
        ThreeDObjects [i]->tl->z = -i * 10;
        if (i <= 2)
            {
            ThreeDObjects [i]->o = &model_figv;
            ThreeDObjects [i]->newinit (FIGHTER_A6M2, 0, 395);
            }
        else if (i <= 5)
            {
            ThreeDObjects [i]->o = &model_figc;
            ThreeDObjects [i]->newinit (FIGHTER_P51D, 0, 395);
            ThreeDObjects [i]->deactivate ();
            }
        else if (i <= 9)
            {
            ThreeDObjects [i]->o = &model_fig;
            ThreeDObjects [i]->newinit (FIGHTER_P38L, 0, 395);
            ThreeDObjects [i]->deactivate ();
            }
        }
    state = 0;
    laststate = 0;
    texttimer = 0;
    if (fplayer->id == FIGHTER_HAWK)
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 4;
        fplayer->missiles [2] = 2;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 10;
        fplayer->flares = 10;
        }
    else
        {
        fplayer->missiles [0] = 0;
        fplayer->missiles [1] = 0;
        fplayer->missiles [2] = 0;
        fplayer->missiles [3] = 0;
        fplayer->missiles [4] = 0;
        fplayer->chaffs = 0;
        fplayer->flares = 0;
        }
    fplayer->realspeed = (fplayer->DiveSpeedLimit1 * 0.75);
    fplayer->InertiallyDampenedPlayerSpeed = (fplayer->DiveSpeedLimit1 * 0.75);
    for (i=0; i<=9; i++)
        {
        fplayer->SpeedHistoryArray[i] = (fplayer->DiveSpeedLimit1 * 0.75);
        }
    RadarZoom = 6;
    RadarOnOff=1;
    IffOnOff=0;
    if (fplayer->id == FIGHTER_HAWK)
       {
       if (HudLadderBarsOnOff == 0)
           {
           event_HudLadderBarsOnOff(); // Turn on Hud Ladder Bars if in a jet
           }
       event_IffOnOff();
       }
    }

int MissionWaves1::processtimer (Uint32 dt)
    {
    bool b = false;
    int i;

    if (texttimer >= 200 * timestep)
        {
        texttimer = 0;
        }
    if (texttimer > 0)
        {
        texttimer += dt;
        }
    timer += dt;
    if (!fplayer->active && fplayer->explode >= 35 * timestep)
        {
        return 2;
        }
    for (i = 0; i <= 9; i ++)
        {
        if (ThreeDObjects [i]->active)
            if (ThreeDObjects [i]->party == 0)
                {
                b = true;
                }
        }
    if (b)
        {
        return 0;
        }
    state ++;
    if (state == 1)
        {
        for (i = 3; i <= 5; i ++)
            {
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
            }
        playerInit ();
        return 0;
        }
    else if (state == 2)
        {
        for (i = 6; i <= 9; i ++)
            {
            ThreeDObjects [i]->activate ();
            ThreeDObjects [i]->tl->x = fplayer->tl->x + 50 + 10 * i;
            ThreeDObjects [i]->tl->z = fplayer->tl->z + 50 + 10 * i;
            ThreeDObjects [i]->tl->y = l->getHeight (ThreeDObjects [i]->tl->x, ThreeDObjects [i]->tl->z) + 15;
            }
        playerInit ();
        return 0;
        }

    return 1;
    }

void MissionWaves1::draw ()
    {
    int timeroff = 100 * timestep, timerlag = 20 * timestep;

    if (laststate != state)
        {
        texttimer = 1;
        laststate = state;
        }
    if (texttimer > 0)
        {
        if (state == 1)
            {
            if (fplayer->id == FIGHTER_HAWK)
                {
                fplayer->missiles [0] = 0;
                fplayer->missiles [1] = 4;
                fplayer->missiles [2] = 2;
                fplayer->missiles [3] = 0;
                fplayer->missiles [4] = 0;
                fplayer->chaffs = 10;
                fplayer->flares = 10;
                }
            else
                {
                fplayer->missiles [0] = 0;
                fplayer->missiles [1] = 0;
                fplayer->missiles [2] = 0;
                fplayer->missiles [3] = 0;
                fplayer->missiles [4] = 0;
                fplayer->chaffs = 0;
                fplayer->flares = 0;
                }
            font1->drawTextCentered (0, 6, -2.5, "NEXT WAVE - YOU ARE FULLY REPAIRED", &textcolor);
            return;
            }
        if (state == 2)
            {
            if (fplayer->id == FIGHTER_HAWK)
                {
                fplayer->missiles [0] = 0;
                fplayer->missiles [1] = 4;
                fplayer->missiles [2] = 2;
                fplayer->missiles [3] = 0;
                fplayer->missiles [4] = 0;
                fplayer->chaffs = 10;
                fplayer->flares = 10;
                }
            else
                {
                fplayer->missiles [0] = 0;
                fplayer->missiles [1] = 0;
                fplayer->missiles [2] = 0;
                fplayer->missiles [3] = 0;
                fplayer->missiles [4] = 0;
                fplayer->chaffs = 0;
                fplayer->flares = 0;
                }
            font1->drawTextCentered (0, 6, -2.5, "NEXT WAVE - YOU ARE FULLY REPAIRED", &textcolor);
            return;
            }
        }
    if (timer >= 0 && timer <= timeroff - timerlag)
        {
        font1->drawTextCentered (0, 12, -2, name, &textcolor);
        }
    }

