/* shapeoftheday.pov
 * Persistence of Vision Raytracer scene description include file
 * A proposed POV-Ray Object Collection Demo.
 *
 * Demonstrates usage of shapeoftheday.inc.
 *
 * Copyright (C) 2022 Richard Callwood III.  Some rights reserved.
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
 * Vers  Date         Notes
 * ----  ----         -----
 * 2.0   2022-Nov-01  Initial demo.
 */
#version max (3.5, min (3.8, version));

#ifndef (Rad) #declare Rad = no; #end

// Use animation to rotate the object.
#if (clock_on) #declare sotd_Yaw = clock * 360; #end

#include "shapeoftheday.inc"
Sotd_Setup (Rad)
#if (Rad)
  global_settings
  { radiosity
    { count 200
      error_bound 0.5
      #if (version < 3.7)
        pretrace_start 0.08
        pretrace_end 0.01
      #else
        pretrace_start 64 / image_width
        pretrace_end 2 / image_width
      #end
      recursion_limit 2
    }
  }
#end

// An arbitrary shape.  For this demo, we'll make a trefoil knot.
#declare N = 200;
#declare rMinor = 0.15;
#declare RMajor = 1 - rMinor;
union
{ #local Tix = 0;
  #while (Tix <= N) // Always count with integers!
    #local T = Tix / N * 2 * pi; // then convert to real
    #local NewPt =
      <2 * sin (2 * T) - sin (T), 2 * cos (2 * T) + cos (T), sin (3 * T)>
      * RMajor / 3;
    #local c_New = CHSV2RGB (<Tix / N * 360, 0.9, 1>);
    #if (Tix > 0)
      sphere
      { NewPt, rMinor
        pigment { c_New }
      }
      cylinder
      { OldPt, NewPt, rMinor
        pigment
        { gradient NewPt - OldPt
          color_map { [0 c_Old] [1 c_New] }
          scale vlength (NewPt - OldPt)
          translate OldPt
        }
      }
    #end
    #local OldPt = NewPt;
    #local c_Old = c_New;
    #local Tix = Tix + 1;
  #end
  Sotd_Position_std() // Position the object within the scene
}
// end of shapeoftheday.pov
