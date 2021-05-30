#define BYTE unsigned char
struct Frame {
  // 2 bytes
  int length; // 7 bits
  int cmd; // 4 bits
  int sa; // 5 bits
  // 127 bytes
  BYTE data[127]; // 127 bytes
  // total 129 bytes
  BYTE frame[129]; // 127 + 2 bytes
};

void generateFrameToSend(Frame &f);

void generateReceivedFrame(Frame &f);

void getByteArrayOfInteger(int v, BYTE arr[]); 

void getIntegerOfByteArray(BYTE arr[], int *v_ptr);

void doCmdAction(int cmd);
