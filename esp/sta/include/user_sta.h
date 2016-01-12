#ifndef __USER_STA_H__
#define __USER_STA_H__

#define SSID "DRUPPEL"
#define WIFI_KEY "welkom08"

void user_sta_init(void); 
void user_sta_setup_config(void);
void user_sta_setup_static_ip(void);
void user_sta_get_auto_connect(void);
void user_softap_ipinfo(void);

#endif
