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

/* This file includes all AI objects instancing models. */

#ifndef IS_AIOBJECT_H

#include <iostream>

#include "aiobject.h"
#include "land.h"
#include "glland.h"
#include "main.h"
#include "mathtab.h"
#include "mission.h"
#include "NetworkApi.h"

extern bool ClearSpeedHistoryArrayFlag;
extern bool GearUpDamageAlreadyInflicted;
extern bool LandedAtSafeSpeed;
extern bool OnTheGround;
extern bool LandingLogged;
extern bool MissileFired;
extern bool MissionNetworkBattle01BlueTeamVictory;
extern bool MissionNetworkBattle01RedTeamVictory;
extern bool MissionNetworkBattle02BlueTeamVictory;
extern bool MissionNetworkBattle02RedTeamVictory;
extern bool MissionNetworkBattle03BlueTeamVictory;
extern bool MissionNetworkBattle03RedTeamVictory;
extern bool NetworkMissionIsActiveWithDataFlow;
extern bool NewSystemMessageNeedsScrolling;
extern bool RadarReflectorBlueHasBeenDestroyedAtLeastOnce;
extern bool RadarReflectorRedHasBeenDestroyedAtLeastOnce;
extern bool RecoveredFromStall;
extern bool TakeoffLogged;

extern char CurrentMissionNumber;

extern char  DebugBuf[100];
extern char SystemMessageBufferA[];

extern unsigned char MissionHeadToHead00State;
extern unsigned char MyNetworkId;
extern unsigned char NetworkOpponent;
extern unsigned char RecentVictim;

extern int AirfieldXMax;
extern int AirfieldXMin;
extern int AirfieldYMax;
extern int AirfieldYMin;
extern int IffOnOff;
extern int MissionNumber;
extern int NetworkReceiveIntermediateTimer1;
extern int NetworkReceiveTimerInterval;
extern int RadarOnOff;

extern Uint32 MissionEndingTimer;
extern Uint32 MostRecentBombFlightTimer;
extern Uint32 DeltaTime;
extern Uint32 ViewResetTimer;

extern float BlackoutSensitivity;
extern float CompressibilityRollFactor;
extern float ConfiguredViewDistance;
extern float InPacketDeltaXPosition[];
extern float InPacketDeltaYPosition[];
extern float InPacketDeltaZPosition[];
extern float RecentVictimAltitude;
extern float RecentVictimGamma;
extern float RecentVictimPhi;
extern float RecentVictimTheta;
extern float RecentVictimXPosition;
extern float RecentVictimZPosition;
extern float RecentVictimVelocity;

extern float RedoutSensitivity;
extern float RegulatedForceX;
extern float RegulatedForceY;
extern float RegulatedForceZ;
extern float SeaLevel;
extern float ThetaDelta;
extern float TrueAirSpeed;

extern LacUdpApiPacket InPacket;

extern FILE *OnlineScoreLogFile;

/*
*
* Function Prototypes:
*/

void CalculateTrueAirspeed();
void RegulateGamma ();
void RegulateXYZForces(float forcex, float forcey, float forcez);
void TestAltitudeAirDensity();
void TestDiveSpeedStructuralLimit();
void TestForExcessGamma();
void TestForFlapEffects();
void TestForLoopBeyondVertical();
void TestForSpeedBrakeDrag();
void TestForUndercarriageEffects();
void TestForWindNoise();
extern void UpdateOnlineScoreLogFileWithLandings();
extern void UpdateOnlineScoreLogFileWithTakeoffs();
extern void UpdateOnlineScoreLogFileWithCrashes();

void DynamicObj::activate ()
    {
    active = true;
    draw = true;
    }

void DynamicObj::deactivate ()
    {
    active = false;
    draw = false;
    }

void DynamicObj::dinit ()
    {
    rot->a = 90;
    phi = 0;
    theta = 0;
    gamma = 180;
    rectheta = 0;
    tl->z = 0;
    tl->x = 0;
    forcex = 0;
    forcez = 0;
    forcey = 0;
    maxthrust = 0.3;
    braking = 0;
    manoeverability = 0.5;
    thrust = maxthrust;
    recthrust = thrust;
    recheight = 5.0;
    ttl = -1;
    Durability = 0.01F;
    maxDurability = 0.01F;
    immunity = 0;
    recgamma = 180;
    id = CANNON1;
    impact = 7;
    source = NULL;
    points = 0;
    party = 0;
    elevatoreffect = 0;
    ruddereffect = 0;
    rolleffect = 0;
    maxgamma = 70;
    maxtheta = 90;
    gamma = 180;
    theta = 0;
    explode = 0;
    sink = 0;
    RollRate = 1.0;
    fighterkills = 0;
    shipkills = 0;
    tankkills = 0;
    otherkills = 0;
    killed = false;
    realism = physics;
    accx = accy = accz = 0;
    DragEffect=1.0;
    }

DynamicObj::DynamicObj ()
    {
    dinit ();
    }

DynamicObj::DynamicObj (Space *space2, CModel *o2, float zoom2)
    {
    this->space = space2;
    o = o2;
    zoom = zoom2;
    dinit ();
    space->addObject (this);
    }

void DynamicObj::thrustUp ()
    {
    recthrust += maxthrust / 12;

    if (recthrust > maxthrust)
        {
        recthrust = maxthrust;
        }
    }

void DynamicObj::thrustDown ()
    {
    recthrust -= maxthrust / 12;

    if (recthrust < maxthrust / 2)
        {
        recthrust = maxthrust / 2;
        }
    }

float DynamicObj::distance (DynamicObj *target)
    {
    float dx = target->tl->x - tl->x;
    float dz = target->tl->z - tl->z;
    float dy = target->tl->y - tl->y;
    return sqrt (dx * dx + dz * dz + dy * dy);
    }

float DynamicObj::distanceXZ (DynamicObj *target)
    {
    float dx = target->tl->x - tl->x;
    float dz = target->tl->z - tl->z;
    return sqrt (dx * dx + dz * dz);
    }

// check whether the object is exploding or sinking and deactivate if necessary
void DynamicObj::checkExplosion (Uint32 dt)
    {
    if (explode > 0)
        {
        if (explode == 1)
            {
            ttl = -1;

            if (
               id == STATIC_CONTAINER1 ||
               id == STATIC_RADAR1 ||
               id == STATIC_COMPLEX1 ||
               id == STATIC_DEPOT1 ||
               id == TANK1 ||
               id == STATIC_AIRFIELD00
               )
                {

                setExplosion (2.0, 100 * timestep);
                setBlackSmoke (14.0, 600 * timestep);
                }
            else if (id == STATIC_OILRIG1)
                {
                setExplosion (3.0, 40 * timestep);
                setBlackSmoke (5.5, 80 * timestep);
                }
            else
                {
                float zoom2 = zoom * 2;

                if (zoom2 > 2)
                    {
                    zoom2 = 2;
                    }
                setExplosion (zoom2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                }
            }

        if (id >= STATIC_GROUND || (id >= MOVING_GROUND && id <= MOVING_WATER))
            {
            if (explode >= 25 * timestep && ttl == -1)
                {
                setExplosion (zoom * 2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                ttl = -2;
                }

            if (explode >= 30 * timestep && ttl == -2)
                {
                setExplosion (zoom * 2, 35 * timestep);
                setBlackSmoke (1.0, 60 * timestep);
                ttl = -3;
                }
            }

        if (explode >= 35 * timestep)
            {
            deactivate ();
            ttl = -1;
            explode += dt;
            setBlackSmoke (18.0, 600 * timestep);
            if (id == STATIC_RADARREFLECTOR)
               {

               if (tl->x > 0)
                  {
                  if (ThreeDObjects[29]->Durability > (ThreeDObjects[29]->maxDurability * 0.4))
                     {
                     ThreeDObjects[29]->Durability = ThreeDObjects[29]->maxDurability * 0.39;
                     RadarReflectorRedHasBeenDestroyedAtLeastOnce = true;
                     }
                  }
               else
                  {
                  if (ThreeDObjects[28]->Durability > (ThreeDObjects[28]->maxDurability * 0.4))
                     {
                     ThreeDObjects[28]->Durability = ThreeDObjects[28]->maxDurability * 0.39;
                     RadarReflectorBlueHasBeenDestroyedAtLeastOnce = true;
                     }
                  }
               }

            if ((id != STATIC_AIRFIELD00) && (id != STATIC_RADARREFLECTOR) && id >= STATIC_GROUND || (id >= MOVING_GROUND && id <= MOVING_WATER))
                {
                explode = 0;
                draw = true;
                id = STATIC_PASSIVE;
                Durability = 100000;
                o = &model_rubble1;
                zoom *= 0.7F;

                if (zoom > 1)
                    {
                    zoom = 1;
                    }

                tl->y = l->getExactHeight (tl->x, tl->z) + zoom / 4;
                }
            }
        else
            {
            explode += dt;
            }
        }
    if (sink)
        {
        sink += dt;
        if (sink > 100 * timestep)
            {
            deactivate ();
            ttl = -1;
            }
        }
    }

// check the objects Durability value for damage. Explode/sink if necessary
void DynamicObj::checkDurability ()
    {
    if (Durability <= 0)
        {
        Durability = 0;

        if (explode <= 0)
            {
            if (id >= MISSILE1 && id <= MISSILE2)
                {
                explode = 1;
                active = false;
                }

            if (id >= FIGHTER1 && id <= FIGHTER2)
                {
                explode = 1;
                active = false;
                }

            if (id >= TANK1 && id <= TANK2)
                {
                explode = 1;
                active = false;
                }

            if (id >= FLAK1 && id <= FLAK2)
                {
                explode = 1;
                active = false;
                }

            if (id >= STATIC_PASSIVE)
                {
                if (
                   CurrentMissionNumber != MISSION_NETWORKBATTLE01
                   && CurrentMissionNumber != MISSION_NETWORKBATTLE02
                   && CurrentMissionNumber != MISSION_NETWORKBATTLE03
                   )
                   {
                   explode = 1;
                   active = false;
                   }
                else
                   {
                   static bool latch = false;
                   if (!latch)
                      {
                      sound->setVolume (SOUND_EXPLOSION1, 120);
                      sound->play (SOUND_EXPLOSION1, false);
                      latch = true;
                      }
                   if (ThreeDObjects[28]->Durability <= 400)
                      {
                      ThreeDObjects[24]->Durability = 0;
                      if (fplayer->target == ThreeDObjects[28])
                         {
                         fplayer->targetNext ((AIObj **) ThreeDObjects);
                         display ("DynamicObj::checkDurability() advancing target beyond destroyed airfield 28.", LOG_MOST);
                         }
                      sound->haltMusic();
                      display ("DynamicObj::checkDurability Playing mission-end music...", LOG_MOST);
                      if (MyNetworkId%2)
                         {
                         sound->setVolume (SOUND_MISSIONENDINGINSTRUMENTAL, 127);
                         sound->play (SOUND_MISSIONENDINGINSTRUMENTAL, false);
                         }
                      else
                         {
                         sound->setVolume (SOUND_DEFEAT00, 127);
                         sound->play (SOUND_DEFEAT00, false);
                         }

                      sound->setVolume (SOUND_REDTEAMHASWONBATTLE, 127);
                      sound->play (SOUND_REDTEAMHASWONBATTLE, false);

                      sprintf (SystemMessageBufferA, "THE RED TEAM HAS WON THE BATTLE.");
                      NewSystemMessageNeedsScrolling = true;
                      MissionNetworkBattle01RedTeamVictory = true;
                      MissionNetworkBattle02RedTeamVictory = true;
                      MissionNetworkBattle03RedTeamVictory = true;
                      MissionEndingTimer = 13000;
                      if (MyNetworkId & 0x01)
                         {
                         }
                      else
                         {
                         IffOnOff = 0;
                         RadarOnOff = 0;
                         }
                      }
                   else if (ThreeDObjects[29]->Durability <= 400)
                      {
                      ThreeDObjects[25]->Durability = 0;
                      if (fplayer->target == ThreeDObjects[29])
                         {
                         fplayer->targetNext ((AIObj **) ThreeDObjects);
                         display ("DynamicObj::checkDurability() advancing target beyond destroyed airfield 29.", LOG_MOST);
                         }
                      sound->haltMusic();
                      display ("DynamicObj::checkDurability Playing mission-ending music...", LOG_MOST);
                      if (!MyNetworkId%2)
                         {
                         sound->setVolume (SOUND_DEFEAT00, 127);
                         sound->play (SOUND_DEFEAT00, false);
                         }
                      else
                         {
                         sound->setVolume (SOUND_MISSIONENDINGINSTRUMENTAL, 127);
                         sound->play (SOUND_MISSIONENDINGINSTRUMENTAL, false);
                         }
                      sound->setVolume (SOUND_BLUETEAMHASWONBATTLE, 127);
                      sound->play (SOUND_BLUETEAMHASWONBATTLE, false);

                      sprintf (SystemMessageBufferA, "THE BLUE TEAM HAS WON THE BATTLE.");
                      NewSystemMessageNeedsScrolling = true;
                      MissionNetworkBattle01BlueTeamVictory = true;
                      MissionNetworkBattle02BlueTeamVictory = true;
                      MissionNetworkBattle03BlueTeamVictory = true;
                      MissionEndingTimer = 6000;
                      if (MyNetworkId & 0x01)
                         {
                         IffOnOff = 0;
                         RadarOnOff = 0;
                         }
                      else
                         {
                         }
                      }
                   else if (ThreeDObjects[24]->Durability <= 0)
                      {
                      sprintf (SystemMessageBufferA, "BLUETEAM RADAR DESTROYED");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   else if (ThreeDObjects[25]->Durability <= 0)
                      {
                      sprintf (SystemMessageBufferA, "REDTEAM RADAR DESTROYED");
                      NewSystemMessageNeedsScrolling = true;
                      }
                   NewSystemMessageNeedsScrolling = true;
                   explode = 1;
                   active = false;
                   }
                }
            }

        if (sink <= 0)
            if (id >= SHIP1 && id <= SHIP2)
                {
                sink = 1;
                }
        }
    }

// check whether the object collides on the ground and alter gamma and y-translation
void DynamicObj::crashGround (Uint32 dt)
    {
    static bool NoseWheelDamageInflicted;
    static bool WheelsUpLandingDamageInflicted;
    static bool TireSqueakSounded;
    static bool LandingDamageSounded;
    static bool OceanCrashSounded;
    static bool BellyScrapeSounded;
    static bool WheelRolloutSounded;
    if (id >= MOVING_GROUND)
        {
        return;
        }
    float TerrainHeightHere = l->getExactHeight (tl->x, tl->z);
    float height = tl->y - TerrainHeightHere;
    if (id >= CANNON1 && id <= CANNON2)
        {
        if (tl->y < TerrainHeightHere)
           {
           setExplosion (3.0, 30 * timestep);
           deactivate ();
           }
        }
    if (height < zoom)
        {
        float PlayerAlt;
        float RunwayAlt;
        PlayerAlt = fabs(l->getExactHeight(tl->x, tl->z));
        RunwayAlt = fabs(l->getExactHeight(AirfieldXMin+1, AirfieldYMin+2));
        if (this == (DynamicObj *) fplayer)
           {
           tl->y -= (height - zoom);
           }
        else
           {
           tl->y -= (height - zoom*0.3);
           }
        if (realspeed < StallSpeed * 0.5)
           {
           if (gamma > 180)
              {
              gamma -= (.005 * timestep);
              }
           if (gamma < 180)
              {
              gamma = 180;
              }
           theta = 0;
           }
        if (Durability > 0)
            {
            if (id >= MISSILE1 && id <= MISSILE2)
                {
                setExplosion (1.2, 30 * timestep);
                setBlackSmoke (1.2, 30 * timestep);
                }
            }
        float decfac = 200.0F;
        if (realism && this == (DynamicObj *) fplayer && game == GAME_PLAY)
            {
            OnTheGround = true;
            if (fplayer->realspeed < (fplayer->StallSpeed * 1.0))
               {
               if (gamma < 174.0 && !NoseWheelDamageInflicted)
                  {
                  Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 7);
                  NoseWheelDamageInflicted = true;
                  }
               float height2 = tl->y - l->getExactHeight (tl->x+1, tl->z);
               float height3 = tl->y - l->getExactHeight (tl->x-1, tl->z);
               float height4 = tl->y - l->getExactHeight (tl->x, tl->z+1);
               float height5 = tl->y - l->getExactHeight (tl->x, tl->z-1);
               float diff1 = (fabs)(height2 - height3);
               float diff2 = (fabs)(height3 - height4);
               float diff3 = (fabs)(height4 - height5);
               if (diff1 < 0.4 && diff2 < 0.4 && diff3 < 0.4)
                  {

                  if (fabs(l->getExactHeight(tl->x, tl->z) + SeaLevel) < 0.1)
                     {
                     if (!OceanCrashSounded)
                        {
                        sound->setVolume (SOUND_CRASHOCEAN00, 90);
                        sound->play (SOUND_CRASHOCEAN00, false);

                        Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 15.0);
                        if (fplayer->UndercarriageLevel)
                           {
                           Durability -= ((fplayer->realspeed * decfac * dt / timestep) * 9.0);
                           }
                        OceanCrashSounded = true;
                        }
                     }
                  else if (fplayer->UndercarriageLevel)
                     {
                     float PlayerAlt;
                     float RunwayAlt;
                     PlayerAlt = fabs(l->getExactHeight(tl->x, tl->z));
                     RunwayAlt = fabs(l->getExactHeight(AirfieldXMin+1, AirfieldYMin+2));
                     if (PlayerAlt == RunwayAlt && (fabs(fplayer->theta)) < 5.0)
                        {
                        Durability -= 0;
                        if (fplayer->thrust < 0.01)
                           {

                           ClearSpeedHistoryArrayFlag = true;
                           fplayer->realspeed = 0.0;
                           fplayer->thrust *= 0.1;
                           }
                        if (TrueAirSpeed < 50.00 && (
                                                    fplayer->id == BOMBER_B17       ||
                                                    fplayer->id == BOMBER_B24       ||
                                                    fplayer->id == BOMBER_JU87      ||
                                                    fplayer->id == BOMBER_G5M       ||
                                                    fplayer->id == BOMBER_B25       ||
                                                    fplayer->id == BOMBER_B26       ||
                                                    fplayer->id == BOMBER_B29       ||
                                                    fplayer->id == BOMBER_JU88      ||
                                                    fplayer->id == BOMBER_DORNIER   ||
                                                    fplayer->id == BOMBER_HE111     ||
                                                    fplayer->id == BOMBER_LANCASTER
                                                    )
                           )
                           {
                           LandedAtSafeSpeed = true;
                           UpdateOnlineScoreLogFileWithLandings();

                           if (TakeoffLogged == true)
                              {
                              TakeoffLogged = false;
                              }
                           }
                        else if (TrueAirSpeed < 3.5)
                           {
                           LandedAtSafeSpeed = true;
                           UpdateOnlineScoreLogFileWithLandings();

                           if (TakeoffLogged == true)
                              {
                              TakeoffLogged = false;
                              }
                           }
                        if (!TireSqueakSounded)
                           {
                           sound->setVolume (SOUND_TIRESQUEAK1, 90);
                           sound->play (SOUND_TIRESQUEAK1, false);
                           TireSqueakSounded = true;
                           }
                        }
                     if ((fabs(fplayer->theta)) > 10.0 && (fplayer->realspeed > 0.03 && !LandedAtSafeSpeed))
                        {
                        Durability =- 15;
                        fplayer->theta = 0;
                        sound->setVolume (SOUND_BELLYSCRAPE00, 90);
                        sound->play (SOUND_BELLYSCRAPE00, false);
                        }
                     else
                        {
                        if (!WheelRolloutSounded)
                           {
                           sound->setVolume (SOUND_WHEELROLLOUT00, 90);
                           sound->play (SOUND_WHEELROLLOUT00, false);
                           WheelRolloutSounded = true;
                           }
                        }
                     }
                  else
                     {
                     if (!BellyScrapeSounded)
                        {
                        sound->setVolume (SOUND_BELLYSCRAPE00, 90);
                        sound->play (SOUND_BELLYSCRAPE00, false);
                        BellyScrapeSounded = true;
                        }
                     if (!GearUpDamageAlreadyInflicted)
                        {
                        Durability -= fplayer->realspeed * decfac * dt / timestep;;
                        GearUpDamageAlreadyInflicted = true;
                        }
                     if (fplayer->gamma > 2)
                        {
                        fplayer->gamma -=0.5;
                        }

                     }
                  }
               else
                  {

                  Durability -= fplayer->realspeed * decfac * dt / timestep;
                  if (!LandingDamageSounded)
                     {
                     sound->setVolume (SOUND_DAMAGE00, 90);
                     sound->play (SOUND_DAMAGE00, false);
                     LandingDamageSounded = true;
                     }
                  }
               }
            else
               {

               if (!LandedAtSafeSpeed)
                  {
                  Durability -= fplayer->realspeed * decfac * dt / timestep;
                  if (!LandingDamageSounded)
                     {
                     sound->setVolume (SOUND_DAMAGE00, 90);
                     sound->play (SOUND_DAMAGE00, false);
                     LandingDamageSounded = true;
                     }
                  }
               if (Durability < 0.5)
                  {
                  setExplosion (0.2, 25 * timestep);
                  setBlackSmoke (0.5, 25 * timestep);
                  UpdateOnlineScoreLogFileWithCrashes();
                  std::cout << "You crashed into the ocean---------------------------------------------------------------------------------------" << std::endl;
                  }
               }
            }

        else
            {

            if (id == BOMB01)
               {
               Durability -= decfac * dt / timestep;
               }
            if (realspeed > 0.33)
               {
               if (id != BOMB01)
                  {
                  Durability -= decfac * dt / timestep;
                  }
                  if (Durability < 0 && (id >= FIGHTER1 && id <= FIGHTER2))
                   {
                   setExplosion (0.2, 25 * timestep);
                   setBlackSmoke (0.5, 25 * timestep);
                   active = false;
                   }
               }
            }

        }

    else
       {
       if (this == (DynamicObj *) fplayer && height > zoom + 1)
          {
          OnTheGround = false;
          if (LandedAtSafeSpeed)
             {
             UpdateOnlineScoreLogFileWithTakeoffs();
             }
          LandedAtSafeSpeed = false;

          LandingLogged = false;
          GearUpDamageAlreadyInflicted = false;
          NoseWheelDamageInflicted = false;
          TireSqueakSounded = false;
          LandingDamageSounded = false;
          OceanCrashSounded = false;
          BellyScrapeSounded = false;
          WheelRolloutSounded = false;
          }
       }
    }

// check for collision, simplified model, each model is surrounded by a cube
// this works pretty well, but we must use more than one model for complex models or scenes
void DynamicObj::collide (DynamicObj *d, Uint32 dt) // d must be the medium (laser, missile)
    {

    if (immunity > 0 || d->immunity > 0)
        {
        return;
        }

    if (explode > 0 || sink > 0)
        {
        return;
        }

    bool collide = false;

    if (    tl->x + o->cubex >= d->tl->x - d->o->cubex && tl->x - o->cubex <= d->tl->x + d->o->cubex &&
            tl->y + o->cubey >= d->tl->y - d->o->cubey && tl->y - o->cubey <= d->tl->y + d->o->cubey &&
            tl->z + o->cubez >= d->tl->z - d->o->cubez && tl->z - o->cubez <= d->tl->z + d->o->cubez)
        {
        collide = true;
        }
    if (collide)
        {
        if (id == STATIC_AIRFIELD00)
           {
           if (d->id > FIGHTER1 && d->id < FIGHTER2)
              {
              if (
                 tl->x + 1 >= d->tl->x - 1 && tl->x - 1 <= d->tl->x + 1 &&
                 tl->y + 1 >= d->tl->y - 1 && tl->y - 1 <= d->tl->y + 1 &&
                 tl->z + 1 >= d->tl->z - 1 && tl->z - 1 <= d->tl->z + 1
                 )
                    {
                    if (d == (DynamicObj *) fplayer)
                       {
                       fplayer->Durability -= 4.0F;
                       }
                    }
              else
                    {
                    return;
                    }
              }
           else if (d->id == BOMB01)
              {
              d->Durability -= 30000.0;
              DamageInNetQueue = 30000.0;
              }
           else if (d->id == MISSILE_DF1)
              {
              d->Durability -= 3400;
              DamageInNetQueue = 3400.0;
              }
           }
        if (this == (DynamicObj *) fplayer && game == GAME_PLAY && realism && d->id >= AIR && d->id < MOVING_GROUND)
            { // Get here if the player has collided with another aircraft
            Durability = -1.0F; // Damage the player
            d->Durability = -1.0F; // Damage the other aircraft with which we collided
            }

        if (id < STATIC_PASSIVE || (id >= STATIC_PASSIVE && d->id >= MISSILE1 && d->id <= MISSILE2))
            {
            Durability -= (float) d->impact;
            }
        if (d->id == CANNON1)
           {
           Durability -= fplayer->statLethality;
           }
        if (d->id == CANNON2)
           {
           Durability -= fplayer->statLethality;
           }
        if (d->source->party == party)
           { // Somebody is attempting fratricide
           if (d->source == (DynamicObj *) fplayer)
              { // player is shooting his own team
              if (id == STATIC_AIRFIELD00)
                 {
                 //sprintf (SystemMessageBufferA, "YOU ARE SHOOTING YOUR OWN AIRFIELD!");
                 //NewSystemMessageNeedsScrolling = true;
                 //fplayer->Durability -= 0.02;
                 }
              }
           }
        if ((d->id != BOMB01) && (d->id != MISSILE_DF1))
           {
           DamageInNetQueue += fplayer->statLethality * 0.4;
           }
        if (d->source != NULL && active)   // only for missiles/cannons
            {
            if (Durability <= 0)
                {
                if (active && draw && !killed)
                   {
                   if (d->source->id >= FIGHTER1 && d->source->id <= FIGHTER2)
                       {
                       killed = true;
                       DamageInNetQueue = 50000;
                       if (id >= FIGHTER1 && id <= FIGHTER2)
                           {
                           d->source->fighterkills ++;

                           RecentVictim = id;
                           RecentVictimAltitude = tl->y;
                           RecentVictimXPosition = tl->x;
                           RecentVictimZPosition = tl->z;
                           RecentVictimGamma = gamma;
                           RecentVictimPhi = phi;
                           RecentVictimTheta = theta;
                           RecentVictimVelocity = realspeed * 0.8;
                           }
                       else if (id >= SHIP1 && id <= SHIP2)
                           {
                           d->source->shipkills ++;
                           }
                       else if ((id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
                           {
                           d->source->tankkills ++;
                           }
                       else
                           {
                           d->source->otherkills ++;
                           }
                       }
                   }
                }
            }
        std::cout << "You crashed from a collision---------------------------------------------------------------------------------------" << std::endl;
        setExplosion (0.2, 20 * timestep);
        setBlackSmoke (0.5, 30 * timestep);
        }
    }

void DynamicObj::setExplosion (float maxzoom, int len)
    {
    int i;
    float TerrainHeightHere;

    for (i = 0; i < maxexplosion; i ++) // search a free explosion instance
        if (explosion [i]->ttl <= 0)
            {
            break;
            }

    if (i >= maxexplosion)
        {
        i = 0;
        }
    if (id >= CANNON1 && id <= CANNON2)
       {

       explosion [i]->setExplosion (tl->x, tl->y+1.0, tl->z, forcex, forcey, forcez, maxzoom/2, len);
       }
    else if (id == MISSILE_DF1)
       {
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, tl->y * 0.4, len);
       }
    else if (id == BOMB01)
       {
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, tl->y * 0.9, len);
       }
    else if (id == STATIC_AIRFIELD00)
       {
       ;
       }
    else
       {
       explosion [i]->setExplosion (tl->x, tl->y, tl->z, forcex, forcey, forcez, maxzoom, len);
       }
    }

void DynamicObj::setBlackSmoke (float maxzoom, int len)
    {
    int i;

    for (i = 0; i < maxblacksmoke; i ++) // search a free blacksmoke instance
        if (blacksmoke [i]->ttl <= 0)
            {
            break;
            }

    if (i >= maxblacksmoke)
        {
        i = 0;
        }

    blacksmoke [i]->setBlackSmoke (tl->x, tl->y, tl->z, phi, maxzoom, len);
    }

// return heading difference towards enemy
int DynamicObj::getAngle (DynamicObj *o)
    {
    float dx2 = o->tl->x - tl->x, dz2 = o->tl->z - tl->z;
    int a, w = (int) phi;

    if (dz2 > -0.0001 && dz2 < 0.0001)
        {
        dz2 = 0.0001;
        }

    a = (int) (atan (dx2 / dz2) * 180 / PI);

    if (dz2 > 0)
        {
        if (dx2 > 0)
            {
            a -= 180;
            }
        else
            {
            a += 180;
            }
        }

    int aw = a - w;

    if (aw < -180)
        {
        aw += 360;
        }

    if (aw > 180)
        {
        aw -= 360;
        }

    return aw;
    }

// return elevation difference towards enemy
int DynamicObj::getAngleH (DynamicObj *o)
    {
    float disttarget = distance (o);
    return (int) (atan ((o->tl->y - tl->y) / disttarget) * 180 / PI - (gamma - 180));
    }

// check for a looping, this is tricky :-)
bool DynamicObj::checkLooping ()
    {
    if (gamma > 270)
        {
        gamma = 540 - gamma;
        theta += 180;
        phi += 180;
        rectheta += 180;

        if (theta >= 360)
            {
            theta -= 360;
            }

        if (rectheta >= 360)
            {
            rectheta -= 360;
            }

        if (phi >= 360)
            {
            phi -= 360;
            }
        return true;
        }
    else if (gamma < 90)
        {
        gamma = 180 - gamma;
        theta += 180;
        phi += 180;
        rectheta += 180;

        if (theta >= 360)
            {
            theta -= 360;
            }

        if (rectheta >= 360)
            {
            rectheta -= 360;
            }

        if (phi >= 360)
            {
            phi -= 360;
            }
        return true;
        }
    return false;
    } // end DynamicObj::checkLooping ()

void DynamicObj::move (Uint32 dt)
    {
    float DegreesToRadiansFactor = 0.01745333;
    float SpeedHistoryAccumulator = 0;
    float AveragingTemp;
    unsigned char HistoryArrayPointer;
    if (dt <= 0)
        {
        return;
        }
    if (OnTheGround)
       {
       CalculateTrueAirspeed();
       }

    if (ViewResetTimer >0)
       {
       ViewResetTimer += dt;
       if (ViewResetTimer > 2000)
          {
          view = ConfiguredViewDistance;
          ViewResetTimer = 0;
          }
       }
    if (realspeed <= 0)
        {
        realspeed = 1.0F;
        }
    float brakepower = 1.0F;
    float timefac = (float) dt / (float) timestep;
    checkExplosion (dt); // check if this object is exploding
    if (sink > 0)   // only ships (they will not explode)
        {
        tl->y -= 0.02 * timefac; // sink down
        gamma = recgamma = 180.0 + 0.5 * (float) sink / timestep; // change angle when sinking
        return; // and exit move()
        }
    if (!active && !draw)
        {
        return;    // exit if not active
        }

    if (id == MISSILE_DF1)
       {
       static bool ExplosionSoundCommenced;
       static int ExplosionIntervalTimer = 0;
       float RunwayAlt;
       RunwayAlt = fabs(l->getExactHeight(AirfieldXMin+1, AirfieldYMin+2));
       if (tl->y < (RunwayAlt + 1))
          {
          if (ExplosionSoundCommenced == false)
             {
             float Xdistance;
             float Zdistance;
             float XZdistance;
             Xdistance = (fabs)(fplayer->tl->x - tl->x);
             Zdistance = (fabs)(fplayer->tl->z - tl->z);
             XZdistance = (Xdistance + Zdistance) * 1.5;
             if (XZdistance > 126)
                {
                XZdistance = 126;
                }
             sound->setVolume (SOUND_EXPLOSION1, (127 - (int)XZdistance));
             sound->play (SOUND_EXPLOSION1, false);
             ExplosionSoundCommenced = true;
             }
          }
       if (ExplosionSoundCommenced == true)
          {
          ExplosionIntervalTimer += dt;
          if (ExplosionIntervalTimer > 2000)
             {
             ExplosionSoundCommenced = false;
             ExplosionIntervalTimer = 0;
             }
          }
       }

    if (id == BOMB01)
       {
       static bool ExplosionSoundCommenced;
       static int ExplosionIntervalTimer = 0;
       MostRecentBombFlightTimer += dt;
       recgamma -= dt/15;
       gamma -= dt/15;
       if (recgamma < 109)
          {
          recgamma = 109;
          }
       if (gamma < 109)
          {
          gamma = 109;
          }
       float RunwayAlt;
       RunwayAlt = fabs(l->getExactHeight(AirfieldXMin+1, AirfieldYMin+2));
       if (tl->y < (RunwayAlt + 1))
          {
          if (ExplosionSoundCommenced == false)
             {
             float Xdistance;
             float Zdistance;
             float XZdistance;
             Xdistance = (fabs)(fplayer->tl->x - tl->x);
             Zdistance = (fabs)(fplayer->tl->z - tl->z);
             XZdistance = (Xdistance + Zdistance) * 2;
             if (XZdistance > 126)
                {
                XZdistance = 126;
                }
             sound->setVolume (SOUND_EXPLOSION1, (127 - (int)XZdistance));
             sound->play (SOUND_EXPLOSION1, false);
             ExplosionSoundCommenced = true;
             }
          }
       if (ExplosionSoundCommenced == true)
          {
          ExplosionIntervalTimer += dt;
          if (ExplosionIntervalTimer > 2000)
             {
             ExplosionSoundCommenced = false;
             ExplosionIntervalTimer = 0;
             }
          }
       }
    if (id >= STATIC_PASSIVE)   // only buildings, static objects
        {
        // set the correct angles to diplay
        rot->setAngles ((short) (90 + gamma - 180), (short) theta + 180, (short) -phi);
        checkDurability ();
        return; // and exit this function
        }
    if (id == FLARE1)   // only flares
        {
        tl->y -= 0.04 * timefac; // fall down (gravity, constant)
        zoom = 0.12F + 0.03F * sin ((float) ttl / (float) timestep / 15); // blink (high frequency)
        phi = camphi; // angles to viewer (player)
        theta = 0;
        gamma = camgamma;
        }
    if (id == CHAFF1)   // only chaff
        {
        tl->y -= 0.04 * timefac; // fall down (gravity, constant)
        zoom = 0.12F + 0.01F * (80 * timestep - ttl) / timestep; // spread out
        phi = camphi; // angles to viewer (player)
        theta = 0;
        gamma = camgamma;
        }
    (void) checkLooping ();
    // The core of directional alterations and force calculations:
    int vz = 1;
    if (gamma < 90 || gamma > 270)
        {
        vz = -1;
        }

    if (maxthrust + thrust <= -0.00001 || maxthrust + thrust >= 0.00001)
        {
        float DegreesToRadiansFactor = -0.01745333;

        phi += vz * SIN(theta) * elevatoreffect * manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        gamma += COS(theta) * elevatoreffect * manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        phi += -vz * COS(theta) * ruddereffect * manoeverability * (0.66 + 3.0 * realspeed) * timefac;
        gamma += SIN(theta) * ruddereffect * manoeverability * (0.66 + 3.0 * realspeed) * timefac;
        // change roll due to roll ;-)
        if (rolleffect)
            {
            theta += rolleffect * (RollRate * (1.0 + realspeed)) * timefac * 5.0F;
            rectheta = theta;
            }
        }
    if (phi < 0)
        {
        phi += 360.0;    // validate heading
        }
    else if (phi >= 360.0)
        {
        phi -= 360.0;
        }
    if (theta < -180 && rectheta < -180)
        {
        rectheta += 360;
        theta += 360;
        }
    else if (theta >= 180 && rectheta >= 180)
        {
        rectheta -= 360;
        theta -= 360;
        }
    if (recthrust > maxthrust)   // check maximum throttle
        {
        recthrust = maxthrust;
        }
    float throttlechange = maxthrust / 100 * timefac;
    if (recthrust > thrust + throttlechange)
        {
        thrust += (throttlechange * 2);
        }
    else if (recthrust < thrust - throttlechange )
        {
        thrust -= (throttlechange / 2);
        }

    if ( party == 0 && NetworkOpponent && (MissionNumber == MISSION_HEADTOHEAD00))
    {

    ThreeDObjects[MissionHeadToHead00State]->realspeed = InPacket.UdpObjSpeed;
    ThreeDObjects[MissionHeadToHead00State]->thrust = InPacket.UdpObjThrust;
    ThreeDObjects[MissionHeadToHead00State]->recthrust = InPacket.UdpObjThrust;
    }

    // PHYSICS (simplified model)
    CVector3 vaxis, uaxis, utemp, utemp2, utemp3;
    float gammaup, phiup, thetaup, RealSpeedTemp2;
    float ForceXsquared2, ForceYsquared2, ForceZsquared2;
    bool stop;
    float gravityforce;

    if (id <= CANNON2)
        {
        tl->x += forcex * timefac; // add our vector to the translation
        tl->z += forcez * timefac;
        tl->y += forcey * timefac;
        goto cannondone; // jump down to decrease ttl and test collision
        }

    vaxis.set (COS(gamma) * SIN(phi), SIN(gamma), COS(gamma) * COS(phi));

    if (realism)
        {

        gammaup = gamma + 90;
        thetaup = -theta;
        phiup = phi;
        uaxis.set (COS(gammaup) * SIN(phiup), SIN(gammaup), COS(gammaup) * COS(phiup)); // upward axis (theta = 0)

        utemp.take (&uaxis);
        utemp.mul (COS(thetaup));
        utemp2.take (&vaxis);
        utemp2.mul ((1 - COS(thetaup)) * uaxis.dotproduct (&vaxis));
        utemp3.take (&uaxis);
        utemp3.crossproduct (&vaxis);
        utemp3.mul (SIN(thetaup));
        utemp.add (&utemp2);
        utemp.add (&utemp3);
        uaxis.take (&utemp);

        braking = (fabs (ruddereffect * 20.0) + fabs (elevatoreffect * 35.0) + fabs (rolleffect * 18.0) + fplayer->DragEffect)/200;
        brakepower = pow (0.93 - braking, timefac);

        accx *= brakepower;
        accy *= brakepower;
        accz *= brakepower;

        accz += thrust * vaxis.z * 0.3 * timefac;
        accx += thrust * vaxis.x * 0.3 * timefac;
        accy -= thrust * vaxis.y * 0.1 * timefac;

        accz += thrust * uaxis.z * 0.067 * timefac;
        accx += thrust * uaxis.x * 0.067 * timefac;
        accy -= thrust * uaxis.y * 0.067 * timefac * cos((fplayer->theta) * DegreesToRadiansFactor);

        accy -= 0.015 * timefac;

        accy -= fplayer->deadweight * timefac;

        accy += sin((gamma - 180)* DegreesToRadiansFactor) * ((fplayer->InertiallyDampenedPlayerSpeed)/ 0.4 ); // was 0.8

        accx -= sin(fplayer->phi * DegreesToRadiansFactor) * ((fplayer->realspeed)/5);
        accz -= cos(fplayer->phi * DegreesToRadiansFactor) * ((fplayer->realspeed)/5);
        float stepfac = 0.24;
        tl->x += accx * timefac * stepfac;
        tl->z += accz * timefac * stepfac;
        tl->y += accy * timefac * stepfac;
        float scalef = 1.1;
        forcex = accx * stepfac * scalef;
        forcey = accy * stepfac * scalef;
        forcez = accz * stepfac * scalef;

        gravityforce = sqrt (realspeed) * vaxis.y * 1.10 * timefac;
        forcez += gravityforce * vaxis.z;
        forcex += gravityforce * vaxis.x;
        forcey += gravityforce * vaxis.y;

        forcey -= gravityforce * vaxis.y * 2.2;
        }
    stop = false;

    if (id >= TANK1 && id <= TANK2)   // tanks cannot climb steep faces
        {
        float newy = l->getExactHeight (tl->x + forcex, tl->z + forcez) + zoom / 2;

        if (fabs (newy - tl->y) > 0.05)
            {
            stop = true;
            }
        else if (fabs (newy - tl->y) > 2)
            {
            stop = false;
            }
        }

    //
    //  InertiaEffects
    //

    InertiaTimer += DeltaTime;

    unsigned static char InertiaPhase;

    if (InertiaTimer < (1000 * inertia))
       {
       InertiaPhase = 0;
       }
    else if (InertiaTimer < (2000 * inertia))
       {
       InertiaPhase = 1;
       }
    else if (InertiaTimer < (3000 * inertia))
       {
       InertiaPhase = 2;
       }
    else if (InertiaTimer < (4000 * inertia))
       {
       InertiaPhase = 3;
       }
    else if (InertiaTimer < (5000 * inertia))
       {
       InertiaPhase = 4;
       }
    else if (InertiaTimer < (6000 * inertia))
       {
       InertiaPhase = 5;
       }
    else if (InertiaTimer < (7000 * inertia))
       {
       InertiaPhase = 6;
       }
    else if (InertiaTimer < (8000 * inertia))
       {
       InertiaPhase = 7;
       }
    else if (InertiaTimer < (9000 * inertia))
       {
       InertiaPhase = 8;
       }
    else if (InertiaTimer < (10000 * inertia))
       {
       InertiaPhase = 9; // Last of 10 phases is phase "9".
       }

    SpeedHistoryArray[InertiaPhase] = (fplayer->realspeed);

    if (RecoveredFromStall)
       {
       for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
          {
          SpeedHistoryArray[HistoryArrayPointer] = fplayer->StallSpeed;
          RecoveredFromStall = false;
          }
       }

    if (ClearSpeedHistoryArrayFlag)
       {
       for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
          {
          SpeedHistoryArray[HistoryArrayPointer] = 0.0;
          ClearSpeedHistoryArrayFlag = false;
          }
       }

    for (HistoryArrayPointer=0; HistoryArrayPointer<=9; HistoryArrayPointer++)
        {
        SpeedHistoryAccumulator += SpeedHistoryArray[HistoryArrayPointer];
        }

    InertiallyDampenedPlayerSpeed =  SpeedHistoryAccumulator / 10;

    if (InertiaTimer > (int)(10000.0 * inertia))
        {
        InertiaTimer = 0;
        }

    if (!fplayer->ai)
       {
       TestAltitudeAirDensity();
       forcex /= AirDensityDrag;
       forcey /= AirDensityDrag;
       forcez /= AirDensityDrag;

       TestForExcessGamma();
       forcex /= GammaDrag;
       forcey /= GammaDrag;
       forcez /= GammaDrag;

       TestForLoopBeyondVertical();
       forcex /= LoopedBeyondVerticalDrag;
       forcey /= LoopedBeyondVerticalDrag;
       forcez /= LoopedBeyondVerticalDrag;

       TestDiveSpeedStructuralLimit();
       forcex /= SpeedBeyondStructuralLimitsDrag;
       forcey /= SpeedBeyondStructuralLimitsDrag;
       forcez /= SpeedBeyondStructuralLimitsDrag;

       TestForFlapEffects();
       forcex /= FlapDrag;
       forcey /= FlapDrag;
       forcez /= FlapDrag;

       TestForUndercarriageEffects();
       forcex /= UndercarriageDrag;
       forcey /= UndercarriageDrag;
       forcez /= UndercarriageDrag;

       TestForSpeedBrakeDrag();
       forcex /= SpeedBrakeDrag;
       forcey /= SpeedBrakeDrag;
       forcez /= SpeedBrakeDrag;

       RegulateXYZForces(forcex, forcey, forcez);
       forcex = RegulatedForceX;
       forcey = RegulatedForceY;
       forcez = RegulatedForceZ;

       RegulateGamma();

       }
    RealSpeedTemp2 = fabs(forcex) + fabs(forcey) + fabs(forcez);

    if (RealSpeedTemp2 <= 0)
       {
       RealSpeedTemp2 = 0.000001;
       }
    realspeed = (((RealSpeedTemp2 * 0.8) + InertiallyDampenedPlayerSpeed) /2.0);

    if (!fplayer->ai)
        {

        realspeed = (realspeed + InertiallyDampenedPlayerSpeed)/2;
        TestForWindNoise();
        }

    // objects moving on the ground should always change their elevation due to the surface
    if (id >= TANK1 && id <= TANK2 && thrust > 0 && !stop)
        {
        float newy = l->getExactHeight (tl->x, tl->z) + zoom / 2;
        float dy = newy - tl->y + forcey;
        float dx = fabs (forcex) + fabs (forcez);
        float gamma2 = 0;

        if (fabs (dx) > 0.0001)
            {
            gamma2 = atan (dy / dx);
            }

        gamma = 180.0 + 180.0 / PI * gamma2;
        tl->y = newy;
        }

        // set angles to correctly display the object
        rot->setAngles ((short) (90 + gamma - 180), (short) theta + 180, (short) -phi);

cannondone:
    ;

    if (ttl > 0)
        {
        ttl -= dt;

        if (ttl <= 0)
            {
            ttl = -1;

            if (id >= MISSILE1 && id <= MISSILE2)
                {
                recheight = -10;    // missiles drop
                }
            else
                {
                deactivate ();    // cannon shots vanish
                }
            }
        }

    checkDurability (); // check Durability issues
    crashGround (dt); // check ground collision

    if (immunity > 0)
        {
        immunity -= dt;
        }
    }

void AIObj::aiinit ()
    {
    int i;
    acttype = 0;
    dualshot = false;
    intelligence = 100;
    aggressivity = 100;
    precision = 100;
    Durability = 0.01F;
    ai = true;
    active = true;
    draw = true;
    target = NULL;
    dtheta = 0;
    dgamma = 0;
    id = MISSILE1;
    impact = 30;
    manoevertheta = 0;
    manoeverheight = 0;
    manoeverthrust = 0;
    idle = 0;
    smokettl = 0;
    firecannonttl = 0;
    firemissilettl = 0;
    fireflarettl = 0;
    firechaffttl = 0;
    flares = 0;
    aw = 0;
    source = NULL;
    points = 0;
    elevatoreffect = 0;
    ruddereffect = 0;
    gamma = 180;
    recgamma = 180;
    dgamma = 0;
    theta = 0;
    maxgamma = 70;
    maxtheta = 90;
    missiletype = 0;
    autofire = false;
    ttl = -1;
    ttf = 30 * timestep;

    for (i = 0; i < missiletypes; i ++)
        {
        missiles [i] = 0;
        }

    for (i = 0; i < missileracks; i ++)
        {
        missilerack [i] = -1;
        missilerackn [i] = 0;
        }

    bomber = 0;
    timer = 0;
    ammo = -1;
    manoeverstate = 0;
    }

void AIObj::missileCount ()
    {
    if (id < FIGHTER1 || id > FIGHTER2)
        {
        return;
        }

    int i;

    for (i = 0; i < missiletypes; i ++)
        {
        missiles [i] = 0;
        }

    for (i = 0; i < missileracks; i ++)
        {
        if (missilerackn [i] > 0)
            {
            missiles [missilerack [i]] += missilerackn [i];
            }
        }
    }

void AIObj::newinit (int id, int party, int intelligence, int precision, int aggressivity)
    {
    int i;
    ai = true;
    this->id = id;
    this->party = party;
    manoeverstate = 0;
    activate ();

    for (i = 0; i < missileracks; i ++)
        {
        missilerackn [i] = 0;
        }

    ammo = -1;
    bomber = 0;
    dualshot = false;
    float cubefac = 0.6F; // fighter
    float cubefac1 = 0.7F; // tanks and sams
    o = getModel (id);
    o->cubex = zoom;
    o->cubey = zoom;
    o->cubez = zoom;

    if (id == STATIC_AIRFIELD00)
       {
       o->cubex *= 90.0;
       o->cubey *= 4.0;
       o->cubez *= 28.0;
       maxDurability = 1530000;
       Durability = 1530000;
       }

    if (id == CANNON1)
       {
       statLethality = 2;
       impact = 2;
       }

    if (id == STATIC_RADARREFLECTOR)
       {
       statLethality = 0;
       impact = 10;
       maxDurability = 65000;
       Durability = 65000;
       o->cubex *= 6.0;
       o->cubey *= 1.0;
       o->cubez *= 5.0;
       }

    if (id == FIGHTER_P38L)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.81;
        RollRate = 0.60;
        manoeverability = 0.20;
        maxDurability = 85;
        zoom = 0.39;
        maxtheta = 90.0;
        maxgamma = 16.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.01;
        MaxFullPowerAltRatio = 0.77;
        inertia = 0.38;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.095;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_FIATG55)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.55;
        RollRate = 0.65;
        manoeverability = 0.22;
        maxDurability = 70;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2;
        ammo = 1600;
        impact = 600;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=1600;
        FlapsLevelElevatorEffect4=1600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.76;
        inertia = .26;
        deadweight = 0.08;
        CompressibilitySpeed = 0.295; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.38; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 840;
        }

    if (id == FIGHTER_A6M2)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.40; // At sea level, 1.51 yields 268MPH
        RollRate = 0.70;
        manoeverability = 0.24;
        maxDurability = 35;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 12.2;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.47;
        inertia = .255;
        deadweight = 0.07;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.26; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.28; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.09;
        ServiceCeilingAltitude = 750;
        }

    if (id == FIGHTER_KI43)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.384;
        RollRate = 0.70;
        manoeverability = 0.25;
        maxDurability = 30;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 1500;
        impact = 280;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.49;
        inertia = .25;
        deadweight = 0.06;
        CompressibilitySpeed = 0.23; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.26; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.280; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.09;
        ServiceCeilingAltitude = 730;
        }

    if (id == FIGHTER_IL16)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.384; //
        RollRate = 0.90;
        manoeverability = 0.22;
        maxDurability = 50;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 12.3;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.04;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.48;
        inertia = .31;
        deadweight = 0.07;
        CompressibilitySpeed = 0.24; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.24; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.25; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.25; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.28; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 713;
        }

    if (id == FIGHTER_F4F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.74;
        RollRate = 0.64;
        manoeverability = 0.21;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.15;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .325;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == FIGHTER_F6F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.72;
        RollRate = 0.64;
        manoeverability = 0.21;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .35;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == FIGHTER_F4U)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.76; // 3.0 yields 375 MPH at sea level
        RollRate = 0.67;
        manoeverability = 0.18;
        maxDurability = 84;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 9.23;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.75;
        inertia = .335;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.36; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.37; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 730;
        }

    if (id == FIGHTER_P47D)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.742;
        RollRate = 0.68;
        manoeverability = 0.14;
        maxDurability = 190;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 9.85;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1300;
        impact = 1150;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.04;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.92;
        inertia = .365;
        deadweight = 0.11;
        CompressibilitySpeed = 0.37; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.37; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.38; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.39; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.50; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_FW190)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.724; // 3.0 yields 375 MPH at sea level
        RollRate = 0.67;
        manoeverability = 0.15;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.08;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68;
        inertia = .325;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.36; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.37; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.105;
        ServiceCeilingAltitude = 640;
        }

    if (id == FIGHTER_P51D)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.844; // 2.05 yields 379MPH WEP at sea level.
        RollRate = 0.67;
        manoeverability = 0.18;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 8.62;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 106.9;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.79;
        inertia = .327;
        deadweight = 0.11;
        CompressibilitySpeed = 0.34; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.36; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.37; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 733;
        }

    else if (id == FIGHTER_HAWK)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 3;
        DefensiveLethality = 0;
        maxthrust = 3.0; // 3.0 yields 375 MPH at sea level
        RollRate = 0.70;
        manoeverability = 0.14;
        maxDurability = 85;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 24.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 6.0;
        ammo = 1000;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 94.0;
        ClipDistance = 0.03;
        FlapSpeed = .166;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.5;
        MaxFullPowerAltRatio = 0.78;
        inertia = .29;
        deadweight = 0.19;
        CompressibilitySpeed = 0.50; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.53; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.54; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.55; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.62; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.115;
        ServiceCeilingAltitude = 1000;
        }

    else if (id == BOMBER_B17)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 1;
        DefensiveLethality = 39;
        maxthrust = 1.465;
        RollRate = 0.28;
        manoeverability = 0.13;
        maxDurability = 700;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 11.07;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 1.02;
        inertia = .5556;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.35; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 680;
        }

    else if (id == BOMBER_B24)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 2;
        DefensiveLethality = 30;
        maxthrust = 1.492;
        RollRate = 0.25;
        manoeverability = 0.12;
        maxDurability = 650;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 9.85;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.80;
        inertia = .610;
        deadweight = 0.13;
        CompressibilitySpeed = 0.26; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.26; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.28; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.34; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 550;
        }

    else if (id == FIGHTER_SPIT9)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.69; // WEP 1.88 yields 401MPH, 1.68 yields 356MPH
        RollRate = 0.55;
        manoeverability = 0.21;
        maxDurability = 70;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma =17.8;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2200;
        FlapsLevelElevatorEffect2=2200;
        FlapsLevelElevatorEffect3=2200;
        FlapsLevelElevatorEffect4=2200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .355;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 840;
        }

    else if (id == FIGHTER_ME109G)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.653;
        RollRate = 0.5;
        manoeverability = 0.20;
        maxDurability = 65;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.94;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1500;
        impact = 500;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=1600;
        FlapsLevelElevatorEffect4=1600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.62;
        inertia = .34;
        deadweight = 0.10;
        CompressibilitySpeed = 0.32; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.32; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.106;
        ServiceCeilingAltitude = 760;
        }

    else if (id == FIGHTER_HURRICANE)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.626;
        RollRate = 0.5;
        manoeverability = 0.22;
        maxDurability = 75;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 16.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1650;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=1600;
        FlapsLevelElevatorEffect4=1600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.60;
        inertia = .335;
        deadweight = 0.10;
        CompressibilitySpeed = 0.30; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.36; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.101;
        ServiceCeilingAltitude = 713;
        }

    else if (id == FIGHTER_P40)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.626;
        RollRate = 0.5;
        manoeverability = 0.19;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 9.85;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1550;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=3600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.55;
        inertia = .336;
        deadweight = 0.10;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.104;
        ServiceCeilingAltitude = 680;
        }

    else if (id == FIGHTER_P39)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.742;
        RollRate = 0.5;
        manoeverability = 0.19;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 9.85;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.6;
        ammo = 1550;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=3600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.40;
        inertia = .34;
        deadweight = 0.10;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.104;
        ServiceCeilingAltitude = 680;
        }

    else if (id == FIGHTER_YAK9)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.983;
        RollRate = 0.5;
        manoeverability = 0.19;
        maxDurability = 65;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 17.72;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1550;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.61;
        inertia = .34;
        deadweight = 0.10;
        CompressibilitySpeed = 0.31; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.31; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.104;
        ServiceCeilingAltitude = 660;
        }

    else if (id == FIGHTER_N1K1)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.858;
        RollRate = 0.60;
        manoeverability = 0.20;
        maxDurability = 75;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 16.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.4;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 117.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.70;
        inertia = .39;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.35; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.103;
        ServiceCeilingAltitude = 680;
        }

    if (id == BOMBER_JU87)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.43;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 330;
        zoom = 0.4;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = .501;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_G5M)
        {
        o->cubex = zoom * cubefac * 1.6;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.6;
        HistoricPeriod = 1;
        DefensiveLethality = 10;
        maxthrust = 1.43;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 55;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 15.38;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.60;
        inertia = .501;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_B25)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 2;
        DefensiveLethality = 12;
        maxthrust = 1.48;
        RollRate = 0.26;
        manoeverability = 0.17;
        maxDurability = 170;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 12.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.65;
        inertia = .501;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_B26)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 12;
        maxthrust = 1.43;
        RollRate = 0.26;
        manoeverability = 0.17;
        maxDurability = 170;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 14.77;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 9.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.65;
        inertia = .501;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == FIGHTER_LA5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.768;
        RollRate = 0.67;
        manoeverability = 0.18;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.70;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68;
        inertia = .337;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.36; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.37; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.105;
        ServiceCeilingAltitude = 640;
        }

    if (id == FIGHTER_LA7)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.777;
        RollRate = 0.67;
        manoeverability = 0.17;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.44;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=3600;
        FlapsLevelElevatorEffect4=3600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.68;
        inertia = .337;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.36; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.37; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.105;
        ServiceCeilingAltitude = 640;
        }

    if (id == FIGHTER_IL2)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 3;
        maxthrust = 1.45;
        RollRate = 0.67;
        manoeverability = 0.15;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 12.43;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.38;
        inertia = .34;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.35; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.36; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.37; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.55; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.105;
        ServiceCeilingAltitude = 640;
        }

    if (id == FIGHTER_MACCIC202)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.710;
        RollRate = 0.68;
        manoeverability = 0.22;
        maxDurability = 85;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 15.5;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.5;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.72;
        inertia = 0.36;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.095;
        ServiceCeilingAltitude = 880;
        }

    if (id == BOMBER_LANCASTER)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 2;
        DefensiveLethality = 7;
        maxthrust = 1.41;
        RollRate = 0.28;
        manoeverability = 0.13;
        maxDurability = 700;
        zoom = 1.0;
        maxtheta = 90.0;
        maxgamma = 11.07;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 1.02;
        inertia = .555086;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.35; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 680;
        }

    if (id == BOMBER_MOSQUITOB)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.761;
        RollRate = 0.45;
        manoeverability = 0.20;
        maxDurability = 85;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 16.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=2600;
        FlapsLevelElevatorEffect3=2600;
        FlapsLevelElevatorEffect4=2600;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.70;
        inertia = 0.376;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.097;
        ServiceCeilingAltitude = 810;
        }

    if (id == FIGHTER_TYPHOON)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.760;
        RollRate = 0.60;
        manoeverability = 0.20;
        maxDurability = 85;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 16.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.01;
        MaxFullPowerAltRatio = 0.77;
        inertia = 0.377;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.095;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_YAK1)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.96;
        RollRate = 0.5;
        manoeverability = 0.19;
        maxDurability = 62;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 16.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 1550;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 105.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.55;
        inertia = .30;
        deadweight = 0.10;
        CompressibilitySpeed = 0.29; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.29; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.31; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.32; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.38; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.100;
        ServiceCeilingAltitude = 550;
        }

    if (id == BOMBER_B29)
        {
        o->cubex = zoom * cubefac * 2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 2;
        HistoricPeriod = 2;
        DefensiveLethality = 30;
        maxthrust = 1.55;
        RollRate = 0.25;
        manoeverability = 0.12;
        maxDurability = 650;
        zoom = 1.25;
        maxtheta = 90.0;
        maxgamma = 9.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.80;
        inertia = .66000;
        deadweight = 0.13;
        CompressibilitySpeed = 0.26; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.26; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.28; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.34; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 550;
        }

    if (id == FIGHTER_DW520)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.66;
        RollRate = 0.64;
        manoeverability = 0.21;
        maxDurability = 90;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 13.30;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.65;
        inertia = .275;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == BOMBER_SB2C)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 2;
        DefensiveLethality = 6;
        maxthrust = 1.59;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 330;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 12.20;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_TBF)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 2;
        DefensiveLethality = 7;
        maxthrust = 1.55;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 330;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == FIGHTER_ME163)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 3.1;
        RollRate = 0.60;
        manoeverability = 0.20;
        maxDurability = 60;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 42.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 13.0;
        ammo = 120;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=3600;
        FlapsLevelElevatorEffect3=3600;
        FlapsLevelElevatorEffect4=3600;
        SpeedBrakePower=1.1;
        MaxFullPowerAltRatio = 1.00;
        inertia = 0.375;
        deadweight = 0.13;
        CompressibilitySpeed = 0.36; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.36; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.44; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.46; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.48; // Faster than this will induce severe airframe damage.
        WepCapable = false;
        StallSpeed = 0.090;
        ServiceCeilingAltitude = 920;
        }

    if (id == FIGHTER_TEMPEST)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.83;
        RollRate = 0.50;
        manoeverability = 0.15;
        maxDurability = 65;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 17.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 10.0;
        ammo = 800;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.55;
        inertia = 0.33;
        deadweight = 0.13;
        CompressibilitySpeed = 0.28; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.28; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.39; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.110;
        ServiceCeilingAltitude = 650;
        }

    if (id == FIGHTER_D3A)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 1;
        maxthrust = 1.4;
        RollRate = 0.33;
        manoeverability = 0.15;
        maxDurability = 100;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 15.5;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.7;
        ammo = 1200;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = .50214;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.08;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_B5N)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 1;
        maxthrust = 1.45;
        RollRate = 0.30;
        manoeverability = 0.15;
        maxDurability = 250;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 10.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 0.8;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=4800;
        FlapsLevelElevatorEffect4=4800;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_DAUNTLESS)
        {
        o->cubex = zoom * cubefac * 1.2;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.2;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.47;
        RollRate = 0.44;
        manoeverability = 0.16;
        maxDurability = 220;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 13.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.2;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.62;
        inertia = .44000;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == FIGHTER_ME110)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 2;
        maxthrust = 1.75;
        RollRate = 0.44;
        manoeverability = 0.16;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 5.5;
        ammo = 1200;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2500;
        FlapsLevelElevatorEffect2=2500;
        FlapsLevelElevatorEffect3=2500;
        FlapsLevelElevatorEffect4=2500;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .55000;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == BOMBER_DORNIER)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 6;
        maxthrust = 1.40;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 200;
        zoom = 0.60;
        maxtheta = 90.0;
        maxgamma = 14.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_HE111)
        {
        o->cubex = zoom * cubefac * 1.4;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.4;
        HistoricPeriod = 1;
        DefensiveLethality = 7;
        maxthrust = 1.45;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 200;
        zoom = 0.60;
        maxtheta = 90.0;
        maxgamma = 11.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == BOMBER_JU88)
        {
        o->cubex = zoom * cubefac * 1.5;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac * 1.5;
        HistoricPeriod = 2;
        DefensiveLethality = 6;
        maxthrust = 1.6;
        RollRate = 0.25;
        manoeverability = 0.15;
        maxDurability = 250;
        zoom = 0.70;
        maxtheta = 90.0;
        maxgamma = 10.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 1.3;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=3200;
        FlapsLevelElevatorEffect2=3200;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=3200;
        SpeedBrakePower=1.30;
        MaxFullPowerAltRatio = 0.48;
        inertia = 0.8;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.27; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.28; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.11;
        ServiceCeilingAltitude = 513;
        }

    if (id == FIGHTER_KI84)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.81;
        RollRate = 0.50;
        manoeverability = 0.17;
        maxDurability = 70;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 15.4;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.5;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.01;
        MaxFullPowerAltRatio = 0.70;
        inertia = 0.375;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.095;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_KI61)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.70;
        RollRate = 0.62;
        manoeverability = 0.21;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 14.9;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 800;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .325;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.27; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.29; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.37; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 713;
        }

    if (id == FIGHTER_GENERIC01)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.60;
        RollRate = 0.60;
        manoeverability = 0.20;
        maxDurability = 85;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 16.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.01;
        MaxFullPowerAltRatio = 0.77;
        inertia = 0.375;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.095;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_A6M5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 2;
        DefensiveLethality = 0;
        maxthrust = 1.5; // At sea level, 1.51 yields 268MPH
        RollRate = 0.70;
        manoeverability = 0.24;
        maxDurability = 35;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 12.00;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.2;
        ammo = 1600;
        impact = 300;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .111;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2400;
        FlapsLevelElevatorEffect2=2400;
        FlapsLevelElevatorEffect3=2400;
        FlapsLevelElevatorEffect4=2400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.47;
        inertia = .26;
        deadweight = 0.07;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.25; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.26; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.26; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.28; // Faster than this will induce severe airframe damage
        WepCapable = false;
        StallSpeed = 0.09;
        ServiceCeilingAltitude = 750;
        }

    if (id == FIGHTER_SPIT5)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.61; // WEP 1.88 yields 401MPH, 1.68 yields 356MPH
        RollRate = 0.56;
        manoeverability = 0.23;
        maxDurability = 70;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma =18.5;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.0;
        ammo = 1600;
        impact = 700;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 108.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=2200;
        FlapsLevelElevatorEffect2=2200;
        FlapsLevelElevatorEffect3=2200;
        FlapsLevelElevatorEffect4=2200;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.72;
        inertia = .35;
        deadweight = 0.11;
        CompressibilitySpeed = 0.33; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.35; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.44; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.09;
        ServiceCeilingAltitude = 800;
        }

    if (id == FIGHTER_P51B)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.80; // 2.05 yields 379MPH WEP at sea level.
        RollRate = 0.69;
        manoeverability = 0.19;
        maxDurability = 80;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 8.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 2.4;
        ammo = 1600;
        impact = 750;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 106.9;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.76;
        inertia = .3392;
        deadweight = 0.11;
        CompressibilitySpeed = 0.34; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.36; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.37; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.38; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.45; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 733;
        }

    if (id == FIGHTER_P47B)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.72;
        RollRate = 0.68;
        manoeverability = 0.145;
        maxDurability = 190;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 7.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 1300;
        impact = 1150;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 107.0;
        ClipDistance = 0.04;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=6400;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.92;
        inertia = .343;
        deadweight = 0.11;
        CompressibilitySpeed = 0.37; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.37; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.38; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.39; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.50; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 880;
        }

    if (id == FIGHTER_ME109F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.64;
        RollRate = 0.5;
        manoeverability = 0.22;
        maxDurability = 63;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 11.1;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 3.0;
        ammo = 1500;
        impact = 500;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 110.0;
        ClipDistance = 0.03;
        FlapSpeed = .122;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=1600;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=1600;
        FlapsLevelElevatorEffect4=1600;
        SpeedBrakePower=1.0;
        MaxFullPowerAltRatio = 0.62;
        inertia = .33000;
        deadweight = 0.10;
        CompressibilitySpeed = 0.32; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active.
        CompressibilitySpeedWithSpeedBrakes = 0.32; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.33; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.43; // Faster than this will induce severe airframe damage
        WepCapable = true;
        StallSpeed = 0.10;
        ServiceCeilingAltitude = 760;
        }

    if (id == FIGHTER_P38F)
        {
        o->cubex = zoom * cubefac;
        o->cubey = zoom * cubefac;
        o->cubez = zoom * cubefac;
        HistoricPeriod = 1;
        DefensiveLethality = 0;
        maxthrust = 1.805;
        RollRate = 0.55;
        manoeverability = 0.21;
        maxDurability = 85;
        zoom = 0.35;
        maxtheta = 90.0;
        maxgamma = 15.0;
        missilerackn [0] = 0;
        missilerackn [1] = 0;
        missilerackn [2] = 0;
        missilerackn [3] = 0;
        missilerack [0] = 0;
        missilerack [1] = 0;
        missilerack [2] = 0;
        missilerack [3] = 0;
        flares = 0;
        chaffs = 0;
        statLethality = 4.0;
        ammo = 2450;
        impact = 1000;
        BlackoutSensitivity = 20.0F;
        RedoutSensitivity = 40.0F;
        StaticDrag = 104.0;
        ClipDistance = 0.03;
        FlapSpeed = .133;
        FlapsLevelElevatorEffect0=0;
        FlapsLevelElevatorEffect1=800;
        FlapsLevelElevatorEffect2=1600;
        FlapsLevelElevatorEffect3=3200;
        FlapsLevelElevatorEffect4=8000;
        SpeedBrakePower=1.00;
        MaxFullPowerAltRatio = 0.77;
        inertia = 0.37;
        deadweight = 0.13;
        CompressibilitySpeed = 0.25; // Faster than this degrades elevator and aileron response unless SpeedBrakes are available and active. Was .29
        CompressibilitySpeedWithSpeedBrakes = 0.33; // Replaces above setting when SpeedBrakes are active.
        DiveSpeedLimitThreshold = 0.34; // Faster than begins to regulate overspeed. 0.36 ~= 414 MPH
        DiveSpeedLimit1 = 0.34; // Faster than this induces extra turbulence
        DiveSpeedStructuralLimit = 0.42; // Faster than this will induce severe airframe damage.
        WepCapable = true;
        StallSpeed = 0.093;
        ServiceCeilingAltitude = 870;
        }

    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        recthrust = maxthrust;
        Durability = maxDurability;
        thrust = recthrust = maxthrust;
        smoke->type = 1;
        impact = 2;
        forcez = recthrust;
        }

    if (id == FLAK_AIR1)
        {
        maxthrust = 0;
        thrust = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 12.0;
        Durability = maxDurability = 80;
        zoom = 0.35;
        }

    if (id == FLARAK_AIR1)
        {
        maxthrust = 0;
        thrust = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 6.0;
        Durability = maxDurability = 70;
        zoom = 0.3;
        missiles [6] = 100;
        }

    if (id >= FLAK1 && id <= FLAK2)
        {
        o->cubex = zoom * cubefac1;
        o->cubey = zoom * cubefac1;
        o->cubez = zoom * cubefac1;
        }

    if (id == TANK_AIR1)
        {
        maxthrust = 0.04;
        thrust = 0;
        gamma = 180;
        theta = 0;
        phi = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 8.0;
        Durability = maxDurability = 160;
        zoom = 0.35;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.45;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_GROUND1)
        {
        maxthrust = 0.04;
        thrust = 0;
        gamma = 180;
        theta = 0;
        phi = 0;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 8.0;
        Durability = maxDurability = 200;
        zoom = 0.4;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_PICKUP1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 30;
        zoom = 0.25;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.55;
        o->cubez = zoom * 0.7;
        }
    else if (id == TANK_TRUCK1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 20;
        zoom = 0.45;
        o->cubex = zoom * 0.6;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.6;
        }
    else if (id == TANK_TRUCK2)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 40;
        zoom = 0.4;
        o->cubex = zoom * 0.6;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.6;
        }
    else if (id == TANK_TRSAM1)
        {
        maxthrust = 0;
        thrust = 0.02;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 0;
        Durability = maxDurability = 50;
        zoom = 0.35;
        missiles [6] = 200;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.6;
        o->cubez = zoom * 0.7;
        }

    if (id >= TANK1 && id <= TANK2)
        {
        }

    if (id == SHIP_CRUISER)
        {
        zoom = 5.0;
        maxthrust = 0.05;
        thrust = 0.05;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 4.0;
        impact = 200;
        Durability = maxDurability = 9500;
        missiles [6] = 200;
        o->cubex = zoom * 0.35;
        o->cubey = zoom * 0.1;
        o->cubez = zoom * 0.35;
        }
    else if (id == SHIP_DESTROYER1)
        {
        zoom = 2.5;
        maxthrust = 0.05;
        thrust = 0.05;
        maxgamma = 0;
        maxtheta = 0.03;
        manoeverability = 6.0;
        impact = 300;
        Durability = maxDurability = 4800;
        o->cubex = zoom * 0.4;
        o->cubey = zoom * 0.12;
        o->cubez = zoom * 0.4;
        }

    float missilethrustbase = 6.5F;

    if (id == BOMB01)
        {
        intelligence = 0;
        maxthrust = 0.01 * missilethrustbase;
        RollRate = 0.0;
        manoeverability = 0.0;
        ai = true;
        StaticDrag = 8;
        ttl = 2000 * timestep;
        impact = 30000;
        }
    else if (id == MISSILE_AIR2)
        {
        intelligence = 100;
        maxthrust = 0.5 * missilethrustbase;
        RollRate = 3.5;
        manoeverability = 4.5;
        StaticDrag = 8;
        ttl = 320 * timestep;
        impact = 2100;
        }
    else if (id == MISSILE_AIR3)
        {
        intelligence = 100;
        maxthrust = 0.65 * missilethrustbase;
        RollRate = 4.5;
        manoeverability = 5.5;
        StaticDrag = 8;
        ttl = 340 * timestep;
        impact = 2200;
        }
    else if (id == MISSILE_GROUND1)
        {
        intelligence = 10;
        maxthrust = 0.60 * missilethrustbase;
        RollRate = 1.2;
        manoeverability = 1.0;
        ai = true;
        StaticDrag = 8;
        ttl = 300 * timestep;
        impact = 3000;
        }
    else if (id == MISSILE_GROUND2)
        {
        intelligence = 10;
        maxthrust = 0.75 * missilethrustbase;
        RollRate = 1.5;
        manoeverability = 1.0;
        ai = true;
        StaticDrag = 8;
        ttl = 400 * timestep;
        impact = 4000;
        }
    else if (id == MISSILE_DF1)
        {
        intelligence = 1;
        maxthrust = 0.80 * missilethrustbase;
        RollRate = 0.0;
        manoeverability = 0.0;
        ai = true;
        StaticDrag = 8;
        ttl = 350 * timestep;
        impact = 8000;
        }
    else if (id == MISSILE_FF1)
        {
        intelligence = 100;
        maxthrust = 0.85 * missilethrustbase;
        RollRate = 2.0;
        manoeverability = 5.0;
        StaticDrag = 10;
        ttl = 300 * timestep;
        impact = 2100;
        }
    else if (id == MISSILE_FF2)
        {
        intelligence = 100;
        maxthrust = 0.90 * missilethrustbase;
        RollRate = 3.0;
        manoeverability = 6.0;
        StaticDrag = 10;
        ttl = 320 * timestep;
        impact = 2200;
        }
    else if (id == MISSILE_MINE1)
        {
        intelligence = 0;
        maxthrust = 0.1;
        RollRate = 1.0;
        manoeverability = 1.0;
        ai = true;
        StaticDrag = 8;
        ttl = -1;
        impact = 500;
        zoom = 0.3;
        }

    if (id >= MISSILE1 && id <= MISSILE2)
        {
        o->cubex = zoom;
        o->cubey = zoom;
        o->cubez = zoom;
        }
    if (id >= STATIC_PASSIVE)
        {
        intelligence = 0;
        maxthrust = 0;
        RollRate = 0;
        manoeverability = 0;
        impact = 5;
        maxtheta = 0;
        maxgamma = 0;
        }

    if (id == STATIC_CONTAINER1)
        {
        Durability = maxDurability = 30;
        zoom = 1.0;
        impact = 20;
        o->cubex = zoom * 0.4;
        o->cubey = zoom * 0.35;
        o->cubez = zoom * 0.9;
        }

    if (id == STATIC_HALL1)
        {
        Durability = maxDurability = 450;
        zoom = 1.8;
        impact = 20;
        o->cubex = zoom * 0.45;
        o->cubey = zoom * 0.42;
        o->cubez = zoom;
        }

    if (id == STATIC_HALL2)
        {
        Durability = maxDurability = 900;
        zoom = 2.5;
        impact = 20;
        o->cubex = zoom;
        o->cubey = zoom * 0.45;
        o->cubez = zoom;
        }

    if (id == STATIC_OILRIG1)
        {
        Durability = maxDurability = 1400;
        zoom = 3.5;
        impact = 20;
        o->cubex = zoom * 0.95;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.95;
        }

    if (id == STATIC_COMPLEX1)
        {
        Durability = maxDurability = 5000;
        zoom = 2.0;
        impact = 20;
        o->cubex = zoom * 0.75;
        o->cubey = zoom * 0.6;
        o->cubez = zoom * 0.75;
        }

    if (id == STATIC_RADAR1)
        {
        Durability = maxDurability = 500;
        zoom = 1.3;
        impact = 20;
        o->cubex = zoom * 0.5;
        o->cubey = zoom * 0.7;
        o->cubez = zoom * 0.5;
        }

    if (id == STATIC_BASE1)
        {
        Durability = maxDurability = 5500;
        zoom = 4.0;
        impact = 20;
        o->cubex = zoom * 0.7;
        o->cubey = zoom * 0.5;
        o->cubez = zoom * 0.7;
        }

    if (id == STATIC_DEPOT1)
        {
        Durability = maxDurability = 3000;
        zoom = 1.5;
        impact = 20;
        o->cubex = zoom;
        o->cubey = zoom * 0.5;
        o->cubez = zoom;
        }

    if (id == STATIC_BARRIER1)
        {
        Durability = maxDurability = 1000;
        zoom = 12.0;
        impact = 2000;
        o->cubex = 0.8;
        o->cubey = 11;
        o->cubez = 11;
        }

    this->intelligence = intelligence;
    this->precision = precision;
    this->aggressivity = aggressivity;
    missileCount ();
    }

void AIObj::newinit (int id, int party, int intelligence)
    {
    newinit (id, party, intelligence, intelligence, intelligence);
    }

AIObj::AIObj ()
    {
    o = NULL;
    zoom = 1.0;
    aiinit ();
    smoke = new CSmoke (0);
    }

AIObj::AIObj (Space *space2, CModel *o2, float zoom2)
    {
    this->space = space2;
    o = o2;
    zoom = zoom2;
    aiinit ();
    smoke = new CSmoke (0);
    space->addObject (this);
    }

AIObj::~AIObj ()
    {
    delete smoke;
    }

void AIObj::initValues (DynamicObj *dobj, float phi)
    {
    float fac = zoom / 8;

    if (dobj->id == FLARE1 || dobj->id == CHAFF1)
        {
        fac = -fac;
        }

    // use the exact polar coordinates because of gamma and theta
    float cgamma = gamma;
    dobj->tl->x = tl->x + COS(cgamma) * SIN(phi) * fac;
    dobj->tl->y = tl->y - SIN(cgamma) * fac;

    if ((id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
        {
        dobj->tl->y += fac;
        }

    dobj->tl->z = tl->z + COS(cgamma) * COS(phi) * fac;
    dobj->phi = phi;
    dobj->rectheta = dobj->theta;
    dobj->forcex = forcex;
    dobj->forcey = forcey;
    dobj->forcez = forcez;
    dobj->rot->setAngles ((short) (90 + dobj->gamma - 180), (short) dobj->theta + 180, (short) -dobj->phi);
    }

void AIObj::fireCannon (DynamicObj *laser, float phi)
    {
    if (firecannonttl > 0)
        {
        return;
        }

    if (ammo == 0)
        {
        return;
        }

    ammo --;
    laser->thrust = 0;
    laser->recthrust = laser->thrust;
    laser->manoeverability = 0.0;
    laser->maxthrust = 0;

    if (target != NULL && ai)
        {
        if (target->active)
            {
            // exact calculation to hit enemy (non-static turret!)
            if (id >= FIGHTER1 && id <= FIGHTER2)
                {
                laser->gamma = gamma;
                }
            else
                {
                laser->gamma = 180.0 + atan ((target->tl->y - tl->y) / distance (target)) * 180.0 / pitab;
                }
            }
        }
    else
        {
        laser->gamma = gamma;
        }

    laser->party = party;
    laser->ttl = 80 * timestep;
    laser->Durability = 1;
    laser->immunity = (int) (zoom * 3) * timestep;
    laser->source = this;
    laser->phi = phi;
    laser->theta = theta;
    initValues (laser, phi);
    float fac = 0.7F;
    laser->forcex += COS(laser->gamma) * SIN(laser->phi) * fac;
    laser->forcey -= SIN(laser->gamma) * fac;
    laser->forcez += COS(laser->gamma) * COS(laser->phi) * fac;
    laser->activate ();
    firecannonttl += 45;

    if (day)
        {
        if (dualshot)
            {
            laser->o = &model_cannon1b;
            }
        else
            {
            laser->o = &model_cannon1;
            }
        }
    else
        {
        if (dualshot)
            {
            laser->o = &model_cannon2b;
            }
        else
            {
            laser->o = &model_cannon2;
            }
        }
    }

void AIObj::fireCannon (DynamicObj **laser, float phi)
    {
    int i;

    if (firecannonttl > 0)
        {
        return;
        }

    if (ammo == 0)
        {
        return;
        }

    for (i = 0; i < maxlaser; i ++)
        {
        if (!laser [i]->active)
            {
            break;
            }
        }

    if (i < maxlaser)
        {
        fireCannon (laser [i], phi);
        }
    }

void AIObj::fireCannon (DynamicObj **laser)
    {
    if (firecannonttl > 0)
        {
        return;
        }

    if (ammo == 0)
        {
        return;
        }

    fireCannon (laser, phi);
    }

void AIObj::fireMissile2 (int id, AIObj *missile, AIObj *target)
    {
    char buf [STDSIZE];
    sprintf (DebugBuf, "Missile: party=%d, id=%d", party, id);
    display (DebugBuf, LOG_MOST);
    ttf = 50 * timestep;
    missile->dinit ();
    missile->aiinit ();
    missile->newinit (id, party, 0);
    initValues (missile, phi);
    missile->id = id;
    missile->explode = 0;
    missile->thrust = thrust + 0.001;
    missile->recthrust = missile->maxthrust;
    missile->gamma = gamma;
    missile->target = target;
    missile->recgamma = gamma;
    missile->Durability = 1;
    missile->party = party;
    missile->immunity = (45 + (int) (zoom * 6.0)) * timestep;
    missile->dtheta = 0;
    missile->dgamma = 0;
    missile->source = this;
    if (missile->id == MISSILE_DF1)
       {
       display ("AIObj::fireMissile2() dropped MISSILE_DF1", LOG_MOST);
       missile->gamma -= 11;
       missile->recgamma -= 11;
       if (missile->gamma < 0)
          {
          missile->gamma = 0;
          missile->recgamma = 0;
          }
        missile->tl->y -=0.3;
       }
    if (missile->id == BOMB01)
       {
       display ("AIObj::fireMissile2() dropped BOMB02", LOG_MOST);
       MostRecentBombFlightTimer = 0;
       missile->immunity = (30 + (int) (zoom * 6.0)) * timestep;
       missile->gamma -=10;
       missile->recgamma -=10;
       if (missile->gamma <0)
          {
          missile->gamma = 0;
          missile->recgamma = 0;
          }
       }
    if (missile->id > BOMB01 && missile->id < FIGHTER1)
       {
       MissileFired = true;
       }
    display ("AIObj::fireMissile2() missile->gamma =", LOG_MOST);
    sprintf (DebugBuf, "%f", missile->gamma);
    display (DebugBuf, LOG_MOST);
    missile->activate ();

    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        missile->manoeverheight = 30 * timestep;
        missile->recheight = missile->tl->y - l->getHeight (missile->tl->x, missile->tl->z) - 4;
        }
    }

void AIObj::fireFlare2 (DynamicObj *flare)
    {
    char buf [STDSIZE];

    if (DebugLevel == LOG_ALL)
        {
        sprintf (buf, "Flare: party=%d", party);
        display (buf, DebugLevel);
        }

    flare->dinit ();
    flare->thrust = 0;
    flare->realspeed = 0;
    flare->recthrust = 0;
    flare->manoeverability = 0.0;
    flare->maxthrust = 1.0;
    flare->gamma = 0;
    flare->party = party;
    flare->ttl = 80 * timestep;
    flare->Durability = 1;
    flare->immunity = (int) (zoom * 12) * timestep;
    flare->source = this;
    flare->phi = phi;
    flare->id = FLARE1;
    initValues (flare, phi);
    flare->activate ();
    flare->explode = 0;
    }

void AIObj::fireChaff2 (DynamicObj *chaff)
    {
    char buf [STDSIZE];

    if (debug == LOG_ALL)
        {
        sprintf (buf, "Chaff: party=%d", party);
        display (buf, LOG_ALL);
        }

    chaff->dinit ();
    chaff->thrust = 0;
    chaff->realspeed = 0;
    chaff->recthrust = 0;
    chaff->manoeverability = 0.0;
    chaff->maxthrust = 1.0;
    chaff->gamma = 0;
    chaff->party = party;
    chaff->ttl = 80 * timestep;
    chaff->Durability = 1;
    chaff->immunity = (int) (zoom * 12) * timestep;
    chaff->source = this;
    chaff->phi = phi;
    chaff->id = CHAFF1;
    initValues (chaff, phi);
    chaff->activate ();
    chaff->explode = 0;
    chaff->zoom = 0.12F;
    }

int AIObj::firstMissile ()
    {
    int i = 0;

    while (!missiles [i])
        {
        i ++;

        if (i >= missiletypes)
            {
            return 0;
            }
        }

    ttf = 50 * timestep;
    return i;
    }

int AIObj::nextMissile (int from)
    {
    int i = from + 1;

    if (i >= missiletypes)
        {
        i = 0;
        }

    while (!missiles [i])
        {
        i ++;

        if (i >= missiletypes)
            {
            i = 0;
            }

        if (i == from)
            {
            break;
            }
        }

    ttf = 50 * timestep;
    return i;
    }

bool AIObj::haveMissile (int id)
    {
    char buf [STDSIZE];
    id -= MISSILE1;

    if (id < 0 || id >= missiletypes)
        {
        sprintf (buf, "Wrong missile ID in %s, line %d", __FILE__, __LINE__);
        display (buf, LOG_ERROR);
        }

    if (missiles [id] > 0)
        {
        return true;
        }

    return false;
    }

bool AIObj::haveMissile () // due to missiletype
    {
    if (missiles [missiletype] > 0)
        {
        return true;
        }

    return false;
    }

void AIObj::decreaseMissile (int id)
    {
    char buf [STDSIZE];
    int i;
    id -= MISSILE1;

    if (id < 0 || id >= missiletypes)
        {
        sprintf (buf, "Wrong missile ID in %s, line %d", __FILE__, __LINE__);
        display (buf, LOG_ERROR);
        }

    missiles [id] --;
    int ptrrack = 0, maxrack = 0;

    for (i = 0; i < missileracks; i ++)
        if (missilerack [i] == id)
            if (missilerackn [i] > maxrack)
                {
                ptrrack = i;
                maxrack = missilerackn [i];
                }

    if (maxrack > 0)
        {
        missilerackn [ptrrack] --;
        refscale [ptrrack * 3 + 2 - missilerackn [ptrrack]] = 0;
        }
    }

bool AIObj::fireMissile (int id, AIObj **missile, AIObj *target)
    {
    int i;

    if (!haveMissile (id))
        {
        return false;
        }
    if (ttf > 0)
        {
        return false;
        }
    for (i = 0; i < maxmissile; i ++)
        {
        if (missile [i]->ttl <= 0)
            {
            break;
            }
        }
    if (i < maxmissile)
        {
        fireMissile2 (id, missile [i], target);
        decreaseMissile (id);
        firemissilettl = 20 * timestep;
        return true;
        }
    return false;
    }

bool AIObj::fireMissile (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }

    return fireMissile (missiletype + MISSILE1, missile, (AIObj *) target);
    }

bool AIObj::fireMissile (int id, AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }

    return fireMissile (id, missile, (AIObj *) target);
    }

bool AIObj::fireMissile (AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }

    return fireMissile (missiletype + MISSILE1, missile);
    }

bool AIObj::fireFlare (DynamicObj **flare, AIObj **missile)
    {
    int i, i2;

    if (flares <= 0)
        {
        return false;
        }

    if (fireflarettl > 0)
        {
        return false;
        }

    for (i = 0; i < maxflare; i ++)
        {
        if (flare [i]->ttl <= 0)
            {
            break;
            }
        }

    if (i < maxflare)
        {
        fireFlare2 (flare [i]);
        flares --;
        fireflarettl = 8 * timestep;
        for (i2 = 0; i2 < maxmissile; i2 ++)
            {
            if (missile [i2]->ttl > 0)
                {
                if (missile [i2]->id >= MISSILE_AIR2 && missile [i2]->id <= MISSILE_AIR3) // only heat seeking missiles
                    if (missile [i2]->target == this)   // only change target if angle is good
                        {
                        bool hit = false;
                        if (myrandom ((int) (fabs (elevatoreffect) * 90 + 20)) > 50)
                            {
                            hit = true;
                            }
                        if (hit)
                            {
                            if (DebugLevel == LOG_ALL)
                                {
                                display ("AIObj::fireFlare() Missile to flare", LOG_ALL);
                                }
                            missile [i2]->target = flare [i];
                            }
                        }
                }
            }

        return true;
        }

    return false;
    }

bool AIObj::fireChaff (DynamicObj **chaff, AIObj **missile)
    {
    int i, i2;

    if (chaffs <= 0)
        {
        return false;
        }

    if (firechaffttl > 0)
        {
        return false;
        }

    for (i = 0; i < maxchaff; i ++)
        {
        if (chaff [i]->ttl <= 0)
            {
            break;
            }
        }

    if (i < maxchaff)
        {
        fireChaff2 (chaff [i]);
        chaffs --;
        firechaffttl = 8 * timestep;

        for (i2 = 0; i2 < maxmissile; i2 ++)
            {
            if (missile [i2]->ttl > 0)
                {
                if (missile [i2]->id > MISSILE_AIR3) // only radar seeking missiles
                    if (missile [i2]->target == this)   // only change target if angle is good
                        {
                        bool hit = false;
                        if (myrandom ((int) (fabs (elevatoreffect) * 90 + 20)) > 50)
                            {
                            hit = true;
                            }
                        if (hit)
                            {
                            if (DebugLevel == LOG_ALL)
                                {
                                display ("AIObj::fireChaff() Missile to chaff", LOG_ALL);
                                }

                            missile [i2]->target = chaff [i];
                            }
                        }
                }
            }

        return true;
        }

    return false;
    }

bool AIObj::fireMissileAir (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }

    if (target->id >= MOVING_GROUND)
        {
        return false;
        }

    if (haveMissile (MISSILE_AIR3))
        {
        return fireMissile (MISSILE_AIR3, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_AIR2))
        {
        return fireMissile (MISSILE_AIR2, missile, (AIObj *) target);
        }
    else if (haveMissile (BOMB01))
        {
        return fireMissile (BOMB01, missile, (AIObj *) target);
        }

    return false;
    }

bool AIObj::selectMissileAir (AIObj **missile)
    {
    bool sel = false;

    if (haveMissile (MISSILE_AIR3))
        {
        missiletype = MISSILE_AIR3 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_AIR2))
        {
        missiletype = MISSILE_AIR2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (BOMB01))
        {
        missiletype = BOMB01 - MISSILE1;
        sel = true;
        }

    return sel;
    }

bool AIObj::fireMissileAirFF (AIObj **missile, AIObj *target)
    {
    if (ttf > 0)
        {
        return false;
        }

    if (target->id >= MOVING_GROUND)
        {
        return false;
        }

    if (haveMissile (MISSILE_FF2))
        {
        return fireMissile (MISSILE_FF2, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_FF1))
        {
        return fireMissile (MISSILE_FF1, missile, (AIObj *) target);
        }

    return false;
    }

bool AIObj::selectMissileAirFF (AIObj **missile)
    {
    bool sel = false;

    if (haveMissile (MISSILE_FF2))
        {
        missiletype = MISSILE_FF2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_FF1))
        {
        missiletype = MISSILE_FF1 - MISSILE1;
        sel = true;
        }

    return sel;
    }

bool AIObj::fireMissileGround (AIObj **missile)
    {
    if (ttf > 0)
        {
        return false;
        }

    if (target->id < MOVING_GROUND)
        {
        return false;
        }

    if (haveMissile (MISSILE_GROUND2))
        {
        return fireMissile (MISSILE_GROUND2, missile, (AIObj *) target);
        }
    else if (haveMissile (MISSILE_GROUND1))
        {
        return fireMissile (MISSILE_GROUND1, missile, (AIObj *) target);
        }

    return false;
    }

bool AIObj::selectMissileGround (AIObj **missile)
    {
    bool sel = false;

    if (haveMissile (MISSILE_GROUND2))
        {
        missiletype = MISSILE_GROUND2 - MISSILE1;
        sel = true;
        }
    else if (haveMissile (MISSILE_GROUND1))
        {
        missiletype = MISSILE_GROUND1 - MISSILE1;
        sel = true;
        }

    return sel;
    }

void AIObj::targetNearestGroundEnemy (AIObj **f)
    {
    int i;
    float d = 1E12;
    ttf = 50 * timestep;

    for (i = 0; i < maxfighter; i ++)
        {
        if (this != f [i] && party != f [i]->party && f [i]->active)
            {
            float phi = getAngle (f [i]);
            float d2 = distance (f [i]) * (60 + fabs (phi)); // prefer enemies in front

            if (bomber)
                if (f [i]->id < MOVING_GROUND)
                    {
                    d2 += 1E10;    // only use this target if no ground targets exist
                    }

            if (d2 < d)
                {
                d = d2;
                target = f [i];
                }
            }
        }

    if (target)
        if (distance (target) > 400)
            {
            target = NULL;
            }
    }

void AIObj::targetNearestEnemy (AIObj **f)
    {
    int i;
    float d = 1E12;
    ttf = 50 * timestep;

    for (i = 0; i < maxfighter; i ++)
        {
        if (this != f [i] && party != f [i]->party && f [i]->active)
            {
            float phi = getAngle (f [i]);
            float d2 = distance (f [i]) * (60 + fabs (phi)); // prefer enemies in front

            if (d2 < d)
                {
                d = d2;
                target = f [i];
                }
            }
        }

    if (!ai && target)
        if (distance (target) > 400)
            {
            target = NULL;
            }
    }

void AIObj::targetLockingEnemy (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;

    if (target == NULL)
        {
        target = f [0];
        }

    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }

    int z = 0;

    do
        {
        i ++;

        if (i >= maxfighter)
            {
            i = 0;
            z ++;
            }
        }
    while ((!f [i]->active || f [i]->party == party || f [i]->target != this || distance (f [i]) > 200) && z <= 1);

    target = f [i];

    if (z > 1 && !ai)
        {
        target = NULL;
        }
    }

void AIObj::targetNext (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;
    if (target == NULL)
        {
        target = f [0];
        }
    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }
    int z = 0;
    do
        {
        i ++;
        if (i >= maxfighter)
            {
            i = 0;
            }
        if (f [i] == this)
            {
            i ++;
            z ++;
            }
        if (i >= maxfighter)
            {
            i = 0;
            }
        }
    while ((!f [i]->active || distance (f [i]) > 11000) && z <= 1);
    target = f [i];
    if (z > 1 && !ai)
        {
        target = NULL;
        }
    }

void AIObj::targetNextEnemy (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;

    if (target == NULL)
        {
        target = f [0];
        }

    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }

    int z = 0;

    do
        {
        i ++;

        if (i >= maxfighter)
            {
            i = 0;
            }

        if (f [i] == this)
            {
            i ++;
            z ++;
            }

        if (i >= maxfighter)
            {
            i = 0;
            }
        }
    while ((!f [i]->active || distance (f [i]) > 11000 || party == f [i]->party) && z <= 1);

    target = f [i];

    if (z > 1 && !ai)
        {
        target = NULL;
        }
    }

void AIObj::targetPrevious (AIObj **f)
    {
    int i;
    ttf = 50 * timestep;
    if (target == NULL)
        {
        target = f [0];
        }
    for (i = 0; i < maxfighter; i ++)
        if (target == f [i])
            {
            break;
            }
    int z = 0;
    do
        {
        i --;
        if (i < 0)
            {
            i = maxfighter - 1;
            }
        if (f [i] == this)
            {
            i --;
            z ++;
            }
        if (i < 0)
            {
            i = maxfighter - 1;
            }
        }
    while ((!f [i]->active || distance (f [i]) > 11000) && z <= 1);
    target = f [i];
    if (z > 1 && !ai)
        {
        target = NULL;
        }
    }

// core AI method
void AIObj::aiAction (Uint32 dt, AIObj **f, AIObj **m, DynamicObj **c, DynamicObj **flare, DynamicObj **chaff)
    {
    int i;
    // which height???
    float recheight2; // this is the height the object wants to achieve
    timer += dt;

    if (!active && !draw)   // not active, not drawn, then exit
        {
        return;
        }
    if (firecannonttl > 0)
        {
        firecannonttl -= dt;    // time to fire the next cannon
        }

    if (firemissilettl > 0)
        {
        firemissilettl -= dt;    // time to fire the next missile
        }

    if (fireflarettl > 0)
        {
        fireflarettl -= dt;    // time to fire the next flare
        }

    if (firechaffttl > 0)
        {
        firechaffttl -= dt;    // time to fire the next chaff
        }

    if (smokettl > 0)
        {
        smokettl -= dt;        // time to activate next smoke element
        }

    // move object according to our physics
    move (dt);
    float timefac = (float) dt / (float) timestep;

    if (id >= STATIC_PASSIVE)   // no AI for static ground objects (buildings)
        {
        return;
        }

    if (haveMissile())
       {
       if ((missiletype == BOMB01 - MISSILE1) || (missiletype == MISSILE_DF1 - MISSILE1))
          {
          ttf = 0;
          }
       }
    // set smoke for objects in motion:
    if ((id >= MISSILE1 && id < MISSILE_MINE1) || (id >= FIGHTER1 && id <= FIGHTER2))   // missile or fighter
        {
        float sz = COS(gamma) * COS(phi) * zoom * 1.1; // polar (spherical) coordinates
        float sy = -SIN(gamma) * zoom * 1.1;
        float sx = COS(gamma) * SIN(phi) * zoom * 1.1;
        // some smoke elements per discrete movement
        float fg = sqrt (forcex * forcex + forcey * forcey + forcez * forcez) * 13;
        if (fg >= MAXSMOKEELEM)
            {
            fg = (float) MAXSMOKEELEM - 0.5;
            }
        for (i = 0; i < (int) fg; i ++)
            {
            float fac = (float) i / fg;
            smoke->setSmoke (tl->x - sx - forcex * fac, tl->y - sy - forcey * fac, tl->z - sz - forcez * fac, (int) phi, 39 - i);
            }
        smoke->move (dt, (int) fg + 1);
        smokettl += timestep;
        }
    if (!active)   // not active, then exit
        {
        return;
        }
    if (explode > 0 || sink > 0)   // exploding or sinking, then exit
        {
        thrust = 0;
        return;
        }
    if (NetworkMissionIsActiveWithDataFlow)
        {
        return;
        }

    // do expensive calculations only once
    float myheight = l->getExactHeight (tl->x, tl->z);
    float targetheight = tl->y;

    if (target != NULL)
        {
        targetheight = l->getExactHeight (target->tl->x, target->tl->z);
        }

    if (target != NULL)
        {
        disttarget = distance (target);    // distance to target
        }
    else
        {
        disttarget = 1;
        }

    // get a new target if necessary
    if (id >= MISSILE1 && id <= MISSILE2)
        {
        if (target == NULL)
            {
            ttl = 0;
            }
        else if (!target->active)
            {
            ttl = 0;
            }
        }

    if (target == NULL)
        {
        if (bomber)
            {
            targetNearestGroundEnemy (f);
            }
        else
            {
            targetNearestEnemy (f);
            }
        }

    if (target != NULL)
        if (!target->active)
            {
            if (bomber)
                {
                targetNearestGroundEnemy (f);
                }
            else
                {
                targetNearestEnemy (f);
                }
            }

    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        if (haveMissile () && target != NULL)
            {

            float dgamma = atan ((target->tl->y - tl->y) / disttarget) * 180 / PI - (gamma - 180);
            float dphi = getAngle (target);

            if ((missiletype == BOMB01 - MISSILE1) || (missiletype == MISSILE_DF1 - MISSILE1))
                {
                ttf = 0;
                }
            else if (fabs (dphi) < 50 && fabs (dgamma) < 50 && party != target->party)
                {
                if (disttarget < 75)
                    {
                    if (ttf > 0)
                        {
                        if (missiletype >= 0 && missiletype <= 2)
                            {
                            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                                {
                                float dphi = fabs (phi - target->phi);

                                if (dphi > 270)
                                    {
                                    dphi = 360 - dphi;
                                    }

                                if (dphi < 45)
                                    {
                                    ttf -= 2 * dt;
                                    }
                                else
                                    {
                                    ttf = 50 * timestep;
                                    }
                                }
                            }
                        else if (missiletype == 6 || missiletype == 7)
                            {
                            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                                {
                                ttf -= 2 * dt;
                                }
                            }
                        else
                            {
                            if (target->id > FIGHTER2)
                                {
                                ttf -= 2 * dt;
                                }
                            }
                        }
                    }
                }
            else
                {
                ttf = 50 * timestep;
                }
            }
        }

    if (!ai)
        {
        return;
        }

    if (
        NetworkOpponent &&
           (
            MissionNumber == MISSION_HEADTOHEAD00    ||
            MissionNumber == MISSION_NETWORKBATTLE01 ||
            MissionNumber == MISSION_NETWORKBATTLE02 ||
            MissionNumber == MISSION_NETWORKBATTLE03
            )
        )
           {
           manoeverstate = 100;
           if (MissionNumber == MISSION_HEADTOHEAD00)
              {
              ThreeDObjects[MissionHeadToHead00State]->realspeed = InPacket.UdpObjSpeed;
              ThreeDObjects[MissionHeadToHead00State]->elevatoreffect = InPacket.UdpObjElevator;
              ThreeDObjects[MissionHeadToHead00State]->rolleffect = InPacket.UdpObjAileron;
              ThreeDObjects[MissionHeadToHead00State]->ruddereffect = InPacket.UdpObjRudder;
              ThreeDObjects[MissionHeadToHead00State]->thrust = InPacket.UdpObjThrust;
              }
           }

    int lsdist = 15;

    float flyx = tl->x + forcex * lsdist, flyz = tl->z + forcez * lsdist;
    int flyxs = l->getCoord ((int) flyx), flyzs = l->getCoord ((int) flyz);
    if (manoeverheight > 0 & manoeverstate !=100)
      {

      recheight2 = l->getExactHeight (flyx, flyz) + recheight;
      }
    else
      {
      // missiles and non intelligent objects will not change their height due to the surface
      if (
            (
            id >= MISSILE1 &&
            id <= MISSILE2 &&
            target != NULL
            )
          ||
            (
            tl->y - myheight > 8 &&
            target != NULL &&
            tl->y - myheight < 50
            )
          )
          {
          recheight2 = target->tl->y - 8 * target->thrust * SIN(target->gamma);
          }
      else if (manoeverstate!=100)
          {

          float flyx2 = tl->x + forcex * lsdist * 3, flyz2 = tl->z + forcez * lsdist * 3;
          float flyx3 = tl->x + forcex * lsdist * 8, flyz3 = tl->z + forcez * lsdist * 8;
          float h1 = l->getMaxHeight (flyx, flyz);
          float h2 = l->getMaxHeight (flyx2, flyz2);
          float h3 = l->getMaxHeight (flyx3, flyz3);
          h1 = h1 > h2 ? h1 : h2;
          h1 = h1 > h3 ? h1 : h3;
          recheight2 = recheight + h1;
          }
      }
    // fire flares and chaff
    if (id >= FIGHTER1 && id <= FIGHTER2 && manoeverstate!=100)   // for fighters do the following
        {
        if (manoevertheta <= 0)
            for (i = 0; i < maxmissile; i ++)
                if (m [i]->ttl > 0)
                    if (m [i]->target == this)
                        {
                        if (m [i]->id >= 0 && m [i]->id <= MISSILE_AIR3)
                            {
                            if (fplayer->elevatoreffect >= 0.5)
                                {
                                fireFlare (flare, m);
                                fireflarettl += intelligence / 20 * timestep;
                                }
                            manoevertheta = 35 * timestep;
                            }
                        else
                            {
                            if (fplayer->elevatoreffect >= 0.5)
                                {
                                fireChaff (chaff, m);
                                firechaffttl += intelligence / 20 * timestep;
                                }
                            manoevertheta = 35 * timestep;
                            }
                        }
        }

    // manoevers (may use the height information)
    if (manoeverstate && active && draw)
        {
        if (manoeverstate == 1)   // Immelmann
            {
            recelevatoreffect = 0.05;
            if (fabs (theta) > 10)
                {
                recrolleffect = -1;
                }
            else
                {
                recrolleffect = 0;
                manoeverstate = 2;
                }
            }
        else if (manoeverstate == 2)
            {
            if (fabs (theta) <= 150)
                {
                recrolleffect = 0;
                recelevatoreffect = 1;
                }
            else
                {
                manoeverstate = 3;
                }
            }
        else if (manoeverstate == 3)
            {
            if (gamma < 170 || gamma > 190)
                {
                recrolleffect = 0;
                recelevatoreffect = 1;
                }
            else
                {
                manoeverstate = 4;
                }
            }
        else if (manoeverstate == 4)
            {
            if (fabs (theta) > 20)
                {
                recrolleffect = 1;
                recelevatoreffect = 0.05;
                }
            else
                {
                manoeverstate = 0;
                }
            }

        if (manoeverstate == 10)   // climb vertical
            {
            recrolleffect = 0;
            recelevatoreffect = 1;

            if (gamma > 260 || gamma < 90)
                {
                recrolleffect = 0;
                recelevatoreffect = 0;
                manoeverstate = 11;
                }
            }
        else if (manoeverstate == 11)
            {
            if (fabs (tl->y - myheight) > 3)
                {
                manoeverstate = 12;
                }
            }
        else if (manoeverstate == 12)
            {
            recelevatoreffect = -0.5;

            if (gamma > 170 && gamma < 190)
                {
                recelevatoreffect = 0;
                manoeverstate = 0;
                }
            }

        if (manoeverstate == 20)   // Roll
            {
            recelevatoreffect = 0.55;
            recrolleffect = 1;
            if (theta > 80 && theta < 90)
                {
                manoeverstate = 21;
                }
            }
        else if (manoeverstate == 21)
            {
            if (theta > -10 && theta < 10)
                {
                manoeverstate = 0;
                }
            }

        float pulljoystick = 0.005;
        float nocorrection = 0.1;

        if ( manoeverstate != 100)
           {
           if (recrolleffect > rolleffect + nocorrection)
             {
             rolleffect += pulljoystick * timestep;
             }
           else if (recrolleffect < rolleffect - nocorrection)
             {
             rolleffect -= pulljoystick * timestep;
             }

           if (recelevatoreffect > elevatoreffect + nocorrection)
               {
               elevatoreffect += pulljoystick * timestep;
               }
           else if (recelevatoreffect < elevatoreffect - nocorrection)
               {
               elevatoreffect -= pulljoystick * timestep;
               }
           }
        return;
        }

    if ( manoeverstate != 100)
    {
       // calculate the recommended height, recheight2 depends on it
       if (manoeverheight > 0)
           {
           manoeverheight -= dt;
           }

       if (manoeverheight <= 0)
           {
           if (!(id >= FIGHTER1 && id <= FIGHTER2) && target != NULL)   // no fighter, has target (missile, mine)
               {
               recheight = target->tl->y - targetheight;
               }
           else if (id == BOMBER_B17)     // transporters have to stay higher
               {
               recheight = 40;
               manoeverheight = 1;
               }
           else if (id >= FIGHTER1 && id <= FIGHTER2 && target != NULL)     // fighter, has target
               {
               if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                   {
                   recheight = target->tl->y - targetheight;    // target is a fighter
                   }
               else
                   {
                   recheight = target->tl->y - targetheight + 5;    // target is no fighter
                   }

               if (!l->isWater (l->f [flyxs] [flyzs]))   // not flying above water
                   {
                   if (recheight < 3.5 + 0.01 * aggressivity)
                       {
                       recheight = 3.5 + 0.01 * aggressivity;
                       }
                   }

               float minh = 5.5 + 0.01 * aggressivity; // minimum height

               if (l->type == LAND_CANYON)
                   {
                   minh = 6.5 + 0.01 * aggressivity;    // stay higher in canyons
                   }

               if (fabs (tl->y - myheight) < minh)
                   {
                   recheight = 9 + 0.015 * aggressivity;

                   if (fabs (tl->y - myheight) < minh * 0.3)
                       {
                       manoeverstate = 10;
                       display ("Manoever: Vertical climb", LOG_ALL);
                       }
                   else
                       {
                       manoeverheight = 5 * timestep; // fly manoever to gain height
                       }
                   }

               if (disttarget < 50 && fabs (tl->y - myheight) > 25)
                   {
                   recheight = 8 + 0.025 * aggressivity;
                   manoeverheight = 12 * timestep;
                   }
               }
           }
        }

    if (ttl <= 0 && id >= MISSILE1 && id <= MISSILE2 && id != MISSILE_MINE1)
        {
        recheight = -100;
        recheight2 = -100;
        recgamma = 90;
        }
    else if (ai)
        {
        if (target != NULL && ((id >= MISSILE1 && id <= MISSILE2) || (id >= FIGHTER1 && id <= FIGHTER2 && manoeverheight <= 0)))   // is AGM
            {
            float dgamma = 0;

            if (disttarget <= -0.00001 || disttarget >= 0.00001)   // no division by zero
                {
                dgamma = atan ((target->tl->y - tl->y) / disttarget) * 180 / PI - (gamma - 180);
                }

            recgamma = gamma + dgamma; // get recommended elevation to target
            }
        else
            {
            recgamma = (int) ((recheight2 - tl->y) * 10 - gamma + 360);
            }
        }

    // do a smooth roll
    float deltatheta;
    deltatheta = rectheta - theta;
    if (fabs (dtheta) > 30)
        {
        dtheta = 0;
        }
    float myRollRate = fabs (deltatheta) / 5.0F * RollRate;
    if (myRollRate > RollRate)
        {
        myRollRate = RollRate;
        }
    float RollRate2 = myRollRate;
    if (RollRate2 >= -0.00001 && RollRate2 <= 0.00001)
        {
        RollRate2 = 0.00001;
        }
    if (deltatheta > 0 && dtheta < 0)
        {
        dtheta += myRollRate * timefac;
        }
    else if (deltatheta < 0 && dtheta > 0)
        {
        dtheta -= myRollRate * timefac;
        }
    else if (deltatheta > 0)
        {
        float estimatedtheta = dtheta * (dtheta + RollRate2 * 5 / timefac) / 2 / RollRate2;
        if (deltatheta > estimatedtheta)
            {
            dtheta += myRollRate * timefac;
            }
        else if (deltatheta < estimatedtheta)
            {
            dtheta -= myRollRate * timefac;
            }
        }
    else
        {
        float estimatedtheta = -dtheta * (dtheta - RollRate2 * 5 / timefac) / 2 / RollRate2;
        if (deltatheta < estimatedtheta)
            {
            dtheta -= myRollRate * timefac;
            }
        else if (deltatheta > estimatedtheta)
            {
            dtheta += myRollRate * timefac;
            }
        }
    if (dtheta > (RollRate * (1.0 + realspeed)) * timefac * 5.0F)
        {
        dtheta = (RollRate * (1.0 + realspeed)) * timefac * 5.0F;
        }
    theta += dtheta;
    // height changes
    float RollRate1 = RollRate / 5;
    if (RollRate1 >= -0.00001 && RollRate1 <= 0.00001)
        {
        RollRate1 = 0.00001;
        }
    if (theta > maxtheta)
        {
        theta = maxtheta;    // restrict roll angle
        }
    else if (theta < -maxtheta)
        {
        theta = -maxtheta;
        }
    float deltagamma = recgamma - gamma;
    if (deltagamma > 0 && dgamma < 0)
        {
        dgamma += RollRate1 * timefac;
        }
    else if (deltagamma < 0 && dgamma > 0)
        {
        dgamma -= RollRate1 * timefac;
        }
    else if (deltagamma > 0)
        {
        float estimatedgamma = dgamma * (dgamma + RollRate1 * 2) / RollRate1;
        if (id == 200)
            {
            id = id;
            }
        if (deltagamma > estimatedgamma + 2)
            {
            dgamma += RollRate1 * timefac;
            }
        else if (deltagamma < estimatedgamma - 2)
            {
            dgamma -= RollRate1 * timefac;
            }
        }
    else if (deltagamma < 0)
        {
        float estimatedgamma = -dgamma * (dgamma + RollRate1 * 2) / RollRate1;
        if (id == 200)
            {
            id = id;
            }
        if (deltagamma < estimatedgamma - 2)
            {
            dgamma -= RollRate1 * timefac;
            }
        else if (deltagamma > estimatedgamma + 2)
            {
            dgamma += RollRate1 * timefac;
            }
        }
    if (dgamma > manoeverability * (3.33 + 15.0 * realspeed) * timefac)
        {
        dgamma = manoeverability * (3.33 + 15.0 * realspeed) * timefac;
        }
    gamma += dgamma;
    if (gamma > 180 + maxgamma)
        {
        gamma = 180 + maxgamma;
        }
    else if (gamma < 180 - maxgamma)
        {
        gamma = 180 - maxgamma;
        }

    if (id >= MISSILE1 && id <= MISSILE2)
        {
        if (target == NULL)
            {
            ttl = 0;
            return;
            }
        else if (target->active == false)
            {
            ttl = 0;
            return;
            }
        }

    if (target == NULL)
        {
        return;
        }

    // fighter's targeting mechanism for missiles
    if (id >= FIGHTER1 && id <= FIGHTER2)   // for fighters do the following
        {
        if (ai)
            {
            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                {
                if (!selectMissileAirFF (m))
                    {
                    selectMissileAir (m);
                    }
                }
            else
                {
                selectMissileGround (m);
                }
            }
        }

    if (!ai || target == NULL)
        {
        return;
        }

    int firerate;
    firerate = 3;
    float dx2, dz2, ex, ez;
    float dx = target->tl->x - tl->x, dz = target->tl->z - tl->z; // current distances
    if ((id >= FIGHTER1 && id <= FIGHTER2) || (id >= MISSILE1 && id <= MISSILE2) || (id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
        {
        float t = 10.0 * disttarget; // generous time to new position
        if (t > 60)
            {
            t = 60;    // higher values will not make sense
            }
        t *= (float) (400 - precision) / 400;
        int tt = (int) target->theta;
        if (tt < 0)
            {
            tt += 360;
            }
        float newphi = t * SIN(tt) * 5.0 * target->manoeverability; // new angle of target after time t
        if (newphi > 90)
            {
            newphi = 90;
            }
        else if (newphi < -90)
            {
            newphi = -90;
            }
        newphi += (float) target->phi;
        if (newphi >= 360)
            {
            newphi -= 360;
            }

        if (newphi < 0)
            {
            newphi += 360;
            }

        if ((id >= FIGHTER1 && id <= FIGHTER2) || (id >= FLAK1 && id <= FLAK2) || (id >= TANK1 && id <= TANK2))
            {
            ex = target->tl->x - SIN(newphi) * t * target->realspeed * 0.25; // estimated target position x
            ez = target->tl->z - COS(newphi) * t * target->realspeed * 0.25; // estimated target position z
            }
        else
            {
            ex = target->tl->x - SIN(newphi) * t * target->realspeed * 0.05; // estimated target position x
            ez = target->tl->z - COS(newphi) * t * target->realspeed * 0.05; // estimated target position z
            }

        dx2 = ex - tl->x;
        dz2 = ez - tl->z; // estimated distances
        }
    else
        {
        dx2 = dx;
        dz2 = dz;
        }

    float a, w = phi;

    if (dz2 > -0.0001 && dz2 < 0.0001)
        {
        dz2 = 0.0001;
        }

    // get heading to target
    a = atan (dx2 / dz2) * 180 / PI;
    if (dz2 > 0)
        {
        if (dx2 > 0)
            {
            a -= 180.0F;
            }
        else
            {
            a += 180.0F;
            }
        }
//    this->aw = a;
    aw = a - w; // aw=0: target in front, aw=+/-180: target at back
    if (aw < -180)
        {
        aw += 360;
        }
    if (aw > 180)
        {
        aw -= 360;
        }
    if (manoevertheta > 0)
        {
        manoevertheta -= dt;
        }
    if (manoeverthrust > 0)
        {
        manoeverthrust -= dt;
        }

    // heading calculations

    if (id >= FIGHTER1 && id < FIGHTER2)   // for aircraft do the following
        {
        if (!acttype && disttarget <= 1000 && manoevertheta <= 0)   // no special action, near distance, no roll manoever
            {
            if (aw > 0)   // positive angle
                {
                if (aw > 140 && disttarget > 50)
                    {
                    manoeverstate = 1;// Immelman
                    }
                else if (aw > 160.0F + 0.05 * intelligence && disttarget < 4 + 0.01 * intelligence)     // target very near at the back
                    { // Immelman
                    manoeverstate = 1;
                    }
                else if (aw > 160 && disttarget < 25)     // target is at the back
                    {
                    if (fabs (tl->y - myheight) > 7 && gamma >= 175 + intelligence / 100)   // high enough over ground
                        { // roll
                        manoeverstate = 20;
                        }
                    else
                        {
                        rectheta = -90;
                        if (manoevertheta <= 0)
                            { // Turn
                            manoevertheta = timestep * (100 + myrandom ((400 - intelligence) / 8)); // turn hard left or right
                            }
                        if (manoeverthrust <= 0)
                            {
                            recthrust = maxthrust / (1.05F + (float) intelligence * 0.0015);    // fly faster
                            }
                        if (intelligence < 280 && manoeverheight <= 0)
                            { // Height change
                            recheight = 5;
                            manoeverheight = timestep * (20 - intelligence / 50);
                            } // stay low
                        }
                    }
                else if (aw < 40 && disttarget > 60)
                    {
                    rectheta = 0;
                    }
                else if (aw < 20 && disttarget > 30)
                    {
                    rectheta = 0;
                    }
                else     // otherwise fly to target direction
                    {
                    int maw = aw > 90 ? 90 : (int) aw;
                    int maw2 = 90 - maw;
                    rectheta = 90 - maw2 * intelligence / 400;
                    if (maw < 30)
                        {
                        rectheta /= 2;
                        }
                    if (maw < 5)
                        {
                        rectheta = 0;
                        if (target->id >= FIGHTER1 && target->id <= FIGHTER2 && disttarget < 20)
                            {
                            ((AIObj *) target)->manoevertheta = timestep * (50 - intelligence / 10);
                            }
                        }
                    }
                }
            else     // same for negative angle
                {
                if (aw < -140 && disttarget > 50)
                    { // Immelmann
                    manoeverstate = 1;
                    }
                else if (aw < -160.0F - 0.05 * intelligence && disttarget < 4 + 0.01 * intelligence)     // target very near at the back
                    { // Immelmann
                    manoeverstate = 1;
                    }
                else if (aw < -160 && disttarget < 25)
                    {
                    if (fabs (tl->y - myheight) > 7 && gamma >= 175 + intelligence / 100)   // high enough over ground
                        { // roll
                        manoeverstate = 20; // roll
                        }
                    else
                        {
                        rectheta = 90;
                        if (manoevertheta <= 0)
                            { // Turn
                            manoevertheta = timestep * (100 + myrandom ((400 - intelligence) / 8));
                            }
                        if (manoeverthrust <= 0)
                            {
                            recthrust = maxthrust / (1.05F + (float) intelligence * 0.0015);
                            }
                        if (intelligence < 280 && manoeverheight <= 0)
                            { // Height change
                            recheight = 5;
                            manoeverheight = timestep * (20 - intelligence / 50);
                            }
                        }
                    }
                else if (aw > -40 && disttarget > 60)
                    {
                    rectheta = 0;
                    }
                else if (aw > -20 && disttarget > 30)
                    {
                    rectheta = 0;
                    }
                else
                    {
                    int maw = aw < -90 ? -90 : (int) aw;
                    int maw2 = -90 - maw;
                    rectheta = -90 - maw2 * intelligence / 400;
                    if (maw > -30)
                        {
                        rectheta /= 2;
                        }
                    if (maw > -5)
                        {
                        rectheta = 0;
                        if (target->id >= FIGHTER1 && target->id <= FIGHTER2 && disttarget < 20)
                            {
                            ((AIObj *) target)->manoevertheta = timestep * (50 - intelligence / 10);
                            }
                        }
                    }
                }
            }
        }
    else if (id >= MISSILE1 && id <= MISSILE2)     // for missiles do the following
        {
        if (fabs (aw) < 50 && disttarget > 50)   // target in front and minimum distance, then no roll
            {
            rectheta = 0;
            }
        else     // otherwise chase target
            {
            if (aw < -90 || aw > 90)
                {
                rectheta = 0;
                }
            else if (aw > 0)
                {
                rectheta = aw > 90 ? 90 : aw;
                }
            else
                {
                rectheta = aw < -90 ? -90 : aw;
                }
            }
        }
    else if (id >= FLAK1 && id <= FLAK2)     // ground-air-cannon
        {
        recthrust = 0;
        thrust = 0;
        if (aw > 5)
            {
            rectheta = maxtheta;
            }
        else if (aw < -5)
            {
            rectheta = -maxtheta;
            }
        else
            {
            rectheta = 0;
            }
        }
    else if (id >= TANK1 && id <= TANK2)     // tanks
        {
        recthrust = maxthrust;
        thrust = maxthrust; // always at maximum thrust
        if (aw > 5)
            {
            rectheta = maxtheta;
            }
        else if (aw < -5)
            {
            rectheta = -maxtheta;
            }
        else
            {
            rectheta = 0;
            }
        if (firecannonttl <= 0)
            {
            if (id == TANK_AIR1)
                if (fabs (rectheta - theta) < 2 && fabs (aw) < 20 && disttarget < 40 && target->tl->y > tl->y + 2)
                    {
                    fireCannon (c);
                    firecannonttl += firerate * timestep;
                    }
            if (id == TANK_GROUND1)
                if (fabs (rectheta - theta) < 2 && fabs (aw) < 20 && disttarget < 35 && target->tl->y <= tl->y + 1 && target->tl->y >= tl->y - 1)
                    {
                    fireCannon (c);
                    firecannonttl += firerate * timestep;
                    }
            }
        }

    // thrust and manoever calculations
    if (id >= FIGHTER1 && id <= FIGHTER2)   // fighters
        {
        if (disttarget > 5 + aggressivity / 12)   // 2.5 seems to be best, but fighters become far too strong
            {
            if (disttarget < 50 && fabs (aw) > 30 && manoeverthrust <= 0)
                {
                recthrust = maxthrust / (1.0F + (float) intelligence * 0.0025);
                }
            else
                {
                thrustUp ();    // otherwise fly faster
                }
            }
        else if (manoeverthrust <= 0)
            {
            if (recthrust > target->thrust)   // adopt thrust of target
                {
                thrustDown ();
                }
            else
                {
                thrustUp ();
                }
            }
        if (disttarget > 50 && fabs (aw) < 20)   // high distance and target in front, then fly straight
            {
            rectheta = 0;
            }
        if (manoeverthrust <= 0)
            if (disttarget < 25 && fabs (aw) > 160 && target->id >= TANK1)   // avoid collisions
                {
                manoeverthrust = 25 * timestep;
                recthrust = maxthrust;
                manoevertheta = 25 * timestep;
                rectheta = 0;
                manoeverheight = 25 * timestep;
                recheight = 10;
                }

        // fire cannon?
        float agr = 4.0 - (float) aggressivity / 100;
        if (firecannonttl <= 0)
            {
            if (fabs (rectheta - theta) < agr && fabs (aw) < 20 + agr * 4 && disttarget < 30)
                {
                fireCannon (c);
                }
            else if (disttarget < 2 + agr && fabs (aw) < 20 + agr * 4)
                {
                fireCannon (c);
                }
            }

        // fire missile?
        if (firemissilettl <= 0)
            {
            if (target->id >= FIGHTER1 && target->id <= FIGHTER2)
                {
                int z1 = 0;

                if (disttarget < 15 && fabs (aw) < 20)
                    {
                    for (i = 0; i < maxfighter; i ++)
                        {
                        if (target == f [i]->target && party == f [i]->party)
                            {
                            z1 ++;
                            }
                        }
                    if (z1 >= 3)
                        {
                        manoevertheta = 15 * timestep;
                        firemissilettl = 10 * timestep;
                        if (myrandom (2))
                            {
                            rectheta = 90;
                            }
                        else
                            {
                            rectheta = -90;
                            }
                        targetNextEnemy (f);
                        }
                    }
                if (firemissilettl <= 0)
                    {
                    if (fabs (rectheta - theta) < agr / 2 && fabs (aw) < agr && disttarget < 45)
                        {
                        fireMissile (m, (AIObj *) target);
                        firemissilettl += aggressivity * timestep;
                        }
                    }
                }
            else     // ground target
                {
                if (fabs (rectheta - theta) < 5 + agr * 4 && fabs (aw) < 5 + agr * 4 && disttarget < 50)
                    if (!(l->lsticker & 7))
                        {
                        fireMissileGround (m);
                        firemissilettl += aggressivity / 2 * timestep;
                        }
                }
            }
        }

    if ((id >= FLAK1 && id <= FLAK2) || id == SHIP_CRUISER || id == SHIP_DESTROYER1 || id == TANK_TRSAM1)
        {
        if (firecannonttl <= 0)
            for (int i = 0; i < maxfighter; i ++)
                if (f [i]->active)
                    if (party != f [i]->party)
                        {
                        disttarget = distance (f [i]); // distance to target
                        ex = f [i]->tl->x; // estimated target position x
                        ez = f [i]->tl->z; // estimated target position z
                        dx2 = ex - tl->x;
                        dz2 = ez - tl->z; // estimated distances
                        w = (int) phi;

                        if (dz2 > -0.0001 && dz2 < 0.0001)
                            {
                            dz2 = 0.0001;
                            }

                        a = (atan (dx2 / dz2) * 180 / PI);

                        if (dz2 > 0)
                            {
                            if (dx2 > 0)
                                {
                                a -= 180;
                                }
                            else
                                {
                                a += 180;
                                }
                            }

                        aw = a - w;

                        if (aw < -180)
                            {
                            aw += 360;
                            }

                        if (aw > 180)
                            {
                            aw -= 360;
                            }

                        if (id == FLAK_AIR1)
                            if (f [i]->tl->y > tl->y + 2)
                                {
                                if (fabs (aw) <= 20 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                firecannonttl = firerate * timestep;
                                }

                        if (id == SHIP_DESTROYER1)
                            if (f [i]->tl->y > tl->y + 2)
                                {
                                if (aw >= 0 && aw < 40 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                if (aw >= -40 && aw < 0 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                if (aw >= 120 && aw < 160 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                if (aw >= -160 && aw < -120 && disttarget < 50)   // + aggressive
                                    {
                                    fireCannon (c, phi + aw);
                                    }

                                firecannonttl = firerate * timestep;
                                }

                        if (firemissilettl <= 0)
                            if (id == FLARAK_AIR1)
                                if (fabs (aw) < 25 && disttarget < 45) // + aggressive
                                    if (f [i]->tl->y > tl->y + 2)
                                        {
                                        ttf = 0;
                                        fireMissileAirFF (m, f [i]);
                                        firemissilettl += (20 + firerate * 10) * timestep;
                                        }

                        if (id == TANK_TRSAM1)
                            {
                            if (firemissilettl <= 0)
                                if (aw >= -30 && aw < 30 && disttarget < 60)   // + aggressive
                                    {
                                    ttf = 0;
                                    fireMissileAirFF (m, f [i]);
                                    firemissilettl += aggressivity / 5 * timestep;
                                    missiles [6] ++; // unlimited ammo
                                    }
                            }

                        if (id == SHIP_CRUISER)
                            {
                            if (firemissilettl <= 0)
                                if (aw >= -30 && aw < 30 && disttarget < 60)   // + aggressive
                                    {
                                    ttf = 0;
                                    fireMissileAirFF (m, f [i]);
                                    firemissilettl += aggressivity / 5 * timestep;
                                    missiles [6] ++; // unlimited ammo
                                    }

                            if (firecannonttl <= 0)
                                {
                                fireCannon (c, phi + aw);
                                }
                            }
                        }
        }

    if (id >= FIGHTER1 && id <= FIGHTER2)
        {
        if (rectheta > 90 - precision / 5)
            {
            rectheta = 90 - precision / 5;
            }
        else if (rectheta < -90 + precision / 5)
            {
            rectheta = -90 + precision / 5;
            }
        }
    }

#endif
