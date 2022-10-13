/* shapeoftheday_cloudy.pov
 * Persistence of Vision Raytracer scene description file
 *
 * A mirrored dome for coordinating ShapeOfTheDay clouds with solar position.
 *
 * Copyright © 2019, 2021 Richard Callwood III.  All rights reserved.
 *
 * 2019-Sep-30  Created.
 * 2021-Jan-02  The z-shift is echoed.
 */
#version 3.5;

#declare sotd_Setting = 0;
#ifndef (sotd_Exposure) #declare sotd_Exposure = 2; #end

#include "screen.inc"
#include "shapeoftheday.inc"
Sotd_Setup (no)

sphere
{ 0, 2
  pigment { rgb 0 }
  finish
  { reflection { 1 }
    ambient 0 diffuse 0
    specular 12500
    roughness 0.00001
  }
}

#macro Text (s_Text, uv_Where)
  Screen_Object
  ( text { ttf "cyrvetic" s_Text 0.001, 0 scale 0.065 pigment { rgb 0 } },
    uv_Where, <0.015, 0.01>, yes, 1
  )
#end

Text (concat ("sotd_Sun_alt = ", str (sotd_Sun_alt, 0, 1)), <0, 1>)
Text (concat ("sotd_Sun_az = ", str (sotd_Sun_az, 0, 1)), <0, 0.92>)
Text (concat ("sotd_xClouds = ", str (sotd_xClouds, 0, 0)), <1, 1>)
Text (concat ("sotd_zClouds = ", str (sotd_zClouds, 0, 0)), <1, 0.92>)
