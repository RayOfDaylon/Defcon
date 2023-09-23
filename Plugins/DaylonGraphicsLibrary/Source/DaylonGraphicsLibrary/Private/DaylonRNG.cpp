// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#include "DaylonRNG.h"
#include "DaylonRange.h"


Daylon::MTRand Daylon::Rng;

double Daylon::FRand      () { return Rng.rand(); }
double Daylon::FRandRange (double Min, double Max) { return (Min + Rng.rand(Max - Min)); }
int32  Daylon::RandRange  (int32  Min, int32  Max) { return (Min + Rng.randInt(Max - Min)); }
bool   Daylon::RandBool   () { return (Rng.rand() >= 0.5); }

