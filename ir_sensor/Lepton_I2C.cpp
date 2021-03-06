

#include "Lepton_I2C.h"
bool _connected;

LEP_CAMERA_PORT_DESC_T _port;
int lepton_connect() {
	LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	_connected = true;
	return 0;
}

#ifdef OEM
void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
        LEP_RunOemFFC(&_port);
}
#else
void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
      LEP_RunSysFFCNormalization(&_port);

}
#endif


