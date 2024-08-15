/* shapeoftheday_cloudy.pov version 2.1
 * Persistence of Vision Raytracer scene description file
 * POV-Ray Object Collection
 *
 * A mirrored dome for coordinating ShapeOfTheDay clouds with solar position.
 *
 * Copyright  2019 - 2024 Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the GNU-LGPL
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
 * Vers  Date         Comments
 * ----  ----         --------
 *       2019-Sep-30  Created.
 *       2021-Jan-02  The z-shift is echoed.
 * 2.0   2022-Nov-01  Uploaded.
 * 2.1   2024-Aug-14  No change.
 */
#version max (3.5, min (3.8, version));

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

// end of shapeoftheday_cloudy.pov
