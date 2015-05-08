#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <math.h>
#include <boost/lexical_cast.hpp>

#include <libv4l1-videodev.h>

#ifdef HAVE_SYS_KLOG_H
#include <sys/klog.h>
#endif

#include <linux/videodev2.h>
#include <libv4l2.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

// #include "guvcview/v4l2uvc.h"
#include "camerav4l.h"

void getCamDevices(camsMap &cards)
{
	DIR *dp;
	struct dirent *ep;
	dev_vec files;
	dev_map links;
	dev_map cardstmp;
	// dev_map cards;
	struct v4l2_capability vcap;
	
	dp = opendir("/dev");
	if (dp == NULL) {
		perror ("Couldn't open the directory");
		return;
	}
	while ((ep = readdir(dp)))
		if (is_v4l_dev(ep->d_name))
			files.push_back(std::string("/dev/") + ep->d_name);
	closedir(dp);

	/* Find device nodes which are links to other device nodes */
	for (dev_vec::iterator iter = files.begin();
			iter != files.end(); ) {
		char link[64+1];
		int link_len;
		std::string target;

		link_len = readlink(iter->c_str(), link, 64);
		if (link_len < 0) {	/* Not a link or error */
			iter++;
			continue;
		}
		link[link_len] = '\0';

		/* Only remove from files list if target itself is in list */
		if (link[0] != '/')	/* Relative link */
			target = std::string("/dev/");
		target += link;
		if (find(files.begin(), files.end(), target) == files.end()) {
			iter++;
			continue;
		}
		
		/* Move the device node from files to links */
		if (links[target].empty())
			links[target] = *iter;
		else
			links[target] += ", " + *iter;
		files.erase(iter);
	}

	std::sort(files.begin(), files.end(), sort_on_device_name);

	for (dev_vec::iterator iter = files.begin();
			iter != files.end(); ++iter) {
		int fd = open(iter->c_str(), O_RDWR);
		std::string bus_info;

		if (fd < 0)
			continue;
		ioctl(fd, VIDIOC_QUERYCAP, &vcap);
		close(fd);
		if (cardstmp[(*iter)].empty())
			cardstmp[(*iter)] += std::string((char *)vcap.card);
		int devNr = getCamNr((*iter));
		cards[devNr] = std::string((char *)vcap.card);
	}
}

void getCamBusName(int deviceNr, char *busname)
{
	struct v4l2_capability vcap;
	
	char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceNr);
	
	int fd = open(deviceAddr, O_RDWR);
	
	if (fd < 0)
	{
		busname[0] = '\0';
		return;
	}
	ioctl(fd, VIDIOC_QUERYCAP, &vcap);
	close(fd);
	
	strcpy(busname,(char *)vcap.card);
	// printf("%s\n",busname->c_str());
}
int getCamNr(std::string camAddr)
{
	char *camAddrTmp;
	camAddrTmp = (char *)strstr(camAddr.c_str(),"/dev/video");
	strcpy(camAddrTmp,&camAddrTmp[strlen("/dev/video")]);
	return atoi(camAddrTmp);
}

static bool is_v4l_dev(const char *name)
{
	return !memcmp(name, "video", 5);
}
static bool sort_on_device_name(const std::string &s1, const std::string &s2)
{
	int n1 = calc_node_val(s1.c_str());
	int n2 = calc_node_val(s2.c_str());

	return n1 < n2;
}
static int calc_node_val(const char *s)
{
	int n = 0;

	s = strrchr(s, '/') + 1;
	if (!memcmp(s, "video", 5)) n = 0;
	else if (!memcmp(s, "radio", 5)) n = 0x100;
	else if (!memcmp(s, "vbi", 3)) n = 0x200;
	else if (!memcmp(s, "v4l-subdev", 10)) n = 0x300;
	n += atol(s + (n >= 0x200 ? 3 : 5));
	return n;
}

void getFPS(int deviceNr, dev_map *fps)
{
	
	
	// close(fd);
	
	// strcpy(busname,(char *)vcap.card);
	// printf("%s\n",busname->c_str());
}

int getResolutions(int deviceNr, std::vector<resolution> &resolutions)
// int getResolutions(int deviceNr, std::string res[100])
{
	// INFOMSG(("getting res of device number: %d",deviceNr));
        
	char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceNr);
	// printf("opening: %s\n",deviceAddr);
	int fd = open(deviceAddr, O_RDONLY);
	
	if (fd < 0)
	{
		return 0;
	}
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	// char resTxt[100];
	int idx = 0;
	
	fmt.index = 0;
	fmt.type = type;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
		// if (fmt.flags)
			// printf(" (compressed)");
		// printf("\tName        : %s\n", fmt.description);
		// fmt.index++;
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				// sprintf(resTxt,"%dx%d", 
				// 	frmsize.discrete.width,
				// 	frmsize.discrete.height,
				// 	fmt.description);
				bool newRes = true;
				for(int i=0;i<idx;i++)
				{
				        if(resolutions[i].w == frmsize.discrete.width && resolutions[i].h == frmsize.discrete.height)
				        {
				                newRes = false;
				                break;
				        }
				}
				if(newRes){
				        resolution newResolution;
				        newResolution.w = frmsize.discrete.width;
				        newResolution.h = frmsize.discrete.height;
				        newResolution.caption =  boost::lexical_cast<std::string>(newResolution.w)+"x"+boost::lexical_cast<std::string>(newResolution.h);
				        resolutions.push_back(newResolution);
				        // res[idx] = resTxt; idx++;
				}
				
				// INFOMSG(("%d ==> %dx%d",idx,frmsize.discrete.width,
				// 	frmsize.discrete.height));
				
			}
			frmsize.index++;
		}
		fmt.index++;
	}
	close(fd);
	// exit(1);
	return idx;
}

int setResolution(int deviceNr, int idxFormat, int &width, int &height)
{
        return false;
	char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceNr);
	int fd = open(deviceAddr, O_RDWR);
	
	if (fd < 0)
	{
		return 0;
	}
	
	
	
	
	// else{
	//         if (set_fmts & FMTWidth)
	//                 in_vfmt.fmt.pix.width = vfmt.fmt.pix.width;
	//         if (set_fmts & FMTHeight)
	//                 in_vfmt.fmt.pix.height = vfmt.fmt.pix.height;
	//         if (ioctl(fd, VIDIOC_S_FMT, &in_vfmt) < 0)
	//                 fprintf(stderr, "ioctl: VIDIOC_S_FMT failed\n");
	// }
	
	
	
	
	//search for the new width and height
	// enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	int idx = 0;
	//format to set:
	
	
	
	int w,h,pf;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				if(idxFormat == idx)
				{
				        w = frmsize.discrete.width;
				        h = frmsize.discrete.height;
				        pf = fmt.pixelformat;
				        
					// in_vfmt.fmt.pix_mp.width = frmsize.discrete.width;
					// in_vfmt.fmt.pix_mp.height = frmsize.discrete.height;
					// in_vfmt.fmt.pix_mp.pixelformat = fmt.pixelformat;
					break;
				}
				idx++;
			}
			frmsize.index++;
		}
		fmt.index++;
	}
	
	struct v4l2_format in_vfmt;
	in_vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (ioctl(fd, VIDIOC_G_FMT, &in_vfmt) < 0)
	{
			ERRMSG(("ioctl: VIDIOC_G_FMT failed"));
	}
	// in_vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	in_vfmt.fmt.pix_mp.width = w;
	in_vfmt.fmt.pix_mp.height = h;
	in_vfmt.fmt.pix_mp.pixelformat = pf;
	for (unsigned i = 0; i < in_vfmt.fmt.pix_mp.num_planes; i++)
				in_vfmt.fmt.pix_mp.plane_fmt[i].bytesperline = 0;
	int ret = 0;	
	if(ret = ioctl(fd, VIDIOC_S_FMT, &in_vfmt) < 0)
	{
	        ERRMSG(("ioctl: VIDIOC_S_FMT failed. %d",ret));
	        close(fd);
	
	}
	close(fd);
	
	// width = w;
	// height = h;
	
	return ret;
}

bool getResolution(int deviceNr,int idxx, int &width, int &height)
{
	width = -1;
	height = -1;
	
	DEBUGMSG(("getting res of device number: %d",deviceNr));
	
	char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceNr);
	int fd = open(deviceAddr, O_RDWR);
	
	if (fd < 0)
	{
		ERRMSG(("FAILED TO LOAD DEVICE %s",deviceAddr));
		return false;
	}
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct v4l2_fmtdesc fmt;
	struct v4l2_frmsizeenum frmsize;
	
	int idx = 0;
	
	fmt.index = 0;
	fmt.type = type;
	
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
		// if (fmt.flags)
			// printf(" (compressed)");
		// printf("\tName        : %s\n", fmt.description);
		// fmt.index++;
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
			        if(idx == idxx)
			        {
			                width = frmsize.discrete.width;
					height = frmsize.discrete.height;
					close(fd);
					return true;
			        }
			        idx++;
			}
			frmsize.index++;
		}
		fmt.index++;
	}
	
	/*while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) >= 0) {
		frmsize.pixel_format = fmt.pixelformat;
		frmsize.index = 0;
		while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
			if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				// printf("---> %d -- %dx%d(%s) \n", 
				// 	idx,frmsize.discrete.width,
				// 	frmsize.discrete.height,
				// 	fmt.description);
				if(idx == idxx)
				{
					width = frmsize.discrete.width;
					height = frmsize.discrete.height;
					return true;
				}
				idx++;
			}
			frmsize.index++;
		}
		fmt.index++;
	}*/
	close(fd);
	INFOMSG(("returninnnng false....???"));
	return false;
}


static std::string flags2s(unsigned val, const flag_def *def)
{
	std::string s;

	while (def->flag) {
		if (val & def->flag) {
			if (s.length()) s += " ";
			s += def->str;
		}
		def++;
	}
	return s;
}

static std::string name2var(unsigned char *name)
{
	std::string s;

	while (*name) {
		if (*name == ' ') s += "_";
		else s += std::string(1, tolower(*name));
		name++;
	}
	return s;
}

static void print_qctrl(int fd, struct v4l2_queryctrl *queryctrl,
		struct v4l2_ext_control *ctrl, int show_menus)
{
	struct v4l2_querymenu qmenu = { 0 };
	std::string s = name2var(queryctrl->name);
	int i;

	qmenu.id = queryctrl->id;
	switch (queryctrl->type) {
	case V4L2_CTRL_TYPE_INTEGER:
		printf("%31s (int)  : min=%d max=%d step=%d default=%d value=%d",
				s.c_str(),
				queryctrl->minimum, queryctrl->maximum,
				queryctrl->step, queryctrl->default_value,
				ctrl->value);
		break;
	case V4L2_CTRL_TYPE_INTEGER64:
		printf("%31s (int64): value=%lld", queryctrl->name, ctrl->value64);
		break;
	case V4L2_CTRL_TYPE_BOOLEAN:
		printf("%31s (bool) : default=%d value=%d",
				s.c_str(),
				queryctrl->default_value, ctrl->value);
		break;
	case V4L2_CTRL_TYPE_MENU:
		printf("%31s (menu) : min=%d max=%d default=%d value=%d",
				s.c_str(),
				queryctrl->minimum, queryctrl->maximum,
				queryctrl->default_value, ctrl->value);
		break;
	case V4L2_CTRL_TYPE_BUTTON:
		printf("%31s (button)\n", s.c_str());
		break;
	default: break;
	}
	if (queryctrl->flags) {
		const flag_def def[] = {
			{ V4L2_CTRL_FLAG_GRABBED,   "grabbed" },
			{ V4L2_CTRL_FLAG_READ_ONLY, "readonly" },
			{ V4L2_CTRL_FLAG_UPDATE,    "update" },
			{ V4L2_CTRL_FLAG_INACTIVE,  "inactive" },
			{ V4L2_CTRL_FLAG_SLIDER,    "slider" },
			{ 0, NULL }
		};
		printf(" flags=%s", flags2s(queryctrl->flags, def).c_str());
	}
	printf("\n");
	if (queryctrl->type == V4L2_CTRL_TYPE_MENU && show_menus) {
		for (i = 0; i <= queryctrl->maximum; i++) {
			qmenu.index = i;
			if (ioctl(fd, VIDIOC_QUERYMENU, &qmenu))
				continue;
			printf("\t\t\t\t%d: %s\n", i, qmenu.name);
		}
	}
}

static int print_control(int fd, struct v4l2_queryctrl &qctrl, int show_menus)
{
	struct v4l2_control ctrl = { 0 };
	struct v4l2_ext_control ext_ctrl = { 0 };
	struct v4l2_ext_controls ctrls = { 0 };

	if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
		return 1;
	if (qctrl.type == V4L2_CTRL_TYPE_CTRL_CLASS) {
		printf("\n%s\n\n", qctrl.name);
		return 1;
	}
	ext_ctrl.id = qctrl.id;
	ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(qctrl.id);
	ctrls.count = 1;
	ctrls.controls = &ext_ctrl;
	if (V4L2_CTRL_ID2CLASS(qctrl.id) != V4L2_CTRL_CLASS_USER &&
	    qctrl.id < V4L2_CID_PRIVATE_BASE) {
		if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls)) {
			printf("error %d getting ext_ctrl %s\n",
					errno, qctrl.name);
			return 0;
		}
	}
	else {
		ctrl.id = qctrl.id;
		if (ioctl(fd, VIDIOC_G_CTRL, &ctrl)) {
			printf("error %d getting ctrl %s\n",
					errno, qctrl.name);
			return 0;
		}
		ext_ctrl.value = ctrl.value;
	}
	print_qctrl(fd, &qctrl, &ext_ctrl, show_menus);
	return 1;
}

void list_controls(int fd, int show_menus)
{
	struct v4l2_queryctrl qctrl = { V4L2_CTRL_FLAG_NEXT_CTRL };
	int id;

	while (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0) {
			print_control(fd, qctrl, show_menus);
		qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}
	if (qctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL)
		return;
	for (id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
		qctrl.id = id;
		if (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0)
			print_control(fd, qctrl, show_menus);
	}
	for (qctrl.id = V4L2_CID_PRIVATE_BASE;
			ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0; qctrl.id++) {
		print_control(fd, qctrl, show_menus);
	}
}

void videoCapGetControls(std::vector<videoCapControl> &camControls, int deviceId)
{
        char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceId);
	int fd = open(deviceAddr, O_RDONLY);
	
	struct v4l2_queryctrl qctrl = { V4L2_CTRL_FLAG_NEXT_CTRL };

	while (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0) {
	        bool enabled = true;
	        
	        struct v4l2_control ctrl = { 0 };
	        struct v4l2_ext_control ext_ctrl = { 0 };
	        struct v4l2_ext_controls ctrls = { 0 };
	        
	        if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
	                enabled = false;
	        if (qctrl.type == V4L2_CTRL_TYPE_CTRL_CLASS) {
	                enabled = false;
	        }
	        ext_ctrl.id = qctrl.id;
	        ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(qctrl.id);
	        ctrls.count = 1;
	        ctrls.controls = &ext_ctrl;
	        if (V4L2_CTRL_ID2CLASS(qctrl.id) != V4L2_CTRL_CLASS_USER &&
	                qctrl.id < V4L2_CID_PRIVATE_BASE)
	        {
	                if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls)) 
	                {
	                        enabled = false;
	                }
	        }
	        else {
	                ctrl.id = qctrl.id;
	                if (ioctl(fd, VIDIOC_G_CTRL, &ctrl)) 
	                {
	                        enabled = false;
	                }
	                ext_ctrl.value = ctrl.value;
	        }
	        if(!enabled)
	        {
	        }else
	        {
	                struct v4l2_querymenu qmenu = { 0 };
	                std::string s = name2var(qctrl.name);
	                int i;
	                videoCapControl tmpCtrl;
	               
	                qmenu.id = qctrl.id;
	                switch (qctrl.type) {
	                case V4L2_CTRL_TYPE_INTEGER:
	                        printf("%31s (int)  : min=%d max=%d step=%d default=%d value=%d",
	                                s.c_str(),
	                                qctrl.minimum, qctrl.maximum,
	                                qctrl.step, qctrl.default_value,
	                                ctrl.value);
	                        
	                        tmpCtrl.typeControl = ctrlInteger;
	                        tmpCtrl.min         = qctrl.minimum;
	                        tmpCtrl.max         = qctrl.maximum;
	                        tmpCtrl.step        = qctrl.step;
	                        tmpCtrl.default_val = qctrl.step;
	                        tmpCtrl.value       = ctrl.value;
	                        tmpCtrl.name        = s;
	                        tmpCtrl.id          = qctrl.id;
	                        
	                        break;
	                // case V4L2_CTRL_TYPE_INTEGER64:
	                //         printf("%31s (int64): value=%lld", queryctrl.name, ctrl.value64);
	                //         break;
	                case V4L2_CTRL_TYPE_BOOLEAN:
	                        printf("%31s (bool) : default=%d value=%d",
	                                s.c_str(),
	                                qctrl.default_value, ctrl.value);
	                        
	                        
	                        tmpCtrl.typeControl = ctrlBoolean;
	                        tmpCtrl.default_val = qctrl.default_value;
	                        tmpCtrl.value       = ctrl.value;
	                        tmpCtrl.name        = s;
	                        tmpCtrl.id          = qctrl.id;
	                        
	                        break;
	                case V4L2_CTRL_TYPE_MENU:
	                        printf("%31s (menu) : min=%d max=%d default=%d value=%d",
	                                s.c_str(),
	                                qctrl.minimum, qctrl.maximum,
	                                qctrl.default_value, ctrl.value);
	                        
	                        tmpCtrl.min         = qctrl.minimum;
	                        tmpCtrl.max         = qctrl.maximum;
	                        tmpCtrl.typeControl = ctrlMenu;
	                        tmpCtrl.default_val = qctrl.default_value;
	                        tmpCtrl.value       = ctrl.value;
	                        tmpCtrl.name        = s;
	                        tmpCtrl.id          = qctrl.id;
	                        break;
	                case V4L2_CTRL_TYPE_BUTTON:
	                        printf("%31s (button)\n", s.c_str());
	                        break;
	                default: break;
	                }
	                // if (qctrl.flags) {
	                //         const flag_def def[] = {
	                //                 { V4L2_CTRL_FLAG_GRABBED,   "grabbed" },
	                //                 { V4L2_CTRL_FLAG_READ_ONLY, "readonly" },
	                //                 { V4L2_CTRL_FLAG_UPDATE,    "update" },
	                //                 { V4L2_CTRL_FLAG_INACTIVE,  "inactive" },
	                //                 { V4L2_CTRL_FLAG_SLIDER,    "slider" },
	                //                 { 0, NULL }
	                //         };
	                //         printf(" flags=%s", flags2s(qctrl.flags, def).c_str());
	                // }
	                // printf("\n");
	                // if (qctrl.type == V4L2_CTRL_TYPE_MENU ){//&& show_menus) {
	                //         for (i = 0; i <= qctrl.maximum; i++) {
	                //                 qmenu.index = i;
	                //                 if (ioctl(fd, VIDIOC_QUERYMENU, &qmenu))
	                //                         continue;
	                //                 printf("\t\t\t\t%d: %s\n", i, qmenu.name);
	                //         }
	                // }
	                
	                camControls.push_back(tmpCtrl);
	        }
		qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}
	/*if (qctrl.id != V4L2_CTRL_FLAG_NEXT_CTRL)
		return;
	for (id = V4L2_CID_USER_BASE; id < V4L2_CID_LASTP1; id++) {
		qctrl.id = id;
		if (ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0)
			print_control(fd, qctrl, show_menus);
	}
	for (qctrl.id = V4L2_CID_PRIVATE_BASE;
			ioctl(fd, VIDIOC_QUERYCTRL, &qctrl) == 0; qctrl.id++) {
		print_control(fd, qctrl, show_menus);
	}*/
}
bool videoCapSetControl(int deviceId, int controlId, int controlValue)
{
        char deviceAddr[50];
	sprintf(deviceAddr,"/dev/video%d",deviceId);
	
        struct v4l2_control ctrl;
        int fd = open(deviceAddr, O_RDWR);
        ctrl.id = controlId;
        ctrl.value = controlValue;
        if (ioctl(fd, VIDIOC_S_CTRL, &ctrl, "VIDIOC_S_CTRL")) 
        {
                // fprintf(stderr, "%s: %s\n",
                        // ctrl_id2str[ctrl.id].c_str(),
                        // strerror(errno));
                        INFOMSG(("error, on setting %s %d to %d",deviceAddr,controlId,controlValue));
                        return false;
        }
        return true;
}
