#ifndef LEPTON_I2C
#define LEPTON_I2C

#define OEM

#ifdef OEM

# include "leptonSDKEmb32OEMPUB/LEPTON_SDK.h"
#include "leptonSDKEmb32OEMPUB/LEPTON_SYS.h"
#include "leptonSDKEmb32OEMPUB/LEPTON_OEM.h"
#include "leptonSDKEmb32OEMPUB/LEPTON_Types.h"

#else

# include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"
#endif



void lepton_perform_ffc();

#endif
