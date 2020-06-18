# Priority service

This service is used to adjust the process priorities according your settings.
Even if the process is started with Normal priority, if you define it as low priority, 
Priority automatically change it priority as you want.

## How to install

1. copy priority.exe and priority.ini into your system folder (e.g. c:\windows)
2. execute 'priority -install' (now the priority service is installed)
3. execute 'priority -start'

## How to configure

edit priority.ini with your favorite editor (e.g. notepad)
and add:
```
[application name without extension]
priority=low/below/normal/above/high or off to kill it
```

priority.ini must be location under **Windows\system32**

**NOTE:**
Works on:
* Windows NT
* Windows 7
