/* shapeoftheday_ref.inc version 2.0
 * Persistence of Vision Raytracer scene description include file
 *
 * ShapeOfTheDay tools that may be needed prior to #including shapeoftheday.inc.
 *
 * Copyright © 2020 - 2022 Richard Callwood III.  Some rights reserved.
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
 * Vers.  Date         Comments
 * -----  ----         --------
 * 1.8    2020-Nov-08  Created.
 * 1.9.7  2022-Sep-13  Last update.
 * 2.0    2022-Sep-15  Version match
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (ShapeOfTheDay_Ref_Inc_Temp)
#declare ShapeOfTheDay_Ref_Inc_Temp = version;
#version max (3.5, min (3.8, version)); // Bracket the POV version.
#declare SHAPEOFTHEDAY__REF_VERSION = 2.0;

#ifdef (View_POV_Include_Stack)
  #debug "#include \"shapeoftheday_ref.inc\"\n"
#end

#include "math.inc"
#include "sunpos.inc"

//----------------- lighting and colors --------------------

// Standard illuminants
#declare SOTD_ILLUM_A = -100;
#declare SOTD_ILLUM_B = -200;
#declare SOTD_ILLUM_C = -300;
#declare SOTD_ILLUM_D50 = -450;
#declare SOTD_ILLUM_D55 = -455;
#declare SOTD_ILLUM_D65 = -465;
#declare SOTD_ILLUM_D75 = -475;
#declare SOTD_ILLUM_E = -500;
#declare SOTD_ILLUM_F1 = -601;
#declare SOTD_ILLUM_F2 = -602;
#declare SOTD_ILLUM_F3 = -603;
#declare SOTD_ILLUM_F4 = -604;
#declare SOTD_ILLUM_F5 = -605;
#declare SOTD_ILLUM_F6 = -606;
#declare SOTD_ILLUM_F7 = -607;
#declare SOTD_ILLUM_F8 = -608;
#declare SOTD_ILLUM_F9 = -609;
#declare SOTD_ILLUM_F10 = -610;
#declare SOTD_ILLUM_F11 = -611;
#declare SOTD_ILLUM_F12 = -612;

//---------- units of length -------------------------------

#declare SOTD_MM = 0.001;
#declare SOTD_CM = 0.01;
#declare SOTD_DM = 0.1;
#declare SOTD_METER = 1;
#declare SOTD_METRE = 1;
#declare SOTD_FOOT = 0.3048;
#declare SOTD_INCH = SOTD_FOOT / 12;
#declare SOTD_HAND = SOTD_FOOT / 3;
#declare SOTD_YARD = SOTD_FOOT * 3;

//---------------- other reference codes -------------------

#declare SOTD_OUTDOORS = 0;
#declare SOTD_OUTDOOR_CHECK = 0.5;
#declare SOTD_OUTDOOR_PATIO = 1;
#declare SOTD_OUTDOOR_PATIO_RAIL = 1.5;
#declare SOTD_INDOORS_OPEN = 2;
#declare SOTD_INDOORS_CLOSED = 3;
#declare SOTD_BLANK_WALL = 0;
#declare SOTD_DOOR_ON_LEFT = 1;
#declare SOTD_DOOR_ON_RIGHT = 2;
#declare SOTD_ENTRANCE_ON_LEFT = SOTD_DOOR_ON_LEFT;
#declare SOTD_ENTRANCE_ON_RIGHT = SOTD_DOOR_ON_RIGHT;
#declare SOTD_WINDOWS = 4;

//------------------------ macros --------------------------

global_settings
{ assumed_gamma 1 // Prevent the first two macros from aborting in v3.7+
}

#macro Sotd_Color (Color)
  #local sotd_C = color Color;
  #if (VMin (sotd_C) < 0 | VMax (sotd_C) > 1)
    #warning concat
    ( "Sotd_Color(): red, green, and blue should be from 0 to 1, but  <"
      vstr (5, sotd_C, ", ", 0, 4), "> was passed in."
    )
  #end
  // See "Stock colors and assumed_gamma 1 in POV-Ray 3.6" (2020-Oct-12) in
  // povray.binaries.images for why the formulas differ by POV version.
  // https://news.povray.org/5f83d3cb%40news.povray.org
  #if (version < 3.7)
    #local sotd_Out = rgbft
    < pow (sotd_C.red, 2.2), pow (sotd_C.green, 2.2), pow (sotd_C.blue, 2.2),
      sotd_C.filter, sotd_C.transmit
    >;
  #else
    #local sotd_Out = srgbft sotd_C;
  #end
  sotd_Out
#end

#macro Sotd_Colour (Colour)
  Sotd_Color (Colour)
#end

#macro Sotd_SunPos (Year, Month, Day, Hour, Minute, Lstm, LAT, LONG)
  #local sotd_Dummy = SunPos (Year, Month, Day, Hour, Minute, Lstm, LAT, LONG);
  #declare sotd_Sun_alt = Al;
  #declare sotd_Sun_az = 180 - Az;
#end

#version ShapeOfTheDay_Ref_Inc_Temp;
#end
// end of shapeoftheday_ref.inc
