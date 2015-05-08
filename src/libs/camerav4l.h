#include "utils.h"
#define MAXNUM_CAMS 64

typedef struct {
	unsigned flag;
	const char *str;
} flag_def;

enum videoCapControlType {ctrlInteger, ctrlBoolean, ctrlMenu, ctrlButton}; 

typedef struct {
	videoCapControlType typeControl;
	int min,max,step,default_val,value,id;
	std::string name;
} videoCapControl;

typedef std::vector<std::string> dev_vec;
typedef std::map<std::string, std::string> dev_map;
typedef std::map<int, std::string> camsMap;

static bool is_v4l_dev(const char *name);
static bool sort_on_device_name(const std::string &s1, const std::string &s2);
static int calc_node_val(const char *s);

void getCamDevices(camsMap &cards);
void getCamBusName(int deviceNr, char *busname);
int getCamNr(std::string camAddr);

int getResolutions(int deviceNr, std::vector<resolution> &resolutions);
int setResolution(int deviceNr, int idxFormat, int &width, int &height);

bool getResolution(int deviceNr,int idxx, int &width, int &height);

void videoCapGetControls(std::vector<videoCapControl> &camControls, int deviceId);
bool videoCapSetControl(int deviceId, int controlId, int controlValue);
