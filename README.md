huawei_emoncms
========
This tool reads out statistics from Huawei GSM/UMTS sticks and sends them to an emonCMS (http://emoncms.org/) host.
This is a command line tool. 
It can be brought into background (via Ctrl-Z, bg), but does not make itself into a daemon - yet.

* **MacOSX**: not yet tried
* **Raspi**: GSM/UMTS traffic data total and monthly, Raspi core temp
* **Linux**: not yet tried

* Prerequisites: libconfig9, libconfig-dev

raspi_internal_emoncms
========
This tool read out Raspi's internal temperature sensor and send that value to an emonCMS (http://emoncms.org/) host.
This is a command line tool.
It should be run as a cronjob, either as user (crontab -e) or globally (/etc/crontab).

* **Raspi**: Raspi core temp

* Prerequisites: libconfig9, libconfig-dev

raspi_pulsecount_emoncms
========
This tool senses S0 counter pulses and sends these pulses to an emonCMS (http://emoncms.org) host. Additionally,
energy used since the last pulse is sent.
It can be brought into background (via Ctrl-Z, bg), but does not make itself into a daemon - yet. It can be run
from /etc/rc.local.

* Connect S0+ to GPIO 4 (P1:07).
* Connect S0- to GND (P1:09).
* No external components required, GPIO pull-ups are enabled.

* Prerequisites: libconfig9, libconfig-dev, libpigpio (http://abyz.co.uk/rpi/pigpio/)
