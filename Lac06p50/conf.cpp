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

/* This file contains all configuration parsing code. */

#ifndef IS_CONF_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "aiobject.h"
#include "conf.h"
#include "common.h"
#include "dirs.h"

// Global Variables:

// global bool variables:
bool mouse_reverse = false;
bool mouse_relative = false;

// global unsigned char variables:
unsigned char mouse_firecannon = MOUSE_BUTTON_LEFT, mouse_firemissile = MOUSE_BUTTON_RIGHT;
unsigned char mouse_selectmissile = MOUSE_BUTTON_MIDDLE;

// global int variables:
int antialiasing = 1;
int controls = CONTROLS_MOUSE;
int difficulty = 2;
int dithering = 1;
int dynamiclighting = 1;
int fullscreen = 0;
int mouse_autorudder = 30;
int NetworkMode = 1; // 0=PeerToPeer, 1=Client/Server
int physics = 1;
int quality = 3;
int specialeffects = 1;
int volumemusic = 100;
int volumesound = 100;
int width = 640, height = 480, bpp = 32;
int wantwidth = 640, wantheight = 480, wantfullscreen = 0;

/* New global Joystick Variables, used in Linux Air Combat */
int joystick_DROPCHAFF;
int joystick_DROPFLARE;
int joystick_FIREPRIMARY;
int joystick_FIRESECONDARY;
int joystick_FlapsDN;
int joystick_FlapsUP;
int joystick_HudLadderBarsOnOff = 0;
int joystick_HudOnOff;
int joystick_IffOnOff = 0;
int joystick_LookBK;
int joystick_LookDN;
int joystick_LookFD;
int joystick_LookLT;
int joystick_LookRT;
int joystick_LookUP;
int joystick_MapScrollEast = 0;
int joystick_MapScrollNorth = 0;
int joystick_MapScrollSouth = 0;
int joystick_MapScrollWest = 0;
int joystick_MapViewOnOff = 0;
int joystick_MapZoomIn = 0;
int joystick_MapZoomOut = 0;
int joystick_RadarOnOff;
int joystick_RadarZoomIn;
int joystick_RadarZoomOut;
int joystick_SpeedBrake;
int joystick_TargetNext;
int joystick_TargetPrev;
int joystick_TargetVocalize;
int joystick_ToggleExtIntView;
int joystick_TrimElevatorDn;
int joystick_TrimElevatorUp;
int joystick_TrimRudderLeft;
int joystick_TrimRudderRight;
int joystick_Undercarriage;
int joystick_WEAPONSELECT;
int joystick_WEPorAfterburner;
int joystick_ZoomFOVin;
int joystick_ZoomFOVout;

/* global Joystick Axis Variables: */
int joystick_aileron = 0;
int joystick_elevator = 1;
int joystick_rudder = 2;
int joystick_throttle = 3;
int joystick_view_x = 4;
int joystick_view_y = 5;

// global unsigned int variables:
unsigned int key_AutoPilot = 275;    
unsigned int key_CommsMacro01 = 49;  
unsigned int key_CommsMacro02 = 50;  
unsigned int key_CommsMacro03 = 51;  
unsigned int key_CommsMacro04 = 52;  
unsigned int key_CommsMacro05 = 53;  
unsigned int key_CommsMacro06 = 54;  
unsigned int key_CommsMacro07 = 55;  
unsigned int key_CommsMacro08 = 56;  
unsigned int key_CommsMacro09 = 57;  
unsigned int key_CommsMacro10 = 48;  
unsigned int key_CommsMacro11 = 538; 
unsigned int key_CommsMacro12 = 539; 
unsigned int key_CommsMacro13 = 540; 
unsigned int key_CommsMacro14 = 541; 
unsigned int key_CommsMacro15 = 542; 
unsigned int key_CommsMacro16 = 543; 
unsigned int key_DROPCHAFF = 537;       
unsigned int key_DROPFLARE = 536;       
unsigned int key_FlapsDn = 68;          
unsigned int key_FlapsUp = 85;          
unsigned int key_GunCameraToggle = 67;  
unsigned int key_HudLadderBarsOnOff = 0;
unsigned int key_HudOnOff = 69;
unsigned int key_IffOnOff = 0;
unsigned int key_LookBK = 2;            // defaults to num2
unsigned int key_LookDN = 11;           // defaults to numSlash
unsigned int key_LookFD = 8;            // defaults to num8
unsigned int key_LookLT = 4;            // defaults to num4
unsigned int key_LookRT = 6;            // defaults to num6
unsigned int key_LookUP = 5;            // defaults to num5
unsigned int key_MapScrollEast = 0;
unsigned int key_MapScrollNorth = 0;
unsigned int key_MapScrollSouth = 0;
unsigned int key_MapScrollWest = 0;
unsigned int key_MapViewOnOff = 0;
unsigned int key_MapZoomIn = 0;
unsigned int key_MapZoomOut = 0;
unsigned int key_MumblePtt = 538;       // Should match PTT key used by optional "Mumble" companion voice app.
unsigned int key_MumbleWhisper = 539;   // Should match key used by optional "Mumble" companion voice app for "Broadcast Whisper".
unsigned int key_PRIMARY = 32;          // space
unsigned int key_RadarOnOff = 82;       // defaults to "r"
unsigned int key_RadarZoomIn  = 14;     
unsigned int key_RadarZoomOut = 13;     
unsigned int key_SECONDARY = 8;         
unsigned int key_SpeedBrake = 66;       
unsigned int key_TARGETNEXT = 540;      
unsigned int key_TARGETPREVIOUS = 541;  
unsigned int key_TARGETVOCALIZE = 542;  
unsigned int key_THRUSTDOWN = 91;       
unsigned int key_THRUSTUP = 93;         
unsigned int key_ToggleViewIntExt = 538;
unsigned int key_TrimElevatorDn = 530;  
unsigned int key_TrimElevatorUp = 529;  
unsigned int key_TrimRudderLeft = 532;  
unsigned int key_TrimRudderRight = 531; 
unsigned int key_Undercarriage = 71;    
unsigned int key_WEAPONSELECT = 539;    
unsigned int key_WepAfterburner = 80;   
unsigned int key_ZoomFovIn = 534;       
unsigned int key_ZoomFovOut = 535;      
unsigned int mouse_sensitivity = 100;

// global float variables:
float view = 50.0;

/* External Variables: */

// external unsigned chars:
extern unsigned char DefaultAircraft;
extern unsigned char MyNetworkId;
extern unsigned char Realm;
extern unsigned char ServerIpByte1of4;
extern unsigned char ServerIpByte2of4;
extern unsigned char ServerIpByte3of4;
extern unsigned char ServerIpByte4of4;

// external Single-dimensional char arrays:
extern char CommunityHandle[20];
extern char CommsMacro01String1[64];
extern char CommsMacro01String2[64];
extern char CommsMacro02String1[64];
extern char CommsMacro02String2[64];
extern char CommsMacro03String1[64];
extern char CommsMacro03String2[64];
extern char CommsMacro04String1[64];
extern char CommsMacro04String2[64];
extern char CommsMacro05String1[64];
extern char CommsMacro05String2[64];
extern char CommsMacro06String1[64];
extern char CommsMacro06String2[64];
extern char CommsMacro07String1[64];
extern char CommsMacro07String2[64];
extern char CommsMacro08String1[64];
extern char CommsMacro08String2[64];
extern char CommsMacro09String1[64];
extern char CommsMacro09String2[64];
extern char CommsMacro10String1[64];
extern char CommsMacro10String2[64];
extern char CommsMacro11String1[64];
extern char CommsMacro11String2[64];
extern char CommsMacro12String1[64];
extern char CommsMacro12String2[64];
extern char CommsMacro13String1[64];
extern char CommsMacro13String2[64];
extern char CommsMacro14String1[64];
extern char CommsMacro14String2[64];
extern char CommsMacro15String1[64];
extern char CommsMacro15String2[64];
extern char CommsMacro16String1[64];
extern char CommsMacro16String2[64];
extern char DebugBuf[100];
extern char NetworkPassword [32]; 
extern char RemoteDnsIpAddress[];
extern char RemoteDnsIpAddressHost[32];
extern char RemoteDnsIpAddressSecondLevelDomain[32];
extern char RemoteDnsIpAddressTopLevelDomain[32];

// external Two-dimensional char arrays:
extern char PlayerIdStrings[10][9];

//external integers:
extern int MyInPortNumber; // Network API default value 51100 (Unsigned).
extern int NetworkTransmitTimerInterval;
extern int PeerInPortNumber; // Network API default value 51100 (Unsigned).

/* Function Prototypes: */

void StringToUpperCase(char *StringPointer);

/* Functions and Methods: */

ConfigFile::ConfigFile () {}

ConfigFile::ConfigFile (char *fname)
    {
    char buf2 [STDSIZE];
    bool commentmode = false;
    FILE *in;
    length = 0;
    in = fopen (fname, "rb");

    if (in != NULL)
        {
        length = fread (buf, 1, 32000, in);
        fclose (in);
        }
    else
        {
        sprintf (buf2, "Could not load %s", fname);
        display (buf2, LOG_MOST);
        buf [0] = 0;
        }
    for (int i = 0; i < length; i ++)
        {
        if (buf [i] == '#')
            {
            commentmode = true;
            }

        if (buf [i] == '\n')
            {
            commentmode = false;
            }

        if (commentmode)
            {
            buf [i] = ' ';
            }
        else
            {
            buf [i] = tolower (buf [i]);
            }
        }
    } 

char *ConfigFile::skipwhite (char *str)
    {
    while (*str == ' ' || *str == '\t')
        {
        str ++;
        }

    return str;
    } 

char *ConfigFile::skipnum (char *str)
    {
    while (*str >= '0' && *str <= '9')
        {
        str ++;
        }

    return str;
    } 

char *ConfigFile::skipalphanum (char *str)
    {
    while ((*str >= '0' && *str <= 'z') || (*str == '-'))
        {
        str ++;
        }
    return str;
    } 

char *ConfigFile::getString (char *dest, char *str)
    {
    
    char *strf, *stre;
    char cmpstr [256];
    int i;

    if (strlen (str) >= 256)
        { 
        return NULL;
        }
    
    strncpy (cmpstr, str, 256);
    if (buf [0] == 0)
        { 
        return NULL;
        }
    
    for (i = 0; i < (int) strlen (cmpstr); i ++)
        {
        cmpstr [i] = tolower (cmpstr [i]);
        }
    
    strf = strstr (buf, cmpstr);
    if (strf == NULL)
        { 
        return NULL;
        }
    
    strf += strlen (cmpstr); 
    
    strf = skipwhite (strf);

    if (*strf == 0)
        {  
        return NULL;
        }
    
    strf ++; 
    if (*strf == 0)
        { 
        return NULL;
        }
    
    strf = skipwhite (strf); 
    if (*strf == 0)
        { 
        return NULL;
        }
    
    stre = skipalphanum (strf); 
    
    if (stre - strf >= 256)
        { 
        return NULL; 
        }
    if (*strf == 0)
        { 
        return NULL; 
        }
    
    for (i = 0; i < (int) (stre - strf); i ++)
        {
        dest [i] = *(strf+i);
        }
    dest [i] = 0; 
    return dest;
    } 

int ConfigFile::getValue (char *str)
    {
    char res [256];
    getString (res, str);

    if (res == NULL)
        {
        return -1;
        }

    return atoi (res);
    } 

int ConfigFile::openOutput (char *fname)
    {
    out = fopen (fname, "wb");

    if (out == NULL)
        {
        return 0;
        }
    return 1;
    } 

int ConfigFile::write (char *str1, int n)
    {
    char str [256];

    if (strlen (str1) + 8 > 256)
        {
        return 0;
        }
    sprintf (str, "%s = %d\n", str1, n);
    fwrite (str, 1, strlen (str), out);
    return 1;
    } 

int ConfigFile::write (char *str1, char c)
    {
    char str [256];

    if (c <= 32 || c >= 97)
        {
        write (str1, (int) c);
        return 1;
        }
    if (strlen (str1) + 8 > 256)
        {
        return 0;
        }
    sprintf (str, "%s = %c\n", str1, c);
    fwrite (str, 1, strlen (str), out);
    return 1;
    }  

void ConfigFile::writeText (char *str)
    {
    fwrite (str, 1, strlen (str), out);
    fwrite ("\n", 1, 1, out);
    } 

void ConfigFile::close ()
    {
    fclose (out);
    } 

void save_config ()
    {
    char buf [STDSIZE];
    ConfigFile *cf = new ConfigFile ();
    char *confname = dirs->getSaves ("LacConfig.txt");
    sprintf (buf, "Saving %s ", confname);
    display (buf, LOG_MOST);
    int ret1 = cf->openOutput (confname);

    if (ret1 == 0)
        {
        display ("Could not save configuration", LOG_ERROR);
        fflush (stderr);
        return;
        }
    cf->writeText ("# This is the configuration file for LINUX AIR COMBAT.\n\n");
    cf->writeText ("# This file should be named LacConfig.txt and it should be stored");
    cf->writeText ("# in a hidden folder named .LAC beneath your home folder.\n");
    cf->writeText ("# For example, if your LINUX username is fred and your home");
    cf->writeText ("# folder is at /home/fred, then this file should be found at:\n");
    cf->writeText ("# /home/fred/.LAC/LacConfig.txt\n");
    cf->writeText ("# This file contains two different kinds of text. Firstly, notice");
    cf->writeText ("# that many lines (like this one) commence with a pound sign. Any");
    cf->writeText ("# line that commences with a pound sign is ignored by LINUX AIR COMBAT.");
    cf->writeText ("# All lines of that type are known as Comment Lines.\n");
    cf->writeText ("# Comment lines are displayed for your convenience in understanding the");
    cf->writeText ("# purpose of the OTHER lines, which do NOT commence with a pound sign.\n");
    cf->writeText ("# Secondly, other lines all contain configuration details such as the name");
    cf->writeText ("# of a configurable item, followed on the same line or on the next line");
    cf->writeText ("# with an associated number or text value (usually separated by an = sign)");
    cf->writeText ("# that LINUX AIR COMBAT needs to know about in order to function well on");
    cf->writeText ("# your computer, using your display and your controls, in your network.\n");
    cf->writeText ("# All lines of that type are known as Configuration Lines.\n");
    cf->writeText ("# First we will specify the resolution of the LAC display window in two");
    cf->writeText ("# Configuration Lines below. The first of these two lines will specify the");
    cf->writeText ("# width of LAC's display in pixels. The second of these two lines will specify");
    cf->writeText ("# the height of LAC's display in pixels. If you have already tried running");
    cf->writeText ("# LINUX AIR COMBAT at least once, then it has already tried to fill in those");
    cf->writeText ("# lines for you with its best guess, but you can use any simple text editor");
    cf->writeText ("# to change the values, and sometimes you can make things look a lot better");
    cf->writeText ("# by specifying values that exactly match the maximum display resolution of");
    cf->writeText ("# your own display harware, because sometimes the automated guessing isn't");
    cf->writeText ("# perfect, especially if you have a high resolution display. Here are some");
    cf->writeText ("# very popular video monitor width x height values for fullscreen mode:");
    cf->writeText ("#  640x480, 800x600, 1024x768, 1152x864, 1280x768, 1280x960, 1280x1024, 1920x1080\n");
    cf->writeText ("# Accordingly, just below this block of Comment Lines you will find two");
    cf->writeText ("# lines that DON'T commence with pound signs, and that you can use to configure");
    cf->writeText ("# LINUX AIR COMBAT for optimal use of your display. For example, if you have a");
    cf->writeText ("# modern, powerful, gaming-class computer with a High Definition display");
    cf->writeText ("# and native resolution that's 1920 pixels wide and 1080 pixels high, then");
    cf->writeText ("# LINUX AIR COMBAT will look its very best if those lines look like this,");
    cf->writeText ("# (without the # sign):");
    cf->writeText ("# width = 1920");
    cf->writeText ("# height = 1080\n");
    cf->write     (" width", wantwidth);
    cf->write     (" height", wantheight);
    cf->writeText ("");
    cf->writeText ("# Now we specify color depth, designated bpp or bits per pixel. Possible values");
    cf->writeText ("# are 8, 16, 24, or 32. If you are unsure, choose 32:\n");
    cf->write     (" bpp", bpp);
    cf->writeText ("");
    cf->writeText ("# LINUX AIR COMBAT can run in full-screen mode or in a rectangular window that won't");
    cf->writeText ("# completely fill your display. Many people prefer full-screen mode, but sometimes");
    cf->writeText ("# this may be incompatible with other work you need to do or it may not perform");
    cf->writeText ("# well on your hardware. Furthermore, if you are using the recommended MUMBLE voice");
    cf->writeText ("# communication system to speak with other LAC players, you will find some advantages");
    cf->writeText ("# to configuring a display window that is slightly smaller than full-screen. You can");
    cf->writeText ("# configure LINUX AIR COMBAT for full-screen mode by setting the following value to");
    cf->writeText ("# 1. If you set it to 0 you will get a window sized as specified by the width and");
    cf->writeText ("# height configuration from above, which (in that case) should always be less than");
    cf->writeText ("# your full-screen resolution. Configure either 0 or 1 here:\n");
    cf->write     (" fullscreen", wantfullscreen);
    cf->writeText ("");
    cf->writeText ("# LINUX AIR COMBAT can be configured to run at any of 6 different levels of video");
    cf->writeText ("# detail, ranging from 0 for the simplest graphics up to 5 for the most complex.");
    cf->writeText ("# A modern, powerful gaming computer with hardware-accelerated graphics may");
    cf->writeText ("# be able to deliver smooth performance at level 5, but an older, less powerful one");
    cf->writeText ("# might best be configured for Level 0 in order to achieve smooth, stable flight.");
    cf->writeText ("# Furthermore, we have found that 32-bit LINUX systems tend to need less RAM and to");
    cf->writeText ("# provide more stable graphics libraries than 64-bit LINUX systems at this time.");
    cf->writeText ("# Accordingly, when using higher levels of visual detail on a 64-bit LINUX system,");
    cf->writeText ("# you may see odd visual anomalies (especially on systems with little RAM) that");
    cf->writeText ("# do not trouble users of 32-bit LINUX systems. If this happens to you, consider");
    cf->writeText ("# setting the following value at a low level (0, 1, or 2) so your system will begin");
    cf->writeText ("# each session without those display anomalies. We have found that (on most 64-bit");
    cf->writeText ("# systems) once a mission has started in this way, the graphic detail can then be");
    cf->writeText ("# increased (using the in-game setup menus while in the missions) without triggering");
    cf->writeText ("# these anomalies.");
    cf->writeText ("");
    cf->writeText ("# The first time you run LINUX AIR COMBAT it will automatically test your system and");
    cf->writeText ("# try to optimally configure this value for you, but you can change it below if you");
    cf->writeText ("# want to experiment. Choose a number between 0 and 5:");
    cf->writeText ("");
    cf->write     (" quality", quality);
    cf->writeText ("");
    cf->writeText ("# The next five Configuration Lines can help you further optimize the visual quality");
    cf->writeText ("# of LINUX AIR COMBAT on your hardware. Experimenting with these may be worthwhile:");
    cf->writeText ("");
    cf->writeText ("# Fog distance miles (lower values may help slower computers perform better): 20 to 200 (default=130)");
    cf->write     (" view", (int) view);
    cf->writeText ("# Dithering: 0=off, 1=on (may help make displayed graphics look better)");
    cf->write     (" dithering", dithering);
    cf->writeText ("# Antialiasing: 0=off, 1=on (may help displayed text look better)");
    cf->write     (" antialiasing", antialiasing);
    cf->writeText ("# Special effects: 0=off, 1=on (may help water surfaces, reflections, etc. look better,");
    cf->writeText ("# and may help avoid visual anomalies in 64-bit LINUX systems.)");
    cf->write     (" specialeffects", specialeffects);
    cf->writeText ("# Dynamic lighting: 0=off, 1=on (may help explosions look better)");
    cf->write     (" dynamiclighting", dynamiclighting);
    cf->writeText ("");
    cf->writeText ("# Adjust the next two Configuration Lines according to your preference for the volume level of sound effects");
    cf->writeText ("# and music/narration within missions. (100 = full volume, 0 = silent.)");
    cf->writeText ("");
    cf->write     (" sound", (int) volumesound);
    cf->write     (" music", (int) volumemusic);
    cf->writeText ("");
    cf->writeText ("# If you have a joystick, you will want to use it as your primary control when flying aircraft within");
    cf->writeText ("# LINUX AIR COMBAT. To do that, you will enter 2 on the next Configuration Line. However, if you");
    cf->writeText ("# don't have a joystick you can try using a mouse pointer instead, by entering 1 below:");
    cf->writeText ("");
    cf->write     (" controls", controls);
    cf->writeText ("");
    cf->writeText ("# Difficulty level: 2 = REALISTIC. No other levels are now supported.");
    cf->writeText ("");
    cf->write     (" difficulty", difficulty);
    cf->writeText ("");
    cf->writeText ("# Brightness: -50..50 per cent (default=0)");
    cf->writeText ("");
    cf->write     (" brightness", brightness);
    cf->writeText ("");
    cf->writeText ("# Simulator Precision: 1 = SIMULATOR. No other levels are now supported.");
    cf->writeText ("");
    cf->write     (" physics", physics);
    cf->writeText ("");
    cf->writeText ("# If you are going to use LINUX AIR COMBAT on a network, you must configure the IP address");
    cf->writeText ("# of a Linux Air Combat Server or of one other computer also using LINUX AIR COMBAT. To do");
    cf->writeText ("# this using a numeric IP address like 192.168.1.2, you must enter four numbers into the");
    cf->writeText ("# next four Configuration Lines. For Example, if you want your computer to connect with");
    cf->writeText ("# another that is using IP address 192.168.1.2, you would enter 192 on the first of the");
    cf->writeText ("# next four lines, after ServerIpByte1of4 =.  Then you would enter 168 on the second of ");
    cf->writeText ("# those lines, 1 on the third line, and 2 at the end of the fourth line, after it says");
    cf->writeText ("# ServerIpByte4of4.");
    cf->writeText ("");
    cf->write     (" ServerIpByte1of4", ServerIpByte1of4 );
    cf->write     (" ServerIpByte2of4", ServerIpByte2of4 );
    cf->write     (" ServerIpByte3of4", ServerIpByte3of4 );
    cf->write     (" ServerIpByte4of4", ServerIpByte4of4 );
    cf->writeText ("");
    cf->writeText ("# As an alternative to entering a numeric IP address above, you might be able to use a");
    cf->writeText ("# DNS name instead. To do that, use the next 3 Configuration Lines to specify the");
    cf->writeText ("# remote network peer DNS Address, expressed as three separate words. For example,");
    cf->writeText ("# If you know that your peer has configured his computer behind a NAT Router and");
    cf->writeText ("# has set up port forwarding and dynamic DNS according to industry norms and if his");
    cf->writeText ("# DNS address is bigshot33.my-ip.biz then you would enter bigshot33 in the first");
    cf->writeText ("# Configuration Line below, you would enter my-ip in the second line below, and you");
    cf->writeText ("# enter biz in the third line below. If your network peer does not use DNS then you");
    cf->writeText ("# should enter void in all three lines below.");
    cf->writeText ("");
    cf->writeText ("# First of 3 sections of network peer DNS name (defaults to our server at LacServer2) :");
    cf->writeText (" RemoteDnsIpAddressHost");
    cf->writeText (&RemoteDnsIpAddressHost[0]);
    cf->writeText ("# Second of 3 sections of network peer DNS name (defaults to our server at LinuxAirCombat) :");
    cf->writeText (" RemoteDnsIpAddressSecondLevelDomain");
    cf->writeText (&RemoteDnsIpAddressSecondLevelDomain[0]);
    cf->writeText ("# Third of 3 sections of network peer DNS name (defaults to our server at com) :");
    cf->writeText (" RemoteDnsIpAddressTopLevelDomain");
    cf->writeText (&RemoteDnsIpAddressTopLevelDomain[0]);
    cf->writeText ("");
    cf->writeText ("# If you are going to use LINUX AIR COMBAT on a network, you must also configure it for");
    cf->writeText ("# an available UDP Port (both for sending to a corresponding UDP port on the addressed");
    cf->writeText ("# remote peer computer and for listening for incoming UDP messages on your own computer).");
    cf->writeText ("# As a general rule, you can just leave this at the default value of 51100 in both of");
    cf->writeText ("# the following Configuration Lines:");
    cf->writeText ("");
    cf->write     (" PeerInPortNumber", PeerInPortNumber);
    cf->write     (" MyInPortNumber", MyInPortNumber);
    cf->writeText ("");
    cf->writeText ("# The following field is used to assert your preferred, default identity in Multiplayer");
    cf->writeText ("# missions. Within each mission, every player must have a unique number. LAC will try to");
    cf->writeText ("# use the number you specify here, but if somebody else is already using that number in a");
    cf->writeText ("# network mission, LAC will choose a different number for you, so the number you choose");
    cf->writeText ("# doesn't matter very much unless you arrange private missions with closed membership.");
    cf->writeText ("# Generally, we recommend starting with this value at 0 and letting LAC choose a new");
    cf->writeText ("# value for you, renegotiating among other players when necessary. Note that today's");
    cf->writeText ("# missions all demand a value between 1 and 10. Odd numbers put you on the RED team.");
    cf->writeText ("# Even numbers put you on the BLUE team.");
    cf->writeText ("");
    cf->write     (" MyNetworkId", MyNetworkId);
    cf->writeText ("");
    cf->writeText ("# All network players wishing to see one another within any online mission are now");
    cf->writeText ("# required to configure the same NetworkPassword in the Configuration Line below.");
    cf->writeText ("# If all other network configuration details are properly set up but you cannot see");
    cf->writeText ("# your network opponent or opponents, you might want to review this setting to confirm");
    cf->writeText ("# that it matches precisely for all players:");
    cf->writeText ("");
    cf->writeText (" NetworkPassword");
    cf->writeText (&NetworkPassword[0]);
    cf->writeText ("");
    cf->writeText ("# Network transmit timer interval in milliseconds (between 100 and 500).");
    cf->writeText ("# Adjust this as necessary for the smoothest flight on your network. If all computers");
    cf->writeText ("# are on the same high-speed LAN then values of 100 to 200 work well.  Across the");
    cf->writeText ("# worldwide Internet, values between 200 and 500 may be better. All players who");
    cf->writeText ("# wish to interact with one another within the same Realm and mission should use");
    cf->writeText ("# identical values for NetworkTransmitTimerInterval. The default value is 200.");
    cf->writeText ("");
    cf->write     (" NetworkTransmitTimerInterval", NetworkTransmitTimerInterval );
    cf->writeText ("");
    cf->writeText ("# NetworkMode. On a network, LAC can operate in either of two distinct modes.");
    cf->writeText ("# The simplest of these is PeerToPeer mode. In PeerToPeer mode, pairs of");
    cf->writeText ("# players connect with one another directly, with no need for a network server.");
    cf->writeText ("# PeerToPeer mode is limited to two-player missions.");
    cf->writeText ("# ");
    cf->writeText ("# The other mode is known as ClientServer mode. In ClientServer mode, all players");
    cf->writeText ("# must connect with a remote LAC Server, and several players can participate in");
    cf->writeText ("# any multiplayer mission. The number of players in each mission is limited only");
    cf->writeText ("# by the design of the mission itself and the quality of the network connections");
    cf->writeText ("# in use.");
    cf->writeText ("# ");
    cf->writeText ("# Set NetworkMode as follows:");
    cf->writeText ("# ");
    cf->writeText ("# For PeerToPeer mode, set NetworkMode to 0.");
    cf->writeText ("# For ClientServer mode, set NetworkMode to 1. This is the default.");
    cf->writeText ("");
    cf->write     (" NetworkMode", NetworkMode);
    cf->writeText ("");
    cf->writeText ("# ");
    cf->writeText ("# Realm. As Linux Air Combat becomes more popular, users within the community will");
    cf->writeText ("# be able to divide into different Realms. In the future, this will help LAC");
    cf->writeText ("# servers to manage network traffic by ensuring that players participating in a");
    cf->writeText ("# different Realm won't be clogging your Internet connection with their packets.");
    cf->writeText ("# For now, everybody should just leave Realm set to 0 so we can all interact with");
    cf->writeText ("# each other.");
    cf->writeText ("# ");
    cf->writeText ("");
    cf->write     (" Realm", Realm);
    cf->writeText ("");
    cf->writeText ("# CommunityHandle. This is your online name among the LAC community. When other");
    cf->writeText ("# online players use their RADAR target select commands to keep track of you,");
    cf->writeText ("# they will see the name you issue to yourself here. Choose a name that will");
    cf->writeText ("# represent you. It will also help you claim your guncamera film files from");
    cf->writeText ("# the LAC Server. It's best if the first 9 characters of this name match your");
    cf->writeText ("# Mumble Username, since the LAC community will use both. You can enter up to 16");
    cf->writeText ("# characters, but only the first 9 characters are ever seen by others. Use the");
    cf->writeText ("# remaining characters to ensure that the entire name is unique just to you. Also,");
    cf->writeText ("# you can use the reverse apostrophe character to fill any of those 16 characters");
    cf->writeText ("# but it will be displayed as a blank space when other LAC players see it. So a");
    cf->writeText ("# good, 16-character CommunityHandle for a person named Thomas, born in the USA in");
    cf->writeText ("# 1972 might be: THOMAS```1972USA");
    cf->writeText ("");
    cf->writeText (" CommunityHandle");
    cf->writeText (&CommunityHandle[0]);
    cf->writeText ("");
    cf->writeText ("");
    cf->writeText ("# DefaultAircraft. All missions will place the player in this aircraft by default.");
    cf->writeText ("# Choose your preferred aircraft by entering the three-digit number from the");
    cf->writeText ("# following list.");
    cf->writeText ("# ");
    cf->writeText ("# Acceptable values at this time are:");
    cf->writeText ("# ");
    cf->writeText ("# Whimsical      jet   Hawk:            201");
    cf->writeText ("# Fiat           G55   Centauro:        202");
    cf->writeText ("# Messerschmidt  ME109 Bf109            203");
    cf->writeText ("# Curtis         P40   Warhawk          204");
    cf->writeText ("# Hawker               Hurricane        205");
    cf->writeText ("# Nakajima       KI43  Oscar            206");
    cf->writeText ("# Supermarine:         Spitfire         207");
    cf->writeText ("# Polykarpov     I-16  Horsefly:        208");
    cf->writeText ("# Junkers        JU87  Stuka:           209");
    cf->writeText ("# Mitsubishi     A6M2   Zero:           210");
    cf->writeText ("# Chance Vought  F4U   Corsair:         211");
    cf->writeText ("# Grumman        F6F   Hellcat:         212");
    cf->writeText ("# Lockheed       P38   Lightning:       213");
    cf->writeText ("# Republic       P47   Thunderbolt:     214");
    cf->writeText ("# North American P51   Mustang:         215");
    cf->writeText ("# Boeing         B17   Flying Fortress: 216");
    cf->writeText ("# Focke-Wulf     FW190 Butcherbird:     217");
    cf->writeText ("# Yakolev              YAK9:            218");
    cf->writeText ("# Nakajima       N1K1  Shiden:          219");
    cf->writeText ("# Consolidated   B24   Liberator:       220");
    cf->writeText ("# Bell           P39   Airacobra:       221");
    cf->writeText ("# Mitsubishi     G4M   Betty:           222");
    cf->writeText ("# North American B25   Mitchell:        223");
    cf->writeText ("# Martin         B26   Marauder:        224");
    cf->writeText ("# Grumman        F4F   Wildcat:         225");
    cf->writeText ("# Lavochkin      LA5   Fantail:         226");
    cf->writeText ("# Lavochkin      LA7   Fin:             227");
    cf->writeText ("# Ilyushin       IL2   Sturmovik:       228");
    cf->writeText ("# Machi          C.202 Folgore:         229");
    cf->writeText ("# Avro                 Lancaster:       230");
    cf->writeText ("# De Haviland    DH.98 MosquitoB:       231");
    cf->writeText ("# Hawker               Typhoon:         232");
    cf->writeText ("# Yakovlev             Yak1:            233");
    cf->writeText ("# Boeing         B29   Superfortress:   234");
    cf->writeText ("# Dewoitine            D.320:           235");
    cf->writeText ("# Curtiss        SB2C  Helldiver:       236");
    cf->writeText ("# Grumman        TBF   Avenger:         237");
    cf->writeText ("# Messerschmidt  ME163 Komet:           238");
    cf->writeText ("# Hawker               Tempest:         239");
    cf->writeText ("# Aichi          D3A   Val:             240");
    cf->writeText ("# Nakajima       B5N   Kate:            241");
    cf->writeText ("# Douglas        SBD5  Dauntless:       242");
    cf->writeText ("# Messerschmidt  ME110 Zerstorer:       243");
    cf->writeText ("# Dornier        DO17:                  244");
    cf->writeText ("# Heinkel        HE111:                 245");
    cf->writeText ("# Junkers        JU88:                  246");
    cf->writeText ("# Nakajima       KI84  Hayate:          247");
    cf->writeText ("# Kawasaki       KI61  Hien:            248");
    cf->writeText ("# Generic        Fighter Generic01:     249");
    cf->writeText ("# Mitsubishi     A6M5  Zero:            250");
    cf->writeText ("# Supermarine    MK5   Spitfire:        251");
    cf->writeText ("# North American P51B  Mustang:         252");
    cf->writeText ("# Republic       P47B  Thunderbolt:     253");
    cf->writeText ("# Messerschmidt  ME109F:                254");
    cf->writeText ("# Lockheed       P38F  Lightning:       255");
    cf->writeText ("");
    cf->write     (" DefaultAircraft", DefaultAircraft);
    cf->writeText ("");
    cf->writeText ("");
    cf->writeText ("# The following 16 items define text labels to be displayed on your cockpit console");
    cf->writeText ("# when you press any of the 16 keyboard keys defined in your LacControls.txt file");
    cf->writeText ("# named key_CommsMacro01 through key_CommsMacro16. By default, their values are set up");
    cf->writeText ("# to integrate the recommended, free, open-source VOIP application named Mumble into");
    cf->writeText ("# LAC. Each of these is an upper-case text label that is associated with a keyboard");
    cf->writeText ("# key, and which is displayed on your cockpit when you press the corresponding key.");
    cf->writeText ("# Think of these labels as little reminders to tell you exactly what you intend for");
    cf->writeText ("# some external, co-resident program (not LAC) to accomplish whenever you use the");
    cf->writeText ("# corresponding keyboard key. You can edit the corresponding entries in your");
    cf->writeText ("# LacControls.txt file and create a corresponding set of Mumble shortcuts so that when");
    cf->writeText ("# your Mumble application is connected with our Mumble server at LacServer2.LinuxAirCombat.com, it");
    cf->writeText ("# will instantly perform the communication function described in each of the following");
    cf->writeText ("# 16 phrases. That way, whenever you use those keys with Mumble, LAC can confirm what");
    cf->writeText ("# Mumble will do by displaying the corresponding phrase on your cockpit panel. (Until");
    cf->writeText ("# you install and configure Mumble as described, you won't be able to communicate with");
    cf->writeText ("# other LAC users and these 16 items will be of no use to you.)");
    cf->writeText ("# Because of the way LAC uses these text labels to predict the way your copy of Mumble");
    cf->writeText ("# will respond when you press the corresponding keyboard keys, you probably don't want");
    cf->writeText ("# to change any of these items. If you leave them alone, LAC knows that (at least in");
    cf->writeText ("# the future) you intend to use them with our Mumble server for easy voice comms, and");
    cf->writeText ("# LAC enables some extra cockpit display functions in that case. If you change any of");
    cf->writeText ("# these labels, LAC will skip some of that extra functionality, but you could use the");
    cf->writeText ("# corresponding key with some other external program like TeamSpeak, Ventrillo, etc.");
    cf->writeText ("# with diminished cockpit integration.");
    cf->writeText ("#");
    cf->writeText ("# When editing these 16 items, avoid whitespace and avoid punctuation. Separate distinct");
    cf->writeText ("# words with dashes.");
    cf->writeText ("# ");
    cf->writeText (" CommsMacro01");
    cf->writeText (&CommsMacro01String1[0]);
    cf->writeText (" CommsMacro02");
    cf->writeText (&CommsMacro02String1[0]);
    cf->writeText (" CommsMacro03");
    cf->writeText (&CommsMacro03String1[0]);
    cf->writeText (" CommsMacro04");
    cf->writeText (&CommsMacro04String1[0]);
    cf->writeText (" CommsMacro05");
    cf->writeText (&CommsMacro05String1[0]);
    cf->writeText (" CommsMacro06");
    cf->writeText (&CommsMacro06String1[0]);
    cf->writeText (" CommsMacro07");
    cf->writeText (&CommsMacro07String1[0]);
    cf->writeText (" CommsMacro08");
    cf->writeText (&CommsMacro08String1[0]);
    cf->writeText (" CommsMacro09");
    cf->writeText (&CommsMacro09String1[0]);
    cf->writeText (" CommsMacro10");
    cf->writeText (&CommsMacro10String1[0]);
    cf->writeText (" CommsMacro11");
    cf->writeText (&CommsMacro11String1[0]);
    cf->writeText (" CommsMacro12");
    cf->writeText (&CommsMacro12String1[0]);
    cf->writeText (" CommsMacro13");
    cf->writeText (&CommsMacro13String1[0]);
    cf->writeText (" CommsMacro14");
    cf->writeText (&CommsMacro14String1[0]);
    cf->writeText (" CommsMacro15");
    cf->writeText (&CommsMacro15String1[0]);
    cf->writeText (" CommsMacro16");
    cf->writeText (&CommsMacro16String1[0]);
    cf->writeText ("# ");

    cf->writeText ("");
    cf->writeText ("");
    cf->writeText ("# The following 10 items define text labels representing friendly names or handles");
    cf->writeText ("# for the people that fly with you in multiplayer missions. Each is associated with");
    cf->writeText ("# an integer value between 01 and 10 because multiplayer missions can have as many");
    cf->writeText ("# as 10 players. As an example of using these items, suppose you know that one of");
    cf->writeText ("# the people that will join your next mission is named FRED and that he has");
    cf->writeText ("# configured his LacConfig.txt file with 2 in his MyNetworkID field. You would then");
    cf->writeText ("# write FRED in the second value below. That will cause your copy of LAC to identify");
    cf->writeText ("# any player joining any online, multiplayer mission as player #2 as FRED on your");
    cf->writeText ("# cockpit instrument panel.");
    cf->writeText ("#");
    cf->writeText ("# Note that this system is only reliable if you are running private missions with");
    cf->writeText ("# closed membership and if you can reliably predict the numeric value each mission");
    cf->writeText ("# player configures into his own MyNetworkId field in his own copy of this");
    cf->writeText ("# LacConfig.txt file. If you allow strangers into your missions, they are likely");
    cf->writeText ("# to be mis-identified if you customize any of these ten labels. If you don't plan");
    cf->writeText ("# to manage private missions with closed membership, you should not change these.");
    cf->writeText ("# ");
    cf->writeText ("# Each of these 10 items must consist of a single, upper-case word, with no more than");
    cf->writeText ("# 8 characters.");
    cf->writeText ("#");
    cf->writeText (" PlayerIdString01");
    cf->writeText (&PlayerIdStrings[0][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString02");
    cf->writeText (&PlayerIdStrings[1][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString03");
    cf->writeText (&PlayerIdStrings[2][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString04");
    cf->writeText (&PlayerIdStrings[3][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString05");
    cf->writeText (&PlayerIdStrings[4][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString06");
    cf->writeText (&PlayerIdStrings[5][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString07");
    cf->writeText (&PlayerIdStrings[6][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString08");
    cf->writeText (&PlayerIdStrings[7][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString09");
    cf->writeText (&PlayerIdStrings[8][0]);
    cf->writeText ("#");
    cf->writeText (" PlayerIdString10");
    cf->writeText (&PlayerIdStrings[9][0]);
    cf->writeText ("#");
    cf->writeText ("# This file will self-generate if it is missing, and it is meant to give sensible  ");
    cf->writeText ("# startup settings.");
    cf->writeText ("# ");
    cf->writeText ("# To get back to default settings, just delete this file.");
    cf->writeText ("# ");
    cf->writeText ("# Online help is available at AskMisterWizard.com/FlightSimMovies/LAC");
    cf->close ();
    delete cf;
    } 

int load_config ()
    {
    char buf [STDSIZE];
    char ret [256];
    char *str;
    char *confname = dirs->getSaves ("LacConfig.txt");
    int DnsNameSize = 0;
    sprintf (buf, "Loading %s ", confname);
    display (buf, LOG_MOST);
    ConfigFile *cf = new ConfigFile (confname);
    if (cf->buf [0] == 0)   // no file found
        {
        delete cf;
        return 0;
        }
    str = cf->getString (ret, "width");
    if (str == NULL)
        {
        width = 800;
        }
    else
        {
        width = atoi (str);
        }
    if (width < 100)
        {
        width = 100;
        }
    else if (width > 3000)
        {
        width = 1024;
        }
    wantwidth = width;
    str = cf->getString (ret, "height");
    if (str == NULL)
        {
        height = 600;
        }
    else
        {
        height = atoi (str);
        }
    if (height < 100)
        {
        height = 100;
        }
    else if (height > 2000)
        {
        height = 768;
        }
    wantheight = height;
    str = cf->getString (ret, "bpp");
    if (str == NULL)
        {
        bpp = 32;
        }
    else
        {
        bpp = atoi (str);
        }
    if (bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32)
        {
        bpp = 32;
        }
    str = cf->getString (ret, "fullscreen");
    if (str == NULL)
        {
        fullscreen = 1;
        }
    else
        {
        fullscreen = atoi (str);
        }
    if (fullscreen)
        {
        fullscreen = 1;
        }
    wantfullscreen = fullscreen;
    str = cf->getString (ret, "quality");
    if (str == NULL)
        {
        quality = 2;
        }
    else
        {
        quality = atoi (str);
        }
    if (quality < 0)
        {
        quality = 0;
        }
    else if (quality > 5)
        {
        quality = 5;
        }
    str = cf->getString (ret, "view");
    if (str == NULL)
        {
        view = 50;
        }
    else
        {
        view = atoi (str);
        }
    if (view < VIEW_MIN)
        {
        view = VIEW_MIN;
        }
    else if (view > VIEW_MAX)
        {
        view = VIEW_MAX;
        }
    str = cf->getString (ret, "dithering");
    dithering = (str == NULL) ? 1 : atoi (str);
    if (dithering)
        {
        dithering = 1;
        }
    str = cf->getString (ret, "antialiasing");
    antialiasing = (str == NULL) ? 1 : atoi (str);
    if (antialiasing)
        {
        antialiasing = 1;
        }
    str = cf->getString (ret, "specialeffects");
    specialeffects = (str == NULL) ? 1 : atoi (str);
    if (specialeffects)
        {
        specialeffects = 1;
        }
    str = cf->getString (ret, "dynamiclighting");
    dynamiclighting = (str == NULL) ? 1 : atoi (str);
    if (dynamiclighting)
        {
        dynamiclighting = 1;
        }
    str = cf->getString (ret, "sound");
    if (str == NULL)
        {
        volumesound = 100;
        }
    else
        {
        volumesound = atoi (str);
        }
    if (volumesound < 0)
        {
        volumesound = 0;
        }
    else if (volumesound > 100)
        {
        volumesound = 100;
        }
    str = cf->getString (ret, "music");
    if (str == NULL)
        {
        volumemusic = 100;
        }
    else
        {
        volumemusic = atoi (str);
        }
    if (volumemusic < 0)
        {
        volumemusic = 0;
        }
    else if (volumemusic > 100)
        {
        volumemusic = 100;
        }
    str = cf->getString (ret, "controls");
    if (str == NULL)
        {
        controls = CONTROLS_MOUSE;
        }
    else
        {
        controls = atoi (str);
        }
    if (controls < 0)
        {
        controls = 0;
        }
    else if (controls > 2)
        {
        controls = 0;
        }
    str = cf->getString (ret, "difficulty");
    if (str == NULL)
        {
        difficulty = 2;
        }
    else
        {
        difficulty = atoi (str);
        }
    if (difficulty < 1)
        {
        difficulty = 2;
        }
    else if (difficulty > 2)
        {
        difficulty = 2;
        }
    str = cf->getString (ret, "brightness");
    if (str == NULL)
        {
        brightness = 0;
        }
    else
        {
        brightness = atoi (str);
        }
    if (brightness < -50)
        {
        brightness = -50;
        }
    else if (brightness > 50)
        {
        brightness = 50;
        }
    str = cf->getString (ret, "physics");
    if (str == NULL)
        {
        physics = 1;
        }
    else
        { 
        physics = 1;
        }
    str = cf->getString (ret, "ServerIpByte1of4");
    if (str == NULL)
        {
        ServerIpByte1of4 = 0;
        }
    else
        {
        ServerIpByte1of4 = atoi (str);
        }
    str = cf->getString (ret, "ServerIpByte2of4");
    if (str == NULL)
        {
        ServerIpByte2of4 = 0;
        }
    else
        {
        ServerIpByte2of4 = atoi (str);
        }
    str = cf->getString (ret, "ServerIpByte3of4");
    if (str == NULL)
        {
        ServerIpByte3of4 = 0;
        }
    else
        {
        ServerIpByte3of4 = atoi (str);
        }
    str = cf->getString (ret, "ServerIpByte4of4");
    if (str == NULL)
        {
        ServerIpByte4of4 = 0;
        }
    else
        {
        ServerIpByte4of4 = atoi (str);
        }
    str = cf->getString (ret, "RemoteDnsIpAddressHost");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find RemoteDnsIpAddressHost in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&RemoteDnsIpAddressHost[0], "LacServer2.", 32);
        }
    else
        {
        display ("load_config() Found RemoteDnsIpAddressHost in LacConfig.txt. str=", LOG_MOST);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&RemoteDnsIpAddressHost[0], str, 32); 
        DnsNameSize = strlen(RemoteDnsIpAddressHost);
        }
    str = cf->getString (ret, "RemoteDnsIpAddressSecondLevelDomain");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find RemoteDnsIpAddressSecondLevelDomain in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&RemoteDnsIpAddressSecondLevelDomain[0], "LinuxAirCombat.", 32);
        }
    else
        {
        display ("load_config() Found RemoteDnsIpAddressSecondLevelDomain in LacConfig.txt. str=", LOG_MOST);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&RemoteDnsIpAddressSecondLevelDomain[0], str, 32); 
        DnsNameSize = strlen(RemoteDnsIpAddressSecondLevelDomain);
        }
    str = cf->getString (ret, "RemoteDnsIpAddressTopLevelDomain");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find RemoteDnsIpAddressTopLevelDomain in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&RemoteDnsIpAddressTopLevelDomain[0], "com.", 5); 
        }
    else
        {
        display ("load_config() Found RemoteDnsIpAddressTopLevelDomain in LacConfig.txt. str=", LOG_MOST);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&RemoteDnsIpAddressTopLevelDomain[0], str, 32); 
        DnsNameSize = strlen(RemoteDnsIpAddressTopLevelDomain);
        }
    
    sprintf (RemoteDnsIpAddress, "%s.%s.%s", RemoteDnsIpAddressHost, RemoteDnsIpAddressSecondLevelDomain, RemoteDnsIpAddressTopLevelDomain);
    str = cf->getString (ret, "PeerInPortNumber");
    if (str == NULL)
        {
        PeerInPortNumber = 0;
        }
    else
        {
        PeerInPortNumber = atoi (str);
        }
    str = cf->getString (ret, "MyInPortNumber");
    if (str == NULL)
        { 
        MyInPortNumber = 51100; 
        }
    else
        { 
        MyInPortNumber = atoi (str);
        }
    str = cf->getString (ret, "NetworkTransmitTimerInterval");
    if (str == NULL)
        { 
        NetworkTransmitTimerInterval = 300; 
        }
    else
        { 
        NetworkTransmitTimerInterval = atoi (str);
        if (NetworkTransmitTimerInterval < 100)
           { 
           NetworkTransmitTimerInterval = 100;  
           }
        else if (NetworkTransmitTimerInterval > 4000)
           { 
           NetworkTransmitTimerInterval = 4000; 
           }
        }
    str = cf->getString (ret, "MyNetworkId");
    if (str == NULL)
        {
        MyNetworkId = 1;
        }
    else
        {
        MyNetworkId = atoi (str);
        }
    str = cf->getString (ret, "NetworkPassword");
    if (str == NULL)
        { 
        display ("load_config() Could NOT find NetworkPassword within LacConfig.txt file.", LOG_MOST);
        strncpy(&NetworkPassword[0], "password", 32); // Default to "password"
        }
    else
        { 
        display ("load_config() Found NetworkPassword within LacConfig.txt file.", LOG_MOST);
        strncpy (&NetworkPassword[0], str, 32); 
        }
    display ("ServerIpByte1of4=", LOG_MOST);
    sprintf (DebugBuf, "%d", ServerIpByte1of4);
    display (DebugBuf, LOG_MOST);
    display ("ServerIpByte2of4=", LOG_MOST);
    sprintf (DebugBuf, "%d", ServerIpByte2of4);
    display (DebugBuf, LOG_MOST);
    display ("ServerIpByte3of4=", LOG_MOST);
    sprintf (DebugBuf, "%d", ServerIpByte3of4);
    display (DebugBuf, LOG_MOST);
    display ("ServerIpByte4of4=", LOG_MOST);
    sprintf (DebugBuf, "%d", ServerIpByte4of4);
    display (DebugBuf, LOG_MOST);
    display ("RemoteDnsIpAddress=", LOG_MOST);
    sprintf (DebugBuf, "%s", RemoteDnsIpAddress);
    display (DebugBuf, LOG_MOST);
    display ("PeerInPortNumber =", LOG_MOST);
    sprintf (DebugBuf, "%d", PeerInPortNumber);
    display (DebugBuf, LOG_MOST);
    display ("MyInPortNumber (only when in PeerToPeer mode) =", LOG_MOST);
    sprintf (DebugBuf, "%d", MyInPortNumber);
    display (DebugBuf, LOG_MOST);
    display ("MyNetworkId=", LOG_MOST);
    sprintf (DebugBuf, "%d", MyNetworkId);
    display (DebugBuf, LOG_MOST);
    display ("NetworkPassword=", LOG_MOST);
    sprintf (DebugBuf, "%s", NetworkPassword);
    display (DebugBuf, LOG_MOST);
    display ("NetworkTransmitTimerInterval=", LOG_MOST);
    sprintf (DebugBuf, "%d", NetworkTransmitTimerInterval);
    display (DebugBuf, LOG_MOST);
    str = cf->getString (ret, "NetworkMode");
    if (str == NULL)
        { 
        display ("load_config() Could NOT find NetworkMode within LacConfig.txt file.", LOG_MOST);
        NetworkMode = 0;
        }
    else
        {
        NetworkMode = atoi (str);
        display ("NetworkMode=", LOG_MOST);
        sprintf (DebugBuf, "%d", NetworkMode);
        display (DebugBuf, LOG_MOST);
        }
    str = cf->getString (ret, "Realm");
    if (str == NULL)
        { 
        display ("load_config() Could NOT find Realm within LacConfig.txt file.", LOG_MOST);
        Realm = 1; // Default to 1.
        }
    else
        {
        Realm = atoi (str);
        display ("Realm=", LOG_MOST);
        sprintf (DebugBuf, "%d", Realm);
        display (DebugBuf, LOG_MOST);
        }
    str = cf->getString (ret, "CommunityHandle");
    if (str == NULL)
        { 
        display ("load_config() Could NOT find CommunityHandle within LacConfig.txt file.", LOG_MOST);
        strncpy(&CommunityHandle[0], "00000000000000", 16); // Default to "00000000000000"
        }
    else
        { 
        display ("load_config() Found CommunityHandle within LacConfig.txt file.", LOG_MOST);
        strncpy (&CommunityHandle[0], str, 16);
        }

    str = cf->getString (ret, "DefaultAircraft");
    if (str == NULL)
        { 
        display ("load_config() Could NOT find DefaultAircraft within LacConfig.txt file.", LOG_MOST);
        DefaultAircraft = FIGHTER_P38L; // Default to P38L.
        }
    else
        {
        DefaultAircraft = atoi (str);
        display ("DefaultAircraft=", LOG_MOST);
        sprintf (DebugBuf, "%d", DefaultAircraft);
        display (DebugBuf, LOG_MOST);
        }

    str = cf->getString (ret, "CommsMacro01");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro01 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro01String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE01 CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro01 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro01String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro02");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro02 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro02String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE01 BLUE CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro02 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro02String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro03");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro03 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro03String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE01 RED CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro03 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro03String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro04");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro04 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro04String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE02 CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro04 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro04String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro05");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro05 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro05String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE02 BLUE CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro05 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro05String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro06");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro06 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro06String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE02 RED CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro06 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro06String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro07");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro07 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro07String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE03 CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro07 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro07String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro08");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro08 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro08String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE03 BLUE CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro08 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro08String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro09");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro09 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro09String1[0], "SWITCHED MUMBLE RADIO TO MISSIONNETWORKBATTLE03 RED CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro09 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro09String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro10");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro10 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro10String1[0], "SWITCHED MUMBLE RADIO TO ROOT CHANNEL.", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro10 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro10String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro11");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro11 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro11String1[0], "MUMBLE MACRO 11", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro11 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro11String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro12");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro12 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro12String1[0], "MUMBLE MACRO 12", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro12 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro12String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro13");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro13 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro13String1[0], "MUMBLE MACRO 13", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro13 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro13String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro14");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro14 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro14String1[0], "MUMBLE MACRO 14", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro14 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro14String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro15");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro15 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro15String1[0], "MUMBLE MACRO 15", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro15 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro15String1[0], str, 64); 
        }

    str = cf->getString (ret, "CommsMacro16");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find CommsMacro16 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&CommsMacro16String1[0], "MUMBLE MACRO 16", 64);
        }
    else
        {
        display ("load_config() Found CommsMacro16 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&CommsMacro16String1[0], str, 64); 
        }

    str = cf->getString (ret, "PlayerIdString01");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString01 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[0][0], "PLAYER01", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString01 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[0][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString02");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString02 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[1][0], "PLAYER02", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString02 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[1][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString03");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString03 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[2][0], "PLAYER03", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString03 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[2][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString04");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString04 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[3][0], "PLAYER04", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString04 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[3][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString05");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString05 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[4][0], "PLAYER05", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString05 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[4][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString06");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString06 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[5][0], "PLAYER06", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString06 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[5][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString07");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString07 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[6][0], "PLAYER07", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString07 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[6][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString08");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString08 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[7][0], "PLAYER08", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString08 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[7][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString09");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString09 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[8][0], "PLAYER09", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString09 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[8][0], str, 9); 
        }

    str = cf->getString (ret, "PlayerIdString10");
    if (str == NULL)
        { 
        display ("Load_config() Did NOT find PlayerIdString10 in LacConfig.txt. Loading default....", LOG_MOST);
        strncpy(&PlayerIdStrings[9][0], "PLAYER10", 9);
        }
    else
        {
        display ("load_config() Found PlayerIdString10 in LacConfig.txt. str=", LOG_MOST);
        StringToUpperCase(str);
        sprintf (DebugBuf, "%s", str);
        display (DebugBuf, LOG_MOST);
        strncpy (&PlayerIdStrings[9][0], str, 9); 
        }

    delete cf;
    return 1;
    } 

int load_saveconfig ()
    {
    char buf [STDSIZE];
    char ret [256];
    char *str;
    char *confname = dirs->getSaves ("saveconf");
    sprintf (buf, "Loading %s ", confname);
    display (buf, LOG_MOST);
    ConfigFile *cf = new ConfigFile (confname);

    if (cf->buf [0] == 0)   // no file found
        {
        delete cf;
        return 0;
        }
    str = cf->getString (ret, "savewidth");
    if (str == NULL)
        {
        width = 800;
        }
    else
        {
        width = atoi (str);
        }
    if (width < 100)
        {
        width = 100;
        }
    else if (width > 3000)
        {
        width = 1024;
        }
    str = cf->getString (ret, "saveheight");
    if (str == NULL)
        {
        height = 600;
        }
    else
        {
        height = atoi (str);
        }
    if (height < 100)
        {
        height = 100;
        }
    else if (height > 2000)
        {
        height = 768;
        }
    str = cf->getString (ret, "savebpp");
    if (str == NULL)
        {
        bpp = 32;
        }
    else
        {
        bpp = atoi (str);
        }
    if (bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32)
        {
        bpp = 32;
        }
    str = cf->getString (ret, "savefullscreen");
    if (str == NULL)
        {
        fullscreen = 1;
        }
    else
        {
        fullscreen = atoi (str);
        }
    if (fullscreen)
        {
        fullscreen = 1;
        }
    delete cf;
    return 1;
    } 

void writeJoystick (ConfigFile *cf, char *str, int jn)
    {
    char buf [STDSIZE];
    sprintf (buf, "%s = %c%d", str, 'A' + (jn / 1000), jn % 1000);
    cf->writeText (buf);
    } 

void save_configInterface ()
    {
    char buf [STDSIZE];
    ConfigFile *cf = new ConfigFile ();
    char *confname = dirs->getSaves ("LacControls.txt");
    sprintf (buf, "Saving %s ", confname);
    display (buf, LOG_MOST);
    int ret1 = cf->openOutput (confname);

    if (ret1 == 0)
        {
        display ("Could not save interface configuration", LOG_ERROR);
        return;
        }
    cf->writeText ("# This is the Controls Setup file for LINUX AIR COMBAT.\n\n");
    cf->writeText ("# This file should be named LacControls.txt and it should be stored");
    cf->writeText ("# in a hidden folder named .LAC beneath your home folder.\n");
    cf->writeText ("# For example, if your LINUX username is fred and your home");
    cf->writeText ("# folder is at /home/fred, then this file should be found at:\n");
    cf->writeText ("# /home/fred/.LAC/LacControls.txt\n");
    cf->writeText ("# This file contains two different kinds of text. Firstly, notice");
    cf->writeText ("# that many lines (like this one) commence with a pound sign. Any");
    cf->writeText ("# line that commences with a pound sign is ignored by LINUX AIR COMBAT.");
    cf->writeText ("# All lines of that type are known as Comment Lines.\n");
    cf->writeText ("# Comment lines are displayed for your convenience in understanding the");
    cf->writeText ("# purpose of the OTHER lines, which do NOT commence with a pound sign.\n");
    cf->writeText ("# Secondly, other lines all contain information defining the behavior");
    cf->writeText ("# of your computer's input devices (such as your joystick, keyboard, and mouse)");
    cf->writeText ("# when controlling the available functions within LINUX AIR COMBAT.\n");
    cf->writeText ("# By editing this file, you can assign any keyboard key or any joystick");
    cf->writeText ("# button to any of 44 different digital flight functions. You can also");
    cf->writeText ("# assign any joystick axis to function as your aircraft's elevator, rudder,");
    cf->writeText ("# aileron, or throttle control. You can also assign three aircraft functions");
    cf->writeText ("# to handy buttons on your computer's mouse pointer, and you can influence");
    cf->writeText ("# the simulator's sensitivity to mouse movement when you use your mouse to");
    cf->writeText ("# control your aircraft instead of a joystick.\n");
    cf->writeText ("# Accordingly, this file is divided into three sections: one for");
    cf->writeText ("# keyboard mapping, one for mouse mapping, and one for joystick");
    cf->writeText ("# mapping.\n");
    cf->writeText ("# Notice that all of the configuration lines begin with a");
    cf->writeText ("# description of a computer or aircraft control, followed by");
    cf->writeText ("# an = sign and then a number. When you use your favorite,");
    cf->writeText ("# simple text editor to change those numbers, you can change ");
    cf->writeText ("# the effect of the corresponding control.\n");
    cf->writeText ("# In the beginning, you will need help in order to know the ");
    cf->writeText ("# appropriate values for numbers in this situation.\n");
    cf->writeText ("# This is best done by running LINUX AIR COMBAT in a small");
    cf->writeText ("# window, launched from a bash console, so that you can see ");
    cf->writeText ("# accompanying debug information. In that situation, every");
    cf->writeText ("# time you press a keyboard key or joystick button, the ");
    cf->writeText ("# associated numeric value is immediately displayed in a");
    cf->writeText ("# debug message that you will see among other messages ");
    cf->writeText ("# scrolling within your display according to long-established");
    cf->writeText ("# traditions. (Learn how to run LAC in a small window by");
    cf->writeText ("# consulting the LacConfig.txt file.)\n\n");
    cf->writeText ("# ---------------------------------------------------------------------");
    cf->writeText ("# Keyboard section");
    cf->writeText ("# ---------------------------------------------------------------------\n");
    cf->writeText ("# Use ASCII-Code values for alphabetic keyboard keys. Examples:");
    cf->writeText ("#  8=BACKSPACE, 13=ENTER, 32=SPACE, 65=A...90=Z (NOT case sensitive)");
    cf->write       (" key_PRIMARY",               (int) key_PRIMARY);
    cf->write       (" key_SECONDARY",             (int) key_SECONDARY);
    cf->write       (" key_DROPFLARE",             (int) key_DROPFLARE);
    cf->write       (" key_DROPCHAFF",             (int) key_DROPCHAFF);
    cf->write       (" key_WEAPONSELECT",          (int) key_WEAPONSELECT);
    cf->write       (" key_TARGETNEXT",            (int) key_TARGETNEXT);
    cf->write       (" key_TARGETPREVIOUS",        (int) key_TARGETPREVIOUS);
    cf->write       (" key_TARGETVOCALIZE",        (int) key_TARGETVOCALIZE);
    cf->write       (" key_ZoomFovIn",             (int) key_ZoomFovIn);
    cf->write       (" key_ZoomFovOut",            (int) key_ZoomFovOut);
    cf->write       (" key_ToggleViewIntExt",      (int) key_ToggleViewIntExt);
    cf->write       (" key_FlapsUp",               (int) key_FlapsUp);
    cf->write       (" key_FlapsDn",               (int) key_FlapsDn);
    cf->write       (" key_WepAfterburner",        (int) key_WepAfterburner);
    cf->write       (" key_SpeedBrake",            (int) key_SpeedBrake);
    cf->write       (" key_Undercarriage",         (int) key_Undercarriage);
    cf->write       (" key_TrimRudderLeft",        (int) key_TrimRudderLeft);
    cf->write       (" key_TrimRudderRight",       (int) key_TrimRudderRight);
    cf->write       (" key_TrimElevatorUp",        (int) key_TrimElevatorUp);
    cf->write       (" key_TrimElevatorDn",        (int) key_TrimElevatorDn);
    cf->write       (" key_LookBK",                (int) key_LookBK);
    cf->write       (" key_LookDN",                (int) key_LookDN);
    cf->write       (" key_LookFD",                (int) key_LookFD);
    cf->write       (" key_LookLT",                (int) key_LookLT);
    cf->write       (" key_LookRT",                (int) key_LookRT);
    cf->write       (" key_LookUP",                (int) key_LookUP);
    cf->write       (" key_RadarZoomIn",           (int) key_RadarZoomIn);
    cf->write       (" key_RadarZoomOut",          (int) key_RadarZoomOut);
    cf->write       (" key_RadarOnOff",            (int) key_RadarOnOff);
    cf->write       (" key_HudOnOff",              (int) key_HudOnOff);
    cf->write       (" key_IffOnOff",              (int) key_IffOnOff);
    cf->write       (" key_HudLadderBarsOnOff",    (int) key_HudLadderBarsOnOff);
    cf->write       (" key_MapViewOnOff",          (int) key_MapViewOnOff);
    cf->write       (" key_MapZoomIn",             (int) key_MapZoomIn);
    cf->write       (" key_MapZoomOut",            (int) key_MapZoomOut);
    cf->write       (" key_MapScrollEast",         (int) key_MapScrollEast);
    cf->write       (" key_MapScrollWest",         (int) key_MapScrollWest);
    cf->write       (" key_MapScrollNorth",        (int) key_MapScrollNorth);
    cf->write       (" key_MapScrollSouth",        (int) key_MapScrollSouth);
    cf->write       (" key_MumblePtt",             (int) key_MumblePtt);
    cf->write       (" key_MumbleWhisper",         (int) key_MumbleWhisper);
    cf->write       (" key_CommsMacro01",          (int) key_CommsMacro01);
    cf->write       (" key_CommsMacro02",          (int) key_CommsMacro02);
    cf->write       (" key_CommsMacro03",          (int) key_CommsMacro03);
    cf->write       (" key_CommsMacro04",          (int) key_CommsMacro04);
    cf->write       (" key_CommsMacro05",          (int) key_CommsMacro05);
    cf->write       (" key_CommsMacro06",          (int) key_CommsMacro06);
    cf->write       (" key_CommsMacro07",          (int) key_CommsMacro07);
    cf->write       (" key_CommsMacro08",          (int) key_CommsMacro08);
    cf->write       (" key_CommsMacro09",          (int) key_CommsMacro09);
    cf->write       (" key_CommsMacro10",          (int) key_CommsMacro10);
    cf->write       (" key_CommsMacro11",          (int) key_CommsMacro11);
    cf->write       (" key_CommsMacro12",          (int) key_CommsMacro12);
    cf->write       (" key_CommsMacro13",          (int) key_CommsMacro13);
    cf->write       (" key_CommsMacro14",          (int) key_CommsMacro14);
    cf->write       (" key_CommsMacro15",          (int) key_CommsMacro15);
    cf->write       (" key_CommsMacro16",          (int) key_CommsMacro16);
    cf->write       (" key_GunCameraToggle",       (int) key_GunCameraToggle);
    cf->writeText   ("\n\n# ---------------------------------------------------------------------");
    cf->writeText   ("# Mouse section");
    cf->writeText   ("# ---------------------------------------------------------------------\n");
    cf->writeText   ("# Sensitivity: 70...200%, 70%=full screen area, 200%=max sensitivity");
    cf->write       (" mouse_sensitivity", (int) mouse_sensitivity);
    cf->writeText   ("# Reverse y-axis: 0 (no) or 1 (yes)");
    cf->write       (" mouse_reverse", (int) mouse_reverse);
    cf->writeText   ("# Relative coordinates: 0 (no) or 1 (yes)");
    cf->write       (" mouse_relative", (int) mouse_relative);
    cf->writeText   ("# Auto rudder on x-axis, dead area for rolls: 0...100");
    cf->write       (" mouse_autorudder", (int) mouse_autorudder);
    cf->writeText   ("\n# Buttons: 1=Left, 2=Middle, 3=Right");
    int mousebutton = 1;
    if (mouse_firecannon == MOUSE_BUTTON_MIDDLE)
        {
        mousebutton = 2;
        }
    else if (mouse_firecannon == MOUSE_BUTTON_RIGHT)
        {
        mousebutton = 3;
        }
    else
        {
        mousebutton = 1;
        }
    cf->write       (" mouse_firecannon", mousebutton);
    if (mouse_firemissile == MOUSE_BUTTON_MIDDLE)
        {
        mousebutton = 2;
        }
    else if (mouse_firemissile == MOUSE_BUTTON_RIGHT)
        {
        mousebutton = 3;
        }
    else
        {
        mousebutton = 1;
        }
    cf->write       (" mouse_firemissile", mousebutton);
    if (mouse_selectmissile == MOUSE_BUTTON_MIDDLE)
        {
        mousebutton = 2;
        }
    else if (mouse_selectmissile == MOUSE_BUTTON_RIGHT)
        {
        mousebutton = 3;
        }
    else
        {
        mousebutton = 1;
        }
    cf->write       (" mouse_selectmissile", mousebutton);
    cf->writeText   ("\n# ---------------------------------------------------------------------");
    cf->writeText   ("# Joystick section");
    cf->writeText   ("# ---------------------------------------------------------------------\n");
    cf->writeText   ("# The number of axes, buttons, and the coolie hat depends on your joystick!");
    cf->writeText   ("# Numbers start with A=first joystick, B=second joystick...J=10th joystick");
    cf->writeText   ("# followed by a number to identify axes, buttons, and coolie hat\n");
    cf->writeText   ("# Axis: 0...MAX-1 (maybe 0=aileron 1=elevator 3=throttle 2=rudder)");
    cf->writeText   ("\n# Buttons: 0...MAX-1, Coolie: 100=Right, 101=Up, 102=Left, 103=Down");
    writeJoystick   (cf, " joystick_aileron", joystick_aileron);
    writeJoystick   (cf, " joystick_elevator", joystick_elevator);
    writeJoystick   (cf, " joystick_throttle", joystick_throttle);
    writeJoystick   (cf, " joystick_rudder", joystick_rudder);
    writeJoystick   (cf, " joystick_view_x", joystick_view_x);
    writeJoystick   (cf, " joystick_view_y", joystick_view_y);
    writeJoystick   (cf, " joystick_FIREPRIMARY", joystick_FIREPRIMARY);
    writeJoystick   (cf, " joystick_FIRESECONDARY", joystick_FIRESECONDARY);
    writeJoystick   (cf, " joystick_DROPFLARE", joystick_DROPFLARE);
    writeJoystick   (cf, " joystick_DROPCHAFF", joystick_DROPCHAFF);
    writeJoystick   (cf, " joystick_WEAPONSELECT", joystick_WEAPONSELECT);
    writeJoystick   (cf, " joystick_ZoomFOVin", joystick_ZoomFOVin);
    writeJoystick   (cf, " joystick_ZoomFOVout", joystick_ZoomFOVout);
    writeJoystick   (cf, " joystick_TargetNext", joystick_TargetNext);
    writeJoystick   (cf, " joystick_TargetPrev", joystick_TargetPrev);
    writeJoystick   (cf, " joystick_ToggleExtIntView", joystick_ToggleExtIntView);
    writeJoystick   (cf, " joystick_FlapsUP", joystick_FlapsUP);
    writeJoystick   (cf, " joystick_FlapsDN", joystick_FlapsDN);
    writeJoystick   (cf, " joystick_WEPorAfterburner", joystick_WEPorAfterburner);
    writeJoystick   (cf, " joystick_SpeedBrake", joystick_SpeedBrake);
    writeJoystick   (cf, " joystick_Undercarriage", joystick_Undercarriage);
    writeJoystick   (cf, " joystick_TrimRudderLeft", joystick_TrimRudderLeft);
    writeJoystick   (cf, " joystick_TrimRudderRight", joystick_TrimRudderRight);
    writeJoystick   (cf, " joystick_TrimElevatorUp", joystick_TrimElevatorUp);
    writeJoystick   (cf, " joystick_TrimElevatorDn", joystick_TrimElevatorDn);
    writeJoystick   (cf, " joystick_LookBK", joystick_LookBK);
    writeJoystick   (cf, " joystick_LookDN", joystick_LookDN);
    writeJoystick   (cf, " joystick_LookFD", joystick_LookFD);
    writeJoystick   (cf, " joystick_LookLT", joystick_LookLT);
    writeJoystick   (cf, " joystick_LookRT", joystick_LookRT);
    writeJoystick   (cf, " joystick_LookUP", joystick_LookUP);
    writeJoystick   (cf, " joystick_RadarZoomIn", joystick_RadarZoomIn);
    writeJoystick   (cf, " joystick_RadarZoomOut", joystick_RadarZoomOut);
    writeJoystick   (cf, " joystick_RadarOnOff", joystick_RadarOnOff);
    writeJoystick   (cf, " joystick_HudOnOff", joystick_HudOnOff);
    writeJoystick   (cf, " joystick_IffOnOff", joystick_IffOnOff);
    writeJoystick   (cf, " joystick_HudLadderBarsOnOff", joystick_HudLadderBarsOnOff);
    writeJoystick   (cf, " joystick_MapViewOnOff", joystick_MapViewOnOff);
    writeJoystick   (cf, " joystick_MapZoomIn", joystick_MapZoomIn);
    writeJoystick   (cf, " joystick_MapZoomOut", joystick_MapZoomOut);
    writeJoystick   (cf, " joystick_MapScrollEast", joystick_MapScrollEast);
    writeJoystick   (cf, " joystick_MapScrollWest", joystick_MapScrollWest);
    writeJoystick   (cf, " joystick_MapScrollNorth", joystick_MapScrollNorth);
    writeJoystick   (cf, " joystick_MapScrollSouth", joystick_MapScrollSouth);
    writeJoystick   (cf, " joystick_TargetVocalize", joystick_TargetVocalize);
    cf->writeText   ("\n# This file is meant to give sensible custom interface settings");
    cf->writeText   ("\n# To get back to default settings, just delete this file!");
    cf->writeText   ("\n# Online help is available at AskMisterWizard.com/FlightSimMovies/LAC");
    cf->close ();
    delete cf;
    } 

int getKey (char *str, int n)
    {
    int tmp; 

    if (str == NULL)
        {
        return n;    
        }
    tmp = atoi (str); 
    if (tmp == 0)
        {
        
        if (str [0] >= 33)
            {
            
            return toupper (str [0]); 
            }
        return n; 
        }
    return tmp; 
    } 

int getJoystick (char *str, int n)
    {
    int tmp, jn = 0;

    if (str == NULL)
        {
        return n;
        }
    int str0 = toupper (str [0]) - (int) 'A';
    if (str0 >= -1 && str0 < 10)
        {
        jn = str0;
        }
    else
        {
        return n;
        }
    tmp = atoi (&str [1]);
    return jn * 1000 + tmp;
    } 

int load_configInterface ()
    {
    
    char buf [STDSIZE];
    char ret [256];
    char *str;
    
    char *confname = dirs->getSaves ("LacControls.txt");
    sprintf (buf, "Loading %s ", confname);
    display (buf, LOG_MOST);
    ConfigFile *cf = new ConfigFile (confname);
    
    str = cf->getString (ret, "key_PRIMARY"); // Get number from file, for key invoking this command.
    
    display ("load_configInterface() 150804CA. Searched for key_PRIMARY.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_PRIMARY = getKey (str, 32); // Defaults to 32 on error
    
    str = cf->getString (ret, "key_SECONDARY"); 
    
    display ("load_configInterface() 150804CB. Searched for key_SECONDARY.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_SECONDARY = getKey (str, 13); // Defaults to 13 on error
    
    str = cf->getString (ret, "key_DROPCHAFF");
    
    display ("load_configInterface() 150804CC. Searched for key_DROPCHAFF.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_DROPCHAFF = getKey (str, 'C');
    
    str = cf->getString (ret, "key_DROPFLARE");
    
    display ("load_configInterface() 150804CD. Searched for key_DROPFLARE.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_DROPFLARE = getKey (str, 'F');
    
    str = cf->getString (ret, "key_WEAPONSELECT");
    
    display ("load_configInterface() 150804CE. Searched for key_WEAPONSELECT.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_WEAPONSELECT = getKey (str, 'M');
    
    str = cf->getString (ret, "key_TARGETNEXT");
    
    display ("load_configInterface() 150804CF. Searched for key_TARGETNEXT.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TARGETNEXT = getKey (str, 'E');
    
    str = cf->getString (ret, "key_TARGETPREVIOUS");
    
    display ("load_configInterface() 150804CG. Searched for key_TARGETPREVIOUS.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TARGETPREVIOUS = getKey (str, 'P');
    
    str = cf->getString (ret, "key_TARGETVOCALIZE");
    
    display ("load_configInterface() 150804CG. Searched for key_TARGETVOCALIZE.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TARGETVOCALIZE = getKey (str, 'V');
    
    str = cf->getString (ret, "key_ZoomFovIn");
    
    display ("load_configInterface() 150804CH. Searched for key_ZoomFovIn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_ZoomFovIn = getKey (str, '[');
    
    str = cf->getString (ret, "key_ZoomFovOut");
    
    display ("load_configInterface() 150804CI. Searched for key_ZoomFovOut.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_ZoomFovOut = getKey (str, ']');
    
    str = cf->getString (ret, "key_ToggleViewIntExt");
    
    display ("load_configInterface() 150804CJ. Searched for key_ToggleViewIntExt.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_ToggleViewIntExt = getKey (str, 'V');
    
    str = cf->getString (ret, "key_FlapsUp");
    
    display ("load_configInterface() 150804CK. Searched for key_FlapsUp.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_FlapsUp = getKey (str, 'W');
    
    str = cf->getString (ret, "key_FlapsDn");
    
    display ("load_configInterface() 150804CL. Searched for key_FlapsDn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_FlapsDn = getKey (str, 'Q');
    
    str = cf->getString (ret, "key_WepAfterburner");
    
    display ("load_configInterface() 150804CM. Searched for key_WepAfterburner.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_WepAfterburner = getKey (str, 'A');
    
    str = cf->getString (ret, "key_SpeedBrake");
    
    display ("load_configInterface() 150804CN. Searched for key_SpeedBrake.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_SpeedBrake = getKey (str, 'B');
    
    str = cf->getString (ret, "key_Undercarriage");
    
    display ("load_configInterface() 150804CO. Searched for key_Undercarriage.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_Undercarriage = getKey (str, 'G');
    
    str = cf->getString (ret, "key_TrimRudderLeft");
    
    display ("load_configInterface() 150804CP. Searched for key_TrimRudderLeft.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TrimRudderLeft = getKey (str, 'L');
    
    str = cf->getString (ret, "key_TrimRudderRight");
    
    display ("load_configInterface() 150804CQ. Searched for key_TrimRudderRight.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TrimRudderRight = getKey (str, 'R');
    
    str = cf->getString (ret, "key_TrimElevatorUp");
    
    display ("load_configInterface() 150804CR. Searched for key_TrimElevatorUp.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TrimElevatorUp = getKey (str, 'U');
    
    str = cf->getString (ret, "key_TrimElevatorDn");
    
    display ("load_configInterface() 150804CS. Searched for key_TrimElevatorDn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_TrimElevatorDn = getKey (str, 'D');
    
    str = cf->getString (ret, "key_LookBK");
    
    display ("load_configInterface() 150804CS. Searched for key_LookBK.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookBK = getKey (str, '2');
    
    str = cf->getString (ret, "key_LookDn");
    
    display ("load_configInterface() 150804CS. Searched for key_LookDn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookDN = getKey (str, '/');
    
    str = cf->getString (ret, "key_LookFD");
    
    display ("load_configInterface() 150804CS. Searched for key_LookFD.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookFD = getKey (str, '8');
    
    str = cf->getString (ret, "key_LookLT");
    
    display ("load_configInterface() 150804CS. Searched for key_LookLT.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookLT = getKey (str, '4');
    
    str = cf->getString (ret, "key_LookRT");
    
    display ("load_configInterface() 150804CS. Searched for key_LookRT.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookRT = getKey (str, '6');
    
    str = cf->getString (ret, "key_LookUP");
    
    display ("load_configInterface() 150804CS. Searched for key_LookUP.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_LookUP = getKey (str, '5');
    
    str = cf->getString (ret, "key_RadarZoomIn");
    
    display ("load_configInterface() 150804CS. Searched for key_RadarZoomIn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_RadarZoomIn = getKey (str, 'D');
    
    str = cf->getString (ret, "key_RadarZoomOut");
    
    display ("load_configInterface() 150804CS. Searched for key_RadarZoomOut.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_RadarZoomOut = getKey (str, 'D');
    
    str = cf->getString (ret, "key_RadarOnOff");
    
    display ("load_configInterface() 150804CS. Searched for key_RadarOnOff.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_RadarOnOff = getKey (str, 'r');
    
    str = cf->getString (ret, "key_HudOnOff");
    
    display ("load_configInterface() 150804CS. Searched for key_HudOnOff.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_HudOnOff = getKey (str, 'h');
    
    str = cf->getString (ret, "key_IffOnOff");
    
    display ("load_configInterface() 150804CS. Searched for key_IffOnOff.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_IffOnOff = getKey (str, 'i');
    
    str = cf->getString (ret, "key_HudLadderBarsOnOff");
    
    display ("load_configInterface() 150804CS. Searched for key_HudLadderBarsOnOff.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_HudLadderBarsOnOff = getKey (str, 'l');
    
    str = cf->getString (ret, "key_MapViewOnOff");
    
    display ("load_configInterface() 150804CS. Searched for key_MapViewOnOff.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapViewOnOff = getKey (str, 'm');
    
    str = cf->getString (ret, "key_MapZoomIn");
    
    display ("load_configInterface() 150804CS. Searched for key_MapZoomIn.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapZoomIn = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MapZoomOut");
    
    display ("load_configInterface() 150804CS. Searched for key_MapZoomOut.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapZoomOut = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MapScrollEast");
    
    display ("load_configInterface() 150804CS. Searched for key_MapScrollEast.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapScrollEast = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MapScrollWest");
    
    display ("load_configInterface() 150804CS. Searched for key_MapScrollWest.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapScrollWest = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MapScrollNorth");
    
    display ("load_configInterface() 150804CS. Searched for key_MapScrollNorth.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapScrollNorth = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MapScrollSouth");
    
    display ("load_configInterface() 150804CS. Searched for key_MapScrollSouth.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MapScrollSouth = getKey (str, 'h');
    
    str = cf->getString (ret, "key_MumblePtt");
    
    display ("load_configInterface() 150804CS. Searched for key_MumblePtt.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MumblePtt = getKey (str, 'h');

    str = cf->getString (ret, "key_MumbleWhisper");
    
    display ("load_configInterface() 150804CS. Searched for key_MumbleWhisper.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_MumbleWhisper = getKey (str, 'h');

    str = cf->getString (ret, "key_CommsMacro01");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro01.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro01 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro02");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro02.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro02 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro03");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro03.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro03 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro04");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro04.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro04 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro05");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro05.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro05 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro06");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro06.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro06 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro07");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro07.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro07 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro08");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro08.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro08 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro09");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro09.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro09 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro10");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro10.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro10 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro11");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro11.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro11 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro12");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro12.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro12 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro13");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro13.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro13 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro14");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro14.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro14 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro15");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro15.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro15 = getKey (str, '1');

    str = cf->getString (ret, "key_CommsMacro16");
    
    display ("load_configInterface() 150804CS. Searched for key_CommsMacro16.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_CommsMacro16 = getKey (str, '1');

    str = cf->getString (ret, "key_GunCameraToggle");
    
    display ("load_configInterface() 150804CK. Searched for key_GunCameraToggle.", LOG_MOST);
    sprintf (DebugBuf, "%s", str); 
    display (DebugBuf, LOG_MOST);  
    key_GunCameraToggle = getKey (str, 'C');

    str = cf->getString (ret, "mouse_sensitivity");
    if (str == NULL)
        {
        mouse_sensitivity = 100;
        }
    else
        {
        mouse_sensitivity = atoi (str);
        }
    if (mouse_sensitivity < 70)
        {
        mouse_sensitivity = 70;
        }
    else if (mouse_sensitivity > 600)
        {
        mouse_sensitivity = 600;
        }
    str = cf->getString (ret, "mouse_reverse");
    if (str == NULL)
        {
        mouse_reverse = false;
        }
    else
        {
        mouse_reverse = (atoi (str) != 0);
        }
    str = cf->getString (ret, "mouse_relative");
    if (str == NULL)
        {
        mouse_relative = false;
        }
    else
        {
        mouse_relative = (atoi (str) != 0);
        }
    str = cf->getString (ret, "mouse_autorudder");
    if (str == NULL)
        {
        mouse_autorudder = 30;
        }
    else
        {
        mouse_autorudder = atoi (str);
        }
    if (mouse_autorudder < 0)
        {
        mouse_autorudder = 0;
        }
    else if (mouse_autorudder > 100)
        {
        mouse_autorudder = 100;
        }
    int mousebutton = 1;
    str = cf->getString (ret, "mouse_firecannon");
    if (str == NULL)
        {
        mousebutton = 1;
        }
    else
        {
        mousebutton = atoi (str);
        }
    if (mousebutton == 2)
        {
        mouse_firecannon = MOUSE_BUTTON_MIDDLE;
        }
    else if (mousebutton == 3)
        {
        mouse_firecannon = MOUSE_BUTTON_RIGHT;
        }
    else
        {
        mouse_firecannon = MOUSE_BUTTON_LEFT;
        }
    str = cf->getString (ret, "mouse_firemissile");
    if (str == NULL)
        {
        mousebutton = 3;
        }
    else
        {
        mousebutton = atoi (str);
        }
    if (mousebutton == 2)
        {
        mouse_firemissile = MOUSE_BUTTON_MIDDLE;
        }
    else if (mousebutton == 3)
        {
        mouse_firemissile = MOUSE_BUTTON_RIGHT;
        }
    else
        {
        mouse_firemissile = MOUSE_BUTTON_LEFT;
        }
    str = cf->getString (ret, "mouse_selectmissile");
    if (str == NULL)
        {
        mousebutton = 2;
        }
    else
        {
        mousebutton = atoi (str);
        }
    if (mousebutton == 2)
        {
        mouse_selectmissile = MOUSE_BUTTON_MIDDLE;
        }
    else if (mousebutton == 3)
        {
        mouse_selectmissile = MOUSE_BUTTON_RIGHT;
        }
    else
        {
        mouse_selectmissile = MOUSE_BUTTON_LEFT;
        }
    
    str = cf->getString (ret, "joystick_aileron");
    joystick_aileron = getJoystick (str, 0);
    str = cf->getString (ret, "joystick_elevator");
    joystick_elevator = getJoystick (str, 1);
    str = cf->getString (ret, "joystick_throttle");
    joystick_throttle = getJoystick (str, 2);
    str = cf->getString (ret, "joystick_rudder");
    joystick_rudder = getJoystick (str, 3);
    str = cf->getString (ret, "joystick_view_x");
    joystick_view_x = getJoystick (str, 4);
    str = cf->getString (ret, "joystick_view_y");
    joystick_view_y = getJoystick (str, 5);
    
    str = cf->getString (ret, "joystick_FIREPRIMARY");
    joystick_FIREPRIMARY = getJoystick (str, 0);
    str = cf->getString (ret, "joystick_FIRESECONDARY");
    joystick_FIRESECONDARY = getJoystick (str, 1);
    str = cf->getString (ret, "joystick_DROPFLARE");
    joystick_DROPFLARE = getJoystick (str, 7);
    str = cf->getString (ret, "joystick_DROPCHAFF");
    joystick_DROPCHAFF = getJoystick (str, 8);
    str = cf->getString (ret, "joystick_WEAPONSELECT");
    joystick_WEAPONSELECT = getJoystick (str, 10);
    str = cf->getString (ret, "joystick_ZoomFOVin");
    joystick_ZoomFOVin = getJoystick (str, 100);
    str = cf->getString (ret, "joystick_ZoomFOVout");
    joystick_ZoomFOVout = getJoystick (str, 102);
    str = cf->getString (ret, "joystick_TargetNext");
    joystick_TargetNext = getJoystick (str, 101);
    str = cf->getString (ret, "joystick_TargetPrev");
    joystick_TargetPrev = getJoystick (str, 104);
    str = cf->getString (ret, "joystick_ToggleExtIntView");
    joystick_ToggleExtIntView = getJoystick (str, 11);
    str = cf->getString (ret, "joystick_FlapsUP");
    joystick_FlapsUP = getJoystick (str, 4);
    str = cf->getString (ret, "joystick_FlapsDN");
    joystick_FlapsDN = getJoystick (str, 2);
    str = cf->getString (ret, "joystick_WEPorAfterburner");
    joystick_WEPorAfterburner = getJoystick (str, 5);
    str = cf->getString (ret, "joystick_SpeedBrake");
    joystick_SpeedBrake = getJoystick (str, 3);
    str = cf->getString (ret, "joystick_Undercarriage");
    joystick_Undercarriage = getJoystick (str, 12);
    str = cf->getString (ret, "joystick_TrimRudderLeft");
    joystick_TrimRudderLeft = getJoystick (str, 13);
    str = cf->getString (ret, "joystick_TrimRudderRight");
    joystick_TrimRudderRight = getJoystick (str, 14);
    str = cf->getString (ret, "joystick_TrimElevatorUp");
    joystick_TrimElevatorUp = getJoystick (str, 9);
    str = cf->getString (ret, "joystick_TrimElevatorDn");
    joystick_TrimElevatorDn = getJoystick (str, 6);
    str = cf->getString (ret, "joystick_LookBK");
    joystick_LookBK = getJoystick (str, 15);
    str = cf->getString (ret, "joystick_LookDN");
    joystick_LookDN = getJoystick (str, 16);
    str = cf->getString (ret, "joystick_LookFD");
    joystick_LookFD = getJoystick (str, 17);
    str = cf->getString (ret, "joystick_LookLT");
    joystick_LookLT = getJoystick (str, 18);
    str = cf->getString (ret, "joystick_LookRT");
    joystick_LookRT = getJoystick (str, 19);
    str = cf->getString (ret, "joystick_LookUP");
    joystick_LookUP = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_RadarZoomIn");
    joystick_RadarZoomIn = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_RadarZoomOut");
    joystick_RadarZoomOut = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_RadarOnOff");
    joystick_RadarOnOff = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_HudOnOff");
    joystick_HudOnOff = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_IffOnOff");
    joystick_IffOnOff = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_HudLadderBarsOnOff");
    joystick_HudLadderBarsOnOff = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapViewOnOff");
    joystick_MapViewOnOff = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapZoomIn");
    joystick_MapZoomIn = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapZoomOut");
    joystick_MapZoomOut = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapScrollEast");
    joystick_MapScrollEast = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapScrollWest");
    joystick_MapScrollWest = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapScrollNorth");
    joystick_MapScrollNorth = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_MapScrollSouth");
    joystick_MapScrollSouth = getJoystick (str, 20);
    str = cf->getString (ret, "joystick_TargetVocalize");
    joystick_TargetVocalize = getJoystick (str, 103);
    if (cf->buf [0] == 0)   // no file found
        {
        delete cf;
        return 0;
        }
    delete cf;
    return 1;
    } 

    void StringToUpperCase(char *StringPointer)
    {
    while(*StringPointer != '\0')
        {
        *StringPointer = toupper(*StringPointer);
        StringPointer++;
        }
    }

#endif
