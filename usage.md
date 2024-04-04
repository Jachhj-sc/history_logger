# stealth_get_active_network_adapter.c
	Usage: stealth_get_active_network_adapter.exe [-o <output file>]

### info
- saves error output to .\log.txt  
- output file contains adapter id, that has an assigned ip address.  
- And that can succesfully ping google.com

### example output
	{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
	{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
	{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
	{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
	{XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

where each line represents an adapter id.

### Building
```
clang stealth_get_active_network_adapter.c -o stealth_get_active_network_adapter.exe -Wl,/subsystem:windows -liphlpapi -lws2_32
```
  
# append_to_path.c
	Usage: 

### info
- x
- x
- x

### example output


### Building

```
build command
```

# example_template
	Usage: 

### info
- x
- x
- x

### example output


### Building

```
build command
```