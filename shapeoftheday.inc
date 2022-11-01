/* shapeoftheday.inc
 * Persistence of Vision Raytracer scene description include file
 * A proposed POV-Ray Object Collection module
 *
 * A prefab virtual environment for prototypes and experiments.
 *
 * Copyright (C) 2012 - 2022 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the CC-LGPL
 * a.k.a. the GNU Lesser General Public License version 2.1.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html for
 * the text of the GNU Lesser General Public License version 2.1.
 *
 * Version 2.0    2022-Nov-01
 * Updated        2022-Nov-01
 *
 * See shapeoftheday_history.txt for development history.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (ShapeOfTheDay_Inc_Temp) #declare ShapeOfTheDay_Inc_Temp = version;
#version max (3.5, min (3.8, version)); // Bracket the POV version.
#declare SHAPEOFTHEDAY_VERSION = 2.0;
#declare SOTD__S_INCV = "-20221101"

#declare SHAPEOFTHEDAY_S_VERSION = concat
( str (floor (SHAPEOFTHEDAY_VERSION * 10) / 10, 0, 1),
  ".", str (mod (SHAPEOFTHEDAY_VERSION * 100, 10), 1, 0),
  SOTD__S_INCV
)
#ifdef (View_POV_Include_Stack)
  #debug concat
  ( "#include \"shapeoftheday.inc\" //version ", SHAPEOFTHEDAY_S_VERSION, "\n"
  )
  #debug concat ("  #version ", str(version,0,2), ";\n")
#end

#include "colors.inc"
#include "consts.inc"
#include "functions.inc"
#include "transforms.inc"
#include "desklamp.inc"
#include "roundedge.inc"
#declare sotd__PipeMrg = file_exists ("pipemrg.inc");
#if (sotd__PipeMrg) #include "pipemrg.inc" #end

//============================ DEBUGGING SWITCHES ==============================

#ifndef (sotd__Debug_ambient) #declare sotd__Debug_ambient = no; #end
#ifndef (sotd__Debug_clouds) #declare sotd__Debug_clouds = 80; #end
#ifndef (sotd__Debug_shade) #declare sotd__Debug_shade = no; #end
#ifndef (sotd__Debug_unit) #declare sotd__Debug_unit = no; #end
#ifndef (sotd__Sky_trial) #declare sotd__Sky_trial = 1; #end

//==================== PUBLIC REFERENCE VALUES AND CODES =======================

#include "shapeoftheday_ref.inc"
#if (SHAPEOFTHEDAY__REF_VERSION != SHAPEOFTHEDAY_VERSION)
  #error concat
  ( "Version mismatch between shapeoftheday_ref.inc (",
    str (SHAPEOFTHEDAY__REF_VERSION, 0, 2), ") and shapeoftheday.inc (",
    str (SHAPEOFTHEDAY_VERSION, 0, 2), ")."
  )
#end

#declare SOTD_PERSPECTIVE = 1;
#declare SOTD_ORTHOGRAPHIC = 2;
#declare SOTD_ULTRA_WIDE_ANGLE = 5;

//========================= PRIVATE REFERENCE VALUES ===========================

#declare sotd__s_Settings = array[5]
{ "outdoors",
  "outdoor tiled",
  "patio with brick wall",
  "patio with railing",
  "indoors, windows "
}
#declare sotd__Settingxs = array[5]
{ SOTD_OUTDOORS, SOTD_OUTDOOR_CHECK,
  SOTD_OUTDOOR_PATIO, SOTD_OUTDOOR_PATIO_RAIL, SOTD_INDOORS_OPEN,
}
#declare sotd__s_Walls = array[5]
{ "blank wall",
  "door on left",
  "door on right",
  "",
  "windows"
}

#declare sotd__Units = array[14]
{ SOTD_INCH, SOTD_HAND, SOTD_FOOT, SOTD_YARD,
  SOTD_MM, SOTD_CM, SOTD_DM, SOTD_METER,
  10, 100, 1000, SOTD_FOOT * 16.5, SOTD_FOOT * 660, SOTD_FOOT * 5280,
}
#declare sotd__s_Units = array[14]
{ "inch", "hand", "foot", "yard",
  "millimeter", "centimeter", "decimeter", "meter",
  "decameter", "hectometer", "kilometer", "rod", "furlong", "mile",
}

//----------------- lighting and colors --------------------

// Light-and-shadow adjustments:
#declare SOTD__GRASS_SHADE = 0.975;
#declare SOTD__FAKE_FOREST_SHADE = (sotd__Debug_shade? 1: 0.55);

// Diffuse 1.0 pigment colors:
#macro Sotd__C2V (C) <C.red, C.green, C.blue> #end
#declare SOTD__C_BROADLEAF = Sotd__C2V (CHSV2RGB (<92, 0.80, 0.12>));
#declare SOTD__C_CONIFER = Sotd__C2V (CHSV2RGB (<115, 0.65, 0.08>));
#declare SOTD__C_GRASS = Sotd__C2V (CHSV2RGB (<91, 0.80, 0.18>));
#declare SOTD__C_DIRT = Sotd__C2V (CHSV2RGB (<23, 0.75, 0.12>));
#declare SOTD__C_BRICK = <0.6, 0.15, 0.15> * 0.6; //brick pigment default
#declare SOTD__C_MORTAR = <0.5, 0.5, 0.5> * 0.6; //brick pigment default

// Maximum light intensity, to avoid radiosity artifacts:
#declare SOTD__LIGHT_MAX = 20;
// Lumens at rgb 1.0:
#declare SOTD__LM_UNIT = 1600;
#declare SOTD__HOOD_FACTOR = 1.0201; // #debug'd from DeskLamp

// White point indices:
#declare SOTD__3200K = 0;
#declare SOTD__4100K = 1;
#declare SOTD__D50 = 2;
#declare SOTD__D55 = 3;
#declare SOTD__D65 = 4;
#declare SOTD__D75 = 5;
#declare SOTD__NWHITES = 6;

// Luminance functions by white point:
#declare sotd__v_Grays = array[SOTD__NWHITES]
{ <0.3571, 0.6194, 0.0235>,
  <0.2895, 0.6730, 0.0375>,
  <0.2500, 0.6978, 0.0522>,
  <0.2347, 0.7058, 0.0595>,
  <0.2126, 0.7152, 0.0722>, // ITU-R BT.709 for D65
  <0.1975, 0.7198, 0.0827>,
}

// Standard illuminants:
#declare SOTD__NILLUMS = 20;
#declare sotd__s_Illums = array[SOTD__NILLUMS]
{ "A", "B", "C", "D50", "D55", "D65", "D75", "E", "F1", "F2",
  "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
}
#declare sotd__Illum_codes = array[SOTD__NILLUMS]
{ SOTD_ILLUM_A, SOTD_ILLUM_B, SOTD_ILLUM_C, SOTD_ILLUM_D50, SOTD_ILLUM_D55,
  SOTD_ILLUM_D65, SOTD_ILLUM_D75, SOTD_ILLUM_E, SOTD_ILLUM_F1, SOTD_ILLUM_F2,
  SOTD_ILLUM_F3, SOTD_ILLUM_F4, SOTD_ILLUM_F5, SOTD_ILLUM_F6, SOTD_ILLUM_F7,
  SOTD_ILLUM_F8, SOTD_ILLUM_F9, SOTD_ILLUM_F10, SOTD_ILLUM_F11, SOTD_ILLUM_F12,
}
#declare SOTD__HSV_IX = 3; // D50 to be filtered by HSV bulbs
#declare sotd__c_Illums = array[SOTD__NWHITES][SOTD__NILLUMS]
{ { rgb <1.0986, 0.9540, 0.7160>, // black body 3200 K
    rgb <0.7434, 1.0983, 2.3075>,
    rgb <0.6260, 1.1254, 3.3772>,
    rgb <0.7001, 1.1268, 2.2161>,
    rgb <0.6573, 1.1397, 2.5269>,
    rgb <0.5953, 1.1548, 3.0692>,
    rgb <0.5531, 1.1622, 3.5146>,
    rgb <0.7174, 1.0951, 2.7893>,
    rgb <0.5685, 1.1767, 2.8995>,
    rgb <0.7985, 1.0886, 1.7299>,
    rgb <0.9393, 1.0285, 1.1712>,
    rgb <1.0744, 0.9638, 0.8237>,
    rgb <0.5453, 1.1963, 2.7353>,
    rgb <0.7841, 1.1058, 1.4953>,
    rgb <0.5957, 1.1548, 3.0646>,
    rgb <0.7006, 1.1266, 2.2128>,
    rgb <0.8199, 1.0754, 1.7499>,
    rgb <0.7000, 1.1271, 2.2105>,
    rgb <0.8417, 1.0672, 1.6342>,
    rgb <1.0543, 0.9751, 0.8329>,
  },
  { rgb <1.3552, 0.8779, 0.4489>, // daylight 4100 K
    rgb <0.9171, 1.0108, 1.4469>,
    rgb <0.7723, 1.0357, 2.1177>,
    rgb <0.8637, 1.0370, 1.3896>,
    rgb <0.8108, 1.0488, 1.5845>,
    rgb <0.7344, 1.0628, 1.9245>,
    rgb <0.6823, 1.0696, 2.2038>,
    rgb <0.8849, 1.0078, 1.7490>,
    rgb <0.7013, 1.0829, 1.8181>,
    rgb <0.9850, 1.0018, 1.0847>,
    rgb <1.1587, 0.9466, 0.7344>,
    rgb <1.3254, 0.8870, 0.5165>,
    rgb <0.6727, 1.1010, 1.7152>,
    rgb <0.9672, 1.0176, 0.9376>,
    rgb <0.7349, 1.0627, 1.9216>,
    rgb <0.8643, 1.0368, 1.3875>,
    rgb <1.0115, 0.9897, 1.0973>,
    rgb <0.8635, 1.0372, 1.3861>,
    rgb <1.0384, 0.9822, 1.0247>,
    rgb <1.3005, 0.8974, 0.5222>,
  },
  { rgb <1.5691, 0.8467, 0.3231>, // D50
    rgb <1.0618, 0.9748, 1.0413>,
    rgb <0.8942, 0.9988, 1.5240>,
    rgb <1.0000, 1.0000, 1.0000>,
    rgb <0.9388, 1.0115, 1.1403>,
    rgb <0.8503, 1.0249, 1.3850>,
    rgb <0.7900, 1.0315, 1.5860>,
    rgb <1.0246, 0.9719, 1.2587>,
    rgb <0.8120, 1.0443, 1.3084>,
    rgb <1.1404, 0.9661, 0.7806>,
    rgb <1.3416, 0.9128, 0.5285>,
    rgb <1.5346, 0.8554, 0.3717>,
    rgb <0.7789, 1.0617, 1.2344>,
    rgb <1.1199, 0.9814, 0.6748>,
    rgb <0.8509, 1.0248, 1.3830>,
    rgb <1.0007, 0.9999, 0.9985>,
    rgb <1.1711, 0.9544, 0.7897>,
    rgb <0.9998, 1.0003, 0.9975>,
    rgb <1.2023, 0.9472, 0.7375>,
    rgb <1.5058, 0.8654, 0.3758>,
  },
  { rgb <1.6715, 0.8371, 0.2833>, // D55
    rgb <1.1311, 0.9637, 0.9132>,
    rgb <0.9525, 0.9875, 1.3365>,
    rgb <1.0652, 0.9887, 0.8770>,
    rgb <1.0000, 1.0000, 1.0000>,
    rgb <0.9057, 1.0133, 1.2146>,
    rgb <0.8415, 1.0198, 1.3909>,
    rgb <1.0914, 0.9609, 1.1038>,
    rgb <0.8650, 1.0325, 1.1475>,
    rgb <1.2148, 0.9551, 0.6846>,
    rgb <1.4291, 0.9025, 0.4635>,
    rgb <1.6347, 0.8457, 0.3260>,
    rgb <0.8297, 1.0497, 1.0825>,
    rgb <1.1929, 0.9702, 0.5918>,
    rgb <0.9063, 1.0132, 1.2128>,
    rgb <1.0659, 0.9885, 0.8757>,
    rgb <1.2475, 0.9436, 0.6925>,
    rgb <1.0650, 0.9889, 0.8748>,
    rgb <1.2807, 0.9364, 0.6467>,
    rgb <1.6040, 0.8556, 0.3296>,
  },
  { rgb <1.8454, 0.8261, 0.2333>, // D65
    rgb <1.2488, 0.9511, 0.7518>,
    rgb <1.0516, 0.9745, 1.1004>,
    rgb <1.1761, 0.9757, 0.7220>,
    rgb <1.1041, 0.9869, 0.8233>,
    rgb <1.0000, 1.0000, 1.0000>,
    rgb <0.9291, 1.0064, 1.1451>,
    rgb <1.2050, 0.9483, 0.9088>,
    rgb <0.9550, 1.0190, 0.9447>,
    rgb <1.3412, 0.9426, 0.5636>,
    rgb <1.5778, 0.8907, 0.3816>,
    rgb <1.8048, 0.8346, 0.2684>,
    rgb <0.9160, 1.0360, 0.8912>,
    rgb <1.3170, 0.9575, 0.4872>,
    rgb <1.0007, 1.0000, 0.9985>,
    rgb <1.1769, 0.9756, 0.7210>,
    rgb <1.3773, 0.9312, 0.5702>,
    rgb <1.1758, 0.9760, 0.7202>,
    rgb <1.4139, 0.9242, 0.5325>,
    rgb <1.7709, 0.8444, 0.2714>,
  },
  { rgb <1.9861, 0.8208, 0.2037>, // D75
    rgb <1.3440, 0.9450, 0.6565>,
    rgb <1.1318, 0.9683, 0.9609>,
    rgb <1.2657, 0.9695, 0.6305>,
    rgb <1.1883, 0.9806, 0.7190>,
    rgb <1.0762, 0.9936, 0.8733>,
    rgb <1.0000, 1.0000, 1.0000>,
    rgb <1.2969, 0.9422, 0.7936>,
    rgb <1.0278, 1.0125, 0.8250>,
    rgb <1.4435, 0.9366, 0.4922>,
    rgb <1.6981, 0.8850, 0.3333>,
    rgb <1.9424, 0.8293, 0.2344>,
    rgb <0.9858, 1.0293, 0.7783>,
    rgb <1.4175, 0.9514, 0.4255>,
    rgb <1.0770, 0.9936, 0.8720>,
    rgb <1.2666, 0.9694, 0.6296>,
    rgb <1.4823, 0.9253, 0.4979>,
    rgb <1.2655, 0.9698, 0.6290>,
    rgb <1.5217, 0.9183, 0.4650>,
    rgb <1.9059, 0.8390, 0.2370>,
  },
}

//----------------- internal dimensions --------------------

#declare SOTD__FT_PER_IN = 1/12;
#declare SOTD__FT_PER_KM = 1000 / 0.3048;
#declare SOTD__CM_PER_IN = 2.54;
#declare SOTD__NBRICKS = 6;  // number of brick layers above checkered floor
#declare SOTD__RAMP = 12;    // 1 / ramp slope

// DeskLamp (cm)
#declare SOTD__RBULB = 3;                // light source recessed into hood
#declare SOTD__DLAMP = 5 * SOTD__CM_PER_IN - SOTD__RBULB; // light src to neck
#declare SOTD__ZLAMP = -SOTD__CM_PER_IN; // lamp neck off-center position
#declare SOTD__HLAMP = 30;               // minimum height
#declare SOTD__HRIGID =                  // rigid upper end of neck
  (sqrt (pow (1.25, 2) - pow (9/32, 2)) + 0.75) * SOTD__CM_PER_IN;
#declare SOTD__HFLEX = 8 * SOTD__CM_PER_IN; // flexible part of neck
#declare SOTD__HUPPER = SOTD__HRIGID + SOTD__HFLEX;
// DeskLamp (feet)
#declare SOTD__HFILL = 14 * SOTD__FT_PER_IN;

// Railings (inches)
#declare SOTD__RRAIL = 5/6;
#declare SOTD__RELBOW = 2.5;
#declare SOTD__XRAIL_RAMP = 20; // railing position left of ctr of level part
// Note: before increasing SOTD__RELBOW, check sotd_REnd in Sotd__Build_ramp().
#declare SOTD__HRAIL = 36;

// Dimensions below are all in feet.
// Walls
#declare SOTD__MICROABS = 0.001; // microspace, absolute scale
#declare SOTD__DWALL = 2/3;      // building wall diameter
#declare SOTD__DRAIL = 1/2;      // margin for railing around patio
#declare SOTD__HFOUNDATION = 0.75;
#declare SOTD__LBRICK = 2/3;     // brick length includes mortar
#declare SOTD__MORTAR = 3/8 * SOTD__FT_PER_IN;
#declare SOTD__HBRICK = 2.25 * SOTD__FT_PER_IN + SOTD__MORTAR;
#declare SOTD__HTOP = (2 - SOTD__MORTAR) * SOTD__FT_PER_IN;   //patio wall top
#declare SOTD__DTOPV = (1 - SOTD__MORTAR / 2) * SOTD__FT_PER_IN; // " overhang
#declare sotd__hBricks = SOTD__NBRICKS * SOTD__HBRICK + SOTD__MORTAR;
// Windows
#declare SOTD__HWINDOW = 40 * SOTD__FT_PER_IN;
#declare SOTD__WWINDOW = 36 * SOTD__FT_PER_IN;
#declare SOTD__WWINMINSPACE = 40 * SOTD__FT_PER_IN;
#declare SOTD__WWINFRAME = 3 * SOTD__FT_PER_IN;
#declare SOTD__YSILL = 40 * SOTD__FT_PER_IN;
#declare SOTD__HSTORYAVG = 12;
#declare SOTD__HSTORYMIN = 8;
// Doors @@ TO BE RE-PARADIGMED
#declare SOTD__WDOORWAY = 36.75 * SOTD__FT_PER_IN;
#declare SOTD__XDOORWAY = 3.5 * SOTD__FT_PER_IN;
#declare SOTD__HDOORWAY = 80.5 * SOTD__FT_PER_IN;
// Clouds
#declare SOTD__REARTH = 6371.0 * SOTD__FT_PER_KM; // average

//@@ FOR FUTURE USE:
// Doors
#declare SOTD__WPASSAGE = 3;
#declare SOTD__DBASEBOARD = 0.375 * SOTD__FT_PER_IN;

//================================= SETTINGS ===================================

/***********************************************************
 * Converts blur samples input to POV-Ray unit.
 */
#macro Sotd__Blur_samples (Input)
( #switch (Input)
    #case (1) Hex_Blur1 #break
    #case (2) Hex_Blur2 #break
    #case (3) Hex_Blur3 #break
    #else Input
  #end
)
#end

/***********************************************************
 * Extracts hue from input of format hhhsss.vvv.
 */
#macro Sotd__Decode_hue (Input)
  (floor (abs (Input) / 1000) * select (Input, -1, 1))
#end

/***********************************************************
 * Extracts saturation from input of format hhhsss.vvv.
 * sss has an implied decimal point after the first digit.
 */
#macro Sotd__Decode_saturation (Input)
  (floor (mod (abs (Input), 1000)) / 100)
#end

/***********************************************************
 * Extracts value from input of format hhhsss.vvv.
 * vvv has an implied decimal point after the first digit.
 */
#macro Sotd__Decode_value (Input)
  (mod (abs (Input), 1) * 10)
#end

/***********************************************************
 * Converts HSV values to input format, hhhsss.vvv.  sss and
 * vvv each have an implied decimal point after the first digit.
 */
#macro Sotd__Encode_color (Hue, Sat, Value)
  #local sotd_Hue = mod(Hue,360);
  #if (sotd_Hue < 0) #local sotd_Hue = sotd_Hue + 360; #end
  (   floor (sotd_Hue + 0.5) * 1000
    + floor (min(max(Sat,0),1) * 100 + 0.5)
    + min (max (Value * 0.1, 0), 0.999)
  )
#end

/***********************************************************
 * Converts HSV values or an illuminant code to input format,
 * hhhsss.vvv or -iii.lll.  lll, sss, and vvv each have an
 * implied decimal point after the first digit.
 */
#macro Sotd__Encode_light (Code, Hue, Sat, Value)
( #local sotd_I = Sotd__Illumindex (Code);
  #if (sotd_I < SOTD__NILLUMS)
    ceil (Code) - min (max (Value * 0.1, 0), 0.999) // standard illuminant
  #else
    Sotd__Encode_color (Hue, Sat, Value) // HSV
  #end
)
#end

/***********************************************************
 * Converts the white point global parameter to an index.
 */
#macro Sotd__Encode_white()
( #local sotd_hKs = array[SOTD__NWHITES] { 32, 41, 50, 55, 65, 75, }
  #local sotd_Bad = no;
  #if (sotd_White < 1000) #local sotd_P = sotd_White;
  #else
    #local sotd_Bad = yes;
    #local sotd_P = sotd_White / 100;
  #end
  #local sotd_Index = 0;
  #local sotd_Min_diff = abs (sotd_P - sotd_hKs[0]);
  #local sotd_I = 1;
  #while (sotd_I < SOTD__NWHITES)
    #local sotd_Diff = abs (sotd_P - sotd_hKs[sotd_I]);
    #if (sotd_Diff < sotd_Min_diff)
      #local sotd_Index = sotd_I;
      #local sotd_Min_diff = sotd_Diff;
    #end
    #local sotd_I = sotd_I + 1;
  #end
  #if (sotd_White != sotd_hKs [sotd_Index])
    #warning concat
    ( "Unrecognized white point: sotd_White = ", Sotd__Str(sotd_White,6),
      "; ", str (sotd_hKs [sotd_Index], 0, 0), " is substituted."
    )
  #end
  sotd_Index
)
#end

/***********************************************************
 * Estimates the flux of a light source on a surface.
 * Arguments:
 *   v_Light     - Location of light source
 *   v_Target    - Location of light source's target
 *   v_Surface   - Representative point on surface
 *   v_Normal    - Surface normal
 *   Is_parallel - If true, light rays are parallel, with no distance
 *                 attenuation; if false, the light is a 45-90 spotlight,
 *                 with inverse squared attenuation
 */
#macro Sotd__Estimate_flux (v_Light, v_Target, v_Surface, v_Normal, Is_parallel)
( #local sotd_v_From_target = vnormalize (v_Light - v_Target);
  #local sotd_v_From_surface = vnormalize (v_Light - v_Surface);
  #local sotd_InvSq =
    pow (vlength (v_Light - v_Target) / vlength (v_Light - v_Surface), 2);
  #if (Is_parallel)
    max (0, vdot (sotd_v_From_target, vnormalize (v_Normal)))
  #else
    #local sotd_AoI_ill = vdot (sotd_v_From_surface, vnormalize (v_Normal));
    #local sotd_Spread =
      degrees (acos (vdot (sotd_v_From_target, sotd_v_From_surface)));
    sotd_AoI_ill * sotd_InvSq
    #switch (sotd_Spread)
      #range (90, 180) * 0 #break
      #range (45, 90) * (90 - sotd_Spread) / 45 #break
    #end
  #end
)
#end

/***********************************************************
 * Returns a white point filter for an indoor light.
 *   IsStd      - Whether of not the light is a standard illuminant
 *   HasFixture - Whether or not the light has a fixture
 * Comments:
 *   Standard illuminants are pre-filtered, so don't need filtering.
 *   Sotd__Lamp() calls DeskLamp with a white point, so fixtured HSV lights
 *    don't need filtering either.
 */
#macro Sotd__Filter (IsStd, HasFixture)
( #if (IsStd | HasFixture) 1
  #else sotd__c_Illums [sotd__Whitex][SOTD__HSV_IX]
  #end
)
#end

/***********************************************************
 * Returns the color of an indoor light.
 *   Code  - The float-encoded HSV color or standard illuminant code
 *   Hue   - The hue of an HSV color
 *   Sat   - The saturation of an HSV color
 *   Value - The value of an HSV color or luminance of a standard illuminant
 * If Code represents a standard illuminant, then Value overrides
 * Code's luminance, and Hue and Sat are disregarded; otherwise,
 * Hue, Sat, and Value are used and Code is disregarded.
 */
#macro Sotd__Get_lamp (Code, Hue, Sat, Value)
( #local sotd_I = Sotd__Illumindex (Code);
  #if (sotd_I < SOTD__NILLUMS) // standard illuminant
    sotd__c_Illums [sotd__Whitex] [sotd_I] * Value * sotd_Exposure
  #else // HSV
    CHSV2RGB (<Hue, Sat, Value>) * sotd_Exposure
  #end
)
#end

/**************************************************************************
 * Returns the height of a story in feet.
 */
#macro Sotd__hStory_ft()
  (sotd_v_Room_size.y / (sotd__Scale * Sotd__NStories()))
#end

/**************************************************************************
 * Returns the height of a window in feet.
 */
#macro Sotd__hWindow_ft()
  (Sotd__hStory_ft() * SOTD__HWINDOW / SOTD__HSTORYMIN)
#end

/***********************************************************
 * Returns the array index for a standard illuminant.
 *   Code - The non-truncated illuminant code
 */
#macro Sotd__Illumindex (Code)
( #local sotd_Illum = SOTD__NILLUMS;
  #local sotd_I = 0;
  #while (sotd_I < SOTD__NILLUMS)
    #if (ceil (Code) = sotd__Illum_codes [sotd_I])
      #local sotd_Illum = sotd_I;
    #end
    #local sotd_I = sotd_I + 1;
  #end
  sotd_Illum
)
#end

/***********************************************************
 * Estimates the illumination of an indoor surface.
 * Arguments:
 *   v_Surface   - Representative point on surface
 *   v_Normal    - Surface normal
 * References several calculated global variables.
 */
#macro Sotd__Indoor_c (v_Surface, v_Normal)
(   Sotd__Estimate_flux
    ( sotd_v_Light, sotd_Height/2 * y, v_Surface, v_Normal, no
    ) * sotd_c_Light
  + Sotd__Estimate_flux
    ( sotd_v_Fill1, sotd_Height/2 * y, v_Surface, v_Normal, (sotd_Quality < 2)
    ) * sotd_c_Fill1
  + Sotd__Estimate_flux
    ( sotd_v_Fill2, sotd_Height/2 * y, v_Surface, v_Normal, (sotd_Quality < 2)
    ) * sotd_c_Fill2
)
#end

/**************************************************************************
 * Returns the number of stories.
 */
#macro Sotd__NStories()
  max
  ( floor
    ( (sotd_v_Room_size.y / sotd__Scale + (SOTD__HSTORYAVG - SOTD__HSTORYMIN))
        / SOTD__HSTORYAVG
    ),
    1
  )
#end

/**************************************************************************
 * Returns the number of windows per story per wall.
 */
#macro Sotd__NWindows()
  floor
  ( ( 2 * (sotd_v_Room_size.x / sotd__Scale + SOTD__DWALL - SOTD__WWINFRAME)
    - SOTD__WWINDOW + SOTD__WWINMINSPACE
    )
  / (SOTD__WWINDOW + SOTD__WWINMINSPACE)
  )
#end

/**************************************************************************
 * Reads user settings and sets defaults.
 * Argument:
 *   Uses_radiosity - If true, then all object-level ambients are set to zero
 */
#macro Sotd__Get_settings (Uses_radiosity)

 //~~~~~~~~~~~~~~~~~~~~~~~~~~~ PARAMETERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  #ifndef (sotd_Debug) #declare sotd_Debug = 0; #end
  #ifndef (sotd_Max_trace) #declare sotd_Max_trace = 15; #end

 //------------------ object positioning --------------------

  #ifndef (sotd_Pitch) #declare sotd_Pitch = 0; #end
  #ifndef (sotd_Roll) #declare sotd_Roll = 0; #end
  #ifndef (sotd_Yaw) #declare sotd_Yaw = 0; #end
  #ifndef (sotd_Nudge) #declare sotd_Nudge = 0; #end
  #ifndef (sotd_Lift) #declare sotd_Lift = 0; #end
  #ifndef (sotd_Push) #declare sotd_Push = 0; #end

 //------------------------ camera --------------------------

  #ifndef (sotd_Aperture) #declare sotd_Aperture = 0; #end
  #ifndef (sotd_Boom) #declare sotd_Boom = 0; #end
  #ifndef (sotd_hBoom) #declare sotd_hBoom = 0; #end
  #ifndef (sotd_Dolly) #declare sotd_Dolly = 0; #end
  #ifndef (sotd_Focus) #declare sotd_Focus = 0; #end
  #ifndef (sotd_FOV) #declare sotd_FOV = 20; #end
  #if (sotd_FOV >= 180 | sotd_FOV < 0)
    #error concat
    ( "sotd_FOV was set to ", str (sotd_FOV, 0, -1),
      "; it must be non-negative and less than 180."
    )
  #end
  #ifndef (sotd_Height) #declare sotd_Height = 2; #end
  #ifndef (sotd_Margin) #declare sotd_Margin = 0.5; #end
  #ifndef (sotd_Pan) #declare sotd_Pan = 0; #end
  #ifndef (sotd_Pixel_aspect) #declare sotd_Pixel_aspect = 1; #end
  #ifndef (sotd_Samples) #declare sotd_Samples = 2; #end
  #ifndef (sotd_Stereo) #declare sotd_Stereo = 0; #end
  #ifndef (sotd_Tilt) #declare sotd_Tilt = 0; #end
  //No default sotd_Confidence
  //No default sotd_Variance

 //----------------------- scenery --------------------------

  #ifndef (sotd_Cloudy) #declare sotd_Cloudy = 0.5; #end
  #ifndef (sotd_xClouds) #declare sotd_xClouds = 0; #end
  #ifndef (sotd_zClouds) #declare sotd_zClouds = 0; #end
  #ifndef (sotd_Quality) #declare sotd_Quality = 2; #end
  #ifndef (sotd_Setting) #declare sotd_Setting = SOTD_INDOORS_CLOSED; #end
  #switch (sotd_Setting)
    #case (SOTD_OUTDOORS)
    #case (SOTD_OUTDOOR_CHECK)
    #case (SOTD_OUTDOOR_PATIO)
    #case (SOTD_OUTDOOR_PATIO_RAIL)
    #range (SOTD_INDOORS_OPEN, SOTD_INDOORS_CLOSED)
      #break
    #else
      #error concat ("Unknown sotd_Setting ", str (sotd_Setting, 0, -1), ".")
  #end
  #ifndef (sotd_Unit) #declare sotd_Unit = SOTD_FOOT; #end
  #ifndef (sotd_Microspace)
    #declare sotd_Microspace = (sotd_Unit > 10? 0.01 / sotd_Unit: 0.001);
    // 0.001 is sufficient to avoid transparency artifacts.
  #end

 //++++++ floor tiles +++++++++
  #ifndef (sotd_Check1) #declare sotd_Check1 = 0.005; #end
  #ifndef (sotd_hCheck1)
    #declare sotd_hCheck1 = Sotd__Decode_hue (sotd_Check1);
  #end
  #ifndef (sotd_sCheck1)
    #declare sotd_sCheck1 = Sotd__Decode_saturation (sotd_Check1);
  #end
  #ifndef (sotd_vCheck1)
    #declare sotd_vCheck1 = Sotd__Decode_value (sotd_Check1);
  #end

  #ifndef (sotd_Check2) #declare sotd_Check2 = 0.1; #end
  #ifndef (sotd_hCheck2)
    #declare sotd_hCheck2 = Sotd__Decode_hue (sotd_Check2);
  #end
  #ifndef (sotd_sCheck2)
    #declare sotd_sCheck2 = Sotd__Decode_saturation (sotd_Check2);
  #end
  #ifndef (sotd_vCheck2)
    #declare sotd_vCheck2 = Sotd__Decode_value (sotd_Check2);
  #end
  #ifndef (sotd_Gloss) #declare sotd_Gloss = 0; #end
  #ifndef (sotd_Tile_spacing) #declare sotd_Tile_spacing = 0; #end

 //++++++++ ceiling +++++++++++
  #ifndef (sotd_Ceiling) #declare sotd_Ceiling = 0.1; #end
  #ifndef (sotd_hCeiling)
    #declare sotd_hCeiling = Sotd__Decode_hue (sotd_Ceiling);
  #end
  #ifndef (sotd_sCeiling)
    #declare sotd_sCeiling = Sotd__Decode_saturation (sotd_Ceiling);
  #end
  #ifndef (sotd_vCeiling)
    #declare sotd_vCeiling = Sotd__Decode_value (sotd_Ceiling);
  #end

 //+++++++++ walls ++++++++++++
  #ifndef (sotd_Back) #declare sotd_Back = SOTD_BLANK_WALL; #end
  #ifndef (sotd_Front) #declare sotd_Front = SOTD_BLANK_WALL; #end
  #ifndef (sotd_Left) #declare sotd_Left = SOTD_WINDOWS; #end
  #ifndef (sotd_Right) #declare sotd_Right = SOTD_DOOR_ON_RIGHT; #end
  #ifndef (sotd_Max_windows) #declare sotd_Max_windows = 20000; #end
  //Colors deferred until after room size is calculated:
  //  sotd_Wall
  //  sotd_hWall
  //  sotd_sWall
  //  sotd_vWall
  //  sotd_p_Wall
  //  sotd_t_Wall

 //+++++++++ hills ++++++++++++
  #ifndef (sotd_Hills_max) #declare sotd_Hills_max = 1500; #end
  #ifndef (sotd_Hills_res)
    #declare sotd_Hills_res =
      ceil (1000 - (pow (min (sotd_Hills_max / 1500, 1) - 1, 2) * 900));
  #end
  #ifndef (sotd_Hills_param) #declare sotd_Hills_param = 7; #end
  #ifndef (sotd_Trees_level) #declare sotd_Trees_level = 0; #end
  #ifndef (sotd_Trees_density) #declare sotd_Trees_density = 120; #end

 //-------------------- light sources -----------------------

 //+++++++++++ main lights ++++++++++++++
  #ifndef (sotd_Light) #declare sotd_Light = 0.1; #end
  #ifndef (sotd_hLight)
    #declare sotd_hLight = Sotd__Decode_hue (sotd_Light);
  #end
  #ifndef (sotd_sLight)
    #declare sotd_sLight = Sotd__Decode_saturation (sotd_Light);
  #end
  #ifndef (sotd_vLight)
    #declare sotd_vLight = Sotd__Decode_value (sotd_Light);
  #end
  #ifndef (sotd_Light_alt) #declare sotd_Light_alt = 45; #end
  #ifndef (sotd_Light_az) #declare sotd_Light_az = -30; #end
  #if (sotd_Setting < SOTD_INDOORS_OPEN)
    #ifndef (sotd_Sun_alt) #declare sotd_Sun_alt = sotd_Light_alt; #end
    #ifndef (sotd_Sun_az) #declare sotd_Sun_az = sotd_Light_az; #end
  #else
    #ifndef (sotd_Sun_alt) #declare sotd_Sun_alt = 45; #end
    #ifndef (sotd_Sun_az) #declare sotd_Sun_az = -30; #end
  #end

 //+++++++++++ fill lights ++++++++++++++
  #ifndef (sotd_Fill)
    #declare sotd_Fill = Sotd__Encode_light
    ( sotd_Light, sotd_hLight, sotd_sLight, sotd_vLight * 0.1
    );
  #end
  #ifndef (sotd_hFill)
    #declare sotd_hFill = Sotd__Decode_hue (sotd_Fill);
  #end
  #ifndef (sotd_sFill)
    #declare sotd_sFill = Sotd__Decode_saturation (sotd_Fill);
  #end
  #ifndef (sotd_vFill)
    #declare sotd_vFill = Sotd__Decode_value (sotd_Fill);
  #end
  #local sotd_True_fill =
    Sotd__Encode_light (sotd_Fill, sotd_hFill, sotd_sFill, sotd_vFill);

  #ifndef (sotd_Fill1) #declare sotd_Fill1 = sotd_True_fill; #end
  #ifndef (sotd_hFill1)
    #declare sotd_hFill1 = Sotd__Decode_hue (sotd_Fill1);
  #end
  #ifndef (sotd_sFill1)
    #declare sotd_sFill1 = Sotd__Decode_saturation (sotd_Fill1);
  #end
  #ifndef (sotd_vFill1)
    #declare sotd_vFill1 = Sotd__Decode_value (sotd_Fill1);
  #end

  #ifndef (sotd_Fill2) #declare sotd_Fill2 = sotd_True_fill; #end
  #ifndef (sotd_hFill2)
    #declare sotd_hFill2 = Sotd__Decode_hue (sotd_Fill2);
  #end
  #ifndef (sotd_sFill2)
    #declare sotd_sFill2 = Sotd__Decode_saturation (sotd_Fill2);
  #end
  #ifndef (sotd_vFill2)
    #declare sotd_vFill2 = Sotd__Decode_value (sotd_Fill2);
  #end

  #ifndef (sotd_Fill_area) #declare sotd_Fill_area = no; #end
  #ifndef (sotd_Fill_ph) #declare sotd_Fill_ph = no; #end

 //------------------- general lighting ---------------------

  #ifndef (sotd_Area) #declare sotd_Area = 0; #end
  #ifndef (sotd_Area_res) #declare sotd_Area_res = 9; #end
  #ifndef (sotd_Area_adaptive) #declare sotd_Area_adaptive = 0; #end
  #ifndef (sotd_Exposure) #declare sotd_Exposure = 1; #end
  #ifndef (sotd_Ph) #declare sotd_Ph = 0; #end
  #ifndef (sotd_Ph_autostop) #declare sotd_Ph_autostop = 0; #end
  #ifndef (sotd_Ph_load) #declare sotd_Ph_load = no; #end
  #ifndef (sotd_Ph_media) #declare sotd_Ph_media = 0; #end
  #ifndef (sotd_Ph_media_factor) #declare sotd_Ph_media_factor = 0; #end
  #ifndef (sotd_Ph_name) #declare sotd_Ph_name = "sotd_"; #end
  #ifndef (sotd_Ph_num) #declare sotd_Ph_num = frame_number; #end
  #ifndef (sotd_Ph_radius2) #declare sotd_Ph_radius2 = 0; #end
  #ifndef (sotd_Ph_radius4) #declare sotd_Ph_radius4 = 0; #end
  #ifndef (sotd_Ph_save) #declare sotd_Ph_save = no; #end
  #ifndef (sotd_Reflect) #declare sotd_Reflect = no; #end
  #ifndef (sotd_White) #declare sotd_White = 50; #end
  #declare sotd__Whitex = Sotd__Encode_white();
  //Parameters deferred until after room size is calculated:
  //  sotd_Max_sample
  //  sotd_Solar

 //~~~~~~~~~~~~~~~~~~~~~~~ CALCULATED VALUES ~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Artificial structures are measured in feet:
  #declare sotd__Scale = SOTD_FOOT / sotd_Unit;

 //------------------------ camera --------------------------

 //+++++++++++++ position +++++++++++++++
  #declare sotd_Camera_distance =
    sotd_Height / (2 * tan (radians ((sotd_FOV? sotd_FOV: 20) / 2)));
  #declare sotd_Dolly_final =
    sotd_Dolly + degrees (sotd_Stereo / sotd_Camera_distance);
  #declare sotd_Boom_final =
    sotd_Boom + degrees (sotd_hBoom / sotd_Camera_distance);
  #declare sotd_v_Camera_location = vtransform
  ( -sotd_Camera_distance * z,
    transform
    { rotate <sotd_Boom_final, -sotd_Dolly_final, 0>
      translate sotd_Height/2 * y
    }
  );
  #declare sotd_v_Camera_look_at = vtransform
  ( sotd_Camera_distance * z,
    transform
    { rotate
      < sotd_Boom_final - degrees (sotd_Tilt/sotd_Camera_distance),
        degrees (sotd_Pan/sotd_Camera_distance) - sotd_Dolly_final,
        0
      >
      translate sotd_v_Camera_location
    }
  );
  #declare sotd_v_Focus = sotd_v_Camera_look_at
    + sotd_Focus * vnormalize (sotd_v_Camera_look_at - sotd_v_Camera_location);
 //++++++++++++ projection ++++++++++++++
  #declare sotd_Lens = SOTD_PERSPECTIVE;
  #declare sotd_Camera_aspect =
    image_width / (sotd_Pixel_aspect * image_height);
  #declare sotd_Camera_up = 1;
  #declare sotd_Camera_right = sotd_Camera_aspect;
  #declare sotd_Camera_angle = degrees
  ( atan2
    ( sotd_Height * (1 + sotd_Margin) * image_width / image_height,
      2 * sotd_Camera_distance
    )
  ) * 2 / sotd_Pixel_aspect;
 //If screen.inc is not used
 //  Use orthographic camera if FOV is 0
 //  Use ultra-wide angle camera if wider than a threshold
  #ifndef (Screen_Inc_Temp)
    #if (sotd_FOV = 0)
      #declare sotd_Lens = SOTD_ORTHOGRAPHIC;
      #declare sotd_Camera_up = sotd_Height * (1 + sotd_Margin);
      #declare sotd_Camera_right = sotd_Camera_up * sotd_Camera_aspect;
      #declare sotd_Camera_angle = 0;
    #else #if
    (   max (sotd_Camera_angle, sotd_Camera_angle * image_height / image_width)
      > degrees (2 * atan2 (2, 3))
    )
      #declare sotd_Lens = SOTD_ULTRA_WIDE_ANGLE;
      #declare sotd_Camera_angle = sotd_FOV
        * (1 + sotd_Margin) * image_width / (image_height * sotd_Pixel_aspect);
    #end
    #end
  #end

 //----------------------- scenery --------------------------

  #local SOTD_V_LAMP_SPACE = <1, 1.5, 1> * sotd__Scale;
  #local SOTD_RBIGGER_THAN_BULB =
    (19/8) / 2 * SOTD__FT_PER_IN * 1.1 * sotd__Scale; // 19 = A19
  #if (sotd__Debug_unit)
    Sotd__DebugV ("SOTD_V_LAMP_SPACE", SOTD_V_LAMP_SPACE, 6)
    Sotd__DebugF ("SOTD_RBIGGER_THAN_BULB", SOTD_RBIGGER_THAN_BULB, 6)
  #end
  #local Sotd_fn_Ceil = function (x) // Rounds to the next higher feet.
  { ceil (x / sotd__Scale) * sotd__Scale
  }
  #local sotd__RRoom = Sotd_fn_Ceil
  ( max
    ( sotd_Height/2 + SOTD_V_LAMP_SPACE.x,
      sotd_Camera_distance + SOTD_V_LAMP_SPACE.x,
      6 * sotd__Scale
    )
  );
  #declare sotd_v_Room_size =
  < sotd__RRoom,
    Sotd_fn_Ceil
    ( max
      ( sotd_Height + SOTD_V_LAMP_SPACE.y,
        sotd_Height/2 + sotd_Camera_distance + SOTD_V_LAMP_SPACE.y,
        SOTD__HSTORYMIN * sotd__Scale
      )
    ),
    sotd__RRoom
  >;
  #declare sotd_c_Check1 =
    CHSV2RGB (<sotd_hCheck1, sotd_sCheck1, sotd_vCheck1>);
  #declare sotd_c_Check2 =
    CHSV2RGB (<sotd_hCheck2, sotd_sCheck2, sotd_vCheck2>);
  #declare sotd_c_Ceiling =
    CHSV2RGB (<sotd_hCeiling, sotd_sCeiling, sotd_vCeiling>);
 //++++++ wall: ambient estimate ++++++++
 // sotd_c_Wallpaper = average color of wallpaper
  #ifdef (sotd_p_Wall)
    #local sotd_fn_Wall = function { pigment { sotd_p_Wall } }
    #local sotd_Rnd = seed(0);
    #local sotd_c_Wallpaper = color 0;
    #local sotd_I = 0;
    #while (sotd_I < 1000)
      #local sotd_c_Wallpaper = sotd_c_Wallpaper + sotd_fn_Wall
      ( (rand (sotd_Rnd) * 2 - 1) * sotd_v_Room_size.x,
        rand (sotd_Rnd) * sotd_v_Room_size.y,
        0
      );
      #local sotd_I = sotd_I + 1;
    #end
    #local sotd_c_Wallpaper = sotd_c_Wallpaper / 1000;
    #ifndef (sotd_Wall)
      #local sotd_hsv_Wp = CRGB2HSV (sotd_c_Wallpaper);
      #declare sotd_Wall =
        Sotd__Encode_color (sotd_hsv_Wp.x, sotd_hsv_Wp.y, sotd_hsv_Wp.z);
      #if (sotd_Debug >= 1)
        #debug "ShapeOfTheDay: sotd_Wall is estimated from sotd_p_Wall.\n"
      #end
    #end
  #end
 //+++++++++ wall: parameters +++++++++++
  #ifndef (sotd_Wall)
    #if (sotd_Debug >= 1) #debug "ShapeOfTheDay: defaulting sotd_Wall.\n" #end
    #declare sotd_Wall = 0.1;
  #end
  #ifndef (sotd_hWall)
    #declare sotd_hWall = Sotd__Decode_hue (sotd_Wall);
  #end
  #ifndef (sotd_sWall)
    #declare sotd_sWall = Sotd__Decode_saturation (sotd_Wall);
  #end
  #ifndef (sotd_vWall)
    #declare sotd_vWall = Sotd__Decode_value (sotd_Wall);
  #end
  //No default sotd_p_Wall
  //No default sotd_t_Wall
 //+++++ wall: calculated values ++++++++
  #declare sotd_c_Wall =
    CHSV2RGB (<sotd_hWall, sotd_sWall, sotd_vWall>);
  #if (sotd_Setting < SOTD_INDOORS_OPEN)
    #local sotd_Brick_part =
      (SOTD__LBRICK - SOTD__MORTAR) * (SOTD__HBRICK - SOTD__MORTAR)
    / (SOTD__LBRICK * SOTD__HBRICK) / (sotd__hBricks + SOTD__HTOP);
    #local sotd_c_Wallpaper =
      SOTD__C_BRICK * sotd_Brick_part + SOTD__C_MORTAR * (1 - sotd_Brick_part);
  #else #ifndef (sotd_c_Wallpaper)
    #local sotd_c_Wallpaper = sotd_c_Wall;
  #end
  #end

 //-------------------- light sources -----------------------

  // unfiltered by white point
  #declare sotd__c_Light_unf =
    Sotd__Get_lamp (sotd_Light, sotd_hLight, sotd_sLight, sotd_vLight);
  #declare sotd__c_Fill1_unf =
    Sotd__Get_lamp (sotd_Fill1, sotd_hFill1, sotd_sFill1, sotd_vFill1);
  #declare sotd__c_Fill2_unf =
    Sotd__Get_lamp (sotd_Fill2, sotd_hFill2, sotd_sFill2, sotd_vFill2);
  // filtered by white point
  #declare sotd_c_Light =
    sotd__c_Light_unf * Sotd__Filter ((sotd_Light < 0), no);
  #declare sotd_c_Fill1 =
    sotd__c_Fill1_unf * Sotd__Filter ((sotd_Fill1 < 0), no);
  #declare sotd_c_Fill2 =
    sotd__c_Fill2_unf * Sotd__Filter ((sotd_Fill2 < 0), no);
  #declare sotd__Sun_dist =
    max (1000 * sotd_v_Room_size.y, 200 * sotd_Hills_max / sotd_Unit, 1000000);
  #if (sotd__Debug_unit)
    Sotd__DebugF ("sotd__Sun_dist", sotd__Sun_dist, 6)
  #end
  #declare sotd_v_Sun =
    vrotate (-sotd__Sun_dist * z, <sotd_Sun_alt, -sotd_Sun_az, 0>);
  #local sotd_Lamp_clip = box
  { SOTD_V_LAMP_SPACE - SOTD_RBIGGER_THAN_BULB - sotd_v_Room_size,
    sotd_v_Room_size - SOTD_V_LAMP_SPACE + SOTD_RBIGGER_THAN_BULB
  }
  #declare sotd_v_Light = trace
  ( sotd_Lamp_clip, sotd_Height/2 * y,
    vrotate (-z, <sotd_Light_alt, -sotd_Light_az, 0>)
  );
  #declare sotd_v_Fill1 = trace
  ( sotd_Lamp_clip, SOTD__HFILL * sotd__Scale * y,
    vrotate (-z, (-sotd_Light_az + 120) * y)
  );
  #declare sotd_v_Fill2 = trace
  ( sotd_Lamp_clip, SOTD__HFILL * sotd__Scale * y,
    vrotate (-z, (-sotd_Light_az - 120) * y)
  );

 //---------------- brightness calibration ------------------

  #local sotd_Dist = vlength (sotd_v_Light - sotd_Height/2 * y) * sotd_Unit;
  #local sotd_Lux =
    SOTD__LM_UNIT * (1 + SOTD__HOOD_FACTOR) / (4 * pi * pow (sotd_Dist, 2));
  #local sotd_Solar_lux = 87799; // sqrt(2) air mass assumed
  #declare Lamp_Lumen = sotd_Exposure / sotd_Lux;
  #ifndef (sotd_Solar)
    #declare sotd_Solar =
      (sotd_Setting < SOTD_INDOORS_OPEN? 1: sotd_Solar_lux / sotd_Lux);
  #end
  #ifndef (sotd_Max_sample)
    #declare sotd_Max_sample =
    ( sotd_Setting < SOTD_INDOORS_OPEN | sotd_Setting = SOTD_INDOORS_CLOSED?
      SOTD__LIGHT_MAX: max (SOTD__LIGHT_MAX, sotd_Solar * 0.4)
    ) * sotd_Exposure;
  #end

 //----------------- outdoor light colors -------------------

  // Sky values are sampled from ShapeOfTheDay outdoor test scenes.
  #switch (sotd__Whitex)
    #case (SOTD__3200K)
      #declare sotd_c_Sun = rgb <0.727, 1.115, 2.119>;
      #local sotd_c_Sky = rgb <0.073, 0.163, 0.597>;
      #break
    #case (SOTD__4100K)
      #declare sotd_c_Sun = rgb <0.897, 1.026, 1.329>;
      #local sotd_c_Sky = rgb <0.090, 0.150, 0.374>;
      #break
    #case (SOTD__D50)
      #declare sotd_c_Sun = rgb <1.038, 0.989, 0.956>;
      #local sotd_c_Sky = rgb <0.104, 0.144, 0.269>;
      #break
    #case (SOTD__D55)
      #declare sotd_c_Sun = rgb <1.106, 0.978, 0.839>;
      #local sotd_c_Sky = rgb <0.111, 0.143, 0.236>;
      #break
    #case (SOTD__D65)
      #declare sotd_c_Sun = rgb <1.221, 0.965, 0.690>;
      #local sotd_c_Sky = rgb <0.122, 0.141, 0.195>;
      #break
    #case (SOTD__D75)
      #declare sotd_c_Sun = rgb <1.314, 0.959, 0.603>;
      #local sotd_c_Sky = rgb <0.132, 0.140, 0.170>;
      #break
    #else
      #error concat ("Bug: sotd__Whitex = ", str (sotd__Whitex, 0, -1))
  #end
  #declare sotd__Outdoor_bright = sotd_Solar * sotd_Exposure;
  #declare sotd_c_Sun = sotd_c_Sun * sotd__Outdoor_bright;
  #local sotd_c_Sky = sotd_c_Sky * sotd__Outdoor_bright;

 //------------------- general lighting ---------------------

  #declare sotd_s_Photons_file =
    concat (sotd_Ph_name, str (sotd_Ph_num,1,0), ".ph")
  #ifndef (sotd_Diffuse) #declare sotd_Diffuse = 0.6; #end
  #if (Uses_radiosity)
    #declare sotd_c_Ground_ambient = rgb 0;
    #local sotd_c_Outdoor_ambient = rgb 0;
    #local sotd_c_Forest_ambient = rgb 0;
    #declare sotd_c_Ambient = rgb 0;
  #else

   //+++ Estimate some ambient values +++++

    // Ideally, proportional area of a square projected onto a sphere.
    // Actually, just an estimate.  (It's correct at 3 boundary conditions.)
    //   sotd_P0_Side   - length of side
    //   sotd_P1_Offset - offset of the square from the center of the sphere
    #local Sotd_fn_Proj_area = function (sotd_P0_Side, sotd_P1_Offset)
    { abs(sotd_P0_Side) / (2 * abs(sotd_P0_Side) + 8 * abs(sotd_P1_Offset))
    }
    //For some ambients, an ad hoc compensation for lack of ambient occlusion,
    // at the expense of overall brightness:
    #local SOTD_OCCLUSION = 0.8;

   // Prefix convention: A = area, c_F = flux, c_I = illuminated average color
    #declare sotd_c_Ground_ambient = sotd_c_Sky;
    #local sotd_c_Fground =
       Sotd__Estimate_flux (sotd_v_Sun, 0, 0, y, yes) * sotd_c_Sun
     + sotd_c_Ground_ambient;
    #if (sotd_Setting = SOTD_OUTDOOR_CHECK)
      #local sotd_c_Iground =
        sotd_c_Fground * (sotd_c_Check1 + sotd_c_Check2) / 2 * sotd_Diffuse;
    #else
      // sotd_Dirt_est is a guess.
      #local sotd_Dirt_est = (sotd_Setting < SOTD_OUTDOOR_PATIO? 0.25: 0.05);
      #local sotd_c_Iground = sotd_c_Fground *
      (  SOTD__C_GRASS * SOTD__GRASS_SHADE * (1 - sotd_Dirt_est)
       + SOTD__C_DIRT * sotd_Dirt_est
      );
    #end
    #local sotd_c_Itop = sotd_c_Fground * SOTD__C_MORTAR;
    #local sotd_c_Ifloor =
      sotd_c_Fground * (sotd_c_Check1 + sotd_c_Check2) / 2 * sotd_Diffuse;
    #declare sotd_c_Ground_ambient = sotd_c_Ground_ambient * SOTD_OCCLUSION;
    #local sotd_c_Outdoor_ambient = (sotd_c_Sky + sotd_c_Iground) / 2;
    #local sotd_c_Forest_ambient = sotd_c_Ground_ambient; //@@PLACE HOLDER!

    #local sotd_Afloor = Sotd_fn_Proj_area
      (sotd_v_Room_size.x * 2, sotd_Height/2);

    #switch (sotd_Setting)

      #case (SOTD_OUTDOORS)
      #case (SOTD_OUTDOOR_CHECK)
        #declare sotd_c_Ambient = sotd_c_Outdoor_ambient * SOTD_OCCLUSION;
        #break

      #case (SOTD_OUTDOOR_PATIO_RAIL)
        #local sotd_Afloor = Sotd_fn_Proj_area
          ((sotd_v_Room_size.x + SOTD__DRAIL) * 2, sotd_Height/2);
        #local sotd_Asky = 0.5;
        #local sotd_Aground = 0.5 - sotd_Afloor;
        #declare sotd_c_Ambient =
        (  sotd_Asky * sotd_c_Sky
         + sotd_Aground * sotd_c_Iground
         + sotd_Afloor * sotd_c_Ifloor
        ) * SOTD_OCCLUSION;
        #break

      #case (SOTD_OUTDOOR_PATIO)
        #local sotd_hPatio = sotd__hBricks + SOTD__HTOP;
       //== exposure to look_at =====
        #if (sotd_Height/2 > sotd_hPatio) //ground is visible above walls
          #local sotd_Asky = 0.5;
          #local sotd_Aouter = Sotd_fn_Proj_area
          ( (sotd_v_Room_size.x + SOTD__DWALL + SOTD__DTOPV) * 2,
            sotd_Height/2 - sotd_hPatio
          );
          #local sotd_Ainner = Sotd_fn_Proj_area
          ( (sotd_v_Room_size.x - SOTD__DTOPV) * 2,
            sotd_Height/2 - sotd_hPatio
          );
          #local sotd_Aground = 0.5 - sotd_Aouter;
          #local sotd_Atop = sotd_Aouter - sotd_Ainner; // top of walls
          #local sotd_Awalls = (sotd_Ainner - sotd_Afloor); // sides of walls
        #else //ground is hidden
          #local sotd_Asky = Sotd_fn_Proj_area
          ( (sotd_v_Room_size.x - SOTD__DTOPV) * 2,
            sotd_hPatio - sotd_Height/2
          );
          #local sotd_Awalls = (1 - sotd_Asky - sotd_Afloor);
          #local sotd_Atop = 0;
          #local sotd_Aground = 0;
        #end
       //== exposure from walls =====
        #local sotd_yMid = sotd_hPatio / 2;
        #local sotd_c_Iwalls =
        (  (  Sotd__Estimate_flux
                (sotd_v_Sun, 0, <0, sotd_yMid, sotd_v_Room_size.z>, -z, yes)
            + Sotd__Estimate_flux
                (sotd_v_Sun, 0, <sotd_v_Room_size.x, sotd_yMid, 0>, -x, yes)
            + Sotd__Estimate_flux
                (sotd_v_Sun, 0, <0, sotd_yMid, -sotd_v_Room_size.z>, z, yes)
            + Sotd__Estimate_flux
                (sotd_v_Sun, 0, <-sotd_v_Room_size.x, sotd_yMid, 0>, x, yes)
           ) / 4 * sotd_c_Sun
         + (sotd_c_Sky + sotd_c_Ifloor) / 2 //rough cut ambient
        ) * sotd_c_Wallpaper;
       //==== general ambient =======
        #declare sotd_c_Ambient =
        (  sotd_Asky * sotd_c_Sky
         + sotd_Aground * sotd_c_Iground
         + sotd_Atop * sotd_c_Itop
         + sotd_Awalls * sotd_c_Iwalls
         + sotd_Afloor * sotd_c_Ifloor
        ) * SOTD_OCCLUSION;
        #break

      #range (SOTD_INDOORS_OPEN, SOTD_INDOORS_CLOSED)
        #local sotd_Aceiling = Sotd_fn_Proj_area
        ( sotd_v_Room_size.x * 2, sotd_v_Room_size.y - sotd_Height/2
        );
        #local sotd_Awall = (1 - sotd_Afloor - sotd_Aceiling) / 4;
        #local sotd_Awins = max
        ( (1 - cos (min (0.5, SOTD_INDOORS_CLOSED - sotd_Setting) * pi))
            * Sotd__NWindows() * SOTD__WWINDOW * SOTD__HWINDOW * sotd__Scale
            / (2 * sotd_v_Room_size.x * SOTD__HSTORYMIN),
          0
        );
        #declare sotd_c_Ambient =
          Sotd__Indoor_c (0, y) * (sotd_c_Check1 + sotd_c_Check2) / 2
            * sotd_Diffuse * sotd_Afloor
        + Sotd__Indoor_c (sotd_v_Room_size * y, -y) * sotd_c_Ceiling
            * sotd_Diffuse * sotd_Aceiling
        + (  Sotd__Indoor_c (<0, sotd_v_Room_size.y/2, sotd_v_Room_size.z>, -z)
               * (1 - (floor(sotd_Front) = SOTD_WINDOWS) * sotd_Awins)
           + Sotd__Indoor_c (<sotd_v_Room_size.x, sotd_v_Room_size.y/2, 0>, -x)
               * (1 - (floor(sotd_Right) = SOTD_WINDOWS) * sotd_Awins)
           + Sotd__Indoor_c (<0, sotd_v_Room_size.y/2, -sotd_v_Room_size.z>, z)
               * (1 - (floor(sotd_Back) = SOTD_WINDOWS) * sotd_Awins)
           + Sotd__Indoor_c (<-sotd_v_Room_size.x, sotd_v_Room_size.y/2, 0>, x)
               * (1 - (floor(sotd_Left) = SOTD_WINDOWS) * sotd_Awins)
          ) * sotd_c_Wallpaper * sotd_Diffuse * sotd_Awall
        + (  (floor(sotd_Front) = SOTD_WINDOWS)
           + (floor(sotd_Right) = SOTD_WINDOWS)
           + (floor(sotd_Back) = SOTD_WINDOWS)
           + (floor(sotd_Left) = SOTD_WINDOWS)
          ) * sotd_Awins * sotd_c_Outdoor_ambient * sotd_Awall;
        #if (sotd__Debug_ambient)
          #debug concat ( "Setting ", str (sotd_Setting, 0, 4), "\n")
          Sotd__DebugF ("sotd_Aceiling", sotd_Aceiling, 4)
          Sotd__DebugF ("sotd_Awall", sotd_Awall, 4)
          Sotd__DebugF ("sotd_Awins", sotd_Awins, 4)
          Sotd__DebugV ("sotd_c_Ambient rgb", sotd_c_Ambient, 4)
        #end
        #break

      #else // should never get here
        #declare sotd_c_Ambient = rgb 0.1;
        #warning concat ("Unknown sotd_Setting ", str (sotd_Setting, 0, 6), ".")

    #end // #switch (sotd_Setting)

  #end // estimate ambient colors

  #declare sotd__mmPerUnit = 1000 * sotd_Unit;
  #default
  { finish
    { diffuse sotd_Diffuse ambient sotd_c_Ambient * sotd_Diffuse
      #if (version >= 3.7) emission 0 #end
    }
  }
  #declare sotd__f_Default1 = finish
  { diffuse 1 ambient sotd_c_Ambient
    #if (version >= 3.7) emission 0 #end
  }
  #declare sotd__f_Ground = finish
  { diffuse sotd_Diffuse ambient sotd_c_Ground_ambient * sotd_Diffuse
    #if (version >= 3.7) emission 0 #end
  }
  #declare sotd__f_Ground1 = finish
  { diffuse 1 ambient sotd_c_Ground_ambient
    #if (version >= 3.7) emission 0 #end
  }
  #declare sotd__f_Forest1 = finish
  { diffuse 1 ambient sotd_c_Forest_ambient
    #if (version >= 3.7) emission 0 #end
  }
  #declare sotd__f_Outdoor = finish
  { diffuse sotd_Diffuse ambient sotd_c_Outdoor_ambient * sotd_Diffuse
    #if (version >= 3.7) emission 0 #end
  }

#end //macro Sotd__Get_settings

//============================== ECHO SETTINGS =================================

/***********************************************************
 * Writes an annotated float value to the debug stream.
 */
#macro Sotd__DebugF (s_Name, Value, Sig)
  #debug concat
  ( "@@ ", s_Name,
    #if (!strcmp (strupr (substr (concat (s_Name, "...."), 1, 4)), "SOTD"))
      " = "
    #else
      " "
    #end,
    Sotd__Str (Value, Sig), "\n"
  )
#end

/***********************************************************
 * Writes an annotated vector to the debug stream.
 */
#macro Sotd__DebugV (s_Name, v_Value, Sig)
  #debug concat
  ( "@@ ", s_Name,
    #if (!strcmp (strupr (substr (concat (s_Name, "...."), 1, 4)), "SOTD"))
      " = "
    #else
      " "
    #end,
    Sotd__VStr (v_Value, Sig), "\n"
  )
#end

/***********************************************************
 * Writes HSV color inputs to debug stream.
 */
#macro Sotd__Echo_color_input (s_Name, Code, Hue, Sat, Value)
  #debug concat
  ( "  ", s_Name, " = ", str(Code,-8,3), ", hsv <",
    str(Hue,0,0), ", ", str(Sat,0,2), ", ", str(Value,0,4), ">\n"
  )
#end

/***********************************************************
 * Writes standard illuminant or HSV color inputs to debug stream.
 */
#macro Sotd__Echo_light_input (s_Name, Code, Hue, Sat, Value)
  #local sotd_I = Sotd__Illumindex (Code);
  #if (sotd_I < SOTD__NILLUMS) // standard illuminant
    #debug concat
    ( "  ", s_Name, " = ", str(Code,0,3), ", L ", str(Value,0,4),
      ": illuminant ", sotd__s_Illums [sotd_I], "\n"
    )
  #else // HSV
    Sotd__Echo_color_input (s_Name, Code, Hue, Sat, Value)
  #end
#end

/***********************************************************
 * Writes a light source declaration to the debug stream.
 * Arguments:
 *   v_Where - The light's location
 *   v_Aim   - The light's point_at
 *   c_Color - The light's color
 *   IsFill  - true = shadowless; ignore remaining arguments
 *             false = shadows; implement remaining arguments
 *   rFade   - The light's fade distance
 *   Radius  - Spotlight radius, or 0 for parallel light
 *   Tight   - Spotlight tightness
 *   dArea   - Area light diameter
 *   Photons - Whether or not to use photons
 */
#macro Sotd__Echo_light_source
( v_Where, v_Aim, c_Color, IsFill, rFade, Radius, Tight, dArea, Photons
)
  #debug concat
  ( "light_source\n{ ", Sotd__VStr (v_Where, 5), ", rgb ",
    Sotd__VStr (c_Color, 4), "\n"
  )
  #if (IsFill)
    #debug concat
    ( "  parallel point_at ", Sotd__VStr (v_Aim, 5), "\n  shadowless\n"
    )
  #else
    #if (Tight = 0)
      #debug "  parallel point_at "
      #debug Sotd__VStr (v_Aim, 5)
      #debug "\n"
    #else
      #debug concat
      ( "  fade_power 2 fade_distance ", Sotd__Str (rFade, 5),
        "\n  spotlight point_at ", Sotd__VStr (v_Aim, 5),
        "\n  radius ", Sotd__Str (Radius, 5),
        " falloff 90 tightness ", Sotd__Str (Tight, 5), "\n"
      )
    #end
    #if (dArea)
      #debug concat
      ( "  area_light ", Sotd__Str (dArea, 5), " * x, ", Sotd__Str (dArea, 5),
        " * z, ", str (sotd_Area_res, 0, 0), ", ", str (sotd_Area_res, 0, 0),
        "\n  circular orient adaptive ", str (sotd_Area_adaptive, 0, 0),
        " jitter\n"
      )
    #end
  #end
  #if
  ( !Photons | IsFill |
    (c_Color.red = 0 & c_Color.green = 0 & c_Color.blue = 0)
  )
    #debug "  photons { reflection off refraction off }\n"
  #end
  #debug "}\n"
#end

/***********************************************************
 * Writes derived RGB color to debug stream.
 */
#macro Sotd__Echo_rgb (s_Name, c_Color)
  #debug concat ("  ", s_Name, " = rgb ", Sotd__VStr (c_Color, 4), "\n")
#end

/***********************************************************
 * Writes input for wall features to debug stream.
 */
#macro Sotd__Echo_wall (s_Name, Code)
  #debug concat
  ( "  ", s_Name, " = ", Sotd__Str (Code, 6), " (",
    #if (Code < 0 | Code >= dimension_size (sotd__s_Walls,1))
      "undefined",
    #else #if (!strcmp (sotd__s_Walls [Code], ""))
      "undefined",
    #else
      sotd__s_Walls [Code],
    #end
    #end
    ")\n"
  )
#end

/***********************************************************
 * Echoes the unit of length parameter; if it represents a recognized
 * unit of length, the name of the unit is also echoed.
 */
#macro Sotd__Echo_unit (Param)
  // Function returns true if x and y are equivalent
  // to within z significant figures:
  #local Sotd_fn_Compare = function
  { select (0.5 - abs (x - y) * pow (10, z - floor(log(x)) - 1), 0, 1)
  }
  #local sotd_U = -1;
  #local sotd_I = 0;
  #while (sotd_I < dimension_size (sotd__Units, 1))
    #if (Sotd_fn_Compare (sotd__Units [sotd_I], Param, 4))
      #local sotd_U = sotd_I;
    #end
    #local sotd_I = sotd_I + 1;
  #end
  #debug concat
  ( "  Unit = ", Sotd__Str(sotd_Unit,6), " (",
    #if (sotd_U < 0) "unrecognized" #else sotd__s_Units [sotd_U], #end
    ")\n"
  )
#end

/***********************************************************
 * Writes an annotated Boolean value to the debug stream.
 */
#macro Sotd__EchoB (s_Name, Value)
  #debug
    concat ("  ", s_Name, " = ", #if (Value) "true" #else "false" #end, "\n")
#end

/***********************************************************
 * Writes an annotated float value to the debug stream.
 * If Sig is positive then it is significant figures;
 * otherwise it is decimal places.
 */
#macro Sotd__EchoF (s_Name, Value, Sig, Newline)
  #debug concat
  ( "  ", s_Name, " = ",
    #if (Sig > 0) Sotd__Str (Value, Sig) #else str (Value, 0, -Sig) #end
    #if (Newline) , "\n" #end
  )
#end

/***********************************************************
 * Writes an annotated float value to the debug stream.
 */
#macro Sotd__EchoV (s_Name, v_Value, Sig)
  #debug concat ("  ", s_Name, " = ", Sotd__VStr (v_Value, Sig), "\n")
#end

/***********************************************************
 * Converts a number to a string of so many significant digits.
 */
#macro Sotd__Str (Number, Sig)
  #if (Number < 0 | Number > 0) //This circumlocution avoids the epsilon.
    str (Number, 1, max (Sig - floor(log(abs(Number))) - 1, 0))
  #else
    str (0, 1, Sig - 1)
  #end
#end

/***********************************************************
 * Converts a vector to a string of so many significant digits.
 */
#macro Sotd__VStr (v_Vector, Sig)
  #local sotd_Max = max (abs (v_Vector.x), abs (v_Vector.y), abs (v_Vector.z));
  #if (sotd_Max = 0)
    concat ("<", vstr (3, v_Vector, ", ", 0, Sig - 1), ">")
  #else
    concat
    ( "<",
      vstr (3, v_Vector, ", ", 0, max (Sig - floor(log(sotd_Max)) - 1, 0)),
      ">"
    )
  #end
#end

/**************************************************************************
 * Writes scene parameters and values to the debug stream.
 * Arguments:
 *   Echo           - If true, writes the input parameters and calculated
 *                    values; otherwise, writes only the calculated values
 *   Uses_radiosity - If true, and not indoors with windows closed, then
 *                    radiosity media on is echoed
 */
#macro Sotd__Debug (Echo, Uses_radiosity)
  #if (Echo)
    #debug "ShapeOfTheDay: Positioning Parameters\n"
    Sotd__EchoF ("Pitch", sotd_Pitch, 6, no)
    Sotd__EchoF ("Roll", sotd_Roll, 6, no)
    Sotd__EchoF ("Yaw", sotd_Yaw, 6, yes)
    Sotd__EchoF ("Nudge", sotd_Nudge, 6, no)
    Sotd__EchoF ("Lift", sotd_Lift, 6, no)
    Sotd__EchoF ("Push", sotd_Push, 6, yes)
    #debug "ShapeOfTheDay: Scene Setting Parameters\n"
    Sotd__EchoF ("Aperture", sotd_Aperture, 6, yes)
    Sotd__EchoF ("Area", sotd_Area, 6, yes)
    Sotd__EchoF ("Area_adaptive", sotd_Area_adaptive, 6, yes)
    Sotd__EchoF ("Area_res", sotd_Area_res, 6, yes)
    Sotd__Echo_wall ("Back", sotd_Back)
    Sotd__EchoF ("Boom", sotd_Boom, 6, yes)
    Sotd__EchoF ("hBoom", sotd_hBoom, 6, yes)
    Sotd__Echo_color_input
      ("Ceiling", sotd_Ceiling, sotd_hCeiling, sotd_sCeiling, sotd_vCeiling)
    Sotd__Echo_color_input
      ("Check1", sotd_Check1, sotd_hCheck1, sotd_sCheck1, sotd_vCheck1)
    Sotd__Echo_color_input
      ("Check2", sotd_Check2, sotd_hCheck2, sotd_sCheck2, sotd_vCheck2)
    Sotd__EchoF ("xClouds", sotd_xClouds, 6, yes)
    Sotd__EchoF ("zClouds", sotd_zClouds, 6, yes)
    Sotd__EchoF ("Cloudy", sotd_Cloudy, 6, yes)
    #ifdef (sotd_Confidence)
      Sotd__EchoF ("Confidence", sotd_Confidence, 6, yes)
    #else
      #debug "  Confidence is undefined.\n"
    #end
    Sotd__EchoF ("Debug", sotd_Debug, 6, yes)
    Sotd__EchoF ("Diffuse", sotd_Diffuse, 6, yes)
    Sotd__EchoF ("Dolly", sotd_Dolly, 6, yes)
    Sotd__EchoF ("Exposure", sotd_Exposure, 6, yes)
    Sotd__Echo_light_input
      ("Fill", sotd_Fill, sotd_hFill, sotd_sFill, sotd_vFill)
    Sotd__Echo_light_input
      ("Fill1", sotd_Fill1, sotd_hFill1, sotd_sFill1, sotd_vFill1)
    Sotd__Echo_light_input
      ("Fill2", sotd_Fill2, sotd_hFill2, sotd_sFill2, sotd_vFill2)
    Sotd__EchoB ("Fill_area", sotd_Fill_area)
    Sotd__EchoB ("Fill_ph", sotd_Fill_ph)
    Sotd__EchoF ("Focus", sotd_Focus, 6, yes)
    Sotd__EchoF ("FOV", sotd_FOV, 6, yes)
    Sotd__Echo_wall ("Front", sotd_Front)
    Sotd__EchoF ("Gloss", sotd_Gloss, 6, yes)
    Sotd__EchoF ("Height", sotd_Height, 6, yes)
    Sotd__EchoF ("Hills_max", sotd_Hills_max, 6, yes)
    Sotd__EchoF ("Hills_param", sotd_Hills_param, 6, yes)
    Sotd__EchoF ("Hills_res", sotd_Hills_res, 6, yes)
    Sotd__Echo_wall ("Left", sotd_Left)
    Sotd__Echo_light_input
      ("Light", sotd_Light, sotd_hLight, sotd_sLight, sotd_vLight)
    Sotd__EchoF ("Light_alt", sotd_Light_alt, 6, yes)
    Sotd__EchoF ("Light_az", sotd_Light_az, 6, yes)
    Sotd__EchoF ("Margin", sotd_Margin, 6, yes)
    Sotd__EchoF ("Max_sample", sotd_Max_sample, 6, yes)
    Sotd__EchoF ("Max_trace", sotd_Max_trace, 6, yes)
    Sotd__EchoF ("Max_windows", sotd_Max_windows, 6, yes)
    Sotd__EchoF ("Microspace", sotd_Microspace, 6, yes)
    Sotd__EchoF ("Pan", sotd_Pan, 6, yes)
    Sotd__EchoF ("Ph", sotd_Ph, 6, yes)
    Sotd__EchoF ("Ph_autostop", sotd_Ph_autostop, 6, yes)
    Sotd__EchoB ("Ph_load", sotd_Ph_load)
    Sotd__EchoF ("Ph_media", sotd_Ph_media, 6, yes)
    Sotd__EchoF ("Ph_media_factor", sotd_Ph_media_factor, 6, yes)
    #debug concat ("  Ph_name = \"", sotd_Ph_name, "\"\n")
    Sotd__EchoF ("Ph_num", sotd_Ph_num, 6, yes)
    Sotd__EchoF ("Ph_radius2", sotd_Ph_radius2, 6, yes)
    Sotd__EchoF ("Ph_radius4", sotd_Ph_radius4, 6, yes)
    Sotd__EchoB ("Ph_save", sotd_Ph_save)
    Sotd__EchoF ("Pixel_aspect", sotd_Pixel_aspect, 6, yes)
    Sotd__EchoF ("Reflect", sotd_Reflect, 6, yes)
    Sotd__EchoF ("Quality", sotd_Quality, 6, yes)
    Sotd__Echo_wall ("Right", sotd_Right)
    Sotd__EchoF ("Samples", sotd_Samples, 6, yes)
    #debug concat
    ( "  Setting = ", Sotd__Str(sotd_Setting,6), " (",
      #local sotd_Sx = 0;
      #while (sotd_Sx < dimension_size (sotd__s_Settings, 1))
        #if (min (sotd_Setting, SOTD_INDOORS_OPEN) = sotd__Settingxs [sotd_Sx])
          sotd__s_Settings [sotd_Sx],
          #if (sotd_Setting > SOTD_INDOORS_OPEN + 0.5)
            str ((sotd_Setting - SOTD_INDOORS_OPEN) * 100, 0, 3), "% closed",
          #else #if (sotd_Setting >= SOTD_INDOORS_OPEN)
            str ((SOTD_INDOORS_CLOSED - sotd_Setting) * 100, 0, 3), "% open",
          #end
          #end
        #end
        #local sotd_Sx = sotd_Sx + 1;
      #end
      ")\n"
    )
    Sotd__EchoF ("Solar", sotd_Solar, 6, yes)
    Sotd__EchoF ("Stereo", sotd_Stereo, 6, yes)
    Sotd__EchoF ("Sun_alt", sotd_Sun_alt, 6, yes)
    Sotd__EchoF ("Sun_az", sotd_Sun_az, 6, yes)
    Sotd__EchoF ("Tile_spacing", sotd_Tile_spacing, 6, yes)
    Sotd__EchoF ("Tilt", sotd_Tilt, 6, yes)
    Sotd__EchoF ("Trees_density", sotd_Trees_density, 6, yes)
    Sotd__EchoF ("Trees_level", sotd_Trees_level, 6, yes)
    Sotd__Echo_unit (sotd_Unit)
    #ifdef (sotd_Variance)
      Sotd__EchoF ("Variance", sotd_Variance, 6, yes)
    #else
      #debug "  Variance is undefined.\n"
    #end
    Sotd__Echo_color_input
      ("Wall", sotd_Wall, sotd_hWall, sotd_sWall, sotd_vWall)
    #debug concat ("  p_Wall is ", #ifndef(sotd_p_Wall) "un", #end "defined.\n")
    #debug concat ("  t_Wall is ", #ifndef(sotd_t_Wall) "un", #end "defined.\n")
    Sotd__EchoF ("White", sotd_White, 6, yes)
    #debug concat
    ( "  File pipemrg.inc is ", #if (!sotd__PipeMrg) "not ", #end "detected.\n"
    )
  #end //#if (Echo)
 //----------------------------------------------------------
  #debug "ShapeOfTheDay: Scene Settings\n"
  #ifdef (Screen_Inc_Temp)
    #debug "  Set_Camera ("
    #debug Sotd__VStr (sotd_v_Camera_location, 5 )
    #debug ", "
    #debug Sotd__VStr (sotd_v_Camera_look_at, 5)
    #debug ", "
    #debug str (sotd_Camera_angle, 0, 4)
    #debug ")\n  Set_Camera_Aspect_Ratio ("
    #debug str (sotd_Camera_aspect, 0, 4)
    #debug ")\n"
  #else
    #debug "  camera\n  { "
    #switch (sotd_Lens)
      #case (SOTD_ORTHOGRAPHIC) #debug "orthographic\n    " #break
      #case (SOTD_ULTRA_WIDE_ANGLE) #debug "ultra_wide_angle\n    " #break
    #end
    #debug concat ("location ", Sotd__VStr (sotd_v_Camera_location, 5), "\n")
    #debug concat ("    look_at ", Sotd__VStr(sotd_v_Camera_look_at, 5), "\n")
    #debug concat ("    right ", str (sotd_Camera_right, 0, 4), " * x\n")
    #debug concat ("    up ", str (sotd_Camera_up, 0, 4), " * y\n")
    #if (sotd_Lens != SOTD_ORTHOGRAPHIC)
      #debug concat ("    angle ", str(sotd_Camera_angle, 0, 4), "\n")
    #end
    #if (sotd_Aperture)
      #debug concat ("    aperture ", Sotd__Str (sotd_Aperture, 4), "\n")
      #debug concat
      ( "    focal_point ", Sotd__VStr (sotd_v_Focus, 5), "\n"
      )
      #debug concat
      ( "    blur_samples ",
        str (floor (Sotd__Blur_samples (sotd_Samples)), 0, 0), "\n"
      )
      #ifdef (sotd_Confidence)
        #debug concat ("    confidence ", str (sotd_Confidence, 0, 4), "\n")
      #end
      #ifdef (sotd_Variance)
        #debug concat ("    variance ", Sotd__Str (sotd_Variance, 4), "\n")
      #end
    #end
    #debug "  }\n"
  #end
  #debug "  #default\n"
  #debug concat
  ( "  { finish\n    { diffuse ", str(sotd_Diffuse,0,5), " ambient rgb ",
    Sotd__VStr (sotd_c_Ambient * sotd_Diffuse, 5),
    #if (version >= 3.7) "\n      emission 0", #end
    "\n    }\n"
  )
  #debug "  }\n"
  #debug "  global_settings\n"
  #debug "  { assumed_gamma 1\n"
  #debug
    concat ("    max_trace_level ", str (floor (sotd_Max_trace), 0, 0), "\n")
  #if (sotd_Ph > 0)
    #debug "    photons\n"
    #debug concat
    ( "    { "
      #if (sotd_Ph > 1)
        "count ", str (floor (sotd_Ph), 0, 0),
      #else
        "spacing ", Sotd__Str (sotd_Ph, 4),
      #end
      "\n"
    )
    #debug concat ("      autostop ", Sotd__Str (sotd_Ph_autostop, 4), "\n")
    #if (sotd_Ph_media)
      #debug concat
      ( "      media ", str (floor (sotd_Ph_media), 0, 0),
        #if (sotd_Ph_media_factor)
          ", ", Sotd__Str (sotd_Ph_media_factor, 4),
        #end
        "\n"
      )
    #end
    #if (sotd_Ph_radius2 | sotd_Ph_radius4)
      #debug concat
      ( "      radius , ",
        #if (sotd_Ph_radius2) Sotd__Str (sotd_Ph_radius2, 4), #end
        #if (sotd_Ph_radius4)
          ", , ", Sotd__Str (sotd_Ph_radius4, 4),
        #end
        "\n"
      )
    #end
    #if (sotd_Ph_load)
      #debug concat ("      load_file \"", sotd_s_Photons_file, "\"\n")
    #end
    #if (sotd_Ph_save)
      #debug concat ("      save_file \"", sotd_s_Photons_file, "\"\n")
    #end
    #debug "    }\n"
  #end
  #if (Uses_radiosity)
    #debug "    radiosity\n"
    #debug concat ("    { max_sample ", Sotd__Str (sotd_Max_sample, 4), "\n")
    #if (sotd_Setting < SOTD_INDOORS_CLOSED) #debug "      media on\n" #end
    #debug "    }\n"
  #end
  #if (version >= 3.7)
    #debug concat ("    mm_per_unit ", Sotd__Str (sotd__mmPerUnit, 6), "\n")
  #end
  #debug "  }\n"
 //----------------------------------------------------------
  #debug "ShapeOfTheDay: Other Calculated Values\n"
  Sotd__EchoF ("Boom_final", sotd_Boom_final, -4, no)
  Sotd__EchoF ("Camera_distance", sotd_Camera_distance, 5, no)
  Sotd__EchoF ("Dolly_final", sotd_Dolly_final, -4, yes)
  Sotd__Echo_rgb ("c_Ambient", sotd_c_Ambient)
  Sotd__Echo_rgb ("c_Ceiling", sotd_c_Ceiling)
  Sotd__Echo_rgb ("c_Check1", sotd_c_Check1)
  Sotd__Echo_rgb ("c_Check2", sotd_c_Check2)
  Sotd__Echo_rgb ("c_Fill1", sotd_c_Fill1)
  Sotd__Echo_rgb ("c_Fill2", sotd_c_Fill2)
  Sotd__Echo_rgb ("c_Ground_ambient", sotd_c_Ground_ambient)
  Sotd__Echo_rgb ("c_Light", sotd_c_Light)
  Sotd__Echo_rgb ("c_Sun", sotd_c_Sun)
  Sotd__Echo_rgb ("c_Wall", sotd_c_Wall)
  Sotd__EchoV ("v_Fill1", sotd_v_Fill1, 5)
  Sotd__EchoV ("v_Fill2", sotd_v_Fill2, 5)
  Sotd__EchoV ("v_Light", sotd_v_Light, 5)
  Sotd__EchoV ("v_Room_size", sotd_v_Room_size, 5)
  Sotd__EchoV ("v_Sun", sotd_v_Sun, 5)
 //----------------------------------------------------------
  #debug "ShapeOfTheDay: Light Sources\n"
#end //macro Sotd__Debug

//============================= AUXILIARY MACROS ===============================

/***********************************************************
 * Returns an adjustment factor for the diameter of an area light.
 * Argument:
 *   x - the resolution of the area light
 */
#declare Sotd__fn_Area_adj = function (x) { (x - 1) / x }

/***********************************************************
 * Returns a brick wall object topped with concrete, on the +z side
 * of the patio, with an entrance and ramp if indicated.  The bricks
 * are untextured, but the remainder of the object is textured.
 * Argument:
 *   Feature - a flag for door or windows (the request for windows is ignored)
 * Implementation note: construction is initially in feet, then scaled to the
 * user-chosen unit.
 */
#macro Sotd__Build_patio_wall (Feature)
  #local SOTD_FUZZ = 0.006; // same as normal accuracy?
  #local sotd_xWall = sotd_v_Room_size.x / sotd__Scale;
  #local sotd_xOuter = sotd_xWall + SOTD__LBRICK;
  #local sotd_rMortar = SOTD__MORTAR / 2;
 // opening starts at a brick surface:
  #local sotd_wBrick = SOTD__LBRICK / 2;
  #local sotd_xOpen = sotd_xWall - sotd_wBrick;
  #local sotd_xOpen2 = sotd_xOpen + sotd_rMortar;
 // opening must be >= 3 feet, but also end at a brick surface:
  #local sotd_xOpen1 =
    sotd_xOpen - ceil (3 / sotd_wBrick) * sotd_wBrick - sotd_rMortar;
  #local sotd_v_Ramp =
    <(sotd_xOpen1 + sotd_xOpen2) / 2, 0, SOTD__LBRICK - sotd_rMortar>;
  union
  { prism //bricks
    { -SOTD__HFOUNDATION - sotd_Microspace / sotd__Scale, sotd__hBricks,
      #switch (Feature)
        #case (SOTD_ENTRANCE_ON_LEFT)
        #case (SOTD_ENTRANCE_ON_RIGHT)
          10,
          <sotd_xWall + sotd_rMortar, sotd_rMortar>,
          <sotd_xOuter - sotd_rMortar, SOTD__LBRICK - sotd_rMortar>,
          <sotd_xOpen2 + SOTD_FUZZ, SOTD__LBRICK - sotd_rMortar>,
          <sotd_xOpen2 + SOTD_FUZZ, sotd_rMortar>,
          <sotd_xWall + sotd_rMortar, sotd_rMortar>,
          <-sotd_xWall - sotd_rMortar, sotd_rMortar>,
          <sotd_xOpen1 - SOTD_FUZZ, sotd_rMortar>,
          <sotd_xOpen1 - SOTD_FUZZ, SOTD__LBRICK - sotd_rMortar>,
          <sotd_rMortar - sotd_xOuter, SOTD__LBRICK - sotd_rMortar>,
          <-sotd_xWall - sotd_rMortar, sotd_rMortar>
          #if (Feature = SOTD_ENTRANCE_ON_LEFT) scale <-1, 1, 1> #end
          #break
        #else
          5,
          <sotd_xWall + sotd_rMortar, sotd_rMortar>,
          <sotd_xOuter - sotd_rMortar, SOTD__LBRICK - sotd_rMortar>,
          <sotd_rMortar - sotd_xOuter, SOTD__LBRICK - sotd_rMortar>,
          <-sotd_xWall - sotd_rMortar, sotd_rMortar>,
          <sotd_xWall + sotd_rMortar, sotd_rMortar>
      #end
    }
   // concrete top
    #local sotd_t_Concrete = texture
    { object
      { plane { -y, -sotd__hBricks - SOTD__HTOP }
        texture
        { pigment { rgb SOTD__C_MORTAR }
          finish { sotd__f_Default1 }
        }
        texture
        { pigment { rgb SOTD__C_MORTAR }
          finish { sotd__f_Ground1 }
        }
      }
    }
    #switch (Feature)
      #case (SOTD_ENTRANCE_ON_LEFT)
        box
        { <SOTD__DTOPV - sotd_xWall, sotd__hBricks, -SOTD__DTOPV>,
          <-sotd_xOpen2, sotd__hBricks + SOTD__HTOP, SOTD__LBRICK + SOTD__DTOPV>
          texture  { sotd_t_Concrete }
        }
        box
        { <-sotd_xOpen1, sotd__hBricks, -SOTD__DTOPV>,
          < sotd_xOuter + SOTD__DTOPV,
            sotd__hBricks + SOTD__HTOP,
            SOTD__LBRICK + SOTD__DTOPV
          >
          texture  { sotd_t_Concrete }
        }
        object { Sotd__Build_ramp() translate sotd_v_Ramp scale <-1, 1, 1> }
        #break
      #case (SOTD_ENTRANCE_ON_RIGHT)
        box
        { <SOTD__DTOPV - sotd_xWall, sotd__hBricks, -SOTD__DTOPV>,
          <sotd_xOpen1, sotd__hBricks + SOTD__HTOP, SOTD__LBRICK + SOTD__DTOPV>
          texture  { sotd_t_Concrete }
        }
        box
        { <sotd_xOpen2, sotd__hBricks, -SOTD__DTOPV>,
          < sotd_xOuter + SOTD__DTOPV,
            sotd__hBricks + SOTD__HTOP,
            SOTD__LBRICK + SOTD__DTOPV
          >
          texture  { sotd_t_Concrete }
        }
        object { Sotd__Build_ramp() translate sotd_v_Ramp }
        #break
      #else
        box
        { <SOTD__DTOPV - sotd_xWall, sotd__hBricks, -SOTD__DTOPV>,
          < sotd_xOuter + SOTD__DTOPV,
            sotd__hBricks + SOTD__HTOP,
            SOTD__LBRICK + SOTD__DTOPV
          >
          texture  { sotd_t_Concrete }
        }
    #end
    scale sotd__Scale
    translate sotd_v_Room_size * z
  }
#end // Sotd__Build_patio_wall

/***********************************************************
 * Returns a railing.
 * Argument:
 *   Feature - a flag for door or windows (the request for windows is ignored)
 * Implementation note: construction is initially in inches, scaled to feet,
 * then scaled to the user-chosen unit.
 */
#macro Sotd__Build_railing (Feature)
  Sotd__Make_railing_parts()
  #local sotd_zRail = sotd_v_Room_size.z / sotd__Scale * 12
    + SOTD__DRAIL * 12 - SOTD__RELBOW;
  #local sotd_xRail = sotd_zRail - SOTD__RELBOW;
  #local sotd_Post =
    #if (sotd__PipeMrg)
      union
      { object { sotd__Post }
        object { sotd__Tjunction translate SOTD__HRAIL * y }
        object { sotd__Crossing translate SOTD__HRAIL / 2 * y }
      }
    #else
      sotd__Post
    #end
  #local sotd_Rails = union
  { object { sotd__Elbow translate <sotd_zRail, SOTD__HRAIL, sotd_zRail> }
    object { sotd__Elbow translate <sotd_zRail, SOTD__HRAIL / 2, sotd_zRail> }
    #switch (Feature)
      #case (SOTD_ENTRANCE_ON_LEFT)
      #case (SOTD_ENTRANCE_ON_RIGHT)
        #local sotd_xMatch = sotd_xRail - SOTD__RELBOW;
        #local sotd_Span = sotd_zRail + sotd_xMatch - 2 * SOTD__XRAIL_RAMP;
        #local sotd_N = ceil (sotd_Span / 36 + 0.5);
        #local sotd_Step = sotd_Span / (sotd_N - 0.5);
        #if (sotd__Debug_unit)
          Sotd__DebugF ("accessible railing sotd_Span", sotd_Span, 5)
          Sotd__DebugF ("accessible railing sotd_N", sotd_N, 5)
          Sotd__DebugF ("accessible railing sotd_Step", sotd_Step, 5)
        #end
        union
        { object
          { sotd__Elbow rotate 180 * y
            translate <sotd_xRail - SOTD__RELBOW, SOTD__HRAIL, sotd_zRail>
          }
          object
          { sotd__Elbow rotate 180 * y
            translate <sotd_xRail - SOTD__RELBOW, SOTD__HRAIL / 2, sotd_zRail>
          }
          #local sotd_I = 0.5;
          #while (sotd_I < sotd_N - 1)
            object
            { sotd_Post
              translate <sotd_I * sotd_Step - sotd_zRail, 0, sotd_zRail>
            }
            #local sotd_I = sotd_I + 1;
          #end
          object
          { sotd__End_post
            rotate -90 * y
            translate <sotd_I * sotd_Step - sotd_zRail, 0, sotd_zRail>
          }
          cylinder
          { -sotd_xRail * x, (sotd_Span - sotd_zRail - SOTD__RELBOW) * x,
            SOTD__RRAIL
            translate <0, SOTD__HRAIL, sotd_zRail>
          }
          cylinder
          { -sotd_xRail * x, (sotd_Span - sotd_zRail) * x, SOTD__RRAIL
            translate <0, SOTD__HRAIL / 2, sotd_zRail>
          }
          #if (Feature = SOTD_ENTRANCE_ON_LEFT) scale <-1, 1, 1> #end
        }
        #break
      #else
        cylinder
        { -sotd_xRail * x, sotd_xRail * x, SOTD__RRAIL
          translate <0, SOTD__HRAIL, sotd_zRail>
        }
        cylinder
        { -sotd_xRail * x, sotd_xRail * x, SOTD__RRAIL
          translate <0, SOTD__HRAIL / 2, sotd_zRail>
        }
        #local sotd_N = ceil (sotd_zRail / 18);
        #local sotd_Step = 2 * sotd_zRail / sotd_N;
        #if (sotd__Debug_unit)
          Sotd__DebugF ("railing sotd_N", sotd_N, 5)
          Sotd__DebugF ("railing sotd_Step", sotd_Step, 5)
        #end
        #local sotd_I = 0.5;
        #while (sotd_I < sotd_N)
          object
          { sotd_Post
            translate <sotd_I * sotd_Step - sotd_zRail, 0, sotd_zRail>
          }
          #local sotd_I = sotd_I + 1;
        #end
    #end
    texture { sotd__t_Rail }
    scale SOTD__FT_PER_IN
  }
  #switch (Feature)
    #case (SOTD_ENTRANCE_ON_LEFT)
    #case (SOTD_ENTRANCE_ON_RIGHT)
      union
      { object { sotd_Rails }
        object
        { Sotd__Build_ramp()
          translate
            < (sotd_xMatch - SOTD__XRAIL_RAMP) * SOTD__FT_PER_IN,
              0,
              sotd_v_Room_size.z / sotd__Scale + SOTD__DRAIL
            >
          #if (Feature = SOTD_ENTRANCE_ON_LEFT) scale <-1, 1, 1> #end
        }
        scale sotd__Scale
      }
      #break
    #else
      object { sotd_Rails scale sotd__Scale }
  #end
#end // Sotd__Build_railing

/***********************************************************
 * Returns a ramp object, turning to the left as one leaves the building.
 * Implementation note: construction is initially in inches, then scaled
 * to feet.
 */
#macro Sotd__Build_ramp()

  #ifndef (sotd__Ramp)
    #local sotd_aRail = atan2 (1, SOTD__RAMP);
    #local sotd_adRail = degrees (sotd_aRail);
    #local sotd_hRamp = SOTD__HFOUNDATION / SOTD__FT_PER_IN;
    #local sotd_rLevel = SOTD__XRAIL_RAMP + 2; // half-width of level part
    #local sotd_lRamp = sotd_hRamp * SOTD__RAMP;
    #local sotd_dRamp = 40;
    #local sotd_xPost1 = SOTD__XRAIL_RAMP - 6;
    #local sotd_xPost2 = 2 - sotd_rLevel - sotd_lRamp;
    #local sotd_zRail = sotd_dRamp - 2;
    #local sotd_zEnd = 0.5; // upper end
    #local sotd_wEnd = 7;   // lower end
    #local sotd_REnd = 18;  // lower end rail curvature

    Sotd__Make_railing_parts()
    #if (sotd__PipeMrg)
      #local sotd_Skewed_xing = union
      { PipeMrg (0, SOTD__RRAIL, 0.2, 90 - sotd_adRail, sotd_adRail * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90 + sotd_adRail, 90 * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90 - sotd_adRail, (180 + sotd_adRail) * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90 + sotd_adRail, 270 * z)
      }
      #local sotd_Skewed_T = union
      { PipeMrg (0, SOTD__RRAIL, 0.2, 90 - sotd_adRail, (180 + sotd_adRail) * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90 + sotd_adRail, 270 * z)
      }
    #end
    #local sotd_Test_top = plane
    { y, SOTD__HRAIL
      rotate sotd_adRail * z
      translate -sotd_rLevel * x
    }
    // endpoints of inclined rails:
    #local sotd_Test_end = plane { x, -sotd_rLevel - sotd_lRamp }
    #local sotd_v_Top1 = vrotate (SOTD__HRAIL * y, sotd_adRail * z)
      + <-sotd_rLevel, 0, sotd_zRail>;
    #local sotd_v_Mid1 = vrotate (SOTD__HRAIL / 2 * y, sotd_adRail * z)
      + <-sotd_rLevel, 0, sotd_zRail>;
    #local sotd_v_Top2 =
      trace (sotd_Test_end, sotd_v_Top1, <-SOTD__RAMP, -1, 0>);
    #local sotd_v_Mid2 =
      trace (sotd_Test_end, sotd_v_Mid1, <-SOTD__RAMP, -1, 0>);
    //
    #local sotd_Ramp_surface = prism
    { 0, sotd_dRamp, 5,
      <sotd_rLevel, 0>,
      <sotd_rLevel, sotd_hRamp>,
      <-sotd_rLevel - sotd_lRamp, sotd_hRamp>,
      <-sotd_rLevel, 0>,
      <sotd_rLevel, 0>
      rotate 90 * x
      texture
      { pigment { rgb <0.43, 0.41, 0.35> }
        finish { sotd__f_Ground1 }
      }
    }
    #declare sotd__Ramp = union
    { object { sotd_Ramp_surface }
      // right side posts + junctions
      object
      { #if (sotd_Setting = SOTD_OUTDOOR_PATIO_RAIL)
          sotd__Post
        #else
          sotd__End_post
        #end
        translate <SOTD__XRAIL_RAMP, 0, 2>
      }
      object { sotd__Post translate <SOTD__XRAIL_RAMP, 0, sotd_zRail - 6> }
      #if (sotd__PipeMrg)
        object
        { sotd__Tjunction
          rotate 90 * y
          translate <SOTD__XRAIL_RAMP, SOTD__HRAIL, sotd_zRail - 6>
        }
        object
        { sotd__Crossing
          rotate 90 * y
          translate <SOTD__XRAIL_RAMP, SOTD__HRAIL / 2, sotd_zRail - 6>
        }
        #if (sotd_Setting = SOTD_OUTDOOR_PATIO_RAIL)
          object
          { sotd__Crossing
            rotate <-90, 0, 90>
            translate <SOTD__XRAIL_RAMP, SOTD__HRAIL / 2, 2>
          }
          object
          { sotd__Tjunction
            rotate 90 * y
            translate <SOTD__XRAIL_RAMP, SOTD__HRAIL, 2>
          }
        #else
          object
          { sotd__Tjunction
            rotate <-90, 0, 90>
            translate <SOTD__XRAIL_RAMP, SOTD__HRAIL / 2, 2>
          }
        #end
      #end
      // posts + junctions along the ramp
      #local sotd_N = ceil ((sotd_xPost1 - sotd_xPost2) / 30);
      #local sotd_Step = (sotd_xPost2 - sotd_xPost1) / sotd_N;
      #local sotd_I = 0;
      #while (sotd_I <= sotd_N)
        #local sotd_Locate = <sotd_xPost1 + sotd_Step * sotd_I, 0, sotd_zRail>;
        #if (sotd_Locate.x > -sotd_rLevel)
          object { sotd__Post translate sotd_Locate }
          #if (sotd__PipeMrg)
            object
            { sotd__Tjunction
              translate sotd_Locate + SOTD__HRAIL * y
            }
            object
            { sotd__Crossing
              translate sotd_Locate + SOTD__HRAIL / 2 * y
            }
          #end
        #else
          // This trace works if SOTD_HFOUNDATION < SOTD__HRAIL:
          #local sotd_v_Top = trace (sotd_Test_top, sotd_Locate, y);
          object
          { sotd__Post
            translate sotd_v_Top - SOTD__HRAIL * y
          }
          #if (sotd__PipeMrg)
            object { sotd_Skewed_T translate sotd_v_Top }
            object
            { sotd_Skewed_xing
              translate sotd_v_Top + sotd_v_Mid2 - sotd_v_Top2
            }
          #end
        #end
        #local sotd_I = sotd_I + 1;
      #end
      // top rail, level platform
      cylinder
      { (sotd_zRail - SOTD__RELBOW) * z,
        (sotd_Setting = SOTD_OUTDOOR_PATIO_RAIL? 0: 2 + SOTD__RELBOW) * z,
        SOTD__RRAIL
        translate <SOTD__XRAIL_RAMP, SOTD__HRAIL, 0>
      }
      object
      { sotd__Elbow translate <SOTD__XRAIL_RAMP, SOTD__HRAIL, sotd_zRail>
      }
      cylinder
      { <SOTD__XRAIL_RAMP - SOTD__RELBOW, SOTD__HRAIL, sotd_zRail>
        <-sotd_rLevel, SOTD__HRAIL, sotd_zRail>, SOTD__RRAIL
      }
      // lower rail, level platform
      cylinder
      { (sotd_zRail - SOTD__RELBOW) * z,
        (sotd_Setting = SOTD_OUTDOOR_PATIO_RAIL? 0: 2) * z,
        SOTD__RRAIL
        translate <SOTD__XRAIL_RAMP, SOTD__HRAIL / 2, 0>
      }
      object
      { sotd__Elbow translate <SOTD__XRAIL_RAMP, SOTD__HRAIL / 2, sotd_zRail>
      }
      cylinder
      { <SOTD__XRAIL_RAMP - SOTD__RELBOW, SOTD__HRAIL / 2, sotd_zRail>
        <-sotd_rLevel, SOTD__HRAIL / 2, sotd_zRail>, SOTD__RRAIL
      }
      // inclined rails
      cylinder { sotd_v_Top1, sotd_v_Top2, SOTD__RRAIL }
      cylinder { sotd_v_Mid1, sotd_v_Mid2, SOTD__RRAIL }
      object
      { Sotd__Rail_bend (SOTD__HRAIL)
        rotate <-90, 180, 0>
        translate <-sotd_rLevel, SOTD__HRAIL, sotd_zRail>
      }
      object
      { Sotd__Rail_bend (SOTD__HRAIL / 2)
        rotate <-90, 180, 0>
        translate <-sotd_rLevel, SOTD__HRAIL / 2, sotd_zRail>
      }
      // end loop
      #local sotd_v_End_ofs =
        vrotate (sotd_REnd * y, sotd_adRail * z) - sotd_REnd * y;
      #local sotd_hEnd = (sotd_v_Top2 - sotd_v_Mid2).y;
      #local sotd_End_bend = object
      { Sotd__Rail_bend (sotd_REnd)
        rotate 90 * x
        translate sotd_v_End_ofs
      }
      object { sotd_End_bend translate sotd_v_Top2 }
      object { sotd_End_bend translate sotd_v_Mid2 }
      union
      { object { sotd__Elbow rotate <-90, 0, 90> }
        object { sotd__Elbow rotate <90, 0, -90> translate -sotd_hEnd * y }
        cylinder
        { -SOTD__RELBOW * y, (SOTD__RELBOW - sotd_hEnd) * y, SOTD__RRAIL
        }
        cylinder
        { SOTD__RELBOW * x, (sotd_wEnd + sotd_v_End_ofs) * x, SOTD__RRAIL
        }
        cylinder
        { SOTD__RELBOW * x, (sotd_wEnd + sotd_v_End_ofs) * x, SOTD__RRAIL
          translate -sotd_hEnd * y
        }
        translate sotd_v_Top2 - sotd_wEnd * x + sotd_v_End_ofs * y
      }
      //
      texture { sotd__t_Rail }
      scale SOTD__FT_PER_IN
      translate <0, -sotd_Microspace, -SOTD__MICROABS>
    }
  #end
  sotd__Ramp
#end // Sotd__Build_ramp

/***********************************************************
 * Returns a wall object, on the +z side of the room.
 * Arguments:
 *   Feature    - Flag for door or windows
 *   Window     - A window object
 *   Door       - A door object
 *   maxWins    - Maximum number of windows before switching to a texture with
 *                transparent holes
 * Implementation note: construction is initially in feet, then scaled to the
 * user-chosen unit prior to being textured.
 */
#macro Sotd__Build_wall (Feature, Window, Door, maxWins)

  #local sotd_v_inFeet = sotd_v_Room_size / sotd__Scale;
  #if (sotd__Debug_unit) Sotd__DebugV ("sotd_v_inFeet", sotd_v_inFeet, 10) #end
  #local sotd_Baseboard = union
  { box
    { <0, -sotd_Microspace / (SOTD__FT_PER_IN * sotd__Scale), -0.375>,
      <1, 1.5, 0>
    }
    cylinder { 0, x, 1 scale <1, 0.75, 0.375> translate 1.5 * y }
    scale <1, SOTD__FT_PER_IN, SOTD__FT_PER_IN>
    pigment { sotd_c_Wall }
  }
  #local SOTD_DOOR_XTRA = 1.75 * SOTD__FT_PER_IN;
  #local sotd_Baseboard_short = SOTD__XDOORWAY - SOTD_DOOR_XTRA;
  #local sotd_Baseboard_long =
    sotd_v_inFeet.x * 2 - SOTD__XDOORWAY - SOTD__WDOORWAY - SOTD_DOOR_XTRA;

 //------------ paint and wallpaper ---------------
  #local sotd_Interior = box //interior wall surface
  { -1, 1
    scale
    < sotd_v_Room_size.x,
      sotd_v_Room_size.y,
      SOTD__MICROABS * sotd__Scale
    >
  }
  #local sotd_Exterior = box //exterior wall surface
  { -1, 1 scale
    < sotd_v_Room_size.x + (SOTD__DWALL - SOTD__MICROABS) * sotd__Scale,
      sotd_v_Room_size.y,
      (SOTD__DWALL - SOTD__MICROABS) * sotd__Scale
    >
    inverse
  }
  #local sotd_t_Interior = texture
  { #ifdef (sotd_t_Wall)
      sotd_t_Wall
      #ifdef (sotd_p_Wall) pigment { sotd_p_Wall } #end
    #else #ifdef (sotd_p_Wall)
      pigment { sotd_p_Wall }
    #else
      pigment { sotd_c_Wall }
    #end
    #end
  }
  #local sotd_c_Exterior = Sotd__Exterior_c();

  union
  {//------------- main wall features ---------------
    #local SOTD_YFLOOR = -0.01;
    #local sotd_v_Ramp =
      <sotd_v_inFeet.x - SOTD__XDOORWAY - SOTD__WDOORWAY / 2, 0, SOTD__DWALL>;
    #local sotd_Door_common = union
    { box
      { <-sotd_v_inFeet.x, SOTD__HDOORWAY, 0>,
        <sotd_v_inFeet.x + SOTD__DWALL, sotd_v_inFeet.y, SOTD__DWALL>
      }
      box
      { <-sotd_v_inFeet.x, -1, 0>,
        <sotd_v_inFeet.x + SOTD__DWALL, SOTD_YFLOOR, SOTD__DWALL>
      }
    }
    #switch (Feature)

      #case (SOTD_DOOR_ON_LEFT) //--------------------------
        object { sotd_Door_common }
        box
        { <-sotd_v_inFeet.x, SOTD_YFLOOR, 0>,
          <SOTD__XDOORWAY - sotd_v_inFeet.x, SOTD__HDOORWAY, SOTD__DWALL>
        }
        box
        { <SOTD__XDOORWAY + SOTD__WDOORWAY - sotd_v_inFeet.x, SOTD_YFLOOR, 0>,
          <sotd_v_inFeet.x + SOTD__DWALL, SOTD__HDOORWAY, SOTD__DWALL>
        }
        object { Door translate -sotd_v_inFeet * x }
        object
        { sotd_Baseboard
          scale <sotd_Baseboard_short, 1, 1>
          translate -sotd_v_inFeet * x
        }
        object
        { sotd_Baseboard
          scale <-sotd_Baseboard_long, 1, 1>
          translate sotd_v_inFeet * x
        }
       // not normally in camera view, but in case someone goes looking...
        object { Sotd__Build_ramp() translate sotd_v_Ramp scale <-1, 1, 1> }
        #break

      #case (SOTD_DOOR_ON_RIGHT) //-------------------------
        object { sotd_Door_common }
        box
        { <-sotd_v_inFeet.x, SOTD_YFLOOR, 0>,
          <sotd_v_inFeet.x - SOTD__XDOORWAY - SOTD__WDOORWAY,
           SOTD__HDOORWAY, SOTD__DWALL>
        }
        box
        { <sotd_v_inFeet.x - SOTD__XDOORWAY, SOTD_YFLOOR, 0>,
          <sotd_v_inFeet.x + SOTD__DWALL, SOTD__HDOORWAY, SOTD__DWALL>
        }
        object { Door translate -sotd_v_inFeet * x scale <-1, 1, 1> }
        object
        { sotd_Baseboard
          scale <-sotd_Baseboard_short, 1, 1>
          translate sotd_v_inFeet * x
        }
        object
        { sotd_Baseboard
          scale <sotd_Baseboard_long, 1, 1>
          translate -sotd_v_inFeet * x
        }
       // not normally in camera view, but in case someone goes looking...
        object { Sotd__Build_ramp() translate sotd_v_Ramp }
        #break

      #case (SOTD_WINDOWS) //-------------------------------
        #local sotd_nW = Sotd__NWindows();
        #local sotd_wSect = max // windows spacing
        ( (sotd_v_Room_size.x / sotd__Scale + SOTD__DWALL) * 2 / sotd_nW,
          SOTD__WWINMINSPACE + SOTD__WWINDOW
        );
        #local sotd_Dx = (sotd_nW - 1) / 2;
        #local sotd_nS = Sotd__NStories();
        #local sotd_hS = Sotd__hStory_ft();
        #local sotd_hW = Sotd__hWindow_ft();
        #local sotd_rwWin = SOTD__WWINDOW / 2;
        #if (sotd__Debug_unit)
          #debug concat
          ( "@@ ", str (sotd_nS, 0, 0),
            #if (sotd_nS = 1) " story, ", #else " stories, ", #end
            str (sotd_nW, 0, 0), " windows per story, ",
            str (sotd_nS * sotd_nW, 0, 0), " windows per wall\n"
          )
        #end
        #if (maxWins < sotd_nS * sotd_nW) // simulate windows with a texture
         // shutter texture
          #local SOTD_DSILL = (SOTD__DWALL - 1/6);
          #local sotd_Shadow = SOTD_DSILL / 2;
          #local sotd_xW1 = (sotd_wSect - SOTD__WWINDOW + sotd_Shadow) / 2;
          #local sotd_xW2 = (sotd_wSect + SOTD__WWINDOW - sotd_Shadow) / 2;
          #local sotd_yW2 = sotd_hW - sotd_Shadow;
          #local sotd_fn_Window = function
          { max
            ( select (x - sotd_xW1, 1, select (x - sotd_xW2, 0, 1)),
              select (y - sotd_yW2, 0, 1)
            )
          }
          #local sotd_t_Shutter = texture
          { pigment { sotd_c_Wall }
            normal
            { function
              { sotd_fn_Window (mod (abs (x), sotd_wSect), mod (y, sotd_hS), 0)
              }
              //bump_size SOTD_DSILL // this is too subtle
              bump_size 10 // trial and error
              accuracy sotd_Shadow
              translate
              < (mod (sotd_nW, 2) = 1? sotd_wSect / 2: 0),
                SOTD__YSILL + sotd_Shadow / 2,
                0
              >
              scale 1 / sotd_wSect
            }
          }
         // combined wall & windows texture
          #local sotd_Half_open = (SOTD_INDOORS_OPEN + SOTD_INDOORS_CLOSED) / 2;
          #local sotd_Closure =
            sin (min (pi/2, max (0, pi * (sotd_Setting - sotd_Half_open))));
          #local sotd_HingeR = sotd_rwWin / sotd_wSect; // right hinge gradient
          #local sotd_OpenR = // right shutter distal end gradient
            sotd_HingeR - sotd_rwWin * sotd_Closure / sotd_wSect;
          #local sotd_1st_Scale = 1 / (sotd__Scale * sotd_wSect);
          #local sotd_2nd_Scale = 1 / sotd_hS;
          #local sotd_Tween_scale = 1 / (sotd__Scale * sotd_hS);
          #local sotd_t_Solid_wall = texture
          { object
            { sotd_Interior
              texture { pigment { sotd_c_Exterior } }
              texture { sotd_t_Interior }
            }
          }
          #local sotd_t_Open = texture { pigment { rgbt 1 } }
          #local sotd_t_Wins = texture
          { gradient x texture_map
            { #if (mod (sotd_nW, 2) = 1) // odd windows per row
                #if (sotd_Setting < SOTD_INDOORS_CLOSED)
                  [sotd_OpenR sotd_t_Open]
                  [sotd_OpenR sotd_t_Shutter]
                #end
                [sotd_HingeR sotd_t_Shutter]
                [sotd_HingeR sotd_t_Solid_wall scale sotd_1st_Scale]
                [1 - sotd_HingeR sotd_t_Solid_wall scale sotd_1st_Scale]
                [1 - sotd_HingeR sotd_t_Shutter]
                #if (sotd_Setting < SOTD_INDOORS_CLOSED)
                  [1 - sotd_OpenR sotd_t_Shutter]
                  [1 - sotd_OpenR sotd_t_Open]
                #end
              #else // even windows per row
                [0.5 - sotd_HingeR sotd_t_Solid_wall scale sotd_1st_Scale]
                [0.5 - sotd_HingeR sotd_t_Shutter]
                #if (sotd_Setting < SOTD_INDOORS_CLOSED)
                  [0.5 - sotd_OpenR sotd_t_Shutter]
                  [0.5 - sotd_OpenR sotd_t_Open]
                  [0.5 + sotd_OpenR sotd_t_Open]
                  [0.5 + sotd_OpenR sotd_t_Shutter]
                #end
                [0.5 + sotd_HingeR sotd_t_Shutter]
                [0.5 + sotd_HingeR sotd_t_Solid_wall scale sotd_1st_Scale]
              #end
            }
            scale sotd_wSect
          }
          box
          { <-sotd_v_inFeet.x, -sotd_v_inFeet.y, 0>,
            <sotd_v_inFeet.x + SOTD__DWALL, sotd_v_inFeet.y, SOTD__DWALL>
            texture
            { gradient y texture_map
              { [SOTD__YSILL / sotd_hS
                  sotd_t_Solid_wall scale sotd_Tween_scale]
                [SOTD__YSILL / sotd_hS
                  sotd_t_Wins scale sotd_2nd_Scale]
                [(SOTD__YSILL + sotd_hW) / sotd_hS
                  sotd_t_Wins scale sotd_2nd_Scale]
                [(SOTD__YSILL + sotd_hW) / sotd_hS
                  sotd_t_Solid_wall scale sotd_Tween_scale]
              }
              scale sotd_hS
            }
            photons { pass_through }
          }
        #else // model the windowed wall
          box // top of wall
          { < -sotd_v_inFeet.x,
              (sotd_nS - 1) * sotd_hS + SOTD__YSILL + sotd_hW,
              0
            >,
            <sotd_v_inFeet.x + SOTD__DWALL, sotd_v_inFeet.y, SOTD__DWALL>
          }
          #local sotd_I = 1;
          #while (sotd_I < sotd_nS)
            box // horizontal interstory parts of wall
            { < -sotd_v_inFeet.x,
                (sotd_I - 1) * sotd_hS + SOTD__YSILL + sotd_hW,
                0
              >,
              < sotd_v_inFeet.x + SOTD__DWALL,
                sotd_I * sotd_hS + SOTD__YSILL,
                SOTD__DWALL
              >
            }
            #local sotd_I = sotd_I + 1;
          #end
          box // bottom of wall
          { <-sotd_v_inFeet.x, -1, 0>,
            <sotd_v_inFeet.x + SOTD__DWALL, SOTD__YSILL, SOTD__DWALL>
          }
          #local sotd_I = 0;
          #while (sotd_I < sotd_nS)
            #local sotd_Winbottom = sotd_I * sotd_hS + SOTD__YSILL;
            #local sotd_Wintop = sotd_Winbottom + sotd_hW;
            box // left of windows
            { <-sotd_v_inFeet.x, sotd_Winbottom, 0>,
              <-sotd_Dx * sotd_wSect - sotd_rwWin, sotd_Wintop, SOTD__DWALL>
            }
            #local sotd_J = 1;
            #while (sotd_J < sotd_nW)
              box // between windows
              { <(sotd_J - 1 - sotd_Dx) * sotd_wSect + sotd_rwWin,
                sotd_Winbottom, 0>,
                <(sotd_J - sotd_Dx) * sotd_wSect - sotd_rwWin,
                sotd_Wintop, SOTD__DWALL>
              }
              #local sotd_J = sotd_J + 1;
            #end
            box // right of windows
            { <sotd_Dx * sotd_wSect + sotd_rwWin, sotd_Winbottom, 0>,
              <sotd_v_inFeet.x + SOTD__DWALL, sotd_Wintop, SOTD__DWALL>
            }
            #local sotd_I = sotd_I + 1;
          #end
          #local sotd_I = 0;
          #while (sotd_I < sotd_nS)
            #local sotd_J = 0;
            #while (sotd_J < sotd_nW)
              object
              { Window
                translate <(sotd_J - sotd_Dx) * sotd_wSect, sotd_hS * sotd_I, 0>
              }
              #local sotd_J = sotd_J + 1;
            #end
            #local sotd_I = sotd_I + 1;
          #end
        #end
        object
        { sotd_Baseboard
          translate -x/2 scale <2 * sotd_v_inFeet.x, 1, 1>
        }
        #break

      #else // blank wall ----------------------------------
        box
        { <-sotd_v_inFeet.x, -1, 0>,
          <sotd_v_inFeet.x + SOTD__DWALL, sotd_v_inFeet.y, SOTD__DWALL>
        }
        object
        { sotd_Baseboard
          translate -x/2 scale <2 * sotd_v_inFeet.x, 1, 1>
        }

    #end
    scale sotd__Scale // Convert to user unit
    texture
    { object
      { sotd_Interior
        texture
        { pigment
          { object
            { sotd_Exterior
              sotd_c_Wall //trim
              sotd_c_Exterior
            }
          }
        }
        texture { sotd_t_Interior }
      }
    }
    translate sotd_v_Room_size * z
  }
#end // Sotd__Build_wall

/***********************************************************
 * Transforms a linear value to a triangular distribution (because the
 * Gaussian distribution takes too much parse time).
 */
#macro Sotd__Distribute (Linear, Min, Max)
( #if (Linear < 0.5)
    #local sotd_Dist = sqrt (Linear * 2) - 1;
  #else
    #local sotd_Dist = 1 - sqrt (2 - Linear * 2);
  #end
  (sotd_Dist * (Max - Min) + Max + Min) / 2
)
#end

/***********************************************************
 * Doorknob, in inches
 */
#local Sotd_fn_Lock_tab = function
{ sqrt
  ( RE_fn_Blob2
    ( f_superellipsoid (x / 48, z / 48, max (y, 0) / 40, 1, 0.75) * 48, 8
    )
  + RE_fn_Blob2
    ( f_sphere (0, min (y - 28, 0), min (z - 12 - 28, 0), 28), 8
    )
  ) - 1
}

#declare sotd__Doorknob = union
{ lathe
  { bezier_spline 68,
    <0, 0>, <55, 0>, <113, 0>, <168, 0>,
    <168, 0>, <168, 6>, <168, 14.5>, <168, 20.5>,
    <168, 20.5>, <168, 25.2940>, <164.6771, 29.4477>, <160, 30.5>,
    <160, 30.5>, <140, 35>, <120, 39.5>, <100, 44>,
    <100, 44>, <81.2917, 48.2094>, <68, 64.8240>, <68, 84>,
    <68, 84>, <68, 85.3>, <68, 86.7>, <68, 88>,
    <68, 88>, <68, 92.4183>, <64.4183, 96>, <60, 96>,
    <60, 96>, <60, 95>, <60, 94>, <60, 93>,
    <60, 93>, <62.7614, 93>, <65, 90.7614>, <65, 88>,
    <65, 88>, <65, 10>, <65, 10>, <56, 10>,
    <56, 10>, <56, 39>, <56, 67>, <56, 96>,
    <56, 96>, <56, 112.8398>, <63.5116, 128.8018>, <76.4873, 139.5354>,
    <76.4873, 139.5354>, <104.0462, 162.3324>, <120, 196.2341>, <120, 232>,
    <120, 232>, <120, 267.5118>, <104.2712, 301.1998>, <77.0454, 324>,
    <77.0454, 324>, <69.3788, 330.4205>, <62, 332>, <56, 332>,
    <56, 332>, <51.5817, 332>, <48, 328.4183>, <48, 324>,
    <48, 324>, <48, 216>, <0, 108>, <0, 0>
  }
  isosurface
  { function { Sotd_fn_Lock_tab (x, y, abs (z)) }
    contained_by { box { -<48, 8, 48>, <48, 40, 48> } }
    max_gradient 0.15
    translate 332 * y
  }
  scale 1 / 128
  rotate -90 * x
} // sotd__Doorknob


/***********************************************************
 * Returns an exterior paint color by constraining the
 * interior trim color: not too saturated, not too dark.
 */
#macro Sotd__Exterior_c()
  #local sotd_C = CHSV2RGB
  ( <sotd_hWall, min (sotd_sWall, 0.7), max (sotd_vWall, 0.5)>
  );
  sotd_C
#end


/***********************************************************
 * Returns a forest object.
 * Arguments:
 *   Hills      - the terrain on which to plant the trees
 *   Class      - the tree type: 0 = broadleaf, 1 = conifer
 *   NPotential - (output) the maximum potential number of trees
 *   NActual    - (output) the actual number of trees planted
 */
#macro Sotd__Grow_forest (Hills, Class, NPotential, NActual)
// wood starting point: <0.0999, 0.0561, 0.0395> (gray 0.0672 avg 0.0652)
// try <0.1380, 0.0775, 0.0546>
// try <0.1385, 0.0762, 0.0554>
// try <0.135, 0.081, 0.054>
  #if (Class = 0)
    #local sotd_Tree = sphere { y, 1 scale <1, 0.5, 1> }
    #local sotd_hMin = 10.75;
    #local sotd_hMax = 16.75;
  #else
    #local sotd_Tree = intersection
    { sphere { -4 * y, 5 scale <1/3, 1, 1/3> }
      cylinder { 0, y, 1 }
    }
    #local sotd_hMin = 13.75;
    #local sotd_hMax = 19.75;
  #end
  #local sotd_Spacing = 1 / (sqrt (sotd_Trees_density) * 0.01);
  #local sotd_1D = floor (20 * sotd_Hills_max / sotd_Spacing);
  #local sotd_Base = -sotd_1D / 2;
  #local NPotential = sotd_1D * sotd_1D;
  #local NActual = 0;
  #local sotd_Test = <0, 0, 0>; // dummy value
 // Seed values are from rand.inc, in case Ingo knows something I don't:
  #local sotd_Switch = seed (574647); // tree class
  // To prevent regularities between Sotd__Grow_forest()
  // calls, each Class needs a different seed value:
  #local sotd_Stream = seed (324879 - Class * 28144); // tree position & size
  union
  { #local sotd_NS = 0;
    #while (sotd_NS < sotd_1D)
      #local sotd_EW = 0;
      #while (sotd_EW < sotd_1D)
        #if (floor (rand (sotd_Switch) + 0.5) = Class)
         // For consistency between renders at different camera positions,
         // all pseudorandom values must be assigned prior to any tests:
          #local sotd_X = rand (sotd_Stream);
          #local sotd_Z = rand (sotd_Stream);
          #local sotd_R = rand (sotd_Stream);
          #local sotd_H = rand (sotd_Stream);
         //
          #local sotd_Root = trace
          ( Hills,
            < (sotd_Base + sotd_EW + sotd_X) * sotd_Spacing,
              -1,
              (sotd_Base + sotd_NS + sotd_Z) * sotd_Spacing
            >,
            y
          );
          #if (sotd_Root.y > 0)
            #local sotd_hTree = 40; //@@PLACE HOLDER!
            #if (sotd_Trees_level >= 2)
              #local sotd_Made = yes;
            #else
              #local sotd_Treetop = sotd_Root + sotd_hTree * y;
              #local sotd_Line = trace
              ( Hills, sotd_v_Camera_location,
                sotd_Treetop - sotd_v_Camera_location, sotd_Test
              );
              #local sotd_Made =
              ( vlength (sotd_Test) = 0 |
                vlength (sotd_Line - sotd_v_Camera_location) >
                vlength (sotd_Treetop - sotd_v_Camera_location)
              );
            #end
            #if (sotd_Made)
              #local sotd_RTree = Sotd__Distribute (sotd_R, 5.2, 7);
              object
              { sotd_Tree
                scale
                < sotd_RTree,
                  Sotd__Distribute (sotd_H, sotd_hMin, sotd_hMax),
                  sotd_RTree
                >
                translate sotd_Root
              }
              #local NActual = NActual + 1;
            #end
          #end
        #end
        #local sotd_EW = sotd_EW + 1;
      #end
      #local sotd_NS = sotd_NS + 1;
    #end
    texture
    { #if (Class = 0)
        pigment { rgb SOTD__C_BROADLEAF }
        normal { granite 1 scale <12, 9, 12> }
      #else
        pigment { rgb SOTD__C_CONIFER }
        normal { granite 1 scale <12, 6, 12> }
      #end
      finish { sotd__f_Forest1 }
    }
  }
#end // Sotd__Grow_forest

/***********************************************************
 * Returns an indoor light, with optional lamp fixture. If there is a fixture,
 * standard illumnants get a white bulb, and HSV lights get a colored bulb.
 * Arguments:
 *   v_Where   - Light source location
 *   c_Color   - Color of the light
 *   HueSat    - -1 for a standard illuminant, otherwise <hue, saturation>
 *   t_Fixture - A texture for the hood exterior and base
 *   Quality   - 0 = shadowless, no attenuation, no photons, no fixture
 *               1 = spotlight, shadows, inverse-squared attenuation, no fixture
 *               2 = fixture, shadows, inverse-squared attenuation
 *   Area      - If true and user has requested area lights and Quality is 1 or
 *               more, then it will be an area light
 *   Photons   - If false, photons will not be shot.
 */
#macro Sotd__Lamp (v_Where, c_Color, HueSat, t_Fixture, Quality, Area, Photons)
  #local sotd_v_Aim = sotd_Height/2 * y;
  #local sotd_Cm = sotd_Unit * 100;
  #local sotd_Dist = vlength (v_Where - sotd_v_Aim);
  #switch (Quality)
    #case (0) // shadowless, no attenuation, no photons, no fixture
      #if (!VZero (c_Color))
        light_source
        { v_Where, c_Color
          parallel point_at sotd_v_Aim
          shadowless
          photons { reflection off refraction off }
        }
        #if (sotd_Debug >= 1)
          Sotd__Echo_light_source
          ( v_Where, sotd_v_Aim, c_Color, true, 0, 0, 0, 0, 0
          )
        #end
      #end
      #break
    #case (1) // spotlight, shadows, inverse-squared attenuation, no fixture
      #local sotd_rFade = Lamp_Fade / sotd_Cm;
      // Brightness formula: the inverse of POV's attenuation formula:
      #local sotd_Surface = pow (sotd_Dist/sotd_rFade, 2);
      #local sotd_Brightness = (1 + sotd_Surface) / 2;
      #local sotd_Radius = // span where entire bulb is visible
        90 - 2 * degrees (atan2 (3, 2.5 * SOTD__CM_PER_IN - 0.08));
      #local sotd_Tightness = 0.25; // resembles DeskLamp render with area light
      light_source
      { v_Where, c_Color * sotd_Brightness
        fade_power 2 fade_distance sotd_rFade
        spotlight point_at sotd_v_Aim radius sotd_Radius falloff 90
        tightness sotd_Tightness
        #if (Area & sotd_Area)
          #local sotd_dA =
            6 / sotd_Cm * sotd_Area * Sotd__fn_Area_adj (sotd_Area_res);
          area_light sotd_dA * x, sotd_dA * z, sotd_Area_res, sotd_Area_res
          circular orient adaptive sotd_Area_adaptive jitter
        #else
          #local sotd_dA = 0;
        #end
        #if
        ( !Photons | (c_Color.red = 0 & c_Color.green = 0 & c_Color.blue = 0)
        )
          photons { reflection off refraction off }
        #end
      }
      #if (sotd_Debug >= 1)
        Sotd__Echo_light_source
        ( v_Where, sotd_v_Aim, c_Color * sotd_Brightness, false,
          sotd_rFade, sotd_Radius, sotd_Tightness, sotd_dA, Photons
        )
      #end
      #break
    #else // fixture, shadows, inverse-squared attenuation
      #local sotd_HS = HueSat + <0, 0>;
      #local sotd_Dist = vlength (v_Where - sotd_v_Aim);
     // Is lamp hung from ceiling?
      #local sotd_From_ceil = (v_Where.y > sotd_v_Room_size.y * 2/3);
     // angle from light source to aim position:
      #local sotd_Angle = sgn (sotd_v_Aim.y - v_Where.y)
        * VAngle (v_Where - sotd_v_Aim, (v_Where - sotd_v_Aim) * <1, 0, 1>);
      #if (sotd_Angle = 0)
        #local sotd_AnchorPt = <0, -SOTD__HUPPER, -SOTD__DLAMP>;
      #else
        #local sotd_Sin = sin (sotd_Angle);
        #local sotd_Cos = cos (sotd_Angle);
        #local sotd_AnchorPt = // centimeters
          #if (sotd_From_ceil)
            < 0,
              -SOTD__DLAMP * sotd_Sin + SOTD__HRIGID * sotd_Cos
              + SOTD__HFLEX * sotd_Sin / sotd_Angle,
              -SOTD__DLAMP * sotd_Cos - SOTD__HRIGID * sotd_Sin
              - SOTD__HFLEX * (1 - sotd_Cos) / sotd_Angle - SOTD__ZLAMP,
            >;
          #else
            < 0,
              -SOTD__DLAMP * sotd_Sin - SOTD__HRIGID * sotd_Cos
              - SOTD__HFLEX * sotd_Sin / sotd_Angle,
              -SOTD__DLAMP * sotd_Cos + SOTD__HRIGID * sotd_Sin
              + SOTD__HFLEX * (1 - sotd_Cos) / sotd_Angle - SOTD__ZLAMP,
            >;
          #end
      #end
      #local sotd_Basexz =
        #if (v_Where.x = 0 & v_Where.z = 0)
          <0, 0, 0>;
        #else
          vrotate (sotd_AnchorPt, degrees (atan2 (-v_Where.x, -v_Where.z)) * y);
        #end
      #local sotd_Basexz = // POV units
        (sotd_Basexz / sotd_Cm + v_Where) * <1, 0, 1>
      + (sotd_From_ceil? sotd_v_Room_size: 0) * y;
      #local sotd_hLamp =  // centimeters
      ( sotd_From_ceil?
        sotd_v_Room_size.y * sotd_Cm - (v_Where.y * sotd_Cm + sotd_AnchorPt.y)
          + SOTD__HUPPER:
        v_Where.y * sotd_Cm + sotd_AnchorPt.y + SOTD__HUPPER
      );
      #if (sotd_hLamp < SOTD__HLAMP)
        #if (degrees (sotd_Angle) = -sotd_Light_alt)
          #error concat (str (sotd_Light_alt, 0, 2) " is too low a value for sotd_Light_alt.")
        #else
          #warning concat ("sotd_Unit ", Sotd__Str (sotd_Unit, 6))
          #warning concat ("sotd_Height ", Sotd__Str (sotd_Height, 6))
          #warning concat ("sotd_v_Room_size ", Sotd__VStr (sotd_v_Room_size, 6))
          #warning concat ("v_Where ", Sotd__VStr (v_Where, 6))
          #warning concat ("Angle ", Sotd__Str (degrees (sotd_Angle), 6), " degrees")
          #warning concat ("sotd_hLamp ", Sotd__Str (sotd_hLamp, 6))
          #error "Sotd__Lamp(): bug in ShapeOfTheDay. Please report the above warning values to povray.object-collection."
        #end
      #end
      /*Sotd__DebugF ("Angle   ", degrees (sotd_Angle), 6)
      Sotd__DebugV ("v_Where ", v_Where, 6)
      Sotd__DebugV ("AnchorPt", sotd_AnchorPt, 6)
      Sotd__DebugV ("Basexz  ", sotd_Basexz, 6)
      Sotd__DebugF ("Height  ", sotd_hLamp, 6)*/
      #local sotd_Max = VMax (c_Color);
      #local sotd_c_Bulb =
      #if (sotd_HS.y < 0)
        rgb 1;
      #else #if (sotd_Max = 0)
        CHSV2RGB (<sotd_HS.x, sotd_HS.y, 1>);
      #else
        c_Color / sotd_Max;
      #end
      #end

      object
      { Lamp_Flexneck
        ( sotd_hLamp, sotd_Basexz, (sotd_From_ceil? -y: y), sotd_v_Aim,
          !VZero (c_Color),
          #if (sotd_HS.x < 0)
            c_Color, SOTD__LM_UNIT * Lamp_Luminance (c_Color) / sotd_Exposure,
          #else
            sotd__c_Illums [sotd__Whitex] [3],
            -SOTD__LM_UNIT * sotd_Max / sotd_Exposure,
          #end
          t_Fixture, Lamp_Bulb_A60, sotd_c_Bulb,
          <(Area & sotd_Area), sotd_Area_res, sotd_Area_adaptive, 0>,
          Photons, <1, 0>
        )
        interior { ior 1.49 }
      }

  #end // #switch (Quality)
#end //macro Sotd__Lamp

/***********************************************************
 * Returns a door object, facing the +z direction, opening inward (-z)
 * if it were to open, with hinges on the left.  Y-axis is a few inches
 * left of the door.
 * Argument:
 *   t_Hardware - A texture for hardware items
 */
#macro Sotd__Make_door (t_Hardware)

  #local SOTD_WJAMB = 0.25 * SOTD__FT_PER_IN;
  #local SOTD_SPACE = 0.125 * SOTD__FT_PER_IN;
  #local SOTD_DDOOR = 1.75 * SOTD__FT_PER_IN;
  #local SOTD_rDOOR = 0.625 * SOTD__FT_PER_IN;
  #local SOTD_WTHICK = 6.5 * SOTD__FT_PER_IN;
  #local SOTD_WOUTER = 1.75 * SOTD__FT_PER_IN;
  #local SOTD_ZOUTER = SOTD_DDOOR + 1.375 * SOTD__FT_PER_IN;
  #local SOTD_ZHINGE = -0.25; //inches
  #local SOTD_WBUMBER = 0.25; //inches

  #local sotd_y2Door = SOTD__HDOORWAY - SOTD_WJAMB - SOTD_SPACE;
  #local sotd_y1Door = SOTD_SPACE;
  #local sotd_x1Door = SOTD_WJAMB + SOTD_SPACE;
  #local sotd_x2Door = SOTD__WDOORWAY - SOTD_WJAMB - SOTD_SPACE;
  #local sotd_x1Cross = sotd_x1Door + SOTD_WTHICK - SOTD_rDOOR;
  #local sotd_x2Cross = sotd_x2Door - SOTD_WTHICK + SOTD_rDOOR;
  #local sotd_v_Doorknob =
    <sotd_x2Door - 2.75 * SOTD__FT_PER_IN, sotd_y1Door + 3, 0>;

  #local sotd_Jamb = union
  { box { 0, <SOTD_WJAMB, 1, SOTD__DWALL> }
    box { <SOTD_WJAMB, 0, SOTD_DDOOR>, <2 * SOTD_WJAMB, 1, SOTD_ZOUTER> }
    union
    { cylinder { -x, <-1, 1, 0>, 1 }
      box { <-3, 0, -1>, <-1, 1, 0> }
      scale <SOTD_WOUTER / 3, 1, 0.375 * SOTD__FT_PER_IN>
    }
  }
  #local sotd_Side = RE_Box_y_right
  ( <0, sotd_y1Door, 0>, <SOTD_WTHICK, sotd_y2Door, SOTD_DDOOR>,
    SOTD_rDOOR, no
  )
  #local sotd_Bottom = RE_Box_x_up
  ( <sotd_x1Cross, 0, 0>, <sotd_x2Cross, SOTD_WTHICK, SOTD_DDOOR>,
    SOTD_rDOOR, no
  )

  #local sotd_rwHinge = (SOTD_SPACE/2 - SOTD__MICROABS) / SOTD__FT_PER_IN;
  #local sotd_Hinge_plate = box
  { <sotd_rwHinge, -1.5, -0.25>, <sotd_rwHinge + 3/32, 1.5, 1.125>
  }
  #local sotd_Hinge = union
  { cylinder
    { <0, -1, 1>, <0, 1, 1>, 7/8
      scale <SOTD_ZHINGE, 1.625, SOTD_ZHINGE>
    }
    object { sotd_Hinge_plate }
    object { sotd_Hinge_plate scale <-1, 1, 1> }
    texture { t_Hardware }
    scale SOTD__FT_PER_IN
    translate (sotd_x1Door - SOTD_SPACE/2) * x
  }

  #local sotd_Bumper = lathe
  { bezier_spline 12
    <0, SOTD_WBUMBER>, <2, SOTD_WBUMBER>, <2.4, 2>, <3, 2>,
    <3, 2>, <3.6, 2>, <4, 1.6>, <4, 1>,
    <4, 1>, <4, 1>, <4, 0>, <4, 0>
    rotate -90 * z
    scale 0.25 * SOTD__FT_PER_IN
  }

  union
  { object { sotd_Side translate sotd_x1Door * x }
    object { sotd_Side scale <-1, 1, 1> translate sotd_x2Door * x }
    RE_Box_y
    ( <(SOTD__WDOORWAY - SOTD_WTHICK) / 2, sotd_y1Door, 0>,
      <(SOTD__WDOORWAY + SOTD_WTHICK) / 2, sotd_y2Door, SOTD_DDOOR>,
      SOTD_rDOOR, no
    )
    object { sotd_Bottom translate sotd_y1Door * y }
    object { sotd_Bottom scale <1, -1, 1> translate sotd_y2Door * y }
    RE_Box_x
    ( <sotd_x1Cross, sotd_v_Doorknob.y - SOTD_WTHICK/2, 0>,
      <sotd_x2Cross, sotd_v_Doorknob.y + SOTD_WTHICK/2, SOTD_DDOOR>,
      SOTD_rDOOR, no
    )
    box
    { <sotd_x1Door + SOTD_WTHICK, sotd_y1Door + SOTD_WTHICK, SOTD_rDOOR>,
      <sotd_x2Door - SOTD_WTHICK, sotd_y2Door - SOTD_WTHICK,
       SOTD_DDOOR - SOTD_rDOOR>
    }
   //--------- hardware -----------
    object { sotd_Hinge translate (sotd_y2Door - 0.75) * y }
    object { sotd_Hinge translate (sotd_y1Door + 1) * y }
    object
    { //Doorknob (DOORKNOB_SPHERE, DOORKNOB_TWIST_TAB, 0, yes, 0, t_Hardware)
      sotd__Doorknob
      texture { t_Hardware }
      scale SOTD__FT_PER_IN
      translate sotd_v_Doorknob
    }
   //---------- bumper ------------
    #local sotd_v_Pivot =
      <sotd_x1Door - SOTD_SPACE/2, 0, SOTD_ZHINGE * SOTD__FT_PER_IN>;
    #local sotd_v_Knob_end = <sotd_v_Doorknob.x, 0, -23/8 * SOTD__FT_PER_IN>;
    #local sotd_Swing = cylinder
    { -y, y, vlength (sotd_v_Knob_end - sotd_v_Pivot)
      translate sotd_v_Pivot
    }
    #local sotd_v_Contact = trace
      (sotd_Swing, <SOTD_WBUMBER - SOTD__XDOORWAY, 0, 0>, -z);
    object
    { sotd_Bumper
      translate <-SOTD__XDOORWAY, sotd_v_Doorknob.y, sotd_v_Contact.z>
    }
   //----------- jamb -------------
    object
    { sotd_Jamb
      scale <1, SOTD__HDOORWAY + SOTD_WOUTER + sotd_Microspace / sotd__Scale, 1>
      translate -(sotd_Microspace / sotd__Scale) * y
    }
    object
    { sotd_Jamb
      scale
        <-1, SOTD__HDOORWAY + SOTD_WOUTER + sotd_Microspace / sotd__Scale, 1>
      translate <SOTD__WDOORWAY, -(sotd_Microspace / sotd__Scale), 0>
    }
    object
    { sotd_Jamb
      rotate -90 * z
      scale <SOTD__WDOORWAY + 2 * SOTD_WOUTER, 1, 1>
      translate <-SOTD_WOUTER, SOTD__HDOORWAY, 0>
    }
   //-------- threshold -----------
    box
    { <2 * SOTD_WJAMB, 0, SOTD_DDOOR>,
      <SOTD__WDOORWAY - 2 * SOTD_WJAMB, SOTD_WJAMB, SOTD_ZOUTER>
      translate -SOTD__MICROABS * y
    }
    pigment { sotd_c_Wall }
    translate SOTD__XDOORWAY * x
  }
#end // Sotd__Make_door

/***********************************************************
 * Creates global parts for railings.
 * Unit = 1 inch
 */
#macro Sotd__Make_railing_parts()
  #ifndef (sotd__t_Rail)
    #declare sotd__t_Rail = texture
    { pigment
      { crackle solid color_map
        { [0 rgb <0.22, 0.24, 0.26>]
          [1 rgb <0.44, 0.48, 0.52>]
        }
        turbulence 0.25
        scale 0.4
      }
      finish
      { sotd__f_Default1
        brilliance 2
        #if (!sotd_Reflect)
          specular 0.917 metallic
          roughness 0.05
        #end
      }
    }
  #end
  #if (sotd__PipeMrg)
    #ifndef (sotd__Crossing)
      #declare sotd__Crossing = union
      { PipeMrg (0, SOTD__RRAIL, 0.2, 90, 0)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90, 90 * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90, 180 * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90, 270 * z)
      }
    #end
    #ifndef (sotd__Tjunction)
      #declare sotd__Tjunction = union
      { PipeMrg (0, SOTD__RRAIL, 0.2, 90, 180 * z)
        PipeMrg (0, SOTD__RRAIL, 0.2, 90, 270 * z)
      }
    #end
  #end
  #ifndef (sotd__Elbow)
    // connects -x and -z
    #declare sotd__Elbow = intersection
    { torus { SOTD__RELBOW, SOTD__RRAIL }
      box { -SOTD__RRAIL * y, <SOTD__RELBOW, 0, SOTD__RELBOW> + SOTD__RRAIL }
      translate -<SOTD__RELBOW, 0, SOTD__RELBOW>
    }
  #end
  #ifndef (sotd__Post)
    // does not include PipeMrg
    #declare sotd__Post = cylinder { SOTD__HRAIL * y, -6 * y, SOTD__RRAIL }
  #end
  #ifndef (sotd__End_post)
    // connects +z at height SOTD__HRAIL; includes PipeMrg if available
    #declare sotd__End_post = union
    { cylinder { (SOTD__HRAIL - SOTD__RELBOW) * y, -6 * y, SOTD__RRAIL }
      object { sotd__Elbow rotate <-90, 90, 0> translate SOTD__HRAIL * y }
      #if (sotd__PipeMrg)
        object
        { sotd__Tjunction rotate <-90, 0, 90> translate (SOTD__HRAIL / 2) * y
        }
      #end
    }
  #end
#end // Sotd__Make_railing_parts

/***********************************************************
 * Returns a window object, with the +z direction as outside.
 * Unit = 1 foot
 * Argument:
 *   t_Hardware - A texture for hardware items
 */
#macro Sotd__Make_window (t_Hardware)

  #local SOTD_2BY = 1.5 * SOTD__FT_PER_IN;
  #local SOTD_4BY = 3.5 * SOTD__FT_PER_IN;
  #local SOTD_PLY = 0.5 * SOTD__FT_PER_IN;
  #local SOTD_rBEAM = 0.0075;
  #local sotd_Beam = RE_Box_y
    (<0, 0, -SOTD_PLY - SOTD_2BY>, <SOTD_4BY, 1, -SOTD_PLY>, SOTD_rBEAM, no)
  #local sotd_Wintop = SOTD__YSILL + Sotd__hWindow_ft();
  #local sotd_wShutter = SOTD__WWINDOW / 2;
  #local sotd_Shutter = union
  { box
    { <0, SOTD__YSILL, -SOTD_PLY>,
      <sotd_wShutter, sotd_Wintop, 0>
    }
    object
    { sotd_Beam
      scale <1, sotd_Wintop - SOTD__YSILL, 1>
      translate <0, SOTD__YSILL, 0>
    }
    object
    { sotd_Beam
      scale <-1, sotd_Wintop - SOTD__YSILL, 1>
      translate <sotd_wShutter, SOTD__YSILL, 0>
    }
    object
    { sotd_Beam
      rotate -90 * z
      scale <sotd_wShutter - 2 * (SOTD_4BY - SOTD_rBEAM), 1, 1>
      translate <SOTD_4BY - SOTD_rBEAM, sotd_Wintop, 0>
    }
    object
    { sotd_Beam
      rotate -90 * z
      scale <sotd_wShutter - 2 * (SOTD_4BY - SOTD_rBEAM), -1, 1>
      translate <SOTD_4BY - SOTD_rBEAM, SOTD__YSILL, 0>
    }
    rotate min (sotd_Setting - SOTD_INDOORS_CLOSED, 0) * 180 * y
    translate <-sotd_wShutter, 0, SOTD__DWALL>
  }

  #local sotd_Estimate = 0.053; //somewhat of a Fresnel average;
                                // but breaks down at shallow angles
  #local sotd_Roughness = 0.0005;
  #local sotd_Pane = box
  { <SOTD__MICROABS - sotd_wShutter, SOTD__YSILL + SOTD__MICROABS, 0>,
    <sotd_wShutter - SOTD__MICROABS, sotd_Wintop - SOTD__MICROABS, 1/48>
    pigment { rgbt 1 }
    finish
    { reflection { 0 1 fresnel } conserve_energy
      #if (!sotd_Reflect)
        roughness sotd_Roughness
        specular sotd_Estimate
          * (1/sotd_Roughness + 2) / (4 * (2 - pow (2, -0.5 / sotd_Roughness)))
      #end
    }
    interior { ior 1.523 }
    photons { pass_through } //for sunlight with settings 2.*
  }

  union
  { object { sotd_Shutter }
    object { sotd_Shutter scale <-1, 1, 1> }
    #local sotd_nPanes = min (3, sotd_Quality - 2);
    #local sotd_P = 0;
    #while (sotd_P < sotd_nPanes)
      object { sotd_Pane translate (SOTD__DWALL/2 - sotd_P / 32) * z }
      #local sotd_P = sotd_P + 1;
    #end
    pigment { sotd_c_Wall }
  }
#end // Sotd__Make_window

/***********************************************************
 * Rail elbow transition from horizontal to ramp angle.
 * Origin is at horizontal end; horizontal is toward -x;
 * the bend leans toward -z.
 */
#macro Sotd__Rail_bend (rMajor)
  #local sotd_Angle = atan2 (1, SOTD__RAMP);
  intersection
  { torus { rMajor, SOTD__RRAIL }
    plane { <SOTD__RAMP, 0, -1>, 0 }
    box // a box yields better bounding than a plane
    { <0, -SOTD__RRAIL, (rMajor - SOTD__RRAIL) * sin (sotd_Angle)>,
      < (rMajor + SOTD__RRAIL) * cos (sotd_Angle),
        SOTD__RRAIL,
        rMajor + SOTD__RRAIL
      >
    }
    translate -rMajor * z
  }
#end

/***********************************************************
 * Returns a horizontal tile normal.
 * Argument:
 *   Seam - width of the seam between tiles.
 */
#macro Sotd__Tile_n (Seam)
  normal
  { function
    { min
      ( Sotd__fn_Tile (mod (abs(x), 1), Seam / 2),
        Sotd__fn_Tile (mod (abs(z), 1), Seam / 2)
      )
    } Seam * 3
    accuracy Seam * 0.05
  }
#end

#declare Sotd__fn_Tile = function (x, sotd_P0_Edge)
{ select
  ( x - sotd_P0_Edge,
    1 - pow (1 - x / sotd_P0_Edge, 3),
    select
    ( x + sotd_P0_Edge - 1, 1,
      1 - pow (1 - (1 - x) / sotd_P0_Edge, 3)
    )
  )
}

//================================ MAIN SETUP ==================================

Sotd__Get_settings (no) //Check settings immediately upon #include.
#declare sotd__Set_up = no;

/**************************************************************************
 * Builds the prefab scene.
 */
#macro Sotd_Setup (Uses_radiosity)

 // Check if Sotd_Setup() was already called:
  #if (sotd__Set_up)
    #error "Sotd_Setup() is called more than once!"
  #end
  #declare sotd__Set_up = yes;

 // Check settings again, in case user has changed any since the #include:
  Sotd__Get_settings (Uses_radiosity)

 // Echo
  #if (sotd_Debug >= 1) Sotd__Debug ((sotd_Debug >= 2), Uses_radiosity) #end
  #if (sotd__Debug_unit)
    Sotd__DebugF ("sotd_Unit", sotd_Unit, 10)
    Sotd__DebugF ("sotd__Scale", sotd__Scale, 10)
  #end

 //-------------- general lighting ----------------

  #if (sotd_Setting >= SOTD_INDOORS_OPEN) // set up DeskLamp parameters
    #declare Lamp_c_Ambient = sotd_c_Ambient * sotd_Diffuse;
    #declare Lamp_Diffuse = sotd_Diffuse;
    #declare Lamp_Max_Sample = sotd_Max_sample;
    #declare Lamp_Radiosity = Uses_radiosity;
    #declare Lamp_Scale = sotd_Unit * 100;
    #declare Lamp_c_Weights = sotd__v_Grays[sotd__Whitex];
    #local sotd_Lamp_debug = Lamp_Debug;
    #declare Lamp_Debug = (sotd_Debug != 0);
    // Lamp_Lumen is set by Sotd__Get_settings().
  #end

  global_settings
  { // assumed_gamma 1 // set in shapeoftheday_ref.inc
    max_trace_level floor (sotd_Max_trace)
    #if (sotd_Ph > 0)
      photons
      { #if (sotd_Ph > 1)
          count floor (sotd_Ph)
        #else
          spacing sotd_Ph
        #end
        autostop sotd_Ph_autostop
        #if (sotd_Ph_media)
          media floor (sotd_Ph_media)
          #if (sotd_Ph_media_factor) , sotd_Ph_media_factor #end
        #end
        #if (sotd_Ph_radius2 | sotd_Ph_radius4)
          radius ,
          #if (sotd_Ph_radius2) sotd_Ph_radius2 #end
          #if (sotd_Ph_radius4) , , sotd_Ph_radius4 #end
        #end
        #if (sotd_Ph_load) load_file sotd_s_Photons_file #end
        #if (sotd_Ph_save) save_file sotd_s_Photons_file #end
      }
    #end
    #if (Uses_radiosity)
      radiosity
      { max_sample sotd_Max_sample
        #if (sotd_Setting < SOTD_INDOORS_CLOSED) media on #end
      }
    #end
    #if (version >= 3.7) mm_per_unit sotd__mmPerUnit #end
  }

 //------------------- camera ---------------------

  #ifdef (Screen_Inc_Temp)
    Set_Camera
    ( sotd_v_Camera_location, sotd_v_Camera_look_at, sotd_Camera_angle
    )
    Set_Camera_Aspect_Ratio (sotd_Camera_aspect)
    #if (sotd_Aperture != 0)
      #warning "screen.inc is used; sotd_Aperture is ignored."
    #end
    #if (sotd_FOV = 0)
      #warning "screen.inc is used; the orthographic camera will not be used."
    #end
  #else
    camera
    { #switch (sotd_Lens)
        #case (SOTD_ORTHOGRAPHIC) orthographic #break
        #case (SOTD_ULTRA_WIDE_ANGLE) ultra_wide_angle #break
      #end
      location sotd_v_Camera_location
      look_at sotd_v_Camera_look_at
      right sotd_Camera_right * x
      up sotd_Camera_up * y
      #if (sotd_Lens != SOTD_ORTHOGRAPHIC) angle sotd_Camera_angle #end
      #if (sotd_Aperture)
        aperture sotd_Aperture
        focal_point sotd_v_Focus
        blur_samples floor (Sotd__Blur_samples(sotd_Samples))
        #ifdef (sotd_Confidence) confidence sotd_Confidence #end
        #ifdef (sotd_Variance) variance sotd_Variance #end
        #if (sotd_Focus <= -sotd_Camera_distance)
          #warning "The focal plane is behind the camera."
        #end
      #end
    }
  #end

 //------------- checkered texture ----------------

  #local sotd_t_Matte_check = texture
  { pigment { checker sotd_c_Check1 sotd_c_Check2 }
    #if (sotd_Setting < SOTD_INDOORS_OPEN) finish { sotd__f_Ground } #end
    #if (sotd_Tile_spacing) Sotd__Tile_n (sotd_Tile_spacing) #end
  }
  // The glossy floor needs features unavailable in older POV-Ray versions.
  // Finish-level Fresnel (3.8) is reproduced by adding a transparent glossy
  // layer, with reduced highlight (0.1) for pre-3.8, and specular albedo
  // (3.7) is replicated using the formula in
  // Parser::Parse_Finish() in parstxtr.cpp in the POV-Ray 3.7 source.
  #if (sotd_Gloss > 0)
    #local sotd_Rough = pow (10, -2 - sotd_Gloss * 1.7); // arbitrary!
    #local sotd_Spec_albedo = sotd_Gloss
      * (1 / sotd_Rough + 2) / (4 * (2 - pow (2, -0.5 / sotd_Rough)));
    /*Sotd__DebugF ("reflection {", sotd_Gloss, 2)
    #debug " fresnel } conserve_energy\n"
    Sotd__DebugF ("roughness", sotd_Rough, 6)
    Sotd__DebugF ("specular", sotd_Spec_albedo, 6)*/
    #local sotd_t_Gloss = texture
    { pigment { rgbf 1 }
      finish
      { #if (version >= 3.8) fresnel sotd_Gloss #end
        reflection { sotd_Gloss fresnel } conserve_energy
        #if (!sotd_Reflect)
          roughness sotd_Rough
          specular sotd_Spec_albedo * (version < 3.8? 0.1: 1)
        #end
      }
      #if (sotd_Tile_spacing) Sotd__Tile_n (sotd_Tile_spacing) #end
    }
    #local sotd_t_Checkerboard =
      texture { sotd_t_Matte_check }
      texture { sotd_t_Gloss }
  #else
    #local sotd_t_Checkerboard = texture { sotd_t_Matte_check }
  #end


 //-------------- outdoor scenery------------------

  #if (sotd_Setting < SOTD_INDOORS_CLOSED)

   //++++++++++++ sky +++++++++++++
    #local sotd_sinAlt = sin (radians (sotd_Sun_alt));
    #local sotd_cosAlt = cos (radians (sotd_Sun_alt));
    // A hack: a curve fit to a sampling of Scott Boham's SkySim
    // (Turbidity 1.5, SunPos altitude 45):
    #local Sotd_fn_Curve = function (x)
    { pow (1 - x, 1.34 * pow (1 - x, 0.69) + 1)
    }
    #local Sotd_fn_Halo = function
    { select (z, Sotd_fn_Curve (sqrt (x*x + y*y)), 0)
    }
    #local sotd_Reduce = 0.04; // to prevent pigment function from wrapping

    #switch (sotd__Sky_trial)

      #case (1) // single fog
        #local SOTD_DIST_HIGH = 230000;
        #local SOTD_ALT_HIGH = 18400;
        #switch (sotd__Whitex)
          #case (SOTD__3200K)
            #local SOTD_C_HALO = rgb <0.144, 0.361, 1.458>;
            #local SOTD_C_SKY_HIGH = rgb <0.094, 0.342, 1.877>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.272, 0.443, 1.004>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.051, 0.084, 0.189>;
            #break
          #case (SOTD__4100K)
            #local SOTD_C_HALO = rgb <0.177, 0.332, 0.914>;
            #local SOTD_C_SKY_HIGH = rgb <0.116, 0.315, 1.177>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.335, 0.408, 0.629>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.063, 0.077, 0.119>;
            #break
          #case (SOTD__D50)
            #local SOTD_C_HALO = rgb <0.205, 0.320, 0.658>;
            #local SOTD_C_SKY_HIGH = rgb <0.135, 0.304, 0.847>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.388, 0.393, 0.453>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.073, 0.074, 0.085>;
            #break
          #case (SOTD__D55)
            #local SOTD_C_HALO = rgb <0.218, 0.316, 0.577>;
            #local SOTD_C_SKY_HIGH = rgb <0.143, 0.300, 0.743>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.413, 0.389, 0.397>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.078, 0.073, 0.075>;
            #break
          #case (SOTD__D65)
            #local SOTD_C_HALO = rgb <0.241, 0.312, 0.475>;
            #local SOTD_C_SKY_HIGH = rgb <0.158, 0.296, 0.612>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.456, 0.384, 0.327>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.086, 0.072, 0.062>;
            #break
          #case (SOTD__D75)
            #local SOTD_C_HALO = rgb <0.259, 0.310, 0.415>;
            #local SOTD_C_SKY_HIGH = rgb <0.170, 0.294, 0.534>;
            #local SOTD_C_CLOUDS_LIGHT = rgb <0.491, 0.381, 0.286>;
            #local SOTD_C_CLOUDS_DARK = rgb <0.093, 0.072, 0.054>;
            #break
          #else
            #error concat ("Bug: sotd__Whitex = ", str (sotd__Whitex, 0, -1))
        #end
        // A hack: a curve fit to the fog appearance gradient until I can
        // either figure out what's going on in Trace::ComputeGroundFogColour()
        // or integrate the fog formula myself:
        #local Sotd_fn_Grad = function (y)
        { 0.4286 / (0.4286 + pow (max (y, 0) / 0.35, 1.1 + pow (1 - y, 10)))
        }
        #break // sotd__Sky_trial = 1

      #case (2) // a stab at varying color saturation from zenith to horizon
        #local SOTD_ALT_HIGH = 36700;
        #local SOTD_ALT_LOW = 36700;
        //@@ PLACE HOLDERS BELOW:
        #local SOTD_DIST_HIGH = 100000;
        #local SOTD_ALT_HIGH = 36000;
        #local SOTD_C_SKY_HIGH = rgbt <0.000000, 0.004000, 0.120000, 0>;
        #local SOTD_DIST_LOW = 400000;
        #local SOTD_ALT_LOW = 36000;
        #local SOTD_C_SKY_LOW = rgbt <0.189000, 0.270000, 0.229500, 0>;
        //#local SOTD_C_HALO = rgb <0.6244, 0.7170, 1.0274>;
        // Why 2.5?
        #local SOTD_C_HALO = rgb <0.2505, 0.2877, 0.4123> * 2.5;
        #local SOTD_C_CLOUDS_LIGHT = rgb <0.469, 0.394, 0.336>;
        #local SOTD_C_CLOUDS_DARK = rgb <0.110, 0.092, 0.079>;
        // A hack: an earlier incarnation of the single fog hack, but should
        // really be its own curve:
        #local Sotd_fn_Grad = function (y)
        { 0.04 + 0.96 / (1 + pow (max (y, 0) * 4, 1.5 - 0.1 * y))
        }
        #break // sotd__Sky_trial = 2

      #else // no sky color
        #local SOTD_C_HALO = rgb 1;
        #local SOTD_C_CLOUDS_DARK = rgb 0.36;
        #local SOTD_C_CLOUDS_LIGHT = rgb 0.77;
        #local Sotd_fn_Grad = function (y) { 0 }

    #end // switch sotd__Sky_trial

   // Adjust exposure:
    #local SOTD_C_HALO = SOTD_C_HALO * sotd__Outdoor_bright;
    #ifdef (SOTD_C_SKY_HIGH)
      #local SOTD_C_SKY_HIGH = SOTD_C_SKY_HIGH * sotd__Outdoor_bright;
    #end
    #ifdef (SOTD_C_SKY_LOW)
      #local SOTD_C_SKY_LOW = SOTD_C_SKY_LOW * sotd__Outdoor_bright;
    #end
    #local SOTD_C_CLOUDS_LIGHT = SOTD_C_CLOUDS_LIGHT * sotd__Outdoor_bright;
    #local SOTD_C_CLOUDS_DARK = SOTD_C_CLOUDS_DARK * sotd__Outdoor_bright;

    #ifdef (SOTD_C_SKY_HIGH) // For darker, more saturated blue at zenith
      fog
      { fog_type 2
        distance SOTD_DIST_HIGH * sotd__Scale
        color SOTD_C_SKY_HIGH
        fog_offset 0
        fog_alt SOTD_ALT_HIGH * sotd__Scale
      }
    #end
    #ifdef (SOTD_C_SKY_LOW) // For lighter, less saturated blue at horizon
      fog
      { fog_type 2
        distance SOTD_DIST_LOW * sotd__Scale
        color SOTD_C_SKY_LOW
        fog_offset 0
        fog_alt SOTD_ALT_LOW * sotd__Scale
      }
    #end
    #local sotd_nFogs = defined (SOTD_C_SKY_HIGH) + defined (SOTD_C_SKY_LOW);
    sky_sphere
    { pigment
      { function
        { min
          (  1,
             Sotd_fn_Halo
             ( x,
               z * sotd_sinAlt + y * sotd_cosAlt,
               z * sotd_cosAlt - y * sotd_sinAlt
             )
           / max (1e-10, 1 - Sotd_fn_Grad (y)) * sotd_Reduce
          )
        }
        color_map
        { [0 rgb 0]
          [1 SOTD_C_HALO / sotd_Reduce]
        }
        rotate -sotd_Sun_az * y
      }
    }
    #local sotd_p_Clouds = pigment
    { bumps
      turbulence 0.65
      octaves 6
      omega 0.7
      rotate sotd__Debug_clouds * y
    }
    #local sotd_Darker = SOTD_C_CLOUDS_LIGHT * 5/8 + SOTD_C_CLOUDS_DARK * 3/8;
    #local sotd_Lighter = (SOTD_C_CLOUDS_LIGHT + SOTD_C_CLOUDS_DARK) / 2;
    #local sotd_Cfactor = asin (2 * sotd_Cloudy - 1) / pi + 0.5;
    #local sotd_Clear_exp = ln (0.4) / ln (0.5);
    #local sotd_Dark_opq = pow (sotd_Cfactor, 2);
    #local sotd_f_Clouds = finish
    { diffuse 0 ambient #if (version >= 3.7) 0 emission #end 1
    }
    union // Note: clouds are modeled in feet, then converted to meters.
    { sphere
      { -SOTD__REARTH * y, SOTD__REARTH + 5250
        hollow
        pigment
        { sotd_p_Clouds
          color_map
          { [sotd_Cfactor * 0.4 SOTD_C_CLOUDS_LIGHT]
            [sotd_Cfactor * 0.7 sotd_Darker]
            [sotd_Cfactor sotd_Darker transmit 1]
          }
          scale 4000
          translate <sotd_xClouds, 0, sotd_zClouds> / SOTD_FOOT
        }
        finish { sotd_f_Clouds }
      }
      sphere
      { -SOTD__REARTH * y, SOTD__REARTH + 5000
        hollow
        pigment
        { sotd_p_Clouds
          color_map
          { [sotd_Dark_opq * (0.6 - 0.4 * sotd_Cfactor) SOTD_C_CLOUDS_DARK]
            [sotd_Dark_opq sotd_Lighter]
            [pow (sotd_Cfactor, sotd_Clear_exp) sotd_Lighter transmit 1]
          }
          scale 4000
          translate <sotd_xClouds, 0, sotd_zClouds> / SOTD_FOOT
        }
        finish { sotd_f_Clouds }
      }
      scale sotd__Scale
      photons { pass_through }
    }

   //++++++++++ Sun +++++++++++++
    #local SOTD_RSUNRATIO = 215;
    #local sotd_rSun = sotd__Sun_dist / SOTD_RSUNRATIO;
    // sky area / solar disc area:
    #local sotd_Surface_brightness =
      2 * pow (SOTD_RSUNRATIO, 2) * sotd__Outdoor_bright;
    light_source
    { sotd_v_Sun, sotd_c_Sun
      parallel point_at 0
      #if (sotd_Area)
        #local sotd_dSun =
          2 * sotd_rSun * sotd_Area * Sotd__fn_Area_adj (sotd_Area_res);
        area_light sotd_dSun * x, sotd_dSun * z, sotd_Area_res, sotd_Area_res
        circular orient adaptive sotd_Area_adaptive jitter
      #else
        #local sotd_dSun = 0;
      #end
      looks_like
      { sphere
        { 0, sotd_rSun
          pigment { sotd_c_Sun }
          finish
          { diffuse 0
            #if (version >= 3.7)
              ambient 0 emission sotd_Surface_brightness
            #else
              ambient sotd_Surface_brightness
            #end
          }
          #if (version >= 3.7) no_radiosity #end
          #if (!sotd_Reflect) no_reflection #end
        }
      }
    }
    #if (sotd_Debug >= 1)
      Sotd__Echo_light_source
      ( sotd_v_Sun, <0, 0, 0>, sotd_c_Sun, false, 0, 0, 0, sotd_dSun, yes
      )
    #end

   //+++++++++ ground +++++++++++
    #local sotd_t_Dirt = texture
    { pigment { rgb SOTD__C_DIRT }
      finish { sotd__f_Ground1 }
    }
    #local sotd_t_Grass = texture
    { pigment { rgb SOTD__C_GRASS }
      finish { sotd__f_Ground1 }
      normal { bumps 1.25 warp { turbulence 0.5 omega 1 } scale 0.05 }
    }
    #local sotd_yPlain =
      (sotd_Setting < SOTD_OUTDOOR_PATIO? 0: -SOTD__HFOUNDATION);
    #if (sotd__Debug_unit) Sotd__DebugF ("sotd_yPlain", sotd_yPlain, 6) #end
    // A cylinder is used here instead of a plane to
    // prevent radiosity artifacts in POV-Ray 3.7.0.
    cylinder
    { 0, -y, max (sotd_Hills_max, 1500) / SOTD_FOOT * 20
      translate sotd_yPlain * y
      #if (sotd_Setting = SOTD_OUTDOOR_CHECK)
        texture { sotd_t_Checkerboard scale 1 / sotd__Scale }
        #if (sotd_Gloss > 0) interior { ior 1.486 } #end
      #else #if (version < 3.7)
        texture
        { bozo texture_map { [0.45 sotd_t_Grass] [1.0 sotd_t_Dirt] } scale 3
        }
      #else
        texture
        { aoi texture_map
          { [0.5 sotd_t_Grass]
            [0.55 bozo
             texture_map { [0.2 sotd_t_Grass] [0.8 sotd_t_Dirt] }
             scale 3
            ]
          }
        }
      #end
      #end
      scale sotd__Scale
      translate -sotd_Microspace * y
    }

   //+++++++++ hills ++++++++++++
    #if (sotd_Hills_max > 0)
      #local sotd_v_Hills = (sotd_yPlain - sotd_Microspace / sotd__Scale) * y;
      #if (sotd__Debug_unit) Sotd__DebugV ("sotd_v_Hills", sotd_v_Hills, 6) #end
      #local SOTD_DTREE = 30; // If these values are changed, then the
      #local SOTD_HTREE = 60; // max gradient must be reformulated.
      #local sotd_t_Fake_trees = texture
      { pigment
        { bozo warp { turbulence 0.5 omega 0.8 }
          color_map
          { [0.45 rgb SOTD__C_BROADLEAF * SOTD__FAKE_FOREST_SHADE]
            [0.55 rgb SOTD__C_CONIFER * SOTD__FAKE_FOREST_SHADE]
          }
        }
        normal
        { bozo 3.5 warp { turbulence 0.5 omega 0.8 }
          slope_map { [0.0, <1, 0>] [0.5 <0, -2>] [0.5 <0, 2>] [1.0 <1, 0>] }
        }
        finish { sotd__f_Forest1 }
      }
      #local sotd_Rugged = sqrt (sotd_Hills_max * 0.000026);
      #local Sotd_fn_Hills = function
      { y + f_noise_generator (z * 0.8, sotd_Hills_param, x * 0.8, 2) - 1
          + f_hetero_mf (x, 0, z, 1, 2, 5, 0, 0, 2) * sotd_Rugged
      }
      #local Sotd_fn_Valley = function
      { -0.5 * (cos (min (sqrt (x*x + z*z) * pi * 0.2, pi)) + 1)
      }
     // Height field hills
      #if (sotd_Hills_res >= 2)
        #local sotd_Hills = height_field
        { function sotd_Hills_res, sotd_Hills_res
          { max
            ( 0,
              Sotd_fn_Valley ((x - 0.5) * 20, 0, (y - 0.5) * 20)
            - Sotd_fn_Hills ((x - 0.5) * 20, 0, (0.5 - y) * 20)
            )
          }
          smooth
          translate -<0.5, 0, 0.5>
          scale <20, 1, 20> * sotd_Hills_max
          #if (sotd_Trees_level < 1)
            texture
            { sotd_t_Fake_trees
              scale SOTD_DTREE * SOTD_FOOT
            }
          #else
            texture
            { pigment
              { rgb (SOTD__C_GRASS * SOTD__GRASS_SHADE + SOTD__C_DIRT) / 2
              }
              finish { sotd__f_Forest1 }
            }
          #end
        }
        #if (sotd_Trees_level >= 1)
          #local sotd_NPotential = 0; // dummy value
          #local sotd_NBroadleaf = 0; // dummy value
          #local sotd_NConifer = 0; // dummy value
          union
          { Sotd__Grow_forest
            ( sotd_Hills, 0, sotd_NPotential, sotd_NBroadleaf
            )
            Sotd__Grow_forest
            ( sotd_Hills, 1, sotd_NPotential, sotd_NConifer
            )
            translate sotd_v_Hills
            scale sotd__Scale / SOTD_FOOT
          }
          #if (sotd_Debug >= 1)
            #debug concat
            ( "ShapeOfTheDay: ", str (sotd_NBroadleaf + sotd_NConifer, 0, 0),
              " of ", str (sotd_NPotential, 0, 0), " trees were instantiated.\n"
            )
          #end
        #end
     // Isosurface hills
      #else
        // Note: hills were originally scaled in feet.  Because the isosurface
        // trees function is likely to be replaced, rather than waste effort
        // figuring out a new max gradient, I am just converting back to feet.
        #local sotd_Hills_ft = sotd_Hills_max / SOTD_FOOT;
        #if (sotd__Debug_unit)
          Sotd__DebugF ("sotd_Hills_ft", sotd_Hills_ft, 6)
        #end
        #local Sotd_fn_Roots = function
        { f_noise_generator (z / SOTD_DTREE, 0, x / SOTD_DTREE, 3)
        }
        #local Sotd_fn_Tree = function (x)
        { select
          ( x - 0.5,
            (1 - pow (2 * x, 2)) * 0.4,
            pow (2 * (x - 0.5), 2)
          )
        }
        #local sotd_Hills = isosurface
        { function
          { Sotd_fn_Hills
            ( x / sotd_Hills_ft,
              ( y
              - Sotd_fn_Valley
                ( x / sotd_Hills_ft, y / sotd_Hills_ft, z / sotd_Hills_ft
                ) * sotd_Hills_ft
              ) / sotd_Hills_ft,
              z / sotd_Hills_ft
            )
            #if (sotd_Trees_level >= 1)
              - Sotd_fn_Tree (Sotd_fn_Roots (x, y, z))
                  / sotd_Hills_ft * SOTD_HTREE
            #end
          }
          contained_by
          { box
            { -<sotd_Hills_ft * 10, 1, sotd_Hills_ft * 10>,
              < sotd_Hills_ft * 10,
                sotd_Hills_ft + (sotd_Trees_level < 1? 0: SOTD_HTREE),
                sotd_Hills_ft * 10
              >
            }
          }
          #local sotd_MaxG =
          ( sotd_Trees_level < 1?
            2 / sotd_Hills_ft:
            5.6 / (sotd_Hills_ft + 19)
          );
          #if (defined (RE_Evaluate) & version < 3.7)
            RE_Evaluate (sotd_MaxG, 0.6, 0.7)
          #else
            max_gradient sotd_MaxG
          #end
          #local sotd_Accu0 = sotd_Hills_ft * 0.001;
          #local sotd_Accu = sotd_Accu0 * sotd__Scale;
          #local sotd_Accu_final = min (1, max (0.001, sotd_Accu));
          accuracy sotd_Accu_final
          #if (sotd__Debug_unit)
            Sotd__DebugF ("max_gradient", sotd_MaxG, 6)
            Sotd__DebugF ("sotd_Accu0", sotd_Accu0, 6)
            Sotd__DebugF ("sotd_Accu", sotd_Accu, 6)
            Sotd__DebugF ("accuracy", sotd_Accu_final, 6)
          #end
          texture
          { #if (sotd_Trees_level < 1)
              sotd_t_Fake_trees
              scale SOTD_DTREE
            #else
              pigment
              { function { Sotd_fn_Roots (x, y, z) }
                color_map
                { [0.5 rgb SOTD__C_BROADLEAF]
                  [0.5 rgb SOTD__C_CONIFER]
                }
              }
              normal
              { granite 1
                scale <40, 25, 40>
              }
              finish { sotd__f_Forest1 }
            #end
          }
        }
      #end // isosurface
      object
      { sotd_Hills
        #if (sotd_Hills_res >= 2)
          translate sotd_v_Hills
          scale sotd__Scale / SOTD_FOOT
        #else
          translate sotd_v_Hills / SOTD_FOOT
          scale sotd__Scale
        #end
        translate -0.000001 * y
      }
    #end // hills

  #end // outdoor scenery

 //-------------- checkered floor -----------------

  #local sotd_dWall = #switch (sotd_Setting)
    #case (SOTD_OUTDOOR_PATIO) (SOTD__LBRICK - SOTD__MORTAR / 2); #break
    #case (SOTD_OUTDOOR_PATIO_RAIL) SOTD__DRAIL; #break
    #else SOTD__DWALL;
  #end

  #if (sotd_Setting >= SOTD_OUTDOOR_PATIO)
    #local sotd_xFloor =
      sotd_v_Room_size.x + (sotd_dWall - SOTD__MICROABS) * sotd__Scale;
    box
    { -1, 1 - y scale <sotd_xFloor, 1 / sotd_Unit, sotd_xFloor>
      texture { sotd_t_Checkerboard }
      #if (sotd_Gloss > 0) interior { ior 1.486 } #end
      translate -sotd_Microspace * y
    }
  #end

 //---------------- patio walls -------------------

  #switch (sotd_Setting)
    #case (SOTD_OUTDOOR_PATIO)
      #local sotd_t_Bricks1 = texture
      { pigment
        { brick SOTD__C_MORTAR SOTD__C_BRICK
          brick_size <SOTD__LBRICK, SOTD__HBRICK, SOTD__LBRICK * 2>
          mortar SOTD__MORTAR
        }
        finish { sotd__f_Default1 }
        normal
        { brick 0.06
          brick_size <SOTD__LBRICK, SOTD__HBRICK, SOTD__LBRICK * 2>
          mortar SOTD__MORTAR
          accuracy 0.006
        }
        translate <0.5, 1, 0.5> * SOTD__MORTAR + SOTD__LBRICK / 2 * z
        scale sotd__Scale
        translate sotd_v_Room_size * z
      }
      #local sotd_t_Bricks2 = texture
      { sotd_t_Bricks1
        translate SOTD__HBRICK * sotd__Scale * y
      }
      object
      { Sotd__Build_patio_wall (sotd_Front)
        texture { sotd_t_Bricks1 }
      }
      object
      { Sotd__Build_patio_wall (sotd_Right)
        texture { sotd_t_Bricks2 }
        rotate 90 * y
      }
      object
      { Sotd__Build_patio_wall (sotd_Back)
        texture { sotd_t_Bricks1 }
        rotate 180 * y
      }
      object
      { Sotd__Build_patio_wall (sotd_Left)
        texture { sotd_t_Bricks2 }
        rotate 270 * y
      }
      #break
    #case (SOTD_OUTDOOR_PATIO_RAIL)
      object { Sotd__Build_railing (sotd_Front) }
      object { Sotd__Build_railing (sotd_Right) rotate 90 * y }
      object { Sotd__Build_railing (sotd_Back) rotate 180 * y }
      object { Sotd__Build_railing (sotd_Left) rotate 270 * y }
      // Exterior paint, in case user goes snooping outside:
      #local sotd_extWall = sotd_v_Room_size.x + sotd_dWall * sotd__Scale;
      box
      { -1, 1 - y scale <sotd_extWall, 1 / sotd_Unit, sotd_extWall>
        translate -(sotd_Microspace + SOTD__MICROABS * sotd__Scale) * y
        pigment { Sotd__Exterior_c() }
        finish { sotd__f_Outdoor }
      }
      #break
  #end

 //------------------ indoors ---------------------

  #if (sotd_Setting >= SOTD_INDOORS_OPEN)

    #local sotd_t_Brass = texture
    { pigment { rgb <0.80, 0.57, 0.23> }
      finish
      { reflection { 0.9 metallic }
        diffuse 0.05 ambient sotd_c_Ambient * 0.05
        #if (version >= 3.7) emission 0 #end
        #if (!sotd_Reflect) specular 113 metallic roughness 0.001 #end
      }
    }

   //+++++++++ walls ++++++++++++
    #local sotd_Window = Sotd__Make_window (sotd_t_Brass)
    #local sotd_Door = Sotd__Make_door (sotd_t_Brass)
    //@@#local sotd_Door = box { 0, 1 pigment { red 1 } }
    #local sotd_nWindowed =
    (  (sotd_Front = SOTD_WINDOWS) + (sotd_Back = SOTD_WINDOWS)
     + (sotd_Left = SOTD_WINDOWS) + (sotd_Right = SOTD_WINDOWS)
    );
    #local sotd_Per_wall = floor (sotd_Max_windows / max (sotd_nWindowed, 1));
    #if (sotd__Debug_unit)
      Sotd__DebugF ("max windows per wall", sotd_Per_wall, 0)
    #end
    Sotd__Build_wall (sotd_Front, sotd_Window, sotd_Door, sotd_Per_wall)
    object
    { Sotd__Build_wall (sotd_Right, sotd_Window, sotd_Door, sotd_Per_wall)
      rotate 90 * y
    }
    object
    { Sotd__Build_wall (sotd_Back, sotd_Window, sotd_Door, sotd_Per_wall)
      rotate 180 * y
    }
    object
    { Sotd__Build_wall (sotd_Left, sotd_Window, sotd_Door, sotd_Per_wall)
      rotate 270 * y
    }

   //++++++++ ceiling +++++++++++
    box
    { y - 1, 1 scale sotd_v_Room_size / sotd__Scale * <1, 0, 1> + 2
      pigment { sotd_c_Ceiling }
      translate sotd_v_Room_size / sotd__Scale * y
      scale sotd__Scale
    }

   //+++++++++ lights +++++++++++
    #local sotd_hsv_Wall = CRGB2HSV (sotd_c_Wall);
    #local sotd_c_Lamp = CHSV2RGB
    ( <sotd_hsv_Wall.x, min (sotd_hsv_Wall.y * 2, 1), sotd_hsv_Wall.z * 0.5>
    );
    #if (version < 3.71)
      #local sotd_t_Lamp = texture { pigment { sotd_c_Lamp } }
      texture
      { pigment { rgbf 1 }
        finish
        { reflection { 1 fresnel } conserve_energy
          specular (sotd_Reflect? 0: 6.08) roughness 0.001
        }
      }
    #else
      #local sotd_t_Lamp = texture
      { pigment { sotd_c_Lamp }
        finish
        { fresnel
          reflection { 1 fresnel } conserve_energy
          specular albedo (sotd_Reflect? 0: 1) roughness 0.001
        }
      }
    #end
    #local sotd_v_Rest = sotd_Height/2 * y;
    #local sotd_dMain = vlength (sotd_v_Light - sotd_v_Rest);
    #local sotd_Shadowless = (sotd_Quality < 2? 0: 2);
    #if (sotd_Shadowless = 0)
      #local sotd_Fill1_factor = 1;
      #local sotd_Fill2_factor = 1;
    #else // Adjust brightnesses for differing distances from rest point.
      #local sotd_v_Rest = sotd_Height/2 * y;
      #local sotd_dMain = vlength (sotd_v_Light - sotd_v_Rest);
      #local sotd_Fill1_factor =
        pow (vlength (sotd_v_Fill1 - sotd_v_Rest) / sotd_dMain, 2);
      #local sotd_Fill2_factor =
        pow (vlength (sotd_v_Fill2 - sotd_v_Rest) / sotd_dMain, 2);
    #end
    Sotd__Lamp
    ( sotd_v_Light,
      sotd__c_Light_unf * Sotd__Filter ((sotd_Light < 0), (sotd_Quality != 0)),
      (sotd_Light < 0 ? -1: <sotd_hLight, sotd_sLight>),
      sotd_t_Lamp, (sotd_Quality = 0? 1: 2), yes, yes
    )
    Sotd__Lamp
    ( sotd_v_Fill1,
      sotd__c_Fill1_unf * sotd_Fill1_factor
        * Sotd__Filter ((sotd_Fill1 < 0), (sotd_Shadowless = 2)),
      (sotd_Fill1 < 0 ? -1: <sotd_hFill1, sotd_sFill1>),
      sotd_t_Lamp, sotd_Shadowless, sotd_Fill_area, sotd_Fill_ph
    )
    Sotd__Lamp
    ( sotd_v_Fill2,
      sotd__c_Fill2_unf * sotd_Fill2_factor
        * Sotd__Filter ((sotd_Fill2 < 0), (sotd_Shadowless = 2)),
      (sotd_Fill2 < 0 ? -1: <sotd_hFill2, sotd_sFill2>),
      sotd_t_Lamp, sotd_Shadowless, sotd_Fill_area, sotd_Fill_ph
    )
    #declare Lamp_Debug = sotd_Lamp_debug; // restore user's DeskLamp setting

  #end

#end //macro Sotd_Setup

/**************************************************************************
 * Returns a transformation according to the arguments.
 */
#macro Sotd_Position (Pitch, Roll, Yaw, Nudge, Lift, Push)
  #local sotd_Move = <Nudge, sotd_Height/2 + Lift, Push>;
  #if (sotd_Debug >= 1)
    #debug "Sotd_Position()\n  transform\n  { rotate "
    #debug str (Pitch, 0, 4)
    #debug " * x\n    rotate "
    #debug str (-Roll, 0, 4)
    #debug " * z\n    rotate "
    #debug str (-Yaw, 0, 4)
    #debug " * y\n    translate "
    #debug Sotd__VStr (sotd_Move, 5)
    #debug "\n  }\n"
  #end
  transform
  { rotate Pitch * x
    rotate -Roll * z
    rotate -Yaw * y
    translate sotd_Move
  }
#end

/**************************************************************************
 * Returns a transformation according to the global positioning parameters.
 */
#macro Sotd_Position_std()
  Sotd_Position
  ( sotd_Pitch, sotd_Roll, sotd_Yaw, sotd_Nudge, sotd_Lift, sotd_Push
  )
#end

#version ShapeOfTheDay_Inc_Temp;
#end
// end of shapeoftheday.inc
