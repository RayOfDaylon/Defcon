#pragma once
/*
	gamma.h
	Defcon gamma function.
	Copyright 2003-2004 Daylon Graphics Ltd.
*/

#include <math.h>

// gamma() modifies a normalized gray ramp so that 
// brightness intensities appear intuitively correct.
#define gamma(_f) ( (float)pow(_f, 0.5) ) 


