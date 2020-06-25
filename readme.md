# Priority service (64 Bits)

This service is used to adjust the processes priority according your settings.
Even if the process is started with Normal priority, if you define it as low priority, 
Priority automatically change it priority as you want.

## How to install

1. copy priority.exe into your system folder (e.g. c:\windows)
2. copy priority.ini into your **system32** folder (e.g. **c:\windows\system32**)
2. execute 'priority -i' from an Administror Command Line, to install it as service
3. execute 'priority -s' to start it

other parameters:
- '-e' to stop it
- '-u' to uninstall

## How to configure

edit priority.ini with your favorite editor (e.g. notepad)
and add:
```
[application name without extension]
priority=low/below/normal/above/high or off to kill it
```

**NOTE:**
Works on:
* Windows 10

Not supported/tested:
* Windows NT
* Windows 7
