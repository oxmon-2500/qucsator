/*
 * diode.cpp - diode class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: diode.cpp,v 1.6 2004/04/28 14:39:19 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "diode.h"

diode::diode () : circuit (2) {
  type = CIR_DIODE;
}

void diode::calcSP (nr_double_t frequency) {
  complex z = getOperatingPoint ("gd");
  z += rect (0, getOperatingPoint ("Cd") * 2.0 * M_PI * frequency);
  z = 1.0 / z / z0; 
  setS (1, 1, z / (z + 2.0));
  setS (2, 2, z / (z + 2.0));
  setS (1, 2, 2.0 / (z + 2.0));
  setS (2, 1, 2.0 / (z + 2.0));
}

void diode::initDC (void) {
  setV (1, 0.0);
  setV (2, 0.9);
}

void diode::calcDC (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t n = getPropertyDouble ("n");
  nr_double_t Ud, Id, Ut, T, gd, Ieq, Ucrit;

  T = 290.0;
  Ut = kB * T / Q;
  Ud = real (getV (2) - getV (1));
  Ucrit = Ut * log (Ut / sqrt (2) / Is);
  //if (Ud > Ucrit) {
  //  Ud = Ucrit;
  //}
  gd = Is / Ut / n * exp (Ud / Ut / n);
  //fprintf(stderr, "gd=%g, Ud=%g\n", gd, Ud);
  Id = Is * (exp (Ud / Ut / n) - 1);
  Ieq = Id - Ud * gd;

  setI (1, +Ieq);
  setI (2, -Ieq);

  setY (1, 1, +gd); setY (2, 2, +gd);
  setY (1, 2, -gd); setY (2, 1, -gd);
}

void diode::calcOperatingPoints (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t n = getPropertyDouble ("n");
  nr_double_t z = getPropertyDouble ("z");
  nr_double_t cj0 = getPropertyDouble ("Cj0");
  nr_double_t vd = getPropertyDouble ("Vd");
  
  nr_double_t Ud, Id, Ut, T, gd, Cd;

  T = 290.0;
  Ut = kB * T / Q;
  Ud = real (getV (2) - getV (1));
  gd = Is / Ut / n * exp (Ud / Ut / n);
  Id = Is * (exp (Ud / Ut / n) - 1);

  if (Ud < 0)
    Cd = cj0 * pow (1 - Ud / vd, -z);
  else
    Cd = cj0 * (1 + z * Ud / vd);

  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Ud", Ud);
  setOperatingPoint ("Cd", Cd);
}