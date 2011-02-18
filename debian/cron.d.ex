#
# Regular cron jobs for the ochi package
#
0 4	* * *	root	[ -x /usr/bin/ochi_maintenance ] && /usr/bin/ochi_maintenance
