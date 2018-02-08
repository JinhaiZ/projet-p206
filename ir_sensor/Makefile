SDK= OEM

CFLAGS = -Wall
EXEC_NAME = flir
CC = g++
INCLUDES =
SDK_DIR = leptonSDKEmb32$(SDK)PUB
LIB_DIR =/usr/lib
LIBS = -L/usr/lib/arm-linux-gnueabihf $(LIB_DIR)/libopencv_calib3d.so $(LIB_DIR)/libopencv_contrib.so $(LIB_DIR)/libopencv_core.so $(LIB_DIR)/libopencv_features2d.so $(LIB_DIR)/libopencv_flann.so $(LIB_DIR)/libopencv_gpu.so $(LIB_DIR)/libopencv_highgui.so $(LIB_DIR)/libopencv_imgproc.so $(LIB_DIR)/libopencv_legacy.so $(LIB_DIR)/libopencv_ml.so  $(LIB_DIR)/libopencv_objdetect.so $(LIB_DIR)/libopencv_video.so  -lrt -lm -ldl  -L$(SDK_DIR)/Debug -lLEPTON_SDK



OBJ_FILES = main.o SPI.o Lepton_I2C.o
INSTALL_DIR = /usr/bin

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

Lepton_I2C.o: Lepton_I2C.cpp Lepton_I2C.h \
	$(SDK_DIR)/LEPTON_SDK.h \
	$(SDK_DIR)/LEPTON_Types.h \
	$(SDK_DIR)/LEPTON_ErrorCodes.h \
	$(SDK_DIR)/LEPTON_SDKConfig.h \
	$(SDK_DIR)/LEPTON_I2C_Protocol.h \
	$(SDK_DIR)/LEPTON_I2C_Service.h \
	$(SDK_DIR)/LEPTON_SYS.h \
	$(SDK_DIR)/LEPTON_AGC.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Lepton_I2C.o Lepton_I2C.cpp

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -o $@ -c $<

install :
	cp $(EXEC_NAME) $(INSTALL_DIR)